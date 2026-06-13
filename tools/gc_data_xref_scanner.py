#!/usr/bin/env python3
"""
GC data xref scanner.

Scans GC .text for PowerPC instruction patterns that reference lbl_* data
addresses: lis+addi/ori, lis+load/store, SDA (r2), SDA2 (r13).

Outputs:
  docs/symbol_donors/gc_data_xrefs.tsv
  docs/symbol_donors/mac_data_to_gc_verified.md
"""

import re
import struct
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GC_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
OUT_DIR = ROOT / "docs/symbol_donors"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s+//\s+(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")

# DOL sections: (file_offset, address, size)
DOL_SECTIONS = [
    (0x00000100, 0x80003100, 0x000003A0),  # .init
    (0x000004A0, 0x800034A0, 0x00189660),  # .text
    (0x00189B00, 0x8018CB00, 0x00023B40),  # .data_early
    (0x001AD640, 0x801B0640, 0x0004CA20),  # .data+.rodata
    (0x001FA060, 0x80407A40, 0x00002880),  # .sdata
    (0x001FC8E0, 0x8040B300, 0x00000A40),  # .sdata2
]

# .sbss is between .sdata end (0x8040A2C0) and .sdata2 start (0x8040B300)
SBSS_START = 0x8040A2C0
SBSS_END = 0x8040B300

# SDA base: r2 typically points to middle of .sdata
# SDA2 base: r13 typically points to middle of .sdata2
# We'll detect these from symbols if available, else compute
SDA_BASE = 0x80408E70   # (0x80407A40 + 0x8040A2C0) / 2, approx
SDA2_BASE = 0x8040B820  # (0x8040B300 + 0x8040BD40) / 2, approx

# Access type classification
ACCESS_READ = {"lwz": "read", "lbz": "read", "lhz": "read", "lfs": "read_float",
               "lfd": "read_double", "lha": "read", "lwzu": "read_update"}
ACCESS_WRITE = {"stw": "write", "stb": "write", "sth": "write", "stfs": "write_float",
                "stfd": "write_double", "stwu": "write_update", "stfsu": "write_float_update"}
ACCESS_ADDR = {"addi": "addr", "ori": "addr"}
ALL_OPS = {**ACCESS_READ, **ACCESS_WRITE, **ACCESS_ADDR}
OPCODE_MAP = {
    14: "addi", 24: "ori", 32: "lwz", 34: "lbz", 35: "lhz",
    36: "sth", 37: "stfs", 40: "lfs", 44: "stb",
    46: "sthw", 48: "lfd", 50: "lfds", 52: "stfd",
}


def read_dol_section(sec_offset: int, sec_addr: int, sec_size: int) -> bytes:
    """Read a DOL section's raw bytes."""
    with open(str(GC_DOL), "rb") as f:
        f.seek(sec_offset)
        return f.read(sec_size)


def parse_symbols() -> list[dict]:
    """Parse all symbols from symbols.txt."""
    syms = []
    for line in GC_SYMBOLS.read_text().splitlines():
        m = SYMBOL_RE.match(line)
        if not m:
            continue
        name = m.group("name")
        sec = m.group("section")
        addr = int(m.group("addr"), 16)
        meta = m.group("meta")
        sm = SIZE_RE.search(meta)
        sz = int(sm.group(1), 16) if sm else 0
        syms.append({
            'name': name, 'section': sec, 'address': addr,
            'size': sz, 'meta': meta,
        })
    return syms


def build_func_map(syms: list[dict]) -> dict[int, str]:
    """Build {address: name} for functions in .init and .text."""
    funcs = {}
    for s in syms:
        if s['section'] in (".init", ".text") and "type:function" in s['meta']:
            funcs[s['address']] = s['name']
    return funcs


def find_func(addr: int, func_map: dict[int, str]) -> str:
    """Find the function containing a given address."""
    name = "<unknown>"
    for f_addr in sorted(func_map.keys(), reverse=True):
        if addr >= f_addr:
            name = func_map[f_addr]
            break
    return name


