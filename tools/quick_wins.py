#!/usr/bin/env python3
"""Print prioritized quick-win targets for symbol recovery.

Shows HIGH-confidence islands with unnamed functions, grouped by
difficulty (smallest unnamed functions first). Output is designed
to feed directly into identify → apply cycles.

Usage:
    python tools/quick_wins.py               # all HIGH islands with unknowns
    python tools/quick_wins.py --max-size 0x80   # only tiny functions
    python tools/quick_wins.py --max-unnamed 2   # only 1-2 unknown fns per island
"""

from __future__ import annotations

import argparse
import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
ISLANDS_TSV = ROOT / "docs" / "symbol_donors" / "sdk_islands.tsv"
ASM_FILE = ROOT / "build" / "GL5E4F" / "asm" / "auto_01_8001752C_text.s"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+\.text:0x(?P<addr>[0-9A-Fa-f]+);\s*//\s*(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")
# ASM index line for a function
ASM_LINE_RE = re.compile(r"^# \.text:[\w]+ \| 0x([0-9A-Fa-f]+) \| size: 0x([0-9A-Fa-f]+)")


def parse_symbols() -> dict[int, tuple[str, int]]:
    """addr → (name, size)"""
    out: dict[int, tuple[str, int]] = {}
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        m = SYMBOL_RE.match(line)
        if not m or "type:function" not in m.group("meta"):
            continue
        sm = SIZE_RE.search(m.group("meta"))
        size = int(sm.group(1), 16) if sm else 0
        out[int(m.group("addr"), 16)] = (m.group("name"), size)
    return out


def build_asm_index() -> dict[int, int]:
    """addr → line_number (1-based) in the ASM file."""
    idx: dict[int, int] = {}
    if not ASM_FILE.exists():
        return idx
    with ASM_FILE.open(errors="replace") as fh:
        for lineno, line in enumerate(fh, 1):
            m = ASM_LINE_RE.match(line)
            if m:
                idx[int(m.group(1), 16)] = lineno
    return idx


def load_islands() -> list[dict]:
    rows = []
    with ISLANDS_TSV.open(newline="", errors="replace") as fh:
        reader = csv.DictReader(fh, delimiter="\t")
        for row in reader:
            rows.append(row)
    return rows


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--max-size", type=lambda x: int(x, 0), default=None,
                    help="Only show unnamed functions at most this many bytes")
    ap.add_argument("--max-unnamed", type=int, default=None,
                    help="Only show islands with at most this many unnamed functions")
    ap.add_argument("--priority", default="HIGH",
                    help="Island priority filter (default: HIGH)")
    ap.add_argument("--family", default=None,
                    help="Filter by family (e.g. sdk/GX, sdk/OS)")
    args = ap.parse_args()

    syms = parse_symbols()
    asm_idx = build_asm_index()
    islands = load_islands()

    # Deduplicate: skip the large summary row (first per family group — starts == SDK_START)
    # We identify it as the one with the most functions (> 50) to avoid skipping sub-islands.
    results = []
    for row in islands:
        if row.get("priority") != args.priority:
            continue
        if args.family and row.get("family") != args.family:
            continue
        try:
            start = int(row["start"], 16)
            end = int(row["end"], 16)
            unnamed_bytes = int(row["unnamed_bytes"], 16)
            unnamed_count = int(row["unnamed"])
            total_count = int(row["functions"])
        except (ValueError, KeyError):
            continue

        if unnamed_count == 0:
            continue
        # skip the giant summary rows (> 50 functions = whole-range summary)
        if total_count > 50:
            continue
        if args.max_unnamed is not None and unnamed_count > args.max_unnamed:
            continue

        # Find unnamed functions in this range
        unknowns = [
            (addr, name, size)
            for addr, (name, size) in sorted(syms.items())
            if start <= addr < end and name.startswith("fn_")
        ]

        if args.max_size is not None:
            unknowns = [(a, n, s) for a, n, s in unknowns if s <= args.max_size]
        if not unknowns:
            continue

        results.append({
            "start": start,
            "end": end,
            "note": row.get("note", ""),
            "unnamed_count": unnamed_count,
            "unnamed_bytes": unnamed_bytes,
            "unknowns": unknowns,
        })

    # Sort: islands with fewer unknowns and smaller total unnamed bytes first
    results.sort(key=lambda r: (r["unnamed_count"], r["unnamed_bytes"]))

    if not results:
        print("No matching quick-win targets found.")
        return

    print(f"{'='*70}")
    print(f"Quick-win targets ({args.priority} priority, "
          f"{len(results)} islands with unnamed functions)")
    print(f"{'='*70}\n")

    for r in results:
        print(f"[{r['note']}]")
        print(f"  Range: 0x{r['start']:08X} – 0x{r['end']:08X}")
        for addr, name, size in r["unknowns"]:
            line = asm_idx.get(addr)
            line_str = f"line {line}" if line else "line ?"
            print(f"  {name}  0x{size:X} bytes  @ {line_str}")
        print()


if __name__ == "__main__":
    main()
