#!/usr/bin/env python3
"""Refresh binary-mining artifacts used for GC symbol recovery."""

from __future__ import annotations

import argparse
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


@dataclass(frozen=True)
class Stage:
    name: str
    command: list[str]
    outputs: list[Path]
    required_inputs: list[Path]


STAGES = [
    Stage(
        "cross-lineage",
        [sys.executable, "tools/lsw1_cross_binary_lineage.py"],
        [
            Path("build/cross_binary_lineage/lineage_report.md"),
            Path("build/cross_binary_lineage/string_similarity.tsv"),
            Path("build/cross_binary_lineage/usa_1_1_vs_gh_branch_strings.tsv"),
        ],
        [
            Path("orig/GL5E4F/sys/main.dol"),
            Path("orig/ps2/ps2-retail/usa 1.1/LEGO Star Wars - The Video Game (USA) (v1.01).bin"),
            Path("orig/ps2/ps2-retail/usa 2.0 gh/LEGO Star Wars - The Video Game (USA) (v2.00).iso"),
            Path("orig/mac/mac-debug-symbols/LEGO Star Wars Demo"),
        ],
    ),
    Stage(
        "mac-anchor-queue",
        [sys.executable, "tools/mac_anchor_rename_queue.py"],
        [
            Path("docs/symbol_donors/mac_anchor_rename_queue.tsv"),
            Path("docs/symbol_donors/mac_anchor_rename_queue.md"),
        ],
        [
            Path("orig/mac/mac-debug-symbols/LEGO Star Wars Demo"),
            Path("orig/mac/mac-debug-symbols/LEGO Star Wars II"),
            Path("config/GL5E4F/symbols.txt"),
        ],
    ),
    Stage(
        "nu2-body-matches",
        [sys.executable, "tools/nu2_fn_match.py"],
        [
            Path("docs/symbol_donors/nu2_fn_matches.tsv"),
            Path("docs/symbol_donors/nu2_fn_rename_queue.tsv"),
            Path("docs/symbol_donors/nu2_gc_body_confirmations.tsv"),
            Path("docs/symbol_donors/lsw2_gc_body_matches.tsv"),
        ],
        [
            Path("orig/GL5E4F/sys/main.dol"),
            Path("config/GL5E4F/symbols.txt"),
            Path("orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/extracted/files/crashwoc.elf"),
            Path("orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/extracted/sys/main.dol"),
            Path("orig/nu2/Disney-Pixar Finding Nemo (USA)/extracted/files/GCNemo.elf"),
            Path("orig/nu2/Disney-Pixar Finding Nemo (USA)/extracted/sys/main.dol"),
        ],
    ),
    Stage(
        "pc-versions",
        [sys.executable, "tools/pc_versions_analysis.py"],
        [
            Path("build/pc_analysis/pc_analysis.txt"),
            Path("build/pc_analysis/pc_symbol_proposals.txt"),
        ],
        [
            Path("orig/GL5E4F/sys/main.dol"),
            Path("orig/pc/usa/LEGOStarWarsDemo/program files/Giant/LEGO Star Wars/LegoStarwars.exe"),
            Path("orig/pc/usa/retail/LEGO Star Wars Game/LegoStarwars.exe"),
        ],
    ),
    Stage(
        "gc-data-xrefs",
        [sys.executable, "tools/gc_data_xref_scanner.py"],
        [
            Path("docs/symbol_donors/gc_data_xrefs.tsv"),
            Path("docs/symbol_donors/mac_data_to_gc_verified.md"),
        ],
        [
            Path("orig/GL5E4F/sys/main.dol"),
            Path("config/GL5E4F/symbols.txt"),
            Path("orig/mac/mac-debug-symbols/LEGO Star Wars Demo"),
        ],
    ),
    Stage(
        "sdk-islands",
        [sys.executable, "tools/sdk_island_analysis.py"],
        [
            Path("docs/symbol_donors/sdk_islands.tsv"),
            Path("docs/symbol_donors/sdk_islands.md"),
        ],
        [
            Path("config/GL5E4F/symbols.txt"),
            Path("docs/symbol_donors/call_graph.tsv"),
        ],
    ),
    Stage(
        "sdk-anchor-queue",
        [sys.executable, "tools/sdk_anchor_rename_queue.py"],
        [
            Path("docs/symbol_donors/sdk_anchor_rename_queue.tsv"),
            Path("docs/symbol_donors/sdk_anchor_rename_queue.md"),
        ],
        [
            Path("config/GL5E4F/symbols.txt"),
            Path("docs/symbol_donors/sdk_symbol_sources/README.md"),
        ],
    ),
]


def missing_inputs(stage: Stage) -> list[Path]:
    return [p for p in stage.required_inputs if not (ROOT / p).exists()]


def run_stage(stage: Stage, dry_run: bool = False) -> bool:
    missing = missing_inputs(stage)
    if missing:
        print(f"[skip] {stage.name}: missing inputs")
        for path in missing:
            print(f"       - {path}")
        return False

    print(f"[run] {stage.name}: {' '.join(stage.command)}")
    if not dry_run:
        subprocess.run(stage.command, cwd=ROOT, check=True)
    for output in stage.outputs:
        status = "ok" if (ROOT / output).exists() else "missing"
        print(f"      {status}: {output}")
    return True


def write_index(stages: list[Stage]) -> None:
    out = ROOT / "build" / "binary_mining_index.md"
    out.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Binary Mining Index",
        "",
        "Generated by `python tools/binary_mining_pipeline.py`.",
        "",
        "## Stages",
    ]
    for stage in stages:
        missing = missing_inputs(stage)
        lines.append(f"- `{stage.name}`: {'ready' if not missing else 'missing inputs'}")
        if missing:
            for path in missing:
                lines.append(f"  - missing `{path}`")
        for output in stage.outputs:
            exists = (ROOT / output).exists()
            lines.append(f"  - {'ok' if exists else 'missing'} `{output}`")
    out.write_text("\n".join(lines) + "\n")
    print(f"[index] {out.relative_to(ROOT)}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "stages",
        nargs="*",
        help="Stages to run. Defaults to all. Use --list to show names.",
    )
    parser.add_argument("--list", action="store_true", help="List stages and exit.")
    parser.add_argument("--dry-run", action="store_true", help="Print commands without running.")
    args = parser.parse_args()

    if args.list:
        for stage in STAGES:
            print(stage.name)
        return

    stage_by_name = {stage.name: stage for stage in STAGES}
    selected = STAGES
    if args.stages:
        selected = []
        for name in args.stages:
            if name not in stage_by_name:
                known = ", ".join(stage_by_name)
                raise SystemExit(f"Unknown stage {name!r}. Known stages: {known}")
            selected.append(stage_by_name[name])

    for stage in selected:
        run_stage(stage, args.dry_run)
    write_index(STAGES)


if __name__ == "__main__":
    main()
