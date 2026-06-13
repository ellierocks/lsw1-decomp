#!/usr/bin/env python3
"""
Function fingerprint matching: match unnamed GC functions to Mac prototype
functions by call-graph structure, size, and callee identity.

Uses the dispatch table renames as ground truth anchors.

Outputs:
  docs/symbol_donors/fingerprint_matches.md
"""

import re
import struct
import subprocess
from collections import defaultdict, Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GC_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
MAC_DEMO = ROOT / "orig/mac-debug-symbols/LEGO Star Wars Demo"
MAC_LSW2 = ROOT / "orig/mac-debug-symbols/LEGO Star Wars II"
OUT_DIR = ROOT / "docs/symbol_donors"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s*//\s*(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"size:0x([0-9A-Fa-f]+)")

TEXT_START = 0x800034A0
TEXT_END = 0x8018CB00

# Mac binaries to analyze
MAC_BINS = [
    ("LSW1_Demo", MAC_DEMO),
    ("LSW2_PPC", MAC_LSW2),
]


def read_dol():
    return open(GC_DOL, "rb").read()


def parse_gc_symbols():
    funcs = {}
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
        if sec == ".text" and not name.startswith("lbl_"):
            sm = SIZE_RE.search(meta)
            size = int(sm.group(1), 16) if sm else 0
            funcs[addr] = {"name": name, "size": size}
    return funcs


def resolve_func(addr, funcs):
    for f_addr in sorted(funcs.keys(), reverse=True):
        if f_addr <= addr:
            off = addr - f_addr
            if off < 0x20000:
                return funcs[f_addr]["name"]
            break
    return f"fn_{addr:08X}"


def extract_gc_call_graph(data, funcs):
    """Extract call graph from GC .text."""
    text_off = 0x000004A0
    text_size = 0x00189660
    text_data = data[text_off:text_off + text_size]

    callers = defaultdict(set)
    callees = defaultdict(set)

    for off in range(0, len(text_data) - 4, 4):
        inst = struct.unpack_from(">I", text_data, off)[0]
        if (inst >> 26) != 18:  # bl opcode
            continue
        ins_addr = TEXT_START + off
        li = inst & 0x03FFFFFF
        if li & 0x02000000:
            li |= ~0x03FFFFFF
        target = ins_addr + li * 4

        caller = resolve_func(ins_addr, funcs)
        callee = resolve_func(target, funcs)

        callers[callee].add(caller)
        callees[caller].add(callee)

    return callers, callees


def demangle_mac(raw):
    """Demangle Mac Metrowerks C++ name."""
    name = raw
    if name.startswith("_"):
        name = name[1:]
    m = re.match(r"_Z(\d+)([A-Za-z_]\w*)", name)
    if m:
        length = int(m.group(1))
        extracted = m.group(2)
        if len(extracted) >= length:
            return extracted[:length]
    m2 = re.match(r"Z(\d+)([A-Za-z_]\w*)", name)
    if m2:
        length = int(m2.group(1))
        extracted = m2.group(2)
        if len(extracted) >= length:
            return extracted[:length]
    return raw


