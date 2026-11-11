"""Bounded operand-order alternatives for complete, bare additions."""
from __future__ import annotations

import re
from . import repair

ADD = re.compile(r'(?<=[=(])(?P<space>[ \t]*)(?P<a>[A-Za-z_]\w*) \+ (?P<b>[A-Za-z_]\w*)(?=[);])')
LOCAL = re.compile(r'(?:var_|temp_|arg\d|pad\d)')


@repair("final", 40)
def commuted_adds(body: str, name="", asm="", unit="", limit: int = 8) -> list[str]:
    """Swap one complete a+b with exactly one m2c local/argument, at most LIMIT forms.

    Restrict to a parenthesized expression or a complete assignment RHS. Matching a
    pair inside a+b*c, -a+b or a+b->field would change the expression's meaning.
    """
    out = []
    for m in ADD.finditer(body):
        if bool(LOCAL.match(m['a'])) == bool(LOCAL.match(m['b'])):
            continue
        out.append(body[:m.start()] + m['space'] + m['b'] + ' + ' + m['a'] + body[m.end():])
        if len(out) >= limit:
            break
    return out
