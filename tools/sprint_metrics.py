"""Local sprint accounting. No transcript scanning, pricing guesses or account access."""
import json
import math
from pathlib import Path

BUDGET_SECONDS = 600
FINISH_RESERVE_SECONDS = 90


def active_seconds(state):
    return sum(state.get('seconds', {}).values())


def budget_report(state):
    active = active_seconds(state)
    return {'active_seconds': round(active, 1), 'budget_seconds': BUDGET_SECONDS,
            'remaining_seconds': round(max(0, BUDGET_SECONDS - active), 1),
            'overrun_seconds': round(max(0, active - BUDGET_SECONDS), 1),
            'finish_due': active >= BUDGET_SECONDS - FINISH_RESERVE_SECONDS}


def record_usage(path: Path, record: dict):
    """Import cumulative totals for one provider run; reimport replaces, never adds twice."""
    required = ('run_id', 'model', 'source')
    for key in required:
        if not isinstance(record.get(key), str) or not record[key].strip():
            raise ValueError(f'usage requires nonempty {key}')
    clean = {key: record[key] for key in required}
    for key in ('input_tokens', 'cached_input_tokens', 'output_tokens'):
        value = record.get(key)
        if type(value) is not int or value < 0:
            raise ValueError(f'{key} must be a provider-reported nonnegative integer')
        clean[key] = value
    if clean['cached_input_tokens'] > clean['input_tokens']:
        raise ValueError('cached_input_tokens is a subset of input_tokens')
    cost = record.get('cost_usd')
    if cost is not None and (type(cost) not in (int, float) or not math.isfinite(cost) or cost < 0):
        raise ValueError('cost_usd must be reported nonnegative USD or null (unknown)')
    clean['cost_usd'] = cost
    records = json.loads(path.read_text()) if path.exists() else {}
    previous = records.get(clean['run_id'])
    if previous:
        if any(previous[k] != clean[k] for k in ('source', 'model')):
            raise ValueError('run identity changed; use the correct provider run_id')
        if any(clean[k] < previous[k] for k in ('input_tokens', 'cached_input_tokens', 'output_tokens')):
            raise ValueError('cumulative run counters cannot decrease')
    records[clean['run_id']] = clean
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(records, indent=2) + '\n')
    return usage_report(records)


def usage_report(records):
    if not records:
        return {'coverage': 'unknown: no provider usage imported', 'cost_usd': None}
    result = {key: sum(r[key] for r in records.values())
              for key in ('input_tokens', 'cached_input_tokens', 'output_tokens')}
    result['coverage'] = f'{len(records)} imported runs only; other usage unknown'
    result['models'] = sorted({r['model'] for r in records.values()})
    result['cost_usd'] = (sum(r['cost_usd'] for r in records.values())
                          if all(r['cost_usd'] is not None for r in records.values()) else None)
    return result