def find_macho_text_section(data):
    """Parse Mach-O header to find __TEXT,__text section.
    
    Handles 32-bit and 64-bit Mach-O, and universal (fat) binaries.
    Returns (vmaddr, file_offset, size) or None.
    """
    def _parse_single(buf):
        if len(buf) < 28:
            return None
        magic = struct.unpack_from(">I", buf, 0)[0]
        is_64 = (magic == 0xFEEDFACF)
        if magic not in (0xFEEDFACE, 0xFEEDFACF):
            return None

        header_size = 32 if is_64 else 28
        ncmds = struct.unpack_from(">I", buf, 16)[0]
        pos = header_size

        for _ in range(ncmds):
            if pos + 8 > len(buf):
                break
            cmd = struct.unpack_from(">I", buf, pos)[0]
            cmdsize = struct.unpack_from(">I", buf, pos + 4)[0]
            if pos + cmdsize > len(buf):
                break

            LC_SEGMENT = 0x19 if is_64 else 0x1
            if cmd == LC_SEGMENT:
                segname = buf[pos + 8:pos + 24].rstrip(b"\x00").decode("latin-1", errors="replace")
                if segname != "__TEXT":
                    pos += cmdsize
                    continue

                if is_64:
                    nsects = struct.unpack_from(">I", buf, pos + 72)[0]
                    sect_arr_off = pos + 76
                    sec_size = 80
                else:
                    nsects = struct.unpack_from(">I", buf, pos + 48)[0]
                    sect_arr_off = pos + 56
                    sec_size = 68

                for si in range(nsects):
                    so = sect_arr_off + si * sec_size
                    if so + 32 > len(buf):
                        break
                    sn = buf[so:so + 16].rstrip(b"\x00").decode("latin-1", errors="replace")
                    if sn != "__text":
                        continue
                    # section struct: sectname[16] + segname[16] + addr + size + offset + ...
                    if is_64:
                        svma = struct.unpack_from(">Q", buf, so + 32)[0]
                        ssz = struct.unpack_from(">Q", buf, so + 40)[0]
                        sfo = struct.unpack_from(">I", buf, so + 48)[0]
                    else:
                        svma = struct.unpack_from(">I", buf, so + 32)[0]
                        ssz = struct.unpack_from(">I", buf, so + 36)[0]
                        sfo = struct.unpack_from(">I", buf, so + 40)[0]
                    return (svma, sfo, ssz)
            pos += cmdsize
        return None

    # Check for fat binary
    magic = struct.unpack_from(">I", data, 0)[0]
    if magic == 0xCAFEBABE:
        narch = struct.unpack_from(">I", data, 4)[0]
        for i in range(narch):
            ent_off = 8 + i * 20
            cpu = struct.unpack_from(">I", data, ent_off)[0]
            if cpu == 18:  # CPU_TYPE_POWERPC
                slice_off = struct.unpack_from(">I", data, ent_off + 4)[0]
                slice_size = struct.unpack_from(">I", data, ent_off + 8)[0]
                return _parse_single(data[slice_off:slice_off + slice_size])
        return None

    return _parse_single(data)


def extract_mac_call_graph(bin_path):
    """Extract function names and call graph from Mac PPC binary."""
    funcs = {}
    try:
        res = subprocess.run(
            ["llvm-nm", "-a", str(bin_path)],
            capture_output=True, text=True, timeout=60
        )
        for line in res.stdout.splitlines():
            parts = line.split()
            if len(parts) < 3:
                continue
            if parts[1] not in ("T", "t"):
                continue
            try:
                addr = int(parts[0], 16)
            except ValueError:
                continue
            name = demangle_mac(parts[2])
            funcs[addr] = name
    except Exception as e:
        print(f"    Warning: could not load Mac binary: {e}")
        return {}, {}, {}

    if not funcs:
        return {}, {}, {}

    try:
        mac_data = open(bin_path, "rb").read()
    except Exception:
        return funcs, {}, {}

    # Parse Mach-O to get __text section mapping
    text_info = find_macho_text_section(mac_data)
    if text_info is None:
        print(f"    Warning: could not find __text section in Mach-O")
        return funcs, {}, {}

    text_vmaddr, text_fileoff, text_size = text_info
    print(f"    __text: vmaddr=0x{text_vmaddr:x} fileoff=0x{text_fileoff:x} size=0x{text_size:x}")

    mac_callers = defaultdict(set)
    mac_callees = defaultdict(set)
    text_data = mac_data[text_fileoff:text_fileoff + text_size]

    for off in range(0, len(text_data) - 4, 4):
        inst = struct.unpack_from(">I", text_data, off)[0]
        if (inst >> 26) != 18:  # bl
            continue
        li = inst & 0x03FFFFFF
        if li & 0x02000000:
            li |= ~0x03FFFFFF

        ins_addr = text_vmaddr + off
        target = ins_addr + li * 4

        caller = funcs.get(ins_addr) or resolve_mac_func(ins_addr, funcs)
        callee = funcs.get(target) or resolve_mac_func(target, funcs)

        if caller and callee:
            mac_callers[callee].add(caller)
            mac_callees[caller].add(callee)

    return funcs, mac_callers, mac_callees


