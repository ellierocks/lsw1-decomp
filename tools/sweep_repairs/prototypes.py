"""Callee prototype repairs: known, cr1eq-implied and float-position prototypes."""

from __future__ import annotations

import itertools
import re
import threading

import mac_protos
from m2c_helpers import call_sites, repair_indirect_prototypes

from . import repair
from ._common import ROOT
from .signature import add_params, passthrough_regs


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
    if "[" in decl:
        return "void *"
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


def is_float_param(param: str) -> bool:
    """Only scalar floating parameters use FPRs; float pointers use GPRs."""
    return "*" not in param and "[" not in param and param.split()[0] in FLOATS


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
    ints = [a for a, k in zip(args, kinds) if not is_float_param(k)]
    flts = [a for a, k in zip(args, kinds) if is_float_param(k)]
    out, gpr, stack, end = [], 0, [], before.rfind("\n") + 1
    for p in params:
        if is_float_param(p):
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
            for i, a in enumerate(fixed):
                if i >= n or params[i] not in ("f32", "f64"):
                    continue
                if a in arrays:
                    fixed[i] = f"{a}[0]"
                    continue
                # Guessed float-pointer parameters also leave casts around known
                # floating constants. Only unwrap these two literal-array forms.
                cast = re.fullmatch(r"\(\(f(?:32|64)\s*\*?\)\((\w+)(\[0\])?\)\)", a)
                if cast and cast[1] in arrays:
                    fixed[i] = f"{cast[1]}[0]"
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
    ints = [a for a, k in zip(args, kinds) if not is_float_param(k)]
    flts = [a for a, k in zip(args, kinds) if is_float_param(k)]
    n_r = min(8, sum(not is_float_param(p) for p in params))
    n_f = min(8, sum(is_float_param(p) for p in params))
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
    cls_params = {"r": [p for p in params if not is_float_param(p)][:8],
                  "f": [p for p in params if is_float_param(p)][:8]}
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
        cls = "f" if is_float_param(p) else "r"
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


@repair("variants", 30)
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
        isf = [is_float_param(p) for p in params]
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
