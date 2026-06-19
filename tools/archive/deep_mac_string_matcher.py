#!/usr/bin/env python3
"""
Deep Mac-to-GC string-xref matcher.

Extracts strings from Mac Mach-O __cstring sections, scans the Mac PPC
code sections for lis+addi / lis+load patterns that reference those strings,
and cross-references which GC function references the same string content.

Also extracts STABS SO (source file) entries and data/BSS variable names.
"""

import csv
import re
import struct
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Optional

ROOT = Path(__file__).resolve().parents[1]
GC_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
MAC_LSW1 = ROOT / "orig/mac/mac-debug-symbols/LEGO Star Wars Demo"
MAC_LSW2 = ROOT / "orig/mac/mac-debug-symbols/LEGO Star Wars II"
OUT_DIR = ROOT / "docs/symbol_donors"
OUT_DEEP_TSV = OUT_DIR / "mac_gc_string_xref_match.tsv"
OUT_DEEP_MD = OUT_DIR / "mac_deep_match_report.md"
OUT_MAC_XREFS = OUT_DIR / "mac_string_xrefs.txt"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s+//\s+(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")

DOL_SECTION_NAMES = {
    0: ".init", 1: ".text", 2: ".rodata", 3: ".data",
    7: ".data", 8: ".sdata", 9: ".sdata2",
}

# Metrowerks PPC string reference: lis + addi/ori/load
SCAN_PATTERNS = [
    (15, 14),   # lis + addi
    (15, 24),   # lis + ori
    (15, 32),   # lis + lwz
    (15, 34),   # lis + lbz
    (15, 35),   # lis + lhz
    (15, 40),   # lis + lfs
    (15, 36),   # lis + stw
    (15, 44),   # lis + sth
    (15, 37),   # lis + stfs
]


@dataclass
class DolSection:
    index: int
    name: str
    offset: int
    address: int
    size: int


@dataclass
class GCSymbol:
    name: str
    section: str
    address: int
    size: int
    meta: str


@dataclass
class StringRef:
    """A string reference in code."""
    content: str
    func_name: str
    func_addr: int
    ref_addr: int
    binary: str
    arch: str = ""


def parse_dol_sections(path: Path) -> list[DolSection]:
    header = path.read_bytes()[:0x100]
    sections = []
    for i in range(18):
        offset = struct.unpack(">I", header[i*4:i*4+4])[0]
        address = struct.unpack(">I", header[0x48+i*4:0x4C+i*4])[0]
        size = struct.unpack(">I", header[0x90+i*4:0x94+i*4])[0]
        if size:
            name = DOL_SECTION_NAMES.get(i, f".dol{i}")
            sections.append(DolSection(i, name, offset, address, size))
    return sections


def parse_gc_symbols(path: Path) -> dict[str, GCSymbol]:
    funcs = {}
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
        if section in (".init", ".text") and "type:function" in meta:
            funcs[name] = GCSymbol(name, section, addr, size, meta)
    return funcs


def extract_macho_strings(path: Path, arch: str = "") -> dict[int, bytes]:
    """Extract __cstring section from Mach-O via llvm-objdump."""
    # LLVM 22+ uses --section (double dash) and outputs text format: "addr  string_content"
    cmd = ["llvm-objdump", "--full-contents", "--section", "__TEXT,__cstring", "--macho"]
    if arch:
        cmd.extend(["--arch", arch])
    cmd.append(str(path))
    result = subprocess.run(cmd, capture_output=True, text=True)

    strings = {}
    for line in result.stdout.splitlines():
        # Format: "0019d1f4  string content here" or "0019d1f4  " (empty for null bytes)
        if len(line) < 10:
            continue
        # Address is a bare hex word, not followed by ":"
        try:
            addr_str = line[:8].strip()
            if not addr_str:
                continue
            addr = int(addr_str, 16)
        except ValueError:
            continue
        content = line[10:]  # after "addr  "
        if content:
            strings[addr] = content.encode("latin-1", errors="replace") + b"\x00"
    return strings


