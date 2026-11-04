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
import json
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

OUT = ROOT / "work" / "sweep"
UNKNOWN_TYPES = {"M2C_UNK": "s32", "M2C_UNK8": "s8", "M2C_UNK16": "s16", "M2C_UNK32": "s32",
                 "M2C_UNK64": "s64"}
# Macros that only stand for an instruction m2c could not express; such a draft cannot match.
UNHANDLED = re.compile(r"\bM2C_(?:ERROR|TRAP_IF|BREAK|SYNC|DCACHE|ICACHE|PREFETCH|CARRY|OVERFLOW|"
                       r"LWL|FIRST3BYTES|UNALIGNED32|LOAD_|STORE_)|\b(?:GLUE_F64|MULT_HI|MULTU_HI|"
                       r"CLZ|ROTATE_RIGHT|BSWAP\d+)\(")


def split_args(text: str, start: int) -> tuple[list[str], int]:
    """Comma-separated arguments of the call whose `(` is at text[start]; returns (args, end)."""
    depth, args, cur = 0, [], start + 1
    for i in range(start, len(text)):
        c = text[i]
        if c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
            if depth == 0:
                args.append(text[cur:i])
                return [a.strip() for a in args], i + 1
        elif c == "," and depth == 1:
            args.append(text[cur:i])
            cur = i + 1
    raise ValueError("unbalanced macro call")


def expand_macros(text: str) -> str:
    """Rewrite m2c valid-syntax macros as plain C casts so a matched draft needs no header."""
    for name, ctype in UNKNOWN_TYPES.items():
        text = re.sub(rf"\b{name}\b", ctype, text)
    # With a typed context m2c reads the first member of a struct local as
    # M2C_BITWISE(f32, sp8); a cast cannot convert a struct, so reinterpret its storage.
    struct_locals = set(re.findall(r"^\s+struct \w+ (\w+);", text, flags=re.M))
    while (m := re.search(r"\bM2C_BITWISE\s*\(", text)):
        args, end = split_args(text, m.end() - 1)
        fmt = "(*({0} *)&({1}))" if args[1] in struct_locals else "(({0})({1}))"
        text = text[:m.start()] + fmt.format(*args) + text[end:]
    for macro, fmt in (("M2C_FIELD", "(*({1})((s8 *)({0}) + ({2})))"),
                       ("M2C_STRUCT_COPY", "memcpy({0}, {1}, {2})"),
                       ("M2C_MEMCPY_ALIGNED", "memcpy({0}, {1}, {2})")):
        while (m := re.search(rf"\b{macro}\s*\(", text)):
            args, end = split_args(text, m.end() - 1)
            text = text[:m.start()] + fmt.format(*args) + text[end:]
    return text


# ProDG emits float `<=`/`>=` as `fcmpu; cror un, eq, lt|gt; bso|bns`. m2c cannot read the
# cror, but folding it into the branch is exact, including the unordered case:
# un|eq|lt set <=> !gt, so bso -> ble and bns -> bgt (and with gt: bso -> bge, bns -> blt).
CROR = re.compile(r"^(.*?)\bcror (?P<f>cr[0-7])?un, (?P=f)?eq, (?P=f)?(lt|gt)\s*$")
FOLD = {("lt", "bso"): "ble", ("lt", "bns"): "bgt", ("gt", "bso"): "bge", ("gt", "bns"): "blt"}


def fold_cror(asm: str) -> str:
    """Rewrite cror-based float comparisons into branches m2c understands."""
    lines = asm.splitlines()
    for i, line in enumerate(lines):
        if not (m := CROR.match(line)):
            continue
        nxt = lines[i + 1] if i + 1 < len(lines) else ""
        b = re.search(r"\t(bso|bns)(lr)?\b", nxt)
        f = m.group("f")
        if b and f and not re.match(rf"\s*{f}\b", nxt[b.end():]):
            b = None
        if not b:
            continue  # not the float pattern: the patched m2c evaluates the cror itself
        lines[i + 1] = nxt[:b.start() + 1] + FOLD[(m.group(3), b.group(1))] + (b.group(2) or "") + nxt[b.end():]
        lines[i] = None
    return "\n".join(ln for ln in lines if ln is not None) + "\n"


