#!/usr/bin/env python3
"""Unattended m2c sweep: draft, compile and strict-score every ready target in parallel.

    python3 tools/sweep.py                          # all ready targets, report only
    python3 tools/sweep.py --max-size 0x400 -j 16   # bound the sweep
    python3 tools/sweep.py --unit auto/game/ --apply

For each `next_target.py` target this runs m2c in `--valid-syntax` mode on the retail
assembly, rewrites m2c's macros into plain C, and scores the draft with `score_fn.py
--body` against the function's unit source (appended in a temporary copy, so SOURCE is
untouched). A draft that redeclares a function or global with conflicting types is
retried without m2c's declaration. No model or agent is involved.

m2c also reads the unit source, preprocessed, as `--context`: its structs, globals and
prototypes (including the unit's headers) type the draft's field accesses and calls. Each
target is drafted with and without that context and the better-scoring draft is kept, so
typing a header can only raise the sweep's yield. m2c declares only the arguments a
function reads, so each draft is also scored with its signature padded to the registers
those arguments arrive in (`pad_signature`), keeping the better score.
Each draft's callee declarations are then repaired (`fix_prototypes`: the unit's or the
repo's prototype, or a variadic/unprototyped one where the target sets cr1eq) and its
undeclared stack slots folded into arrays (`stack_arrays`), keeping the better score.

`--apply` writes strict MATCHes into their unit sources one at a time, re-scoring each
against the updated file with `score_fn.py --apply`; run `bash build.sh` and
`sprint_finish.py` afterwards for the build-level strict verification. Everything else is
ranked in `work/sweep/results.json` (and printed) by unmatched bytes closest to a match:
that ranking is the agent queue, biggest progress first. Task packs go to
`work/sweep/NAME/`, which does not claim the target for `next_target.py`.
"""
from __future__ import annotations

import argparse
import itertools
import json
import os
import re
import subprocess
import sys
import tempfile
import threading
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
import draft_fn  # noqa: E402
import mac_protos  # noqa: E402
from m2c_helpers import ASM_COMMENT as ASM_COMMENT_RE  # noqa: E402
from m2c_helpers import (split_args, expand_macros, fold_cror, fold_ps, fix_ps_calls,
                         moves_to_args, type_derefs, strip_vararg_saves, declare_unset,
                         call_sites, PS_CALLS, prepare_asm, repair_indirect_prototypes)

OUT = ROOT / "work" / "sweep"
# Macros that only stand for an instruction m2c could not express; such a draft cannot match.
UNHANDLED = re.compile(r"\bM2C_(?:ERROR|TRAP_IF|BREAK|SYNC|DCACHE|ICACHE|PREFETCH|CARRY|OVERFLOW|"
                       r"LWL|FIRST3BYTES|UNALIGNED32|LOAD_|STORE_)|\b(?:GLUE_F64|MULT_HI|MULTU_HI|"
                       r"CLZ|ROTATE_RIGHT|BSWAP\d+)\(")


_CONTEXTS: dict[str, Path | None] = {}
_CONTEXT_LOCK = threading.Lock()
CPP = ["cpp", "-P", "-nostdinc", "-Isrc", "-Iinclude", "-Ibuild/GL5E4F/include", "-D__attribute__(x)="]


def unit_context(source: str) -> Path | None:
    """SOURCE preprocessed for m2c's `--context` (cached per sweep), or None if cpp fails."""
    with _CONTEXT_LOCK:
        if source not in _CONTEXTS:
            out = OUT / "ctx" / (source.replace("/", "_") + ".i")
            out.parent.mkdir(parents=True, exist_ok=True)
            try:
                r = subprocess.run(CPP + [source], cwd=ROOT, capture_output=True, text=True)
            except OSError:
                r = None
            if r is not None and r.returncode == 0:
                out.write_text(r.stdout)
                _CONTEXTS[source] = out
            else:
                _CONTEXTS[source] = None
        return _CONTEXTS[source]


def draft(name: str, context: Path | None = None, sized: bool = False) -> tuple[str, str]:
    """(status, C body) for NAME; status is 'ok', 'no-asm', 'm2c-failed' or 'unhandled'.

    SIZED sets M2C_SIZED_DEREFS (tools/patches/m2c-sized-derefs.patch): `*(a + b)` derefs
    keep their load width as `*(u8 *)(a + b)`, which the C types may otherwise lose."""
    task = OUT / name
    asm = task / "asm.s"
    if not asm.is_file():
        subprocess.run([sys.executable, str(ROOT / "tools" / "ls1_task_pack.py"), name,
                        "--out-dir", str(OUT)], cwd=ROOT, capture_output=True)
    if not asm.is_file() or not asm.read_text().strip():
        return "no-asm", ""
    m2c = draft_fn.m2c_dir()
    m2c_in = task / "asm_m2c.s"
    m2c_in.write_text(prepare_asm(asm.read_text()))
    cmd = [sys.executable, str(m2c / "m2c.py"), "-t", draft_fn.TARGET, "--valid-syntax"]
    if context is not None:
        cmd += ["--no-cache", "--context", str(context)]
    env = {**os.environ, "M2C_SIZED_DEREFS": "1"} if sized else None
    r = subprocess.run(cmd + [str(m2c_in.resolve())], capture_output=True, text=True, env=env)
    if r.returncode or not r.stdout.strip():
        return "m2c-failed", ""
    out = declare_unset(r.stdout, name)
    if UNHANDLED.search(out):
        return "unhandled", ""
    try:
        body = type_derefs(expand_macros(out))
    except ValueError:
        return "m2c-failed", ""
    body = draft_fn.clean(body, name, asm.read_text())
    body = body.replace('#include "types.h"\n', "", 1).strip() + "\n"
    return "ok", body


# m2c phrases a conditional store as `v = A; if (c) { v = B; } g = v;`. ProDG then hoists
# `v = A` above the call in c into a callee-saved register; the retail code usually
# comes from storing in both arms, so a second variant stores B or A directly.
PRESET_STORE = re.compile(r"(?P<ind>[ \t]*)(?P<v>var_r\w+) = (?P<a>[^;\n]+);\n(?P=ind)if \((?P<c>.*)\) \{\n"
                          r"[ \t]*(?P=v) = (?P<b>[^;\n]+);\n(?P=ind)\}\n(?P=ind)(?P<g>[\w\[\]]+) = (?P=v);\n")


def store_in_arms(body: str) -> str:
    """BODY with each PRESET_STORE written as an if/else of direct stores."""
    def arms(m):
        i = m["ind"]
        return (f"{i}if ({m['c']}) {{\n{i}    {m['g']} = {m['b']};\n{i}}} else {{\n"
                f"{i}    {m['g']} = {m['a']};\n{i}}}\n")
    out = PRESET_STORE.sub(arms, body)
    for v in {m["v"] for m in PRESET_STORE.finditer(body)}:
        decl = re.compile(rf"^[ \t]+\w+ {v};\n", flags=re.M)
        if len(re.findall(rf"\b{v}\b", out)) == len(decl.findall(out)):  # only declared now
            out = decl.sub("", out, count=1)
    return out


_RODATA: set[str] | None = None


def rodata_symbols() -> set[str]:
    """Names that config/GL5E4F/symbols.txt places in .rodata."""
    global _RODATA
    if _RODATA is None:
        text = (ROOT / "config" / "GL5E4F" / "symbols.txt").read_text()
        _RODATA = set(re.findall(r"^(\w+) = \.rodata:", text, flags=re.M))
    return _RODATA


def const_rodata(body: str) -> str:
    """BODY with its .rodata externs declared const. ProDG keeps a load from a non-const
    float array inside a loop that stores floats, but hoists it from a const one."""
    rodata = rodata_symbols()
    return re.sub(r"^extern (?!const )(\w[\w ]*?)\b(\w+)\[\];",
                  lambda m: f"extern const {m[1]}{m[2]}[];" if m[2] in rodata else m[0], body, flags=re.M)


GUARD = re.compile(r"\bif \((?P<n>\(s32\) \w+(?:\[0\])?|\w+(?:\[0\])?|\(s32\) \(\*\([^;{}]*?\)\)|\(\*\([^;{}]*?\)\)) > 0\) \{")


def loop_guard(body: str) -> str:
    """BODY with m2c's loop guard `if (N > 0)` written as `if (i < N)` when the counter i,
    zeroed before it, is compared with the same N at the loop's end. GCC keeps a `for`
    loop's entry test as `cmpw i, N` with i still 0, which a constant compare cannot
    reproduce (fn_80064B40, fn_80063D9C)."""
    def repl(m):
        n = m["n"]
        bare = re.escape(n[6:] if n.startswith("(s32) ") else n)
        cmp = re.search(rf"\b(\w+) (?:>=|<) (?:\(s32\) )?{bare}\)", body[m.end():])
        if not cmp or not re.search(rf"^\s*{cmp[1]} = 0;$", body[:m.start()], flags=re.M):
            return m[0]
        return f"if ({cmp[1]} < {n}) {{"
    return GUARD.sub(repl, body)


