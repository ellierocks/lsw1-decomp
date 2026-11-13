#!/usr/bin/env python3
"""One-pass sprint finish: build once, strict-verify every function, print the PR record.

    python3 tools/sprint_finish.py --baseline            # at kickoff, after bash build.sh
    python3 tools/sprint_finish.py FN1 FN2 ... [--parked "FN3=41/51: mismatch; next: idea"]
    python3 tools/sprint_finish.py FN1 FN2 --merge       # fetch and merge origin/main (pushed branch)
    python3 tools/sprint_finish.py FN1 FN2 --rebase      # rebase; refused once commits are pushed
    python3 tools/sprint_finish.py --queue-add FN...     # add functions to the saved queue

`--baseline` fetches origin and refuses to run when HEAD is behind origin/main, so a sprint never
starts from stale docs or tools. It then saves the objdiff figures, strict-only count and DOL hash to
`work/sprint_baseline.json`. The finish run then does, in order: optional rebase (commit first),
one `bash build.sh`, `verify_fn.py --strict-symbols` on each named function against that build,
`git diff --check origin/main`, and the report delta against the baseline. It prints a filled-in
[sprint record](../docs/sprint_workflow.md#records) and exits non-zero if anything failed, so
the PR body needs only the Donor and Headers lines added. It never edits sources or pushes.

`--matched-from-diff` adds every function newly defined in `--unit` (a source path) since the
merge base with origin/main, so a long run does not have to list its matches by hand. When
`sprint_bench.py start` (or `kickoff.py --start`) opened a bench run, finish closes it with the
verified count and bytes and the active time. The
record's Workflow line lists this branch's `fix(tools)`/`feat(tools)` commits; add what each
one unlocked.
"""
from __future__ import annotations

import argparse
import contextlib
import hashlib
import io
import json
import os
import re
import signal
import subprocess
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import sprint_metrics as metrics
import strict_gap  # noqa: E402
import verify_fn as vf  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
BASELINE = ROOT / "work" / "sprint_baseline.json"
DOL = ROOT / "build" / "GL5E4F" / "main.dol"


FETCH_TIMEOUT = 60


def run(cmd: list[str], **kw) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, **kw)


def fetch(timeout: float = FETCH_TIMEOUT, cmd=None) -> bool:
    """`git fetch origin` with a timeout, so a stalled SSH connection cannot hang a sprint.

    The fetch runs in its own process group with no pipes, so a timeout also kills the
    `ssh` child that git spawns instead of waiting on its open output.
    """
    if cmd is None:
        cmd = (sys.executable, str(Path(__file__).resolve().parent / "git_origin.py"), "fetch", "-q", "origin")
    proc = subprocess.Popen(list(cmd), cwd=ROOT, stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                            stderr=subprocess.DEVNULL, start_new_session=True)
    try:
        return proc.wait(timeout=timeout) == 0
    except subprocess.TimeoutExpired:
        os.killpg(proc.pid, signal.SIGKILL)
        proc.wait()
        return False


def snapshot() -> dict:
    report = json.loads((ROOT / "build" / "GL5E4F" / "report.json").read_text())
    m = report["measures"]
    gap = [r for r in strict_gap.candidates(report, 90.0) if strict_gap.strict_match(r["name"])]
    return {"functions": int(m["matched_functions"]), "code": int(float(m["matched_code"])),
            "strict_only_functions": len(gap), "strict_only_bytes": sum(r["size"] for r in gap),
            "dol_sha1": hashlib.sha1(DOL.read_bytes()).hexdigest() if DOL.is_file() else None,
            "head": run(["git", "rev-parse", "--short", "HEAD"]).stdout.strip()}


def verify(name: str) -> bool:
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf), contextlib.redirect_stderr(io.StringIO()):
        return vf.main([name, "--strict-symbols", "--allow-stale", "--quiet"]) == 0


