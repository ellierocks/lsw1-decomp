"""Signature helpers: the draft's own parameter list by argument register."""

from __future__ import annotations

import re

from m2c_helpers import ASM_COMMENT as ASM_COMMENT_RE


# m2c declares only the arguments a function reads, named by register: GPR `argN` is
# r(N+3) and FPR `fargN` is f(N+1), numbered independently because the PPC EABI assigns
# int and float arguments from separate register files. `f(void *arg2)` compiled as
# written takes its value in r3, so pad each sequence from 0 with placeholders.
PARAM = re.compile(r"^(?P<decl>.*?\b(?P<kind>f?arg)(?P<n>\d+))$")


def pad_signature(body: str, name: str) -> str:
    """BODY with NAME's parameter list padded to arg0..argN and farg0..fargN."""
    m = re.search(rf"^(\w[^\n;(]*?\b{re.escape(name)})\(([^)]*)\)(\s*\{{)", body, flags=re.M)
    if not m or m[2].strip() in ("", "void"):
        return body
    regs: dict[str, dict[int, str]] = {"arg": {}, "farg": {}}
    for param in (p.strip() for p in m[2].split(",")):
        pm = PARAM.match(param)
        # A 64-bit integer takes an aligned GPR pair; stack and variadic args do not pad.
        if not pm or re.search(r"\b(?:s64|u64|long long)\b", pm["decl"]):
            return body
        regs[pm["kind"]][int(pm["n"])] = param
    if regs["arg"] and max(regs["arg"]) > 7 or regs["farg"] and max(regs["farg"]) > 7:
        return body
    params = []
    for kind, pad in (("arg", "s32 pad"), ("farg", "f32 fpad")):
        if regs[kind]:
            params += [regs[kind].get(i, f"{pad}{i}") for i in range(max(regs[kind]) + 1)]
    new = ", ".join(params)
    if new == m[2].strip():
        return body
    return body[:m.start(2)] + new + body[m.end(2):]


WRITES_NONE = re.compile(r"(?:st|cmp|b|mt|tw|dcb|icb|sync|isync|cr)")


def passthrough_regs(asm: str, fn: str) -> tuple[set[int], set[int]] | None:
    """Argument registers (GPR r3.. as 0.., FPR f1.. as 0..) the target leaves unwritten
    before its first call, when that call is `bl FN`: the callee receives the target's own
    incoming arguments there (a thin wrapper `f(a) { g(a, 0); }`). None when FN is not the
    first call."""
    gprs, fprs = set(), set()
    for line in asm.splitlines():
        line = ASM_COMMENT_RE.sub("", line).strip()
        if not line or line.startswith((".", "#")) or line.endswith(":"):
            continue
        op, _, rest = line.partition(" ")
        if op in ("bl", "bctrl", "blrl"):
            if op != "bl" or rest.strip() != fn:
                return None
            return ({i for i in range(8) if 3 + i not in gprs},
                    {i for i in range(8) if 1 + i not in fprs})
        if WRITES_NONE.match(op) or op.startswith(("psq_st", "fcmp")):
            continue  # stores, compares, branches and moves to SPRs write no argument register
        d = re.match(r"([rf])(\d+)\b", rest.strip())
        if d:
            (gprs if d[1] == "r" else fprs).add(int(d[2]))
    return None


def add_params(body: str, name: str, extra: dict[str, str]) -> str:
    """BODY with NAME's m2c-style parameter list (argN/fargN) extended by EXTRA (param name
    to type) and padded to keep each register class contiguous; unchanged when the list
    has parameters not in m2c's register naming."""
    m = re.search(rf"^(\w[^\n;(]*?\b{re.escape(name)})\(([^)]*)\)(\s*\{{)", body, flags=re.M)
    if not m:
        return body
    regs: dict[str, dict[int, str]] = {"arg": {}, "farg": {}}
    if m[2].strip() not in ("", "void"):
        for param in (p.strip() for p in m[2].split(",")):
            pm = PARAM.match(param)
            if not pm or re.search(r"\b(?:s64|u64|long long)\b", pm["decl"]):
                return body
            regs[pm["kind"]][int(pm["n"])] = param
    for nm, typ in extra.items():
        pm = re.fullmatch(r"(f?arg)(\d+)", nm)
        regs[pm[1]].setdefault(int(pm[2]), f"{typ}{nm}" if typ.endswith("*") else f"{typ} {nm}")
    params = []
    for kind, pad in (("arg", "s32 pad"), ("farg", "f32 fpad")):
        if regs[kind]:
            params += [regs[kind].get(i, f"{pad}{i}") for i in range(max(regs[kind]) + 1)]
    return body[:m.start(2)] + (", ".join(params) or "void") + body[m.end(2):]
