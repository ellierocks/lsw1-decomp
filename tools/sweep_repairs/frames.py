"""Frame repairs: a target frame size the draft does not reach."""

from __future__ import annotations

import re

from . import repair
from ._common import SIZES, SP_DECL


EMPTY_FRAME = re.compile(r"\A\s*stwu r1, -(0x[0-9A-Fa-f]+)\(r1\)\s+addi r1, r1, \1\s+blr\s*\Z")
EMPTY_BODY = re.compile(r"(\)\s*\{)\s*(\})\s*\Z")


@repair("frame", 10)
def empty_frame(body: str, asm: str) -> str:
    """An empty function whose target still opens a stack frame: GCC keeps the frame for an unused
    volatile local, so give the empty body a volatile array sized to the frame (8-byte header)."""
    insns = "\n".join(m.group(1) for m in re.finditer(r"\*/\s*(.+)$", asm, re.M))
    m = EMPTY_FRAME.match(insns)
    if not m or not EMPTY_BODY.search(body):
        return body
    words = max(1, (int(m.group(1), 16) - 8) // 4)
    return EMPTY_BODY.sub(lambda b: f"{b.group(1)}\n    volatile s32 pad[{words}];\n{b.group(2)}\n", body)


FRAME = re.compile(r"\bstwu r1, -(0x[0-9A-Fa-f]+)\(r1\)")
SAVED = re.compile(r"\b(?:stw|stmw|stfd) (?:r(?:1[3-9]|2\d|3[01])|f(?:1[4-9]|2\d|3[01])), (0x[0-9A-Fa-f]+)\(r1\)")


@repair("frame", 20)
def frame_buffer(body: str, asm: str) -> str:
    """BODY with the topmost address-taken `spXX` scalar widened to a `char` array reaching
    the first callee-saved slot (or the frame top) of the target frame, rounded down to 8
    bytes: m2c types a stack buffer such as a `sprintf` target by its first word, so the
    draft's frame comes out far too small (LoadObjectFile, LoadObjectAnimFile). Unchanged
    when the target frame leaves no gap above the slot."""
    insns = "\n".join(m.group(1) for m in re.finditer(r"\*/\s*(.+)$", asm, re.M)) or asm
    frame = FRAME.search(insns)
    if not frame:
        return body
    saved = [int(m.group(1), 16) for m in SAVED.finditer(insns)]
    top = min(saved) if saved else int(frame.group(1), 16)
    taken = [m for m in SP_DECL.finditer(body)
             if f"&{m['v']}" in body and int(m["off"], 16) < top]
    if not taken:
        return body
    m = max(taken, key=lambda d: int(d["off"], 16))
    size = (top - int(m["off"], 16)) & ~7  # GCC rounds a stack array up to 8 bytes
    if size < 4 * SIZES[m["t"]] or size <= 8:
        return body
    body = body.replace(m[0], f"{m['ind']}char {m['v']}[{size:#x}];\n", 1)
    body = re.sub(rf"\(\w+ \*\)\s*&{m['v']}\b|&{m['v']}\b", m["v"], body)
    return re.sub(rf"(?<![\w&]){m['v']}\b(?!\[)(?=\s*(?:=[^=]|\+\+|--|[-+*/|&^]=))",
                  f"*(s32 *){m['v']}", body)