def validate_queue(queue: list[str], matched: list[str], parked: list[str], deferred=None) -> None:
    queue = metrics.dedupe(queue)
    if not queue:
        raise ValueError("supply the complete sprint queue with --queue (or save it at --baseline)")
    deferred = deferred or []
    names = list(matched) + deferred
    for row in parked:
        name, sep, note = row.partition("=")
        if not sep or not note.strip() or "next:" not in note:
            raise ValueError("--parked requires FN=score: mismatch; next: hypothesis")
        names.append(name)
    if len(names) != len(set(names)):
        raise ValueError("a function cannot appear twice or be both matched and parked")
    missing = sorted(set(queue) - set(names))
    extra = sorted(set(names) - set(queue))
    if missing or extra:
        raise ValueError(f"queue accounting failed: unresolved={missing}, outside queue={extra}; "
                         "continue matching or record bounded plateaus with --parked; "
                         "context pressure requires a continuation, not an incomplete finish")


def park_evidence(names: list[str], runs=None) -> dict[str, int | None]:
    """Distinct compiled candidates per parked function, from every bench log workflow_metrics
    can find (this checkout, sibling worktrees, the bench cache). None: no hashed scores."""
    if not names:
        return {}
    import workflow_metrics as wm
    if runs is None:
        runs = wm.load_runs(wm.log_paths())
    return wm.hypothesis_counts(names, runs)


def short_parks(evidence: dict[str, int | None], floor: int | None = None) -> list[str]:
    """Parked functions with fewer than FLOOR distinct compiled candidates on record.

    A later pass matched 53% of the functions short passes left (docs/workflow_gaps.md#12):
    under the floor a function is unfinished, not a plateau. Unknown counts are not refused."""
    if floor is None:
        import workflow_metrics as wm
        floor = wm.HYPOTHESIS_FLOOR
    return [f"{fn} ({n})" for fn, n in evidence.items() if n is not None and n < floor]


def phase(path: Path, name: str, now: float | None = None, cap: float | None = None) -> dict:
    """Switch the timer phase. Time since the last activity event counts as active only up to
    the idle cap; `paused` stops the clock. Wall-clock is kept beside it."""
    return metrics.switch_phase(path, name, now, cap)


def pushed_commits(base_ref: str = "origin/main") -> list[str]:
    """Commits that `git rebase base_ref` would rewrite and that are already on a remote ref."""
    upstream = run(["git", "rev-parse", "--abbrev-ref", "--symbolic-full-name", "@{upstream}"])
    refs = [upstream.stdout.strip()] if upstream.returncode == 0 else []
    branch = run(["git", "rev-parse", "--abbrev-ref", "HEAD"]).stdout.strip()
    if branch and branch != "HEAD":
        refs.append(f"origin/{branch}")
    mine = set(run(["git", "rev-list", f"{base_ref}..HEAD"]).stdout.split())
    pushed: set[str] = set()
    for ref in dict.fromkeys(refs):
        if ref == base_ref or run(["git", "rev-parse", "--verify", "-q", ref]).returncode:
            continue
        pushed |= mine & set(run(["git", "rev-list", f"{base_ref}..{ref}"]).stdout.split())
    return sorted(pushed)


def rebase_refusal(commits: list[str]) -> str | None:
    """Message refusing a rebase of already-pushed commits, or None when it is safe."""
    if not commits:
        return None
    return (f"error: {len(commits)} commit(s) to rebase are already on the remote branch "
            f"({', '.join(c[:8] for c in commits[:5])}); rebasing needs a force push, which "
            "rewrites pushed history. Use --merge to bring in origin/main instead")


# A file-scope definition: specifiers (and GCC attributes) on the name's line, then
# the parameter list and the opening brace. _DEF_SPLIT covers the return type on the
# line above a name that starts its own line (`static s32\nfn_X(void) {`).
_ATTR = r"__attribute__\s*\(\([^;{}\n]*?\)\)"
_SPECS = rf"(?![ \t#/*}}])(?:{_ATTR}\s*)*[A-Za-z_](?:{_ATTR}|[^;{{}}()\n=])*?"
_PARAMS = r"\s*\([^;{}]*?\)\s*\{"
_DEF = re.compile(rf"^{_SPECS}\b(?!__attribute__\b)(\w+){_PARAMS}", re.M)
_DEF_SPLIT = re.compile(rf"^({_SPECS})[ \t]*\n(?!__attribute__\b|_DEFUN(?:_VOID)?\b)([A-Za-z_]\w*){_PARAMS}", re.M)


