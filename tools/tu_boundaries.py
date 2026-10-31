#!/usr/bin/env python3
"""Propose translation-unit boundaries for game and Nu2 `.text` (Phase 2, step 1).

Every compiled TU carries its own `.rodata` block, and the linker lays those blocks out in
the same order as the TUs' `.text`. Most game and Nu2 TUs keep a `__FILE__` path in that
block (for example `../nu2api.legogc/nucore/ngc_obj/../nufile.c`), so the path strings in
`.rodata` give the link order. This tool assigns each function a TU so that, in address order:

- TU indices never decrease, because link order is shared by `.text` and `.rodata`;
- a function that loads a TU's path string belongs to that TU (strong evidence);
- a function that loads other `.rodata` belongs to a TU whose path string lies next to it
  (weak evidence, since a block's extent is only known up to its neighbours' paths);
- functions that are adjacent in the Mac demo build's symbol order stay together.

Alignment padding is not used. Every pair of consecutive functions tiles exactly, at known
unit boundaries too, so it carries no signal.

The result is a report. It never edits `config/`. With `--write-splits`, it also writes a
complete candidate `splits.txt`: existing units are kept, and provisional `auto/` `.text`
units are cut around them. Use it to check that a relinked DOL stays byte-identical.
TUs with no path string are absorbed into a neighbour, and a boundary between functions
that have no evidence can move freely. The report gives the range of equally good start
addresses for each TU.
"""

from __future__ import annotations

import argparse
import bisect
import csv
import re
import struct
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import string_xrefs as sx  # noqa: E402

SPLITS = ROOT / "config/GL5E4F/splits.txt"
MAC_SYMBOLS = ROOT / "docs/symbol_donors/mac_lsw1_demo_symbols.tsv"
RODATA_INDEX = 7
DATA_INDICES = range(7, 11)

STRONG = 20.0
WEAK = 1.0
MAC_SAME = 3.0
CHANGE = 0.25
ANON = 0.05

PATH_RE = re.compile(rb"[\x20-\x7e]{4,240}?\.(?:c|cpp)\x00")
LOADS = {32, 33, 34, 35, 40, 41, 42, 43}
UPDATES = {33, 35, 37, 39, 41, 43, 45, 49, 51, 53, 55}
MEMORY = set(range(32, 56))
VOLATILE = (0, *range(3, 13))


@dataclass(frozen=True)
class Anchor:
    address: int
    raw: str
    name: str


@dataclass
class Function:
    name: str
    address: int
    size: int
    exact: set[int] = field(default_factory=set)
    refs: list[int] = field(default_factory=list)


@dataclass
class Unit:
    name: str
    sections: dict[str, tuple[int, int]]


def unit_name(raw: str) -> str:
    """`../nu2api.legogc/nucore/ngc/ngc_obj/../nufile_ngc.c` -> `nucore/ngc/nufile_ngc.c`."""
    parts: list[str] = []
    for part in raw.split("/"):
        if part in ("", "."):
            continue
        if part == "..":
            if parts and parts[-1] != "..":
                parts.pop()
            else:
                parts.append(part)
            continue
        parts.append(part)
    parts = [p for p in parts if p != ".." and not p.endswith(".legogc")]
    return "/".join(parts) if len(parts) > 1 else "game/" + parts[-1]


def find_anchors(data: bytes, sections: list[sx.DolSection]) -> list[Anchor]:
    """Source-path strings in `.rodata`, in address order. A match can start with a stray
    printable byte from the previous object; the name ignores it."""
    sec = next(s for s in sections if s.index == RODATA_INDEX)
    blob = data[sec.offset:sec.offset + sec.size]
    anchors = []
    for m in PATH_RE.finditer(blob):
        text = m.group()[:-1].decode()
        cut = re.search(r"\.\.?/|ngc_obj/", text)
        start = cut.start() if cut else 0
        anchors.append(Anchor(sec.address + m.start() + start, text[start:], unit_name(text[start:])))
    return anchors


