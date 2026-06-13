#!/usr/bin/env python3
"""
Source-file clustering: infer likely original .c files from function naming
patterns and call graph connectivity.

Outputs:
  docs/symbol_donors/proposed_splits.md  — report + suggested splits.txt
"""

import re
from collections import defaultdict, Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
CALL_GRAPH = ROOT / "docs/symbol_donors/call_graph.tsv"
OUT_DIR = ROOT / "docs/symbol_donors"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s*//\s*(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"size:0x([0-9A-Fa-f]+)")


MODULE_RULES = [
    (r"^Action_", "ai/AIScriptActions"),
    (r"^Condition_", "ai/AIScriptConditions"),
    (r"^AIScript", "ai/AIScript"),
    (r"^Menu_", "menu/MenuSystem"),
    (r"^PauseMenu_", "menu/PauseMenu"),
    (r"^DebugMenu_", "menu/DebugMenu"),
    (r"^Nu(?!Mem|File|Dat|Anim|Rndr|PPLoad|SpecialFind)", "nu2/NuEngine"),
    (r"^NuMem", "nu2/NuMemory"),
    (r"^NuFile", "nu2/NuFile"),
    (r"^NuDat", "nu2/NuData"),
    (r"^NuAnim", "nu2/NuAnimation"),
    (r"^NuRndr", "nu2/NuRender"),
    (r"^Player_", "game/Player"),
    (r"^Podrace", "game/Podrace"),
    (r"^Credit", "game/Credits"),
    (r"^GX", "gx/GX"),
    (r"^__GX", "gx/GX"),
    (r"^PPC", "runtime/PPC"),
    (r"^__", "runtime/compiler"),
    (r"^Mem", "runtime/Memory"),
    (r"^UI_", "ui/UI"),
    (r"^Pad", "input/Pad"),
]


def module_for_name(name):
    for pattern, module in MODULE_RULES:
        if re.match(pattern, name):
            return module
    return None


def parse_symbols():
    funcs = []  # list of (addr, name, size)
    for line in open(GC_SYMBOLS):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        m = SYMBOL_RE.match(line)
        if not m:
            continue
        name = m.group("name")
        addr = int(m.group("addr"), 16)
        sec = m.group("section")
        meta = m.group("meta")
        if sec != ".text" or name.startswith("lbl_"):
            continue
        sm = SIZE_RE.search(meta)
        size = int(sm.group(1), 16) if sm else 0
        funcs.append((addr, name, size))
    funcs.sort()
    return funcs


def load_call_graph():
    callers = defaultdict(set)
    callees = defaultdict(set)
    for line in open(CALL_GRAPH):
        if line.startswith("#"):
            continue
        parts = line.strip().split("\t")
        if len(parts) < 4:
            continue
        caller, _, _, callee = parts
        callers[callee].add(caller)
        callees[caller].add(callee)
    return callers, callees


