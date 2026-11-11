"""Restore the call convention hidden by unit prototypes and m2c alias macros."""
from __future__ import annotations

import re
from . import repair
from .prototypes import FN_ALIAS, PROTO


@repair("final", 30)
def unprototyped_calls(body: str, name: str, asm: str, unit: str) -> str:
    """Call through an unprototyped pointer where retail sets CR6 before bl."""
    normalized = re.sub(r"\bcr(?:xor|eqv) (?:6, 6, 6|cr1eq, cr1eq, cr1eq)",
                        "crclr cr1eq", asm)
    normalized = re.sub(r"\b(crclr|crset) 6\b", r"\1 cr1eq", normalized)
    callees = set(re.findall(r"\bcr(?:clr|set) cr1eq\s*\n(?:.*\n){0,2}?.*\tbl (\w+)", normalized))
    if not callees:
        return body
    aliases = {m['fn']: m for line in body.splitlines() if (m := FN_ALIAS.match(line))}
    returns = {m[2]: m[1].strip() for text in (unit, body) for m in PROTO.finditer(text)}
    for fn in sorted(callees):
        if fn in aliases:
            m = aliases[fn]
            body = body.replace(m[0], f"#define {fn} (({m['ret'].strip()} (*)()){fn})")
        elif fn in returns:
            # Parenthesized function name prevents function-like macros expanding.
            pattern = rf'(?<![\w]){re.escape(fn)}\s*\('
            lines = body.splitlines(keepends=True)
            body = ''.join(re.sub(pattern, lambda m: f"(({returns[fn]} (*)())({fn}))(", ln)
                           if ln.startswith((' ', '\t')) else ln for ln in lines)
    return body

