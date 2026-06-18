#!/usr/bin/env python3
"""
Mac String Xref Scanner v3

Extracts strings from Mac Mach-O __cstring sections, scans __text for
PIC-based and absolute string references, and cross-references with GC strings.

Usage:
  python3 tools/mac_string_xref_scanner.py
"""

import re
import struct
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GC_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
MAC_LSW1 = ROOT / "orig/mac/mac-debug-symbols/LEGO Star Wars Demo"
MAC_LSW2 = ROOT / "orig/mac/mac-debug-symbols/LEGO Star Wars II"
OUT_DIR = ROOT / "docs/symbol_donors"

SYMBOL_LINE_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s+//\s+(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")

DOL_SECS = [
    (0x189B00, 0x8018CB00, 0x023B40),
    (0x184840, 0x803B7940, 0x00CC14),
    (0x191568, 0x803C4680, 0x0084A0),
]


def extract_section(path: Path, section: str, arch: str = "") -> tuple[bytes, int, int]:
    """Extract a section as (continuous_bytes, base_address, total_size)."""
    cmd = ["llvm-objdump", "--full-contents", f"--section={section}"]
    if arch:
        cmd.extend(["-arch", arch])
    cmd.append(str(path))
    res = subprocess.run(cmd, capture_output=True, text=True)
    
    chunks = []
    for line in res.stdout.splitlines():
        parts = line.split()
        if not parts:
            continue
        try:
            addr = int(parts[0].rstrip(":"), 16)
        except ValueError:
            continue
        data = bytearray()
        for p in parts[1:]:
            if len(p) == 8 and all(c in "0123456789abcdefABCDEF" for c in p.lower()):
                for i in range(0, 8, 2):
                    data.append(int(p[i:i+2], 16))
            elif len(p) == 2 and all(c in "0123456789abcdefABCDEF" for c in p.lower()):
                data.append(int(p, 16))
            else:
                break
        if data:
            chunks.append((addr, bytes(data)))
    
    if not chunks:
        return b"", 0, 0
    
    chunks.sort()
    base = chunks[0][0]
    result = bytearray()
    prev_end = base
    for addr, data in chunks:
        if addr > prev_end:
            result.extend(b'\x00' * (addr - prev_end))
        result.extend(data)
        prev_end = addr + len(data)
    return bytes(result), base, prev_end - base


def extract_cstring_map(data: bytes, base: int) -> dict[int, str]:
    """Extract null-terminated strings from cstring bytes.
    
    Returns {start_address: string_text}.
    """
    result = {}
    i = 0
    while i < len(data):
        null_pos = data.find(b'\x00', i)
        if null_pos == -1:
            if len(data) - i >= 2:
                result[base + i] = data[i:].decode('latin-1', errors='replace')
            break
        if null_pos - i >= 2:
            addr = base + i
            result[addr] = data[null_pos - 1:null_pos] if null_pos == i else data[i:null_pos].decode('latin-1', errors='replace')
            # Actually just the string part
            result[addr] = data[i:null_pos].decode('latin-1', errors='replace')
        i = null_pos + 1
    return result


def extract_nm(path: Path, arch: str = "") -> dict[int, str]:
    """Get text symbols: {address: name}."""
    cmd = ["llvm-nm", "-a"]
    if arch:
        cmd.extend(["-arch", arch])
    cmd.append(str(path))
    res = subprocess.run(cmd, capture_output=True, text=True)
    funcs = {}
    for line in res.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[1] in ('T', 't'):
            try:
                addr = int(parts[0], 16)
                funcs[addr] = parts[2]
            except ValueError:
                pass
    return funcs


def find_pic_bases(code: bytes, base_addr: int) -> dict[int, int]:
    """Find bcl+mflr r31 pattern. Returns {mflr_address: pc_base}."""
    bases = {}
    for off in range(len(code) - 8):
        if code[off:off+4] == b'\x42\x9f\x00\x05' and code[off+4:off+8] == b'\x7f\xe8\x02\xa6':
            bases[base_addr + off + 4] = base_addr + off + 4
    return bases


