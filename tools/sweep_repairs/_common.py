"""Helpers shared by several repair families."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


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
