#!/usr/bin/env python3
"""Start here: checkout and sweep state, lanes with available bytes, and the command to begin.

    python3 tools/kickoff.py                      # read-only report, a few seconds
    python3 tools/kickoff.py --no-remote          # without the GitHub open-PR ownership query
    python3 tools/kickoff.py --start UNIT [FN ...]           # unit sprint: audit, queue, baseline, bench
    python3 tools/kickoff.py --start-lane sweep|near-misses  # the same for an automatic lane

Recommendation rule:
1. The sweep is missing or stale (a commit touched src/, include/ or the sweep tools since
   it ran) -> rerun it first: it collects the cheapest bytes and refreshes every other figure.
2. Else unclaimed near misses (>=90% exact) -> near-misses; else the top unit by first-wave bytes.
   When finished runs measured verified bytes per active hour for both, the higher rate wins.
3. The top workflow blocker is printed as the alternative when its >=80% bytes exceed the near misses'.

`--start` never builds: it refuses when the checkout is behind origin/main (`--allow-behind`)
or the build is missing, prints ownership conflicts, saves the queue and baseline through
`sprint_finish.record_baseline` and opens a `sprint_bench` run with lane, unit and model
(`--model`, else `$LSW1_MODEL`). Rerunning it for the same lane and unit only adds new queue
names; `--restart` replaces another lane's baseline. `sprint_finish.py` closes the bench run.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import next_target as nt  # noqa: E402
import sprint_bench as sb  # noqa: E402
import sprint_finish as sf  # noqa: E402
import sprint_metrics as metrics  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build" / "GL5E4F"
BLOCKERS = ROOT / "docs" / "sweep_blockers.md"
# Inputs whose change can move a sweep verdict.
SWEEP_INPUTS = ("src", "include", ":(glob)tools/sweep*.py", "tools/sweep_repairs", "tools/patches")
BANDS = ((0.9, ">=90"), (0.8, "80-90"), (0.5, "50-80"), (0.0, "<50"))
NEAR, DRAFT = 0.9, 0.8
QUEUE_LIMIT = 40
LANES = ("sweep", "near-misses", "unit")


def git(*args: str) -> str:
    out = subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True)
    return out.stdout.strip() if out.returncode == 0 else ""


def default_sweep_dir() -> Path:
    return Path(os.environ.get("LSW1_SWEEP_DIR") or ROOT / "work" / "sweep")


# --- checkout -------------------------------------------------------------------------------

def build_state() -> dict:
    """What scoring needs: generated target assembly (sweep, score_fn) and the report and DOL
    (sprint_finish baseline)."""
    asm = BUILD / "asm"
    state = {"asm": asm.is_dir() and any(asm.rglob("*.s")), "report": (BUILD / "report.json").is_file(),
             "dol": (BUILD / "main.dol").is_file()}
    state["ready"] = all(state.values())
    return state


def report_figures() -> dict | None:
    try:
        import progress
        return progress.read_report(BUILD / "report.json")
    except Exception:  # an unreadable report reads as absent
        return None


def checkout_state() -> dict:
    counts = git("rev-list", "--left-right", "--count", "origin/main...HEAD").split()
    behind, ahead = (int(counts[0]), int(counts[1])) if len(counts) == 2 else (None, None)
    fetch_head = Path(git("rev-parse", "--path-format=absolute", "--git-common-dir") or ROOT / ".git") / "FETCH_HEAD"
    return {"branch": git("branch", "--show-current") or "detached", "head": git("rev-parse", "--short", "HEAD"),
            "behind": behind, "ahead": ahead,
            "fetched_hours": (time.time() - fetch_head.stat().st_mtime) / 3600 if fetch_head.exists() else None,
            "build": build_state(), "report": report_figures()}


# --- sweep ----------------------------------------------------------------------------------

def load_sweep(directory: Path) -> tuple[list[dict] | None, dict | None, float | None]:
    """(results rows, meta.json sidecar, results.json mtime); None for whatever is absent."""
    results, meta = directory / "results.json", directory / "meta.json"
    rows = mtime = info = None
    try:
        rows = json.loads(results.read_text())
        mtime = results.stat().st_mtime
    except (OSError, ValueError):
        pass
    try:
        info = json.loads(meta.read_text())
    except (OSError, ValueError):
        pass
    return rows, info, mtime


def changed_commits(*rev_args: str) -> list[set[str]] | None:
    """Paths each matching commit changed under the sweep inputs, newest first (None on error)."""
    out = subprocess.run(["git", "log", "--format=%x00%H", "--name-only", *rev_args, "--", *SWEEP_INPUTS],
                         cwd=ROOT, capture_output=True, text=True)
    if out.returncode:
        return None
    return [{l for l in chunk.splitlines()[1:] if l.strip()} for chunk in out.stdout.split("\0")[1:]]


def commits_since(meta: dict | None, mtime: float | None, applied_sources: set[str] = frozenset()
                  ) -> tuple[int | None, str]:
    """Commits on HEAD touching the sweep inputs since the swept commit (meta.json), else since
    results.json was written. A commit that changed only sources the sweep applied matches to
    is its own output landing and does not count. (None, reason) when neither is known."""
    head = (meta or {}).get("head")
    if head and git("cat-file", "-t", head) == "commit":
        commits, basis = changed_commits(f"{head}..HEAD"), f"since {head[:8]}"
    elif mtime is not None:
        commits, basis = changed_commits(f"--since=@{int(mtime)}", "HEAD"), "since results.json was written"
    else:
        return None, "no sweep"
    if commits is None:
        return None, basis
    return sum(1 for paths in commits if not paths <= applied_sources), basis


def sweep_status(rows, meta, mtime, now: float | None = None) -> dict:
    """missing / stale / fresh, with the reason."""
    if rows is None:
        return {"state": "missing", "why": "no results.json"}
    age = ((now or time.time()) - mtime) / 3600 if mtime else None
    if any(r.get("verdict") == "NO-ASM" for r in rows):
        return {"state": "stale", "why": "NO-ASM rows (swept without a build)", "age_hours": age}
    applied = {r["source"] for r in rows if r.get("applied") and r.get("source")}
    n, basis = commits_since(meta, mtime, applied)
    if n is None:
        return {"state": "stale", "why": "swept commit unknown", "age_hours": age}
    if n:
        return {"state": "stale", "why": f"{n} commit(s) to sweep inputs {basis}", "age_hours": age}
    return {"state": "fresh", "why": f"no commits to sweep inputs {basis}", "age_hours": age}


def closeness(row: dict) -> float:
    if row.get("closeness") is not None:
        return float(row["closeness"])
    return row["exact"] / row["target"] if row.get("target") else 0.0


def verdict_mix(rows: list[dict]) -> dict[str, int]:
    mix: dict[str, int] = {}
    for r in rows:
        mix[r.get("verdict", "?")] = mix.get(r.get("verdict", "?"), 0) + 1
    return dict(sorted(mix.items(), key=lambda kv: -kv[1]))


def bands(rows: list[dict]) -> list[tuple[str, int, int]]:
    """(band, functions, bytes) for MISMATCH rows by exact-instruction ratio."""
    out = {label: [0, 0] for _, label in BANDS}
    for r in rows:
        if r.get("verdict") != "MISMATCH":
            continue
        c = closeness(r)
        label = next(label for low, label in BANDS if c >= low)
        out[label][0] += 1
        out[label][1] += r.get("size", 0)
    return [(label, *out[label]) for _, label in BANDS]


def available(rows: list[dict], low: float, high: float, ready: set[str] | None,
              owned: set[str]) -> list[dict]:
    """MISMATCH rows in [low, high) that are still undone, unclaimed and in no owned unit,
    largest size x ratio first. READY None skips the done/claimed filter."""
    out = [r for r in rows if r.get("verdict") == "MISMATCH" and not r.get("applied")
           and low <= closeness(r) < high and r.get("unit") not in owned
           and (ready is None or r["name"] in ready)]
    return sorted(out, key=lambda r: -r.get("size", 0) * closeness(r))


def top_units(rows: list[dict], n: int = 3) -> list[tuple[str, int]]:
    by: dict[str, int] = {}
    for r in rows:
        by[r["unit"]] = by.get(r["unit"], 0) + r.get("size", 0)
    return sorted(by.items(), key=lambda kv: -kv[1])[:n]


# --- workflow blockers ----------------------------------------------------------------------

def _cells(line: str) -> list[str]:
    return [c.strip() for c in line.strip().strip("|").split("|")]


def _int(text: str) -> int | None:
    m = re.search(r"\d[\d,]*", text.replace("_", ""))
    return int(m.group().replace(",", "")) if m else None


def read_blockers(path: Path, n: int = 3) -> list[dict] | None:
    """Top rows of the "Ranked by bytes" table in docs/sweep_blockers.md, or None when the
    file or table is absent. Each row: its cells and, when a column header names 80%, its
    bytes at >=80% exact."""
    try:
        lines = path.read_text().splitlines()
    except OSError:
        return None
    start = next((i for i, l in enumerate(lines) if re.search(r"ranked by bytes", l, re.I)), None)
    if start is None:
        return None
    table = []
    for line in lines[start + 1:]:
        if line.lstrip().startswith("|"):
            table.append(line)
        elif table or line.startswith("#"):
            break
    if len(table) < 3:
        return None
    header = _cells(table[0])
    col80 = next((i for i, h in enumerate(header) if "80" in h), None)
    cols = {h.lower(): i for i, h in enumerate(header)}
    out = []
    for line in table[2:2 + n]:
        cells = _cells(line)

        def cell(i):
            return _int(cells[i]) if i is not None and i < len(cells) else None
        out.append({"cells": cells, "header": header, "name": cells[0].strip("`") if cells else "?",
                    "functions": cell(cols.get("functions")), "bytes": cell(cols.get("bytes")),
                    "bytes80": cell(col80)})
    return out


# --- measured yield -------------------------------------------------------------------------

def lane_of(rec: dict) -> str | None:
    """A record's lane; records from before the field were unit sprints when they name a unit."""
    return rec.get("lane") or ("unit" if rec.get("unit") else None)


