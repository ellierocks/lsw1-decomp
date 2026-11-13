#!/usr/bin/env python3
"""Workflow metrics: how fast and how well agents match, from the bench event logs.

    python3 tools/workflow_metrics.py report                  # every discovered run, one row each
    python3 tools/workflow_metrics.py report --since 2026-11-12T18:00-08:00 --json
    python3 tools/workflow_metrics.py cohorts                 # named cohorts from config/workflow_metrics.toml
    python3 tools/workflow_metrics.py compare                 # before -> after table for each comparison
    python3 tools/workflow_metrics.py compare --svg docs/img/workflow_metrics.svg
    python3 tools/workflow_metrics.py archive                 # copy discovered logs into the bench cache

The metric set and why each one is actionable is in docs/workflow_metrics.md. Everything is
computed from `score`, `time`, `start` and `finish` events that `score_fn.py`, `sprint_bench.py`
and `sprint_finish.py` already write to `work/sprint_bench*.jsonl`. Logs are found in this
checkout, every worktree `git worktree list` knows, siblings in the worktree parent directory and
`<bench cache>/events/` (where `sprint_bench.finish` and `archive` copy them, so they outlive
worktree cleanup). One log per run id is kept: the one with the most events.

A *segment* is one run's events inside an optional time window, so one log that several agents
worked in (a child replaced mid-run) can be split. Active time inside a segment is the sum of
gaps between consecutive events, each capped at the sprint idle cap (15 minutes), counted from
`since` when one is given, else from the run's start event.

Score events written before 2026-11-13 lack `source`, `seconds` and `codegen`; metrics that need
them read as unknown (None) or fall back to a labelled proxy, never a guess. A `MATCH` here is
`score_fn.py`'s strict verdict during the run, not the final `sprint_finish.py` verification.
"""
from __future__ import annotations

import argparse
import json
import statistics
import subprocess
import sys
import tomllib
from collections import Counter
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import sprint_bench as sb  # noqa: E402
import sprint_metrics  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
CONFIG = ROOT / "config" / "workflow_metrics.toml"
IDLE_CAP = sprint_metrics.DEFAULT_IDLE_CAP
HYPOTHESIS_FLOOR = sprint_metrics.PARK_FLOOR  # distinct candidates before a park is credible


@dataclass(frozen=True)
class Metric:
    key: str
    label: str
    unit: str
    better: str  # "higher" or "lower"


METRICS = (
    Metric("matched_bytes_per_hour", "New matched bytes per child-hour", "B/h", "higher"),
    Metric("exact_gain_per_hour", "Exact-instruction gain per child-hour", "insn/h", "higher"),
    Metric("compile_error_rate", "Compile-error rate of scored attempts", "%", "lower"),
    Metric("distinct_per_unfinished", "Distinct compiled candidates per unfinished function", "", "higher"),
    Metric("first_compile_seconds", "Start to first compiling score", "s", "lower"),
    Metric("park_recovery_rate", "Unfinished functions a later pass matched", "%", "lower"),
    Metric("score_seconds", "score_fn latency (median)", "s", "lower"),
    Metric("build_seconds", "Finish build latency (median)", "s", "lower"),
)
BY_KEY = {m.key: m for m in METRICS}


# ------------------------------------------------------------------ discovery


def worktree_roots(root: Path = ROOT) -> list[Path]:
    """This checkout, every worktree git lists, and siblings in the worktree parent directory."""
    roots = [root]
    try:
        out = subprocess.run(["git", "worktree", "list", "--porcelain"], cwd=root, capture_output=True,
                             text=True, timeout=30).stdout
        roots += [Path(line[9:]) for line in out.splitlines() if line.startswith("worktree ")]
    except (OSError, subprocess.SubprocessError):
        pass
    roots += [p for p in root.parent.iterdir() if p.is_dir()] if root.parent.is_dir() else []
    return list(dict.fromkeys(p.resolve() for p in roots))


def log_paths(roots: list[Path] | None = None, cache: Path | None = None) -> list[Path]:
    paths = []
    for r in roots if roots is not None else worktree_roots():
        paths += sorted((r / "work").glob("sprint_bench*.jsonl"))
    events_dir = cache if cache is not None else sb.cache_dir() / "events"
    if events_dir.is_dir():
        paths += sorted(events_dir.glob("*.jsonl"))
    return paths


