#!/usr/bin/env python3
"""One-command match loop for a decompiled unit (the inner loop, automated).

Wraps the four steps you otherwise run by hand after editing a unit's C:

    1. ninja build/<ver>/src/<unit>.o          (compile your source)
    2. ninja build/<ver>/obj/<unit>.o           (rebuild the objdiff target via dtk split)
    3. ninja build/<ver>/report.json            (byte-level match report)
    4. read report.json + show diffs            (per-function status)

and prints a compact per-function status table. For every function below 100%
it appends the mnemonic-level side-by-side from verify_fn.py so you can see what
to fix without a second command. Designed to be driven repeatedly (by you or an
agent): edit C -> `tools/match.py numath/nuvec` -> read -> edit -> repeat.

Usage:
    python3 tools/match.py numath/nuvec            # unit (no extension)
    python3 tools/match.py numath/nuvec --quiet    # table only, no diffs
    python3 tools/match.py numath/nuvec --only NuVecAdd,NuVecCross
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VERSION = "GL5E4F"
REPORT = ROOT / f"build/{VERSION}/report.json"
PY = sys.executable

GREEN, YELLOW, RED, DIM, RST = "\033[92m", "\033[93m", "\033[91m", "\033[2m", "\033[0m"


def run(cmd: list[str]) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)


def colour(pct: float) -> str:
    c = GREEN if pct >= 100 else YELLOW if pct >= 90 else RED
    return f"{c}{pct:6.2f}%{RST}"


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("unit", help="unit path without extension, e.g. numath/nuvec")
    ap.add_argument("--quiet", action="store_true", help="table only; skip diffs")
    ap.add_argument("--only", help="comma-separated function names to diff")
    args = ap.parse_args()

    unit = args.unit.removesuffix(".c")
    src_obj = f"build/{VERSION}/src/{unit}.o"
    only = set(args.only.split(",")) if args.only else None

    # 1. compile
    r = run(["ninja", src_obj])
    if r.returncode != 0:
        print(f"{RED}compile failed:{RST}\n{r.stdout}{r.stderr}")
        sys.exit(1)

    # 2. target object (produced by `dtk dol split`; re-runs if symbols/splits changed)
    tgt_obj = f"build/{VERSION}/obj/{unit}.o"
    r = run(["ninja", tgt_obj])
    if r.returncode != 0:
        print(f"{RED}target split failed:{RST}\n{r.stdout}{r.stderr}")
        sys.exit(1)

    # 3. report
    r = run(["ninja", str(REPORT.relative_to(ROOT))])
    if r.returncode != 0:
        print(f"{RED}report failed:{RST}\n{r.stdout}{r.stderr}")
        sys.exit(1)

    # 4. status table
    report = json.loads(REPORT.read_text())
    uname = f"main/{unit}"
    funcs = None
    for u in report.get("units", []):
        if u.get("name") == uname:
            funcs = u.get("functions", [])
            break
    if funcs is None:
        print(f"{RED}unit '{uname}' not in report.json{RST}")
        sys.exit(1)

    rows = sorted(funcs, key=lambda f: f.get("fuzzy_match_percent", 0))
    matched = sum(1 for f in funcs if f.get("fuzzy_match_percent", 0) >= 100)
    print(f"\n{unit}: {GREEN}{matched}{RST}/{len(funcs)} matched")
    print("─" * 44)
    below = []
    for f in rows:
        pct = f.get("fuzzy_match_percent", 0)
        name = f.get("name", "?")
        print(f"  {colour(pct)}  {name}")
        if pct < 100:
            below.append(name)

    # 5. diffs for non-matching functions
    if args.quiet or not below:
        return
    targets = [n for n in below if (only is None or n in only)]
    for name in targets:
        print(f"\n{YELLOW}── diff: {name} ──{RST}")
        d = run([PY, "tools/verify_fn.py", name])
        print(d.stdout + (d.stderr if d.returncode else ""))


if __name__ == "__main__":
    main()