def scan_pic_string_refs(code: bytes, base_addr: int,
                          pic_bases: dict[int, int],
                          cstring_data: bytes, cstring_base: int,
                          cstring_map: dict[int, str],
                          func_map: dict[int, str],
                          binary_name: str) -> list[dict]:
    """Scan PIC-based addis+addi pairs that reference cstring range.
    
    Pattern: addis rD, r31, hi / addi rD, rD, lo
    Target = pc_base + (hi << 16) + sign_extend(lo)
    """
    refs = []
    insts_count = len(code) // 4
    
    for i in range(insts_count - 1):
        off = i * 4
        if off + 8 > len(code):
            break
        
        inst1 = struct.unpack(">I", code[off:off+4])[0]
        if inst1 >> 26 != 15:  # not addis/lis
            continue
        rd1 = (inst1 >> 21) & 0x1F
        ra1 = (inst1 >> 16) & 0x1F
        if ra1 != 31:  # not r31 relative
            continue
        hi = inst1 & 0xFFFF
        
        inst2 = struct.unpack(">I", code[off+4:off+8])[0]
        op2 = inst2 >> 26
        if op2 not in (14, 24, 32, 34, 35, 40):  # addi, ori, lwz, lbz, lhz, lfs
            continue
        use_ra = (inst2 >> 16) & 0x1F
        if use_ra != rd1:
            continue
        lo = inst2 & 0xFFFF
        
        ref_addr = base_addr + off
        
        # Find PIC base for this instruction
        pic = 0
        for mflr_addr in sorted(pic_bases.keys(), reverse=True):
            if ref_addr >= mflr_addr:
                pic = pic_bases[mflr_addr]
                break
        
        if pic == 0:
            continue
        
        lo_signed = lo if lo < 0x8000 else lo - 0x10000
        target = (pic + (hi << 16) + lo_signed) & 0xFFFFFFFF
        
        # Check if target falls within cstring range
        if cstring_base <= target < cstring_base + len(cstring_data):
            # Find the null-terminated string at this address
            offset_in_cstr = target - cstring_base
            remaining = cstring_data[offset_in_cstr:]
            null_pos = remaining.find(b'\x00')
            content = (remaining[:null_pos] if null_pos >= 0 else remaining).decode('latin-1', errors='replace') if remaining else ""
            
            if len(content) >= 2:
                func_name = "<unknown>"
                func_start = 0
                for f_addr in sorted(func_map.keys(), reverse=True):
                    if ref_addr >= f_addr:
                        func_name = func_map[f_addr]
                        func_start = f_addr
                        break
                
                refs.append({
                    'addr': ref_addr,
                    'func_name': func_name,
                    'func_start': func_start,
                    'str_addr': target,
                    'content': content,
                    'binary': binary_name,
                    'pattern': 'pic_addis+addi',
                })
    
    return refs


def scan_absolute_string_refs(code: bytes, base_addr: int,
                               cstring_data: bytes, cstring_base: int,
                               func_map: dict[int, str],
                               binary_name: str) -> list[dict]:
    """Scan absolute lis+addi patterns (ra=0) that reference cstring."""
    refs = []
    code_len = len(code)
    
    for off in range(0, code_len - 4, 4):
        inst1 = struct.unpack(">I", code[off:off+4])[0]
        if inst1 >> 26 != 15:
            continue
        ra = (inst1 >> 16) & 0x1F
        if ra != 0:
            continue
        rd = (inst1 >> 21) & 0x1F
        hi_imm = inst1 & 0xFFFF
        hi = hi_imm << 16
        ref_addr = base_addr + off
        
        for la in range(4, min(128, code_len - off - 4), 4):
            inst2 = struct.unpack(">I", code[off+la:off+la+4])[0]
            op2 = inst2 >> 26
            use_ra = (inst2 >> 16) & 0x1F
            if use_ra != rd:
                continue
            lo = inst2 & 0xFFFF
            
            target = None
            if op2 == 14:   # addi
                lo_s = lo if lo < 0x8000 else lo - 0x10000
                target = (hi + lo_s) & 0xFFFFFFFF
            elif op2 == 24:  # ori
                target = (hi | lo) & 0xFFFFFFFF
            elif op2 in (32, 34, 35, 40):  # loads
                lo_s = lo if lo < 0x8000 else lo - 0x10000
                target = (hi + lo_s) & 0xFFFFFFFF
            
            if target and cstring_base <= target < cstring_base + len(cstring_data):
                offset_in_cstr = target - cstring_base
                remaining = cstring_data[offset_in_cstr:]
                null_pos = remaining.find(b'\x00')
                content = (remaining[:null_pos] if null_pos >= 0 else remaining).decode('latin-1', errors='replace') if remaining else ""
                
                if len(content) >= 2:
                    func_name = "<unknown>"
                    func_start = 0
                    for f_addr in sorted(func_map.keys(), reverse=True):
                        if ref_addr >= f_addr:
                            func_name = func_map[f_addr]
                            func_start = f_addr
                            break
                    
                    refs.append({
                        'addr': ref_addr,
                        'func_name': func_name,
                        'func_start': func_start,
                        'str_addr': target,
                        'content': content,
                        'binary': binary_name,
                        'pattern': f'abs_lis+op{op2}',
                    })
                break
    
    return refs


