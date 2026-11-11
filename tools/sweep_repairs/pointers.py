"""Pointer repairs: byte strides and a hoisted global base pointer."""

from __future__ import annotations

import re

from . import repair


@repair("scored", 20)
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


WIDE_PTR = re.compile(r"^(?P<ind>[ \t]+)(?P<t>s16|u16|s32|u32|f32|f64|s64|u64) \*(?P<v>\w+);$", re.M)
PTR_SIZES = {"s16": 2, "u16": 2, "s32": 4, "u32": 4, "f32": 4, "f64": 8, "s64": 8, "u64": 8}


@repair("scored", 10)
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