def scan_text_for_xrefs(text_bytes: bytes, text_base: int,
                         func_map: dict[int, str],
                         lbl_symbols: list[dict]) -> list[dict]:
    """Scan .text for all patterns referencing lbl_* addresses.

    Returns list of {lbl_addr, lbl_name, func_name, ref_addr, op_type, op_name}.
    """
    # Pre-compute lbl lookup by hi16 for fast matching
    lbl_by_hi16 = defaultdict(list)
    lbl_addr_set = set()
    for s in lbl_symbols:
        hi = (s['address'] >> 16) & 0xFFFF
        lo = s['address'] & 0xFFFF
        lbl_by_hi16[hi].append((s['address'], s['name'], lo))
        lbl_addr_set.add(s['address'])

    xrefs = []
    code = text_bytes
    base = text_base
    code_len = len(code)
    lookahead_max = 100  # instructions

    for off in range(0, code_len - 4, 4):
        inst = struct.unpack(">I", code[off:off+4])[0]
        opcode = inst >> 26

        # --- Absolute pattern: lis $rD, hi16 (ra=0) ---
        if opcode == 15:  # lis / addis
            rd = (inst >> 21) & 0x1F
            ra = (inst >> 16) & 0x1F
            if ra != 0:
                continue  # not absolute
            hi16 = inst & 0xFFFF

            # Check if this hi16 matches any lbl address
            candidates = lbl_by_hi16.get(hi16, [])
            if not candidates:
                continue

            ref_addr = base + off
            func_name = find_func(ref_addr, func_map)

            # Look forward for use of rd with lo16
            for la in range(4, min(lookahead_max * 4, code_len - off - 4), 4):
                inst2 = struct.unpack(">I", code[off+la:off+la+4])[0]
                op2 = inst2 >> 26
                use_rd = (inst2 >> 21) & 0x1F
                use_ra = (inst2 >> 16) & 0x1F
                lo = inst2 & 0xFFFF

                if use_ra != rd:
                    continue

                op_name = OPCODE_MAP.get(op2)
                if op_name not in ALL_OPS:
                    continue

                # Compute full target address based on opcode
                target = None
                if op2 in (14,):  # addi: signed lo
                    lo_s = lo if lo < 0x8000 else lo - 0x10000
                    target = ((hi16 << 16) + lo_s) & 0xFFFFFFFF
                elif op2 == 24:  # ori: unsigned lo
                    target = ((hi16 << 16) | lo) & 0xFFFFFFFF
                elif op2 in ACCESS_READ or op2 in ACCESS_WRITE:  # load/store: signed offset
                    lo_s = lo if lo < 0x8000 else lo - 0x10000
                    target = ((hi16 << 16) + lo_s) & 0xFFFFFFFF

                if target is None or target not in lbl_addr_set:
                    continue

                # Found a reference!
                a_type = ALL_OPS.get(op_name, "unknown")
                lbl_info = [(a, n, l) for a, n, l in candidates if a == target]
                if not lbl_info:
                    continue

                _, lbl_name, _ = lbl_info[0]

                xrefs.append({
                    'lbl_addr': target,
                    'lbl_name': lbl_name,
                    'func_name': func_name,
                    'ref_addr': ref_addr + la,
                    'op_type': a_type,
                    'op_name': op_name,
                })
                break  # first use per lis, not all uses (too noisy)

    return xrefs