def parse_gc_strings() -> tuple[dict[str, set[str]], dict[str, str]]:
    """Parse GC string xrefs and extract content from DOL."""
    xref_path = ROOT / "build/xrefs/string_xrefs.txt"
    str_to_func = defaultdict(set)
    
    if xref_path.exists():
        for line in xref_path.read_text().splitlines():
            parts = line.split()
            if len(parts) >= 4 and parts[0].startswith("str_"):
                str_to_func[parts[0]].add(parts[2])
    
    dol_data = GC_DOL.read_bytes()
    str_name_to_content = {}
    
    for line in GC_SYMBOLS.read_text().splitlines():
        m = SYMBOL_LINE_RE.match(line)
        if not m or not m.group("name").startswith("str_"):
            continue
        name = m.group("name")
        addr = int(m.group("addr"), 16)
        meta = m.group("meta")
        size_m = SIZE_RE.search(meta)
        size = int(size_m.group(1), 16) if size_m else 0
        
        for file_off, base_addr, sec_size in DOL_SECS:
            if base_addr <= addr < base_addr + sec_size:
                offset = file_off + (addr - base_addr)
                raw = dol_data[offset:offset + max(size, 4)] if size else dol_data[offset:offset+512]
                content = raw.split(b'\x00')[0].decode('latin-1', errors='replace')
                if content:
                    str_name_to_content[name] = content
                break
    
    if xref_path.exists():
        for line in xref_path.read_text().splitlines():
            parts = line.split()
            if len(parts) >= 2 and parts[0].startswith("str_"):
                sn = parts[0]
                if sn not in str_name_to_content:
                    try:
                        sa = int(parts[1], 16)
                    except ValueError:
                        continue
                    for fo, ba, sz in DOL_SECS:
                        if ba <= sa < ba + sz:
                            off = fo + (sa - ba)
                            raw = dol_data[off:off+256]
                            ct = raw.split(b'\x00')[0].decode('latin-1', errors='replace')
                            if ct:
                                str_name_to_content[sn] = ct
                            break
    
    return str_to_func, str_name_to_content


