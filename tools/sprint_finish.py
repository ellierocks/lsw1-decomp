#!/usr/bin/env python3
"""One-pass sprint finish: build once, strict-verify every function, print the PR record.

    python3 tools/sprint_finish.py --baseline            # at kickoff, after bash build.sh
    python3 tools/sprint_finish.py FN1 FN2 ... [--parked "FN3=41/51: mismatch; next: idea"]
    python3 tools/sprint_finish.py FN1 FN2 --rebase      # fetch and rebase onto origin/main first

`--baseline` fetches origin and refuses to run when HEAD is behind origin/main, so a sprint never
starts from stale docs or tools. It then saves the objdiff figures, strict-only count and DOL hash to
`work/sprint_baseline.json`. The finish run then does, in order: optional rebase (commit first),
one `bash build.sh`, `verify_fn.py --strict-symbols` on each named function against that build,
`git diff --check origin/main`, and the report delta against the baseline. It prints a filled-in
[sprint record](../docs/sprint_workflow.md#records) and exits non-zero if anything failed, so
the PR body needs only the Donor and Headers lines added. It never edits sources or pushes.
"""
from __future__ import annotations

import argparse
import contextlib
import hashlib
import io
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import strict_gap  # noqa: E402
import verify_fn as vf  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
BASELINE = ROOT / "work" / "sprint_baseline.json"
DOL = ROOT / "build" / "GL5E4F" / "main.dol"


def run(cmd: list[str], **kw) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, **kw)


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


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("functions", nargs="*", help="functions matched in this sprint")
    ap.add_argument("--baseline", action="store_true", help="record the kickoff baseline and exit")
    ap.add_argument("--rebase", action="store_true", help="git fetch and rebase onto origin/main first")
    ap.add_argument("--parked", action="append", default=[], metavar="FN=NOTE", help="parked function row")
    ap.add_argument("--unit", help="unit name for the record heading")
    ap.add_argument("--allow-behind", action="store_true",
                    help="record a baseline even when HEAD is behind origin/main")
    args = ap.parse_args(argv)

    if args.baseline:
        if not args.allow_behind:
            run(["git", "fetch", "-q", "origin"])
            behind = run(["git", "rev-list", "--count", "HEAD..origin/main"]).stdout.strip()
            if behind not in ("", "0"):
                print(f"error: HEAD is {behind} commits behind origin/main, so the workflow docs and "
                      "tools may be stale; rebase onto origin/main (or pass --allow-behind)", file=sys.stderr)
                return 2
        snap = snapshot()
        BASELINE.parent.mkdir(exist_ok=True)
        BASELINE.write_text(json.dumps(snap, indent=2))
        print(f"baseline: {snap['functions']:,} functions, {snap['code']:,} bytes, "
              f"{snap['strict_only_functions']} strict-only; saved to {BASELINE.relative_to(ROOT)}")
        return 0
    if not args.functions and not args.parked:
        ap.error("name the matched functions (or use --baseline)")

    base = json.loads(BASELINE.read_text()) if BASELINE.is_file() else None
    failures: list[str] = []
    if args.rebase:
        if run(["git", "fetch", "-q", "origin"]).returncode or run(["git", "rebase", "origin/main"]).returncode:
            print("error: rebase failed; resolve it, then rerun", file=sys.stderr)
            return 2
    b = run(["bash", "build.sh"])
    if b.returncode:
        print(b.stdout[-1500:] + b.stderr[-1500:], file=sys.stderr)
        print("error: build.sh failed", file=sys.stderr)
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
    print(f"\nSprint: `{args.unit or 'UNIT'}` ({sum(results.values())} matched, {len(parked)} parked)\n")
    print("| Function | Result |\n|---|---|")
    for fn, ok in results.items():
        print(f"| {fn} | {'VERIFIED' if ok else '**FAILED strict check**'} |")
    for fn, note in parked:
        print(f"| {fn} | parked {note} |")
    print("\n- Promoted: none | UNIT (bytes), verify_unit and linked range clean")
    if base:
        drift = " (includes main drift if rebased)" if args.rebase or base["head"] != snap["head"] else ""
        print(f"- Report: functions {base['functions']:,} → {snap['functions']:,}, code bytes "
              f"{base['code']:,} → {snap['code']:,}{drift}; strict-only "
              f"{base['strict_only_functions']} → {snap['strict_only_functions']} functions, "
              f"{base['strict_only_bytes']:,} → {snap['strict_only_bytes']:,} bytes")
    else:
        print(f"- Report: functions {snap['functions']:,}, code bytes {snap['code']:,} (no baseline saved)")
    print(f"- DOL: {'byte-identical' if same_dol else 'CHANGED'} (sha1 {str(snap['dol_sha1'])[:8]}…)")
    print("- Donor: <one line, or \"no OpenSaga hits\">\n- Headers: <fields added with their consumer, or none>")
    if failures:
        print(f"\nFAILED: {', '.join(failures)}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
