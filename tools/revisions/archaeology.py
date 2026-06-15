#!/usr/bin/env python3
"""
Revision archaeology — compare pairs of builds and report differences.

For each build pair with available executables, produces a report covering:
  - Added / removed strings
  - Added / removed source file paths
  - Added / removed Nu debug message patterns
  - Executable size delta
  - String count delta

For the Xbox demo (loose files): additionally reports added/removed files
relative to another build's file inventory.

Output: research/revisions/archaeology_reports/<a>_vs_<b>.md

Usage:
    python3 tools/revisions/archaeology.py [build_a build_b]
    python3 tools/revisions/archaeology.py  # generates all priority pairs
"""
from __future__ import annotations

import csv
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILDS_TSV = ROOT / "research" / "revisions" / "builds.tsv"
HASHES_TSV = ROOT / "research" / "revisions" / "hashes.tsv"
OUT_DIR = ROOT / "research" / "revisions" / "archaeology_reports"

MIN_STRING_LEN = 8

SOURCE_RE = re.compile(r"[A-Za-z0-9_./\\-]+\.(c|cpp|h|cc)$", re.IGNORECASE)
NU_RE = re.compile(r"^(Nu[A-Z][A-Za-z0-9_]+|instNu[A-Z])", re.IGNORECASE)
ASSERT_RE = re.compile(
    r"(assert|ASSERT|cannot|Unable|failed|internal error|not implemented|"
    r"buffer overflow|out of memory|null pointer)",
    re.IGNORECASE,
)

PRIORITY_PAIRS = [
    ("ps2_prototype", "ps2_eu_demo"),
    ("ps2_eu_demo", "ps2_eu_retail_v1"),
    ("ps2_eu_retail_v1", "ps2_us_retail_v1"),
    ("ps2_us_retail_v1", "ps2_us_retail_gh"),
    ("gc_us_retail", "gc_uk_retail"),
    ("gc_us_retail", "ps2_us_retail_v1"),
    ("xbox_demo_oxm045", "xbox_retail"),
    ("pc_us_demo", "pc_us_retail"),
    ("gc_us_retail", "xbox_demo_oxm045"),
    ("gc_us_retail", "pc_us_retail"),
]


def load_builds() -> dict[str, dict]:
    with open(BUILDS_TSV, newline="", encoding="utf-8") as f:
        return {r["build_id"]: r for r in csv.DictReader(f, delimiter="\t")}


