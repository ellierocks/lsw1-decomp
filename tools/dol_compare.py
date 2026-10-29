#!/usr/bin/env python3
"""Compare a relinked DOL with the retail DOL, byte for byte.

This is the end-to-end check for promoted (linked) source units: objdiff and
mnemonic comparison cannot see the final relocation values, but the linked
image can. Every byte of a promoted unit's section ranges must equal retail.

Usage:
  python3 tools/dol_compare.py build/GL5E4F/main.dol orig/GL5E4F/sys/main.dol --linked-units
  python3 tools/dol_compare.py a.dol b.dol --range 0x8000C0C0:0x8000C1B8

Exit status is 1 if a requested range (or a linked unit's range) differs, or
if the two images have different layouts. Differences outside the requested
ranges are summarised, never ignored silently.
"""

from __future__ import annotations

import argparse
import json
import re
import struct
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Tuple

HEADER_SIZE = 0x100
TEXT_SECTIONS = 7
SECTION_SLOTS = 18
SDA_BASE_REG = 13
SDA2_BASE_REG = 2
# D-form opcodes whose rA field is a base register (addi and integer/float loads/stores).
D_FORM_BASE_OPCODES = frozenset([14, *range(32, 56)])


@dataclass(frozen=True)
class Section:
    index: int
    offset: int
    address: int
    size: int

    @property
    def end(self) -> int:
        return self.address + self.size

    @property
    def is_text(self) -> bool:
        return self.index < TEXT_SECTIONS


@dataclass(frozen=True)
class Dol:
    data: bytes
    sections: Tuple[Section, ...]

    def read(self, address: int, size: int) -> bytes:
        for sec in self.sections:
            if sec.address <= address and address + size <= sec.end:
                start = sec.offset + (address - sec.address)
                return self.data[start:start + size]
        raise ValueError(f"0x{address:08X}+0x{size:X} is not inside one DOL section")

    def section_at(self, address: int) -> Optional[Section]:
        for sec in self.sections:
            if sec.address <= address < sec.end:
                return sec
        return None


def parse_dol(data: bytes) -> Dol:
    if len(data) < HEADER_SIZE:
        raise ValueError("file is smaller than a DOL header")
    fmt = f">{SECTION_SLOTS}I"
    step = SECTION_SLOTS * 4
    offsets = struct.unpack_from(fmt, data, 0)
    addresses = struct.unpack_from(fmt, data, step)
    sizes = struct.unpack_from(fmt, data, step * 2)
    sections = []
    for i in range(SECTION_SLOTS):
        if sizes[i] == 0:
            continue
        if offsets[i] + sizes[i] > len(data):
            raise ValueError(f"section {i} extends past end of file")
        sections.append(Section(i, offsets[i], addresses[i], sizes[i]))
    return Dol(data, tuple(sections))


def layout(dol: Dol) -> List[Tuple[int, int, int]]:
    return [(s.index, s.address, s.size) for s in dol.sections]


def classify_word(built: int, ref: int) -> str:
    """Name the cause when two instruction words differ only in the small-data base."""
    if (built >> 26) == (ref >> 26) and (ref >> 26) in D_FORM_BASE_OPCODES:
        same_dest = ((built >> 21) & 0x1F) == ((ref >> 21) & 0x1F)
        if same_dest and ((ref >> 16) & 0x1F) == SDA_BASE_REG and ((built >> 16) & 0x1F) == SDA2_BASE_REG:
            return "sdata2-via-r2 (retail uses r13)"
    return "other"


def diff_bytes(a: Dol, b: Dol) -> List[int]:
    """Addresses of every differing byte, assuming identical layouts."""
    out: List[int] = []
    for sec in a.sections:
        left = a.data[sec.offset:sec.offset + sec.size]
        right = b.data[sec.offset:sec.offset + sec.size]
        if left == right:
            continue
        out.extend(sec.address + i for i in range(sec.size) if left[i] != right[i])
    return out


def classify_diffs(a: Dol, b: Dol, diffs: Iterable[int]) -> Counter:
    """Count differing words by cause (text) or section (data)."""
    counts: Counter = Counter()
    seen = set()
    for addr in diffs:
        sec = a.section_at(addr)
        assert sec is not None
        if not sec.is_text:
            counts[f"data section {sec.index}"] += 1
            continue
        word_addr = addr & ~3
        if word_addr in seen:
            continue
        seen.add(word_addr)
        if word_addr < sec.address:
            counts[f"text section {sec.index} (unaligned)"] += 1
            continue
        built = struct.unpack(">I", a.read(word_addr, 4))[0]
        ref = struct.unpack(">I", b.read(word_addr, 4))[0]
        cause = classify_word(built, ref)
        counts[cause if cause != "other" else f"other text word in section {sec.index}"] += 1
    return counts