def load_runs(paths: list[Path]) -> dict[str, list[dict]]:
    """run id -> events, keeping the longest log when the same run appears twice."""
    runs: dict[str, list[dict]] = {}
    for path in paths:
        try:
            events = sb.load(path)
        except (OSError, ValueError):
            continue
        start = next((e for e in events if e.get("kind") == "start"), None)
        if not start or not start.get("run_id"):
            continue
        if len(events) > len(runs.get(start["run_id"], [])):
            runs[start["run_id"]] = events
    return runs


# ------------------------------------------------------------------ segments


def parse_time(value) -> float | None:
    if value in (None, ""):
        return None
    if isinstance(value, (int, float)):
        return float(value)
    if isinstance(value, datetime):
        return value.timestamp()
    return datetime.fromisoformat(str(value)).timestamp()


real_score = sprint_metrics.real_score


@dataclass
class Segment:
    run_id: str
    events: list[dict]
    since: float | None = None
    until: float | None = None
    start: dict = field(default_factory=dict)

    @classmethod
    def of(cls, run_id: str, events: list[dict], since=None, until=None) -> "Segment":
        lo, hi = parse_time(since), parse_time(until)
        start = next((e for e in events if e.get("kind") == "start"), {})
        inside = [e for e in events if (lo is None or e["t"] >= lo) and (hi is None or e["t"] <= hi)]
        return cls(run_id, inside, lo, hi, start)

    @property
    def origin(self) -> float | None:
        """When the window's clock starts: `since`, else the start event, else the first event."""
        if self.since is not None:
            return self.since
        if self.start and self.start in self.events:
            return self.start["t"]
        return self.events[0]["t"] if self.events else None

    @property
    def end(self) -> float | None:
        return self.events[-1]["t"] if self.events else None

    def active_seconds(self, cap: float = IDLE_CAP) -> float:
        times = sorted(([self.origin] if self.origin is not None else []) + [e["t"] for e in self.events])
        return sum(min(b - a, cap) for a, b in zip(times, times[1:]))

    def scores(self) -> list[dict]:
        return [e for e in self.events if real_score(e)]


def function_stats(scores: list[dict]) -> dict[str, dict]:
    """Per function: first verdict, whether it reached MATCH, exact gain and distinct candidates."""
    out: dict[str, dict] = {}
    for e in scores:
        f = out.setdefault(e["name"], {"first": e.get("verdict"), "matched": False, "target": 0,
                                        "first_exact": None, "best_exact": 0, "attempts": 0,
                                        "errors": 0, "codegen": set(), "exacts": set(),
                                        "all_hashed": True})
        f["attempts"] += 1
        if e.get("verdict") == "ERROR":
            f["errors"] += 1
            continue
        exact = e.get("exact") or 0
        f["target"] = max(f["target"], e.get("target") or 0)
        if f["first_exact"] is None:
            f["first_exact"] = exact
        f["best_exact"] = max(f["best_exact"], exact)
        f["exacts"].add(exact)
        if e.get("codegen"):
            f["codegen"].add(e["codegen"])
        else:
            f["all_hashed"] = False
        if e.get("verdict") == "MATCH":
            f["matched"] = True
    return out


def converted(stats: dict[str, dict]) -> list[str]:
    """Functions this segment turned into a MATCH: first scored as non-MATCH, later MATCH.

    A function whose first score was already MATCH is a reverification, not new work."""
    return [n for n, f in stats.items() if f["matched"] and f["first"] != "MATCH"]


def unfinished(stats: dict[str, dict]) -> list[str]:
    """Functions worked here but not matched. One whose every score is a compile error (a
    broken file scored in passing, say mid-merge) had no candidate here and is left out, so it
    neither counts as a park nor drags the distinct-candidate median to zero."""
    return [n for n, f in stats.items() if not f["matched"] and f["attempts"] > f["errors"]]


def distinct_candidates(f: dict) -> tuple[int, bool]:
    """(count, exact) -- distinct codegen hashes when every compiling score has one, else the
    number of distinct exact-instruction counts, a lower bound (flagged inexact)."""
    if f["codegen"] and f["all_hashed"]:
        return len(f["codegen"]), True
    return len(f["exacts"]), False


