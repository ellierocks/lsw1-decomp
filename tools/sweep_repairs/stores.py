"""Store repairs: conditional stores in both arms, const .rodata, store order."""

from __future__ import annotations

import re

from . import repair
from ._common import ROOT


# m2c phrases a conditional store as `v = A; if (c) { v = B; } g = v;`. ProDG then hoists
# `v = A` above the call in c into a callee-saved register; the retail code usually
# comes from storing in both arms, so a second variant stores B or A directly.
PRESET_STORE = re.compile(r"(?P<ind>[ \t]*)(?P<v>var_r\w+) = (?P<a>[^;\n]+);\n(?P=ind)if \((?P<c>.*)\) \{\n"
                          r"[ \t]*(?P=v) = (?P<b>[^;\n]+);\n(?P=ind)\}\n(?P=ind)(?P<g>[\w\[\]]+) = (?P=v);\n")


@repair("variant", 10)
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


@repair("variant", 20)
def const_rodata(body: str) -> str:
    """BODY with its .rodata externs declared const. ProDG keeps a load from a non-const
    float array inside a loop that stores floats, but hoists it from a const one."""
    rodata = rodata_symbols()
    return re.sub(r"^extern (?!const )(\w[\w ]*?)\b(\w+)\[\];",
                  lambda m: f"extern const {m[1]}{m[2]}[];" if m[2] in rodata else m[0], body, flags=re.M)


@repair("variant", 30)
def both_rewrites(body: str) -> str:
    return const_rodata(store_in_arms(body))


MEM_STORE = re.compile(r"^(?P<ind>[ \t]+)(?P<lhs>\(*\*\(.+?\)\)*|\w+(?:\[[^\]]*\])?(?:(?:->|\.)\w+)+) = (?P<rhs>[^;]+);$")


FIELD_LHS = re.compile(r"^\(\*\([^()]+\)\(\(s8 \*\)\((?P<base>\w+)\) \+ \((?P<off>-?(?:0x[0-9A-Fa-f]+|\d+))\)\)\)$")


@repair("variant", 60)
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


@repair("variants", 10)
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
