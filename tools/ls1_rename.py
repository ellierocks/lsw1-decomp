#!/usr/bin/env python3
"""Safely rename a symbol in symbols.txt and verify the build.

Searches Mac TSVs for name suggestions when called with --suggest.

Usage:
    python tools/ls1_rename.py fn_8001E76C NuAnimKeyLerp
    python tools/ls1_rename.py --check fn_8001E76C NuAnimKeyLerp
    python tools/ls1_rename.py --suggest fn_8001E76C
"""
import argparse
import csv
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"
MAC_TSVS = [
    ROOT / "docs/symbol_donors/mac_lsw1_demo_symbols.tsv",
    ROOT / "docs/symbol_donors/mac_lsw2_symbols.tsv",
]


def load_symtab_funcs():
    result = []
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
            size_str = line.split("size:")[1].split(";")[0].strip() if "size:" in line else ""
            try:
                size = int(size_str, 16) if size_str else 0
            except ValueError:
                size = 0
            result.append((addr, name, size, line))
    result.sort()
    return result


def find_mac_suggestions(fn_addr):
    """Look at surrounding named functions and suggest Mac names."""
    funcs = load_symtab_funcs()
    idx = None
    for i, (a, n, s, l) in enumerate(funcs):
        if a == fn_addr:
            idx = i
            break
    if idx is None:
        return []

    prev_name = funcs[idx - 1][1] if idx > 0 else None
    next_name = funcs[idx + 1][1] if idx + 1 < len(funcs) else None

    mac_syms = {}
    for tsv in MAC_TSVS:
        if not tsv.exists():
            continue
        with open(tsv) as f:
            for row in csv.reader(f, delimiter="\t"):
                if len(row) < 8:
                    continue
                if row[4].strip() != "text":
                    continue
                clean = row[7].strip()
                dem = row[8].strip() if len(row) > 8 else ""
                mac_syms[clean] = dem

    suggestions = []
    if prev_name:
        prefix = prev_name.rstrip("0123456789")
        for name in mac_syms:
            if name.startswith(prefix) and name != prev_name:
                suggestions.append((name, mac_syms[name]))
    if next_name:
        prefix = next_name.rstrip("0123456789")
        for name in mac_syms:
            if name.startswith(prefix) and name != next_name:
                suggestions.append((name, mac_syms[name]))

    return sorted(set(suggestions))


def main():
    parser = argparse.ArgumentParser(
        description="Safely rename a symbol in symbols.txt and verify the build."
    )
    parser.add_argument("old_name", nargs="?", help="Existing symbol name to rename")
    parser.add_argument("new_name", nargs="?", help="New symbol name")
    parser.add_argument("--check", action="store_true", help="Preview rename without applying")
    parser.add_argument("--suggest", metavar="FN", help="Search Mac TSVs for name suggestions for a function")
    args = parser.parse_args()

    # --suggest mode
    if args.suggest:
        funcs = load_symtab_funcs()
        target = args.suggest
        target_addr = None
        for a, n, s, l in funcs:
            if n == target or (target.startswith("0x") and a == int(target, 16)):
                target_addr = a
                break
        if target_addr is None:
            print(f"Not found: {target}")
            sys.exit(1)
        suggestions = find_mac_suggestions(target_addr)
        if suggestions:
            print(f"Mac name suggestions for {target} @ 0x{target_addr:08X}:\n")
            for name, dem in suggestions[:10]:
                if dem:
                    print(f"  {name}  ({dem})")
                else:
                    print(f"  {name}")
        else:
            print(f"No Mac suggestions for {target}")
        return

    # rename mode
    if not args.old_name or not args.new_name:
        parser.print_help()
        sys.exit(1)

    old_name, new_name = args.old_name, args.new_name
    check_only = args.check

    syms = SYMTAB.read_text()
    old_line = None
    for line in syms.splitlines():
        if line.startswith(f"{old_name} ="):
            old_line = line
            new_line = line.replace(f"{old_name} =", f"{new_name} =", 1)
            break

    if old_line is None:
        print(f"Error: '{old_name}' not found in symbols.txt")
        sys.exit(1)

    print(f"Old: {old_line}")
    print(f"New: {new_line}")

    if check_only:
        suggestions = find_mac_suggestions(
            [a for a, n, s, l in load_symtab_funcs() if n == old_name][0]
        )
        if suggestions:
            print(f"\nMac name suggestions for {old_name}:")
            for name, dem in suggestions[:5]:
                print(f"  → {name}")
        return

    confirm = input("Apply? [y/N] ")
    if confirm.lower() != "y":
        print("Cancelled")
        return

    syms = syms.replace(old_line, new_line, 1)
    SYMTAB.write_text(syms)
    print(f"Applied: {old_name} -> {new_name}")

    print("Verifying build...")
    result = subprocess.run(
        ["bash", "build.sh"], cwd=ROOT,
        capture_output=True, text=True, timeout=120
    )
    if result.returncode == 0:
        print("Build: OK")
    else:
        print("Build FAILED — reverting...")
        print(result.stderr[-300:])
        syms = SYMTAB.read_text()
        syms = syms.replace(new_line, old_line, 1)
        SYMTAB.write_text(syms)
        print("Reverted")


if __name__ == "__main__":
    main()