def main():
    print("=" * 60)
    print("Mac String Xref Scanner v3 (Range-based)")
    print("=" * 60)
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    
    # 1. Load GC strings
    print("\n[1] Loading GC string xrefs and content...")
    gc_str_to_func, gc_str_content = parse_gc_strings()
    print(f"  {len(gc_str_to_func)} GC str_* symbols")
    print(f"  {len(gc_str_content)} with content")
    
    content_to_gc = {}
    for name, text in gc_str_content.items():
        clean = text.strip().rstrip('\x00').strip()
        if clean and len(clean) >= 3:
            content_to_gc[clean] = name
    
    for name, text in gc_str_content.items():
        clean = text.strip().rstrip('\x00').strip()
        for i in range(8, min(30, len(clean)), 2):
            sub = clean[:i]
            if sub and sub not in content_to_gc:
                content_to_gc[sub] = name
    
    print(f"  {len(content_to_gc)} string keys for matching")
    
    # 2. Process each Mac binary
    print("\n[2] Processing Mac binaries...")
    
    all_refs = []
    mac_binaries = [
        ("LSW1 Demo", MAC_LSW1, ""),
        ("LSW2 PPC", MAC_LSW2, "ppc"),
    ]
    
    for label, exe, arch in mac_binaries:
        print(f"\n  --- {label} ---")
        
        # Extract __cstring
        cstr_data, cstr_base, cstr_size = extract_section(exe, "__cstring", arch)
        if not cstr_data:
            # Try without arch for universal binary
            cstr_data, cstr_base, cstr_size = extract_section(exe, "__cstring")
        if not cstr_data:
            print(f"  WARNING: No __cstring section")
            continue
        print(f"  __cstring: {cstr_size:,} bytes at 0x{cstr_base:x}")
        
        # Extract string map
        str_map = extract_cstring_map(cstr_data, cstr_base)
        print(f"  Individual strings: {len(str_map)}")
        
        # Extract __text
        text_data, text_base, text_size = extract_section(exe, "__text", arch)
        if not text_data:
            text_data, text_base, text_size = extract_section(exe, "__text")
        if not text_data:
            print(f"  WARNING: No __text section")
            continue
        print(f"  __text: {text_size:,} bytes at 0x{text_base:x}")
        
        # Get symbols
        func_map = extract_nm(exe, arch)
        if not func_map:
            func_map = extract_nm(exe)
        print(f"  Text symbols: {len(func_map)}")
        
        # Find PIC bases
        pic_bases = find_pic_bases(text_data, text_base)
        print(f"  PIC bases: {len(pic_bases)}")
        
        # Scan PIC refs
        refs_pic = scan_pic_string_refs(
            text_data, text_base, pic_bases,
            cstr_data, cstr_base, str_map,
            func_map, label
        )
        print(f"  PIC string refs: {len(refs_pic)}")
        
        # Scan absolute refs (for LSW2 which uses absolute addressing)
        refs_abs = scan_absolute_string_refs(
            text_data, text_base,
            cstr_data, cstr_base,
            func_map, label
        )
        print(f"  Absolute string refs: {len(refs_abs)}")
        
        all_refs.extend(refs_pic)
        all_refs.extend(refs_abs)
    
    if not all_refs:
        print("\n  ERROR: No string references found")
        sys.exit(1)
    
    # 3. Cross-reference by content
    print(f"\n[3] Cross-referencing with GC strings...")
    
    mac_func_to_gc = defaultdict(list)
    matched_refs = []
    
    for ref in all_refs:
        content = ref.get('content', '').strip()
        if not content or len(content) < 3:
            continue
        
        matched_gc = None
        if content in content_to_gc:
            matched_gc = content_to_gc[content]
        else:
            for gc_key, gc_name in content_to_gc.items():
                if content.startswith(gc_key) or gc_key.startswith(content):
                    matched_gc = gc_name
                    break
        
        if matched_gc:
            gc_funcs = gc_str_to_func.get(matched_gc, set())
            matched_refs.append((ref, matched_gc, gc_funcs))
            mac_func_to_gc[ref['func_name']].append(
                (matched_gc, gc_funcs, content[:80])
            )
    
    print(f"  Content-matched: {len(matched_refs)}")
    print(f"  Unique Mac functions linked: {len(mac_func_to_gc)}")
    
    named = sum(1 for _, m in mac_func_to_gc.items() for _, fs, _ in m for f in fs if not f.startswith("fn_"))
    unnamed = sum(1 for _, m in mac_func_to_gc.items() for _, fs, _ in m for f in fs if f.startswith("fn_"))
    print(f"  -> named GC: {named}, unnamed: {unnamed}")
    
    # 4. Write outputs
    print(f"\n[4] Writing outputs...")
    
    # 4a. All Mac string xrefs
    lines = [
        "# Mac String Xrefs",
        f"# Total: {len(all_refs)}",
        "# ref_addr | binary | str_addr | pattern | func_name | content",
        ""
    ]
    for ref in sorted(all_refs, key=lambda r: (r['binary'], r['addr'])):
        lines.append(
            f"0x{ref['addr']:08x} | {ref['binary']:12s} | "
            f"0x{ref['str_addr']:08x} | {ref['pattern']:15s} | "
            f"{ref['func_name'][:40]:40s} | \"{ref.get('content', '')[:100]}\""
        )
    (OUT_DIR / "mac_string_xrefs.txt").write_text("\n".join(lines) + "\n")
    print(f"  mac_string_xrefs.txt: {len(all_refs)} refs")
    
    # 4b. Match report
    md = [
        "# Mac-GC String Cross-Reference Report",
        "",
        f"- Mac refs: {len(all_refs)}, matched: {len(matched_refs)}",
        f"- Unique Mac functions with string evidence: {len(mac_func_to_gc)}",
        "",
        "## String-Evidence Matches",
        "",
        "| # | Mac Function | Binary | GC String | GC Funcs | Evidence |",
        "|---|-------------|--------|-----------|----------|----------|",
    ]
    
    def sk(item):
        for _, fs, _ in item[1]:
            for f in fs:
                if not f.startswith("fn_"):
                    return 0
        return 1
    
    c = 0
    for mf, gs in sorted(mac_func_to_gc.items(), key=sk):
        for gn, gfs, ev in gs:
            c += 1
            if c > 500:
                break
            fs = ", ".join(sorted(gfs)[:4]) if gfs else "(orphan)"
            bn = ev[:6] if ev else "?"
            md.append(f"| {c} | `{mf[:50]}` | {gn.split('_')[0] if '_' in gn else '-'} | {gn} | `{fs}` | `{ev[:60]}` |")
        if c > 500:
            md.append("| ... |")
            break
    
    md.extend([
        "",
        "## Summary",
        "",
        f"| Metric | Value |",
        "|--------|-------|",
        f"| Mac string refs | {len(all_refs)} |",
        f"| Content matches | {len(matched_refs)} |",
        f"| Mac funcs matched | {len(mac_func_to_gc)} |",
        f"| To named GC funcs | {named} |",
        f"| To unnamed GC funcs | {unnamed} |",
    ])
    
    (OUT_DIR / "mac_gc_string_match_report.md").write_text("\n".join(md) + "\n")
    print(f"  mac_gc_string_match_report.md")
    
    # 4c. TSV
    tsv = ["# mac_func\tbinary\tcontent\tgc_string\tgc_func\tref_addr"]
    for ref, gn, gfs in matched_refs:
        for gf in sorted(gfs)[:3]:
            tsv.append(
                f"{ref['func_name']}\t{ref['binary']}\t"
                f"{ref.get('content', '')[:120]}\t{gn}\t{gf}\t0x{ref['addr']:08x}"
            )
    (OUT_DIR / "mac_gc_string_match_table.tsv").write_text("\n".join(tsv) + "\n")
    print(f"  mac_gc_string_match_table.tsv")
    
    # 4d. Proposed renames
    seen = set()
    rn = ["# Proposed GC Renames (string-evidence)", "# mac_name -> gc_fn | evidence", ""]
    for mf, gs in sorted(mac_func_to_gc.items(), key=sk):
        for gn, gfs, ev in gs:
            for gf in gfs:
                if gf.startswith("fn_") and mf not in seen:
                    clean = mf.lstrip("_")
                    rn.append(f"MEDIUM | {clean} -> {gf} | `{ev}`")
                    seen.add(mf)
    (OUT_DIR / "proposed_gc_renames_v2.txt").write_text("\n".join(rn) + "\n")
    print(f"  proposed_gc_renames_v2.txt: {len(rn)-2}")
    
    # 4e. High confidence
    hc = ["# High-Confidence Mac->GC (string-evidence)", "# mac_name\tgc_name\tevidence"]
    for mf, gs in sorted(mac_func_to_gc.items(), key=sk):
        for gn, gfs, ev in gs:
            for gf in gfs:
                if not gf.startswith("fn_"):
                    hc.append(f"{mf.lstrip('_')}\t{gf}\t{ev[:80]}")
    (OUT_DIR / "mac_gc_high_confidence_renames.tsv").write_text("\n".join(hc) + "\n")
    print(f"  mac_gc_high_confidence_renames.tsv: {len(hc)-2}")
    
    print(f"\n{'=' * 60}")
    print(f"Done. {len(all_refs)} refs, {len(matched_refs)} matches, {len(mac_func_to_gc)} funcs.")
    print(f"{'=' * 60}")


if __name__ == "__main__":
    main()
