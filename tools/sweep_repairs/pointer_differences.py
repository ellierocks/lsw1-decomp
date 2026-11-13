"""Recover integer byte differences before m2c's reciprocal multiplication."""
from __future__ import annotations

import re

from . import repair

POINTER = re.compile(r"^\s+(?:void|[su](?:8|16|32|64)|f(?:32|64))\s*\*\s*(\w+)\s*;", re.M)


@repair("final", 45)
def pointer_byte_differences(body: str, name="", asm="", unit="") -> str:
    """Cast a pointer-valued subtraction's left operand to the GC address width.

    m2c sometimes infers `pointer - integer_base` as a pointer, then multiplies
    it by a reciprocal to recover an element index. C rejects that multiply.
    Only repair a parenthesized subtraction immediately multiplied by a hex
    constant, with a declared local pointer on the left. Keep ordinary pointer
    arithmetic and valid pointer-pointer differences untouched.
    """
    pointers = set(POINTER.findall(body))
    pattern = re.compile(r"\((?P<left>\w+) - (?P<right>\(\*\(s32 \*\))")
    edits = []
    for match in pattern.finditer(body):
        if match["left"] not in pointers:
            continue
        depth = 1
        end = match.start() + 1
        while end < len(body) and depth:
            depth += (body[end] == "(") - (body[end] == ")")
            end += 1
        if depth or not re.match(r"\s*\*\s*0x[0-9A-Fa-f]+\b", body[end:]):
            continue
        edits.append(match.start("left"))
    for pos in reversed(edits):
        body = body[:pos] + "(u32) " + body[pos:]
    return body
