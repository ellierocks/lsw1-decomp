#!/usr/bin/env python3
"""Print ranked permuter candidates, one function name per line, to fill a queue backlog.

    python3 tools/permute_backlog.py --limit 60
    tools/permute_long.sh 7 2 --total 8 $(python3 tools/permute_backlog.py --limit 60)
    python3 tools/permute_backlog.py --slots 7 --hours 8 --per 2     # size hint on stderr

Sources: `work/sweep/results.json` (rows whose verdict is not MATCH and whose exact/target
ratio is at least --min-ratio) and, with `--index PATH`, the markdown table from
`tools/worktree_cleanup.py` (rows whose main column is not `matched`; they carry no sweep
numbers, so they rank after the sweep rows). Dropped: functions the build report already
shows at 100 %, functions with an `output-0*` in work/permute, and duplicates. Ranked by
closeness, then size, both descending.

--slots/--hours/--per print a recommended backlog size to stderr: at least 2 x slots x
hours / per, so a run of early matches does not empty the backlog before the deadline.
"""

from __future__ import annotations

import argparse
import json
import math
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
NAME = re.compile(r"^[A-Za-z0-9_]+$")


def sweep_rows(path: Path, min_ratio: float) -> dict[str, dict]:
    rows: dict[str, dict] = {}
    if not path.is_file():
        return rows
    for row in json.loads(path.read_text()):
        name = row.get("name")
        if not name or row.get("verdict") == "MATCH":
            continue
        closeness = row.get("closeness")
        if closeness is None:
            target = row.get("target")
            closeness = row.get("exact", 0) / target if target else 0.0
        if closeness < min_ratio:
            continue
        rows[name] = {"closeness": closeness, "size": row.get("size") or 0}
    return rows


def index_names(path: Path) -> list[str]:
    """Function names of an INDEX.md table whose `main` column is not 'matched'."""
    names = []
    for line in path.read_text(errors="replace").splitlines():
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if len(cells) < 4 or cells[0] in ("function", "") or set(cells[0]) <= {"-", ":"}:
            continue
        if cells[2] != "matched":
            names.append(cells[0])
    return names


def matched_in_report(path: Path) -> set[str]:
    if not path.is_file():
        return set()
    data = json.loads(path.read_text())
    return {fn["name"] for unit in data.get("units", []) for fn in unit.get("functions", [])
            if fn.get("fuzzy_match_percent") == 100}


def has_zero_output(permute: Path, name: str) -> bool:
    return any(p.is_dir() for p in (permute / name).glob("output-0*"))


def rank(root: Path, min_ratio: float, index: Path | None, report: Path | None) -> list[str]:
    candidates = sweep_rows(root / "work" / "sweep" / "results.json", min_ratio)
    if index:
        for name in index_names(index):
            candidates.setdefault(name, {"closeness": 0.0, "size": 0})
    matched = matched_in_report(report if report else root / "build" / "GL5E4F" / "report.json")
    permute = root / "work" / "permute"
    ordered = sorted(candidates.items(), key=lambda kv: (-kv[1]["closeness"], -kv[1]["size"], kv[0]))
    return [name for name, _ in ordered
            if NAME.match(name) and name not in matched and not has_zero_output(permute, name)]


def recommended_size(slots: int, hours: float, per: float) -> int:
    return max(slots, math.ceil(2 * slots * hours / per))


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--min-ratio", type=float, default=0.9, help="lowest exact-instruction ratio (default 0.9)")
    ap.add_argument("--limit", type=int, help="print at most this many names")
    ap.add_argument("--index", type=Path, help="worktree_cleanup.py INDEX.md to add parked functions from")
    ap.add_argument("--report", type=Path, help="build report.json (default build/GL5E4F/report.json)")
    ap.add_argument("--root", type=Path, default=ROOT, help=argparse.SUPPRESS)
    ap.add_argument("--slots", type=int, help="queue slots, for the size recommendation")
    ap.add_argument("--hours", type=float, help="total hours of the run, for the size recommendation")
    ap.add_argument("--per", type=float, help="hours per target, for the size recommendation")
    args = ap.parse_args(argv)
    sizing = [args.slots, args.hours, args.per]
    if any(v is not None for v in sizing) and not all(v is not None for v in sizing):
        ap.error("--slots, --hours and --per go together")
    if args.slots is not None and (args.slots < 1 or args.hours <= 0 or args.per <= 0):
        ap.error("--slots, --hours and --per must be positive")
    if args.index and not args.index.is_file():
        ap.error(f"--index {args.index} not found")
    names = rank(args.root, args.min_ratio, args.index, args.report)
    if args.slots is not None:
        want = recommended_size(args.slots, args.hours, args.per)
        note = f"recommended backlog size: {want} (2 x {args.slots} slots x {args.hours:g} h / {args.per:g} h per target)"
        if args.limit is not None and args.limit < want:
            note += f"; --limit {args.limit} is smaller"
        if len(names) < want:
            note += f"; only {len(names)} candidates available"
        print(note, file=sys.stderr)
    if not names:
        print("permute_backlog: no candidates (run python3 tools/sweep.py or pass --index)", file=sys.stderr)
    for name in names[:args.limit]:
        print(name)
    return 0


if __name__ == "__main__":
    sys.exit(main())