def sign16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def scan_refs(words: list[int]) -> list[int]:
    """Absolute addresses materialised in one function: `lis`/`addi`/`ori` pairs, and
    D-form loads/stores through a register known to hold such an address. Calls clobber
    the volatile registers; anything else that writes a tracked register forgets it."""
    regs: dict[int, int] = {}
    out = []
    for ins in words:
        op, rd, ra, imm = ins >> 26, (ins >> 21) & 31, (ins >> 16) & 31, ins & 0xFFFF
        if op == 15:
            base = 0 if ra == 0 else regs.get(ra)
            if base is None:
                regs.pop(rd, None)
            else:
                regs[rd] = (base + (sign16(imm) << 16)) & 0xFFFFFFFF
        elif op == 14:
            if ra and ra in regs:
                regs[rd] = (regs[ra] + sign16(imm)) & 0xFFFFFFFF
                out.append(regs[rd])
            else:
                regs.pop(rd, None)
        elif op == 24:
            if rd in regs:
                regs[ra] = regs[rd] | imm
                out.append(regs[ra])
            else:
                regs.pop(ra, None)
        elif op in MEMORY:
            addr = (regs[ra] + sign16(imm)) & 0xFFFFFFFF if ra and ra in regs else None
            if addr is not None:
                out.append(addr)
            if op in LOADS or op == 46:
                for r in (range(rd, 32) if op == 46 else (rd,)):
                    regs.pop(r, None)
            if op in UPDATES:
                if addr is None:
                    regs.pop(ra, None)
                else:
                    regs[ra] = addr
        elif op == 18:
            if ins & 1:
                for r in VOLATILE:
                    regs.pop(r, None)
        elif op == 31 and (ins >> 1) & 0x3FF == 444 and rd == (ins >> 11) & 31:
            if rd in regs:
                regs[ra] = regs[rd]
            else:
                regs.pop(ra, None)
        elif op in (16, 19) or (op == 31 and (ins >> 1) & 0x3FF in (151, 183, 215, 247, 407, 439, 663, 695, 727, 759)):
            pass
        else:
            regs.pop(rd, None)
            regs.pop(ra, None)
    return out


