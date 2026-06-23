#!/usr/bin/env python3
"""Plan or generate asm fallback slices around one recovered C function.

The project currently keeps broad .text fallback objects. A C function cannot
be linked in place until its containing fallback assembly is divided into the
range before the function, the C object, and the range after it. This tool is
intentionally non-destructive by default: it validates exact function
boundaries and emits a plan. Pass --write only after the candidate object has
been verified.

Usage:
    python3 tools/asm_function_slice.py NuVec4MtxTransformVU0
    python3 tools/asm_function_slice.py NuVec4MtxTransformVU0 --write
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
ASM_DIR = ROOT / "build/GL5E4F/asm"
OUT_ROOT = ROOT / "work/asm_slices"

SYM_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+\.text:0x(?P<addr>[0-9A-Fa-f]+);.*?size:0x(?P<size>[0-9A-Fa-f]+)"
)
HEADER_RE = re.compile(r"^# \.text:[^|]+\| 0x([0-9A-Fa-f]+) \| size: 0x([0-9A-Fa-f]+)")


def target(name: str) -> tuple[int, int]:
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        match = SYM_RE.match(line)
        if match and match.group("name") == name:
            return int(match.group("addr"), 16), int(match.group("size"), 16)
    raise ValueError(f"no .text symbol named {name}")


def locate_asm(addr: int, size: int) -> tuple[Path, list[str], int, int]:
    """Find a generated asm file and exact line range for a function."""
    for path in ASM_DIR.glob("*.s"):
        lines = path.read_text(errors="replace").splitlines(keepends=True)
        for start, line in enumerate(lines):
            match = HEADER_RE.match(line)
            if not match or int(match.group(1), 16) != addr:
                continue
            if int(match.group(2), 16) != size:
                raise ValueError(
                    f"{path}: symbol size 0x{size:X} disagrees with asm header "
                    f"0x{int(match.group(2), 16):X}"
                )
            end = start + 1
            while end < len(lines) and not (end > start + 1 and HEADER_RE.match(lines[end])):
                end += 1
            return path, lines, start, end
    raise ValueError(f"no generated asm function at 0x{addr:08X}")


def asm_preamble(source: Path, name: str) -> list[str]:
    return [
        '.include "macros.inc"\n',
        f'.file "{name}"\n',
        "\n",
        ".text\n",
        ".balign 4\n",
        "\n",
    ]


def strip_file_preamble(lines: list[str]) -> list[str]:
    """Drop only the original object preamble; preserve all code/comments."""
    for index, line in enumerate(lines):
        if HEADER_RE.match(line):
            return lines[index:]
    return []


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("symbol")
    parser.add_argument("--write", action="store_true", help="write prefix/suffix asm files")
    args = parser.parse_args()

    try:
        addr, size = target(args.symbol)
        source, lines, start, end = locate_asm(addr, size)
    except ValueError as error:
        sys.exit(f"error: {error}")

    prefix = strip_file_preamble(lines[:start])
    suffix = lines[end:]
    base = f"{addr:08X}_{args.symbol}"
    out_dir = OUT_ROOT / base

    print(f"Target: {args.symbol} @ 0x{addr:08X} size 0x{size:X}")
    print(f"Fallback source: {source.relative_to(ROOT)}")
    print(f"Before: {len(prefix)} asm lines")
    print(f"After:  {len(suffix)} asm lines")
    print("\nReplacement order:")
    print(f"  1. asm prefix: [fallback start, 0x{addr:08X})")
    print(f"  2. C object:   [0x{addr:08X}, 0x{addr + size:08X})")
    print(f"  3. asm suffix: [0x{addr + size:08X}, fallback end)")

    if not args.write:
        print("\nDry run only. Re-run with --write after the candidate is verified.")
        return

    out_dir.mkdir(parents=True, exist_ok=True)
    before_path = out_dir / "before.s"
    after_path = out_dir / "after.s"
    plan_path = out_dir / "plan.md"
    before_path.write_text("".join(asm_preamble(source, before_path.name) + prefix))
    after_path.write_text("".join(asm_preamble(source, after_path.name) + suffix))
    plan_path.write_text(
        f"# {args.symbol} fallback split\n\n"
        f"- Target: `0x{addr:08X}`–`0x{addr + size:08X}`\n"
        f"- Source fallback: `{source.relative_to(ROOT)}`\n"
        f"- Before slice: `{before_path.relative_to(ROOT)}`\n"
        f"- After slice: `{after_path.relative_to(ROOT)}`\n\n"
        "These slices are generated artifacts. Do not replace the original fallback "
        "object until the C candidate is a verified exact match.\n"
    )
    print(f"\nWrote: {out_dir.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