def resolve_mac_func(addr, funcs):
    """Find function name for an address in Mac binary."""
    for f_addr in sorted(funcs.keys(), reverse=True):
        if f_addr <= addr:
            off = addr - f_addr
            if off < 0x10000:
                return funcs[f_addr]
            break
    return None


def compute_gc_fingerprint(name, size, callees_set, callers_set, all_funcs, gc_callees):
    """Compute a structural fingerprint for a GC function."""
    named_callees = {c for c in callees_set if not c.startswith("fn_")}
    named_callers = {c for c in callers_set if not c.startswith("fn_")}

    return {
        "name": name,
        "size": size,
        "call_count": len(callees_set),
        "named_callees": named_callees,
        "named_callers": named_callers,
        "all_callees": callees_set,
    }


def compute_mac_fingerprint(name, callees_set, callers_set, all_mac_funcs):
    """Compute a structural fingerprint for a Mac function."""
    return {
        "name": name,
        "callees": callees_set,
        "callers": callers_set,
    }


def score_match(gc_fp, mac_fp):
    """Score how well a GC function matches a Mac function."""
    gc_named_callees = gc_fp["named_callees"]
    mac_callees = mac_fp["callees"]

    if not gc_named_callees or not mac_callees:
        return 0, []

    # Look for callee name overlap (Action_* / Condition_* matching)
    # For each GC named callee, check if similar name exists in Mac callees
    overlap = set()
    for gc_name in gc_named_callees:
        # Extract base: "Action_Idle" -> "Idle"
        base = gc_name
        if gc_name.startswith("Action_"):
            base = gc_name[7:]
        elif gc_name.startswith("Condition_"):
            base = gc_name[10:]

        for mac_name in mac_callees:
            mac_base = demangle_mac(mac_name)
            # Remove common prefix
            for prefix in ("Action_", "Condition_", "_Action_", "_Condition_"):
                if mac_base.startswith(prefix):
                    mac_base = mac_base[len(prefix):]
                    break
            # Check if bases match
            if base.lower() == mac_base.lower():
                overlap.add(gc_name)
                break
            elif base.lower() in mac_base.lower() or mac_base.lower() in base.lower():
                if len(base) > 3 and len(mac_base) > 3:
                    overlap.add(f"{gc_name}~{mac_base}")

    if overlap:
        # Score: fraction of GC callees matched
        score = len([o for o in overlap if "~" not in o])  # exact matches only
        score += 0.5 * len([o for o in overlap if "~" in o])  # partial matches
        return score, overlap

    return 0, set()