def _definitions(text: str) -> list[tuple[str, str]]:
    """(name, specifiers before it) for each file-scope definition, in source order."""
    found = [(m.start(1), m.group(1), text[m.start():m.start(1)]) for m in _DEF.finditer(text)]
    found += [(m.start(2), m.group(2), m.group(1)) for m in _DEF_SPLIT.finditer(text)]
    return [(name, specs) for _, name, specs in sorted(found)]


def defined_functions(text: str) -> list[str]:
    """Names of functions defined at file scope in C `text`, in order."""
    return list(dict.fromkeys(name for name, _ in _definitions(text)))


def merge_base() -> str:
    return run(["git", "merge-base", "HEAD", "origin/main"]).stdout.strip() or "origin/main"


def global_functions(text: str) -> list[str]:
    """Like defined_functions, but without `static` helpers, which have no retail symbol."""
    # `static` may follow other specifiers (`inline static`, `const static`).
    return list(dict.fromkeys(name for name, specs in _definitions(text)
                              if not re.search(r"\bstatic\b", specs)))


def _unit_path(unit: str) -> Path:
    path = Path(unit)
    return path if path.is_absolute() else ROOT / path


def defined_at_base(unit: str, base: str | None = None) -> set[str]:
    """Functions UNIT already defined at the merge base (matched before this branch)."""
    rel = _unit_path(unit).resolve().relative_to(ROOT)
    old = run(["git", "show", f"{base or merge_base()}:{rel.as_posix()}"])
    return set(defined_functions(old.stdout)) if old.returncode == 0 else set()


def matched_from_diff(unit: str, base: str | None = None) -> list[str]:
    """Non-static functions defined in UNIT now that were not defined there at the merge base."""
    before = defined_at_base(unit, base)
    return [fn for fn in global_functions(_unit_path(unit).read_text()) if fn not in before]


def drop_prior_matches(queue: list[str], named: set[str], prior: set[str]) -> tuple[list[str], list[str]]:
    """(queue, dropped): queue names already defined before this branch and not named in this
    finish. score_fn queues every function it scores, so rechecking an old match would otherwise
    fail queue accounting; it is a reverification, not progress, so it is not counted matched."""
    dropped = [n for n in queue if n in prior and n not in named]
    return [n for n in queue if n not in dropped], dropped


def tool_commits(base: str | None = None) -> list[str]:
    """Subjects of this branch's tooling commits, oldest first."""
    log = run(["git", "log", "--reverse", "--format=%s", f"{base or merge_base()}..HEAD"]).stdout
    return [s for s in log.splitlines() if re.match(r"(fix|feat|perf|refactor)\(tools\)", s)]


def merge_main() -> tuple[bool, str]:
    """Fetch and merge origin/main; abort cleanly on conflict. Returns (ok, message)."""
    if not fetch():
        return False, "git fetch failed or timed out"
    merged = run(["git", "merge", "--no-edit", "origin/main"])
    if merged.returncode:
        run(["git", "merge", "--abort"])
        return False, "merge of origin/main failed (conflict or dirty tree) and was aborted; resolve it by hand, then rerun"
    return True, ""


def behind_origin(do_fetch: bool = True) -> int:
    """Commits HEAD lacks from origin/main, fetching first (a failed fetch warns and uses the
    last fetched origin/main)."""
    if do_fetch and not fetch():
        print(f"warning: git fetch failed or exceeded {FETCH_TIMEOUT}s; checking freshness "
              "against the last fetched origin/main", file=sys.stderr)
    behind = run(["git", "rev-list", "--count", "HEAD..origin/main"]).stdout.strip()
    return int(behind) if behind.isdigit() else 0


BEHIND_ERROR = ("error: HEAD is {} commits behind origin/main, so the workflow docs and tools may be "
                "stale; rebase onto origin/main (or pass --allow-behind)")