def extract_macho_code_section(path: Path, arch: str = "") -> tuple[bytes, int, str]:
    """Extract __text section bytes and base address from Mach-O."""
    # LLVM 22+ outputs disassembly: "    addr:	HH HH HH HH	mnemonic ..."
    cmd = ["llvm-objdump", "--full-contents", "--section", "__TEXT,__text", "--macho"]
    if arch:
        cmd.extend(["--arch", arch])
    cmd.append(str(path))
    result = subprocess.run(cmd, capture_output=True, text=True)

    data = bytearray()
    base_addr = None
    prev_end = None
    hex_re = re.compile(r"^\s+([0-9a-f]+):\s+((?:[0-9a-f]{2}\s+)+)", re.IGNORECASE)
    for line in result.stdout.splitlines():
        m = hex_re.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        hex_bytes = bytes(int(h, 16) for h in m.group(2).split())
        if base_addr is None:
            base_addr = addr
        if prev_end is not None and addr > prev_end:
            data.extend(b"\x00" * (addr - prev_end))
        data.extend(hex_bytes)
        prev_end = addr + len(hex_bytes)
    return bytes(data), base_addr if base_addr else 0, f"macho_{arch}" if arch else "macho"


def extract_gc_strings(path: Path) -> dict[int, bytes]:
    """Extract rodata/data string sections from DOL."""
    sections = parse_dol_sections(path)
    data = path.read_bytes()
    strings = {}
    for sec in sections:
        # GC DOL has no standalone .rodata — strings live in .data sections (indices 7+)
        if sec.index < 7:
            continue
        raw = data[sec.offset:sec.offset + sec.size]
        i = 0
        while i < len(raw):
            end = raw.index(b'\x00', i) if b'\x00' in raw[i:] else len(raw)
            if end - i >= 3:
                strings[sec.address + i] = raw[i:end]
            i = end + 1
    return strings


def scan_for_string_refs(code: bytes, base_addr: int, strings: dict[int, bytes],
                         func_map: dict[int, str], binary: str, arch: str = "") -> list[StringRef]:
    """Scan PPC code for lis+addi/load patterns that reference strings."""
    # Build content -> (addr, text) map with all addrs
    addr_to_text = {addr: text.decode('latin-1', errors='replace') for addr, text in strings.items()}
    
    refs = []
    
    for offset in range(0, len(code) - 8, 4):
        inst1 = struct.unpack(">I", code[offset:offset+4])[0]
        op1 = inst1 >> 26
        if op1 != 15:  # lis / addis
            continue
        rd = (inst1 >> 21) & 0x1F
        ra = (inst1 >> 16) & 0x1F
        if ra != 0:
            continue
        imm = inst1 & 0xFFFF
        high = imm << 16 if imm < 0x8000 else (imm - 0x10000) << 16
        addr = base_addr + offset
        
        for lookahead in range(4, min(32*4, len(code) - offset), 4):
            inst2 = struct.unpack(">I", code[offset+lookahead:offset+lookahead+4])[0]
            op2 = inst2 >> 26
            use_rd = (inst2 >> 21) & 0x1F
            use_ra = (inst2 >> 16) & 0x1F
            use_imm = inst2 & 0xFFFF
            if use_ra != rd:
                continue
            
            target = None
            if op2 == 14:   # addi
                target = (high + use_imm) & 0xFFFFFFFF
            elif op2 == 24:  # ori
                target = (high | use_imm) & 0xFFFFFFFF
            elif op2 in {32, 34, 35, 40}:  # lwz, lbz, lhz, lfs
                target = (high + use_imm) & 0xFFFFFFFF
            
            if target and target in addr_to_text:
                # Find containing function
                func_name = "<unknown>"
                func_start = 0
                for f_addr in sorted(func_map.keys(), reverse=True):
                    if addr >= f_addr:
                        func_name = func_map[f_addr]
                        func_start = f_addr
                        break
                
                refs.append(StringRef(
                    content=addr_to_text[target],
                    func_name=func_name,
                    func_addr=func_start,
                    ref_addr=addr,
                    binary=binary,
                    arch=arch,
                ))
                break  # Only first reference per lis
    
    return refs


