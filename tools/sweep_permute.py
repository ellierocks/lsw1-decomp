#!/usr/bin/env python3
"""Run the strict-scored permuter on the sweep's biggest near-misses, unattended.

    python3 tools/sweep_permute.py                       # top 20 near-misses, 120 s each
    python3 tools/sweep_permute.py --limit 120 --seconds 30 --apply   # quick pass over everything
    python3 tools/sweep_permute.py --improved --seconds 180 --apply   # longer pass on what moved

Reads `work/sweep/results.json` from `sweep.py`, takes MISMATCH rows at or above
`--min-ratio` exact instructions, largest size x ratio first, and for each one splices its
`sweep_draft.c` into a copy of the unit source, sets the permuter up with
`permute_setup.py --source` (scored by the strict verifier, so score 0 means a strict
MATCH) and runs it with `--stop-on-zero` under a wall-clock limit. A score-0 output's
function replaces the draft's definition and `score_fn.py --body` rescoring confirms it;
`--apply` writes confirmed matches into the unit source; `--apply-saved` writes the last
run's confirmed matches without permuting again. By default only the plan target set's near
misses (tools/plan.py: the claimed or next item), then the rest of its stage's, are taken;
`--all` (or `--off-plan REASON`) takes the global order and marks the result rows off plan. Run `bash build.sh` and
`verify_fn.py --strict-symbols` afterwards, as for the sweep. Results go to
`work/sweep/permute_results.json`. No model or agent is involved.
"""
from __future__ import annotations

import argparse
import json
import os
import signal
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
import permute_setup  # noqa: E402
import sweep  # noqa: E402
from variant_search import VariantError, find_function  # noqa: E402

PERMUTE = ROOT / "work" / "permute" / "sweep"


def splice(text: str, name: str, body: str) -> str:
    """TEXT with NAME's definition replaced by BODY, or BODY appended (as score_fn --body)."""
    body = body.strip("\n")
    try:
        fn = find_function(text, name)
        return fn.before + body + fn.tail[1:]
    except VariantError:
        return text.rstrip("\n") + "\n\n" + body + "\n"


def definition(text: str, name: str) -> str:
    fn = find_function(text, name)
    return fn.head + fn.body + "}"


def queue(rows: list[dict], min_ratio: float, limit: int, scope: dict | None = None) -> list[dict]:
    """Near misses, largest size x ratio first. SCOPE (plan.plan_targets()) keeps only the plan
    target set's, then the rest of its stage's; None is the global order."""
    near = [r for r in rows if r.get("verdict") == "MISMATCH" and r.get("target")
            and r["exact"] / r["target"] >= min_ratio]
    near.sort(key=lambda r: -r["size"] * r["exact"] / r["target"])
    if scope is not None:
        item, stage = set(scope["functions"]), set(scope["stage"])
        near = [r for r in near if r["name"] in item] + [r for r in near if r["name"] in stage - item]
    return near[:limit]


def run_limited(cmd: list[str], seconds: float, env: dict, log: Path) -> None:
    with log.open("w") as out:
        p = subprocess.Popen(cmd, cwd=ROOT, env=env, stdout=out, stderr=subprocess.STDOUT,
                             stdin=subprocess.DEVNULL, start_new_session=True)
        try:
            p.wait(timeout=seconds)
        except subprocess.TimeoutExpired:
            os.killpg(p.pid, signal.SIGTERM)
            try:
                p.wait(timeout=10)
            except subprocess.TimeoutExpired:
                os.killpg(p.pid, signal.SIGKILL)
                p.wait()


def best_output(out: Path) -> tuple[int, Path] | None:
    best = None
    for d in out.glob("output-*"):
        try:
            score = int((d / "score.txt").read_text().split()[0])
        except (OSError, ValueError, IndexError):
            continue
        if (d / "source.c").is_file() and (best is None or score < best[0]):
            best = (score, d)
    return best