def load_hashes() -> dict[str, list[dict]]:
    result: dict[str, list[dict]] = {}
    if not HASHES_TSV.exists():
        return result
    with open(HASHES_TSV, newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            result.setdefault(row["build_id"], []).append(row)
    return result


def get_strings(path: Path) -> set[str]:
    r = subprocess.run(
        ["strings", "-n", str(MIN_STRING_LEN), str(path)],
        capture_output=True, text=True,
    )
    return set(r.stdout.splitlines())


def categorize_strings(strings: set[str]) -> dict[str, set[str]]:
    return {
        "source_paths": {s for s in strings if SOURCE_RE.search(s)},
        "nu_debug": {s for s in strings if NU_RE.search(s)},
        "assert_error": {s for s in strings if ASSERT_RE.search(s)},
        "all": strings,
    }


def compare_builds(a: dict, b: dict, hashes: dict) -> str:
    aid, bid = a["build_id"], b["build_id"]
    out = [
        f"# Archaeology: `{aid}` vs `{bid}`",
        "",
        f"**{a.get('variant','')} {a.get('version','')} {a.get('date','')}**  →  "
        f"**{b.get('variant','')} {b.get('version','')} {b.get('date','')}**",
        "",
    ]

    exe_a_raw = a.get("exe_path", "").strip()
    exe_b_raw = b.get("exe_path", "").strip()
    exe_a = ROOT / exe_a_raw if exe_a_raw else None
    exe_b = ROOT / exe_b_raw if exe_b_raw else None

    if not (exe_a and exe_a.exists() and exe_b and exe_b.exists()):
        out.append("_Executable(s) not available — cannot compare._")
        return "\n".join(out)

    size_a = exe_a.stat().st_size
    size_b = exe_b.stat().st_size
    out += [
        "## Executable",
        "",
        f"| | `{aid}` | `{bid}` |",
        "|-|---------|---------|",
        f"| Size | {size_a:,} bytes | {size_b:,} bytes |",
        f"| Delta | | {size_b - size_a:+,} bytes |",
        "",
    ]

    sa = get_strings(exe_a)
    sb = get_strings(exe_b)
    cat_a = categorize_strings(sa)
    cat_b = categorize_strings(sb)

    out += [
        "## String Overview",
        "",
        f"| Category | `{aid}` | `{bid}` | Only in A | Only in B |",
        "|----------|---------|---------|-----------|-----------|",
    ]
    for cat in ("all", "source_paths", "nu_debug", "assert_error"):
        ca, cb = cat_a[cat], cat_b[cat]
        only_a = ca - cb
        only_b = cb - ca
        out.append(
            f"| {cat} | {len(ca)} | {len(cb)} | {len(only_a)} | {len(only_b)} |"
        )

    # Added source paths
    added_src = cat_b["source_paths"] - cat_a["source_paths"]
    removed_src = cat_a["source_paths"] - cat_b["source_paths"]
    if added_src or removed_src:
        out += ["", "## Source File Changes", ""]
        if added_src:
            out.append("**Added source paths:**")
            for s in sorted(added_src)[:40]:
                out.append(f"- `{s}`")
        if removed_src:
            out.append("")
            out.append("**Removed source paths:**")
            for s in sorted(removed_src)[:40]:
                out.append(f"- `{s}`")

    # Nu debug string changes
    added_nu = cat_b["nu_debug"] - cat_a["nu_debug"]
    removed_nu = cat_a["nu_debug"] - cat_b["nu_debug"]
    if added_nu or removed_nu:
        out += ["", "## Nu Engine Debug String Changes", ""]
        if added_nu:
            out.append("**Added:**")
            for s in sorted(added_nu)[:60]:
                out.append(f"- `{s}`")
        if removed_nu:
            out.append("")
            out.append("**Removed:**")
            for s in sorted(removed_nu)[:60]:
                out.append(f"- `{s}`")

    # Other notable added/removed strings (assert/error)
    added_assert = cat_b["assert_error"] - cat_a["assert_error"]
    removed_assert = cat_a["assert_error"] - cat_b["assert_error"]
    if added_assert or removed_assert:
        out += ["", "## Assert / Error Message Changes", ""]
        if added_assert:
            out.append("**Added:**")
            for s in sorted(added_assert)[:40]:
                out.append(f"- `{s}`")
        if removed_assert:
            out.append("")
            out.append("**Removed:**")
            for s in sorted(removed_assert)[:40]:
                out.append(f"- `{s}`")

    # File inventory comparison (for builds with hashed assets)
    ha = {r["rel_path"].split("/", 3)[-1]: r["sha256"]
          for r in hashes.get(aid, []) if r["file_role"] == "asset"}
    hb = {r["rel_path"].split("/", 3)[-1]: r["sha256"]
          for r in hashes.get(bid, []) if r["file_role"] == "asset"}
    if ha or hb:
        files_a = set(ha)
        files_b = set(hb)
        added_files = files_b - files_a
        removed_files = files_a - files_b
        changed_files = {
            f for f in files_a & files_b if ha[f] != hb[f]
        }
        out += [
            "", "## File Inventory Changes", "",
            f"| | `{aid}` | `{bid}` |",
            "|-|---------|---------|",
            f"| Files | {len(files_a)} | {len(files_b)} |",
            f"| Added | | {len(added_files)} |",
            f"| Removed | {len(removed_files)} | |",
            f"| Changed | | {len(changed_files)} |",
        ]
        if added_files:
            out.append("\n**Added files:**")
            for f in sorted(added_files)[:30]:
                out.append(f"- `{f}`")
        if removed_files:
            out.append("\n**Removed files:**")
            for f in sorted(removed_files)[:30]:
                out.append(f"- `{f}`")
        if changed_files:
            out.append("\n**Changed files:**")
            for f in sorted(changed_files)[:30]:
                out.append(f"- `{f}`")

    return "\n".join(out)


def main():
    builds = load_builds()
    hashes = load_hashes()
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    if len(sys.argv) == 3:
        pairs = [(sys.argv[1], sys.argv[2])]
    else:
        pairs = PRIORITY_PAIRS

    for aid, bid in pairs:
        if aid not in builds or bid not in builds:
            print(f"  skipping {aid} vs {bid} — not in builds.tsv", file=sys.stderr)
            continue
        a, b = builds[aid], builds[bid]
        avail_a = a.get("available", "").strip() == "yes"
        avail_b = b.get("available", "").strip() == "yes"
        if not (avail_a and avail_b):
            print(f"  skipping {aid} vs {bid} — one or both unavailable")
            continue

        print(f"  comparing {aid} vs {bid} ...")
        report = compare_builds(a, b, hashes)
        out_path = OUT_DIR / f"{aid}_vs_{bid}.md"
        out_path.write_text(report, encoding="utf-8")
        print(f"    -> {out_path.name}")


if __name__ == "__main__":
    main()