def scan_sda_xrefs(text_bytes: bytes, text_base: int,
                    func_map: dict[int, str],
                    lbl_symbols: list[dict],
                    sda_base: int, sda_reg: int,
                    section_name: str) -> list[dict]:
    """Scan for SDA/SDA2 relative access patterns.
    
    r2 = SDA base for .sdata
    r13 = SDA2 base for .sdata2
    
    Pattern: lwz/lfs/etc $rX, offset($r2/r13)
    where offset = lbl_addr - sda_base, as signed 16-bit.
    """
    # Build lbl lookup by offset from SDA base
    sec_syms = [s for s in lbl_symbols if s['section'] == section_name]
    lbl_by_offset = {}
    for s in sec_syms:
        offset = s['address'] - sda_base
        if -0x8000 <= offset < 0x8000:
            lbl_by_offset[offset & 0xFFFF] = s  # store as unsigned for matching

    if not lbl_by_offset:
        return []

    xrefs = []
    code = text_bytes
    base = text_base
    code_len = len(code)

    for off in range(0, code_len - 4, 4):
        inst = struct.unpack(">I", code[off:off+4])[0]
        opcode = inst >> 26
        ra = (inst >> 16) & 0x1F

        if ra != sda_reg:
            continue

        op_name = OPCODE_MAP.get(opcode)
        if op_name not in ALL_OPS:
            continue

        lo = inst & 0xFFFF
        # Convert back to signed
        lo_s = lo if lo < 0x8000 else lo - 0x10000
        actual_addr = sda_base + lo_s

        if lo not in lbl_by_offset:
            continue

        s = lbl_by_offset[lo]
        if s['address'] != actual_addr:
            continue

        ref_addr = base + off
        func_name = find_func(ref_addr, func_map)
        a_type = ALL_OPS.get(op_name, "unknown")

        xrefs.append({
            'lbl_addr': s['address'],
            'lbl_name': s['name'],
            'func_name': func_name,
            'ref_addr': ref_addr,
            'op_type': a_type,
            'op_name': op_name,
        })

    return xrefs


def get_nearby_functions(func_name: str, func_map: dict[int, str]) -> list[str]:
    """Get nearby function names for context (same neighborhood)."""
    # Find the address of this function
    addr = None
    for f_addr, f_name in func_map.items():
        if f_name == func_name:
            addr = f_addr
            break
    if addr is None:
        return []
    
    nearby = []
    for f_addr, f_name in sorted(func_map.items()):
        if abs(f_addr - addr) < 0x2000 and f_name != func_name:
            nearby.append(f_name)
    return nearby[:6]


