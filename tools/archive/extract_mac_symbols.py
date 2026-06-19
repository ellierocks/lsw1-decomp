#!/usr/bin/env python3
"""Extract and analyze Mac symbol tables from llvm-nm output, cross-reference with GC symbols."""

import csv
import os
import re
from collections import Counter, defaultdict

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.normpath(os.path.join(SCRIPT_DIR, ".."))
DOCS_DIR = os.path.join(PROJECT_ROOT, "docs", "symbol_donors")
GC_SYMBOLS_PATH = os.path.join(PROJECT_ROOT, "config", "GL5E4F", "symbols.txt")

NM_LSW1 = "/tmp/lsw1_demo_nm_all.txt"
NM_LSW2_PPC = "/tmp/lsw2_nm_ppc.txt"
NM_LSW2_I386 = "/tmp/lsw2_nm_i386.txt"

TSV_LSW1 = os.path.join(DOCS_DIR, "mac_lsw1_demo_symbols.tsv")
TSV_LSW2 = os.path.join(DOCS_DIR, "mac_lsw2_symbols.tsv")
SUMMARY_MD = os.path.join(DOCS_DIR, "mac_symbol_summary.md")
CANDIDATES_MD = os.path.join(DOCS_DIR, "mac_to_gc_candidates.md")

TSV_HEADER = [
    "source_binary", "architecture", "address", "symbol_type",
    "section", "type_abbrev", "raw_name", "clean_name", "demangled", "subsystem"
]


def infer_section(sym_type):
    upper = sym_type.upper()
    if upper == 'T':
        return "text"
    if upper == 'D':
        return "data"
    if upper == 'B':
        return "bss"
    if upper == 'S':
        return "sym"
    if upper == 'U':
        return "undef"
    if sym_type == '-':
        return "debug"
    if upper == 'A':
        return "absolute"
    return "other"


def infer_type_abbrev(sym_type):
    upper = sym_type.upper()
    if upper == 'T':
        return "func"
    if upper == 'D':
        return "data"
    if upper == 'B':
        return "bss"
    if sym_type == '-':
        return "debug"
    if upper == 'U':
        return "undef"
    return "other"


def infer_subsystem(clean_name):
    if not clean_name:
        return "Other"
    if clean_name.startswith("Nu"):
        return "Nu2"
    if clean_name.startswith("AI"):
        return "AI"
    if (clean_name.startswith("Menu_") or clean_name.startswith("PauseMenu_")
            or clean_name.startswith("DebugMenu_")):
        return "Menu"
    if clean_name.startswith("SceneSelect_"):
        return "Scene"
    if clean_name.startswith("Player_"):
        return "Player"
    if clean_name.startswith("GX"):
        return "GX"
    if clean_name.startswith("OS"):
        return "OS"
    if clean_name.startswith("PPC"):
        return "PPC"
    if clean_name.startswith("UI_"):
        return "UI"
    if clean_name.startswith("str_"):
        return "String"
    if re.match(r'^g[A-Z]', clean_name):
        return "Global"
    if clean_name.startswith("lbl_"):
        return "Label"
    if clean_name.startswith("fn_"):
        return "Function"
    if clean_name.startswith("jumptable_"):
        return "JumpTable"
    if clean_name.startswith("FifoObj"):
        return "FiFo"
    if (clean_name.startswith("AIScript") or clean_name.startswith("AISys")
            or clean_name.startswith("AIPath") or clean_name.startswith("AIEditor")
            or clean_name.startswith("AILocator")):
        return "AI"
    if clean_name.startswith("Area_"):
        return "Area"
    if re.match(r'^[A-Z].*Anim', clean_name):
        return "Animation"
    if clean_name.startswith("Lev"):
        return "Level"
    if clean_name.startswith("Pad"):
        return "Pad/Input"
    if clean_name.startswith("Camera") or clean_name.startswith("Cam"):
        return "Camera"
    if clean_name.startswith("Sound") or clean_name.startswith("Snd"):
        return "Sound"
    if clean_name.startswith("gcut") or clean_name.startswith("GCut"):
        return "Cutscene"
    if clean_name.startswith("instNuGCutScene") or clean_name.startswith("instCutScene"):
        return "Cutscene"
    return "Other"


