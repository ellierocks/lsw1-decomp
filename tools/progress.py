#!/usr/bin/env python3
"""
Progress tracking CLI for LEGO Star Wars: The Video Game decompilation.

Usage:
  python3 tools/progress.py              # Full report (symbols + matching)
  python3 tools/progress.py --symbols    # Symbol naming progress only
  python3 tools/progress.py --matching   # Matching progress only (objdiff report)
  python3 tools/progress.py --json       # JSON output for scripting
  python3 tools/progress.py --history    # Show progress history from git
  python3 tools/progress.py --report     # Auto-generate objdiff report first
"""

import argparse
import json
import os
import re
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

PROJECT_ROOT = Path(__file__).resolve().parent.parent
SYMBOLS_PATH = PROJECT_ROOT / "config" / "GL5E4F" / "symbols.txt"
REPORT_PATH = PROJECT_ROOT / "build" / "GL5E4F" / "report.json"
BASELINE_PATH = PROJECT_ROOT / "build" / "GL5E4F" / "baseline.json"
OBJDIFF_CLI = PROJECT_ROOT / "build" / "tools" / "objdiff-cli"


SECTION_LABELS = {
    "init": ".init",
    "text": ".text",
    "rodata": ".rodata",
    "data": ".data",
    "bss": ".bss",
    "sdata": ".sdata",
    "sbss": ".sbss",
    "sdata2": ".sdata2",
}

SECTION_ORDER = ["init", "text", "rodata", "data", "bss", "sdata", "sbss", "sdata2"]


def green(s: str) -> str:
    return f"\033[92m{s}\033[0m"


def yellow(s: str) -> str:
    return f"\033[93m{s}\033[0m"


def red(s: str) -> str:
    return f"\033[91m{s}\033[0m"


def cyan(s: str) -> str:
    return f"\033[96m{s}\033[0m"


def bold(s: str) -> str:
    return f"\033[1m{s}\033[0m"


def dim(s: str) -> str:
    return f"\033[2m{s}\033[0m"


def reset() -> str:
    return "\033[0m"


def read_symbols(path: Path) -> List[Dict[str, Any]]:
    symbols = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//") or line.startswith("#"):
                continue
            parts = line.split("=", 1)
            if len(parts) != 2:
                continue
            name = parts[0].strip()
            rest = parts[1].strip()
            sec_match = re.search(r"\.(\w+)\s*:", rest)
            section = sec_match.group(1) if sec_match else "unknown"
            type_match = re.search(r"type:(\w+)", rest)
            stype = type_match.group(1) if type_match else "unknown"
            size_match = re.search(r"size:(0x[0-9a-fA-F]+|\d+)", rest)
            size_str = size_match.group(1) if size_match else "0"
            size = int(size_str, 16) if size_str.startswith("0x") else int(size_str)

            is_named = not (
                name.startswith("fn_")
                or name.startswith("lbl_")
                or name.startswith("gap_")
            )
            is_function = stype in ("func", "function") or name.startswith("fn_")

            symbols.append(
                {
                    "name": name,
                    "section": section,
                    "type": stype,
                    "size": size,
                    "is_named": is_named,
                    "is_function": is_function,
                    "address": parts[0],
                }
            )
    return symbols


def compute_symbol_stats(symbols: List[Dict[str, Any]]) -> Dict[str, Any]:
    all_sections = {}
    for sym in symbols:
        sec = sym["section"]
        if sec not in all_sections:
            all_sections[sec] = {"total": 0, "named": 0, "bytes_total": 0, "bytes_named": 0}
        all_sections[sec]["total"] += 1
        all_sections[sec]["bytes_total"] += sym["size"]
        if sym["is_named"]:
            all_sections[sec]["named"] += 1
            all_sections[sec]["bytes_named"] += sym["size"]

    funcs_total = sum(1 for s in symbols if s["is_function"])
    funcs_named = sum(1 for s in symbols if s["is_function"] and s["is_named"])

    sections = {}
    for sec in SECTION_ORDER:
        if sec in all_sections:
            sections[sec] = all_sections[sec]

    totals = {"total": 0, "named": 0, "bytes_total": 0, "bytes_named": 0}
    for s in all_sections.values():
        totals["total"] += s["total"]
        totals["named"] += s["named"]
        totals["bytes_total"] += s["bytes_total"]
        totals["bytes_named"] += s["bytes_named"]

    return {
        "sections": sections,
        "functions": {"total": funcs_total, "named": funcs_named},
        "totals": totals,
    }