def record_baseline(queue: list[str], allow_behind: bool = False, idle_cap: float | None = None,
                    extra: dict | None = None) -> int:
    """Save the kickoff baseline (objdiff figures, strict-only count, DOL hash, queue). Refuses
    with 2 when HEAD is behind origin/main unless allow_behind. EXTRA keys (kickoff's unit and
    lane) are stored beside the figures."""
    timing = ROOT / "work" / "sprint_timing.json"
    if not allow_behind:
        behind = behind_origin()
        if behind:
            print(BEHIND_ERROR.format(behind), file=sys.stderr)
            return 2
    snap = snapshot()
    snap["queue"] = metrics.dedupe(queue)
    snap["strict_baseline"] = {name: verify(name) for name in snap["queue"]}
    snap.update(extra or {})
    BASELINE.parent.mkdir(exist_ok=True)
    BASELINE.write_text(json.dumps(snap, indent=2))
    print(f"baseline: {snap['functions']:,} functions, {snap['code']:,} bytes, "
          f"{snap['strict_only_functions']} strict-only; saved to {BASELINE.relative_to(ROOT)}")
    # The baseline closes kickoff: a timer still in setup moves to matching here, so a
    # run that never switches by hand does not book all its matching as setup (#74).
    if timing.exists() and json.loads(timing.read_text()).get("active") == "setup":
        phase(timing, "matching", cap=idle_cap)
        print("timer: setup -> matching")
    return 0


def new_verified_names(base: dict | None, results: dict[str, bool]) -> list[str] | None:
    """New strict matches only when every successful result was checked at kickoff.

    Legacy baselines and later queue additions cannot prove novelty. Do not infer
    it from the report delta: strict-only matches and main drift make that unsafe.
    """
    before = (base or {}).get("strict_baseline")
    names = [name for name, ok in results.items() if ok]
    if before is None or any(name not in before for name in names):
        return None
    return [name for name in names if not before[name]]