def parse_range(text: str) -> Tuple[int, int]:
    start, _, end = text.partition(":")
    if not end:
        raise argparse.ArgumentTypeError("range must look like START:END")
    lo, hi = int(start, 0), int(end, 0)
    if hi <= lo:
        raise argparse.ArgumentTypeError("range end must be above its start")
    return lo, hi


def parse_splits(text: str) -> Dict[str, List[Tuple[str, int, int]]]:
    """Map unit path -> [(section, start, end)] from a dtk splits.txt."""
    units: Dict[str, List[Tuple[str, int, int]]] = {}
    current: Optional[str] = None
    pattern = re.compile(r"^\s+(\.\w+)\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)")
    for line in text.splitlines():
        if line and not line[0].isspace() and line.rstrip().endswith(":") and line != "Sections:":
            current = line.rstrip()[:-1]
            units[current] = []
            continue
        match = pattern.match(line)
        if match and current is not None:
            units[current].append((match.group(1), int(match.group(2), 16), int(match.group(3), 16)))
    return units


def linked_unit_ranges(objdiff_json: Path, splits_txt: Path) -> Dict[str, List[Tuple[int, int]]]:
    """Ranges of units that objdiff.json marks complete, i.e. built from source and linked."""
    config = json.loads(objdiff_json.read_text(encoding="utf-8"))
    splits = parse_splits(splits_txt.read_text(encoding="utf-8"))
    ranges: Dict[str, List[Tuple[int, int]]] = {}
    for unit in config.get("units", []):
        meta = unit.get("metadata", {})
        source = meta.get("source_path")
        if not meta.get("complete") or not source:
            continue
        key = source[4:] if source.startswith("src/") else source
        if key not in splits:
            raise SystemExit(f"{source}: linked unit has no entry in {splits_txt}")
        ranges[key] = [(start, end) for _, start, end in splits[key]]
    return ranges


def check_ranges(a: Dol, b: Dol, diffs: List[int], ranges: List[Tuple[int, int]]) -> List[int]:
    bad = []
    for lo, hi in ranges:
        bad.extend(d for d in diffs if lo <= d < hi)
    return bad


def main(argv: Optional[List[str]] = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("built", type=Path)
    ap.add_argument("reference", type=Path)
    ap.add_argument("--range", dest="ranges", action="append", default=[], type=parse_range,
                    metavar="START:END", help="address range that must be byte-identical (repeatable)")
    ap.add_argument("--linked-units", action="store_true",
                    help="require every unit marked complete in objdiff.json to be byte-identical")
    ap.add_argument("--objdiff-json", type=Path, default=Path("objdiff.json"))
    ap.add_argument("--splits", type=Path, default=Path("config/GL5E4F/splits.txt"))
    args = ap.parse_args(argv)

    built = parse_dol(args.built.read_bytes())
    ref = parse_dol(args.reference.read_bytes())
    if layout(built) != layout(ref):
        print("FAIL: section layout differs from reference", file=sys.stderr)
        print(f"  built:     {[(i, hex(a), hex(s)) for i, a, s in layout(built)]}", file=sys.stderr)
        print(f"  reference: {[(i, hex(a), hex(s)) for i, a, s in layout(ref)]}", file=sys.stderr)
        return 1

    diffs = diff_bytes(built, ref)
    failures = 0
    if built.data[:HEADER_SIZE] != ref.data[:HEADER_SIZE]:
        failures += 1
        print("FAIL: DOL header (bss range, entry point or offsets) differs from reference")

    named: Dict[str, List[Tuple[int, int]]] = {}
    if args.linked_units:
        named.update(linked_unit_ranges(args.objdiff_json, args.splits))
    if args.ranges:
        named["(--range)"] = list(args.ranges)

    for name, ranges in sorted(named.items()):
        bad = check_ranges(built, ref, diffs, ranges)
        size = sum(hi - lo for lo, hi in ranges)
        if bad:
            failures += 1
            print(f"FAIL {name}: {len(bad)} of {size} bytes differ, first at 0x{bad[0]:08X}")
        else:
            spans = ", ".join(f"0x{lo:08X}-0x{hi:08X}" for lo, hi in ranges)
            print(f"ok   {name}: {size} bytes identical to reference ({spans})")

    covered = sum(1 for d in diffs if any(lo <= d < hi for rs in named.values() for lo, hi in rs))
    print(f"whole image: {len(diffs)} differing bytes ({covered} inside checked ranges)")
    for cause, count in classify_diffs(built, ref, diffs).most_common():
        print(f"  {count:6d}  {cause}")
    if not named:
        print("no ranges requested; reporting only")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