def base_pointer(body: str) -> str:
    """BODY with each run of two or more consecutive statements that address one global
    through m2c's `(s8 *)(G) + off` wrapped in a block that reads G through a local
    `s8 *G_p`. ProDG then forms G's address once (`lis/addi`, then `off(r9)` loads)
    instead of folding each offset into its own `G+off@ha/@l` pair (fn_800B0120)."""
    lines = body.split("\n")
    out, i = [], 0
    while i < len(lines):
        m = re.match(r"^([ \t]+)\S", lines[i])
        syms = set(re.findall(r"\(s8 \*\)\((\w+)\) \+ ", lines[i])) if m else set()
        sym = next((g for g in sorted(syms) if not g.startswith(("arg", "var_", "temp_", "sp"))), None)
        j = i
        if sym:
            ind = m[1]
            while (j + 1 < len(lines) and lines[j + 1].startswith(ind) and not lines[j + 1][len(ind)].isspace()
                   and f"(s8 *)({sym}) + " in lines[j + 1] and lines[j + 1].rstrip().endswith(";")):
                j += 1
        if sym and j > i and lines[i].rstrip().endswith(";"):
            out.append(f"{ind}{{")
            out.append(f"{ind}    s8 *{sym}_p = (s8 *){sym};")
            out.extend("    " + ln.replace(f"(s8 *)({sym})", f"{sym}_p") for ln in lines[i:j + 1])
            out.append(f"{ind}}}")
            i = j + 1
            continue
        out.append(lines[i])
        i += 1
    return "\n".join(out)


def both_rewrites(body: str) -> str:
    return const_rodata(store_in_arms(body))


NARROW_RET = re.compile(r"^(?:u8|s8|u16|s16) (?=\w+\()", re.M)


def wide_return(body: str) -> str:
    """BODY with a u8/s8/u16/s16 return type widened to s32 when it returns -1. m2c types
    the return from a narrow load (`lbz`), but `li r3, -0x1` with no extension needs an int
    return: `return -1U` from a u8 function emits `li r3, 0xff` (fn_8005A5B0)."""
    if not re.search(r"\breturn -1U?;", body) or not NARROW_RET.search(body):
        return body
    return re.sub(r"\breturn -1U;", "return -1;", NARROW_RET.sub("s32 ", body, count=1))


CALLEE_DECL = re.compile(r"^(?!extern\b)(?:s32|u32|int|s16|u16|s8|u8|void \*|s32 \*|u32 \*) ?(\w+)\(([^()]*)\);$",
                         flags=re.M)


def void_callees(body: str) -> str:
    """BODY with each m2c-declared callee whose result is never used declared `void`.
    m2c types every callee `s32`; a call statement that discards an `int` result
    schedules its argument loads differently from a `void` call, so `li r3`/`li r4`
    land on the other side of an independent `lfs` (fn_800A1C70)."""
    out = body
    for m in CALLEE_DECL.finditer(body):
        fn = m[1]
        uses = len(re.findall(rf"\b{fn}\b", body)) - 1
        stmts = len(re.findall(rf"^[ \t]+{fn}\(.*\);$", body, flags=re.M))
        if uses and uses == stmts:
            out = out.replace(m[0], f"void {fn}({m[2]});", 1)
    return out


VARIANTS = (store_in_arms, const_rodata, both_rewrites, wide_return, void_callees)
VOID_DECL = re.compile(r"^void (\w+)\(([^()]*)\);$", flags=re.M)


def int_callees(body: str, limit: int = 6) -> list[str]:
    """BODY with one `void` callee declared `s32` instead, for each such callee: the
    reverse of `void_callees`. A call to an `int` function whose result is dropped
    schedules the first argument's `addi` later than a `void` call does (OSCreateThread
    in fn_8009D23C), so neither return type is always right for a discarded result."""
    return [body.replace(m[0], f"s32 {m[1]}({m[2]});", 1) for m in VOID_DECL.finditer(body)][:limit]

PROTO_DECL = re.compile(r"^(?:\w+ \**)+?(\w+)\(([^()]+)\);$", flags=re.M)
FLOAT_PARAM = re.compile(r"^(?:const )?(?:f32|f64|float|double)\b(?! *\*)")


def float_blocks(body: str, limit: int = 8) -> list[str]:
    """BODY with prototypes' trailing float parameters moved, as a block, in front of
    earlier integer parameters, and every call reordered to match. Floats and ints use
    separate register files, so m2c cannot see the interleaving and puts the floats
    last; the true position changes argument evaluation order and with it the
    scheduling (fn_8009E6FC: two callees are both `(s32 x4, f32 x4, u32 *, s32)`).
    Each shift is tried on every eligible prototype at once first (sibling callees
    tend to share a layout), then on each prototype alone."""
    protos = []
    for m in PROTO_DECL.finditer(body):
        params = [p.strip() for p in m[2].split(",")]
        kinds = [bool(FLOAT_PARAM.match(p)) for p in params]
        nint = kinds.index(True) if True in kinds else len(kinds)
        if 0 < nint < len(kinds) and all(kinds[nint:]):
            protos.append((m, params, nint))

    def move(text: str, m, params: list[str], nint: int, shift: int) -> str | None:
        pos = nint - shift
        if pos < 0:
            return None
        order = list(range(pos)) + list(range(nint, len(params))) + list(range(pos, nint))
        proto = m[0].replace(m[2], ", ".join(params[i] for i in order))
        text = text.replace(m[0], proto, 1)
        dstart, done = text.index(proto), []
        for call in re.finditer(rf"\b{m[1]}\(", text):
            if dstart <= call.start() < dstart + len(proto):
                continue
            got = call_args(text, call.end() - 1)
            if got and len(got[0]) == len(params):
                done.append((call.end() - 1, got[1], got[0]))
        for start, end, args in reversed(done):
            text = text[:start] + "(" + ", ".join(args[i] for i in order) + ")" + text[end:]
        return text if done else None

    out = []
    longest = max((nint for _, _, nint in protos), default=0)
    if len(protos) > 1:
        for shift in range(1, longest + 1):
            text = body
            for m, params, nint in protos:
                text = move(text, m, params, nint, shift) or text
            if text != body:
                out.append(text)
    for m, params, nint in protos:
        out += [t for t in (move(body, m, params, nint, s) for s in range(1, nint + 1)) if t]
    seen = set()
    return [t for t in out if not (t in seen or seen.add(t))][:limit]


MEM_STORE = re.compile(r"^(?P<ind>[ \t]+)(?P<lhs>\(*\*\(.+?\)\)*|\w+(?:\[[^\]]*\])?(?:(?:->|\.)\w+)+) = (?P<rhs>[^;]+);$")


FIELD_LHS = re.compile(r"^\(\*\([^()]+\)\(\(s8 \*\)\((?P<base>\w+)\) \+ \((?P<off>-?(?:0x[0-9A-Fa-f]+|\d+))\)\)\)$")


def sorted_stores(body: str) -> str:
    """BODY with each run of adjacent independent field stores through one base pointer
    put in ascending offset order. m2c keeps the scheduled order, in which GCC often moves
    float stores ahead (fn_80064860 stores 0x264 first); the source usually wrote the
    fields in layout order."""
    lines = body.split("\n")
    i = 0
    while i < len(lines):
        run = []
        j = i
        while j < len(lines):
            m = MEM_STORE.match(lines[j])
            f = m and FIELD_LHS.match(m["lhs"])
            if not f or (run and (m["ind"] != run[0][1]["ind"] or f["base"] != run[0][2]["base"])):
                break
            run.append((lines[j], m, f))
            j += 1
        if len(run) > 1 and not any(a[1]["lhs"] in b[1]["rhs"] for a in run for b in run if a is not b):
            lines[i:j] = [r[0] for r in sorted(run, key=lambda r: int(r[2]["off"], 0))]
        i = max(j, i + 1)
    return "\n".join(lines)


def swap_stores(body: str, limit: int = 8) -> list[str]:
    """BODY with one pair of adjacent independent memory stores swapped, for each such pair,
    then with each run of three or more such stores rotated by one either way: GCC's
    scheduler often emits a function's last stores in another order. A plain name counts as
    memory when the body declares it `extern` (`lbl_8040A820 = 0;`, fn_8014FE90)."""
    lines, out = body.split("\n"), []
    externs = set(re.findall(r"^extern [^;(]*?\b(\w+)(?:\[\])?;", body, flags=re.M))

    def store(ln: str):
        m = MEM_STORE.match(ln)
        if m:
            return m
        m = re.match(r"^(?P<ind>[ \t]+)(?P<lhs>\w+) = (?P<rhs>[^;]+);$", ln)
        return m if m and m["lhs"] in externs else None

    def independent(a, b) -> bool:
        return (a["ind"] == b["ind"] and a["lhs"] != b["lhs"]
                and not re.search(rf"(?<![\w.>]){re.escape(a['lhs'])}(?!\w)", b["rhs"])
                and not re.search(rf"(?<![\w.>]){re.escape(b['lhs'])}(?!\w)", a["rhs"]))
    ms = [store(ln) for ln in lines]
    for i in range(len(lines) - 1):
        a, b = ms[i], ms[i + 1]
        if a and b and independent(a, b):
            out.append("\n".join(lines[:i] + [lines[i + 1], lines[i]] + lines[i + 2:]))
    out, rots = out[:limit], []
    i = 0
    while i < len(lines):
        j = i
        while j + 1 < len(lines) and ms[j + 1] and ms[i] and all(independent(ms[k], ms[j + 1]) for k in range(i, j + 1)):
            j += 1
        if j - i >= 2:
            run = lines[i:j + 1]
            for rot in (run[-1:] + run[:-1], run[1:] + run[:1]):
                rots.append("\n".join(lines[:i] + rot + lines[j + 1:]))
        i = j + 1
    return out + rots[:limit]