def main():
    print("=" * 60, flush=True)
    print("GC Data Xref Scanner", flush=True)
    print("=" * 60, flush=True)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    # --- 1. Load symbols ---
    print("\n[1] Loading symbols...", flush=True)
    all_syms = parse_symbols()
    lbl_syms = [s for s in all_syms if s['name'].startswith("lbl_")]
    func_map = build_func_map(all_syms)
    print(f"  {len(all_syms)} total, {len(lbl_syms)} lbl_*, {len(func_map)} functions", flush=True)

    # --- 2. Read .text ---
    print("\n[2] Reading .text section...", flush=True)
    text_base = DOL_SECTIONS[1][1]
    text_bytes = read_dol_section(*DOL_SECTIONS[1])
    print(f"  {len(text_bytes):,} bytes at 0x{text_base:x}", flush=True)

    # --- 3. Scan for absolute (lis+addi/load/store) xrefs ---
    print("\n[3] Scanning absolute lis+addi/load/store patterns...", flush=True)
    abs_xrefs = scan_text_for_xrefs(text_bytes, text_base, func_map, lbl_syms)
    print(f"  {len(abs_xrefs)} references found", flush=True)

    # Count by access type
    atypes = defaultdict(int)
    for x in abs_xrefs:
        atypes[x['op_type']] += 1
    for t, c in sorted(atypes.items(), key=lambda x: -x[1]):
        print(f"    {t}: {c}")

    # --- 4. Scan for SDA (r2) xrefs ---
    print("\n[4] Scanning SDA (r2) relative patterns...", flush=True)
    sda_xrefs = scan_sda_xrefs(text_bytes, text_base, func_map, lbl_syms, SDA_BASE, 2, ".sdata")
    print(f"  {len(sda_xrefs)} references found (SDA base=0x{SDA_BASE:08x})", flush=True)

    sda2_xrefs = scan_sda_xrefs(text_bytes, text_base, func_map, lbl_syms, SDA2_BASE, 13, ".sdata2")
    print(f"  {len(sda2_xrefs)} references found (SDA2 base=0x{SDA2_BASE:08x})", flush=True)

    # --- 5. Combine and write TSV ---
    all_xrefs = abs_xrefs + sda_xrefs + sda2_xrefs
    print(f"\n[5] Total xrefs: {len(all_xrefs)}", flush=True)

    # Build per-lbl-address summary
    lbl_xrefs = defaultdict(list)
    for x in all_xrefs:
        lbl_xrefs[x['lbl_addr']].append(x)

    tsv_lines = [
        "# gc_lbl_addr\tgc_lbl_name\tref_func\tref_addr\ttag\top"
    ]
    for x in sorted(all_xrefs, key=lambda x: (x['lbl_addr'], x['ref_addr'])):
        tsv_lines.append(
            f"0x{x['lbl_addr']:08x}\t{x['lbl_name']}\t"
            f"{x['func_name']}\t0x{x['ref_addr']:08x}\t"
            f"{x['op_type']}\t{x['op_name']}"
        )
    tsv_path = OUT_DIR / "gc_data_xrefs.tsv"
    tsv_path.write_text("\n".join(tsv_lines) + "\n")
    print(f"  Wrote {tsv_path} ({len(tsv_lines)} lines)", flush=True)

    # --- 6. Load Mac data candidates and re-score ---
    print("\n[6] Re-scoring Mac data candidates with xref evidence...", flush=True)

    # Load Mac symbols
    import subprocess
    cmd = ["llvm-nm", "-a", str(ROOT / "orig/mac-debug-symbols/LEGO Star Wars Demo")]
    nm_res = subprocess.run(cmd, capture_output=True, text=True)
    mac_raw = []
    for line in nm_res.stdout.splitlines():
        parts = line.split()
        if len(parts) < 3:
            continue
        st = parts[1]
        if st not in ("D", "d", "B", "b", "S", "s"):
            continue
        try:
            addr = int(parts[0], 16)
        except ValueError:
            continue
        name = parts[2]
        if not name.startswith("_"):
            name = "_" + name
        cat = "data" if st in ("D", "d") else "bss" if st in ("B", "b") else "sdata"
        mac_raw.append({'name': name, 'addr': addr, 'cat': cat, 'size': 0})
    mac_raw.sort(key=lambda x: x['addr'])
    for i in range(len(mac_raw) - 1):
        if mac_raw[i]['cat'] == mac_raw[i + 1]['cat']:
            gap = mac_raw[i + 1]['addr'] - mac_raw[i]['addr']
            if 0 < gap <= 0x100000:
                mac_raw[i]['size'] = gap

    mac_by_addr = {m['addr']: m for m in mac_raw}
    mac_by_cat_size = defaultdict(list)
    for m in mac_raw:
        mac_by_cat_size[(m['cat'], m['size'])].append(m)

    def classify_mac(name):
        c = name.lstrip("_")
        if c.startswith("gNu") or c.startswith("Nu"): return "Nu2"
        if c.startswith("gAI") or "AISYS" in c: return "AI"
        if "Pad" in c or "Key" in c or "Mouse" in c: return "Input"
        if "Cam" in c or "View" in c: return "Camera"
        if "Menu" in c or "Gui" in c: return "Menu"
        if "Anim" in c or "Bone" in c: return "Anim"
        if "Sound" in c or "Sfx" in c or "Audio" in c: return "Audio"
        if "Part" in c or "Bolt" in c or "Blast" in c: return "FX"
        if "Force" in c or "Light" in c: return "FX"
        if "Script" in c or "Action" in c or "Cond" in c: return "Script"
        if "File" in c or "Stream" in c: return "FileIO"
        if "Heap" in c or "Alloc" in c or "Mem" in c: return "Memory"
        if "Save" in c or "Load" in c or "Config" in c: return "Config"
        if "Scene" in c or "World" in c or "Level" in c: return "Scene"
        if c.startswith("g") and len(c) > 1 and c[1].isupper(): return "Global"
        return "Other"

    mac_subsystems = {m['name']: classify_mac(m['name']) for m in mac_raw}

    # For each lbl_* that has xrefs, find best Mac candidate
    verified = []
    for lbl_sym in lbl_syms:
        addr = lbl_sym['address']
        if addr not in lbl_xrefs:
            continue

        xrefs_for_lbl = lbl_xrefs[addr]
        xref_funcs = list(set(x['func_name'] for x in xrefs_for_lbl))
        xref_types = list(set(x['op_type'] for x in xrefs_for_lbl))
        
        # Categorize xref functions
        xref_sub = set()
        for fn in xref_funcs:
            fn_lower = fn.lower()
            if "nu" in fn_lower: xref_sub.add("Nu2")
            if "ai" in fn_lower: xref_sub.add("AI")
            if "menu" in fn_lower: xref_sub.add("Menu")
            if "anim" in fn_lower: xref_sub.add("Anim")
            if "sound" in fn_lower or "sfx" in fn_lower: xref_sub.add("Audio")
            if "script" in fn_lower or "action" in fn_lower: xref_sub.add("Script")
            if "file" in fn_lower or "dat" in fn_lower or "stream" in fn_lower: xref_sub.add("FileIO")
            if "cam" in fn_lower or "view" in fn_lower: xref_sub.add("Camera")
            if "pad" in fn_lower or "key" in fn_lower or "input" in fn_lower: xref_sub.add("Input")
            if "part" in fn_lower or "bolt" in fn_lower: xref_sub.add("FX")
            if "save" in fn_lower or "load" in fn_lower: xref_sub.add("Config")
            if "heap" in fn_lower or "alloc" in fn_lower or "mem" in fn_lower: xref_sub.add("Memory")

        # Try to find Mac candidate by size + section + name/sub pattern matching
        gc_sec = lbl_sym['section']
        gc_size = lbl_sym['size']

        mcat_map = {".data": "data", ".bss": "bss", ".sdata": "sdata",
                     ".sbss": "bss", ".sdata2": "sdata2", ".rodata": "data"}
        mcat = mcat_map.get(gc_sec, "")

        best_mac = None
        best_score = 0
        best_ev = []

        # Score each Mac symbol with matching category and close size
        for mm in mac_by_cat_size.get((mcat, gc_size), []):
            mname = mm['name'].lstrip("_")
            msub = mac_subsystems[mm['name']]
            score = 10  # exact size + section
            ev = [f"size+sec"]

            # Subsystem agreement between Mac name and xref functions
            if msub != "Other" and msub in xref_sub:
                score += 5
                ev.append(f"xsub:{msub}")
            elif msub != "Other":
                score += 1  # at least named, even if not matching xref

            # Name word overlap with xref func names
            mname_words = set(re.findall(r'[A-Z][a-z]+', mname))
            for fn in xref_funcs:
                fn_words = set(re.findall(r'[A-Z][a-z]+', fn.replace("_", "")))
                overlap = mname_words & fn_words
                if overlap:
                    score += 2
                    ev.append(f"word:{','.join(overlap)[:30]}")

            # Multiple xref types = more used
            if len(xref_types) >= 2:
                score += 2
                ev.append(f"xtypes:{','.join(xref_types)}")
            if len(xref_funcs) >= 3:
                score += 2
                ev.append(f"xfuncs:{len(xref_funcs)}")

            # Alignment bonus
            align_m = re.search(r"align:(\d+)", lbl_sym['meta'])
            if align_m:
                al = int(align_m.group(1))
                if mm['size'] % al == 0:
                    score += 1

            if score > best_score:
                best_score = score
                best_mac = mm
                best_ev = ev

        # Also check Mac symbols with nearby size
        if best_score < 10:
            for size_delta in (4, 8, 16, 32, 64, 128, 256, -4, -8, -16, -32, -64, -128, -256):
                alt_size = gc_size + size_delta
                if alt_size <= 0:
                    continue
                candidates = mac_by_cat_size.get((mcat, alt_size), [])
                for mm in candidates:
                    mname = mm['name'].lstrip("_")
                    msub = mac_subsystems[mm['name']]
                    score = 5 + (3 if size_delta == 0 else 0)
                    ev = [f"size:near ({mm['size']} vs 0x{gc_size:X})"]

                    if msub != "Other" and msub in xref_sub:
                        score += 5
                        ev.append(f"xsub:{msub}")
                    elif msub != "Other":
                        score += 1

                    mname_words = set(re.findall(r'[A-Z][a-z]+', mname))
                    for fn in xref_funcs:
                        fn_words = set(re.findall(r'[A-Z][a-z]+', fn.replace("_", "")))
                        overlap = mname_words & fn_words
                        if overlap:
                            score += 2
                            ev.append(f"word:{','.join(overlap)[:30]}")

                    if len(xref_funcs) >= 2:
                        score += 1
                    if len(xref_types) >= 2:
                        score += 1

                    if score > best_score:
                        best_score = score
                        best_mac = mm
                        best_ev = ev
                    break  # only the first match per size delta
                if best_score >= 12:
                    break

        if best_mac and best_score >= 0:
            verified.append({
                'score': best_score,
                'gc': lbl_sym,
                'mac': best_mac,
                'ev': best_ev,
                'xref_funcs': xref_funcs,
                'xref_types': xref_types,
                'xref_count': len(xrefs_for_lbl),
            })

    # Sort by score descending, then by GC address
    verified.sort(key=lambda x: (-x['score'], x['gc']['address']))

    # Deduplicate: each Mac symbol can map to only the single best GC match
    # All other GC matches for the same Mac symbol become REJECT
    seen_mac = {}  # mac_name -> winning GC name
    deduped = []
    for v in verified:
        mac_name = v['mac']['name']
        if mac_name in seen_mac:
            winner = seen_mac[mac_name]
            v['rejected_reason'] = f"Mac already matched to better GC ({winner})"
            deduped.append(v)  # still included, but marked as reject
        else:
            seen_mac[mac_name] = v['gc']['name']
            deduped.append(v)

    # Determine confidence (with dedup awareness)
    for v in deduped:
        if 'rejected_reason' in v:
            v['conf'] = "REJECT"
        elif v['score'] >= 15:
            v['conf'] = "HIGH"
        elif v['score'] >= 8:
            v['conf'] = "MEDIUM"
        elif v['score'] >= 4:
            v['conf'] = "LOW"
        else:
            v['conf'] = "REJECT"

    verified = deduped
    high = [v for v in verified if v['conf'] == "HIGH"]
    med = [v for v in verified if v['conf'] == "MEDIUM"]
    low = [v for v in verified if v['conf'] == "LOW"]
    reject = [v for v in verified if v['conf'] == "REJECT"]

    print(f"\n  Scoring results (after dedup):")
    print(f"    HIGH: {len(high)}")
    print(f"    MEDIUM: {len(med)}")
    print(f"    LOW: {len(low)}")
    print(f"    REJECT: {len(reject)}")

    # --- 7. Write verified candidates report ---
    print(f"\n[7] Writing verified candidates report...", flush=True)

    lines = [
        "# Mac Data -> GC lbl_* Verified Candidates (with GC xref evidence)",
        "",
        "Each GC lbl_* object was scanned for code references. The functions",
        "that reference it provide subsystem context. Mac candidates are scored",
        "by size match + section match + xref function subsystem agreement.",
        "",
        "**Confidence:**",
        "- **HIGH** (>=15): exact size + section + xref functions match Mac subsystem",
        "- **MEDIUM** (8-14): exact/close size + some xref context",
        "- **LOW** (4-7): size hint + weak xref context",
        "- **REJECT** (<4): size collision, no useful xrefs, or conflicting",
        "",
        "---",
        "",
        "## HIGH Confidence",
        "",
        "| Mac Symbol | Mac Sz | GC Object | GC Sz | GC Sec | Score | Xref Funcs | Xref Types | Evidence |",
        "|------------|--------|-----------|-------|--------|-------|------------|------------|----------|",
    ]

    for v in high:
        mac = v['mac']
        gc = v['gc']
        ev = "; ".join(v['ev'][:4])
        xf = ", ".join(v['xref_funcs'][:4])
        xt = ", ".join(v['xref_types'])
        ms = f"~{mac['size']}" if mac['size'] else "?"
        lines.append(
            f"| `{mac['name'].lstrip('_')[:45]}` | {ms} | {gc['name']} | "
            f"0x{gc['size']:X} | {gc['section']} | {v['score']} | "
            f"{xf[:60]} | {xt[:40]} | {ev} |"
        )
    if not high:
        lines.append("| *(none)* | | | | | | | | |")

    lines.extend(["", "---", "## MEDIUM Confidence", "",
        "| Mac Symbol | Mac Sz | GC Object | GC Sz | GC Sec | Score | Xref Funcs | Xref Types | Evidence |",
        "|------------|--------|-----------|-------|--------|-------|------------|------------|----------|",
    ])

    for v in med:
        mac = v['mac']
        gc = v['gc']
        ev = "; ".join(v['ev'][:4])
        xf = ", ".join(v['xref_funcs'][:4])
        xt = ", ".join(v['xref_types'])
        ms = f"~{mac['size']}" if mac['size'] else "?"
        lines.append(
            f"| `{mac['name'].lstrip('_')[:45]}` | {ms} | {gc['name']} | "
            f"0x{gc['size']:X} | {gc['section']} | {v['score']} | "
            f"{xf[:60]} | {xt[:40]} | {ev} |"
        )
    if not med:
        lines.append("| *(none)* | | | | | | | | |")

    lines.extend(["", "---", "## LOW Confidence", "",
        "| Mac Symbol | Mac Sz | GC Object | GC Sz | GC Sec | Score | Xref Funcs | Evidence |",
        "|------------|--------|-----------|-------|--------|-------|------------|----------|",
    ])

    for v in low[:60]:
        mac = v['mac']
        gc = v['gc']
        ev = "; ".join(v['ev'][:3])
        xf = ", ".join(v['xref_funcs'][:3])
        ms = f"~{mac['size']}" if mac['size'] else "?"
        lines.append(
            f"| `{mac['name'].lstrip('_')[:45]}` | {ms} | {gc['name']} | "
            f"0x{gc['size']:X} | {gc['section']} | {v['score']} | "
            f"{xf[:60]} | {ev} |"
        )
    if not low:
        lines.append("| *(none)* | | | | | | | | |")

    lines.extend(["", "---", "## REJECT", "",
        "| GC Object | GC Sz | Mac Candidate | Score | Evidence / Reason |",
        "|-----------|-------|---------------|-------|-------------------|",
    ])

    for v in reject[:60]:
        mac = v['mac']
        gc = v['gc']
        reason = v.get('rejected_reason', '')
        ev = "; ".join(v['ev'][:2])
        reason_text = reason if reason else ev
        lines.append(
            f"| {gc['name']} | 0x{gc['size']:X} | "
            f"`{mac['name'].lstrip('_')[:40]}` | {v['score']} | {reason_text} |"
        )
    if not reject:
        lines.append("| *(none)* | | | | |")

    # GC objects with xrefs but no viable Mac candidate
    lines.extend(["", "---", "## GC Data With Xrefs But No Viable Mac Candidate", "",
        "| GC Object | GC Sz | GC Sec | Xref Funcs | Xref Types |",
        "|-----------|-------|--------|------------|------------|",
    ])

    verified_addrs = {v['gc']['address'] for v in verified}
    for addr in sorted(lbl_xrefs.keys()):
        if addr not in verified_addrs:
            xs = lbl_xrefs[addr]
            lbl_s = xs[0]['lbl_name']
            funcs_set = list(set(x['func_name'] for x in xs))[:5]
            types_set = list(set(x['op_type'] for x in xs))
            gc_s = next((s for s in lbl_syms if s['address'] == addr), None)
            if gc_s:
                lines.append(
                    f"| {lbl_s} | 0x{gc_s['size']:X} | {gc_s['section']} | "
                    f"{', '.join(funcs_set)} | {', '.join(types_set)} |"
                )

    lines.extend(["", "---", "## Stats", "",
        f"| Metric | Value |",
        "|--------|-------|",
        f"| GC lbl_* objects | {len(lbl_syms)} |",
        f"| With any xref | {len(lbl_xrefs)} |",
        f"| Absolute lis+addi/load/store xrefs | {len(abs_xrefs)} |",
        f"| SDA (r2) xrefs | {len(sda_xrefs)} |",
        f"| SDA2 (r13) xrefs | {len(sda2_xrefs)} |",
        f"| Total xrefs | {len(all_xrefs)} |",
        f"| Mac symbols with candidate | {len(verified)} |",
        f"| HIGH | {len(high)} |",
        f"| MEDIUM | {len(med)} |",
        f"| LOW | {len(low)} |",
        f"| REJECT | {len(reject)} |",
    ])

    md_path = OUT_DIR / "mac_data_to_gc_verified.md"
    md_path.write_text("\n".join(lines) + "\n")
    print(f"  Wrote {md_path}", flush=True)
    print(f"\n{'=' * 60}", flush=True)
    print(f"Done.", flush=True)


if __name__ == "__main__":
    main()
