#!/usr/bin/env python3
"""Export public statistics only. Never copy report sections, instructions or paths."""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import json
import math
from pathlib import Path
import re

FIELDS = ('total_code', 'matched_code', 'complete_code', 'total_data', 'matched_data',
          'total_functions', 'matched_functions', 'total_units', 'complete_units')


def measures(raw: dict) -> dict:
    result = {}
    for key in FIELDS:
        value = raw.get(key, 0)
        if isinstance(value, bool) or not isinstance(value, (int, str)):
            raise ValueError(f'{key} must be a nonnegative integer')
        value = int(value)
        if value < 0:
            raise ValueError(f'{key} must be nonnegative')
        result[key] = value
    for matched, total in [('matched_code', 'total_code'), ('complete_code', 'total_code'),
                           ('matched_data', 'total_data'), ('matched_functions', 'total_functions'),
                           ('complete_units', 'total_units')]:
        if result[matched] > result[total]:
            raise ValueError(f'{matched} exceeds {total}')
    return result


def address(item: dict) -> int | None:
    """Objdiff records a function's load address in metadata.virtual_address."""
    value = (item.get('metadata') or {}).get('virtual_address')
    try:
        return int(value)
    except (TypeError, ValueError):
        return None


def by_address(entries: list) -> list:
    """Sort (address, original index, value) entries; unknown addresses keep report order at the end."""
    return [value for _, _, value in sorted(entries, key=lambda e: (e[0] is None, e[0] or 0, e[1]))]


SWEEP_FIELDS = ('name', 'unit', 'size', 'verdict', 'closeness', 'applied')


def iso_time(value) -> str:
    """Return an aware ISO 8601 timestamp, or raise ValueError."""
    try:
        parsed = datetime.fromisoformat(str(value).replace('Z', '+00:00'))
    except ValueError:
        raise ValueError('Invalid timestamp') from None
    if parsed.tzinfo is None:
        raise ValueError('Timestamp needs a time zone')
    return parsed.isoformat()


def summarize_sweep(rows: list, swept_at: str) -> dict:
    """Keep only the allowlisted per-function fields of a raw sweep, for committing to the repository."""
    if not isinstance(rows, list):
        raise ValueError('Sweep results must be a list')
    return dict(swept_at=iso_time(swept_at),
                rows=[{key: row[key] for key in SWEEP_FIELDS if key in row} for row in rows])


def load_sweep(path: Path) -> tuple[list, str]:
    """Read a raw local sweep (dated by its file time) or a committed summary (dated by swept_at)."""
    data = json.loads(path.read_text())
    if isinstance(data, dict):
        if set(data) != {'swept_at', 'rows'} or not isinstance(data['rows'], list):
            raise ValueError('Sweep summary must contain only swept_at and rows')
        return data['rows'], iso_time(data['swept_at'])
    if not isinstance(data, list):
        raise ValueError('Sweep results must be a list')
    return data, datetime.fromtimestamp(path.stat().st_mtime, timezone.utc).isoformat()


def export(report: dict, *, report_at: str, revision: str | None = None,
           history: list | None = None, sweep: list | None = None,
           sweep_at: str | None = None, build_url: str | None = None) -> dict:
    if revision and not re.fullmatch(r'[0-9a-f]{40}', revision):
        raise ValueError('revision must be a full Git SHA')
    if build_url and not re.fullmatch(r'https://github\.com/[\w.-]+/[\w.-]+/actions/runs/\d+', build_url):
        raise ValueError('build_url must be a GitHub Actions run URL')
    units = []
    matched_names = set()
    for unit_index, unit in enumerate(report.get('units', [])):
        name = str(unit['name']).removeprefix('main/')
        m = measures(unit.get('measures', {}))
        metadata = unit.get('metadata', {})
        functions = []
        for index, fn in enumerate(unit.get('functions', [])):
            try:
                score = float(fn.get('fuzzy_match_percent', 0))
                size = int(fn['size'])
            except (TypeError, ValueError):
                raise ValueError('Invalid function size or similarity') from None
            if not math.isfinite(score) or not 0 <= score <= 100 or size < 0:
                raise ValueError('Invalid function size or similarity')
            functions.append((address(fn), index, dict(name=str(fn['name']), size=size, similarity=score)))
            if score == 100:
                matched_names.add(fn['name'])
        if m['total_code'] or metadata.get('source_path'):
            category = (metadata.get('progress_categories') or ['other'])[0]
            # The report lists .init units after .text; the site draws its code map in this order.
            start = min((a for a, _, _ in functions if a is not None), default=None)
            units.append((start, unit_index, dict(name=name, category=category, complete=bool(metadata.get('complete')),
                                                  measures=m, functions=by_address(functions))))
    units = by_address(units)
    candidates = None
    if sweep is not None:
        counts = {}
        pending = []
        for row in sweep:
            verdict = str(row.get('verdict', 'UNKNOWN'))
            counts[verdict] = counts.get(verdict, 0) + 1
            if verdict != 'MISMATCH' or row.get('applied') or row['name'] in matched_names:
                continue
            try:
                ratio = float(row.get('closeness', 0))
                size = int(row['size'])
            except (TypeError, ValueError):
                raise ValueError('Invalid sweep size or similarity') from None
            if not math.isfinite(ratio) or not 0 <= ratio <= 1:
                raise ValueError('Invalid sweep similarity')
            if size < 0:
                raise ValueError('Invalid sweep size')
            pending.append(dict(name=str(row['name']), unit=str(row['unit']), size=size, similarity=100*ratio))
        candidates = dict(sweep_at=sweep_at, verdicts=counts,
                          rows=sorted(pending, key=lambda row: -row['size']*row['similarity']))
    snapshot_id = revision or f'local-{report_at}'
    latest = dict(id=snapshot_id, revision=revision, report_at=report_at,
                  generated_at=datetime.now(timezone.utc).isoformat(), build_url=build_url,
                  measures=measures(report['measures']), units=units, candidates=candidates)
    # One point per build revision. Reruns replace that revision instead of inventing progress.
    points = {point['id']: point for point in (history or [])}
    points[snapshot_id] = {key: latest[key] for key in ('id', 'revision', 'report_at', 'measures')}
    return dict(schema_version=1, latest=latest,
                history=sorted(points.values(), key=lambda point: point['report_at']))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path, default=Path('build/GL5E4F/report.json'))
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--previous', type=Path)
    parser.add_argument('--revision', help='Only supply for a report built from this exact revision')
    parser.add_argument('--build-url')
    parser.add_argument('--sweep', type=Path,
                        help='Optional raw local sweep, or a summary from tools/sweep_summary.py')
    args = parser.parse_args()
    stamp = lambda p: datetime.fromtimestamp(p.stat().st_mtime, timezone.utc).isoformat()
    previous = json.loads(args.previous.read_text()) if args.previous and args.previous.exists() else None
    if previous and previous.get('schema_version') != 1:
        raise ValueError('Unsupported previous dataset schema')
    sweep, sweep_at = load_sweep(args.sweep) if args.sweep else (None, None)
    result = export(json.loads(args.report.read_text()), report_at=stamp(args.report),
                    revision=args.revision, build_url=args.build_url,
                    history=previous['history'] if previous else None,
                    sweep=sweep, sweep_at=sweep_at)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    temporary = args.output.with_suffix('.tmp')
    temporary.write_text(json.dumps(result, separators=(',', ':')) + '\n')
    temporary.replace(args.output)


if __name__ == '__main__':
    main()
