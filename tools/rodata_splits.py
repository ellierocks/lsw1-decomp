#!/usr/bin/env python3
"""Propose `.rodata` ranges for game/Nu2 units from the literals their code references.

Each TU's `.rodata` block (literals, strings, its `__FILE__` path) is laid out in the same
order as the TUs' `.text`. This tool reads the relocations dtk recovered in every built
target object (`build/GL5E4F/obj/**.o`), maps each `.rela.text` target to its `.rodata`
symbol in `symbols.txt`, and takes the span from the first referenced symbol to the end of
the last one as the unit's range.

A range is proposed only when it is evidence-only and unambiguous:

- the unit has no `.rodata` split yet and is a game/Nu2 unit (below the first SDK split);
- its span does not overlap the span of any other unit. Overlapping spans leave ownership
  unresolved: they can indicate one TU cut into several units, but are not by themselves
  evidence to merge the units;
- by default the unit is not `Matching` in `configure.py`. A promoted unit would have to
  supply the range from its compiled object, which is a separate proof (`--include-matching`).

Unreferenced data between two spans (alignment padding, data only reached from `.data`)
stays unsplit. Ranges never move a `.text` boundary. Run `bash build.sh` first; afterwards
rebuild and check that the DOL is byte-identical.

    python3 tools/rodata_splits.py                # report
    python3 tools/rodata_splits.py --write        # add the ranges to splits.txt
"""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPLITS = ROOT / "config/GL5E4F/splits.txt"
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
CONFIGURE = ROOT / "configure.py"
OBJ = ROOT / "build/GL5E4F/obj"
READELF = ROOT / "build/binutils/powerpc-eabi-readelf"

SYM_RE = re.compile(r"^(\S+) = \.rodata:0x([0-9A-Fa-f]+);")
SIZE_RE = re.compile(r"size:0x([0-9A-Fa-f]+)")
SEC_RE = re.compile(r"^\s+(\.\w+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)")


@dataclass
class Unit:
    name: str
    sections: dict[str, tuple[int, int]] = field(default_factory=dict)
    refs: set[str] = field(default_factory=set)
    span: tuple[int, int] | None = None
    reason: str = ""


def rodata_symbols(text: str) -> dict[str, tuple[int, int]]:
    """name -> (start, end) for `.rodata` symbols; a symbol without a size ends at its start + 1."""
    out = {}
    for line in text.splitlines():
        m = SYM_RE.match(line)
        if m:
            size = SIZE_RE.search(line)
            start = int(m[2], 16)
            out[m[1]] = (start, start + (int(size[1], 16) if size else 1))
    return out


def parse_splits(text: str) -> list[Unit]:
    units: list[Unit] = []
    for line in text.splitlines():
        if line and not line[0].isspace() and line.endswith(":") and line != "Sections:":
            units.append(Unit(line[:-1]))
            continue
        m = SEC_RE.match(line)
        if m and units:
            units[-1].sections[m[1]] = (int(m[2], 16), int(m[3], 16))
    return units


def matching_units(text: str) -> set[str]:
    return set(re.findall(r'Object\(\s*Matching\s*,\s*"([^"]+)"', text))


def text_relocation_targets(readelf_output: str) -> set[str]:
    """Symbol names targeted from `.rela.text` in `readelf -rW` output."""
    names, section = set(), None
    for line in readelf_output.splitlines():
        if line.startswith("Relocation section"):
            section = line.split("'")[1]
            continue
        parts = line.split()
        if section == ".rela.text" and len(parts) >= 5 and parts[0][:1].isdigit():
            names.add(parts[4])
    return names


def propose(units: list[Unit], symbols: dict[str, tuple[int, int]], matching: set[str],
            include_matching: bool = False) -> list[Unit]:
    """Fill `span` for units with an unambiguous range; set `reason` on the rest."""
    game = [u for u in units if ".text" in u.sections and not u.name.startswith("dolphin/")]
    sdk = [u.sections[".text"][0] for u in units if ".text" in u.sections and u.name.startswith("dolphin/")]
    if sdk:
        game = [u for u in game if u.sections[".text"][0] < min(sdk)]
    game.sort(key=lambda u: u.sections[".text"][0])
    spans = {}
    for u in game:
        hits = [symbols[n] for n in u.refs if n in symbols]
        if hits:
            spans[u.name] = (min(a for a, _ in hits), max(b for _, b in hits))
    overlapping = set()
    names = [u.name for u in game if u.name in spans]
    for i, a in enumerate(names):
        for b in names[i + 1:]:
            (a0, a1), (b0, b1) = spans[a], spans[b]
            if a0 < b1 and b0 < a1:
                overlapping.update((a, b))
    previous_end = 0
    for u in game:
        span = spans.get(u.name)
        if ".rodata" in u.sections:
            u.reason = "has .rodata"
        elif span is None:
            u.reason = "no .rodata references"
        elif u.name in overlapping:
            u.reason = "overlaps another unit (ownership unresolved)"
        elif span[0] < previous_end:
            u.reason = "out of link order"
        elif u.name.rsplit(".", 1)[0] in matching or u.name in matching:
            u.reason = "Matching unit (use --include-matching)" if not include_matching else ""
            u.span = span if include_matching else None
        else:
            u.span = span
        if span is not None:
            previous_end = max(previous_end, span[1])
    return game


def with_ranges(splits_text: str, ranges: dict[str, tuple[int, int]]) -> str:
    """Insert a `.rodata` line after each named unit's `.text` line."""
    out, unit = [], None
    for line in splits_text.splitlines(keepends=True):
        if line and not line[0].isspace() and line.rstrip("\n").endswith(":"):
            unit = line.rstrip("\n")[:-1]
        out.append(line)
        if unit in ranges and re.match(r"^\s+\.text\s", line):
            a, b = ranges[unit]
            out.append(f"\t.rodata     start:0x{a:08X} end:0x{b:08X}\n")
    return "".join(out)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--write", action="store_true", help="add the proposed ranges to splits.txt")
    ap.add_argument("--include-matching", action="store_true", help="also propose ranges for Matching units")
    args = ap.parse_args(argv)
    if not READELF.is_file() or not OBJ.is_dir():
        print("error: run tools/bootstrap.sh and bash build.sh first", file=sys.stderr)
        return 2
    splits_text = SPLITS.read_text()
    units = parse_splits(splits_text)
    symbols = rodata_symbols(SYMBOLS.read_text())
    for u in units:
        obj = OBJ / (u.name.rsplit(".", 1)[0] + ".o")
        if ".text" in u.sections and obj.is_file():
            out = subprocess.run([str(READELF), "-rW", str(obj)], capture_output=True, text=True).stdout
            u.refs = text_relocation_targets(out)
    game = propose(units, symbols, matching_units(CONFIGURE.read_text()), args.include_matching)
    ranges = {u.name: u.span for u in game if u.span}
    for u in game:
        span = f"0x{u.span[0]:08X}..0x{u.span[1]:08X}" if u.span else ""
        print(f"{u.name:44} {span:24} {u.reason}")
    print(f"\n{len(ranges)} ranges, {sum(b - a for a, b in ranges.values()):#x} bytes")
    if args.write and ranges:
        SPLITS.write_text(with_ranges(splits_text, ranges))
        print(f"wrote {SPLITS.relative_to(ROOT)}; rebuild and check the DOL is byte-identical")
    return 0


if __name__ == "__main__":
    sys.exit(main())
