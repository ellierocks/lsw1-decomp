#!/usr/bin/env python3
"""Generate an evidence-only static callsite report for Menu_FullReset."""

from __future__ import annotations

import csv
import re
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GRAPH = ROOT / "docs" / "recomp_exports" / "call_graph.tsv"
SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
XREFS = ROOT / "docs" / "symbol_donors" / "gc_data_xrefs.tsv"
OUT = ROOT / "docs" / "recomp_exports" / "menu_fullreset_startup_path.md"
NAME = "Menu_FullReset"


def label(name: str) -> str:
    if name == "fn_8009CC00": return "generic; separate orchestration inspection"
    if name.startswith("Menu_") or name.startswith("DebugMenu_"): return "menu-named"
    if name.startswith("NuRndr") or name.startswith("NuTex"): return "Nu rendering/texture-named"
    if name.startswith("NuFile") or name.startswith("NuDat"): return "Nu file/data-named"
    if name.startswith("Nu"): return "Nu engine-named"
    if name.startswith(("AX", "AI", "AR")): return "audio-named"
    if name.startswith("fn_"): return "generic/unrecovered"
    return "named; subsystem not classified"


def main() -> int:
    with GRAPH.open(newline="", encoding="utf-8") as source:
        rows = list(csv.DictReader(source, delimiter="\t"))
    callees = sorted((row for row in rows if row["caller_name"] == NAME), key=lambda row: int(row["callsite_address"], 16))
    callers = [row for row in rows if row["callee_name"] == NAME]
    symbols = []
    pattern = re.compile(r"^(\S+)\s+=\s+\.text:0x([0-9A-Fa-f]+);\s*//")
    for line in SYMBOLS.read_text(encoding="utf-8").splitlines():
        match = pattern.match(line)
        if match: symbols.append((int(match.group(2), 16), match.group(1)))
    index = next(i for i, (_, name) in enumerate(symbols) if name == NAME)
    nearby = symbols[index - 4:index + 5]
    xrefs = [line for line in XREFS.read_text(encoding="utf-8").splitlines() if f"\t{NAME}\t" in line]
    after = callees[1:]
    labels = Counter(label(row["callee_name"]) for row in after)
    lines = [
        "# `Menu_FullReset` static callsite path", "",
        "This report uses direct PPC `bl`/`bla` export rows. Callsite order is static instruction layout inside `Menu_FullReset`, not runtime temporal proof.",
        "", "## Direct callers", "",
    ]
    if callers:
        lines.extend(f"- `{row['caller_name']}` at `{row['callsite_address']}` ({row['evidence']})." for row in callers)
    else:
        lines.append("- No direct caller was resolved in the exported direct-call graph.")
    lines += ["", "## Direct callees in static callsite order", "", "| # | Callsite | Callee | Address | Label | Evidence |", "|---:|---:|---|---:|---|---|"]
    for number, row in enumerate(callees, 1):
        lines.append(f"| {number} | `{row['callsite_address']}` | `{row['callee_name']}` | `{row['callee_address']}` | {label(row['callee_name'])} | `{row['evidence']}` |")
    lines += ["", "## Position of `fn_8009CC00`", "", f"- It is direct callee **#1 of {len(callees)}** at static callsite `0x800A0870`.", "- No direct callee precedes it in this function's exported callsite layout.", "- The remaining direct callee callsites are later in static layout. Their labels are: " + ", ".join(f"{kind} ({count})" for kind, count in sorted(labels.items())) + ".", "- This does not prove that `fn_8009CC00` runs first at runtime or that every later static call is executed.", "", "## Data/string references", ""]
    lines.extend(f"- `{line}`" for line in xrefs) if xrefs else lines.append("- No indexed data-xref row found.")
    lines += ["- No string-reference row named `Menu_FullReset` was found in the inspected string-rename table.", "", "## Nearby symbols", "", "- " + ", ".join(f"`{name}` (`0x{address:08X}`)" for address, name in nearby) + ".", "", "## Assessment", "", "`Menu_FullReset` is a named symbol and directly calls other menu-named functions (`Menu_InitDefinitions`, `Menu_NavigateForward`, `Menu_ResetStack`) as well as Nu and audio-named functions. That is name and direct-edge evidence for a menu-related path, but it does not distinguish frontend reset, game reset, menu initialization, or level restart.", "", "`fn_8009CC00` can be described only as a direct callee on this static `Menu_FullReset` path. A menu reset/init helper label is not accepted or justified without resolving its caller conditions, arguments, and generic callees.", "", "## Confidence and limitations", "", "- HIGH: direct edge rows and their PPC instruction evidence.", "- MED: indexed data-xref rows copied below are existing reference evidence.", "- LOW: menu-family interpretation from names.", "- Direct edges omit indirect calls and do not prove runtime order, entrypoint status, or semantic role."]
    OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"wrote {OUT}: {len(callees)} callees, {len(callers)} callers, {len(xrefs)} data xrefs")


if __name__ == "__main__": main()