def symbol_sizes(names) -> dict[str, int]:
    """`.text` sizes from symbols.txt for NAMES (absent names are left out)."""
    want, sizes = set(names), {}
    path = ROOT / "config" / "GL5E4F" / "symbols.txt"
    for line in path.read_text().splitlines() if path.is_file() else []:
        m = re.match(r"^(\S+) = \.text:0x[0-9A-Fa-f]+; // type:function size:0x([0-9A-Fa-f]+)", line)
        if m and m.group(1) in want:
            sizes[m.group(1)] = int(m.group(2), 16)
    return sizes


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("functions", nargs="*", help="functions matched in this sprint")
    ap.add_argument("--baseline", action="store_true", help="record the kickoff baseline and exit")
    ap.add_argument("--rebase", action="store_true",
                    help="git fetch and rebase onto origin/main first (refused when commits are pushed)")
    ap.add_argument("--merge", action="store_true",
                    help="git fetch and merge origin/main first; safe on a pushed branch")
    ap.add_argument("--queue-add", nargs="+", metavar="FN", help="add functions to the saved sprint queue and exit")
    ap.add_argument("--idle-cap", type=float, metavar="SECONDS",
                    help=f"count at most this long between activity events as active time (default "
                         f"{int(metrics.DEFAULT_IDLE_CAP)}; env {metrics.IDLE_CAP_ENV}); saved with the timer")
    ap.add_argument("--parked", action="append", default=[], metavar="FN=NOTE", help="parked function row")
    ap.add_argument("--unit", help="unit name for the record heading (a source path for --matched-from-diff)")
    ap.add_argument("--matched-from-diff", action="store_true",
                    help="add the functions newly defined in --unit since the merge base with origin/main")
    ap.add_argument("--allow-behind", action="store_true",
                    help="record a baseline even when HEAD is behind origin/main")
    ap.add_argument("--queue", nargs="+", help="complete ordered sprint queue; saved with baseline")
    ap.add_argument("--phase", choices=["setup", "matching", "tooling", "finish", "paused"],
                    help="switch elapsed-time phase and exit; pause during user/CI waits")
    ap.add_argument("--timing", "--budget-status", dest="budget_status", action="store_true", help="report measured active time per phase")
    ap.add_argument("--usage", type=Path, help="import provider-reported usage JSON for one run")
    ap.add_argument("--deferred", nargs="*", default=[], help="queue entries not attempted or not finished this run")
    ap.add_argument("--short-park", metavar="REASON",
                    help="park functions with fewer than 25 distinct compiled candidates anyway (recorded)")
    args = ap.parse_args(argv)
    if args.rebase and args.merge:
        ap.error("choose --merge or --rebase")
    if args.idle_cap is not None and args.idle_cap <= 0:
        ap.error("--idle-cap must be positive seconds")
    timing = ROOT / "work" / "sprint_timing.json"
    usage_path = ROOT / "work" / "sprint_usage.json"
    if args.usage:
        try:
            print(json.dumps(metrics.record_usage(usage_path, json.loads(args.usage.read_text())), indent=2))
        except (ValueError, OSError) as exc:
            ap.error(str(exc))
        return 0
    if args.queue_add:
        added = metrics.queue_add(BASELINE, args.queue_add)
        if added is None:
            ap.error("no sprint baseline; run --baseline --queue first")
        print(f"queue: added {', '.join(added) or 'nothing (already queued)'}; "
              f"{len(json.loads(BASELINE.read_text())['queue'])} entries")
        return 0
    if args.budget_status:
        if not timing.exists():
            ap.error("start timing with --phase setup; historical active time is unknown")
        current = json.loads(timing.read_text())
        current = phase(timing, current["active"], cap=args.idle_cap)
        print(json.dumps(metrics.timing_report(current), indent=2))
        return 0
    if args.phase:
        current = phase(timing, args.phase, cap=args.idle_cap)
        print(json.dumps({**current, **metrics.timing_report(current)}, indent=2))
        return 0

    if args.baseline:
        return record_baseline(args.queue or [], args.allow_behind, args.idle_cap)
    if args.matched_from_diff:
        if not args.unit or not (Path(args.unit).is_file() or (ROOT / args.unit).is_file()):
            ap.error("--matched-from-diff needs --unit as the unit's source path")
        parked_names = {p.partition("=")[0] for p in args.parked} | set(args.deferred)
        args.functions = metrics.dedupe(args.functions + [
            fn for fn in matched_from_diff(args.unit) if fn not in parked_names])
    if not args.functions and not args.parked and not args.deferred:
        ap.error("name the matched functions (or use --baseline)")

    base = json.loads(BASELINE.read_text()) if BASELINE.is_file() else None
    if not args.unit:
        ap.error("finish requires --unit and the complete --queue")
    if args.unit:
        queue = metrics.dedupe(args.queue or (base or {}).get("queue", []))
        if args.matched_from_diff:
            named = set(args.functions) | parked_names
            queue, dropped = drop_prior_matches(queue, named, defined_at_base(args.unit))
            if dropped:
                print(f"queue: {', '.join(dropped)} already matched before this branch "
                      "(reverified while scoring, not progress); left out of accounting", file=sys.stderr)
        try:
            validate_queue(queue, args.functions, args.parked, args.deferred)
        except ValueError as exc:
            print(f"error: {exc}", file=sys.stderr)
            return 2
    evidence = park_evidence([p.partition("=")[0] for p in args.parked])
    short = short_parks(evidence)
    if short and not args.short_park:
        print(f"error: too few distinct compiled candidates to park: {', '.join(short)}. A plateau needs 25 "
              "(docs/sprint_workflow.md#matching-brief); keep going, list them under --deferred with the "
              "hypotheses left, or pass --short-park REASON", file=sys.stderr)
        return 2
    if args.rebase:
        if not fetch():
            print("error: git fetch failed or timed out", file=sys.stderr)
            return 2
        refusal = rebase_refusal(pushed_commits())
        if refusal:
            print(refusal, file=sys.stderr)
            return 2
    if timing.exists():
        phase(timing, "finish")
    failures: list[str] = []
    if args.merge:
        ok, why = merge_main()
        if not ok:
            print(f"error: {why}", file=sys.stderr)
            if timing.exists():
                phase(timing, "paused")
            return 2
    if args.rebase and run(["git", "rebase", "origin/main"]).returncode:
        run(["git", "rebase", "--abort"])
        print("error: rebase failed and was aborted; resolve it, then rerun", file=sys.stderr)
        if timing.exists():
            phase(timing, "paused")
        return 2
    import sprint_bench
    t_build = time.time()
    b = run(["bash", "build.sh"])
    sprint_bench.append("time", label="finish-build", seconds=round(time.time() - t_build, 1),
                        returncode=b.returncode)
    if b.returncode:
        print(b.stdout[-1500:] + b.stderr[-1500:], file=sys.stderr)
        print("error: build.sh failed", file=sys.stderr)
        if timing.exists():
            phase(timing, "paused")
        return 2
    results = {fn: verify(fn) for fn in args.functions}
    failures += [f"verify {fn}" for fn, ok in results.items() if not ok]
    check = run(["git", "diff", "--check", "origin/main"])
    if check.returncode:
        failures.append("git diff --check")
        print(check.stdout.strip(), file=sys.stderr)
    snap = snapshot()
    same_dol = base is None or base["dol_sha1"] == snap["dol_sha1"]
    if not same_dol:
        failures.append("DOL hash changed")

    parked = [p.split("=", 1) for p in args.parked]
    print(f"\nSprint: `{args.unit or 'UNIT'}` ({sum(results.values())} matched, {len(parked)} parked, {len(args.deferred)} deferred)\n")
    print("| Function | Result |\n|---|---|")
    for fn, ok in results.items():
        print(f"| {fn} | {'VERIFIED' if ok else '**FAILED strict check**'} |")
    for fn, note in parked:
        n = evidence.get(fn)
        print(f"| {fn} | parked {note} ({n if n is not None else 'unknown'} distinct candidates) |")
    if short:
        print(f"\nShort parks ({args.short_park}): {', '.join(short)}")
    for fn in args.deferred:
        print(f"| {fn} | deferred: not finished this run; resume next run |")
    print("\n- Promoted: none (promotion requires a separate complete-unit gate)")
    if base:
        drift = " (includes main drift)" if args.rebase or args.merge or base["head"] != snap["head"] else ""
        print(f"- Report: functions {base['functions']:,} → {snap['functions']:,}, code bytes "
              f"{base['code']:,} → {snap['code']:,}{drift}; strict-only "
              f"{base['strict_only_functions']} → {snap['strict_only_functions']} functions, "
              f"{base['strict_only_bytes']:,} → {snap['strict_only_bytes']:,} bytes")
    else:
        print(f"- Report: functions {snap['functions']:,}, code bytes {snap['code']:,} (no baseline saved)")
    print(f"- DOL: {'byte-identical' if same_dol else 'CHANGED'} (sha1 {str(snap['dol_sha1'])[:8]}…)")
    print("- Donor: <one line, or \"no OpenSaga hits\">\n- Headers: <fields added with their consumer, or none>")
    tools = tool_commits()
    print("- Workflow: " + ("; ".join(tools) + " (add what each unlocked)" if tools
                            else "<tool fixes made this run and the matches they unlocked, or none>"))
    if timing.exists():
        report = metrics.timing_report(phase(timing, "paused"))
        print("- Active time: " + ", ".join(f"{k}={v:.1f}s" for k, v in sorted(report["phases"].items()))
              + f"; total active {report['active_seconds']:.0f}s vs wall-clock {report['wall_seconds']:.0f}s "
              f"({report['idle_excluded_seconds']:.0f}s idle excluded, gaps over "
              f"{report['idle_cap_seconds']:.0f}s capped); unrecorded earlier effort is unknown")
        print("- Timing JSON: " + json.dumps(report))
    print("- Provider usage: " + json.dumps(metrics.usage_report(
        json.loads(usage_path.read_text()) if usage_path.exists() else {})))
    verified_names = [fn for fn, ok in results.items() if ok]
    record = {"unit": args.unit, "verified": len(verified_names),
              "verified_bytes": sum(symbol_sizes(verified_names).values()), "parked": len(parked),
              "deferred": len(args.deferred), "tool_commits": len(tools), "failed": failures,
              "parked_names": [p[0] for p in parked], "deferred_names": list(args.deferred),
              "parked_hypotheses": evidence, "short_park": args.short_park if short else None}
    new_names = new_verified_names(base, results)
    record["new_verified"] = len(new_names) if new_names is not None else None
    record["new_verified_bytes"] = (sum(symbol_sizes(new_names).values())
                                    if new_names is not None else None)
    print("- New strict matches: " + (str(len(new_names)) if new_names is not None
                                     else "unknown (no complete strict baseline)"))
    if base:
        record["code_bytes_delta"] = snap["code"] - base["code"]
    if timing.exists():
        record.update(active_seconds=report["active_seconds"], phases=report["phases"])
    saved = sprint_bench.finish(record)
    if saved:
        print(f"- Benchmark: {saved}")
    if failures:
        print(f"\nFAILED: {', '.join(failures)}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