# Game code inlines NuVecAdd/NuVecSub as a fixed eight-instruction paired-single asm block
# (src/numath/nuvec_ps.h): psq_l f1,f3,f2,f4 from two base registers, ps_add|ps_sub twice,
# psq_st twice through a third. m2c has no paired-single model, so the block becomes a call
# `NuVec{Add,Sub}PS(dst, a, b)`; the draft includes the header whose macro reproduces it.
PS_BLOCK = re.compile(
    r"psq_l f1, 0x0\((?P<a>r\d+)\), 0, qr0\npsq_l f3, 0x0\((?P<b>r\d+)\), 0, qr0\n"
    r"psq_l f2, 0x8\((?P=a)\), 1, qr0\npsq_l f4, 0x8\((?P=b)\), 1, qr0\n"
    r"ps_(?P<op>add|sub) f1, f1, f3\nps_(?P=op) f2, f2, f4\n"
    r"psq_st f1, 0x0\((?P<d>r\d+)\), 0, qr0\npsq_st f2, 0x8\((?P=d)\), 1, qr0\n")
ASM_COMMENT = re.compile(r"^/\*.*?\*/\s*")
PS_CALLS = ("NuVecAddPS", "NuVecSubPS")


def fix_ps_calls(body: str) -> str:
    """Give every NuVec{Add,Sub}PS call exactly its three arguments."""
    for fn in PS_CALLS:
        for start, end, args in reversed(list(call_sites(body, fn))):
            fixed = (args[:3] + ["0"] * 3)[:3]
            call = f"{fn}({', '.join(fixed)})"
            # The block is an asm statement with no value; m2c models r3 as the return
            # value, and after the fold's `mr r3, dst` that is the destination.
            if (m := re.search(r"([ \t]*)(\w+) = $", body[:start])) and body[end:end + 1] == ";":
                body = (body[:m.start()] + f"{m.group(1)}{call};\n{m.group(1)}{m.group(2)} = {fixed[0]}"
                        + body[end:])
            else:
                body = body[:start] + call + body[end:]
    return body


def moves_to_args(srcs: list[str]) -> list[str]:
    """`mr` lines that load SRCS into r3, r4, r5 in parallel (r0 breaks a cycle). Registers
    already in place get a trailing `mr rN, rN` so m2c still sees them passed."""
    pending = [(f"r{3 + i}", s) for i, s in enumerate(srcs) if s != f"r{3 + i}"]
    out = []
    while pending:
        free = next(((d, s) for d, s in pending if all(d != s2 for _, s2 in pending)), None)
        if free is None:
            d0 = pending[0][0]
            out.append(f"\tmr r0, {d0}")
            pending = [(d, "r0" if s == d0 else s) for d, s in pending]
            continue
        pending.remove(free)
        out.append(f"\tmr {free[0]}, {free[1]}")
    out += [f"\tmr r{3 + i}, r{3 + i}" for i, s in enumerate(srcs) if s == f"r{3 + i}"]
    return out


def fold_ps(asm: str) -> str:
    """Rewrite each inlined paired-single vector add/sub block as a call m2c can read."""
    lines = asm.splitlines()
    text = [ASM_COMMENT.sub("", ln).strip() for ln in lines]
    out, i = [], 0
    while i < len(lines):
        m = PS_BLOCK.match("\n".join(text[i:i + 8]) + "\n") if i + 8 <= len(lines) else None
        if not m:
            out.append(lines[i])
            i += 1
            continue
        body = [*moves_to_args([m["d"], m["a"], m["b"]]), f"\tbl {PS_CALLS[m['op'] == 'sub']}"]
        out.extend(body)
        i += 8
    return "\n".join(out) + "\n"


# m2c writes `*(base + off)` when it cannot type the pointer; ProDG rejects a deref of an
# integer or void *. Most such accesses are words, so give them an s32 pointer. Binary `*`
# is always spaced in m2c output, and an existing `*(T *)` cast is left alone.
UNTYPED_DEREF = re.compile(r"\*\((?![\w ]+\*+\))")


def type_derefs(text: str) -> str:
    return UNTYPED_DEREF.sub("*(s32 *)(", text)


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


def strip_vararg_saves(asm: str) -> str:
    """Drop a variadic prologue's `bne cr1, .L; stfd f1..f8` float save area.

    The compiler emits it itself (cr1eq is the ABI's "no float arguments" flag), and m2c
    would read cr1eq and the float registers before anything sets them. The arguments the
    body reads from the va_list area remain stack accesses in the draft.
    """
    lines = asm.splitlines()
    for i, line in enumerate(lines):
        m = re.search(r"\bbne cr1, (\.L_\w+)\s*$", line)
        if not m:
            continue
        saves = lines[i + 1:i + 9]
        if all(re.search(rf"\bstfd f{n + 1}, 0x[0-9a-f]+\(r1\)\s*$", ln) for n, ln in enumerate(saves)) \
                and len(saves) == 8:
            return "\n".join(lines[:i] + lines[i + 9:]) + "\n"
    return asm


UNSET = re.compile(r"M2C_ERROR\(/\* Read from unset register \$(?P<r>[rf]\d+) \*/\)")


