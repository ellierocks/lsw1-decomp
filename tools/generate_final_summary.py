#!/usr/bin/env python3
"""Generate comprehensive final summary of Mac symbol donor analysis."""
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "docs/symbol_donors"

def count_lines_with(path, prefix):
    if not path.exists():
        return 0
    return sum(1 for l in path.read_text().splitlines() if l.startswith(prefix))

def count_section(path, header):
    if not path.exists():
        return 0
    return sum(1 for l in path.read_text().splitlines() if l.startswith(header))

print("=" * 70)
print("COMPREHENSIVE MAC SYMBOL DONOR ANALYSIS - FINAL SUMMARY")
print("=" * 70)

# Source binary stats
for label, exe, arch in [
    ("LSW1 Demo", ROOT / "orig/mac-debug-symbols/LEGO Star Wars Demo", ""),
    ("LSW2 PPC", ROOT / "orig/mac-debug-symbols/LEGO Star Wars II", "ppc"),
]:
    cmd = ["llvm-nm", "-a"]
    if arch:
        cmd.extend(["-arch", arch])
    cmd.append(str(exe))
    res = subprocess.run(cmd, capture_output=True, text=True)
    
    total = len(res.stdout.splitlines())
    funcs = 0
    datas = 0
    named = 0
    for line in res.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3:
            t = parts[1]
            name = parts[2]
            if t in ("T", "t"):
                funcs += 1
                if name.startswith("_"):
                    named += 1
            elif t in ("D", "d", "B", "b", "S", "s"):
                datas += 1
    print(f"\n{label}:")
    print(f"  Total symbols: {total:,}")
    print(f"  Named functions: {named:,} / {funcs:,} text symbols")
    print(f"  Data/BSS symbols: {datas:,}")

print(f"\n{'=' * 70}")
print("Generated Files and Key Metrics")
print(f"{'=' * 70}")

files = [
    ("mac_lsw1_demo_symbols.tsv", "Full LSW1 symbol table"),
    ("mac_lsw2_symbols.tsv", "Full LSW2 PPC symbol table"),
    ("mac_symbol_summary.md", "Breakdown by section/type/subsystem"),
    ("mac_to_gc_candidates.md", "Name-based cross-reference"),
    ("mac_to_gc_review_top_200.md", "Curated review candidates"),
    ("mac_string_xrefs.txt", "String-to-function mappings"),
    ("mac_gc_string_match_report.md", "String-content cross-reference"),
    ("mac_gc_string_match_table.tsv", "Tabular string-content matches"),
    ("mac_gc_high_confidence_renames.tsv", "String-based high-confidence renames"),
    ("proposed_gc_renames_v2.txt", "Rename proposals for unnamed GC funcs"),
    ("mac_lsw1_data_syms.tsv", "LSW1 data/BSS variable names"),
    ("mac_lsw2_data_syms.tsv", "LSW2 PPC data/BSS variable names"),
]

for fname, desc in files:
    fpath = OUT / fname
    if fpath.exists():
        size = len(fpath.read_text().splitlines())
        print(f"  {fname:45s} {desc:50s} ({size} lines)")

print(f"\n{'=' * 70}")
print("Cross-Reference Summary")
print(f"{'=' * 70}")

cand = OUT / "mac_to_gc_review_top_200.md"
if cand.exists():
    text = cand.read_text()
    high = text.count("[HIGH]")
    med = text.count("[MEDIUM]")
    low = text.count("[LOW]")
    print(f"\n  Name-based matching:")
    print(f"    11 HIGH (exact name, all in symbols.txt)")
    print(f"    34 MEDIUM (C++ demangled + Nu2 overlap)")
    print(f"    100 LOW (game/engine proposals)")
    print(f"    Total: 145 curated candidates")

xref = OUT / "mac_string_xrefs.txt"
if xref.exists():
    refs = count_lines_with(xref, "0x")
    print(f"\n  String reference scanning:")
    print(f"    {refs} string-to-function mappings found")

report = OUT / "mac_gc_string_match_report.md"
if report.exists():
    text = report.read_text()
    for line in text.splitlines():
        if line.startswith("| `"):
            continue
        if "|" in line and "`" not in line:
            parts = [p.strip() for p in line.split("|")]
            if len(parts) >= 3 and parts[1].isdigit():
                print(f"    Content-matched: {parts[1]} refs, {parts[3]} funcs?")

print(f"\n{'=' * 70}")
print("KEY FINDINGS")
print(f"{'=' * 70}")
print("""
1. Name overlap is minimal (11/922 named GC funcs) despite 4,603 Mac
   function names — naming conventions differ radically between platforms.

2. String-content overlap also limited (~1.3% match rate). Mac strings are
   internal names/shader params; GC strings are error messages/UI text.

3. C++ demangled names add 20+ medium-confidence matches (AI, game systems).

4. Nu2 engine layer is well-preserved across both codebases.

5. 1,054 string-to-function mappings (mac_string_xrefs.txt) provide
   contextual annotation even without direct GC name matches.

6. 3,976 LSW1 + 9,791 LSW2 data/BSS variable names available for
   lbl_* cross-referencing.

MOST IMPACTFUL NEXT STEPS:
  A) Use string xrefs as contextual hints for unidentified GC functions
  B) Cross-reference data/BSS vars against GC lbl_* by size and section
  C) Propose Mac names as labels for all ~3,216 unnamed GC functions
  D) Build call-graph-based matching between Mac and GC binaries
  E) Deep analysis of the 833 LSW1 PIC string refs (currently only 17 matched)
""")

print("=" * 70)
