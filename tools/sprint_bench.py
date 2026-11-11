#!/usr/bin/env python3
"""Benchmark a sprint run: startup latency, setup steps, attempt and match throughput, per model.

    python3 tools/sprint_bench.py start --model claude-sonnet-5-5 --launched-at 1762300000 [--lane unit --unit U]
    python3 tools/sprint_bench.py time bootstrap -- bash tools/bootstrap.sh   # timed setup step
    python3 tools/sprint_bench.py mark first-draft                            # free-form milestone
    python3 tools/sprint_bench.py report                                      # this run
    python3 tools/sprint_bench.py compare                                     # all finished runs

Events go to `work/sprint_bench.jsonl` in this checkout. `score_fn.py` appends one `score`
event per scored attempt automatically once `start` has run, so attempts, first score, first
match and matches per hour need no bookkeeping. `sprint_finish.py` appends the verified result
and copies the run summary to `$LSW1_CACHE/bench/` (default `~/.cache/lsw1/bench/`), which
outlives the worktree, so `compare` can rank runs and models side by side.

`--launched-at` is the epoch second the thread was launched (the launcher passes it in the
kickoff prompt). With the worktree's `.git` file time it splits startup into worktree creation
and agent boot. Times are wall-clock; model names are whatever the launcher states, never
inferred. `lane` and `unit` name what the run worked on (`tools/kickoff.py --start` sets them);
records written before the field existed read as lane None. Nothing here reads transcripts,
credentials or quotas.
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EVENTS = ROOT / "work" / "sprint_bench.jsonl"


def cache_dir() -> Path:
    return Path(os.environ.get("LSW1_CACHE", Path.home() / ".cache" / "lsw1")) / "bench"


def append(kind: str, path: Path | None = None, now: float | None = None, **data) -> dict | None:
    """Append one event; a no-op (None) until `start` has created the log, unless kind is start."""
    path = path or EVENTS
    if kind != "start" and not path.exists():
        return None
    event = {"t": round(time.time() if now is None else now, 3), "kind": kind, **data}
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a") as fh:
        fh.write(json.dumps(event) + "\n")
    return event


def load(path: Path | None = None) -> list[dict]:
    path = path or EVENTS
    if not path.exists():
        return []
    return [json.loads(line) for line in path.read_text().splitlines() if line.strip()]


def summarize(events: list[dict]) -> dict:
    start = next((e for e in events if e["kind"] == "start"), None)
    if start is None:
        raise ValueError("no start event; run sprint_bench.py start first")
    t0 = start["t"]
    scores = [e for e in events if e["kind"] == "score"]
    matches = [e for e in scores if e.get("verdict") == "MATCH"]
    matched_names = list(dict.fromkeys(e["name"] for e in matches))
    steps: dict[str, float] = {}
    for e in events:
        if e["kind"] == "time":
            steps[e["label"]] = steps.get(e["label"], 0) + e["seconds"]
    finish = next((e for e in reversed(events) if e["kind"] == "finish"), None)
    end = (finish or events[-1])["t"]
    out = {
        "run_id": start.get("run_id"), "model": start.get("model"), "lane": start.get("lane"),
        "unit": (finish or {}).get("unit") or start.get("unit"),
        "started_at": t0, "wall_seconds": round(end - t0, 1),
        "startup": {k: start[k] for k in ("worktree_seconds", "agent_boot_seconds", "launch_to_start_seconds")
                    if start.get(k) is not None},
        "steps_seconds": {k: round(v, 1) for k, v in steps.items()},
        "marks": {e["label"]: round(e["t"] - t0, 1) for e in events if e["kind"] == "mark"},
        "attempts": len(scores),
        "compile_errors": sum(1 for e in scores if e.get("verdict") == "ERROR"),
        "functions_scored": len({e["name"] for e in scores}),
        "first_score_seconds": round(scores[0]["t"] - t0, 1) if scores else None,
        "first_match_seconds": round(matches[0]["t"] - t0, 1) if matches else None,
        "attempt_matches": len(matched_names),
        "attempts_per_match": round(len(scores) / len(matched_names), 1) if matched_names else None,
    }
    if finish:
        out.update({k: finish[k] for k in ("verified", "verified_bytes", "parked", "deferred", "code_bytes_delta",
                                           "active_seconds", "phases", "tool_commits") if k in finish})
        active = finish.get("active_seconds") or out["wall_seconds"]
        if active:
            out["verified_per_active_hour"] = round(finish.get("verified", 0) * 3600 / active, 1)
            if "verified_bytes" in finish:
                out["verified_bytes_per_active_hour"] = round(finish["verified_bytes"] * 3600 / active)
    return out


def start(model: str, run_id: str | None, launched_at: float | None, now: float | None = None,
          lane: str | None = None, unit: str | None = None) -> dict:
    now = time.time() if now is None else now
    if EVENTS.exists():
        EVENTS.rename(EVENTS.with_name(f"sprint_bench.{int(EVENTS.stat().st_mtime)}.jsonl"))
    data = {"model": model, "run_id": run_id or f"{ROOT.name}-{int(now)}"}
    data.update({k: v for k, v in (("lane", lane), ("unit", unit)) if v})
    git_file = ROOT / ".git"
    if launched_at:
        data["launch_to_start_seconds"] = round(now - launched_at, 1)
        if git_file.is_file():  # a linked worktree; its .git file is written at creation
            created = git_file.stat().st_mtime
            data["worktree_seconds"] = round(max(0.0, created - launched_at), 1)
            data["agent_boot_seconds"] = round(now - max(created, launched_at), 1)
    return append("start", now=now, **data)


def finish(record: dict) -> Path | None:
    """Append the finish event and copy the run summary to the shared bench cache."""
    if append("finish", **record) is None:
        return None
    summary = summarize(load())
    out = cache_dir() / f"{summary['run_id']}.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(summary, indent=2) + "\n")
    return out


COMPARE_KEYS = ("run_id", "model", "lane", "unit", "started_at", "verified", "verified_bytes",
                "code_bytes_delta", "parked", "active_seconds", "verified_per_active_hour",
                "verified_bytes_per_active_hour", "attempts_per_match", "first_match_seconds",
                "startup", "steps_seconds")


def compare(directory: Path | None = None) -> list[dict]:
    """Finished runs from the shared bench cache, oldest first; absent fields read as None."""
    rows = []
    for path in sorted((directory or cache_dir()).glob("*.json")):
        try:
            s = json.loads(path.read_text())
        except (OSError, ValueError):
            continue
        rows.append({k: s.get(k) for k in COMPARE_KEYS})
    return sorted(rows, key=lambda r: r.get("started_at") or 0)


def current_start(path: Path | None = None) -> dict | None:
    """This checkout's open run: its start event, or None when none was started or it finished."""
    events = load(path)
    start_event = next((e for e in events if e["kind"] == "start"), None)
    if start_event is None or any(e["kind"] == "finish" for e in events):
        return None
    return start_event


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("start", help="begin a run's event log")
    s.add_argument("--model", required=True, help="exact model id the launcher selected")
    s.add_argument("--run-id")
    s.add_argument("--launched-at", type=float, help="epoch seconds when the thread was launched")
    s.add_argument("--lane", help="what the run works on: sweep, near-misses, unit, workflow")
    s.add_argument("--unit", help="the unit a unit sprint works on")
    t = sub.add_parser("time", help="run a command and record its duration")
    t.add_argument("label")
    t.add_argument("command", nargs=argparse.REMAINDER)
    m = sub.add_parser("mark", help="record a milestone")
    m.add_argument("label")
    sub.add_parser("report", help="summarize this checkout's run")
    sub.add_parser("compare", help="list finished runs from the shared bench cache")
    args = ap.parse_args(argv)
    if args.cmd == "start":
        print(json.dumps(start(args.model, args.run_id, args.launched_at, lane=args.lane, unit=args.unit),
                         indent=2))
    elif args.cmd == "time":
        cmd = args.command[1:] if args.command[:1] == ["--"] else args.command
        if not cmd:
            ap.error("time needs a command after --")
        t0 = time.time()
        code = subprocess.call(cmd, cwd=ROOT)
        secs = round(time.time() - t0, 1)
        append("time", label=args.label, seconds=secs, returncode=code)
        print(f"bench: {args.label} took {secs}s (exit {code})", file=sys.stderr)
        return code
    elif args.cmd == "mark":
        if append("mark", label=args.label) is None:
            ap.error("no run started; run sprint_bench.py start first")
    elif args.cmd == "report":
        try:
            print(json.dumps(summarize(load()), indent=2))
        except ValueError as exc:
            ap.error(str(exc))
    else:
        print(json.dumps(compare(), indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