def parse_splits(text: str) -> list[Unit]:
    units: list[Unit] = []
    for line in text.splitlines():
        head = re.match(r"^(\S.*):$", line)
        if head and head.group(1) != "Sections":
            units.append(Unit(head.group(1), {}))
            continue
        sec = re.match(r"^\s+(\.\w+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
        if sec and units:
            units[-1].sections[sec.group(1)] = (int(sec.group(2), 16), int(sec.group(3), 16))
    return units


def mac_order(path: Path = MAC_SYMBOLS) -> dict[str, int]:
    """Rank of each Mac text function by address. Itanium-mangled free functions keep only
    their base name; overloads take the first rank."""
    if not path.exists():
        return {}
    with path.open() as f:
        rows = [r for r in csv.DictReader(f, delimiter="\t") if r["section"] == "text" and r["type_abbrev"] == "func"]
    rows.sort(key=lambda r: int(r["address"], 16))
    ranks: dict[str, int] = {}
    for i, r in enumerate(rows):
        name = r["clean_name"]
        m = re.match(r"_Z(\d+)", name)
        if m:
            name = name[m.end():m.end() + int(m.group(1))]
        ranks.setdefault(name, i)
    return ranks


@dataclass
class Segment:
    state: int
    first: int
    last: int
    slack: tuple[int, int] = (0, 0)


def costs(fn: Function, anchors: list[Anchor], starts: list[int], nstates: int) -> list[float]:
    """Per-state cost of one function. State 2k+1 is TU k, and state 2k is the run of
    path-less TUs before it. Exact path loads pin a TU; other `.rodata` loads must fall
    between the neighbouring path strings."""
    hits: dict[int, float] = {}
    for k in fn.exact:
        hits[2 * k + 1] = hits.get(2 * k + 1, 0.0) + STRONG
    for r in fn.refs:
        k = bisect.bisect_right(starts, r) - 1
        for s in ((2 * k + 1, 2 * k + 2, 2 * k + 3) if k >= 0 else (0, 1)):
            if s < nstates:
                hits[s] = hits.get(s, 0.0) + WEAK
    total = STRONG * len(fn.exact) + WEAK * len(fn.refs)
    c = [total - hits.get(s, 0.0) + (ANON if s % 2 == 0 else 0.0) for s in range(nstates)]
    return c


def align(cost: list[list[float]], keep: list[bool]) -> list[int]:
    """Monotone state assignment minimising cost; a change before function i costs CHANGE,
    plus MAC_SAME when `keep[i]` says i is Mac-adjacent to i-1."""
    n, m = len(cost), len(cost[0])
    prev = list(cost[0])
    back: list[list[int]] = []
    for i in range(1, n):
        jump = CHANGE + (MAC_SAME if keep[i] else 0.0)
        best, arg = float("inf"), 0
        row, ptr = [], []
        for s in range(m):
            if s and prev[s - 1] < best:
                best, arg = prev[s - 1], s - 1
            if prev[s] <= best + jump:
                row.append(prev[s] + cost[i][s])
                ptr.append(s)
            else:
                row.append(best + jump + cost[i][s])
                ptr.append(arg)
        back.append(ptr)
        prev = row
    s = min(range(m), key=lambda k: (prev[k], -k))
    path = [s]
    for ptr in reversed(back):
        s = ptr[s]
        path.append(s)
    return path[::-1]


def segments(path: list[int], cost: list[list[float]], keep: list[bool]) -> list[Segment]:
    segs: list[Segment] = []
    for i, s in enumerate(path):
        if segs and segs[-1].state == s:
            segs[-1].last = i
        else:
            segs.append(Segment(s, i, i))
    for left, right in zip(segs, segs[1:]):
        lo, hi = left.first + 1, right.last
        totals = {}
        a = sum(cost[i][left.state] for i in range(left.first, lo))
        b = sum(cost[i][right.state] for i in range(lo, hi + 1))
        for j in range(lo, hi + 1):
            totals[j] = a + b + (MAC_SAME if keep[j] else 0.0)
            a += cost[j][left.state]
            b -= cost[j][right.state]
        best = min(totals.values())
        ties = [j for j, t in totals.items() if t <= best + 1e-9]
        right.slack = (right.first - min(ties), max(ties) - right.first)
    return segs


def analyse(data: bytes, symbols_path: Path, splits_text: str, mac: dict[str, int]):
    sections = parse_sections(data)
    text = next(s for s in sections if s.index == 1)
    units = parse_splits(splits_text)
    sdk_start = min((u.sections[".text"][0] for u in units
                     if ".text" in u.sections and u.name.startswith("dolphin/")), default=text.address + text.size)
    anchors = find_anchors(data, sections)
    starts = [a.address for a in anchors]
    rodata = next(s for s in sections if s.index == RODATA_INDEX)
    exact = {}
    for k, a in enumerate(anchors):
        for d in range(4):
            exact[a.address + d] = k
    functions = []
    for s in sorted(sx.parse_symbols(symbols_path), key=lambda s: s.address):
        if s.section != ".text" or "type:function" not in s.meta or s.address >= sdk_start or not s.size:
            continue
        off = text.offset + s.address - text.address
        words = list(struct.unpack(f">{s.size // 4}I", data[off:off + s.size // 4 * 4]))
        fn = Function(s.name, s.address, s.size)
        for r in scan_refs(words):
            if r in exact:
                fn.exact.add(exact[r])
            elif rodata.address <= r < rodata.address + rodata.size:
                fn.refs.append(r)
        functions.append(fn)
    keep = [False] + [a.name in mac and b.name in mac and mac[b.name] - mac[a.name] == 1
                      for a, b in zip(functions, functions[1:])]
    nstates = 2 * len(anchors) + 1
    cost = [costs(f, anchors, starts, nstates) for f in functions]
    path = align(cost, keep)
    return anchors, functions, segments(path, cost, keep), keep, units, sdk_start


def parse_sections(data: bytes) -> list[sx.DolSection]:
    out = []
    for i in range(18):
        offset, address, size = (struct.unpack(">I", data[base + i * 4: base + i * 4 + 4])[0]
                                 for base in (0, 0x48, 0x90))
        if size:
            out.append(sx.DolSection(i, sx.SECTION_NAMES.get(i, f".dol{i}"), offset, address, size))
    return out


def segment_name(seg: Segment, anchors: list[Anchor], functions: list[Function]) -> str:
    if seg.state % 2:
        return anchors[seg.state // 2].name
    return f"game/unknown_{functions[seg.first].address:08X}.c"


def proposed_ranges(segs, anchors, functions, units, sdk_start) -> list[tuple[str, int, int]]:
    """Provisional `auto/` `.text` units, cut around every existing `.text` split."""
    taken = sorted(u.sections[".text"] for u in units if ".text" in u.sections)
    out = []
    for i, seg in enumerate(segs):
        start = functions[seg.first].address
        end = functions[segs[i + 1].first].address if i + 1 < len(segs) else sdk_start
        pieces, cur = [], start
        for a, b in taken:
            if b <= cur or a >= end:
                continue
            if a > cur:
                pieces.append((cur, a))
            cur = max(cur, b)
        if cur < end:
            pieces.append((cur, end))
        name = segment_name(seg, anchors, functions)
        stem = name.rsplit(".", 1)
        for n, (a, b) in enumerate(pieces):
            label = name if len(pieces) == 1 else f"{stem[0]}_{n + 1}.{stem[1]}"
            out.append((f"auto/{label}", a, b))
    return out


def splits_with(splits_text: str, ranges: list[tuple[str, int, int]]) -> str:
    blocks = "".join(f"\n{name}:\n\t.text       start:0x{a:08X} end:0x{b:08X}\n" for name, a, b in ranges)
    return splits_text.rstrip("\n") + "\n" + blocks


def report(anchors, functions, segs, keep, units) -> list[dict]:
    rows = []
    for seg in segs:
        fns = functions[seg.first:seg.last + 1]
        refs = [r for f in fns for r in f.refs]
        existing = sorted({u.name for u in units for f in fns
                           if ".text" in u.sections and u.sections[".text"][0] <= f.address < u.sections[".text"][1]})
        rows.append({
            "unit": segment_name(seg, anchors, functions),
            "start": f"{fns[0].address:08X}",
            "end": f"{fns[-1].address + fns[-1].size:08X}",
            "functions": len(fns),
            "path_refs": sum(1 for f in fns if f.exact),
            "rodata_refs": len(refs),
            "rodata": f"{min(refs):08X}..{max(refs):08X}" if refs else "",
            "mac_links": sum(keep[seg.first + 1:seg.last + 1]),
            "start_range": (f"{functions[seg.first - seg.slack[0]].address:08X}.."
                            f"{functions[seg.first + seg.slack[1]].address:08X}" if seg.first else ""),
            "existing": " ".join(existing),
        })
    return rows


def summary(anchors, functions, segs, keep) -> list[str]:
    named = {s.state // 2 for s in segs if s.state % 2}
    path = [None] * len(functions)
    for s in segs:
        for i in range(s.first, s.last + 1):
            path[i] = s.state
    broken = sum(1 for i, k in enumerate(keep) if k and path[i] != path[i - 1])
    against = sum(1 for i, f in enumerate(functions) if f.exact and path[i] not in {2 * k + 1 for k in f.exact})
    firm = sum(1 for s in segs[1:] if s.slack == (0, 0))
    return [
        f"path strings in .rodata: {len(anchors)}; TUs with functions: {len(named)}; segments: {len(segs)}",
        f"functions: {len(functions)}; with a path load: {sum(1 for f in functions if f.exact)}; "
        f"with other .rodata loads: {sum(1 for f in functions if f.refs)}",
        f"Mac-adjacent pairs: {sum(keep)}; split by a boundary: {broken}",
        f"path loads assigned to another TU: {against}",
        f"boundaries with no slack: {firm} of {len(segs) - 1}",
    ]


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--dol", type=Path, default=sx.DOL)
    ap.add_argument("--tsv", type=Path, default=ROOT / "work/tu_boundaries.tsv", help="per-TU report")
    ap.add_argument("--write-splits", type=Path, help="write a candidate splits.txt with provisional auto/ units")
    args = ap.parse_args(argv)
    if not args.dol.exists():
        print(f"error: missing {args.dol}; run tools/bootstrap.sh", file=sys.stderr)
        return 2
    splits_text = SPLITS.read_text()
    anchors, functions, segs, keep, units, sdk_start = analyse(args.dol.read_bytes(), sx.SYMBOLS, splits_text, mac_order())
    rows = report(anchors, functions, segs, keep, units)
    args.tsv.parent.mkdir(parents=True, exist_ok=True)
    with args.tsv.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t")
        w.writeheader()
        w.writerows(rows)
    for line in summary(anchors, functions, segs, keep):
        print(line)
    print(f"wrote {args.tsv}")
    if args.write_splits:
        ranges = proposed_ranges(segs, anchors, functions, units, sdk_start)
        args.write_splits.parent.mkdir(parents=True, exist_ok=True)
        args.write_splits.write_text(splits_with(splits_text, ranges))
        print(f"wrote {args.write_splits} with {len(ranges)} provisional units")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
