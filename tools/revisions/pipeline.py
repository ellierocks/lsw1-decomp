#!/usr/bin/env python3
"""
LSW1 Revision Research Pipeline — orchestrator.

Stages (run in order or individually):
  extract    — extract executables from disc images; update builds.tsv
  index      — hash all files per build; write hashes.tsv
  similarity — compute pairwise similarity; write similarity_matrix.tsv
  graph      — generate revision_graph.md + .dot
  archaeology — generate per-pair archaeology reports
  leads      — generate RE leads (rename candidates, symbol leads, subsystems)

Usage:
    python3 tools/revisions/pipeline.py [stage ...]
    python3 tools/revisions/pipeline.py all
    python3 tools/revisions/pipeline.py extract similarity graph
"""
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOOLS = ROOT / "tools" / "revisions"

STAGES = ["extract", "index", "similarity", "graph", "archaeology", "leads"]


def run_stage(name: str, extra_args: list[str] = []):
    script = TOOLS / f"{name}.py"
    cmd = [sys.executable, str(script)] + extra_args
    print(f"\n{'='*60}")
    print(f"  STAGE: {name}")
    print(f"{'='*60}")
    result = subprocess.run(cmd, cwd=ROOT)
    if result.returncode != 0:
        print(f"  [ERROR] Stage '{name}' failed (exit {result.returncode})", file=sys.stderr)
        return False
    return True


def main():
    args = sys.argv[1:]
    if not args or args[0] == "all":
        stages = STAGES
    else:
        stages = [a for a in args if a in STAGES]
        unknown = [a for a in args if a not in STAGES]
        if unknown:
            print(f"Unknown stages: {unknown}", file=sys.stderr)
            print(f"Available: {', '.join(STAGES)}", file=sys.stderr)
            sys.exit(1)

    print(f"Running stages: {', '.join(stages)}")

    for stage in stages:
        ok = run_stage(stage)
        if not ok and stage in ("extract", "similarity"):
            # Non-fatal — continue with remaining stages
            print(f"  Continuing despite {stage} failure.")

    print("\nPipeline complete.")


if __name__ == "__main__":
    main()