def main():
    print("=" * 60)
    print("Function Fingerprint Matcher")
    print("=" * 60)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    # Load GC data
    data = read_dol()
    gc_funcs = parse_gc_symbols()
    print(f"[1] GC: {len(gc_funcs)} functions")

    gc_callers, gc_callees = extract_gc_call_graph(data, gc_funcs)
    print(f"    Call graph: {len(gc_callers)} callees, {len(gc_callees)} callers")

    # Separate named and unnamed
    gc_named = {addr: info for addr, info in gc_funcs.items()
                if not info["name"].startswith("fn_")}
    gc_unnamed = {addr: info for addr, info in gc_funcs.items()
                  if info["name"].startswith("fn_")}
    print(f"    Named: {len(gc_named)}, Unnamed: {len(gc_unnamed)}")

    # Compute fingerprints for unnamed GC functions
    gc_fingerprints = {}
    for addr, info in gc_unnamed.items():
        name = info["name"]
        gc_fingerprints[name] = compute_gc_fingerprint(
            name, info["size"],
            gc_callees.get(name, set()),
            gc_callers.get(name, set()),
            gc_funcs, gc_callees
        )

    # Load Mac function data
    print(f"[2] Loading Mac prototype binaries...")
    mac_all_funcs = {}
    mac_all_callers = defaultdict(set)
    mac_all_callees = defaultdict(set)

    for label, bin_path in MAC_BINS:
        funcs, callers, callees = extract_mac_call_graph(bin_path)
        print(f"    {label}: {len(funcs)} functions, {len(callees)} with callees")
        mac_all_funcs.update(funcs)
        for k, v in callers.items():
            mac_all_callers[k].update(v)
        for k, v in callees.items():
            mac_all_callees[k].update(v)

    # Compute Mac fingerprints (only for Action_*/Condition_* functions)
    mac_fingerprints = {}
    for name in mac_all_funcs.values():
        if name.startswith("Action_") or name.startswith("Condition_"):
            mac_fingerprints[name] = compute_mac_fingerprint(
                name,
                mac_all_callees.get(name, set()),
                mac_all_callers.get(name, set()),
                mac_all_funcs
            )

    print(f"    Mac Action_/Condition_ with call data: {len(mac_fingerprints)}")

    # Match
    print(f"[3] Matching unnamed GC functions to Mac functions...")
    matches = []
    for gc_name, gc_fp in gc_fingerprints.items():
        if not gc_fp["named_callees"]:
            continue
        for mac_name, mac_fp in mac_fingerprints.items():
            score, overlap = score_match(gc_fp, mac_fp)
            if score > 0:
                matches.append({
                    "gc_name": gc_name,
                    "gc_size": gc_fp["size"],
                    "mac_name": mac_name,
                    "score": score,
                    "overlap": overlap,
                    "gc_callees": gc_fp["named_callees"],
                    "mac_callees": mac_fp["callees"],
                })

    # Deduplicate: each GC function matched to best Mac
    best_match = {}
    for m in sorted(matches, key=lambda x: -x["score"]):
        if m["gc_name"] not in best_match:
            best_match[m["gc_name"]] = m

    print(f"    {len(matches)} raw matches, {len(best_match)} best matches")

    # --- Write report ---
    lines = [
        "# Function Fingerprint Matching Results",
        "",
        f"Matched {len(best_match)} unnamed GC functions to Mac Action_/Condition_ functions",
        "using callee-set overlap.",
        "",
        "**Method:** For each unnamed GC function, compute the set of named functions it calls.",
        "Compare against each Mac Action_/Condition_ function's callee set. Score by string",
        "overlap of callee names (after stripping Action_/Condition_ prefix).",
        "",
        "---",
        "## Best Matches (score >= 1)",
        "",
        "| GC Func | Size | Mac Match | Score | GC Callee Overlap | Mac Callees |",
        "|---------|------|-----------|-------|-------------------|-------------|",
    ]

    for m in sorted(best_match.values(), key=lambda x: -x["score"])[:50]:
        ol = ", ".join(list(m["overlap"])[:5])
        mc = ", ".join(list(m["mac_callees"])[:5])
        lines.append(
            f"| {m['gc_name']} | 0x{m['gc_size']:X} | "
            f"`{m['mac_name'][:40]}` | {m['score']} | "
            f"{ol[:60]} | {mc[:60]} |"
        )

    if not best_match:
        lines.append("| *(no matches)* | | | | | |")

    lines.extend(["", "---", "## All Matches", ""])
    for m in sorted(best_match.values(), key=lambda x: -x["score"]):
        ol = "; ".join(list(m["overlap"])[:5])
        mc = "; ".join(list(m["mac_callees"])[:5])
        lines.append(f"- {m['gc_name']} → `{m['mac_name']}` (score={m['score']}, overlap={ol})")

    lines.extend(["", "---", "## Stats", "",
        "| Metric | Value |",
        "|--------|-------|",
        f"| GC unnamed functions | {len(gc_unnamed)} |",
        f"| GC named functions | {len(gc_named)} |",
        f"| GC unnamed with named callees | {len(gc_fingerprints)} |",
        f"| Mac Action_/Condition_ functions | {len(mac_fingerprints)} |",
        f"| Raw matches | {len(matches)} |",
        f"| Best matches | {len(best_match)} |",
    ])

    report_path = OUT_DIR / "fingerprint_matches.md"
    report_path.write_text("\n".join(lines) + "\n")
    print(f"    Wrote {report_path}")

    print(f"\n{'=' * 60}")
    print("Done.")


if __name__ == "__main__":
    main()