def parse_nm_file(filepath, source_binary, architecture):
    rows = []
    with open(filepath, "r", encoding="latin-1") as f:
        for line in f:
            line = line.rstrip("\n").rstrip("\r")
            if not line:
                continue
            parts = line.split(None, 2)
            if len(parts) < 2:
                continue
            address_str = parts[0]
            sym_type = parts[1]
            raw_name = ""
            if len(parts) >= 3:
                raw_name = parts[2]
            clean_name = raw_name
            if clean_name.startswith("_"):
                clean_name = clean_name[1:]
            section = infer_section(sym_type)
            type_abbrev = infer_type_abbrev(sym_type)
            subsystem = infer_subsystem(clean_name)
            addr_clean = ""
            if sym_type.upper() != 'U' and sym_type != '-':
                try:
                    addr_val = int(address_str, 16)
                    if addr_val != 0:
                        addr_clean = hex(addr_val)
                except ValueError:
                    pass
            rows.append({
                "source_binary": source_binary,
                "architecture": architecture,
                "address": addr_clean,
                "symbol_type": sym_type,
                "section": section,
                "type_abbrev": type_abbrev,
                "raw_name": raw_name,
                "clean_name": clean_name,
                "demangled": "",
                "subsystem": subsystem,
            })
    return rows


