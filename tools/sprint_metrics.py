"""Local sprint accounting. No transcript scanning, pricing guesses or account access."""
import json
import math
import os
import time
from pathlib import Path

NEAR_MISS_RATIO = 0.95
PARK_FLOOR = 25  # distinct compiling candidates before a park is credible (docs/sprint_workflow.md)


DEFAULT_IDLE_CAP = 900.0  # seconds; a gap between activity events counts only up to this
IDLE_CAP_ENV = 'LSW1_IDLE_CAP'


def idle_cap(explicit=None, state=None, environ=None):
    """Idle cap in seconds: explicit value, then $LSW1_IDLE_CAP, then the saved one, then 15 min."""
    environ = os.environ if environ is None else environ
    for value in (explicit, environ.get(IDLE_CAP_ENV), (state or {}).get('idle_cap')):
        if value in (None, ''):
            continue
        cap = float(value)
        if cap <= 0:
            raise ValueError('idle cap must be positive seconds')
        return cap
    return DEFAULT_IDLE_CAP


def advance(state, now, cap):
    """Close the gap since the last activity event in the running phase.

    The gap adds to wall-clock in full but to active time only up to `cap`; the rest is idle
    (rate-limit stalls, machine suspend, waits nobody paused). Legacy states without `last`
    treat the phase start as the last event and their recorded seconds as wall-clock.
    """
    state.setdefault('seconds', {})
    state.setdefault('wall', dict(state['seconds']))
    phase = state.get('active')
    if phase in (None, 'paused'):
        return state
    last = state.get('last', state.get('since', now))
    gap = now - last
    if gap < 0:
        raise ValueError('clock moved backwards; retain the timing file and correct the clock')
    counted = min(gap, cap)
    state['seconds'][phase] = state['seconds'].get(phase, 0) + counted
    state['wall'][phase] = state['wall'].get(phase, 0) + gap
    state['idle_seconds'] = state.get('idle_seconds', 0) + (gap - counted)
    state['last'] = now
    return state


def write_state(path: Path, state):
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_name(path.name + f'.{os.getpid()}.tmp')
    tmp.write_text(json.dumps(state, indent=2) + '\n')
    os.replace(tmp, path)


def record_activity(path: Path, now=None, cap=None):
    """Note a tool run as an activity event. Returns the new state, or None without a running timer."""
    try:
        state = json.loads(path.read_text())
    except (OSError, ValueError):
        return None
    if state.get('active') in (None, 'paused'):
        return None
    now = time.time() if now is None else now
    try:
        advance(state, now, idle_cap(cap, state))
    except ValueError:
        return state
    try:
        write_state(path, state)
    except OSError:
        pass
    return state


def switch_phase(path: Path, name, now=None, cap=None):
    """Close the running phase (an activity event) and start `name`; `paused` stops the clock."""
    now = time.time() if now is None else now
    state = json.loads(path.read_text()) if path.exists() else {'seconds': {}}
    if cap is not None:
        state['idle_cap'] = idle_cap(cap)
    advance(state, now, idle_cap(None, state))
    state.update(active=name, since=now, last=now)
    write_state(path, state)
    return state


def active_seconds(state):
    return sum(state.get('seconds', {}).values())


def timing_report(state):
    """Measured active seconds per phase. Time is reported, never used as a stop rule.

    `active_seconds` excludes idle gaps beyond the cap; `wall_seconds` is the plain wall-clock
    inside the same phases, so the difference is visible.
    """
    wall = state.get('wall', state.get('seconds', {}))
    return {'active_seconds': round(active_seconds(state), 1),
            'phases': {k: round(v, 1) for k, v in state.get('seconds', {}).items()},
            'wall_seconds': round(sum(wall.values()), 1),
            'wall_phases': {k: round(v, 1) for k, v in wall.items()},
            'idle_excluded_seconds': round(state.get('idle_seconds', 0), 1),
            'idle_cap_seconds': idle_cap(None, state)}


def dedupe(names):
    """Names without repeats, first occurrence order kept."""
    return list(dict.fromkeys(names))


def queue_add(path: Path, names):
    """Append names missing from the saved sprint queue. Returns the added names, or None
    when no baseline exists (nothing is created)."""
    try:
        base = json.loads(path.read_text())
    except (OSError, ValueError):
        return None
    queue = dedupe(base.get('queue', []))
    added = [n for n in dedupe(names) if n not in queue]
    if added or queue != base.get('queue'):
        base['queue'] = queue + added
        write_state(path, base)
    return added


def near_miss(exact, target):
    """True when a mismatch is at least NEAR_MISS_RATIO exact."""
    return target > 0 and exact < target and exact / target >= NEAR_MISS_RATIO


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


def real_score(e: dict) -> bool:
    """Score events from unit tests (target 0, e.g. the stub function `f`) are not attempts."""
    return e.get("kind") == "score" and (e.get("verdict") == "ERROR" or (e.get("target") or 0) > 0)


PLATEAU_WINDOW = 10  # distinct candidates since the last best-score gain that make a plateau


def plateau(events: list[dict], name: str, floor: int = PARK_FLOOR,
            window: int = PLATEAU_WINDOW) -> dict | None:
    """{"distinct", "best", "target", "since"} once NAME has FLOOR distinct compiled candidates
    in EVENTS and the last WINDOW of them did not raise the best exact count; else None.

    fn_80090FB0 took 9.3 more minutes after it plateaued
    (docs/workflow_gaps.md#17-plateaued-functions-kept-being-scored)."""
    seen: dict[str, int] = {}
    target = 0
    for e in events:
        if e.get("name") == name and real_score(e) and e.get("verdict") != "ERROR" and e.get("codegen"):
            if e.get("verdict") == "MATCH":
                return None
            seen.setdefault(e["codegen"], int(e.get("exact") or 0))
            target = int(e.get("target") or target)
    exacts = list(seen.values())
    if len(exacts) < floor:
        return None
    best = max(exacts)
    since = len(exacts) - 1 - exacts.index(best)
    if since < window:
        return None
    return {"distinct": len(exacts), "best": best, "target": target, "since": since}