def lane_rates(records: list[dict]) -> dict[str, float]:
    """Verified bytes per active hour per lane, over records that measured verified bytes."""
    tot: dict[str, list[float]] = {}
    for rec in records:
        lane, active = lane_of(rec), rec.get("active_seconds")
        if lane and active and rec.get("verified_bytes") is not None:
            t = tot.setdefault(lane, [0.0, 0.0])
            t[0] += rec["verified_bytes"]
            t[1] += active
    return {lane: b * 3600 / s for lane, (b, s) in tot.items() if s}


def yield_lines(records: list[dict], n: int = 3) -> list[str]:
    if not records:
        return ["no measurement (no finished bench runs in ~/.cache/lsw1/bench)"]
    out = []
    for rec in records[-n:]:
        when = time.strftime("%Y-%m-%d", time.localtime(rec["started_at"])) if rec.get("started_at") else "?"
        active = rec.get("active_seconds")
        head = (f"{when} {lane_of(rec) or 'lane?'} {rec.get('unit') or '-'} {rec.get('model') or 'model?'}: "
                f"{rec.get('verified') or 0} verified")
        if not active:
            out.append(head + " (no active time recorded)")
            continue
        hours = active / 3600
        line = f"{head} in {hours:.1f} active h = {(rec.get('verified') or 0) / hours:.0f} fn/h"
        if rec.get("verified_bytes") is not None:
            line += f", {rec['verified_bytes']:,} B = {rec['verified_bytes'] / hours:,.0f} B/h"
        elif rec.get("code_bytes_delta") is not None:
            line += f"; verified bytes not recorded (report delta {rec['code_bytes_delta']:,} B, may include main drift)"
        out.append(line)
    return out


