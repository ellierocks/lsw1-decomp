#!/usr/bin/env python3
"""Generate the donor-port work queue: LSW1 named functions that have a
same-name body in the CrashWOC decomp, ranked easiest-first.

Binary symbol mining is tapped out; the remaining progress lever is porting C.
This builds the brute-force backlog for that: every LSW1 function whose name is
already known AND has a donor definition we can translate. For each it records
size, whether it's paired-single SIMD (much harder to match from C), and the
donor file:line so the work can be picked up directly (by a human or an agent).

Ranking favours small, scalar functions with a single unambiguous donor
definition — the fastest matches. Paired-single functions are flagged and sorted
last (they need ps_* intrinsics, not plain scalar C).

Usage:
    python3 tools/port_queue.py                  # full ranked table to stdout
    python3 tools/port_queue.py --tsv queue.tsv  # also write a TSV
    python3 tools/port_queue.py --max-size 0x80  # only small functions
    python3 tools/port_queue.py --scalar-only    # hide paired-single
    python3 tools/port_queue.py --prefix Nu      # only names starting with Nu
"""
from __future__ import annotations

import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VERSION = "GL5E4F"
SYMBOLS = ROOT / f"config/{VERSION}/symbols.txt"
ASM_DIR = ROOT / f"build/{VERSION}/asm"
DONOR = Path("/tmp/crashwoc-decomp/src")
SPLITS = ROOT / f"config/{VERSION}/splits.txt"

SYM_LINE = re.compile(r"^(\S+)\s*=\s*\.text:0x([0-9A-Fa-f]+);.*?size:0x([0-9A-Fa-f]+)")
# a C function definition: optional return type/qualifiers, name, '(' ... at col 0
DEF_LINE = re.compile(r"^[A-Za-z_][\w \*]*\b([A-Za-z_]\w*)\s*\(")


def named_text_symbols() -> dict[str, tuple[int, int]]:
    """name -> (addr, size) for LSW1 .text symbols that aren't fn_ stubs."""
    out: dict[str, tuple[int, int]] = {}
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        m = SYM_LINE.match(line)
        if m and not m.group(1).startswith("fn_"):
            out[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return out


def already_decompiled() -> set[str]:
    """Names whose addresses fall inside a split unit that has C source."""
    units = []
    cur = None
    src_dir = ROOT / "src"
    for raw in SPLITS.read_text().splitlines():
        s = raw.strip()
        if not raw.startswith((" ", "\t")) and s.endswith(":"):
            cur = s[:-1]
            continue
        if cur and s.startswith(".text") and (src_dir / cur).exists():
            m = re.search(r"start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)", s)
            if m:
                units.append((int(m.group(1), 16), int(m.group(2), 16)))
    return units  # list of (lo, hi)


def donor_defs() -> dict[str, list[tuple[str, int]]]:
    """name -> [(relpath, lineno), ...] for every donor C function definition."""
    defs: dict[str, list[tuple[str, int]]] = {}
    for c in DONOR.rglob("*.c"):
        try:
            lines = c.read_text(errors="replace").splitlines()
        except OSError:
            continue
        for i, line in enumerate(lines, 1):
            m = DEF_LINE.match(line)
            # a real definition: '{' on this or next couple lines, not a ';' decl
            if m and not line.rstrip().endswith(";"):
                kw = m.group(1)
                if kw in ("if", "for", "while", "switch", "return", "sizeof"):
                    continue
                defs.setdefault(kw, []).append((str(c.relative_to(DONOR)), i))
    return defs


def is_paired_single(addr: int, size: int) -> bool:
    """Scan the target asm for ps_/psq_ within [addr, addr+size)."""
    end = addr + size
    pat = re.compile(r"\b(ps_|psq_)")
    line_re = re.compile(r"^\s*/\*\s*([0-9A-Fa-f]+)\s")
    for asm in ASM_DIR.glob("*.s"):
        text = asm.read_text(errors="replace")
        # quick reject
        if f"{addr:08X}".lower() not in text.lower() and f"{addr:08x}" not in text:
            continue
        for line in text.splitlines():
            lm = line_re.match(line)
            if lm:
                va = int(lm.group(1), 16)
                if addr <= va < end and pat.search(line):
                    return True
    return False


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--tsv", help="write the queue to this TSV path")
    ap.add_argument("--max-size", type=lambda s: int(s, 0), default=None)
    ap.add_argument("--scalar-only", action="store_true")
    ap.add_argument("--prefix", default="", help="only names with this prefix")
    ap.add_argument("--limit", type=int, default=60)
    args = ap.parse_args()

    named = named_text_symbols()
    defs = donor_defs()
    done_ranges = already_decompiled()

    def is_done(addr: int) -> bool:
        return any(lo <= addr < hi for lo, hi in done_ranges)

    rows = []
    for name, (addr, size) in named.items():
        if not name.startswith(args.prefix):
            continue
        if name not in defs:
            continue
        if is_done(addr):
            continue
        if args.max_size and size > args.max_size:
            continue
        locs = defs[name]
        ps = is_paired_single(addr, size)
        if args.scalar_only and ps:
            continue
        rows.append({
            "name": name, "addr": addr, "size": size, "ps": ps,
            "ndefs": len(locs), "donor": locs[0][0], "line": locs[0][1],
        })

    # easiest first: scalar before ps, then small, then unique-donor
    rows.sort(key=lambda r: (r["ps"], r["ndefs"] > 1, r["size"]))

    hdr = f"{'SIZE':>6} {'PS':>2} {'D':>2}  {'ADDR':<10} {'NAME':<32} DONOR"
    print(hdr)
    print("─" * len(hdr))
    for r in rows[:args.limit]:
        print(f"0x{r['size']:04X} {'ps' if r['ps'] else '  ':>2} "
              f"{r['ndefs']:>2}  0x{r['addr']:08X} {r['name']:<32} "
              f"{r['donor']}:{r['line']}")
    scalar = sum(1 for r in rows if not r["ps"])
    print(f"\n{len(rows)} portable functions  ({scalar} scalar, "
          f"{len(rows) - scalar} paired-single)  showing {min(args.limit, len(rows))}")

    if args.tsv:
        with open(args.tsv, "w") as f:
            f.write("name\taddr\tsize\tpaired_single\tn_donor_defs\tdonor_file\tdonor_line\n")
            for r in rows:
                f.write(f"{r['name']}\t0x{r['addr']:08X}\t0x{r['size']:X}\t"
                        f"{int(r['ps'])}\t{r['ndefs']}\t{r['donor']}\t{r['line']}\n")
        print(f"wrote {args.tsv}")


if __name__ == "__main__":
    main()
