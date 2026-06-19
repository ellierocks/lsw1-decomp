#!/usr/bin/env python3
"""
Second-pass Mac-to-GC symbol matcher - v2.

Strategies:
1. Exact name match (already verified - 11 HIGH)
2. C++ demangled name matching (LSW1 has _Z mangled names with signatures)
3. Subsystem prefix clustering with address ordering preserved
4. Cross-architecture confirmation (LSW2 PPC vs i386)

Outputs:
  docs/symbol_donors/mac_to_gc_review_top_200.md
  docs/symbol_donors/proposed_gc_renames.txt
"""

import csv
import re
import struct
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Optional

ROOT = Path(__file__).resolve().parents[1]
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
MAC_LSW1_TSV = ROOT / "docs/symbol_donors/mac_lsw1_demo_symbols.tsv"
MAC_LSW2_TSV = ROOT / "docs/symbol_donors/mac_lsw2_symbols.tsv"
OUT_DIR = ROOT / "docs/symbol_donors"
OUT_REVIEW = OUT_DIR / "mac_to_gc_review_top_200.md"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s+//\s+(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")

# Patterns for extracting base function name from C++ mangled names
# _Z<N><name>... or _Z<N><name>...
CPP_MANGLE_RE = re.compile(r'^_Z(\d+)([A-Za-z]\w*)')
# Also match prefix patterns like _Z10NuFileOpen... -> NuFileOpen
CPP_FUNC_RE = re.compile(r'^_Z\d+(Nu[A-Z][A-Za-z0-9]*|AI[A-Z][A-Za-z0-9]*|Menu[A-Z][A-Za-z0-9]*|Scene[A-Z][A-Za-z0-9]*|Player[A-Z][A-Za-z0-9]*|Pad[A-Z][A-Za-z0-9]*|Cam[A-Z][A-Za-z0-9]*|Credit[A-Z][A-Za-z0-9]*)')

KNOWN_SUBSYSTEMS = [
    "NuFile", "NuDat", "NuPP", "NuMem", "NuDebug", "NuSpecial",
    "NuAnim", "NuCamera", "NuSound", "NuRndr", "NuFade", "NuWind",
    "NuThread", "NuConsole", "NuCut", "NuMtl", "NuTex", "NuGScn",
    "NuGHG", "NuStringTable", "NuFPar", "NuVec", "NuGraph",
    "NuFnt", "NuTimeBar", "NuTrig", "NuTerr", "NuLights",
    "NuBloom", "NuBackbuffer", "NuDisplayList", "NuAPI",
    "Area_", "AIScript", "AISys", "AIPath", "AIEditor",
    "Menu", "SceneSelect", "PauseMenu", "DebugMenu",
    "Player_", "Pad", "Camera", "Sound", "gSound",
    "gInput", "gMenu", "gCamera", "gPlayer",
    "Credit", "Progress_", "gTotal", "gDifficulty",
    "gExtras", "gRegistered", "gSuperkit", "gMinikit",
    "Lev", "Lama", "Luminara", "Mace", "ObiWan", "Anakin",
    "Padme", "Amidala", "JarJar", "QuiGon", "DarthMaul",
    "R2D2", "C3PO", "BattleDroid", "DestroyerDroid",
    "Fighter", "Character", "Podrace",
    "NGCCARD", "NGCStreaming", "NGCSound",
    "GX", "OS", "PPC", "FifoObj",
]

# Known type prefixes for stabs debugging
_STABS_LINE_RE = re.compile(r'^\d+ \d+ \d+ (SLINE|SO|LBRAC|RBRAC|FUN|BNSYM|ENSYM|STSYM|ENTRY)')


@dataclass
class MacSymbol:
    source: str
    arch: str
    address: str
    sym_type: str
    section: str
    type_abbrev: str
    raw_name: str
    clean_name: str
    demangled: str
    subsystem: str

    @property
    def addr_int(self) -> Optional[int]:
        try:
            return int(self.address, 16) if self.address else None
        except ValueError:
            return None


@dataclass
class GCSymbol:
    name: str
    section: str
    address: int
    size: int
    meta: str


