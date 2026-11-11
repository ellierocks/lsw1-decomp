"""Stack repairs: undeclared slots as arrays, volatile address-taken locals."""

from __future__ import annotations

import re

from . import repair
from ._common import SIZES, SP_DECL, call_args


@repair("variants", 40)
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