def read_report(path: Path) -> Optional[Dict[str, Any]]:
    if not path.is_file():
        return None
    with open(path) as f:
        data: Dict[str, Any] = json.load(f)
    measures = data.get("measures", {})

    def convert(d: Dict[str, Any]) -> None:
        for k, v in d.items():
            if isinstance(v, str) and v.isdigit():
                d[k] = int(v)

    convert(measures)
    return measures


def generate_report() -> bool:
    if not OBJDIFF_CLI.is_file():
        print(f"  {red('!')} objdiff-cli not found at {OBJDIFF_CLI}")
        print(f"    Run: python3 configure.py && ninja build/tools/objdiff-cli")
        return False

    config_path = PROJECT_ROOT / "build" / "GL5E4F" / "config.json"
    if not config_path.is_file():
        print(f"  {red('!')} config.json not found. Run dtk split first.")
        return False

    print(f"  Generating objdiff report...")
    result = subprocess.run(
        [str(OBJDIFF_CLI), "report", "generate", "-o", str(REPORT_PATH)],
        cwd=str(PROJECT_ROOT),
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(f"  {red('!')} objdiff-cli failed:\n{result.stderr}")
        return False
    return True


def fmt_pct(n: int, total: int) -> str:
    if total == 0:
        return "N/A"
    pct = n / total * 100
    if pct >= 50:
        return green(f"{pct:.1f}%")
    elif pct >= 20:
        return yellow(f"{pct:.1f}%")
    else:
        return red(f"{pct:.1f}%")


def print_symbol_progress(stats: Dict[str, Any]) -> None:
    print(f"  {bold('Symbol Naming Progress')}")
    print(f"  {'─' * 52}")
    print(f"  {'Section':<12} {'Named':>6} {'Total':>6} {'Bytes':>14}")
    print(f"  {'─' * 52}")

    for sec in SECTION_ORDER:
        s = stats["sections"].get(sec)
        if not s:
            continue
        label = SECTION_LABELS.get(sec, sec)
        pct_color = fmt_pct(s["named"], s["total"])
        bytes_str = f"{s['bytes_named']:,} / {s['bytes_total']:,}" if s["bytes_total"] else "N/A"
        bar = _bar(s["named"], s["total"], 12)
        print(f"  {label:<12} {s['named']:>6,} {s['total']:>6,}  {bar} {pct_color}")
        print(f"  {'':<12} {'':>6} {'':>6}  {dim(bytes_str)}")

    print(f"  {'─' * 52}")
    t = stats["totals"]
    pct_color = fmt_pct(t["named"], t["total"])
    bytes_str = f"{t['bytes_named']:,} / {t['bytes_total']:,}" if t["bytes_total"] else "N/A"
    bar = _bar(t["named"], t["total"], 12)
    print(f"  {bold('Total'):<12} {bold(str(t['named'])):>6} {bold(str(t['total'])):>6}  {bar} {pct_color}")
    print(f"  {'':<12} {'':>6} {'':>6}  {dim(bytes_str)}")

    print()
    f = stats["functions"]
    pct_color = fmt_pct(f["named"], f["total"])
    bar = _bar(f["named"], f["total"], 12)
    print(f"  {bold('Functions named')}: {f['named']:,} / {f['total']:,}  {bar} {pct_color}")


def print_matching_progress(measures: Dict[str, Any]) -> None:
    total_code = measures.get("total_code", 0)
    matched_code = measures.get("matched_code", 0)
    fuzzy_match_pct = measures.get("fuzzy_match_percent", 0)
    matched_code_pct = measures.get("matched_code_percent", 0)
    complete_code_pct = measures.get("complete_code_percent", 0)
    total_functions = measures.get("total_functions", 0)
    matched_functions = measures.get("matched_functions", 0)
    total_data = measures.get("total_data", 0)
    matched_data = measures.get("matched_data", 0)
    matched_data_pct = measures.get("matched_data_percent", 0)
    total_units = measures.get("total_units", 0)
    complete_units = measures.get("complete_units", 0)
    total_code_bytes = measures.get("total_code_bytes", 0)

    print(f"  {bold('Matching Progress')}")
    print(f"  {'─' * 52}")

    code_pct_str = f"{matched_code_pct:.1f}%" if matched_code_pct else "N/A"
    fuzzy_str = f"{fuzzy_match_pct:.1f}%" if fuzzy_match_pct else "N/A"
    complete_str = f"{complete_code_pct:.1f}%" if complete_code_pct else "N/A"
    print(
        f"  Code: {green(str(matched_code_pct)+'%') if matched_code_pct >= 50 else yellow(str(matched_code_pct)+'%') if matched_code_pct >= 20 else red(str(matched_code_pct)+'%') if matched_code_pct else dim('N/A')}"
    )
    print(f"    Matched: {matched_code:,} / {total_code:,} bytes")
    print(f"    Functions: {matched_functions:,} / {total_functions:,}")
    print(f"    Fuzzy: {fuzzy_str}")
    print(f"    Complete: {complete_str} ({complete_units} / {total_units} files)")

    if total_data:
        print()
        data_pct_str = f"{matched_data_pct:.1f}%" if matched_data_pct else "N/A"
        print(f"  Data: {data_pct_str}")
        print(f"    Matched: {matched_data:,} / {total_data:,} bytes")
        print(f"    Total: {total_data:,} bytes")


def print_history(symbols_path: Path) -> None:
    result = subprocess.run(
        ["git", "log", "--format=%H %ct", "--", str(symbols_path.relative_to(PROJECT_ROOT))],
        cwd=str(PROJECT_ROOT),
        capture_output=True,
        text=True,
    )
    if result.returncode != 0 or not result.stdout.strip():
        print(f"  {dim('No git history for symbols.txt')}")
        return

    commits = []
    for line in result.stdout.strip().split("\n"):
        if not line:
            continue
        parts = line.split()
        if len(parts) >= 2:
            commits.append((parts[0], int(parts[1])))

    snapshots = []
    for sha, ts in commits[:20]:
        content = subprocess.run(
            ["git", "show", f"{sha}:config/GL5E4F/symbols.txt"],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
        )
        if content.returncode != 0 or not content.stdout.strip():
            continue
        lines = content.stdout.strip().split("\n")
        total = 0
        named = 0
        for line in lines:
            line = line.strip()
            if not line or line.startswith("//") or line.startswith("#"):
                continue
            if "=" not in line:
                continue
            total += 1
            name = line.split("=")[0].strip()
            if not (name.startswith("fn_") or name.startswith("lbl_") or name.startswith("gap_")):
                named += 1
        snapshots.append((sha, ts, total, named))

    if not snapshots:
        print(f"  {dim('No history available')}")
        return

    print(f"  {bold('Symbol Naming History')}")
    print(f"  {'─' * 60}")
    print(f"  {'Date':<16} {'Named':>6} {'Total':>6} {'%':>6}")
    print(f"  {'─' * 60}")

    for sha, ts, total, named in snapshots:
        dt = datetime.fromtimestamp(ts).strftime("%Y-%m-%d")
        pct = named / total * 100 if total else 0
        print(f"  {dt:<16} {named:>6,} {total:>6,} {pct:>5.1f}%")

    if len(snapshots) >= 2:
        first = snapshots[-1]
        last = snapshots[0]
        named_delta = last["named" if False else 2] - first["named" if False else 2]
        print(f"  {'─' * 60}")
        print(f"  Net change: {named_delta:+d} named symbols")


def _bar(n: int, total: int, width: int) -> str:
    if total == 0:
        return dim("[" + " " * width + "]")
    filled = int(n / total * width)
    bar = "█" * filled + "░" * (width - filled)
    if n == 0:
        color = dim
    elif n == total:
        color = green
    elif filled >= width * 0.5:
        color = green
    elif filled >= width * 0.2:
        color = yellow
    else:
        color = red
    return color("[" + bar + "]")


def format_json(sym_stats: Dict[str, Any], matching: Optional[Dict[str, Any]]) -> str:
    sections_out = {}
    for sec in SECTION_ORDER:
        s = sym_stats["sections"].get(sec)
        if s:
            sections_out[sec] = {
                "named": s["named"],
                "total": s["total"],
                "percent": round(s["named"] / s["total"] * 100, 2) if s["total"] else 0,
                "bytes_named": s["bytes_named"],
                "bytes_total": s["bytes_total"],
            }

    out: Dict[str, Any] = {
        "symbols": {
            "named": sym_stats["totals"]["named"],
            "total": sym_stats["totals"]["total"],
            "percent": round(
                sym_stats["totals"]["named"] / sym_stats["totals"]["total"] * 100, 2
            )
            if sym_stats["totals"]["total"]
            else 0,
            "sections": sections_out,
        },
        "functions": {
            "named": sym_stats["functions"]["named"],
            "total": sym_stats["functions"]["total"],
            "percent": round(
                sym_stats["functions"]["named"] / sym_stats["functions"]["total"] * 100, 2
            )
            if sym_stats["functions"]["total"]
            else 0,
        },
    }

    if matching:
        out["matching"] = {
            "total_code": matching.get("total_code", 0),
            "matched_code": matching.get("matched_code", 0),
            "matched_code_percent": matching.get("matched_code_percent", 0),
            "fuzzy_match_percent": matching.get("fuzzy_match_percent", 0),
            "complete_code_percent": matching.get("complete_code_percent", 0),
            "total_functions": matching.get("total_functions", 0),
            "matched_functions": matching.get("matched_functions", 0),
            "total_data": matching.get("total_data", 0),
            "matched_data": matching.get("matched_data", 0),
            "total_units": matching.get("total_units", 0),
            "complete_units": matching.get("complete_units", 0),
        }

    return json.dumps(out, indent=2)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="LSW1 decompilation progress checker",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument(
        "--symbols", "-s",
        action="store_true",
        help="Show symbol naming progress only",
    )
    parser.add_argument(
        "--matching", "-m",
        action="store_true",
        help="Show matching progress only (requires report.json)",
    )
    parser.add_argument(
        "--json", "-j",
        action="store_true",
        help="Output as JSON",
    )
    parser.add_argument(
        "--history", "-H",
        action="store_true",
        help="Show symbol naming history from git",
    )
    parser.add_argument(
        "--report", "-r",
        action="store_true",
        help="Auto-generate objdiff report before display",
    )
    args = parser.parse_args()

    if args.report:
        generate_report()

    symbols = read_symbols(SYMBOLS_PATH)
    sym_stats = compute_symbol_stats(symbols)
    matching = read_report(REPORT_PATH)

    if args.json:
        print(format_json(sym_stats, matching))
        return

    if args.history:
        print_history(SYMBOLS_PATH)
        print()

    if args.matching and not args.symbols:
        if matching:
            print_matching_progress(matching)
        else:
            print(f"  {red('!')} No report.json found. Run with --report or: ninja progress")
            print(f"  Expected at: {REPORT_PATH}")
        return

    if args.symbols and not args.matching:
        print_symbol_progress(sym_stats)
        return

    print(f"  {bold('LSW1 Decompilation Progress')}")
    print(f"  {datetime.now().strftime('%Y-%m-%d %H:%M')}")
    print()

    print_symbol_progress(sym_stats)
    print()

    if matching:
        print_matching_progress(matching)
    else:
        print(f"  {dim('Matching progress: run with --report or use ninja progress')}")
        print(f"  {dim(f'  report.json: {REPORT_PATH}')}")

    baseline = read_report(BASELINE_PATH)
    if baseline and matching:
        print()
        print(f"  {bold('Baseline Comparison')}")
        code_delta = matching.get("matched_code", 0) - baseline.get("matched_code", 0)
        func_delta = matching.get("matched_functions", 0) - baseline.get("matched_functions", 0)
        sign = "+" if code_delta >= 0 else ""
        print(f"    Code matched: {sign}{code_delta:,} bytes since baseline")
        sign = "+" if func_delta >= 0 else ""
        print(f"    Functions: {sign}{func_delta:,} since baseline")

    print()


if __name__ == "__main__":
    main()