def parse_mac_tsv(path: Path) -> list[MacSymbol]:
    syms = []
    with open(path) as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            syms.append(MacSymbol(
                source=row["source_binary"],
                arch=row["architecture"],
                address=row["address"],
                sym_type=row["symbol_type"],
                section=row["section"],
                type_abbrev=row["type_abbrev"],
                raw_name=row["raw_name"],
                clean_name=row["clean_name"],
                demangled=row["demangled"],
                subsystem=row["subsystem"],
            ))
    return syms


def parse_gc_symbols(path: Path) -> tuple[list[GCSymbol], list[GCSymbol], dict[str, GCSymbol], set[str]]:
    symbols = []
    functions = []
    named_by_name = {}
    named_set = set()
    for line in path.read_text().splitlines():
        m = SYMBOL_RE.match(line)
        if not m:
            continue
        name = m.group("name")
        section = m.group("section")
        addr = int(m.group("addr"), 16)
        meta = m.group("meta")
        size_m = SIZE_RE.search(meta)
        size = int(size_m.group(1), 16) if size_m else 0
        sym = GCSymbol(name, section, addr, size, meta)
        symbols.append(sym)
        if section in (".init", ".text") and "type:function" in meta:
            functions.append(sym)
            if not name.startswith("fn_"):
                named_by_name[name] = sym
                named_set.add(name)
        elif not name.startswith(("lbl_", "fn_", "jumptable_")):
            named_by_name[name] = sym
            named_set.add(name)
    return symbols, functions, named_by_name, named_set


def extract_base_name(clean_name: str) -> str:
    """Extract the base C function name from a potentially C++-mangled name."""
    # Already clean (no leading _)
    name = clean_name
    
    # Handle _Z<digits><name>... pattern
    m = CPP_FUNC_RE.match(name)
    if m:
        return m.group(1)
    
    # Generic _Z extraction: find the first identifier after _Z<digits>
    if name.startswith("_Z"):
        rest = name[2:]
        dm = re.match(r'(\d+)([A-Za-z_]\w*)', rest)
        if dm:
            name_len = int(dm.group(1))
            base = dm.group(2)[:name_len]
            return base
    
    return name


def infer_subsystem(name: str) -> str:
    for prefix in KNOWN_SUBSYSTEMS:
        if name.startswith(prefix):
            return prefix.split('_')[0] if '_' in prefix else prefix
    return "Other"