def hypothesis_counts(names, runs: dict[str, list[dict]]) -> dict[str, int | None]:
    """Distinct compiled candidates (codegen hashes) per function over every run in RUNS;
    None when none of its compiling scores carries a hash (logged before 2026-11-13)."""
    seen: dict[str, set[str]] = {n: set() for n in names}
    for events in runs.values():
        for e in events:
            if e.get("name") in seen and real_score(e) and e.get("codegen"):
                seen[e["name"]].add(e["codegen"])
    return {n: len(h) or None for n, h in seen.items()}


def median(values):
    values = [v for v in values if v is not None]
    return statistics.median(values) if values else None


def ratio(num, den):
    return num / den if den else None


# ------------------------------------------------------------------ metrics


def matched_names_after(runs: dict[str, list[dict]], t: float) -> set[str]:
    return {e["name"] for events in runs.values() for e in events
            if real_score(e) and e.get("verdict") == "MATCH" and e["t"] > t}


def measure(segments: list[Segment], all_runs: dict[str, list[dict]] | None = None) -> dict:
    """Cohort metrics over SEGMENTS (sums over the cohort, then rates; medians over runs)."""
    active = sum(s.active_seconds() for s in segments)
    scores = [e for s in segments for e in s.scores()]
    by_source: dict[str, list[int]] = {}
    for e in scores:
        tot = by_source.setdefault(e.get("source", "unrecorded"), [0, 0])
        tot[0] += 1
        tot[1] += e.get("verdict") == "ERROR"
    conv_bytes = gain = 0
    conv_names: list[str] = []
    left: list[tuple[str, float]] = []
    distinct: list[int] = []
    distinct_exact = True
    first_compile: list[float] = []
    for s in segments:
        stats = function_stats(s.scores())
        for n in converted(stats):
            conv_names.append(n)
            conv_bytes += stats[n]["target"] * 4
        gain += sum(f["best_exact"] - f["first_exact"] for f in stats.values() if f["first_exact"] is not None)
        for n in unfinished(stats):
            count, exact = distinct_candidates(stats[n])
            distinct.append(count)
            distinct_exact &= exact
            left.append((n, s.end or 0))
        compiled = [e for e in s.scores() if e.get("verdict") != "ERROR"]
        if compiled and s.origin is not None:
            first_compile.append(compiled[0]["t"] - s.origin)
    last_seen: dict[str, float] = {}
    for n, t in left:  # a function left unfinished by two segments counts once, at the later one
        last_seen[n] = max(t, last_seen.get(n, t))
    recovered = None
    if all_runs is not None and last_seen:
        recovered = sum(1 for n, t in last_seen.items() if n in matched_names_after(all_runs, t))
    timed = [e["seconds"] for e in scores if e.get("seconds") is not None]
    builds = [e["seconds"] for s in segments for e in s.events
              if e.get("kind") == "time" and "build" in e.get("label", "")]
    errors = sum(e.get("verdict") == "ERROR" for e in scores)
    hours = active / 3600
    return {
        "segments": len(segments),
        "active_hours": round(hours, 3),
        "attempts": len(scores),
        "compile_errors": errors,
        "by_source": {k: {"attempts": a, "errors": e, "error_rate": round(e / a, 3)}
                      for k, (a, e) in sorted(by_source.items())},
        "converted": len(conv_names),
        "converted_names": conv_names,
        "converted_bytes": conv_bytes,
        "exact_gain": gain,
        "unfinished": len(last_seen),
        "unfinished_at_floor": sum(c >= HYPOTHESIS_FLOOR for c in distinct),
        "distinct_is_exact": distinct_exact if distinct else None,
        "recovered": recovered,
        "matched_bytes_per_hour": round(conv_bytes / hours) if hours else None,
        "exact_gain_per_hour": round(gain / hours) if hours else None,
        "compile_error_rate": round(100 * errors / len(scores), 1) if scores else None,
        # Logged since gap 25; older ERROR events carry no diagnostic and are left out.
        "error_classes": Counter(e["error"] for e in scores
                                 if e.get("verdict") == "ERROR" and e.get("error")).most_common(5),
        "distinct_per_unfinished": median(distinct),
        "first_compile_seconds": round(median(first_compile)) if first_compile else None,
        "park_recovery_rate": round(100 * recovered / len(last_seen), 1) if recovered is not None else None,
        "score_seconds": round(median(timed), 2) if timed else None,
        "build_seconds": median(builds),
    }


# ------------------------------------------------------------------ cohorts and comparisons


def load_config(path: Path = CONFIG) -> dict:
    return tomllib.loads(path.read_text()) if path.exists() else {}


