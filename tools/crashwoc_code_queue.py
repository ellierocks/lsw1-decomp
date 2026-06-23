#!/usr/bin/env python3
"""Generate donor-guided C matching candidates from CrashWOC source.

Unlike crashwoc_src_match.py, this joins the Crash ELF symbol name already
recorded in nu2_fn_matches.tsv directly to C definitions in a local CrashWOC
checkout. Crash source files do not consistently contain embedded map comments,
so using those comments drops valid exact-body candidates.

Usage:
    python3 tools/crashwoc_code_queue.py /tmp/crashwoc-decomp
    python3 tools/crashwoc_code_queue.py /tmp/crashwoc-decomp --top 20
"""

from __future__ import annotations

import argparse
import csv
import re
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MATCHES = ROOT / "docs/symbol_donors/nu2_fn_matches.tsv"
OUT_TSV = ROOT / "docs/symbol_donors/crashwoc_code_match_queue.tsv"
OUT_MD = ROOT / "docs/symbol_donors/crashwoc_code_match_queue.md"

SAFE_TOP_LEVELS = {"numath", "nu3dx", "nucore", "nusound", "gamelib"}

# Deliberately permissive: this only indexes a candidate's source location.
# The subsequent compiler/diff loop is the authority on whether a definition
# can be ported as-is.
FUNC_DEF_RE = re.compile(
    r"(?m)^\s*(?:static\s+)?[\w\s*]+?\s+([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{"
)


def source_definitions(src_dir: Path) -> dict[str, list[tuple[Path, int]]]:
    definitions: dict[str, list[tuple[Path, int]]] = defaultdict(list)
    for path in src_dir.rglob("*.c"):
        try:
            text = path.read_text(errors="replace")
        except OSError:
            continue
        for match in FUNC_DEF_RE.finditer(text):
            line = text.count("\n", 0, match.start()) + 1
            definitions[match.group(1)].append((path, line))
    return definitions


def is_safe_source(path: Path, src_dir: Path) -> bool:
    try:
        return path.relative_to(src_dir).parts[0] in SAFE_TOP_LEVELS
    except ValueError:
        return False


def load_candidates(
    definitions: dict[str, list[tuple[Path, int]]],
    src_dir: Path,
    min_size: int,
    match_types: set[str],
) -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    seen: set[tuple[str, str, str]] = set()
    # A crash_name matched to many distinct lsw1 targets is a generic idiom
    # (e.g. an 8-instruction stub), not a real one-to-one body match. Count
    # the lsw1 targets per crash_name first so those can be dropped.
    crash_targets: dict[str, set[str]] = defaultdict(set)
    with MATCHES.open(newline="", errors="replace") as handle:
        for row in csv.DictReader(handle, delimiter="\t"):
            if row["source"] == "crashwoc_retail" and row["match_type"] in match_types:
                crash_targets[row["crash_name"]].add(row["lsw1_fn"])

    with MATCHES.open(newline="", errors="replace") as handle:
        for row in csv.DictReader(handle, delimiter="\t"):
            if row["source"] != "crashwoc_retail" or row["match_type"] not in match_types:
                continue
            size = int(row["lsw1_size"], 0)
            if size < min_size:
                continue
            # Many-to-one crash names are ambiguous: skip them (islands of certainty).
            if len(crash_targets[row["crash_name"]]) != 1:
                continue
            locations = [
                (path, line)
                for path, line in definitions.get(row["crash_name"], [])
                if is_safe_source(path, src_dir)
            ]
            if len(locations) != 1:
                continue
            path, line = locations[0]
            key = (row["lsw1_fn"], row["crash_name"], str(path))
            if key in seen:
                continue
            seen.add(key)
            rows.append(
                {
                    "lsw1_fn": row["lsw1_fn"],
                    "lsw1_addr": row["lsw1_addr"],
                    "size": row["lsw1_size"],
                    "match_type": row["match_type"],
                    "crash_name": row["crash_name"],
                    "crash_addr": row["crash_addr"],
                    "source": str(path.relative_to(src_dir)),
                    "line": str(line),
                }
            )
    return sorted(rows, key=lambda row: (-int(row["size"], 0), row["lsw1_addr"]))


def write_outputs(rows: list[dict[str, str]], src_dir: Path) -> None:
    fields = [
        "lsw1_fn", "lsw1_addr", "size", "match_type",
        "crash_name", "crash_addr", "source", "line",
    ]
    with OUT_TSV.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, delimiter="\t")
        writer.writeheader()
        writer.writerows(rows)

    lines = [
        "# CrashWOC donor-guided code-match queue", "",
        "One-to-one LSW1/Crash body matches with a single safe Nu2 C definition in "
        "the local CrashWOC checkout. `exact` rows are byte-identical (mostly stubs); "
        "`norm` rows match after normalization and are the substantial candidates. "
        "Use these as source-port candidates, not as proof that the first C compile "
        "will match.", "",
        f"Source checkout: `{src_dir}`", "",
        "| LSW1 target | Size | Match | Crash function | Crash source |",
        "|---|---:|---|---|---|",
    ]
    for row in rows:
        lines.append(
            f"| `{row['lsw1_fn']}` @ `{row['lsw1_addr']}` | `{row['size']}` | "
            f"`{row['match_type']}` | `{row['crash_name']}` @ `{row['crash_addr']}` | "
            f"`{row['source']}:{row['line']}` |"
        )
    OUT_MD.write_text("\n".join(lines) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("src_dir", type=Path, help="CrashWOC checkout root")
    parser.add_argument("--min-size", type=lambda value: int(value, 0), default=0x20)
    parser.add_argument("--top", type=int, default=20, help="Rows to print (all rows are written)")
    parser.add_argument(
        "--include-norm",
        action="store_true",
        help="Also surface normalized matches (the substantial candidate tier); "
             "exact-only is mostly trivial stubs.",
    )
    args = parser.parse_args()

    src_dir = args.src_dir.resolve()
    source_root = src_dir / "src"
    if not source_root.is_dir():
        parser.error(f"missing Crash source directory: {source_root}")

    match_types = {"exact", "norm"} if args.include_norm else {"exact"}
    definitions = source_definitions(source_root)
    rows = load_candidates(definitions, source_root, args.min_size, match_types)
    write_outputs(rows, src_dir)

    print(f"Crash C definitions indexed: {sum(map(len, definitions.values()))}")
    print(f"Donor-guided candidates ({'+'.join(sorted(match_types))}): {len(rows)}")
    for row in rows[:args.top]:
        print(
            f"{row['lsw1_fn']:28} {row['size']:>6}  {row['match_type']:5}  "
            f"{row['crash_name']:32} {row['source']}:{row['line']}"
        )
    print(f"Wrote: {OUT_TSV.relative_to(ROOT)}")
    print(f"Wrote: {OUT_MD.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