def main():
    print("=" * 60)
    print("Second-Pass Mac-to-GC Symbol Matcher v2")
    print("=" * 60)
    
    # Load data
    print("\nLoading Mac symbols...")
    lsw1_syms = parse_mac_tsv(MAC_LSW1_TSV)
    lsw2_syms = parse_mac_tsv(MAC_LSW2_TSV)
    all_mac = lsw1_syms + lsw2_syms
    
    # Separate by architecture
    lsw1_ppc = [s for s in lsw1_syms]
    lsw2_ppc = [s for s in lsw2_syms if s.arch == "ppc"]
    lsw2_i386 = [s for s in lsw2_syms if s.arch == "i386"]
    
    # Filter to meaningful function symbols (with addresses, not .eh stubs, not debug)
    def is_meaningful_func(s: MacSymbol) -> bool:
        return (s.type_abbrev == "func" and s.addr_int is not None
                and not s.clean_name.endswith(".eh")
                and not _STABS_LINE_RE.match(s.raw_name)
                and s.clean_name not in ("", "start", "__start")
                and len(s.clean_name) > 1)
    
    lsw1_funcs = [s for s in lsw1_ppc if is_meaningful_func(s)]
    lsw2_ppc_funcs = [s for s in lsw2_ppc if is_meaningful_func(s)]
    lsw2_i386_funcs = [s for s in lsw2_i386 if is_meaningful_func(s)]
    
    print(f"  LSW1 Demo functions: {len(lsw1_funcs)}")
    print(f"  LSW2 PPC functions: {len(lsw2_ppc_funcs)}")
    print(f"  LSW2 i386 functions: {len(lsw2_i386_funcs)}")
    
    print("\nLoading GC symbols...")
    gc_all, gc_funcs, gc_named_by_name, gc_named_set = parse_gc_symbols(GC_SYMBOLS)
    # Also build address-to-function map for unnamed GC functions
    gc_func_by_addr = {}
    for f in gc_funcs:
        gc_func_by_addr[f.address] = f
    gc_funcs_sorted = sorted(gc_funcs, key=lambda f: f.address)
    
    print(f"  GC functions: {len(gc_funcs)}, Named: {len(gc_named_set)}")
    
    # Build cross-architecture map for LSW2
    print("\n[Cross-Arch] Building LSW2 PPC<->i386 mapping...")
    ppc_by_addr = {s.addr_int: s for s in lsw2_ppc_funcs}
    i386_by_addr = {s.addr_int: s for s in lsw2_i386_funcs}
    ppc_by_name = {}
    for s in lsw2_ppc_funcs:
        base = extract_base_name(s.clean_name)
        if base:
            ppc_by_name[base] = s
    i386_by_name = {}
    for s in lsw2_i386_funcs:
        base = extract_base_name(s.clean_name)
        if base:
            i386_by_name[base] = s
    
    # Find names that exist in both PPC and i386
    cross_confirmed = set(ppc_by_name.keys()) & set(i386_by_name.keys())
    print(f"  {len(cross_confirmed)} names confirmed across PPC+i386")
    
    candidates = []
    seen_pairs = set()
    
    def add_candidate(mac_name: str, mac_addr: str, source: str, subsystem: str,
                      gc_name: str, gc_addr: str, confidence: str, match_type: str, notes: str,
                      priority: int):
        key = (mac_name, gc_name)
        if key in seen_pairs:
            return
        seen_pairs.add(key)
        candidates.append({
            "mac_name": mac_name,
            "mac_addr": mac_addr,
            "source": source,
            "gc_name": gc_name,
            "gc_addr": gc_addr,
            "subsystem": subsystem,
            "confidence": confidence,
            "match_type": match_type,
            "notes": notes,
            "priority": priority,
        })
    
    # =========================================================
    # Strategy 1: Exact name matches (already confirmed)
    # =========================================================
    print("\n[1/5] Exact name matches...")
    for sym in all_mac:
        clean = sym.clean_name
        if clean in gc_named_set:
            gc_sym = gc_named_by_name[clean]
            section = gc_sym.section if gc_sym.section in (".init", ".text") else "?"
            add_candidate(
                clean, sym.address, f"{sym.source}_{sym.arch}",
                sym.subsystem,
                clean, f"0x{gc_sym.address:08X}",
                "HIGH", "exact_name", "Exact name match",
                priority=1
            )
    print(f"  Found {sum(1 for c in candidates if c['confidence']=='HIGH')} HIGH matches")
    
    # =========================================================
    # Strategy 2: C++ demangled name matching (LSW1)
    # =========================================================
    print("\n[2/5] C++ demangled name matching (LSW1)...")
    lsw1_demangled = 0
    for sym in lsw1_funcs:
        base = extract_base_name(sym.clean_name)
        if base and base != sym.clean_name and not base.startswith("_"):
            # Check if base name exists in GC
            if base in gc_named_set:
                gc_sym = gc_named_by_name[base]
                add_candidate(
                    sym.clean_name, sym.address, "lsw1_demo_ppc",
                    infer_subsystem(base),
                    base, f"0x{gc_sym.address:08X}",
                    "HIGH", "demangled_cpp",
                    f"Mangled C++ -> dematches {base}",
                    priority=1
                )
                lsw1_demangled += 1
                continue
            
            # Try partial match: base contains or is contained in GC name
            for gc_name, gc_sym in gc_named_by_name.items():
                if gc_sym.section not in (".init", ".text"):
                    continue
                if "type:function" not in gc_sym.meta:
                    continue
                if base in gc_name or gc_name in base:
                    if gc_name.startswith("fn_"):
                        continue
                    confidence = "HIGH" if base == gc_name else "MEDIUM"
                    add_candidate(
                        sym.clean_name, sym.address, "lsw1_demo_ppc",
                        infer_subsystem(base),
                        gc_name, f"0x{gc_sym.address:08X}",
                        confidence, "demangled_partial",
                        f"Demangled base '{base}' overlaps with {gc_name}",
                        priority=2
                    )
                    lsw1_demangled += 1
                    break
    
    print(f"  Found {lsw1_demangled} C++ demangled matches")
    
    # =========================================================
    # Strategy 3: LSW2 Nu2 names -> GC via name similarity
    # =========================================================
    print("\n[3/5] LSW2 Nu2/engine name matching...")
    
    # Build a set of GC function names (without fn_ prefix)
    gc_func_names = {}
    for f in gc_funcs:
        if not f.name.startswith(("fn_", "lbl_", "jumptable_")):
            gc_func_names[f.name] = f
    
    lsw2_matched = 0
    # For LSW2, extract base names and match against GC
    for sym in lsw2_ppc_funcs:
        base = extract_base_name(sym.clean_name)
        if not base or base == sym.clean_name:
            base = sym.clean_name
        if base.startswith("_") or len(base) < 3:
            continue
        
        # Skip .eh entries and auto-generated labels
        if base.endswith(".eh") or base.startswith(("lbl_", "fn_", "jumptable_")):
            continue
        
        # Skip exact matches already found
        if base in gc_named_set:
            continue
        
        # Try direct name match
        if base in gc_func_names:
            gc_sym = gc_func_names[base]
            add_candidate(
                sym.clean_name, sym.address, "lsw2_ppc",
                sym.subsystem,
                base, f"0x{gc_sym.address:08X}",
                "HIGH", "lsw2_direct",
                "Direct name match in GC (not in named_set, check if fn_*)",
                priority=2
            )
            lsw2_matched += 1
            continue
        
        # Try substring matching: find GC functions whose name contains base or vice versa
        for gc_name, gc_sym in gc_func_names.items():
            if len(gc_name) < 4 or gc_name.startswith("str_"):
                continue
            # Check if base is a substring of GC name or GC name is a substring of base
            # Use word boundaries for better matching
            if base in gc_name and len(base) >= 4:
                add_candidate(
                    sym.clean_name, sym.address, "lsw2_ppc",
                    sym.subsystem,
                    gc_name, f"0x{gc_sym.address:08X}",
                    "MEDIUM", "lsw2_substr",
                    f"Base '{base}' is substring of GC '{gc_name}'",
                    priority=3
                )
                lsw2_matched += 1
                break
            elif gc_name in base and len(gc_name) >= 4:
                # Avoid matching common short names
                common_short = {"Pad", "Menu", "Area", "Camera", "Sound", "Level", "Player", "Scene", "Input"}
                if gc_name not in common_short:
                    add_candidate(
                        sym.clean_name, sym.address, "lsw2_ppc",
                        sym.subsystem,
                        gc_name, f"0x{gc_sym.address:08X}",
                        "MEDIUM" if len(gc_name) >= 6 else "LOW",
                        "lsw2_superstr",
                        f"GC '{gc_name}' is substring of Mac '{base}'",
                        priority=3
                    )
                    lsw2_matched += 1
                    break
    
    print(f"  Found {lsw2_matched} LSW2 engine name matches")
    
    # =========================================================
    # Strategy 4: Address ordering inference
    # =========================================================
    print("\n[4/5] Address ordering and neighborhood inference...")
    
    # Group GC functions by contiguous named blocks
    gc_named_with_addr = [(f.address, f.name) for f in gc_funcs 
                          if not f.name.startswith(("fn_", "lbl_", "jumptable_"))]
    gc_named_with_addr.sort()
    
    # For each group of consecutive named functions, create a "block"
    # Then look for Mac functions with matching names in similar order
    gc_blocks = []
    current_block = []
    for addr, name in gc_named_with_addr:
        if current_block and addr - current_block[-1][0] > 0x2000:
            if len(current_block) >= 2:
                gc_blocks.append(current_block)
            current_block = [(addr, name)]
        else:
            current_block.append((addr, name))
    if len(current_block) >= 2:
        gc_blocks.append(current_block)
    
    # For each block, extract the prefix pattern
    block_prefixes = set()
    for block in gc_blocks:
        prefix_count = defaultdict(int)
        for _, name in block:
            base = name.split('_')[0] if '_' in name else re.sub(r'\d+$', '', name)
            for known in sorted(KNOWN_SUBSYSTEMS, key=len, reverse=True):
                if name.startswith(known):
                    base = known
                    break
            prefix_count[base] += 1
        if prefix_count:
            dominant = max(prefix_count, key=prefix_count.get)
            if prefix_count[dominant] >= 2:
                block_prefixes.add(dominant)
    
    print(f"  Identified {len(gc_blocks)} GC named blocks across {len(block_prefixes)} prefixes")
    
    # =========================================================
    # Strategy 5: LSW1 game-specific symbols (prioritized)
    # =========================================================
    print("\n[5/5] LSW1 game-specific symbol extraction...")
    
    # Get all LSW1 function names (non-empty, non-trivial)
    lsw1_game_names = []
    for sym in lsw1_funcs:
        base = extract_base_name(sym.clean_name)
        name_to_check = base if base and len(base) > 2 else sym.clean_name
        if name_to_check.startswith("_") and not name_to_check.startswith("_Z"):
            name_to_check = name_to_check[1:]
        if len(name_to_check) < 3:
            continue
        if name_to_check.startswith(("Nu", "GX", "OS", "PPC", "str_")):
            continue
        if name_to_check.endswith(".eh"):
            continue
        # Skip known uninteresting names
        if any(c in name_to_check for c in ["@", "?", "$"]):
            continue
        if re.match(r'^[0-9\s]+$', name_to_check):
            continue
        lsw1_game_names.append((name_to_check, sym.address, sym.clean_name))
    
    # Deduplicate
    seen_names = set()
    unique_lsw1 = []
    for name, addr, raw in lsw1_game_names:
        if name not in seen_names:
            seen_names.add(name)
            unique_lsw1.append((name, addr, raw))
    
    print(f"  {len(unique_lsw1)} unique LSW1 game-specific names")
    
    # Try to match these against GC unnamed functions
    gc_unnamed = [f for f in gc_funcs if f.name.startswith("fn_")]
    
    for name, addr, raw in sorted(unique_lsw1)[:80]:
        if any(c["mac_name"] == raw for c in candidates):
            continue
        
        # Check if base name or clean name matches any GC name partially
        for gc_f in gc_funcs_sorted:
            if gc_f.name.startswith(("str_", "lbl_", "jumptable_")):
                continue
            # Skip already matched
            if gc_f.name in gc_named_set:
                continue
            
            gc_base = gc_f.name
            if gc_base.startswith("fn_"):
                continue
            
            # Check for stem match: longest common prefix
            if name in gc_base or gc_base in name:
                if len(name) >= 4 and len(gc_base) >= 4:
                    add_candidate(
                        raw, addr, "lsw1_demo_ppc",
                        infer_subsystem(name),
                        gc_base, f"0x{gc_f.address:08X}",
                        "LOW", "lsw1_vs_unnamed",
                        f"Name overlap with GC unnamed func, target addr {gc_f.name}",
                        priority=4
                    )
                    break
    
    # =========================================================
    # Strategy 6: Add LSW1 unique game-symbol names with no GC match
    #              (as proposals for new GC renames)
    # =========================================================
    print("\n[6/5] LSW1 game symbols as new GC proposals...")
    
    # Get all LSW1 function names that are C++ mangled but don't match
    lsw1_all_names = set()
    lsw1_added = 0
    for sym in lsw1_funcs:
        base = extract_base_name(sym.clean_name)
        name = base if base else sym.clean_name
        if len(name) < 3 or name.startswith(("Nu", "GX", "OS", "PPC")):
            continue
        if name in lsw1_all_names:
            continue
        lsw1_all_names.add(name)
        
        # Check if already matched
        if any(c["mac_name"] == sym.clean_name for c in candidates):
            continue
        if sym.clean_name.endswith(".eh"):
            continue
        
        # Add as a proposal (no GC match)
        if lsw1_added < 60:
            sub = infer_subsystem(name)
            add_candidate(
                sym.clean_name, sym.address, "lsw1_demo_ppc",
                sub, "—NO GC MATCH—", "—",
                "LOW", "lsw1_proposal",
                f"LSW1-only name, no corresponding GC named function",
                priority=5
            )
            lsw1_added += 1
    
    # Add LSW2 non-Nu2 unique names
    print("\n[7/5] LSW2 non-Nu2 proposals...")
    lsw2_non_nu2 = set()
    lsw2_added = 0
    for sym in lsw2_ppc_funcs:
        base = extract_base_name(sym.clean_name)
        name = base if base else sym.clean_name
        if len(name) < 4 or name.startswith(("Nu", "GX", "OS", "PPC")):
            continue
        if name.endswith(".eh"):
            continue
        if name in lsw2_non_nu2:
            continue
        lsw2_non_nu2.add(name)
        
        if any(c["mac_name"] == sym.clean_name for c in candidates):
            continue
        
        if lsw2_added < 40:
            sub = infer_subsystem(name)
            add_candidate(
                sym.clean_name, sym.address, "lsw2_ppc",
                sub, "—NO GC MATCH—", "—",
                "LOW", "lsw2_proposal",
                f"LSW2-only name (non-Nu2), potential new GC symbol",
                priority=5
            )
            lsw2_added += 1
    
    # =========================================================
    # Sort and trim to top 200
    # =========================================================
    conf_rank = {"HIGH": 0, "MEDIUM": 1, "LOW": 2}
    candidates.sort(key=lambda c: (c["priority"], conf_rank[c["confidence"]], c["mac_name"]))
    
    top200 = candidates[:200]
    
    # =========================================================
    # Generate review file
    # =========================================================
    lines = [
        "# Mac-to-GC Symbol Review: Top 200 Candidates",
        "",
        "**Do not auto-apply.** Each entry requires manual verification.",
        "",
        "## Candidate Table",
        "",
        "| # | Conf. | Pri | Mac Name | Mac Addr | Source | GC Name | GC Addr | Match Type | Notes |",
        "|---|--------|-----|----------|----------|--------|---------|---------|------------|-------|",
    ]
    
    for i, c in enumerate(top200, 1):
        lines.append(
            f"| {i} | {c['confidence']} | P{c['priority']} "
            f"| `{c['mac_name']}` | {c['mac_addr']} "
            f"| {c['source']} "
            f"| `{c.get('gc_name', '?')}` | {c.get('gc_addr', '?')} "
            f"| {c.get('match_type', '?')} "
            f"| {c.get('notes', '')} |"
        )
    
    # Summary
    high_count = sum(1 for c in top200 if c['confidence'] == 'HIGH')
    med_count = sum(1 for c in top200 if c['confidence'] == 'MEDIUM')
    low_count = sum(1 for c in top200 if c['confidence'] == 'LOW')
    
    lines.extend([
        "",
        "---",
        "",
        "## Summary",
        "",
        f"- Total candidates: {len(candidates)}, Top 200 shown",
        f"- HIGH: {high_count}, MEDIUM: {med_count}, LOW: {low_count}",
        "",
        "### By Source",
    ])
    
    src_counts = defaultdict(int)
    for c in top200:
        src_counts[c["source"]] += 1
    for src, count in sorted(src_counts.items()):
        lines.append(f"- {src}: {count}")
    
    lines.extend(["", "### By Subsystem"])
    sub_counts = defaultdict(int)
    for c in top200:
        sub_counts[c.get("subsystem", "Other")] += 1
    for sub, count in sorted(sub_counts.items()):
        lines.append(f"- {sub}: {count}")
    
    lines.extend(["", "### By Match Type"])
    mt_counts = defaultdict(int)
    for c in top200:
        mt_counts[c["match_type"]] += 1
    for mt, count in sorted(mt_counts.items()):
        lines.append(f"- {mt}: {count}")
    
    lines.append("")
    lines.append("### Note: All 11 HIGH exact-name matches already present in GC symbols.txt")
    lines.append("No changes needed for those.")
    
    OUT_REVIEW.write_text("\n".join(lines) + "\n")
    print(f"\nWrote {len(top200)} candidates to {OUT_REVIEW}")
    
    # Propose renames for GC unnamed targets
    proposals = []
    for c in top200:
        gc_name = c.get('gc_name', '?')
        if gc_name.startswith("fn_"):
            proposals.append(f"# {c['confidence']}: {c['mac_name']} -> {gc_name} ({c['gc_addr']})")
    
    if proposals:
        (OUT_DIR / "proposed_gc_renames.txt").write_text("\n".join(proposals) + "\n")
        print(f"Wrote {len(proposals)} proposed renames")
    
    # Summary statistics
    total_high = sum(1 for c in candidates if c['confidence'] == 'HIGH')
    total_med = sum(1 for c in candidates if c['confidence'] == 'MEDIUM')
    total_low = sum(1 for c in candidates if c['confidence'] == 'LOW')
    print(f"\nTotal candidates: {len(candidates)} (HIGH={total_high}, MED={total_med}, LOW={total_low})")
    print("Done!")


if __name__ == "__main__":
    main()