def build_gc_func_map(gc_funcs: dict[str, GCSymbol]) -> dict[int, str]:
    """Build address->name map for GC functions."""
    addr_map = {}
    for name, f in gc_funcs.items():
        addr_map[f.address] = name
    return addr_map


def scan_gc_for_string_refs(gc_funcs: dict[str, GCSymbol], gc_strings: dict[int, bytes]) -> dict[str, set[str]]:
    """Reuse existing string xref output from build/xrefs/string_xrefs.txt."""
    xref_path = ROOT / "build/xrefs/string_xrefs.txt"
    if not xref_path.exists():
        print("  WARNING: build/xrefs/string_xrefs.txt not found. Run tools/string_xrefs.py first.")
        return {}
    
    func_strings = defaultdict(set)
    for line in xref_path.read_text().splitlines():
        parts = line.split()
        if len(parts) >= 4 and parts[0].startswith("str_"):
            func_name = parts[2]
            str_name = parts[0]
            if not func_name.startswith("<"):
                func_strings[func_name].add(str_name)
    return dict(func_strings)


def extract_stabs_info(path: Path, arch: str = "") -> list[str]:
    """Extract STABS SO (source file) entries from Mach-O."""
    cmd = ["llvm-nm", "-a"]
    if arch:
        cmd.extend(["-arch", arch])
    cmd.append(str(path))
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    sources = set()
    for line in result.stdout.splitlines():
        if " SO " in line and "/" in line:
            parts = line.split(" SO ", 1)
            if len(parts) == 2:
                path_part = parts[1].strip()
                if path_part and path_part != "start.s" and not path_part.startswith("00"):
                    sources.add(path_part)
    return sorted(sources)


def extract_macho_data_syms(path: Path, arch: str = "") -> list[tuple[str, str, str]]:
    """Extract data/BSS symbol names with addresses from Mach-O."""
    cmd = ["llvm-nm", "-a"]
    if arch:
        cmd.extend(["-arch", arch])
    cmd.append(str(path))
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    syms = []
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) < 2:
            continue
        sym_type = parts[1] if len(parts) >= 2 else ""
        if sym_type in ('D', 'd', 'B', 'b', 'S', 's', 'C'):
            addr = parts[0]
            name = parts[2] if len(parts) >= 3 else ""
            if name and not name.startswith("_"):
                name = "_" + name
            if name and not any(c in name for c in ['@', ' ', '\t']) and len(name) > 1:
                sec = "data" if sym_type in ('D', 'd') else "bss" if sym_type in ('B', 'b') else "sdata/sbss"
                syms.append((name, addr, sec))
    return syms


