#!/usr/bin/env python3
"""List functions that strictly match but score below 100% in the objdiff report.

objdiff reads a float or string literal reloc that targets a section symbol (`.rodata+0x4`)
differently from the retail `lbl_` symbol, so a function the strict verifier accepts can sit at
99.9% and not count in `matched_functions`. This tool re-checks every report function between
`--min-percent` and 100% with `verify_fn.py --strict-symbols` against the current build and
reports the ones that pass. Run `bash build.sh` first; the objects are not rebuilt here.

    python3 tools/strict_gap.py
    python3 tools/strict_gap.py --json

The result is an addition to the objdiff figures, never a replacement: the report and
`matching_progress.py` keep their definition of matched. It does not touch splits or the oracle.
"""
from __future__ import annotations

import argparse
import contextlib
import io
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import verify_fn as vf  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
REPORT = ROOT / "build" / "GL5E4F" / "report.json"


def candidates(report: dict, min_percent: float) -> list[dict]:
    out = []
    for unit in report.get("units", []):
        for fn in unit.get("functions", []):
            pct = fn.get("fuzzy_match_percent")
            if pct is not None and min_percent <= pct < 100:
                out.append({"name": fn["name"], "unit": unit["name"], "size": int(fn.get("size", 0)),
                            "percent": pct})
    return sorted(out, key=lambda r: (r["unit"], r["name"]))


def strict_match(name: str) -> bool:
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf), contextlib.redirect_stderr(io.StringIO()):
        code = vf.main([name, "--strict-symbols", "--json", "--allow-stale"])
    return code == 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--min-percent", type=float, default=90.0, help="lowest objdiff percent to re-check")
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args(argv)
    if not REPORT.is_file():
        print("error: no build/GL5E4F/report.json; run bash build.sh", file=sys.stderr)
        return 2
    report = json.loads(REPORT.read_text())
    rows = candidates(report, args.min_percent)
    passed = [dict(r) for r in rows if strict_match(r["name"])]
    total = {"functions": len(passed), "bytes": sum(r["size"] for r in passed), "checked": len(rows)}
    if args.json:
        print(json.dumps({"strict_only": passed, **total}, indent=2))
        return 0
    for r in passed:
        print(f"{r['name']:<40}{r['size']:>#8x}  {r['percent']:.2f}%  {r['unit']}")
    measures = report.get("measures", {})
    mf, mc = int(measures.get("matched_functions", 0)), int(float(measures.get("matched_code", 0)))
    print(f"\n{total['functions']} of {total['checked']} sub-100% functions pass the strict verifier "
          f"({total['bytes']:,} bytes).")
    print(f"objdiff matched: {mf:,} functions, {mc:,} bytes; with these: {mf + total['functions']:,} "
          f"functions, {mc + total['bytes']:,} bytes.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