def cohort_segments(cohort: dict, runs: dict[str, list[dict]]) -> tuple[list[Segment], list[str]]:
    segs, missing = [], []
    for spec in cohort.get("segments", []):
        events = runs.get(spec["run"])
        if events is None:
            missing.append(spec["run"])
            continue
        segs.append(Segment.of(spec["run"], events, spec.get("since"), spec.get("until")))
    return segs, missing


def multiplier(metric: Metric, before, after) -> float | None:
    """How many times better AFTER is (>1 means improved), or None when it cannot be stated."""
    if before is None or after is None:
        return None
    if metric.better == "higher":
        return after / before if before else None
    return before / after if after else None


def compare_rows(before: dict, after: dict, keys=None) -> list[dict]:
    rows = []
    for m in METRICS:
        if keys and m.key not in keys:
            continue
        b, a = before.get(m.key), after.get(m.key)
        rows.append({"key": m.key, "label": m.label, "unit": m.unit, "better": m.better,
                     "before": b, "after": a, "multiplier": multiplier(m, b, a)})
    return rows


def fmt(value, unit: str = "") -> str:
    if value is None:
        return "unknown"
    if isinstance(value, float) and not value.is_integer():
        text = f"{value:,.1f}"
    else:
        text = f"{int(value):,}"
    return f"{text}{'%' if unit == '%' else (' ' + unit if unit else '')}"


def fmt_mult(x: float | None) -> str:
    if x is None:
        return "–"
    return f"{x:,.0f}×" if x >= 10 else f"{x:,.1f}×"


def markdown(title: str, rows: list[dict]) -> str:
    out = [f"**{title}**", "", "| Metric | Before → after | Change |", "|---|---|---|"]
    for r in rows:
        out.append(f"| {r['label']} | {fmt(r['before'], r['unit'])} → **{fmt(r['after'], r['unit'])}** "
                   f"| {fmt_mult(r['multiplier'])} |")
    return "\n".join(out)


def svg(title: str, subtitle: str, rows: list[dict], footer: str = "LSW1 decomp") -> str:
    """Before/after bars drawn to the same scale per row, in the style of a release benchmark card."""
    def esc(t: str) -> str:
        return t.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
    w, top, rh = 1000, 120, 64
    h = top + rh * len(rows) + 70
    out = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {w} {h}" width="{w}" height="{h}" '
           'font-family="Inter, -apple-system, Segoe UI, Helvetica, Arial, sans-serif">',
           f'<rect width="{w}" height="{h}" fill="#000"/>',
           f'<text x="28" y="62" fill="#fff" font-size="34" font-weight="600">{esc(title)}</text>',
           f'<text x="{w - 28}" y="62" fill="#8a8a8a" font-size="15" text-anchor="end">{esc(subtitle)}</text>',
           f'<line x1="28" y1="{top - 24}" x2="{w - 28}" y2="{top - 24}" stroke="#222"/>']
    bx, bw = 400, 220
    for i, r in enumerate(rows):
        y = top + i * rh
        b, a = r["before"] or 0, r["after"] or 0
        scale = max(b, a) or 1
        out.append(f'<text x="28" y="{y + 16}" fill="#fff" font-size="19">{esc(r["label"])}</text>')
        out.append(f'<rect x="{bx}" y="{y + 1}" width="{bw * b / scale:.1f}" height="5" rx="2.5" fill="#3a3a3a"/>')
        out.append(f'<rect x="{bx}" y="{y + 12}" width="{max(bw * a / scale, 3):.1f}" height="5" rx="2.5" fill="#fff"/>')
        out.append(f'<text x="{bx + bw + 24}" y="{y + 16}" font-size="17"><tspan fill="#8a8a8a">'
                   f'{esc(fmt(r["before"], r["unit"]))} → </tspan><tspan fill="#fff">'
                   f'{esc(fmt(r["after"], r["unit"]))}</tspan></text>')
        out.append(f'<text x="{w - 28}" y="{y + 20}" fill="#fff" font-size="30" font-weight="500" '
                   f'text-anchor="end">{esc(fmt_mult(r["multiplier"]))}</text>')
        out.append(f'<line x1="28" y1="{y + rh - 24}" x2="{w - 28}" y2="{y + rh - 24}" stroke="#222"/>')
    fy = h - 26
    out += [f'<rect x="28" y="{fy - 8}" width="30" height="6" rx="3" fill="#3a3a3a"/>',
            f'<text x="66" y="{fy}" fill="#8a8a8a" font-size="14">before</text>',
            f'<rect x="140" y="{fy - 8}" width="30" height="6" rx="3" fill="#fff"/>',
            f'<text x="178" y="{fy}" fill="#8a8a8a" font-size="14">after, drawn to the same scale</text>',
            f'<text x="{w - 28}" y="{fy}" fill="#8a8a8a" font-size="14" text-anchor="end">{esc(footer)}</text>',
            "</svg>"]
    return "\n".join(out) + "\n"