def write_tsv(filepath, rows):
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    with open(filepath, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=TSV_HEADER, delimiter="\t", extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def parse_gc_symbols(filepath):
    gc_symbols = []
    gc_names = set()
    with open(filepath, "r") as f:
        for line in f:
            line = line.strip()
            if not line or '=' not in line:
                continue
            name = line.split('=')[0].strip()
            gc_symbols.append(name)
            if not name.startswith("lbl_") and not name.startswith("fn_") and not name.startswith("jumptable_"):
                gc_names.add(name)
    return gc_symbols, gc_names


def match_mac_to_gc(mac_rows, gc_names):
    candidates = []
    gc_lower = {n.lower(): n for n in gc_names}

    for row in mac_rows:
        cn = row["clean_name"]
        raw = row["raw_name"]
        if not cn:
            continue
        cn_lower = cn.lower()
        # Skip auto-generated
        if cn.startswith("lbl_") or cn.startswith("fn_") or cn.startswith("jumptable_"):
            continue

        best_match = None
        match_type = ""
        confidence = "LOW"

        # Exact match (case-insensitive)
        if cn_lower in gc_lower:
            best_match = gc_lower[cn_lower]
            match_type = "exact"
            confidence = "HIGH"
        else:
            # Try partial match: GC name in Mac name or vice versa
            for gc_n in gc_names:
                gc_l = gc_n.lower()
                if cn_lower == gc_l:
                    best_match = gc_n
                    match_type = "exact"
                    confidence = "HIGH"
                    break
                if gc_l in cn_lower or cn_lower in gc_l:
                    if len(gc_l) >= 4 and len(cn_lower) >= 4:
                        best_match = gc_n
                        match_type = "partial"
                        confidence = "MEDIUM"
                        break
                # Same prefix check
                prefix_len = min(3, len(cn_lower), len(gc_l))
                if prefix_len >= 3 and cn_lower[:prefix_len] == gc_l[:prefix_len]:
                    best_match = gc_n
                    match_type = "prefix"
                    confidence = "MEDIUM"
                    break

        if best_match:
            source_label = f"{row['source_binary']}_{row['architecture']}"
            candidates.append({
                "confidence": confidence,
                "gc_name": best_match,
                "mac_name": raw,
                "source_binary": source_label,
                "match_type": match_type,
                "notes": ""
            })
    return candidates


def generate_summary(all_rows_lsw1, all_rows_lsw2, gc_symbols, gc_names):
    out = []
    out.append("# Mac Symbol Summary\n")
    out.append(f"Generated from llvm-nm output for LSW1 Demo (PPC) and LSW2 (PPC + i386).\n")

    # Total symbol counts
    out.append("## Total Symbol Counts\n")
    out.append("| Source | Architecture | Count |")
    out.append("|--------|-------------|-------|")

    def count_source(rows, label):
        by_arch = Counter(r["architecture"] for r in rows)
        for arch, cnt in sorted(by_arch.items()):
            out.append(f"| {label} | {arch} | {cnt} |")
        if len(by_arch) > 1:
            out.append(f"| {label} | **total** | **{len(rows)}** |")
        elif len(by_arch) == 0:
            out.append(f"| {label} | - | {len(rows)} |")

    count_source(all_rows_lsw1, "LSW1 Demo")
    count_source(all_rows_lsw2, "LSW2")
    total = len(all_rows_lsw1) + len(all_rows_lsw2)
    out.append(f"| **Combined** | | **{total}** |")
    out.append("")

    # Section breakdown
    out.append("## Breakdown by Section\n")
    out.append("| Source | Text | Data | BSS | Absolute | Undef | Debug | Other |")
    out.append("|--------|------|------|-----|----------|-------|-------|-------|")

    def section_counts(rows, label):
        c = Counter(r["section"] for r in rows)
        out.append(f"| {label} | {c.get('text', 0)} | {c.get('data', 0)} | {c.get('bss', 0)} | "
                   f"{c.get('absolute', 0)} | {c.get('undef', 0)} | {c.get('debug', 0)} | "
                   f"{c.get('other', 0)} |")
    section_counts(all_rows_lsw1, "LSW1 Demo")
    section_counts(all_rows_lsw2, "LSW2")
    out.append("")

    # Type abbreviation breakdown
    out.append("## Breakdown by Type\n")
    out.append("| Source | func | data | bss | debug | undef | other |")
    out.append("|--------|------|------|-----|-------|-------|-------|")

    def type_counts(rows, label):
        c = Counter(r["type_abbrev"] for r in rows)
        out.append(f"| {label} | {c.get('func', 0)} | {c.get('data', 0)} | {c.get('bss', 0)} | "
                   f"{c.get('debug', 0)} | {c.get('undef', 0)} | {c.get('other', 0)} |")
    type_counts(all_rows_lsw1, "LSW1 Demo")
    type_counts(all_rows_lsw2, "LSW2")
    out.append("")

    # Subsystem breakdown
    out.append("## Breakdown by Subsystem\n")
    out.append("| Subsystem | LSW1 Demo | LSW2 | Total |")
    out.append("|-----------|-----------|------|-------|")

    subsys1 = Counter(r["subsystem"] for r in all_rows_lsw1)
    subsys2 = Counter(r["subsystem"] for r in all_rows_lsw2)
    all_subsystems = sorted(set(list(subsys1.keys()) + list(subsys2.keys())))
    for ss in all_subsystems:
        out.append(f"| {ss} | {subsys1.get(ss, 0)} | {subsys2.get(ss, 0)} | {subsys1.get(ss,0)+subsys2.get(ss,0)} |")
    out.append("")

    # Named vs auto-generated
    out.append("## Named vs Auto-generated Symbols\n")
    out.append("| Category | LSW1 Demo | LSW2 |")
    out.append("|----------|-----------|------|")

    def name_classify(rows):
        named = 0
        lbl = 0
        fn_ = 0
        jt = 0
        for r in rows:
            cn = r["clean_name"]
            if cn.startswith("lbl_"):
                lbl += 1
            elif cn.startswith("fn_"):
                fn_ += 1
            elif cn.startswith("jumptable_"):
                jt += 1
            elif cn:
                named += 1
        return named, lbl, fn_, jt

    n1, l1, f1, j1 = name_classify(all_rows_lsw1)
    n2, l2, f2, j2 = name_classify(all_rows_lsw2)
    out.append(f"| Named | {n1} | {n2} |")
    out.append(f"| lbl_ (auto) | {l1} | {l2} |")
    out.append(f"| fn_ (auto) | {f1} | {f2} |")
    out.append(f"| jumptable_ (auto) | {j1} | {j2} |")
    out.append("")

    # LSW1 vs LSW2 comparison
    out.append("## LSW1 vs LSW2 Comparison\n")
    out.append(f"- LSW1 Demo has **{len(all_rows_lsw1)}** symbols (PPC)")
    out.append(f"- LSW2 has **{len(all_rows_lsw2)}** symbols (PPC + i386)")
    lsw2_ppc = sum(1 for r in all_rows_lsw2 if r["architecture"] == "ppc")
    lsw2_i386 = sum(1 for r in all_rows_lsw2 if r["architecture"] == "i386")
    out.append(f"  - LSW2 PPC: **{lsw2_ppc}**")
    out.append(f"  - LSW2 i386: **{lsw2_i386}**")
    named_mac = [r for r in all_rows_lsw1 + all_rows_lsw2 if r["clean_name"] and not r["clean_name"].startswith("lbl_")]
    out.append(f"- Total named (non-auto) Mac symbols: **{len(named_mac)}**")
    out.append("")

    # Notable observations
    out.append("## Notable Observations\n")
    lsw1_nu = [r for r in all_rows_lsw1 if r["subsystem"] == "Nu2"]
    lsw2_nu = [r for r in all_rows_lsw2 if r["subsystem"] == "Nu2"]
    out.append(f"- Nu2 subsystem: {len(lsw1_nu)} symbols in LSW1 Demo, {len(lsw2_nu)} in LSW2")
    lsw2_cpp = [r for r in all_rows_lsw2 if r["raw_name"].startswith("__Z")]
    lsw1_cpp = [r for r in all_rows_lsw1 if r["raw_name"].startswith("__Z")]
    out.append(f"- C++ mangled symbols (__Z): {len(lsw1_cpp)} in LSW1, {len(lsw2_cpp)} in LSW2")
    lsw2_eh = [r for r in all_rows_lsw2 if r["raw_name"].endswith(".eh")]
    out.append(f"- LSW2 `.eh` (exception handling) entries: {len(lsw2_eh)}")
    out.append(f"- LSW2 debug stabs entries: {sum(1 for r in all_rows_lsw2 if r['symbol_type'] == '-')}")
    out.append("")

    # Categorized reports
    categories = {
        "Nu2": r"^Nu",
        "AI": r"^(AI|AIScript|AISys|AIPath|AIEditor|AILocator)",
        "Menu/Frontend": r"^(Menu_|PauseMenu_|DebugMenu_|Menu)",
        "Scene/Level": r"^(Scene|Lev)",
        "Character/Player": r"^(Player_|Character)",
        "Camera": r"^(Camera|Cam)",
        "Input": r"^(Pad|Input)",
        "Sound": r"^(Sound|Snd)",
        "Cutscene": r"^(instNuGCutScene|instCutScene|gcut|GCut)",
        "Global Data": r"^g[A-Z]",
    }
    out.append("## Categorized Reports\n")
    all_rows = all_rows_lsw1 + all_rows_lsw2
    for cat_name, cat_pattern in sorted(categories.items()):
        pat = re.compile(cat_pattern)
        matching = [r for r in all_rows if r["clean_name"] and pat.match(r["clean_name"])]
        out.append(f"### {cat_name}\n")
        out.append(f"- Count: **{len(matching)}**\n")
        if matching:
            out.append("| clean_name | address | source |")
            out.append("|------------|---------|--------|")
            for r in sorted(matching, key=lambda x: (x["source_binary"], x.get("address", ""))):
                src = f"{r['source_binary']}_{r['architecture']}"
                out.append(f"| {r['clean_name']} | {r['address'] or '-'} | {src} |")
        out.append("")
        # Notable patterns
        prefixes = Counter()
        for r in matching:
            m = re.match(r'^([A-Za-z]+)', r["clean_name"])
            if m:
                prefixes[m.group(1)] += 1
        if prefixes:
            common = prefixes.most_common(5)
            out.append(f"Common prefixes: {', '.join(f'{p[0]} ({p[1]})' for p in common)}\n")
        if len(matching) > 0:
            out.append("---\n")

    out.append("\n")
    return "\n".join(out)


def filter_candidates_best(candidates):
    seen = {}
    for c in candidates:
        key = (c["gc_name"], c["mac_name"])
        if key not in seen:
            seen[key] = c
        else:
            existing = seen[key]
            conf_rank = {"HIGH": 0, "MEDIUM": 1, "LOW": 2}
            if conf_rank.get(c["confidence"], 99) < conf_rank.get(existing["confidence"], 99):
                seen[key] = c
    return list(seen.values())


def generate_candidates_md(candidates, gc_symbols, all_rows_lsw1, all_rows_lsw2):
    out = []
    out.append("# Mac to GC Symbol Candidates\n")
    out.append("## Methodology\n")
    out.append("- For each named Mac symbol, attempt to match against GC symbols")
    out.append("- **HIGH**: exact name match (after stripping leading underscore)")
    out.append("- **MEDIUM**: same prefix + partial name overlap")
    out.append("- **LOW**: possible but uncertain\n")

    out.append("## Statistics\n")
    out.append(f"- Total GC named symbols: {len(gc_symbols)}")
    mac_named = [r for r in all_rows_lsw1 + all_rows_lsw2 if r["clean_name"]
                 and not r["clean_name"].startswith("lbl_")
                 and not r["clean_name"].startswith("fn_")
                 and not r["clean_name"].startswith("jumptable_")]
    out.append(f"- Total Mac named symbols: {len(mac_named)}")
    out.append(f"- Candidate matches found: {len(candidates)}\n")

    conf_counts = Counter(c["confidence"] for c in candidates)
    out.append(f"- HIGH confidence: {conf_counts.get('HIGH', 0)}")
    out.append(f"- MEDIUM confidence: {conf_counts.get('MEDIUM', 0)}")
    out.append(f"- LOW confidence: {conf_counts.get('LOW', 0)}\n")

    out.append("## Candidate Table\n")
    out.append("| Confidence | GC Name | Mac Name | Source Binary | Match Type | Notes |")
    out.append("|-----------|---------|----------|---------------|------------|-------|")

    best_candidates = filter_candidates_best(candidates)
    for c in sorted(best_candidates, key=lambda x: ({"HIGH": 0, "MEDIUM": 1, "LOW": 2}[x["confidence"]], x["gc_name"])):
        out.append(f"| {c['confidence']} | {c['gc_name']} | {c['mac_name']} | {c['source_binary']} | {c['match_type']} | {c['notes']} |")
    out.append("")
    return "\n".join(out)


def main():
    os.makedirs(DOCS_DIR, exist_ok=True)

    print("Parsing LSW1 Demo nm (PPC)...")
    rows_lsw1 = parse_nm_file(NM_LSW1, "lsw1_demo", "ppc")
    print(f"  -> {len(rows_lsw1)} symbols")

    print("Parsing LSW2 nm (PPC)...")
    rows_lsw2_ppc = parse_nm_file(NM_LSW2_PPC, "lsw2", "ppc")
    print(f"  -> {len(rows_lsw2_ppc)} symbols")

    print("Parsing LSW2 nm (i386)...")
    rows_lsw2_i386 = parse_nm_file(NM_LSW2_I386, "lsw2", "i386")
    print(f"  -> {len(rows_lsw2_i386)} symbols")

    rows_lsw2 = rows_lsw2_ppc + rows_lsw2_i386

    print(f"\nWriting TSV: {TSV_LSW1}...")
    write_tsv(TSV_LSW1, rows_lsw1)
    print(f"Writing TSV: {TSV_LSW2}...")
    write_tsv(TSV_LSW2, rows_lsw2)

    print(f"\nParsing GC symbols: {GC_SYMBOLS_PATH}...")
    gc_symbols, gc_names = parse_gc_symbols(GC_SYMBOLS_PATH)
    print(f"  -> {len(gc_symbols)} total, {len(gc_names)} named (non-auto)")

    all_mac = rows_lsw1 + rows_lsw2
    print(f"\nMatching Mac symbols against GC ({len(gc_names)} named)...")
    candidates = match_mac_to_gc(all_mac, gc_names)
    print(f"  -> {len(candidates)} candidate matches")

    print(f"\nWriting summary: {SUMMARY_MD}...")
    summary = generate_summary(rows_lsw1, rows_lsw2, gc_symbols, gc_names)
    with open(SUMMARY_MD, "w") as f:
        f.write(summary)

    print(f"Writing candidates: {CANDIDATES_MD}...")
    candidates_md = generate_candidates_md(candidates, gc_symbols, rows_lsw1, rows_lsw2)
    with open(CANDIDATES_MD, "w") as f:
        f.write(candidates_md)

    print("\n=== Summary ===")
    print(f"  LSW1 Demo symbols: {len(rows_lsw1)}")
    print(f"  LSW2 PPC symbols:  {len(rows_lsw2_ppc)}")
    print(f"  LSW2 i386 symbols: {len(rows_lsw2_i386)}")
    print(f"  GC named symbols:  {len(gc_names)}")
    print(f"  Match candidates:  {len(candidates)}")
    print(f"\nFiles written:")
    print(f"  {TSV_LSW1}")
    print(f"  {TSV_LSW2}")
    print(f"  {SUMMARY_MD}")
    print(f"  {CANDIDATES_MD}")


if __name__ == "__main__":
    main()
