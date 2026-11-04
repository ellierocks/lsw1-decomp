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
import mac_protos  # noqa: E402
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
    m2c_in.write_text(prepare_asm(asm.read_text()))
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


FLOAT_GE = re.compile(r"\(\((?P<a>[^()]+) == (?P<b>[^()]+)\) \|\| \((?P=a) (?P<op>[<>]) (?P=b)\)\)(?: != 0)?")


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


FN_ALIAS = re.compile(r"^#define (?P<fn>\w+) \(\((?P<ret>[^()]*?)\s*\(\*\)\([^()]*\)\)(?P=fn)\)$")


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
    lines, arity = [], {}
    for ln in head.splitlines():
        a = FN_ALIAS.match(ln)
        if a and a["fn"] in unit:
            # `alias_decl` kept m2c's guessed parameters for a callee the unit defines; the
            # return type stays m2c's (it decides the conversion), the parameters are the unit's.
            params = unit[a["fn"]][1]
            arity[a["fn"]] = params
            lines.append(f"#define {a['fn']} (({a['ret']}(*)({', '.join(params) or 'void'})){a['fn']})")
            continue
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
    return repair_indirect_prototypes("\n".join(lines) + ("\n" if lines else "") + code, asm)


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
    for context in [None] + ([ctx] if ctx else []):
        status, body = draft(name, context)
        if status != "ok":
            res = {"verdict": status.upper()}
        else:
            res, body = score_draft(name, source, stack_arrays(body))
            for repair in (byte_strides, base_pointer):  # scored repairs: kept only when better
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
            if (new := alias_decl(body, sym, name) or drop_decl(body, sym, name)):
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
        todo = [t for t in todo if t["name"] in names]
        previous = [r for r in previous if r["name"] not in {t["name"] for t in todo}
                    and not r.get("applied")]
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
        print(f"resweep: {len(rows)} targets; {len(previous)} rows kept from the last results.json")
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