# --- recommendation -------------------------------------------------------------------------

def recommend(sweep: dict, near: dict, unit: dict | None, blocker: dict | None,
              rates: dict[str, float]) -> tuple[str, str, str | None]:
    """(lane, why, alternative or None) by the rule in the module docstring."""
    alt = None
    if (sweep["state"] != "missing" and blocker and blocker.get("bytes80") is not None
            and blocker["bytes80"] > near["bytes"]):
        alt = (f"workflow fix `{blocker.get('name') or blocker['cells'][0]}`: {blocker['bytes80']:,} B at >=80% exceeds the "
               f"near misses' {near['bytes']:,} B")
    if sweep["state"] != "fresh":
        return "sweep", f"sweep is {sweep['state']} ({sweep['why']}); rerunning collects the cheapest bytes", alt
    if near["functions"] and unit and "near-misses" in rates and "unit" in rates:
        lane = max(("near-misses", "unit"), key=lambda k: rates[k])
        return lane, (f"measured {rates[lane]:,.0f} verified B/active h vs "
                      f"{rates['unit' if lane == 'near-misses' else 'near-misses']:,.0f}"), alt
    if near["functions"]:
        return "near-misses", f"{near['functions']} unclaimed near misses, {near['bytes']:,} B", alt
    if unit:
        return "unit", f"no unclaimed near misses; top unit {unit['unit']} ({unit['wave_bytes']:,} B first wave)", alt
    if blocker:
        return "workflow", "no near misses or ready units", None
    return "none", "no near misses, ready units or ranked blockers", None