def volatile_locals(body: str, limit: int = 4) -> list[str]:
    """BODY with one address-taken stack local (`sp8`, `sp10`, ...) declared `volatile`, for
    each such local: GCC gives an address-taken local its slot when the address is first
    expanded (call arguments right to left), a volatile one at its declaration (fn_8009D57C)."""
    out = []
    for m in re.finditer(r"^(?P<ind>[ \t]+)(?P<decl>(?!volatile\b)\w[\w ]*?\b(?P<v>sp[0-9A-F]+));$", body, flags=re.M):
        if re.search(rf"&\s*{m['v']}\b", body):
            out.append(body[:m.start()] + f"{m['ind']}volatile {m['decl']};" + body[m.end():])
            if len(out) == limit:
                break
    return out


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


CONFLICT = re.compile(r"conflicting types for [`'‘](\w+)|redefinition of [`'‘](\w+)|"
                      r"previous declaration of [`'‘](\w+)|[`'‘](\w+)' redeclared as different kind")


def drop_decl(body: str, sym: str, name: str) -> str | None:
    """Remove m2c's top-level declaration of SYM (never NAME's own definition)."""
    if sym == name:
        return None
    lines = body.splitlines()
    kept = [ln for ln in lines if not (ln.rstrip().endswith(";") and not ln.startswith((" ", "\t"))
                                        and re.search(rf"\b{re.escape(sym)}\b", ln))]
    return "\n".join(kept) + "\n" if len(kept) != len(lines) else None


DECL_LINE = re.compile(r"^(?:extern\s+)?(?P<decl>[^;{}()=#]*?\b(?P<sym>\w+)\b(?P<rest>\s*(?:\[[^\]]*\]\s*)*|\s*\([^;{}]*\)\s*));\s*$")


def alias_decl(body: str, sym: str, name: str) -> str | None:
    """Replace m2c's top-level declaration of SYM, which conflicts with the unit's, by a macro
    that reads SYM through m2c's type: `#define g (*(T *(*))&g)` for data and
    `#define f ((R (*)(A))f)` for a function. The body keeps m2c's typing, the code generated
    for the access is the same, and an `#undef` after the body restores the unit's type."""
    if sym == name:
        return None
    lines = body.splitlines()
    for i, ln in enumerate(lines):
        if ln.startswith((" ", "\t")) or not re.search(rf"\b{re.escape(sym)}\b", ln):
            continue
        m = DECL_LINE.match(ln.strip())
        if not m or m["sym"] != sym or m["decl"].lstrip().startswith(("static", "typedef")):
            continue
        decl = re.sub(r"^extern\s+", "", ln.strip()[:-1].strip())
        abstract = re.sub(rf"\b{re.escape(sym)}\b", "(*)", decl, count=1)
        if m["rest"].strip().startswith("("):
            lines[i] = f"#define {sym} (({abstract}){sym})"
        else:
            lines[i] = f"#define {sym} (*({abstract})&{sym})"
        return "\n".join(lines) + f"\n#undef {sym}\n"
    return None


# m2c names a stack slot by offset and declares only the slots it sees by name, so an
# out-parameter `f(&sp8)` whose later words are read as `spC`/`sp10` leaves them
# undeclared. Fold such slots into an array based at the nearest lower declared slot.
SP_DECL = re.compile(r"^(?P<ind>[ \t]+)(?P<t>[su](?:8|16|32)|f32) (?P<v>sp(?P<off>[0-9A-F]+));\n", re.M)
SIZES = {"s8": 1, "u8": 1, "s16": 2, "u16": 2, "s32": 4, "u32": 4, "f32": 4}


def call_args(text: str, start: int) -> tuple[list[str], int] | None:
    """The comma-separated arguments of the call whose `(` is at START, and the index after `)`."""
    depth, args, cur = 0, [], start + 1
    for i in range(start, len(text)):
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
            if depth == 0:
                args.append(text[cur:i].strip())
                return args, i + 1
        elif text[i] == "," and depth == 1:
            args.append(text[cur:i].strip())
            cur = i + 1
    return None


FLOAT_GE = re.compile(r"(?P<o>\()?\((?P<a>[^()]+) == (?P<b>(?:[^()]|\([^()]*\))+)\) \|\| "
                      r"\((?P=a) (?P<op>[<>]) (?P=b)\)(?(o)\))(?: != 0)?")


def float_ge(body: str) -> str:
    """m2c's `cror` fold `((a == b) || (a > b)) != 0` written as the source's `a >= b`."""
    return FLOAT_GE.sub(lambda m: f"({m['a']} {m['op']}= {m['b']})", body)


def native_shifts(body: str) -> str:
    """m2c's libgcc `__ashldi3(0, LO, N)` calls written as the 64-bit shift ProDG lowers to
    them (`1LL << n`); the call order of the argument loads then follows the source."""
    out, pos = [], 0
    for m in re.finditer(r"\b__ashldi3\(", body):
        if m.start() < pos:
            continue
        parsed = call_args(body, m.end() - 1)
        if not parsed or len(parsed[0]) != 3 or parsed[0][0] != "0":
            continue
        (_, lo, n), end = parsed
        value = f"{lo}LL" if re.fullmatch(r"\d+", lo) else f"(u64)(u32)({lo})"
        out += [body[pos:m.start()], f"({value} << {n})"]
        pos = end
    return "".join(out) + body[pos:]


def stack_arrays(body: str) -> str:
    """BODY with undeclared `spXX` slots folded into an array at a declared lower slot,
    m2c's `**(T *)` double dereference typed as `**(T **)`, and m2c's `unkspXX` sub-word
    stack reads declared as locals so the draft compiles (a ranked start, not a match)."""
    body = re.sub(r"\*\*\((\w+) \*\)", r"**(\1 **)", body)
    body = native_shifts(body)
    body = float_ge(body)
    unk = sorted(set(re.findall(r"\bunksp[0-9A-F]+\b", body)) - set(
        re.findall(r"^[ \t]+[\w ]+?\**\s*(unksp[0-9A-F]+);", body, flags=re.M)))
    if unk:
        head = re.search(r"^[^\n]*\)\s*\{\n", body, flags=re.M)
        if head:
            body = body[:head.end()] + "".join(f"    s32 {v};\n" for v in unk) + body[head.end():]
    body = re.sub(r"&(sp[0-9A-F]+)\[0\]", r"&\1", body)  # m2c's `&sp20[0]` for an undeclared slot
    declared = {m["v"]: m for m in SP_DECL.finditer(body)}
    all_decl = set(re.findall(r"^[ \t]+[\w ]+?\**\s*(sp[0-9A-F]+)(?:\[\w*\])?;", body, flags=re.M))
    missing = sorted({v for v in re.findall(r"\bsp[0-9A-F]+\b", body) if v not in all_decl},
                     key=lambda v: int(v[2:], 16))
    groups: dict[str, dict[str, int]] = {}
    for v in missing:
        off = int(v[2:], 16)
        bases = [m for m in declared.values() if int(m["off"], 16) < off and f"&{m['v']}" in body]
        if not bases:
            return body
        base = max(bases, key=lambda m: int(m["off"], 16))
        delta, size = off - int(base["off"], 16), SIZES[base["t"]]
        if delta % size:
            return body
        groups.setdefault(base["v"], {})[v] = delta // size
    for b, slots in groups.items():
        m = declared[b]
        body = body.replace(m[0], f"{m['ind']}{m['t']} {b}[{max(slots.values()) + 1}];\n", 1)
        body = re.sub(rf"(?<![\w&]){b}\b(?!\[)", f"{b}[0]", body)
        body = re.sub(rf"&{b}\b(?!\[)", b, body)
        for v, i in slots.items():
            body = re.sub(rf"\b{v}\b", f"{b}[{i}]", body)
    return body

WIDE_PTR = re.compile(r"^(?P<ind>[ \t]+)(?P<t>s16|u16|s32|u32|f32|f64|s64|u64) \*(?P<v>\w+);$", re.M)
PTR_SIZES = {"s16": 2, "u16": 2, "s32": 4, "u32": 4, "f32": 4, "f64": 8, "s64": 8, "u64": 8}


EMPTY_FRAME = re.compile(r"\A\s*stwu r1, -(0x[0-9A-Fa-f]+)\(r1\)\s+addi r1, r1, \1\s+blr\s*\Z")
EMPTY_BODY = re.compile(r"(\)\s*\{)\s*(\})\s*\Z")