def main():
    print("=" * 60)
    print("Source-file Clustering")
    print("=" * 60)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    funcs = parse_symbols()
    print(f"[1] {len(funcs)} text symbols")

    gc_callers, gc_callees = load_call_graph()
    print(f"[2] Call graph loaded")

    # Assign modules to each function
    func_modules = {}  # name -> module or None
    named_counts = Counter()

    for addr, name, size in funcs:
        mod = module_for_name(name)
        if mod:
            func_modules[name] = mod
            named_counts[mod] += 1

    named_funcs = {n for n, m in func_modules.items() if m is not None}
    unnamed = [f for f in funcs if f[1] not in func_modules]

    print(f"    Named functions: {len(named_funcs)}")
    print(f"    Unnamed: {len(unnamed)}")
    print(f"    Modules: {len(named_counts)}")

    # Propagate modules to unnamed functions via call graph
    for addr, name, size in unnamed:
        # Check callees and callers for known modules
        callee_mods = Counter()
        for callee in gc_callees.get(name, set()):
            mod = func_modules.get(callee)
            if mod:
                callee_mods[mod] += 1
        caller_mods = Counter()
        for caller in gc_callers.get(name, set()):
            mod = func_modules.get(caller)
            if mod:
                caller_mods[mod] += 1

        # Weight: callee association stronger than caller
        weighted = Counter()
        for m, c in callee_mods.items():
            weighted[m] += c * 2
        for m, c in caller_mods.items():
            weighted[m] += c

        if weighted:
            # Pick best module
            best_mod = weighted.most_common(1)[0][0]
            func_modules[name] = best_mod

    # Build address -> module map
    addr_modules = {}
    for addr, name, size in funcs:
        mod = func_modules.get(name)
        if mod:
            addr_modules[addr] = mod

    print(f"    After propagation: {len(addr_modules)}/{len(funcs)} assigned")

    # Cluster by contiguous address ranges with same module
    # Sort functions by address
    sorted_funcs = [(a, n, s) for a, n, s in funcs]

    # Assign module to all functions (even unknown ones get "unknown")
    for addr, name, size in funcs:
        if name not in func_modules:
            func_modules[name] = "_unknown"

    # Create contiguously-ranged modules
    # Group consecutive functions with the same module
    clusters = []
    current_mod = None
    current_start = None
    current_end = None
    current_names = []

    for addr, name, size in funcs:
        mod = func_modules[name]
        if mod != current_mod:
            if current_mod is not None:
                clusters.append((current_mod, current_start, current_end, current_names))
            current_mod = mod
            current_start = addr
            current_end = addr + size
            current_names = [name]
        else:
            current_end = addr + size
            current_names.append(name)
    if current_mod is not None:
        clusters.append((current_mod, current_start, current_end, current_names))

    print(f"\n[3] {len(clusters)} contiguous module clusters")

    # Report by module
    module_clusters = defaultdict(list)
    for c_mod, start, end, names in clusters:
        module_clusters[c_mod].append((start, end, names))

    lines = [
        "# Proposed Source-file Clusters",
        "",
        f"Based on {len(funcs)} text symbols, {len(named_counts)} named modules.",
        f"Call-graph propagation assigned {len(addr_modules)}/{len(funcs)} functions.",
        "",
        "---",
        "## Proposed splits.txt entries",
        "",
        "Format: `module/path.c:0xADDRESS`",
        "",
    ]

    for mod in sorted(set(c[0] for c in clusters)):
        if mod == "_unknown":
            continue
        mod_items = [(s, e, n) for m, s, e, n in clusters if m == mod]
        lines.append(f"### {mod}")
        lines.append("")
        lines.append("| Address | Size | Functions |")
        lines.append("|---------|------|-----------|")
        for start, end, names in mod_items[:20]:
            size = end - start
            count = len(names)
            sample = ", ".join(names[:5])
            if len(names) > 5:
                sample += f" ... +{len(names)-5} more"
            lines.append(f"| 0x{start:08X} | 0x{size:X} | {count} funcs: {sample[:80]} |")
        if len(mod_items) > 20:
            lines.append(f"| ... | | +{len(mod_items)-20} more clusters |")
        lines.append("")

    lines.extend(["", "---", "## Summary", "",
        "| Module | Clusters | Total Funcs |", 
        "|--------|----------|-------------|",
    ])

    for mod, cluster_list in sorted(module_clusters.items()):
        if mod == "_unknown":
            continue
        total_funcs = sum(len(n) for _, _, n in cluster_list)
        lines.append(f"| {mod} | {len(cluster_list)} | {total_funcs} |")

    unknown_count = sum(len(n) for m, _, _, n in clusters if m == "_unknown")
    lines.append(f"| _unknown | - | {unknown_count} |")

    report_path = OUT_DIR / "proposed_splits.md"
    report_path.write_text("\n".join(lines) + "\n")
    print(f"    Wrote {report_path}")

    # Write actual splits.txt format
    splits_lines = []
    for mod, start, end, names in clusters:
        if mod == "_unknown":
            continue
        if len(names) < 3:
            continue
        # splits.txt format: path/file.c:0xADDRESS
        splits_lines.append(f"{mod}.c:0x{start:08X}")

    splits_path = OUT_DIR / "proposed_splits.txt"
    splits_path.write_text("\n".join(splits_lines) + "\n")
    print(f"    Wrote {splits_path} ({len(splits_lines)} entries)")

    print(f"\n{'=' * 60}")
    print("Done.")


if __name__ == "__main__":
    main()