# --- report ---------------------------------------------------------------------------------

def gather(sweep_dir: Path, remote: bool) -> dict:
    rows, meta, mtime = load_sweep(sweep_dir)
    try:
        ready_rows, _ = nt.collect(max_size=0x100000)
    except nt.TargetError as exc:
        ready_rows, ready_error = None, str(exc)
    else:
        ready_error = None
    ready = {r["name"] for r in ready_rows} if ready_rows is not None else None
    owned = nt.owned_units(ready_rows, remote=remote) if ready_rows else set()
    units = []
    if ready_rows is not None:
        small = [r for r in ready_rows if r["size"] <= 0x200]
        ranked, _ = nt.rank_units(small, owned, 3)
        units = [{"unit": u, "ready": len(rs), "left": rs[0]["unit_left"],
                  "wave_bytes": sum(r["size"] for r in rs[:nt.WAVE_SIZE])} for u, rs in ranked]
    return {"checkout": checkout_state(), "sweep_dir": sweep_dir, "rows": rows,
            "sweep": sweep_status(rows, meta, mtime), "meta": meta, "ready": ready,
            "ready_error": ready_error, "owned": owned, "units": units,
            "blockers": read_blockers(BLOCKERS), "records": sb.compare(), "worktrees": worktree_summary()}


def worktree_summary() -> str | None:
    """One line from worktree_cleanup's read-only audit, or None when it fails."""
    try:
        import worktree_cleanup as wc
        common = Path(wc.git("rev-parse", "--path-format=absolute", "--git-common-dir", cwd=ROOT))
        main_checkout = common.parent
        trees = wc.worktrees(main_checkout)
        lines = wc.audit(trees, main_checkout, ROOT.resolve(), main_checkout / "work" / "archive" / "worktrees")
    except Exception:
        return None
    items = [l[2:] for l in lines if l.startswith("- ")]
    kinds: dict[str, int] = {}
    for item in items:
        kind = item.split(":", 1)[0]
        kinds[kind] = kinds.get(kind, 0) + 1
    parked = 0
    for l in lines:
        cells = _cells(l) if l.startswith("| ") and not l.startswith("| worktree") else []
        if len(cells) >= 7 and cells[6].isdigit():
            parked += int(cells[6])
    detail = ", ".join(f"{v} {k}" for k, v in kinds.items()) or "none need attention"
    return f"{len(trees)} worktrees: {detail}; {parked} parked notes"


