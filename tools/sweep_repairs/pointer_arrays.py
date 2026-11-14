"""Undo double scaling of byte offsets on m2c pointer-array cursors."""
from __future__ import annotations

import re

from . import repair

POINTER_ARRAY = re.compile(r"\b(?:void|s8|u8|s16|u16|s32|u32|f32|f64)\s*\*\s*\*\s*(\w+)\b")


@repair("final", 46)
def pointer_array_bytes(body: str, name="", asm="", unit="") -> str:
    """Try four-byte cursor steps as single elements and byte offsets under explicit word loads.

    m2c's raw array cursors can retain PPC byte displacements after inferring a
    pointer-to-pointer type. Like byte_strides, this is a scored alternative:
    a genuine multi-element step is retained unless this draft scores better.
    Ordinary C indexing and pointer additions outside word loads are unchanged.
    """
    for pointer in sorted(set(POINTER_ARRAY.findall(body))):
        escaped = re.escape(pointer)
        step = re.compile(rf"\b{escaped}\s*([+-])=\s*(0x[0-9a-fA-F]+|\d+);")

        def element_step(match):
            count = int(match[2], 16 if match[2].lower().startswith("0x") else 10)
            return f"{pointer} {match[1]}= 1;" if count == 4 else match[0]

        body = step.sub(element_step, body)
        load = re.compile(rf"(\*\((?:s32|u32)\s*\*\)\(\s*){escaped}(\s*\+)")
        body = load.sub(lambda m: f"{m[1]}(u8 *){pointer}{m[2]}", body)
    return body