def main():
    print("=" * 70)
    print("DEEP Mac-to-GC String-Xref Matcher")
    print("=" * 70)
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    
    # Step 1: Load GC data
    print("\n[1] Loading GC symbols and strings...")
    gc_funcs = parse_gc_symbols(GC_SYMBOLS)
    gc_func_map = build_gc_func_map(gc_funcs)
    gc_strings = extract_gc_strings(GC_DOL)
    print(f"  {len(gc_funcs)} GC functions, {len(gc_strings)} GC string regions")
    
    # Load GC string xrefs
    gc_func_strings = scan_gc_for_string_refs(gc_funcs, gc_strings)
    print(f"  {len(gc_func_strings)} GC functions with string xrefs")
    
    # Step 2: Extract Mac strings and scan for refs
    print("\n[2] Extracting Mac strings and scanning code...")
    
    all_mac_refs = []
    
    for label, exe, arch in [
        ("LSW1 Demo PPC", MAC_LSW1, ""),
        ("LSW2 PPC", MAC_LSW2, "ppc"),
    ]:
        print(f"  Processing {label}...")
        mac_strings = extract_macho_strings(exe, arch)
        print(f"    {len(mac_strings)} string regions")
        
        # Scan code
        code, base, _ = extract_macho_code_section(exe, arch)
        print(f"    Code: {len(code)} bytes at 0x{base:x}")
        
        # Build func map from nm output
        cmd = ["llvm-nm", "-a"]
        if arch:
            cmd.extend(["-arch", arch])
        cmd.append(str(exe))
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        nm_func_map = {}
        for line in result.stdout.splitlines():
            parts = line.split()
            if len(parts) >= 3:
                t = parts[1]
                if t in ('T', 't'):
                    try:
                        addr = int(parts[0], 16)
                        name = parts[2]
                        nm_func_map[addr] = name
                    except ValueError:
                        pass
        
        print(f"    {len(nm_func_map)} text symbols")
        
        refs = scan_for_string_refs(code, base, mac_strings, nm_func_map, label, arch)
        print(f"    {len(refs)} string references found")
        all_mac_refs.extend(refs)
    
    # Step 3: Match Mac refs to GC refs by string content
    print("\n[3] Cross-referencing string content...")
    
    # Build content -> GC function mapping
    # We need to read GC string xref output to get str_name -> function mapping
    # Then look up the actual string content
    gc_str_to_func = defaultdict(list)
    gc_xref_path = ROOT / "build/xrefs/string_xrefs.txt"
    if gc_xref_path.exists():
        for line in gc_xref_path.read_text().splitlines():
            parts = line.split()
            if len(parts) >= 4 and parts[0].startswith("str_"):
                str_name = parts[0]
                func_name = parts[2]
                str_addr = parts[1]
                if not func_name.startswith("<"):
                    gc_str_to_func[str_name].append(func_name)
    
    # Build content -> str_name map for GC strings
    # We need the str_* name by content. Use symbols.txt to find str_* entries
    gc_str_content = {}
    for line in GC_SYMBOLS.read_text().splitlines():
        m = SYMBOL_RE.match(line)
        if not m:
            continue
        name = m.group("name")
        if name.startswith("str_"):
            addr = int(m.group("addr"), 16)
            section = m.group("section")
            size_m = SIZE_RE.search(m.group("meta"))
            size = int(size_m.group(1), 16) if size_m else 0
            # Read actual content from DOL (section name in symbols.txt is .rodata
            # but the DOL has no standalone .rodata — strings live in data sections)
            if section in (".rodata", ".data", ".sdata", ".sdata2"):
                sections = parse_dol_sections(GC_DOL)
                for sec in sections:
                    if sec.address <= addr < sec.address + sec.size:
                        offset = sec.offset + (addr - sec.address)
                        dol_data = GC_DOL.read_bytes()
                        content = dol_data[offset:offset + size]
                        if content:
                            # Remove trailing nulls for matching
                            text = content.split(b'\x00')[0].decode('latin-1', errors='replace')
                            gc_str_content[text] = name
                        break
    
    print(f"  {len(gc_str_content)} GC strings with known content")
    
    # Now match Mac refs to GC strings
    content_matches = []  # (mac_func, mac_ref_content, gc_str_name, gc_funcs)
    for ref in all_mac_refs:
        content = ref.content.strip()
        if not content:
            continue
        # Truncate to match GC strings which might be shorter
        for gc_text, gc_name in gc_str_content.items():
            gc_clean = gc_text.strip().rstrip('\x00').strip()
            if not gc_clean:
                continue
            # Match if one is prefix of the other (accounting for truncation)
            if content == gc_clean or content.startswith(gc_clean) or gc_clean.startswith(content):
                gc_ref_funcs = gc_str_to_func.get(gc_name, [])
                content_matches.append((ref, gc_name, gc_ref_funcs))
                break
    
    print(f"  {len(content_matches)} content-based matches found")
    
    # Group by Mac function
    mac_func_matches = defaultdict(list)
    for ref, gc_str_name, gc_ref_funcs in content_matches:
        key = ref.func_name
        mac_func_matches[key].append({
            'string': ref.content,
            'gc_str': gc_str_name,
            'gc_funcs': gc_ref_funcs,
            'binary': ref.binary,
        })
    
    # Step 4: Extract STABS source info
    print("\n[4] Extracting STABS source file paths...")
    lsw1_sources = extract_stabs_info(MAC_LSW1)
    lsw2_ppc_sources = extract_stabs_info(MAC_LSW2, "ppc")
    print(f"  LSW1 Demo: {len(lsw1_sources)} source paths")
    print(f"  LSW2 PPC: {len(lsw2_ppc_sources)} source paths")
    
    # Step 5: Extract data/BSS symbols
    print("\n[5] Extracting Mac data/BSS symbols...")
    lsw1_data = extract_macho_data_syms(MAC_LSW1)
    lsw2_ppc_data = extract_macho_data_syms(MAC_LSW2, "ppc")
    print(f"  LSW1 Demo: {len(lsw1_data)} data symbols")
    print(f"  LSW2 PPC: {len(lsw2_ppc_data)} data symbols")
    
    # Step 6: Generate deep match report
    print("\n[6] Generating deep match report...")
    
    lines = [
        "# Deep Mac-to-GC Match Report",
        "",
        "## Summary",
        "",
        f"- Mac string refs found: {len(all_mac_refs)}",
        f"- Content-based GC matches: {len(content_matches)}",
        f"- Unique Mac functions with string evidence: {len(mac_func_matches)}",
        "",
        "## String Content Matches (Mac function -> GC string -> GC function)",
        "",
        "Each entry shows a Mac function that references a string also referenced by a GC function.",
        "This is strong evidence they are the same function.",
        "",
        f"| # | Mac Func | Mac Binary | String Content | GC String | GC Func(s) |",
        "|---|----------|------------|----------------|-----------|------------|",
    ]
    
    # Sort by number of matching GC functions (most evidence first)
    sorted_matches = sorted(mac_func_matches.items(), 
                           key=lambda x: -len(x[1]))
    
    count = 0
    for mac_func, refs in sorted_matches:
        if count >= 200:
            break
        for r in refs[:3]:  # Show up to 3 strings per function
            gc_funcs_str = ", ".join(r['gc_funcs'][:3]) if r['gc_funcs'] else "(no GC func xref)"
            if not gc_funcs_str:
                gc_funcs_str = "(orphan string - no GC func references it)"
            count += 1
            lines.append(f"| {count} | `{mac_func[:60]}` | {r['binary']} | `{r['string'][:80]}` | {r['gc_str']} | `{gc_funcs_str}` |")
    
    # Source file paths section
    lines.extend([
        "",
        "---",
        "",
        "## STABS Source File Paths",
        "",
        "### LSW1 Demo",
    ])
    for s in sorted(lsw1_sources)[:30]:
        lines.append(f"- `{s}`")
    lines.extend(["", "### LSW2 PPC"])
    for s in sorted(lsw2_ppc_sources)[:30]:
        lines.append(f"- `{s}`")
    
    # Data/BSS symbols section
    lines.extend([
        "",
        "---",
        "",
        "## Selected Data/BSS Symbols (LSW1 Demo)",
        "",
        "These are global variable names from the Mac debug symbols that likely",
        "have corresponding variables in the GC binary (currently labeled lbl_*).",
        "",
        "| Name | Address | Section |",
        "|------|---------|---------|",
    ])
    
    # Filter interesting ones (skip _dyld_, etc.)
    interesting_data = []
    for name, addr, sec in lsw1_data:
        if any(skip in name for skip in ["dyld", "dyld_stub", "__mh_execute"]):
            continue
        if name.startswith("_g") and name[2].isupper():
            interesting_data.append((name, addr, sec))
        elif name.startswith("_Nu") or name.startswith("_AI") or name.startswith("_Menu"):
            interesting_data.append((name, addr, sec))
    
    for name, addr, sec in sorted(interesting_data, key=lambda x: x[1])[:60]:
        clean = name.lstrip("_")
        lines.append(f"| `{clean}` | {addr} | {sec} |")
    
    # Data/BSS from LSW2
    lines.extend([
        "",
        "### Selected Data/BSS Symbols (LSW2 PPC)",
        "",
        "| Name | Address | Section |",
        "|------|---------|---------|",
    ])
    
    interesting_data2 = []
    for name, addr, sec in lsw2_ppc_data:
        if any(skip in name for skip in ["dyld", "dyld_stub", "__mh_execute", "darwin"]):
            continue
        if name.startswith("_g") and name[2].isupper():
            interesting_data2.append((name, addr, sec))
        elif name.startswith("_Nu") or name.startswith("_AI") or name.startswith("_Menu"):
            interesting_data2.append((name, addr, sec))
        elif name.startswith("_Area") or name.startswith("_Pad") or name.startswith("_Cam"):
            interesting_data2.append((name, addr, sec))
    
    for name, addr, sec in sorted(interesting_data2, key=lambda x: x[1])[:60]:
        clean = name.lstrip("_")
        lines.append(f"| `{clean}` | {addr} | {sec} |")
    
    OUT_DEEP_MD.write_text("\n".join(lines) + "\n")
    print(f"  Wrote {OUT_DEEP_MD}")
    
    # Also write all Mac string xrefs for reference
    xref_lines = ["Mac String Xrefs", f"Total: {len(all_mac_refs)}", ""]
    for ref in all_mac_refs:
        func_line = f"  => {ref.func_name} (at 0x{ref.func_addr:x})" if ref.func_name != "<unknown>" else "  => <unknown>"
        xref_lines.append(f"0x{ref.ref_addr:x} {ref.binary:20s} \"{ref.content[:100]}\"{func_line}")
    (OUT_DIR / "mac_string_xrefs.txt").write_text("\n".join(xref_lines) + "\n")
    print(f"  Wrote {OUT_MAC_XREFS}")
    
    # Generate actionable rename proposals
    rename_lines = [
        "# Proposed GC Renames from Mac Deep Match",
        "# Format: confidence | mac_name -> gc_function_name | evidence",
        ""
    ]
    
    for mac_func, refs in sorted_matches:
        if not any(r['gc_funcs'] for r in refs):
            continue
        best_refs = [r for r in refs if r['gc_funcs']]
        if best_refs:
            gc_funcs_list = set()
            for r in best_refs:
                gc_funcs_list.update(r['gc_funcs'])
            for gc_f in gc_funcs_list:
                if gc_f.startswith("fn_"):
                    # This is a proposed rename!
                    clean_mac = mac_func.lstrip("_")
                    if clean_mac.startswith("_Z"):
                        # Extract base name from C++ mangled
                        m2 = re.match(r'_Z(\d+)([A-Za-z_]\w*)', clean_mac)
                        if m2:
                            clean_mac = m2.group(2)[:int(m2.group(1))]
                    rename_lines.append(
                        f"MEDIUM | {clean_mac} -> {gc_f} | string evidence: {best_refs[0]['string'][:60]}"
                    )
    
    (OUT_DIR / "proposed_gc_renames_v2.txt").write_text("\n".join(rename_lines) + "\n")
    print(f"  Wrote {len(rename_lines)} rename proposals")
    
    print("\n" + "=" * 70)
    print("DEEP ANALYSIS COMPLETE")
    print(f"String refs: {len(all_mac_refs)}")
    print(f"Content matches with GC: {len(content_matches)}")
    print(f"Unique Mac functions mapped: {len(mac_func_matches)}")
    print(f"LSW1 sources: {len(lsw1_sources)}")
    print(f"LSW2 sources: {len(lsw2_ppc_sources)}")
    print(f"LSW1 data syms: {len(lsw1_data)}")
    print(f"LSW2 data syms: {len(lsw2_ppc_data)}")
    print("=" * 70)


if __name__ == "__main__":
    main()