# ------------------------------------------------------------------ CLI


def run_rows(runs: dict[str, list[dict]], since=None, until=None) -> list[dict]:
    rows = []
    for run_id, events in runs.items():
        seg = Segment.of(run_id, events, since, until)
        if not seg.scores():
            continue
        m = measure([seg], runs)
        rows.append({"run_id": run_id, "model": seg.start.get("model"), "item": seg.start.get("item"),
                     "started": datetime.fromtimestamp(seg.origin).isoformat(timespec="minutes"), **m})
    return sorted(rows, key=lambda r: r["started"])


def print_rows(rows: list[dict]) -> None:
    cols = ("started", "item", "model", "active_hours", "attempts", "compile_error_rate", "converted",
            "matched_bytes_per_hour", "exact_gain_per_hour", "unfinished", "distinct_per_unfinished",
            "park_recovery_rate")
    print("\t".join(cols))
    for r in rows:
        print("\t".join(str(r.get(c)) for c in cols))


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--config", type=Path, default=CONFIG)
    ap.add_argument("--logs", action="append", type=Path,
                    help="read this event log instead of discovering them (repeatable)")
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("report", help="metrics per discovered run")
    r.add_argument("--since")
    r.add_argument("--until")
    r.add_argument("--json", action="store_true")
    c = sub.add_parser("cohorts", help="metrics per configured cohort")
    c.add_argument("--json", action="store_true")
    k = sub.add_parser("compare", help="before -> after for each configured comparison")
    k.add_argument("--json", action="store_true")
    k.add_argument("--svg", type=Path, help="also write the first comparison as an SVG card")
    sub.add_parser("archive", help="copy every discovered log into <bench cache>/events/")
    args = ap.parse_args(argv)
    paths = args.logs if args.logs else log_paths()
    runs = load_runs(paths)
    if args.cmd == "archive":
        n = 0
        for p in paths:
            if p.parent != sb.cache_dir() / "events" and sb.archive_events(p):
                n += 1
        print(f"archived {n} logs to {sb.cache_dir() / 'events'}")
        return 0
    if args.cmd == "report":
        rows = run_rows(runs, args.since, args.until)
        print(json.dumps(rows, indent=2)) if args.json else print_rows(rows)
        return 0
    config = load_config(args.config)
    cohorts = {}
    for cohort in config.get("cohort", []):
        segs, missing = cohort_segments(cohort, runs)
        cohorts[cohort["name"]] = dict(measure(segs, runs), label=cohort.get("label", cohort["name"]),
                                       missing_runs=missing)
    if args.cmd == "cohorts":
        if args.json:
            print(json.dumps(cohorts, indent=2))
        else:
            for name, m in cohorts.items():
                print(f"{name}: {m['label']}" + (f" (missing logs: {', '.join(m['missing_runs'])})"
                                                 if m["missing_runs"] else ""))
                for metric in METRICS:
                    print(f"  {metric.label}: {fmt(m.get(metric.key), metric.unit)}")
        return 0
    results = []
    for comp in config.get("comparison", []):
        before, after = cohorts.get(comp["before"]), cohorts.get(comp["after"])
        if before is None or after is None:
            print(f"error: comparison {comp.get('title')} names an unknown cohort", file=sys.stderr)
            return 2
        rows = compare_rows(before, after, comp.get("metrics"))
        results.append({"title": comp["title"], "subtitle": comp.get("subtitle", ""), "rows": rows})
    if args.json:
        print(json.dumps(results, indent=2))
    else:
        print("\n\n".join(markdown(r["title"], r["rows"]) for r in results))
    if args.svg and results:
        args.svg.parent.mkdir(parents=True, exist_ok=True)
        args.svg.write_text(svg(results[0]["title"], results[0]["subtitle"], results[0]["rows"]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