def declare_unset(text: str, name: str) -> str:
    """Name registers the target reads before writing as uninitialized locals.

    The retail code passes or stores a variable that was never assigned (an `mr r10, r11`
    with r11 unset); the local keeps that visible in the draft instead of an M2C_ERROR.
    """
    regs = sorted({m["r"] for m in UNSET.finditer(text)})
    # m2c names a callee-saved register read before any write (`saved_reg_r28`) without declaring it.
    saved = sorted({s for s in re.findall(r"\bsaved_reg_\w+\b", text)
                    if not re.search(rf"^\s+\w[\w ]*\b{s};", text, flags=re.M)})
    if not regs and not saved:
        return text
    decls = "".join(f"    {'s32' if r[0] == 'r' else 'f64'} unset_{r};\n" for r in regs)
    decls += "".join(f"    {'f64' if s.startswith('saved_reg_f') else 's32'} {s};\n" for s in saved)
    text = UNSET.sub(lambda m: "unset_" + m["r"], text)
    return re.sub(rf"^(.*\b{re.escape(name)}\(.*\) \{{\n)", lambda m: m.group(1) + decls, text,
                  count=1, flags=re.M)


def draft(name: str, context: Path | None = None) -> tuple[str, str]:
    """(status, C body) for NAME; status is 'ok', 'no-asm', 'm2c-failed' or 'unhandled'."""
    task = OUT / name
    asm = task / "asm.s"
    if not asm.is_file():
        subprocess.run([sys.executable, str(ROOT / "tools" / "ls1_task_pack.py"), name,
                        "--out-dir", str(OUT)], cwd=ROOT, capture_output=True)
    if not asm.is_file() or not asm.read_text().strip():
        return "no-asm", ""
    m2c = draft_fn.m2c_dir()
    m2c_in = task / "asm_m2c.s"
    m2c_in.write_text(fold_ps(fold_cror(strip_vararg_saves(asm.read_text()))))
    cmd = [sys.executable, str(m2c / "m2c.py"), "-t", draft_fn.TARGET, "--valid-syntax"]
    if context is not None:
        cmd += ["--no-cache", "--context", str(context)]
    r = subprocess.run(cmd + [str(m2c_in.resolve())], capture_output=True, text=True)
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
    if any(c in body for c in PS_CALLS):  # the macros stand in for m2c's guessed prototypes
        body = re.sub(rf"^(?=\S)[^\n]*\b(?:{'|'.join(PS_CALLS)})\([^\n]*\);\n", "", body, flags=re.M)
        body = fix_ps_calls(body)
        body = '#include "numath/nuvec_ps.h"\n' + body
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


def both_rewrites(body: str) -> str:
    return const_rodata(store_in_arms(body))


VARIANTS = (store_in_arms, const_rodata, both_rewrites)


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


CONFLICT = re.compile(r"conflicting types for [`'‘](\w+)|redefinition of [`'‘](\w+)|"
                      r"previous declaration of [`'‘](\w+)")


def drop_decl(body: str, sym: str, name: str) -> str | None:
    """Remove m2c's top-level declaration of SYM (never NAME's own definition)."""
    if sym == name:
        return None
    lines = body.splitlines()
    kept = [ln for ln in lines if not (ln.rstrip().endswith(";") and not ln.startswith((" ", "\t"))
                                        and re.search(rf"\b{re.escape(sym)}\b", ln))]
    return "\n".join(kept) + "\n" if len(kept) != len(lines) else None


# m2c names a stack slot by offset and declares only the slots it sees by name, so an
# out-parameter `f(&sp8)` whose later words are read as `spC`/`sp10` leaves them
# undeclared. Fold such slots into an array based at the nearest lower declared slot.
SP_DECL = re.compile(r"^(?P<ind>[ \t]+)(?P<t>[su](?:8|16|32)|f32) (?P<v>sp(?P<off>[0-9A-F]+));\n", re.M)
SIZES = {"s8": 1, "u8": 1, "s16": 2, "u16": 2, "s32": 4, "u32": 4, "f32": 4}


def stack_arrays(body: str) -> str:
    """BODY with undeclared `spXX` slots folded into an array at a declared lower slot."""
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


def call_sites(text: str, fn: str):
    for m in re.finditer(rf"(?<![\w.>]){re.escape(fn)}\s*\(", text):
        try:
            args, end = split_args(text, m.end() - 1)
        except ValueError:
            continue
        yield m.start(), end, [] if args == [""] else args


RAW_PROTO = re.compile(r"^(?:extern\s+|static\s+)?[A-Za-z_][^;{}()\n]*?\b(\w+)\s*\(([^;{}()]*)\)\s*;", re.M)