def empty_frame(body: str, asm: str) -> str:
    """An empty function whose target still opens a stack frame: GCC keeps the frame for an unused
    volatile local, so give the empty body a volatile array sized to the frame (8-byte header)."""
    insns = "\n".join(m.group(1) for m in re.finditer(r"\*/\s*(.+)$", asm, re.M))
    m = EMPTY_FRAME.match(insns)
    if not m or not EMPTY_BODY.search(body):
        return body
    words = max(1, (int(m.group(1), 16) - 8) // 4)
    return EMPTY_BODY.sub(lambda b: f"{b.group(1)}\n    volatile s32 pad[{words}];\n{b.group(2)}\n", body)


FRAME = re.compile(r"\bstwu r1, -(0x[0-9A-Fa-f]+)\(r1\)")
SAVED = re.compile(r"\b(?:stw|stmw|stfd) (?:r(?:1[3-9]|2\d|3[01])|f(?:1[4-9]|2\d|3[01])), (0x[0-9A-Fa-f]+)\(r1\)")


def frame_buffer(body: str, asm: str) -> str:
    """BODY with the topmost address-taken `spXX` scalar widened to a `char` array reaching
    the first callee-saved slot (or the frame top) of the target frame, rounded down to 8
    bytes: m2c types a stack buffer such as a `sprintf` target by its first word, so the
    draft's frame comes out far too small (LoadObjectFile, LoadObjectAnimFile). Unchanged
    when the target frame leaves no gap above the slot."""
    insns = "\n".join(m.group(1) for m in re.finditer(r"\*/\s*(.+)$", asm, re.M)) or asm
    frame = FRAME.search(insns)
    if not frame:
        return body
    saved = [int(m.group(1), 16) for m in SAVED.finditer(insns)]
    top = min(saved) if saved else int(frame.group(1), 16)
    taken = [m for m in SP_DECL.finditer(body)
             if f"&{m['v']}" in body and int(m["off"], 16) < top]
    if not taken:
        return body
    m = max(taken, key=lambda d: int(d["off"], 16))
    size = (top - int(m["off"], 16)) & ~7  # GCC rounds a stack array up to 8 bytes
    if size < 4 * SIZES[m["t"]] or size <= 8:
        return body
    body = body.replace(m[0], f"{m['ind']}char {m['v']}[{size:#x}];\n", 1)
    body = re.sub(rf"\(\w+ \*\)\s*&{m['v']}\b|&{m['v']}\b", m["v"], body)
    return re.sub(rf"(?<![\w&]){m['v']}\b(?!\[)(?=\s*(?:=[^=]|\+\+|--|[-+*/|&^]=))",
                  f"*(s32 *){m['v']}", body)


def byte_strides(body: str) -> str:
    """BODY with each typed local pointer that m2c steps by a byte count (`p += 0x9C` on an
    `s32 *`, which C scales by 4) stepped by bytes again. A pointer only read through
    `(s8 *)(p) + off` casts is declared `void *`, which GCC steps by bytes (fn_800D9824);
    one dereferenced directly keeps its type and steps by elements when the count divides
    (`s16 *p; p += 2` becomes `p += 1`, fn_800C48A4)."""
    for m in reversed(list(WIDE_PTR.finditer(body))):
        v = re.escape(m["v"])
        steps = list(re.finditer(rf"\b{v} ([+-])= (0x[0-9A-Fa-f]+|\d+);", body))
        if not steps:
            continue
        rest = body[:m.start()] + body[m.end():]
        if not re.search(rf"\*\s*\(?{v}\b(?!\s*\))|\b{v}\s*(?:\[|->)|\*\({v}\s*[+-]", rest):
            body = body[:m.start()] + f"{m['ind']}void *{m['v']};" + body[m.end():]
            continue
        size = PTR_SIZES[m["t"]]
        if any(int(st[2], 0) % size for st in steps):
            continue
        for st in reversed(steps):
            body = body[:st.start()] + f"{m['v']} {st[1]}= {int(st[2], 0) // size};" + body[st.end():]
    return body

LOOP_END = re.compile(r"^(?P<ind>[ \t]*)\} while \((?P<cond>.*)\);$")
PTR_BOUND = re.compile(r"(\w+) <= \(void \*\) \(\(\(s8 \*\)(.+)\) \+ (0x[0-9A-Fa-f]+|\d+)\)")


def index_loops(body: str) -> str:
    """BODY with each m2c pointer loop rewritten over an index. m2c prints a loop over an
    array of N-byte records as `p = B; do { ...; p += N; } while (p <= (void *) ((s8 *)B + K));`,
    but GCC formed that pointer from `&B[i]` and compares the eliminated `s32 i` with a signed
    `cmpw` where the draft's pointer compare gives `cmplw` (fn_8014D01C). Each such pointer, and
    each pointer stepped beside a `c += 1` counter that starts at 0 (fn_8014CFA4), is
    recomputed from the index at the top of the body; a pointer bound becomes `i <= K / N`."""
    lines = body.split("\n")
    for end in range(len(lines) - 1, -1, -1):
        new = _index_loop(lines, end)
        if new is not None:
            return index_loops("\n".join(new))  # one loop per pass: line numbers shift
    return body


def _index_loop(lines: list[str], end: int) -> list[str] | None:
    m = LOOP_END.match(lines[end])
    if not m:
        return None
    ind, inner = m["ind"], m["ind"] + "    "
    starts = [i for i in range(end) if lines[i] == f"{ind}do {{"]
    if not starts:
        return None
    start = starts[-1]
    loop = lines[start + 1:end]
    if any(ln.strip() and not ln.startswith(inner) for ln in loop):
        return None  # not the matching `do`
    found: dict[str, list[tuple[int, int]]] = {}
    for k, ln in enumerate(loop):
        sm = re.fullmatch(rf"{inner}(\w+) \+= (0x[0-9A-Fa-f]+|\d+);", ln)
        if sm:
            found.setdefault(sm[1], []).append((k, int(sm[2], 0)))
    steps = {v: st[0] for v, st in found.items() if len(st) == 1}

    def init_of(v: str) -> tuple[int, str] | None:
        for i in range(start - 1, -1, -1):
            if re.search(rf"\b{v}\b", lines[i]):
                im = re.fullmatch(rf"([ \t]*){v} = ([^;]+);", lines[i])
                return (i, im[2]) if im and ind.startswith(im[1]) else None
        return None

    def assigned(v: str, skip: int) -> bool:
        return any(re.search(rf"\b{v}\s*(?:[-+*/|&^]?=(?!=)|\+\+|--)|(?:\+\+|--){v}\b", ln)
                   for k, ln in enumerate(loop) if k != skip)

    after = "\n".join(lines[end + 1:])
    counter = new_index = bound = None
    cm = PTR_BOUND.fullmatch(m["cond"])
    if cm and cm[1] in steps:
        init, n = init_of(cm[1]), steps[cm[1]][1]
        if init and init[1].strip() in (cm[2], f"(void *) {cm[2]}") and int(cm[3], 0) % n == 0:
            new_index, bound = f"i_{cm[1]}", int(cm[3], 0) // n
    else:
        for v, (k, n) in steps.items():
            init = init_of(v)
            if n == 1 and init and init[1] == "0" and re.match(rf"{v} [<>]=? ", m["cond"]) \
                    and not assigned(v, k):
                counter = v
                break
    if not (new_index or counter):
        return None
    ptrs = []
    for v, (k, n) in steps.items():
        if v == counter or (n == 1 and v != (cm[1] if cm else None)):
            continue
        init = init_of(v)
        if init and not re.search(rf"\b{v}\b", after) and not assigned(v, k):
            ptrs.append((v, k, n, init))
    if new_index and not any(v == cm[1] for v, *_ in ptrs):
        return None
    if not ptrs:
        return None
    idx = new_index or counter
    drop = {k for _, k, _, _ in ptrs}
    body = [f"{inner}{v} = (void *) ((s8 *) ({init[1]}) + {idx} * {n});" for v, _, n, init in ptrs]
    body += [ln for k, ln in enumerate(loop) if k not in drop]
    inits = sorted({init[0] for *_, init in ptrs})
    out = list(lines)
    if new_index:
        body.append(f"{inner}{idx} += 1;")
        out[end] = f"{ind}}} while ({idx} <= {bound});"
    out[start + 1:end] = body
    if new_index:
        out[inits[0]] = re.sub(r"\S.*", f"{idx} = 0;", out[inits[0]])
        inits = inits[1:]
    for i in reversed(inits):
        del out[i]
    if new_index:
        top = next((i for i, ln in enumerate(out) if ln == "{" or re.match(r"^\w.*\) \{$", ln)), None)
        if top is None:
            return None
        out.insert(top + 1, f"    s32 {idx};")
    return out


def countdown_loops(body: str) -> str:
    """BODY with each m2c countdown loop written as the `for` loop GCC compiled. m2c prints
    `for (i = 0; i < n; i++) f(a[i]);` as `if (n > 0) { c = n; o = 0; do { f(*(o + a));
    o += 4; c -= 1; } while (c != 0); }`; ProDG schedules the source form differently
    (PodraceLoadLapSettings hoisted a string address out of the draft's loop). Each stepped
    variable is recomputed from the index at the top of the body."""
    lines = body.split("\n")
    for end in range(len(lines) - 1, -1, -1):
        new = _countdown_loop(lines, end)
        if new is not None:
            return countdown_loops("\n".join(new))
    return body


def _countdown_loop(lines: list[str], end: int) -> list[str] | None:
    m = re.fullmatch(r"([ \t]*)\} while \((\w+) != 0\);", lines[end])
    if not m or end + 1 >= len(lines):
        return None
    ind, cnt = m[1], m[2]
    inner, g = ind + "    ", ind[:-4]
    if not ind.endswith("    ") or lines[end + 1] != f"{g}}}":
        return None
    starts = [i for i in range(end) if lines[i] == f"{ind}do {{"]
    if not starts:
        return None
    start = starts[-1]
    loop = lines[start + 1:end]
    if any(ln.strip() and not ln.startswith(inner) for ln in loop):
        return None
    if loop[-1] != f"{inner}{cnt} -= 1;":
        return None
    head = start
    while head > 0 and re.fullmatch(rf"{ind}\w+ = [^;]+;", lines[head - 1]):
        head -= 1
    if head < 1:
        return None
    gm = re.fullmatch(rf"{g}if \((.+) > 0\) \{{", lines[head - 1])
    if not gm:
        return None
    n = gm[1]
    inits = {}
    for ln in lines[head:start]:
        im = re.fullmatch(rf"{ind}(\w+) = ([^;]+);", ln)
        inits[im[1]] = im[2]
    if inits.get(cnt, "").strip("() ") != n.strip("() ") and inits.get(cnt) != n:
        return None
    steps = {}
    rest = []
    for ln in loop[:-1]:
        sm = re.fullmatch(rf"{inner}(\w+) \+= (0x[0-9A-Fa-f]+|\d+);", ln)
        if sm and sm[1] in inits and sm[1] not in steps:
            steps[sm[1]] = int(sm[2], 0)
        else:
            rest.append(ln)
    text = "\n".join(lines)
    after = "\n".join(lines[end + 2:])
    for v in [cnt, *steps]:
        if re.search(rf"\b{v}\b", after):
            return None
    if re.search(rf"\b{cnt}\b", "\n".join(rest)) or set(inits) - set(steps) - {cnt}:
        return None
    for v in steps:
        if re.search(rf"\b{v}\s*(?:[-+*/|&^]?=(?!=)|\+\+|--)", "\n".join(rest)):
            return None
    new = [f"{g}for ({cnt} = 0; {cnt} < {n}; {cnt}++) {{"]
    for v, st in steps.items():
        ptr = re.search(rf"^[ \t]+[\w ]*\*\s*{v};", text, flags=re.M)
        x = inits[v]
        if ptr:
            new.append(f"{ind}{v} = (void *) ((s8 *) ({x}) + {cnt} * {st});")
        else:
            new.append(f"{ind}{v} = {cnt} * {st};" if x == "0" else f"{ind}{v} = ({x}) + {cnt} * {st};")
    new += [ln[4:] for ln in rest]
    new.append(f"{g}}}")
    return lines[:head - 1] + new + lines[end + 2:]


# m2c guesses each callee's prototype from one call site, so a callee read with a
# different argument count elsewhere (or a variadic one) fails to compile. Matched code
# already declares most callees: take the unit's own declaration, else the repo's
# (a definition first), reduced to builtin types so it compiles in any unit.
PROTO = re.compile(r"^(?:extern\s+|static\s+)?((?:const\s+|unsigned\s+|signed\s+|struct\s+)?\w+[\s*]*?)"
                   r"\b(\w+)\s*\(([^()]*)\)\s*(;|\{|$)", re.M)
BUILTIN = {"void", "char", "short", "int", "long", "float", "double", "unsigned", "signed",
           "s8", "u8", "s16", "u16", "s32", "u32", "s64", "u64", "f32", "f64", "BOOL", "size_t"}
NOT_FUNCS = {"if", "while", "for", "switch", "return", "sizeof"}
_PROTOS: dict[str, tuple[str, list[str]]] | None = None
_PROTO_LOCK = threading.Lock()


def plain_type(decl: str, named: bool) -> str | None:
    """DECL's type with any parameter name dropped, pointers as `void *`; None if not builtin."""
    words = decl.replace("*", " * ").replace("const ", " ").split()
    if named and len(words) > 1 and words[-1] not in BUILTIN and words[-1] != "*":
        words = words[:-1]
    if "*" in words:
        return "void *"
    if words and all(w in BUILTIN for w in words):
        return "u32" if words == ["size_t"] else " ".join(words)
    return None


def parse_proto(ret: str, params: str) -> tuple[str, list[str]] | None:
    """(return type, parameter types) with `...` kept, or None when a type is not builtin."""
    rtype = plain_type(ret, False)
    plist = [p.strip() for p in params.split(",")] if params.strip() not in ("", "void") else []
    if rtype is None or params.strip() == "":  # `T f()` says nothing about the arguments
        return None
    types = [p if p == "..." else plain_type(p, True) for p in plist]
    return None if None in types else (rtype, types)


def decl(rtype: str, fn: str) -> str:
    return f"{rtype}{fn}" if rtype.endswith("*") else f"{rtype} {fn}"


def find_protos(text: str) -> dict[str, list[tuple[bool, tuple[str, list[str]]]]]:
    out: dict[str, list] = {}
    for m in PROTO.finditer(text):
        if m[2] not in NOT_FUNCS and (p := parse_proto(m[1], " ".join(m[3].split()))):
            out.setdefault(m[2], []).append((m[4] == "{", p))
    return out


def repo_protos() -> dict[str, tuple[str, list[str]]]:
    """Each function's prototype in src/ and include/: a definition's, else the commonest."""
    global _PROTOS
    with _PROTO_LOCK:
        if _PROTOS is None:
            found: dict[str, list] = {}
            for p in [*(ROOT / "src").rglob("*.[ch]"), *(ROOT / "include").rglob("*.h")]:
                for name, ps in find_protos(p.read_text(errors="replace")).items():
                    found.setdefault(name, []).extend(ps)
            _PROTOS = {}
            for name, ps in found.items():
                defs = [p for d, p in ps if d] or [p for _, p in ps]
                _PROTOS[name] = max(defs, key=lambda p: sum(q == p for _, q in ps))
        return _PROTOS


def variadic_calls(asm: str) -> set[str]:
    """Callees whose calls set or clear cr1eq: variadic or unprototyped at the call."""
    return set(re.findall(r"\bcr(?:clr|set) cr1eq\s*\n(?:.*\n){0,2}?.*\tbl (\w+)", asm))


RAW_PROTO = re.compile(r"^(?:extern\s+|static\s+)?[A-Za-z_][^;{}()\n]*?\b(\w+)\s*\(([^;{}()]*)\)\s*;", re.M)


def raw_protos(text: str) -> dict[str, list[str]]:
    """Parameter lists of the prototypes in TEXT whatever their types (typedef names and
    structs included); `f()` and `f(void)` give []. The last declaration of a name wins."""
    out = {}
    for m in RAW_PROTO.finditer(text):
        if m[1] not in NOT_FUNCS and m[2].strip():
            out[m[1]] = [] if m[2].strip() == "void" else [p.strip() for p in m[2].split(",")]
    return out


FLOATS = ("f32", "f64", "float", "double")


def place_args(args: list[str], kinds: list[str] | None, params: list[str],
               before: str) -> tuple[list[str], tuple[int, int] | None] | None:
    """ARGS (one m2c call whose parameter kinds are KINDS) rearranged for PARAMS by register
    class, or None when nothing moves. m2c lists integer and float arguments by register
    (r3.., then f1..) and leaves an integer argument past r10 as a store to its outgoing
    stack slot (`sp8 = x;` just before the call). Each parameter takes the next argument of
    its class; a ninth or later integer takes that store's value, and the stores are cut:
    the result is (arguments, (start, end) of the stores in BEFORE, or None)."""
    # m2c sometimes misses a trailing float register it did not see written (`fmr f6, f4`
    # read only by the callee): its call is then shorter than its own declaration.
    if not kinds or len(kinds) < len(args) or "..." in kinds:
        return None
    kinds = kinds[:len(args)]
    ints = [a for a, k in zip(args, kinds) if k.split()[0] not in FLOATS]
    flts = [a for a, k in zip(args, kinds) if k.split()[0] in FLOATS]
    out, gpr, stack, end = [], 0, [], before.rfind("\n") + 1
    for p in params:
        if p.split()[0] in FLOATS:
            out.append(flts.pop(0) if flts else "0")
        elif gpr < 8:
            gpr += 1
            out.append(ints.pop(0) if ints else "0")
        else:
            stack.append(len(out))
            out.append("0")
    if ints or flts:
        return None
    if not stack:
        return (out, None) if out != args else None
    # The stores sit on the lines just above the call, in any order.
    cut, found = end, {}
    while True:
        prev = before.rfind("\n", 0, cut - 1) + 1
        m = re.fullmatch(r"[ \t]*sp([0-9A-F]+) = ([^;\n]+);\n", before[prev:cut])
        if not m:
            break
        found.setdefault(int(m[1], 16), m[2])
        cut = prev
    for k, i in enumerate(stack):
        if 8 + 4 * k not in found:
            return None
        out[i] = found[8 + 4 * k]
    return out, (cut, end)


FN_ALIAS = re.compile(r"^#define (?P<fn>\w+) \(\((?P<ret>[^()]*?)\s*\(\*\)\((?P<params>[^()]*)\)\)(?P=fn)\)$")


def fix_prototypes(body: str, name: str, asm: str, unit_text: str, mac: bool = False) -> str:
    """BODY with m2c's callee declarations replaced by known or cr1eq-implied ones and
    each call's argument list trimmed or zero-padded to the declared count. MAC adds the
    Mac demo's parameter types (`mac_protos`) for callees the repo has no prototype for."""
    m = re.search(rf"^\w[^\n;(]*?\b{re.escape(name)}\(", body, flags=re.M)
    if not m:
        return body
    head, code = body[:m.start()], body[m.start():]
    unit = {n: p[-1][1] for n, p in find_protos(unit_text).items()}
    repo, variadic, raw = repo_protos(), variadic_calls(asm), raw_protos(unit_text)
    lines, arity, m2c_kinds = [], {}, {}
    for ln in head.splitlines():
        a = FN_ALIAS.match(ln)
        if a and a["fn"] in unit:
            # `alias_decl` kept m2c's guessed parameters for a callee the unit defines; the
            # return type stays m2c's (it decides the conversion), the parameters are the unit's.
            params = unit[a["fn"]][1]
            arity[a["fn"]] = params
            m2c_kinds[a["fn"]] = [p.strip() for p in a["params"].split(",") if p.strip() not in ("", "void")]
            lines.append(f"#define {a['fn']} (({a['ret']}(*)({', '.join(params) or 'void'})){a['fn']})")
            continue
        d = PROTO.match(ln) if ln.endswith(";") and not ln.startswith("extern") else None
        fn = d[2] if d else None
        if not fn or fn == name:
            lines.append(ln)
            continue
        ret = d[1].strip()
        m2c_kinds[fn] = [p.strip() for p in d[3].split(",") if p.strip() not in ("", "void")]
        if fn in unit:
            arity[fn] = unit[fn][1]
            continue  # the unit's declaration governs
        if fn in raw:  # declared in the unit with types we do not reduce: still fix the arity
            arity[fn] = [(re.match(r"(f32|f64)\b(?!\s*\*)", p) or [p])[0] for p in raw[fn]]
            continue
        known = repo.get(fn) or (mac and mac_protos.proto(fn, ret)) or None
        if fn in variadic:
            if known and "..." in known[1]:
                lines.append(f"{decl(known[0], fn)}({', '.join(known[1])});")
                arity[fn] = known[1]
            else:
                lines.append(f"{ret} {fn}();")
        elif known:
            lines.append(f"{decl(known[0], fn)}({', '.join(known[1]) or 'void'});")
            arity[fn] = known[1]
        else:
            counts = {len(a) for _, _, a in call_sites(code, fn)}
            want = len([p for p in d[3].split(",") if p.strip() not in ("", "void")])
            lines.append(ln if counts <= {want} else f"{ret} {fn}();")
    arrays = set(re.findall(r"^extern (?:const )?f(?:32|64) (\w+)\[\];", head, flags=re.M))
    extra: dict[str, str] = {}
    for fn, params in arity.items():
        n, var = len([p for p in params if p != "..."]), "..." in params
        kinds = m2c_kinds.get(fn)
        sites = list(call_sites(code, fn))
        for k_site, (start, end, args) in reversed(list(enumerate(sites))):
            live = passthrough_regs(asm, fn) if k_site == 0 and not var else None
            new_params: dict[str, str] = {}
            expanded = expand_by_register(args, kinds, params, live, new_params) if live else None
            if expanded:
                # None from place_args is "nothing moves" only when the counts already fit.
                placed = place_args(*expanded, params, code[:start]) or (
                    (expanded[0], None) if len(expanded[0]) == n else None)
            if expanded and placed:
                extra.update((k, v) for k, v in new_params.items() if k not in extra)
            else:
                placed = None if var else place_args(args, kinds, params, code[:start])
            cut = None
            if placed:
                fixed, cut = placed
            else:
                fixed = args[:n] if not var else args
                fixed = fixed + ["0"] * (n - len(fixed))
            # m2c passes a float constant's array by name where the value is meant.
            fixed = [f"{a}[0]" if a in arrays and i < n and params[i] in ("f32", "f64") else a
                     for i, a in enumerate(fixed)]
            if live:
                fixed = passthrough(fixed, params, live, extra)
            if fixed != args:
                open_paren = code.index("(", start)
                code = code[:open_paren] + "(" + ", ".join(fixed) + ")" + code[end:]
            if cut:
                code = code[:cut[0]] + code[cut[1]:]
    if extra:
        code = add_params(code, name, extra)
    return repair_indirect_prototypes("\n".join(lines) + ("\n" if lines else "") + code, asm)


def expand_by_register(args: list[str], kinds: list[str] | None, params: list[str],
                       live: tuple[set[int], set[int]], extra: dict[str, str]
                       ) -> tuple[list[str], list[str]] | None:
    """ARGS (m2c's call, KINDS its parameter kinds) spread over PARAMS' argument registers.

    m2c lists only the argument registers it saw written, in register order (`g(0x1000)`
    for a call that sets just r10), so after an unwritten register (LIVE: the caller's own
    argument passed through) every later argument shifts left. Each unwritten slot takes
    the target's `argN`/`fargN` (recorded in EXTRA) and each written one the next m2c
    argument of its class. Returns (arguments, kinds) in m2c's class-sorted form for
    `place_args`, or None when the counts do not line up or nothing is passed through."""
    if kinds is None or len(kinds) < len(args) or "..." in kinds:
        return None
    kinds = kinds[:len(args)]
    ints = [a for a, k in zip(args, kinds) if k.split()[0] not in FLOATS]
    flts = [a for a, k in zip(args, kinds) if k.split()[0] in FLOATS]
    n_r = min(8, sum(p.split()[0] not in FLOATS for p in params))
    n_f = min(8, sum(p.split()[0] in FLOATS for p in params))
    out = {}
    for cls, n, have, regs, prefix in (("r", n_r, ints, live[0], "arg"), ("f", n_f, flts, live[1], "farg")):
        gaps = [k for k in range(n) if k in regs]
        if not gaps or len(have) != n - len(gaps):
            out[cls] = None
            continue
        it = iter(have)
        out[cls] = [f"{prefix}{k}" if k in regs else next(it) for k in range(n)]
    if out["r"] is None and out["f"] is None:
        return None
    if out["r"] is None:
        out["r"] = ints
    if out["f"] is None:
        out["f"] = flts
    cls_params = {"r": [p for p in params if p.split()[0] not in FLOATS][:8],
                  "f": [p for p in params if p.split()[0] in FLOATS][:8]}
    for cls, prefix in (("r", "arg"), ("f", "farg")):
        for k, a in enumerate(out[cls]):
            if a == f"{prefix}{k}" and k < len(cls_params[cls]):
                extra.setdefault(a, cls_params[cls][k])
    return out["r"] + out["f"], ["s32"] * len(out["r"]) + ["f32"] * len(out["f"])


def passthrough(fixed: list[str], params: list[str], live: tuple[set[int], set[int]],
                extra: dict[str, str]) -> list[str]:
    """FIXED with each literal `0` argument whose register the target never wrote before
    calling FN replaced by the target's own incoming argument (`argN`/`fargN`, recorded in
    EXTRA with the callee's parameter type). A real zero needs an `li`/`fmr` into the
    register, so an unwritten one is the caller's value passed through (m2c and the arity
    repair both fill such registers with `0`)."""
    if "..." in params:
        return fixed
    out, idx = list(fixed), {"f": 0, "r": 0}
    for i, p in enumerate(params):
        if i >= len(out):
            break
        cls = "f" if p.split()[0] in FLOATS else "r"
        k = idx[cls]
        idx[cls] += 1
        if out[i] != "0" or k >= 8 or k not in live[0 if cls == "r" else 1]:
            continue
        nm = f"{'farg' if cls == 'f' else 'arg'}{k}"
        out[i] = nm
        extra.setdefault(nm, p)
    return out


def adopt_own_prototype(body: str, name: str, unit_text: str) -> str:
    """BODY with NAME's definition header taken from the unit's own prototype of NAME.

    A unit often declares the target (`u8 *f(void *, char *);`) for earlier callers; the
    draft's m2c-guessed signature then fails with `conflicting types`. The unit's return
    and parameter types win; m2c's parameter names are kept by position, missing ones are
    padded as `padN`. Returns BODY unchanged when there is no prototype or the header is
    not found."""
    n = re.escape(name)
    proto = re.search(rf"^(?:extern\s+|static\s+)?([^;{{}}()\n]*?)\b{n}\s*\(([^;{{}}]*?)\)\s*;", unit_text, flags=re.M)
    head = re.search(rf"^(\w[^\n;(]*?)\b{n}\((.*?)\) \{{$", body, flags=re.M)
    if not proto or not head:
        return body
    ptypes = [p.strip() for p in proto[2].split(",")] if proto[2].strip() not in ("", "void") else []
    if proto[2].strip() == "":
        return body  # `f()` says nothing about the arguments
    names = [re.split(r"[\s*]+", p.strip())[-1] for p in head[2].split(",") if p.strip() not in ("", "void")]
    params = []
    for i, t in enumerate(ptypes):
        if t == "...":
            params.append(t)
            continue
        # A prototype may name its parameters; keep only the type.
        words = t.replace("*", " * ").split()
        if len(words) > 1 and words[-1] != "*" and words[-1] not in BUILTIN:
            words = words[:-1]
        typ = " ".join(words).replace("* *", "**")
        nm = names[i] if i < len(names) else f"pad{i}"
        params.append(f"{typ}{nm}" if typ.endswith("*") else f"{typ} {nm}")
    ret = proto[1].strip()
    new = f"{decl(ret, name)}({', '.join(params) or 'void'}) {{"
    return body[:head.start()] + new + body[head.end():]


def score(name: str, source: str, body: str, apply: bool = False) -> dict:
    with tempfile.NamedTemporaryFile("w", suffix=".c", delete=False) as f:
        f.write(body)
        path = f.name
    try:
        cmd = [sys.executable, str(ROOT / "tools" / "score_fn.py"), name, source, "--body", path, "--no-queue"]
        cmd += ["--apply"] if apply else ["--json"]
        r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return {"verdict": "TIMEOUT"}
    finally:
        Path(path).unlink(missing_ok=True)
    if apply:
        return {"verdict": "MATCH" if r.returncode == 0 else "NOT-APPLIED", "log": r.stdout[-300:]}
    if r.returncode == 2 and not r.stdout.strip().startswith("{"):
        return {"verdict": "COMPILE-ERROR", "error": r.stderr}
    try:
        rep = json.loads(r.stdout)
    except ValueError:
        return {"verdict": "ERROR", "error": (r.stdout + r.stderr)[-400:]}
    return {"verdict": rep.get("verdict"), "exact": rep.get("exact_insns"), "target": rep.get("target_insns")}


RANK = {"MATCH": 3, "MISMATCH": 2}


def better(a: dict, b: dict) -> bool:
    """Whether score A beats score B (MATCH, then more exact instructions)."""
    return (RANK.get(a["verdict"], 0), a.get("exact") or 0) > (RANK.get(b["verdict"], 0), b.get("exact") or 0)


def improves(new: dict, old: dict) -> bool:
    """Whether a repaired draft's score replaces OLD. Repairs accumulate, so a draft that
    still fails to compile is kept in its further-repaired form (its error is the live one)."""
    return better(new, old) or new["verdict"] == old["verdict"] == "COMPILE-ERROR"


def sweep_one(t: dict) -> dict:
    name, source = t["name"], t["source"]
    row = {"name": name, "unit": t["unit"], "source": source, "size": t["size"]}
    best = None
    ctx = unit_context(source)
    unit_text = (ctx or ROOT / source).read_text(errors="replace")
    drafts = []
    for context in [None] + ([ctx] if ctx else []):
        plain = draft(name, context)
        drafts.append((context, plain))
        sized = draft(name, context, sized=True)
        if sized != plain:  # scored only where the width casts change the draft
            drafts.append((context, sized))
    for context, (status, body) in drafts:
        if status != "ok":
            res = {"verdict": status.upper()}
        else:
            res, body = score_draft(name, source, stack_arrays(body))
            asm_text = (OUT / name / "asm.s").read_text()
            for framer in (empty_frame, frame_buffer):  # target frame size: kept only when better
                framed = framer(body, asm_text)
                if framed != body:
                    framed_res, framed = score_draft(name, source, framed)
                    if improves(framed_res, res):
                        res, body = {**framed_res, framer.__name__: True}, framed
            for repair in (byte_strides, base_pointer, loop_guard, index_loops, countdown_loops, positive_guards):  # scored repairs: kept only when better
                alt = repair(body)
                if alt != body:
                    alt_res, alt = score_draft(name, source, alt)
                    if improves(alt_res, res):
                        res, body = {**alt_res, repair.__name__: True}, alt
            padded = pad_signature(body, name)
            if padded != body:
                pad_res, padded = score_draft(name, source, padded)
                if improves(pad_res, res):
                    res, body = {**pad_res, "padded": True}, padded
            pre = body
            protos = fix_prototypes(body, name, (OUT / name / "asm.s").read_text(), unit_text)
            if protos != body:
                proto_res, protos = score_draft(name, source, protos)
                if improves(proto_res, res):
                    res, body = {**proto_res, "padded": res.get("padded", False), "prototypes": True}, protos
            if res["verdict"] == "COMPILE-ERROR":
                own = adopt_own_prototype(body, name, unit_text)
                if own != body:
                    own_res, own = score_draft(name, source, own)
                    if improves(own_res, res):
                        res, body = {**own_res, "padded": res.get("padded", False), "own_prototype": True}, own
            for mac_body in (fix_prototypes(pre, name, (OUT / name / "asm.s").read_text(), unit_text, True),
                             mac_protos.pad_params(body, name)):  # the Mac demo's types: strictly better only
                if mac_body not in (pre, body):
                    mac_res, mac_body = score_draft(name, source, mac_body)
                    if better(mac_res, res):
                        res, body = {**mac_res, "padded": res.get("padded", False), "mac": True}, mac_body
            res["context"] = context is not None
            res, body = try_variants(name, source, res, body)
        if best is None or better(res, best[0]):
            best = (res, body)
        if res["verdict"] == "MATCH":
            break
    res, body = best
    if res["verdict"] in ("MATCH", "MISMATCH"):
        (OUT / name / "sweep_draft.c").write_text(body)
    elif res["verdict"] == "COMPILE-ERROR" and body:
        (OUT / name / "sweep_error.c").write_text(body)  # the failing draft, for repair work
    err = res.pop("error", "").strip()
    if res["verdict"] in ("COMPILE-ERROR", "ERROR") and err:
        msgs = [ln for ln in err.splitlines() if re.search(r":\d+: ", ln)
                and not re.search(r"\(Each undeclared|for each function it appears in|previous declaration|"
                                  r"this is the first entry", ln)]
        res["error"] = (msgs[0] if msgs else err.splitlines()[-1])[:200]
    return {**row, **res}


def float_positions(body: str, limit: int = 40) -> list[str]:
    """BODY with a draft-declared callee's float parameters moved earlier in its prototype
    and calls. PowerPC passes floats in f1.. whatever their position, so m2c lists them
    last; their real positions only show in the order GCC sets the argument registers
    (`fmr f2, f1` before or after `li r9`). Each alternative keeps the floats' relative
    order and moves the call arguments with them; at most LIMIT in all."""
    out = []
    for d in PROTO.finditer(body):
        fn, end = d[2], d[4]
        params = [p.strip() for p in d[3].split(",") if p.strip() not in ("", "void")]
        if end != ";" or fn in NOT_FUNCS or "..." in params:
            continue
        isf = [p.split()[0] in FLOATS for p in params]
        nf = sum(isf)
        if not nf or nf == len(params) or isf != [False] * (len(params) - nf) + [True] * nf:
            continue
        sites = [s for s in call_sites(body, fn) if len(s[2]) == len(params)]
        if not sites:
            continue
        ints, flts = params[:-nf], params[-nf:]
        for pos in itertools.combinations(range(len(params)), nf):
            if list(pos) == list(range(len(params) - nf, len(params))):
                continue
            def place(seq_i, seq_f):
                it, fl = iter(seq_i), iter(seq_f)
                return [next(fl) if k in pos else next(it) for k in range(len(params))]
            new = body
            for start, stop, args in sorted(sites, reverse=True):
                moved = place(args[:-nf], args[-nf:])
                paren = new.index("(", start)
                new = new[:paren] + "(" + ", ".join(moved) + ")" + new[stop:]
            new = new[:d.start(3)] + ", ".join(place(ints, flts)) + new[d.end(3):]
            out.append(new)
            if len(out) >= limit:
                return out
    return out


SWITCH = re.compile(r"^(?P<ind>[ \t]*)switch \(.*\) \{\n", re.M)


def switch_pivots(body: str) -> list[str]:
    """BODY with an empty `case max+1:` (then `case min-1:`) added to each switch of three
    or more integer cases. GCC 2.95 balances its compare tree on the case count, so a
    source case with an empty body (dropped from the code) moves the pivot."""
    out = []
    for m in SWITCH.finditer(body):
        ind = m["ind"]
        close = re.compile(rf"^{re.escape(ind)}\}}\n?", re.M).search(body, m.end())
        if not close:
            continue
        labels = [int(v, 0) for v in re.findall(rf"^{re.escape(ind)}(?:    )?case (-?(?:0x[0-9A-Fa-f]+|\d+)):",
                                                 body[m.end():close.start()], flags=re.M)]
        if len(labels) < 3:
            continue
        case_ind = re.search(r"^([ \t]*)case ", body[m.end():close.start()], flags=re.M)[1]
        for k in (max(labels) + 1, min(labels) - 1):
            extra = f"{case_ind}case {k}:\n{case_ind}    break;\n"
            out.append(body[:close.start()] + extra + body[close.start():])
    return out


def split_top(expr: str, op: str) -> list[str]:
    """EXPR split at each OP (`||`, `&&`, `==`, ...) outside parentheses."""
    parts, depth, cur, i = [], 0, 0, 0
    while i < len(expr):
        c = expr[i]
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
        elif depth == 0 and expr.startswith(op, i) and not expr.startswith(op + "=", i) \
                and (op not in ("==", "!=") or expr[i - 1] not in "<>=!"):
            parts.append(expr[cur:i].strip())
            i += len(op)
            cur = i
            continue
        i += 1
    parts.append(expr[cur:].strip())
    return parts


def unwrap(expr: str) -> str:
    """EXPR without parentheses that enclose all of it."""
    while expr.startswith("("):
        depth = 0
        for i, c in enumerate(expr):
            depth += (c == "(") - (c == ")")
            if depth == 0:
                break
        if i != len(expr) - 1:
            return expr
        expr = expr[1:-1].strip()
    return expr


def negate(expr: str) -> str:
    """The C negation of EXPR without NaN-unsafe flips: `!(e)` gives e, a top-level `==`
    or `!=` flips, anything else is wrapped in `!(...)`."""
    e = unwrap(expr)
    if e.startswith("!(") and unwrap(e[1:]) != e[1:]:
        return unwrap(e[1:])  # `!(a > b)`: only when the parentheses span the rest
    for op, inv in (("!=", "=="), ("==", "!=")):
        sides = split_top(e, op)
        if len(sides) == 2 and len(split_top(e, "||")) == 1 and len(split_top(e, "&&")) == 1:
            return f"{sides[0]} {inv} {sides[1]}"
    return f"!({e})"


GUARD_RETURN = re.compile(r"^(?P<ind>[ \t]*)if \((?P<cond>.*)\) \{\n(?P=ind)    return (?P<a>[^;\n]+);\n"
                          r"(?P=ind)\}\n(?P=ind)return (?P<b>[^;\n]+);\n", re.M)


def positive_guards(body: str) -> str:
    """BODY with each `if (A || B || !(C)) { return a; } return b;` written the other
    way round as `if (!A && !B && C) { return b; } return a;`. m2c inverts the branch it
    sees, so a source `if (x == 1 && y > z) return 1; return 0;` drafts as the early
    `return 0` guard, and GCC then lays out the comparisons and `li r3` differently."""
    def flip(m: re.Match) -> str:
        terms = split_top(unwrap(m["cond"]), "||")
        if len(terms) == 1 and len(split_top(terms[0], "&&")) > 1:
            return m[0]
        cond = " && ".join(f"({t})" if len(split_top(t, "&&")) > 1 or len(split_top(t, "||")) > 1
                           else t for t in (negate(t) for t in terms))
        ind = m["ind"]
        return (f"{ind}if ({cond}) {{\n{ind}    return {m['b']};\n{ind}}}\n"
                f"{ind}return {m['a']};\n")
    return GUARD_RETURN.sub(flip, body)


def try_variants(name: str, source: str, res: dict, body: str) -> tuple[dict, str]:
    """Score each VARIANTS rewrite of a draft at least 80% exact; keep the best."""
    if res["verdict"] != "MISMATCH" or (res.get("exact") or 0) < 0.8 * (res.get("target") or 1):
        return res, body
    seen = {body}
    alts = [(v.__name__, v(body)) for v in VARIANTS + (sorted_stores,)]
    alts += [("swap_stores", a) for a in swap_stores(body)]
    alts += [("switch_pivots", a) for a in switch_pivots(body)]
    alts += [("float_positions", a) for a in float_positions(body)]
    alts += [("volatile_locals", a) for a in volatile_locals(body)]
    alts += [("int_callees", a) for a in int_callees(body)]
    alts += [("float_blocks", a) for a in float_blocks(body)]
    for vname, alt in alts:
        if alt in seen:
            continue
        seen.add(alt)
        alt_res, alt = score_draft(name, source, alt)
        if better(alt_res, res):
            res, body = {**alt_res, "context": res.get("context"), "rewrite": vname}, alt
            if res["verdict"] == "MATCH":
                break
    return res, body


def score_draft(name: str, source: str, body: str) -> tuple[dict, str]:
    """Score BODY, dropping m2c declarations that conflict with SOURCE; (result, final body)."""
    for _ in range(4):
        res = score(name, source, body)
        if res["verdict"] != "COMPILE-ERROR":
            break
        syms = [next(g for g in m.groups() if g) for m in CONFLICT.finditer(res.get("error", ""))]
        for sym in syms:
            if (new := alias_decl(body, sym, name) or drop_decl(body, sym, name)):
                body = new
                break
        else:
            break
    else:
        res = score(name, source, body)
    return res, body


def kept_rows(previous: list[dict], unit: str, todo: list[dict]) -> list[dict]:
    """Rows of an earlier results.json that a sweep of the unit prefix UNIT does not replace."""
    names = {t["name"] for t in todo}
    return [r for r in previous if not r["unit"].startswith(unit) and r["name"] not in names
            and not r.get("applied")]


def targets(args) -> list[dict]:
    cmd = [sys.executable, str(ROOT / "tools" / "next_target.py"), "--json", "--no-remote",
           "--limit", "100000", "--max-size", args.max_size]
    if args.unit:
        cmd += ["--unit", args.unit]
    return json.loads(subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, check=True).stdout)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--unit", help="unit prefix, as for next_target.py")
    ap.add_argument("--max-size", default="0x100000", help="largest function to sweep")
    ap.add_argument("-j", type=int, default=16, help="parallel jobs")
    ap.add_argument("--apply", action="store_true", help="write strict MATCHes into their unit sources")
    ap.add_argument("--top", type=int, default=30, help="near-misses to print")
    ap.add_argument("--retry", metavar="VERDICT", action="append",
                    help="resweep only the targets the last results.json gave VERDICT, or the named "
                         "target (repeatable, e.g. COMPILE-ERROR), and merge the new rows into it")
    args = ap.parse_args(argv)
    if not (draft_fn.m2c_dir() / "m2c.py").is_file():
        print("error: m2c not fetched; run bash tools/fetch_m2c.sh", file=sys.stderr)
        return 2
    if not any((ROOT / "build/GL5E4F/asm").rglob("*.s")):
        print("error: no generated target assembly; run bash build.sh to completion before sweeping",
              file=sys.stderr)
        return 2
    todo = targets(args)
    previous = []
    if args.retry:
        previous = json.loads((OUT / "results.json").read_text())
        names = {r["name"] for r in previous if r["verdict"] in args.retry or r["name"] in args.retry}
        names |= set(args.retry)  # a named target the last sweep did not cover
        todo = [t for t in todo if t["name"] in names]
        previous = [r for r in previous if r["name"] not in {t["name"] for t in todo}
                    and not r.get("applied")]
    elif args.unit and (OUT / "results.json").is_file():
        # a unit-scoped sweep refreshes that unit's rows and keeps the rest of the ranking
        previous = kept_rows(json.loads((OUT / "results.json").read_text()), args.unit, todo)
    OUT.mkdir(parents=True, exist_ok=True)
    rows = []
    with ThreadPoolExecutor(args.j) as pool:
        futures = {pool.submit(sweep_one, t): t for t in todo if t.get("source")}
        for i, fut in enumerate(as_completed(futures), 1):
            try:
                rows.append(fut.result())
            except Exception as exc:  # one bad target must not lose the sweep
                t = futures[fut]
                rows.append({"name": t["name"], "unit": t["unit"], "source": t["source"],
                             "size": t["size"], "verdict": "ERROR", "error": str(exc)[:200]})
            if i % 100 == 0:
                print(f"  {i}/{len(todo)}", file=sys.stderr)
    applied = []
    if args.apply:
        for row in sorted((r for r in rows if r["verdict"] == "MATCH"), key=lambda r: (r["source"], r["name"])):
            body = (OUT / row["name"] / "sweep_draft.c").read_text()
            ok = score(row["name"], row["source"], body, apply=True)["verdict"] == "MATCH"
            if not ok:  # an earlier match in the same unit can now conflict with its externs
                res, body = score_draft(row["name"], row["source"], body)
                ok = res["verdict"] == "MATCH" and score(
                    row["name"], row["source"], body, apply=True)["verdict"] == "MATCH"
                if ok:
                    (OUT / row["name"] / "sweep_draft.c").write_text(body)
            if ok:
                applied.append(row["name"])
                row["applied"] = True
    if previous:
        print(f"swept {len(rows)} targets; {len(previous)} rows kept from the last results.json")
    rows += previous
    for r in rows:
        r["closeness"] = (r["exact"] / r["target"]) if r.get("target") else 0.0
    rows.sort(key=lambda r: (r["verdict"] != "MATCH", -(r["size"] * r["closeness"])))
    (OUT / "results.json").write_text(json.dumps(rows, indent=1) + "\n")
    counts: dict[str, int] = {}
    for r in rows:
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1
    match_bytes = sum(r["size"] for r in rows if r["verdict"] == "MATCH")
    print(f"swept {len(rows)} targets: " + ", ".join(f"{k} {v}" for k, v in sorted(counts.items())))
    print(f"strict MATCH: {counts.get('MATCH', 0)} functions, {match_bytes:,} bytes"
          + (f"; applied {len(applied)}" if args.apply else " (rerun with --apply to write them)"))
    near = [r for r in rows if r["verdict"] == "MISMATCH" and r["closeness"] >= 0.9]
    print(f"near misses (>=90% exact): {len(near)} functions, {sum(r['size'] for r in near):,} bytes")
    for r in sorted(near, key=lambda r: -r["size"] * r["closeness"])[:args.top]:
        print(f"  {r['name']:<28} {r['unit']:<28} 0x{r['size']:<5x} {r['exact']}/{r['target']}")
    print(f"results: {(OUT / 'results.json').relative_to(ROOT)}")
    if counts.get("NO-ASM"):
        print("error: incomplete sweep (NO-ASM); run bash build.sh, then rerun the affected units",
              file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    sys.exit(main())