def permute_one(row: dict, seconds: float, jobs: int) -> dict:
    name, source = row["name"], ROOT / row["source"]
    res = {"name": name, "unit": row["unit"], "size": row["size"],
           "start": f"{row['exact']}/{row['target']}"}
    draft = (sweep.OUT / name / "sweep_draft.c").read_text()
    cand_dir = sweep.OUT / name / "candidate"
    cand_dir.mkdir(parents=True, exist_ok=True)
    cand = cand_dir / source.name
    cand.write_text(splice(source.read_text(), name, draft))
    out = PERMUTE / name
    r = subprocess.run([sys.executable, str(ROOT / "tools" / "permute_setup.py"), name,
                        "--source", str(cand), "--out", str(out), "--force"],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        return {**res, "verdict": "SETUP-FAILED", "error": (r.stderr or r.stdout).strip()[-200:]}
    env = dict(os.environ)
    env["PYTHONPATH"] = os.pathsep.join(filter(None, [str(permute_setup.SHIM), env.get("PYTHONPATH")]))
    checkout = permute_setup.permuter_checkout()
    t0 = time.monotonic()
    run_limited([sys.executable, str(checkout / "permuter.py"), str(out), "--stop-on-zero",
                 "--better-only", "--quiet", "-j", str(jobs)], seconds, env, out / "permuter.log")
    res["seconds"] = round(time.monotonic() - t0)
    best = best_output(out)
    if best is None:
        return {**res, "verdict": "NO-IMPROVEMENT"}
    res["best_score"] = best[0]
    if best[0] != 0:
        return {**res, "verdict": "IMPROVED", "output": str(best[1].relative_to(ROOT))}
    try:
        fn = definition((best[1] / "source.c").read_text(), name)
        own = find_function(draft, name)
        body = (own.before + own.tail[1:]).strip("\n") + "\n\n" + fn + "\n"
    except VariantError as e:
        return {**res, "verdict": "EXTRACT-FAILED", "error": str(e)}
    (sweep.OUT / name / "permuted.c").write_text(body)
    check = sweep.score(name, row["source"], body)
    return {**res, "verdict": "MATCH" if check["verdict"] == "MATCH" else f"RESCORE-{check['verdict']}"}


def apply_matches(rows: list[dict], sweep_rows: list[dict]) -> None:
    """Write each MATCH row's `permuted.c` into its unit, one at a time so each unit's
    rescoring sees the previous write; the row's verdict becomes APPLIED or NOT-APPLIED."""
    sources = {t["name"]: t["source"] for t in sweep_rows}
    for r in sorted((r for r in rows if r["verdict"] == "MATCH"), key=lambda r: r["name"]):
        body = (sweep.OUT / r["name"] / "permuted.c").read_text()
        ok = sweep.score(r["name"], sources[r["name"]], body, apply=True)["verdict"] == "MATCH"
        r["verdict"] = "APPLIED" if ok else "NOT-APPLIED"


def apply_saved(sweep_rows: list[dict]) -> int:
    """`--apply-saved`: apply the last run's confirmed matches instead of permuting again."""
    prev = sweep.OUT / "permute_results.json"
    if not prev.is_file():
        print("error: no work/sweep/permute_results.json; run sweep_permute.py first", file=sys.stderr)
        return 2
    rows = json.loads(prev.read_text())
    apply_matches(rows, sweep_rows)
    prev.write_text(json.dumps(rows, indent=1) + "\n")
    for r in rows:
        if r["verdict"] in ("APPLIED", "NOT-APPLIED"):
            print(f"  {r['name']:<28} {r['verdict']}")
    return 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--limit", type=int, default=20, help="near-misses to permute")
    ap.add_argument("--min-ratio", type=float, default=0.9, help="lowest exact-instruction ratio")
    ap.add_argument("--seconds", type=float, default=120, help="wall-clock limit per function")
    ap.add_argument("--parallel", type=int, default=4, help="functions permuted at once")
    ap.add_argument("-j", type=int, help="permuter threads per function (default: cores / parallel)")
    ap.add_argument("--improved", action="store_true",
                    help="only the previous run's IMPROVED rows (a longer second pass)")
    ap.add_argument("--apply", action="store_true", help="write confirmed matches into unit sources")
    ap.add_argument("--all", action="store_true",
                    help="the global near-miss order instead of the plan target set and its stage (off plan)")
    ap.add_argument("--off-plan", metavar="REASON", help="as --all, recording why in the results")
    ap.add_argument("--apply-saved", action="store_true",
                    help="write the MATCH rows of the last run's permute_results.json without permuting again")
    args = ap.parse_args(argv)
    results = sweep.OUT / "results.json"
    if not results.is_file():
        print("error: no work/sweep/results.json; run python3 tools/sweep.py first", file=sys.stderr)
        return 2
    if not (permute_setup.permuter_checkout() / "permuter.py").is_file():
        print("error: decomp-permuter not fetched; run bash tools/fetch_permuter.sh", file=sys.stderr)
        return 2
    if args.apply_saved:
        return apply_saved(json.loads(results.read_text()))
    scope = None
    if not (args.all or args.off_plan):
        import plan
        scope = plan.plan_targets(remote=False)
        if scope.get("error") or scope["item"] is None:
            print(f"plan unavailable ({scope.get('error') or 'no recommendable item'}); global order",
                  file=sys.stderr)
            scope = None
        else:
            print(f"plan scope: item {scope['item']}, then the rest of stage {scope['stage_id']} "
                  "(--all for the global order)", file=sys.stderr)
    off_plan = args.off_plan or ("--all" if args.all else None)
    todo = queue(json.loads(results.read_text()), args.min_ratio, 10**6 if args.improved else args.limit, scope)
    if args.improved:
        prev = sweep.OUT / "permute_results.json"
        moved = {r["name"] for r in json.loads(prev.read_text()) if r["verdict"] == "IMPROVED"} \
            if prev.is_file() else set()
        todo = [r for r in todo if r["name"] in moved]
    jobs = args.j or max(1, (os.cpu_count() or 4) // args.parallel)
    rows = []
    print(f"permuting {len(todo)} near-misses, {args.parallel} at a time, {args.seconds:.0f} s each, "
          f"-j {jobs}", file=sys.stderr)
    with ThreadPoolExecutor(args.parallel) as pool:
        futures = {pool.submit(permute_one, r, args.seconds, jobs): r for r in todo}
        for fut in as_completed(futures):
            try:
                r = fut.result()
            except Exception as exc:  # one bad target must not lose the run
                t = futures[fut]
                r = {"name": t["name"], "unit": t["unit"], "size": t["size"],
                     "start": f"{t['exact']}/{t['target']}", "verdict": "ERROR", "error": str(exc)[:200]}
            rows.append(r)
            print(f"  {r['name']:<28} {r['start']:>9} -> {r['verdict']}"
                  + (f" (score {r['best_score']})" if r.get("best_score") else ""), file=sys.stderr)
    for r in rows:
        if off_plan:
            r["off_plan"] = off_plan
        elif scope is not None:
            r["plan_item"] = scope["item"] if r["name"] in scope["functions"] else f"stage {scope['stage_id']}"
    if args.apply:
        apply_matches(rows, todo)
    (sweep.OUT / "permute_results.json").write_text(json.dumps(rows, indent=1) + "\n")
    won = [r for r in rows if r["verdict"] in ("MATCH", "APPLIED")]
    print(f"permuted {len(rows)}: {len(won)} strict matches, {sum(r['size'] for r in won):,} bytes"
          + ("" if args.apply else " (write them with --apply-saved)"))
    print(f"results: {(sweep.OUT / 'permute_results.json').relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