def raw_protos(text: str) -> dict[str, list[str]]:
    """Parameter lists of the prototypes in TEXT whatever their types (typedef names and
    structs included); `f()` and `f(void)` give []. The last declaration of a name wins."""
    out = {}
    for m in RAW_PROTO.finditer(text):
        if m[1] not in NOT_FUNCS and m[2].strip():
            out[m[1]] = [] if m[2].strip() == "void" else [p.strip() for p in m[2].split(",")]
    return out


def fix_prototypes(body: str, name: str, asm: str, unit_text: str) -> str:
    """BODY with m2c's callee declarations replaced by known or cr1eq-implied ones and
    each call's argument list trimmed or zero-padded to the declared count."""
    m = re.search(rf"^\w[^\n;(]*?\b{re.escape(name)}\(", body, flags=re.M)
    if not m:
        return body
    head, code = body[:m.start()], body[m.start():]
    unit = {n: p[-1][1] for n, p in find_protos(unit_text).items()}
    repo, variadic, raw = repo_protos(), variadic_calls(asm), raw_protos(unit_text)
    lines, arity = [], {}
    for ln in head.splitlines():
        d = PROTO.match(ln) if ln.endswith(";") and not ln.startswith("extern") else None
        fn = d[2] if d else None
        if not fn or fn == name:
            lines.append(ln)
            continue
        ret = d[1].strip()
        if fn in unit:
            arity[fn] = unit[fn][1]
            continue  # the unit's declaration governs
        if fn in raw:  # declared in the unit with types we do not reduce: still fix the arity
            arity[fn] = [(re.match(r"(f32|f64)\b(?!\s*\*)", p) or [p])[0] for p in raw[fn]]
            continue
        known = repo.get(fn)
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
    for fn, params in arity.items():
        n, var = len([p for p in params if p != "..."]), "..." in params
        for start, end, args in reversed(list(call_sites(code, fn))):
            fixed = args[:n] if not var else args
            fixed = fixed + ["0"] * (n - len(fixed))
            # m2c passes a float constant's array by name where the value is meant.
            fixed = [f"{a}[0]" if a in arrays and i < n and params[i] in ("f32", "f64") else a
                     for i, a in enumerate(fixed)]
            if fixed != args:
                open_paren = code.index("(", start)
                code = code[:open_paren] + "(" + ", ".join(fixed) + ")" + code[end:]
    return "\n".join(lines) + ("\n" if lines else "") + code


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
        cmd = [sys.executable, str(ROOT / "tools" / "score_fn.py"), name, source, "--body", path]
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
    for context in [None] + ([ctx] if ctx else []):
        status, body = draft(name, context)
        if status != "ok":
            res = {"verdict": status.upper()}
        else:
            res, body = score_draft(name, source, stack_arrays(body))
            padded = pad_signature(body, name)
            if padded != body:
                pad_res, padded = score_draft(name, source, padded)
                if improves(pad_res, res):
                    res, body = {**pad_res, "padded": True}, padded
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
            res["context"] = context is not None
            res, body = try_variants(name, source, res, body)
        if best is None or better(res, best[0]):
            best = (res, body)
        if res["verdict"] == "MATCH":
            break
    res, body = best
    if res["verdict"] in ("MATCH", "MISMATCH"):
        (OUT / name / "sweep_draft.c").write_text(body)
    err = res.pop("error", "").strip()
    if res["verdict"] in ("COMPILE-ERROR", "ERROR") and err:
        res["error"] = err.splitlines()[-1][:200]
    return {**row, **res}


def try_variants(name: str, source: str, res: dict, body: str) -> tuple[dict, str]:
    """Score each VARIANTS rewrite of a draft at least 80% exact; keep the best."""
    if res["verdict"] != "MISMATCH" or (res.get("exact") or 0) < 0.8 * (res.get("target") or 1):
        return res, body
    seen = {body}
    for variant in VARIANTS:
        alt = variant(body)
        if alt in seen:
            continue
        seen.add(alt)
        alt_res, alt = score_draft(name, source, alt)
        if better(alt_res, res):
            res, body = {**alt_res, "context": res.get("context"), "rewrite": variant.__name__}, alt
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
            if (new := drop_decl(body, sym, name)):
                body = new
                break
        else:
            break
    else:
        res = score(name, source, body)
    return res, body


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
    args = ap.parse_args(argv)
    if not (draft_fn.m2c_dir() / "m2c.py").is_file():
        print("error: m2c not fetched; run bash tools/fetch_m2c.sh", file=sys.stderr)
        return 2
    todo = targets(args)
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
            if score(row["name"], row["source"], body, apply=True)["verdict"] == "MATCH":
                applied.append(row["name"])
                row["applied"] = True
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
    return 0


if __name__ == "__main__":
    sys.exit(main())
