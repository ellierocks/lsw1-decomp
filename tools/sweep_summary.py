#!/usr/bin/env python3
"""Write the committed sweep summary: allowlisted per-function statistics and the sweep time.

Source, target, context and other experiment output are dropped. The summary is dated by the
raw results file's modification time unless --swept-at is given.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import json
from pathlib import Path

from export_progress import summarize_sweep


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--results', type=Path, default=Path('work/sweep/results.json'))
    parser.add_argument('--output', type=Path, default=Path('progress/sweep.json'))
    parser.add_argument('--swept-at', help='ISO 8601 time the sweep finished, with a time zone')
    args = parser.parse_args()
    swept_at = args.swept_at or datetime.fromtimestamp(args.results.stat().st_mtime, timezone.utc).isoformat()
    summary = summarize_sweep(json.loads(args.results.read_text()), swept_at)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    # One row per line keeps refreshes reviewable as line diffs.
    rows = ',\n'.join('  ' + json.dumps(row, separators=(',', ':')) for row in summary['rows'])
    args.output.write_text(f'{{"swept_at":{json.dumps(summary["swept_at"])},"rows":[\n{rows}\n]}}\n')
    print(f'{len(summary["rows"])} rows, swept at {summary["swept_at"]} -> {args.output}')


if __name__ == '__main__':
    main()