def lane_table(state: dict) -> tuple[list[str], dict, dict | None, dict | None]:
    rows, ready, owned = state["rows"] or [], state["ready"], state["owned"]
    sweep = state["sweep"]
    near = available(rows, NEAR, 1.01, ready, owned)
    drafts = available(rows, DRAFT, NEAR, ready, owned)
    near_lane = {"functions": len(near), "bytes": sum(r["size"] for r in near)}
    lines = []
    age = f", {sweep['age_hours']:.0f} h old" if sweep.get("age_hours") is not None else ""
    word = "fresh" if sweep["state"] == "fresh" else f"{sweep['state']}: rerun"
    lines.append(f"  sweep        {word} ({sweep['why']}{age})  ->  python3 tools/sweep.py --apply")

    def units_of(rs):
        return ", ".join(f"{u} {b:,}" for u, b in top_units(rs)) or "-"
    filt = "" if ready is not None else " (unfiltered: ready list unavailable)"
    if state["rows"] is None:
        filt = " (no sweep yet)"
    lines.append(f"  near-misses  {len(near)} fn, {near_lane['bytes']:,} B at >=90%{filt}; top {units_of(near)}"
                 f"  ->  python3 tools/sweep_permute.py --apply, or python3 tools/kickoff.py --start-lane near-misses")
    hint = (f"python3 tools/score_fn.py {drafts[0]['name']} {drafts[0]['source']} "
            f"--body {state['sweep_dir'] / drafts[0]['name'] / 'sweep_draft.c'} --diff") if drafts else "-"
    lines.append(f"  drafts 80-90 {len(drafts)} fn, {sum(r['size'] for r in drafts):,} B; top {units_of(drafts)}"
                 f"  ->  {hint}")
    if state["units"]:
        for i, u in enumerate(state["units"], 1):
            lines.append(f"  unit #{i}      {u['unit']}: {u['ready']} ready, {u['left']} left, "
                         f"{u['wave_bytes']:,} B first wave  ->  python3 tools/kickoff.py --start {u['unit']}")
    else:
        lines.append(f"  unit         {state['ready_error'] or 'no ready units'}")
    blockers = state["blockers"]
    if blockers is None:
        lines.append("  workflow     no ranked blockers  ->  run python3 tools/sweep_report.py")
    for i, b in enumerate(blockers or [], 1):
        figures = [f"{b[k]:,} {unit_}" for k, unit_ in (("functions", "fn"), ("bytes", "B")) if b.get(k) is not None]
        if b.get("bytes80") is not None:
            figures.append(f"{b['bytes80']:,} B at >=80%")
        name = b.get("name") or b["cells"][0]
        lines.append(f"  workflow #{i}  {name}: {', '.join(figures) or ' · '.join(b['cells'][1:4])}"
                     f"  ->  python3 tools/sweep_report.py --class {name}")
    wt = state["worktrees"]
    lines.append(f"  worktrees    {wt or 'audit unavailable'}  ->  python3 tools/worktree_cleanup.py --audit")
    unit = state["units"][0] if state["units"] else None
    return lines, near_lane, unit, (blockers or [None])[0]


