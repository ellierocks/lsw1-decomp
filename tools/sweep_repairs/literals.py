"""Float literals for single `.rodata` float labels.

ProDG emits a function's float literals into a per-function `.rodata` pool, so retail
has labels such as `lbl_80190A2C` (0.5) that m2c drafts as `extern f32 lbl_X[];` and
`lbl_X[0]`. The extern and the literal load the same value, but GCC orders and keeps
literal constant-pool loads differently (the `lis` order, and whether a value stays in
a register across calls), so the label form can plateau a few instructions short.
"""
from __future__ import annotations

import re
import struct
from decimal import ROUND_DOWN, Decimal
from functools import lru_cache

from . import repair
from ._common import ROOT

ASM_DIR = ROOT / "build" / "GL5E4F" / "asm"
FLOAT_OBJ = re.compile(r"^# \.rodata:\S+ \| \S+ \| size: 0x4\n\.obj (\w+), \w+\n\t\.float (\S+)\n\.endobj \1$", re.M)
EXTERN = re.compile(r"^extern (?:const )?f32 (\w+)\[\];[ \t]*\n", re.M)


@lru_cache(maxsize=1)
def rodata_floats() -> dict[str, float]:
    """Every 4-byte `.rodata` object that holds exactly one `.float`, by label."""
    out: dict[str, float] = {}
    for path in ASM_DIR.rglob("*.s"):
        text = path.read_text(errors="replace")
        if ".float" not in text:
            continue
        for m in FLOAT_OBJ.finditer(text):
            try:
                out[m.group(1)] = float(m.group(2))
            except ValueError:
                pass
    return out


FLT_MAX = 3.4028234663852886e38


def f32_bits(value: float) -> bytes | None:
    """VALUE rounded to single precision, as C rounds a float literal; None past FLT_MAX.

    dtk prints FLT_MAX as 3.4028235e38, a double just above it, which C still rounds to
    FLT_MAX but struct.pack rejects.
    """
    if value != value:
        return None
    if FLT_MAX < abs(value) < 3.4028236e38:
        value = FLT_MAX if value > 0 else -FLT_MAX
    try:
        return struct.pack(">f", value)
    except (OverflowError, struct.error):
        return None


def c_literal(value: float) -> str | None:
    """Shortest C float literal that rounds to VALUE's single-precision bits."""
    bits = f32_bits(value)
    if bits is None:
        return None
    exact = struct.unpack(">f", bits)[0]
    for digits in range(1, 10):
        # Rounded, then truncated toward zero: near FLT_MAX only the truncated form stays
        # in range, so old GCC does not warn that the literal overflows a float.
        rounded = "%.*g" % (digits, exact)
        truncated = "%.*e" % (digits - 1, Decimal(exact).quantize(
            Decimal(1).scaleb(Decimal(exact).adjusted() - digits + 1), rounding=ROUND_DOWN))
        for text in (rounded, truncated):
            if abs(float(text)) <= FLT_MAX and f32_bits(float(text)) == bits:
                if not re.search(r"[.e]", text):
                    text += ".0"
                return text + "f"
    return None


def to_literal(body: str, label: str, literal: str) -> str:
    body = re.sub(rf"^extern (?:const )?f32 {label}\[\];[ \t]*\n", "", body, flags=re.M)
    return re.sub(rf"\b{label}\[0\]", literal, body)


@repair("final", 50)
def float_literals(body: str, name="", asm="", unit="", floats=None, limit: int = 6) -> list[str]:
    """All rodata float labels as literals, then each label alone (at most LIMIT forms).

    A label qualifies only when every use in BODY is `label[0]`: an address or an
    index other than 0 means a table, not a literal.
    """
    floats = rodata_floats() if floats is None else floats
    swaps = []
    for m in EXTERN.finditer(body):
        label = m.group(1)
        if label not in floats:
            continue
        uses = len(re.findall(rf"\b{label}\b", body)) - 1
        if uses == 0 or uses != len(re.findall(rf"\b{label}\[0\]", body)):
            continue
        literal = c_literal(floats[label])
        if literal:
            swaps.append((label, literal))
    if not swaps:
        return []
    every = body
    for label, literal in swaps:
        every = to_literal(every, label, literal)
    out = [every]
    if len(swaps) > 1:
        out += [to_literal(body, label, literal) for label, literal in swaps[:limit - 1]]
    return out
