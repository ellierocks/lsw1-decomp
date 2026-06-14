#!/usr/bin/env python3
"""Cross-reference a symbol across GC symbols.txt, Mac TSVs, and call graph.

Shows struct types from demangled Mac signatures and module context.

Usage:
    python tools/ls1_lookup_symbol.py NuAnimKeyLerp
    python tools/ls1_lookup_symbol.py 0x8001E76C
    python tools/ls1_lookup_symbol.py fn_8001E76C
"""
import argparse
import csv
import re
import sys
from collections import defaultdict
from pathlib import Path
from typing import Optional

ROOT = Path(__file__).resolve().parent.parent
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"
CALLGRAPH = ROOT / "docs/symbol_donors/call_graph.tsv"
MAC_TSVS = [
    ROOT / "docs/symbol_donors/mac_lsw1_demo_symbols.tsv",
    ROOT / "docs/symbol_donors/mac_lsw2_symbols.tsv",
]

MODULES = [
    ("nufile", 0x800034A0, 0x80006000),
    ("numem", 0x80006F74, 0x80007468),
    ("error", 0x80007468, 0x80008000),
    ("numath", 0x80008000, 0x80012000),
    ("nuanim", 0x80016A00, 0x80024000),
    ("render", 0x80024000, 0x8005C000),
    ("scene", 0x8005C000, 0x80090000),
    ("nusound", 0x80090000, 0x800B0000),
    ("gamelib", 0x800B0000, 0x80100000),
    ("gamecode", 0x80100000, 0x8018CB00),
]


def find_in_symtab(query: str, addr_mode: bool = False) -> Optional[dict]:
    with open(SYMTAB) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            name = line.split("=")[0].strip()
            if addr_mode:
                addr_str = f"0x{int(query, 16):08X}"
                if addr_str in line:
                    return {"name": name, "line": line}
            else:
                if name == query:
                    return {"name": name, "line": line}
    return None


def find_in_mac(query: str) -> list:
    results = []
    for tsv_path in MAC_TSVS:
        if not tsv_path.exists():
            continue
        source = tsv_path.stem.replace("mac_", "").replace("_symbols", "")
        with open(tsv_path) as f:
            reader = csv.reader(f, delimiter="\t")
            for row in reader:
                if len(row) < 8:
                    continue
                clean_name = row[7].strip()
                demangled = row[8].strip() if len(row) > 8 else ""
                if clean_name.endswith(".eh"):
                    continue
                if query in clean_name or query in demangled:
                    results.append({"source": source, "name": clean_name, "demangled": demangled})
    return results


def find_in_callgraph(query: str, addr_mode: bool = False) -> dict:
    calls, called_by = [], []
    with open(CALLGRAPH) as f:
        for line in f:
            if line.startswith("#"):
                continue
            parts = line.strip().split("\t")
            if len(parts) < 4:
                continue
            cn = parts[0].strip()
            an = parts[1].strip()
            bn = parts[2].strip()
            callee = parts[3].strip()
            target = query if not addr_mode else f"fn_{int(query, 16):08X}"
            if callee == target:
                calls.append(f"{cn} @ {an}")
            if cn == target:
                called_by.append(f"{callee} @ {bn}")
    return {"calls": calls[:8], "called_by": called_by[:8]}


def extract_struct_types(clean_name: str, demangled: str = "") -> list[str]:
    """Extract struct type names from a C++ mangled or demangled signature.

    Handles mangled like _Z17NuDatFileFindTreeP10nudathdr_sPc
    and demangled like NuDatFileFindTree(nudathdr_s*, char*).
    """
    types = set()
    if demangled:
        types.update(re.findall(r'(nu[a-z]+_s|NU[A-Z]+_s)', demangled))
    types.update(re.findall(r'(\d+[a-z]+_s)', clean_name))
    types.update(re.findall(r'(nu[a-z]+_s|NU[A-Z]+_s)', clean_name))
    return sorted(types)


def module_for_addr(addr: int) -> Optional[str]:
    for name, lo, hi in MODULES:
        if lo <= addr < hi:
            return name
    return None


def load_sorted_funcs():
    funcs = []
    with open(SYMTAB) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line or "type:function" not in line:
                continue
            name = line.split("=")[0].strip()
            rest = line.split("=", 1)[1].strip()
            if ";" in rest:
                rest = rest.split(";")[0].strip()
            loc = rest.rsplit(":", 1)[-1] if ":" in rest else rest
            try:
                addr = int(loc, 16)
            except ValueError:
                continue
            funcs.append((addr, name, line))
    funcs.sort()
    return funcs


def main():
    parser = argparse.ArgumentParser(
        description="Cross-reference a symbol across GC symbols.txt, Mac TSVs, and call graph."
    )
    parser.add_argument("query", help="Function name, fn_ADDR, or hex address (0x...)")
    args = parser.parse_args()

    query = args.query.strip()
    addr_mode = query.startswith("0x")

    sym = find_in_symtab(query, addr_mode)
    mac = find_in_mac(query)
    cg = find_in_callgraph(query, addr_mode)

    print(f"# {query}\n")

    if sym:
        addr_str = ""
        if not addr_mode and ":" in sym["line"]:
            m = re.search(r'0x[0-9A-Fa-f]+', sym["line"].split(":")[-1])
            if m:
                addr_str = m.group()
        elif addr_mode:
            addr_str = query

        print(f"## GC symbols.txt")
        print(f"  {sym['line']}")
        if addr_str:
            mod = module_for_addr(int(addr_str, 16))
            if mod:
                print(f"  Module: {mod}")
        print()

    if mac:
        print(f"## Mac symbols ({len(mac)} match{'es' if len(mac) > 1 else ''})")
        for m in mac:
            out = f"  [{m['source']}] {m['name']}"
            all_types = extract_struct_types(m["name"], m["demangled"])
            if m["demangled"]:
                out += f"\n    Signature: {m['demangled']}"
            elif all_types:
                out += f"\n    Mangled: {m['name']}"
            if all_types:
                out += f"\n    Structs: {', '.join(all_types)}"
            print(out)
        print()

    if cg["calls"] or cg["called_by"]:
        print("## Call graph")
        for c in cg["calls"]:
            print(f"  ← {c}")
        for c in cg["called_by"]:
            print(f"  → {c}")
        print()

    if not sym and not mac and not cg["calls"] and not cg["called_by"]:
        print("No results found in symbols.txt, Mac TSVs, or call graph.")

    # GC neighbors (only for named symbol lookup, not hex)
    if sym and not addr_mode:
        all_funcs = load_sorted_funcs()
        for i, (a, n, l) in enumerate(all_funcs):
            if n == query:
                if i > 0:
                    print(f"  Previous: {all_funcs[i-1][1]} @ 0x{all_funcs[i-1][0]:08X}")
                if i + 1 < len(all_funcs):
                    print(f"  Next:     {all_funcs[i+1][1]} @ 0x{all_funcs[i+1][0]:08X}")
                break


if __name__ == "__main__":
    main()
