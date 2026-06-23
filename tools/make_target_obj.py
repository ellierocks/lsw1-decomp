#!/usr/bin/env python3
"""Build the objdiff *target* object for a decompiled unit.

objdiff (and therefore the progress script) only measures a unit when a target
object exists at build/<version>/obj/<unit>.o. `dtk dol split` does not produce
these for this repo, so this tool synthesizes one from the DOL-accurate asm
dumps in build/<version>/asm/: it slices the unit's address range out of the
fallback dump, renames the local `fn_XXXXXXXX` labels to the real symbol names
from symbols.txt (so objdiff pairs them with our compiled functions), and
assembles the result into a relocatable ELF — mirroring the existing
obj/numath/nurand.o target.

Usage:
    python3 tools/make_target_obj.py numath/nuvec.c
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VERSION = "GL5E4F"
SYMBOLS = ROOT / f"config/{VERSION}/symbols.txt"
SPLITS = ROOT / f"config/{VERSION}/splits.txt"
ASM_DIR = ROOT / f"build/{VERSION}/asm"
OBJ_DIR = ROOT / f"build/{VERSION}/obj"
AS = ROOT / "build/binutils/powerpc-eabi-as"
DTK = ROOT / "build/tools/dtk_patched"

ADDR_LINE = re.compile(r"^\s*/\*\s*([0-9A-Fa-f]+)\s")
SYM_LINE = re.compile(r"^(\S+)\s*=\s*\.text:0x([0-9A-Fa-f]+);")


def unit_text_ranges(unit: str) -> list[tuple[int, int]]:
    """All .text ranges for a unit. Units can be non-contiguous in the DOL
    (a single source file's functions land in separate code regions), so a
    unit may declare more than one `.text start:.. end:..` line."""
    cur = None
    ranges: list[tuple[int, int]] = []
    for raw in SPLITS.read_text().splitlines():
        s = raw.strip()
        if not raw.startswith((" ", "\t")) and s.endswith(":"):
            cur = s[:-1]
            continue
        if cur == unit and s.startswith(".text"):
            m = re.search(r"start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)", s)
            if m:
                ranges.append((int(m.group(1), 16), int(m.group(2), 16)))
    if not ranges:
        sys.exit(f"no .text range for unit '{unit}' in splits.txt")
    return ranges


def addr_names() -> dict[int, str]:
    names: dict[int, str] = {}
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        m = SYM_LINE.match(line)
        if m:
            names[int(m.group(2), 16)] = m.group(1)
    return names


def in_ranges(addr: int, ranges: list[tuple[int, int]]) -> bool:
    return any(lo <= addr < hi for lo, hi in ranges)


def collect_fn_blocks(ranges: list[tuple[int, int]]) -> dict[int, list[str]]:
    """Return {addr: body lines} for every .fn block within any of the unit's
    ranges, across all dumps.

    The unit's ranges can straddle (or fall between) the broad fallback dumps, so
    scan them all rather than requiring a single covering file.
    """
    blocks: dict[int, list[str]] = {}
    for path in sorted(ASM_DIR.glob("*.s")):
        lines = path.read_text(errors="replace").splitlines()
        grab = False
        addr = None
        body: list[str] = []
        for line in lines:
            s = line.strip()
            if s.startswith(".fn "):
                fn = s.split()[1].rstrip(",")
                a = int(fn[3:], 16) if fn.startswith("fn_") else None
                if a is not None and in_ranges(a, ranges):
                    grab, addr, body = True, a, []
                continue
            if grab and s.startswith(".endfn"):
                blocks[addr] = body
                grab = False
                continue
            if grab:
                body.append(line)
    return blocks


def slice_unit(ranges: list[tuple[int, int]], names: dict[int, str], unit: str) -> tuple[str, int]:
    blocks = collect_fn_blocks(ranges)
    out: list[str] = [
        '.include "macros.inc"',
        f'.file "{Path(unit).name}"',
        "",
        ".text",
        ".balign 4",
        "",
    ]
    for addr in sorted(blocks):
        name = names.get(addr, f"fn_{addr:08X}")
        out.append(f".fn {name}, global")
        out.extend(blocks[addr])
        out.append(f".endfn {name}")
        out.append("")
    return "\n".join(out) + "\n", len(blocks)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("unit", help="unit path, e.g. numath/nuvec.c")
    args = ap.parse_args()

    ranges = unit_text_ranges(args.unit)
    asm, nfns = slice_unit(ranges, addr_names(), args.unit)
    span = " ".join(f"0x{lo:08X}..0x{hi:08X}" for lo, hi in ranges)
    if nfns == 0:
        sys.exit(f"no functions found in {span} for {args.unit}")

    out_obj = OBJ_DIR / Path(args.unit).with_suffix(".o")
    out_obj.parent.mkdir(parents=True, exist_ok=True)
    tmp_s = out_obj.with_suffix(".target.s")
    tmp_s.write_text(asm)

    inc = [f"-I{ROOT}/include", f"-I{ROOT}/build/{VERSION}/include"]
    r = subprocess.run([str(AS), "-mgekko", *inc, "-o", str(out_obj), str(tmp_s)],
                       capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit(f"assemble failed:\n{r.stdout}{r.stderr}")
    subprocess.run([str(DTK), "elf", "fixup", str(out_obj), str(out_obj)],
                   capture_output=True, text=True)
    tmp_s.unlink(missing_ok=True)
    print(f"{args.unit}: wrote {out_obj.relative_to(ROOT)} "
          f"({nfns} functions, {span})")


if __name__ == "__main__":
    main()