def print_report(state: dict) -> None:
    c = state["checkout"]
    rel = (f"{c['behind']} behind, {c['ahead']} ahead of origin/main" if c["behind"] is not None
           else "origin/main unknown")
    fetched = f" (fetched {c['fetched_hours']:.0f} h ago)" if c["fetched_hours"] is not None else ""
    b = c["build"]
    build = "ready" if b["ready"] else ("missing " + ", ".join(k for k in ("asm", "report", "dol") if not b[k])
                                       + ": run bash build.sh")
    print(f"Checkout: {c['branch']} @ {c['head']}, {rel}{fetched}; build {build}")
    rep = c["report"]
    if rep:
        print(f"Report:   {int(rep.get('matched_functions', 0)):,}/{int(rep.get('total_functions', 0)):,} functions, "
              f"{int(float(rep.get('matched_code', 0))):,}/{int(rep.get('total_code', 0)):,} code bytes "
              "(last build/GL5E4F/report.json)")
    else:
        print("Report:   none (bash build.sh writes it)")
    rows = state["rows"]
    if rows is None:
        print(f"Sweep:    none at {state['sweep_dir']}")
    else:
        mix = ", ".join(f"{k} {v}" for k, v in verdict_mix(rows).items())
        print(f"Sweep:    {state['sweep']['state']}, {len(rows):,} rows ({mix})")
        print("          MISMATCH bands: " + "; ".join(f"{label} {n} fn/{size:,} B"
                                                      for label, n, size in bands(rows)))
    lines, near_lane, unit, blocker = lane_table(state)
    print("\nLanes:")
    print("\n".join(lines))
    print("\nMeasured yield (verified per active hour, sprint_bench):")
    print("\n".join("  " + l for l in yield_lines(state["records"])))
    lane, why, alt = recommend(state["sweep"], near_lane, unit, blocker, lane_rates(state["records"]))
    first = "" if c["build"]["ready"] else "bash build.sh first (scoring needs it), then "
    print(f"\nRecommendation: {first}{lane}: {why}.")
    if alt:
        print(f"Alternative:    {alt}.")


# --- start ----------------------------------------------------------------------------------

def build_queue(lane: str, unit: str | None, names: list[str], rows: list[dict] | None,
                ready_rows: list[dict] | None, owned: set[str]) -> list[str]:
    """Given names, else the lane's default queue (docs/sprint_workflow.md#kickoff)."""
    if names or lane == "sweep":
        return metrics.dedupe(names)
    ready = {r["name"] for r in ready_rows} if ready_rows is not None else None
    if lane == "near-misses":
        return [r["name"] for r in available(rows or [], NEAR, 1.01, ready, owned)][:QUEUE_LIMIT]
    near = [r["name"] for r in available(rows or [], NEAR, 1.01, ready, set()) if r["unit"] == unit]
    fill = [r["name"] for r in ready_rows or [] if r["unit"] == unit and r["size"] <= 0x200
            and not r["claimed"]][:QUEUE_LIMIT]
    return metrics.dedupe(near + fill)


def audit_conflicts(unit: str | None, owned: set[str]) -> list[str]:
    out = []
    if unit and unit in owned:
        out.append(f"unit {unit} is owned by another active t3/sprint-* branch or PR")
    summary = worktree_summary()
    if summary:
        out.append(f"worktrees: {summary} (details: python3 tools/worktree_cleanup.py --audit)")
    return out


def start(lane: str, unit: str | None, names: list[str], args) -> int:
    if not args.allow_behind:
        behind = sf.behind_origin()
        if behind:
            print(sf.BEHIND_ERROR.format(behind), file=sys.stderr)
            return 2
    build = build_state()
    if not build["ready"]:
        missing = ", ".join(k for k in ("asm", "report", "dol") if not build[k])
        print(f"error: build output missing ({missing}); run bash build.sh, then rerun kickoff", file=sys.stderr)
        return 2
    try:
        ready_rows, _ = nt.collect(max_size=0x100000)
    except nt.TargetError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2
    owned = nt.owned_units(ready_rows, remote=not args.no_remote)
    conflicts = audit_conflicts(unit, owned)
    for line in conflicts:
        print(f"ownership: {line}")
    if unit and conflicts and conflicts[0].startswith("unit ") and not args.force:
        print("error: another sprint owns this unit; pick another or pass --force", file=sys.stderr)
        return 2
    rows, _, _ = load_sweep(args.sweep_dir)
    queue = build_queue(lane, unit, names, rows, ready_rows, owned)
    tag = {"lane": lane, "unit": unit}
    base = json.loads(sf.BASELINE.read_text()) if sf.BASELINE.is_file() else None
    if base and {k: base.get(k) for k in tag} == tag and not args.restart:
        added = metrics.queue_add(sf.BASELINE, queue) or []
        print(f"baseline: kept ({lane} {unit or ''} at {base.get('head')}); queue +{len(added)}")
    elif base and not args.restart:
        print(f"error: {sf.BASELINE.relative_to(ROOT)} belongs to lane {base.get('lane')} "
              f"{base.get('unit') or ''}; finish it first or pass --restart", file=sys.stderr)
        return 2
    else:
        code = sf.record_baseline(queue, allow_behind=True, extra=tag)
        if code:
            return code
    timing = ROOT / "work" / "sprint_timing.json"
    if not timing.exists():
        sf.phase(timing, "matching")
    current = sb.current_start()
    if current and current.get("lane") == lane and current.get("unit") == unit and not args.restart:
        print(f"bench: run {current.get('run_id')} already open")
    else:
        opened = sb.start(args.model, None, args.launched_at, lane=lane, unit=unit)
        print(f"bench: opened run {opened['run_id']} (lane {lane}, model {args.model})")
    print(f"queue: {len(queue)} names" + (f": {' '.join(queue[:12])}{' ...' if len(queue) > 12 else ''}"
                                          if queue else ""))
    print("\nNext:")
    if lane == "sweep":
        print("  python3 tools/sweep.py --apply && bash build.sh")
        print("  python3 tools/sprint_finish.py FN... --unit sweep --queue FN... --merge")
    else:
        src = f"src/{unit}.c" if unit else "src/UNIT.c"
        print(f"  python3 tools/score_fn.py {queue[0] if queue else 'NAME'} {src} --diff")
        print(f"  python3 tools/sprint_finish.py --unit {src} --matched-from-diff --merge "
              "--parked 'FN=score: mismatch; next: idea' --deferred FN ...")
    return 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--start", nargs="+", metavar="UNIT [FN]", help="start a unit sprint")
    ap.add_argument("--start-lane", choices=["sweep", "near-misses"], help="start an automatic lane")
    ap.add_argument("--no-remote", action="store_true", help="do not ask GitHub for open sprint PRs")
    ap.add_argument("--sweep-dir", type=Path, default=None,
                    help="sweep results to read (default $LSW1_SWEEP_DIR or work/sweep)")
    ap.add_argument("--model", default=os.environ.get("LSW1_MODEL") or "unknown",
                    help="model id for the bench record (default $LSW1_MODEL)")
    ap.add_argument("--launched-at", type=float, help="epoch seconds the thread was launched (bench startup split)")
    ap.add_argument("--allow-behind", action="store_true", help="start even when behind origin/main")
    ap.add_argument("--restart", action="store_true", help="replace an existing baseline and bench run")
    ap.add_argument("--force", action="store_true", help="start a unit another sprint branch owns")
    args = ap.parse_args(argv)
    args.sweep_dir = args.sweep_dir or default_sweep_dir()
    if args.start and args.start_lane:
        ap.error("choose --start or --start-lane")
    if args.start:
        return start("unit", args.start[0].removeprefix("src/").removesuffix(".c"), args.start[1:], args)
    if args.start_lane:
        return start(args.start_lane, None, [], args)
    print_report(gather(args.sweep_dir, remote=not args.no_remote))
    return 0


if __name__ == "__main__":
    sys.exit(main())
