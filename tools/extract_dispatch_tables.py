#!/usr/bin/env python3
"""
Extract dispatch tables from GC DOL, cross-reference with Mac prototype names.

Dispatch tables map string names → handler functions (and optionally extra data).
These are used for AI conditions, AI actions, script commands, menu states, etc.

Outputs:
  docs/symbol_donors/dispatch_tables.md  — full report
  docs/symbol_donors/dispatch_rename_candidates.tsv  — actionable renames
"""

import re
import struct
import subprocess
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GC_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
OUT_DIR = ROOT / "docs/symbol_donors"
MAC_DEMO = ROOT / "orig/mac-debug-symbols/LEGO Star Wars Demo"
MAC_LSW2 = ROOT / "orig/mac-debug-symbols/LEGO Star Wars II"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s*//\s*(?P<meta>.*)$"
)

DOL_SECTIONS = [
    (0x00000100, 0x80003100, 0x000003A0),
    (0x000004A0, 0x800034A0, 0x00189660),
    (0x00189B00, 0x8018CB00, 0x00023B40),
    (0x001AD640, 0x801B0640, 0x0004CA20),
    (0x001FA060, 0x80407A40, 0x00002880),
    (0x001FC8E0, 0x8040B300, 0x00000A40),
]

TEXT_START = 0x800034A0
TEXT_END = 0x8018CB00

# Don't include zero addresses or obviously-bad pointers
POINTER_MIN = 0x80000000
POINTER_MAX = 0x80410000


def read_dol():
    data = open(GC_DOL, "rb").read()
    return data


def dol_addr_to_offset(data, vaddr):
    for foff, base, size in DOL_SECTIONS:
        if base <= vaddr < base + size:
            return foff + (vaddr - base)
    # BSS region: .bss starts at 0x80313150, .sbss at 0x8040A2C0
    offset_guess = vaddr - 0x80313150 + 0x1FAA60
    if 0 <= offset_guess < len(data):
        return offset_guess
    return None


def read_u32(data, off):
    if off is None or off + 4 > len(data):
        return None
    return struct.unpack_from(">I", data, off)[0]


def read_str(data, off):
    if off is None or off >= len(data):
        return ""
    end = off
    while end < len(data) and data[end] != 0:
        end += 1
    return data[off:end].decode("latin-1", errors="replace")


def parse_symbols():
    syms = {}
    funcs = {}
    for line in open(GC_SYMBOLS):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        m = SYMBOL_RE.match(line)
        if m:
            name = m.group("name")
            addr = int(m.group("addr"), 16)
            sec = m.group("section")
            meta = m.group("meta")
            syms[addr] = {"name": name, "section": sec, "meta": meta}
            if sec == ".text":
                funcs[addr] = name
    return syms, funcs


def resolve_func(addr, syms, funcs):
    if addr in syms:
        return syms[addr]["name"]
    # Find containing function
    for f_addr in sorted(funcs.keys(), reverse=True):
        if f_addr <= addr:
            off = addr - f_addr
            if off < 0x1000:  # reasonable max function size
                return funcs[f_addr]
            break
    return f"fn_{addr:08X}"


def extract_dispatch_table(data, addr, entry_size, syms, funcs, name, table_id):
    """Extract a dispatch table at addr with given entry size.
    
    Entry formats:
      8 bytes: string_ptr(4) + func_ptr(4)
      12 bytes: string_ptr(4) + func_ptr(4) + extra(4)
    """
    offset = dol_addr_to_offset(data, addr)
    if offset is None:
        return []

    entries = []
    for i in range(256):
        off = offset + i * entry_size
        if off + entry_size > len(data):
            break

        str_ptr = read_u32(data, off)
        func_ptr = read_u32(data, off + 4)

        # Validate pointers
        if str_ptr is None or func_ptr is None:
            break
        if str_ptr == 0 and func_ptr == 0:
            break
        if func_ptr == 0:
            break  # null terminator
        if not (POINTER_MIN <= str_ptr <= POINTER_MAX):
            break  # not a valid string pointer
        if not (TEXT_START <= func_ptr <= TEXT_END) and not (0x80003100 <= func_ptr < TEXT_START):
            break

        str_val = read_str(data, dol_addr_to_offset(data, str_ptr))
        func_name = resolve_func(func_ptr, syms, funcs)

        extra = None
        if entry_size >= 12:
            extra_ptr = read_u32(data, off + 8)
            extra_name = resolve_func(extra_ptr, syms, funcs) if extra_ptr and POINTER_MIN <= extra_ptr <= POINTER_MAX else ""
            extra = extra_name

        entries.append({
            "index": i,
            "string": str_val,
            "string_addr": str_ptr,
            "func_addr": func_ptr,
            "func_name": func_name,
            "extra": extra,
            "table_addr": addr,
            "table_name": name,
        })

    return entries


def demangle_mac(name):
    """Extract the readable function name from a Mac symbol.
    
    Handles:
      - __Z<len><name><params>  (Metrowerks C++ mangling)
      - __<name>  (C symbol with underscore prefix)
      - <name>    (plain name)
    """
    raw = name
    if raw.startswith("_"):
        raw = raw[1:]

    # Metrowerks mangling: __Z<digits><name>
    m = re.match(r'_Z(\d+)([A-Za-z_]\w*)', raw)
    if m:
        length = int(m.group(1))
        extracted = m.group(2)
        # The name after length prefix should match the claimed length
        if len(extracted) >= length:
            pure_name = extracted[:length]
            return pure_name

    # Also try without leading underscore
    m2 = re.match(r'Z(\d+)([A-Za-z_]\w*)', raw)
    if m2:
        length = int(m2.group(1))
        extracted = m2.group(2)
        if len(extracted) >= length:
            pure_name = extracted[:length]
            return pure_name

    # Plain name
    return raw


def load_mac_functions(binary_path):
    """Load all function symbols from a Mac binary."""
    mac_funcs = {}
    mac_by_demangled = {}
    try:
        res = subprocess.run(
            ["llvm-nm", "-a", str(binary_path)],
            capture_output=True, text=True, timeout=60
        )
        for line in res.stdout.splitlines():
            parts = line.split()
            if len(parts) < 3:
                continue
            stype = parts[1]
            if stype not in ("T", "t"):
                continue
            try:
                addr = int(parts[0], 16)
            except ValueError:
                continue
            raw = parts[2]
            clean = demangle_mac(raw)
            mac_funcs[raw] = addr
            mac_by_demangled[clean] = addr
            # Also store Action_* and Condition_* prefixed
            if clean.startswith("Action_") or clean.startswith("Condition_"):
                mac_by_demangled[clean] = addr
    except Exception as e:
        print(f"  Warning: could not load Mac symbols from {binary_path}: {e}")
    return mac_funcs, mac_by_demangled


def match_mac_action(action_name, mac_by_demangled):
    """Try to find a Mac function matching an action name."""
    candidates = []
    target = f"Action_{action_name}"

    # Substring match through all Action_* functions (prioritize exact match)
    best = None
    for name, addr in mac_by_demangled.items():
        if not name.startswith("Action_"):
            continue
        rest = name[7:]
        if action_name.lower() == rest.lower():
            best = name  # exact match, prefer this
        elif action_name.lower() in name.lower() and len(action_name) > 3 and best is None:
            best = f"{name} (substr)"

    if best:
        candidates.append(best)
    return candidates


def match_mac_condition(cond_name, mac_by_demangled):
    """Try to find a Mac function matching a condition name."""
    candidates = []
    target = f"Condition_{cond_name}"

    best = None
    for name, addr in mac_by_demangled.items():
        if not name.startswith("Condition_"):
            continue
        rest = name[10:]
        if cond_name.lower() == rest.lower():
            best = name
        elif cond_name.lower() in name.lower() and len(cond_name) > 3 and best is None:
            best = f"{name} (substr)"

    if best:
        candidates.append(best)
    return candidates


def main():
    print("=" * 60)
    print("Dispatch Table Extractor & Cross-Referencer")
    print("=" * 60)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    data = read_dol()
    syms, funcs = parse_symbols()
    print(f"[1] Loaded {len(funcs)} functions, {len(syms)} total symbols")

    # Load Mac function names for cross-reference
    mac_raw1, mac_dm1 = load_mac_functions(MAC_DEMO)
    mac_raw2, mac_dm2 = load_mac_functions(MAC_LSW2)
    # Merge: LSW1 Demo preferred, LSW2 as fallback
    mac_all = {**mac_dm2, **mac_dm1}
    print(f"[2] Loaded {len(mac_all)} Mac demangled function names (LSW1 Demo + LSW2)")

    # Define dispatch tables to extract
    tables = [
        (0x801BC040, 8, "Script_Keyword_Parser"),
        (0x801BC068, 8, "Script_Keyword_Parser2"),
        (0x801BC090, 12, "AI_Conditions_API"),
        (0x801BC288, 8, "AI_Actions_API"),
        (0x801E0D14, 12, "Level_Conditions"),
        (0x801E1158, 8, "Level_Actions"),
    ]

    print(f"[3] Extracting {len(tables)} dispatch tables...")
    all_entries = []
    for addr, entry_size, name in tables:
        entries = extract_dispatch_table(data, addr, entry_size, syms, funcs, name, len(all_entries))
        print(f"  {name} @ 0x{addr:08X}: {len(entries)} entries (entry_size={entry_size})")
        all_entries.extend(entries)

    print(f"\n  Total dispatch entries: {len(all_entries)}")

    # --- Mac cross-reference ---
    print("[4] Cross-referencing with Mac functions...")
    matched = 0
    for e in all_entries:
        s = e["string"]
        if not s:
            continue
        is_condition_table = "Condition" in e["table_name"]
        is_action_table = "Action" in e["table_name"]
        cond_cands = match_mac_condition(s, mac_all)
        action_cands = match_mac_action(s, mac_all)

        # Prioritize by table type: condition tables → condition funcs first
        if is_condition_table:
            e["mac_candidates"] = cond_cands + action_cands
        elif is_action_table:
            e["mac_candidates"] = action_cands + cond_cands
        else:
            e["mac_candidates"] = action_cands + cond_cands

        if e["mac_candidates"]:
            matched += 1
    print(f"  {matched} entries with Mac matches")

    # --- Build rename candidates ---
    print("[5] Building rename candidates...")
    rename_candidates = []
    for e in all_entries:
        func_addr = e["func_addr"]
        func_name = e["func_name"]
        is_unnamed = func_name.startswith("fn_")
        best_mac = ""
        if e["mac_candidates"]:
            for c in e["mac_candidates"]:
                best_mac = c
                break
        if is_unnamed and best_mac:
            # HIGH if best_mac is an exact name match (no substr qualifier)
            is_exact = "(substr)" not in best_mac
            rename_candidates.append({
                "table": e["table_name"],
                "index": e["index"],
                "string": e["string"],
                "func_addr": func_addr,
                "cur_name": func_name,
                "proposed_name": best_mac,
                "confidence": "HIGH" if is_exact else "MEDIUM",
            })

    print(f"  Found {len(rename_candidates)} rename candidates")

    # --- Write report ---
    print("[6] Writing reports...")

    lines = [
        "# GC Dispatch Table Recovery Report",
        "",
        f"Extracted {len(all_entries)} dispatch entries from {len(tables)} tables.",
        f"Cross-referenced with Mac prototype. Found {len(rename_candidates)} rename candidates.",
        "",
        "---",
        "",
        "## Rename Candidates (unnamed GC function → Mac name)",
        "",
        "| Table | Idx | String | Cur Func | Proposed Name | Confidence |",
        "|-------|-----|--------|----------|---------------|------------|",
    ]

    for rc in sorted(rename_candidates, key=lambda x: (-1 if x["confidence"] == "HIGH" else 0, x["table"], x["index"])):
        lines.append(
            f"| {rc['table']} | {rc['index']} | `{rc['string'][:30]}` | "
            f"{rc['cur_name']} | `{rc['proposed_name'][:40]}` | {rc['confidence']} |"
        )

    if not rename_candidates:
        lines.append("| *(none)* | | | | | |")

    lines.extend(["", "---", "## All Dispatch Tables", ""])

    current_table = None
    for e in all_entries:
        if e["table_name"] != current_table:
            current_table = e["table_name"]
            lines.append(f"### {current_table} @ 0x{e['table_addr']:08X}")
            lines.append("")
            lines.append("| # | String | Func Addr | Func Name | Extra | Mac Candidates |")
            lines.append("|---|--------|-----------|-----------|-------|----------------|")

        mac_str = "; ".join(list(set(e.get("mac_candidates", [])))[:3]) if e.get("mac_candidates") else ""
        extra_str = e.get("extra", "") or ""
        lines.append(
            f"| {e['index']} | `{e['string'][:40]}` | "
            f"0x{e['func_addr']:08X} | {e['func_name'][:45]} | "
            f"{extra_str[:30]} | {mac_str[:50]} |"
        )

    lines.extend(["", "---", "## Stats", "",
        "| Metric | Value |",
        "|--------|-------|",
        f"| Dispatch tables | {len(tables)} |",
        f"| Total entries | {len(all_entries)} |",
        f"| Rename candidates | {len(rename_candidates)} |",
        f"| Mac functions indexed | {len(mac_all)} |",
    ])

    report_path = OUT_DIR / "dispatch_tables.md"
    report_path.write_text("\n".join(lines) + "\n")
    print(f"  Wrote {report_path}")

    # TSV of rename candidates
    tsv_lines = [
        "# table\tindex\tstring\tfunc_addr\tcur_name\tproposed_name\tconfidence"
    ]
    for rc in sorted(rename_candidates, key=lambda x: (-1 if x["confidence"] == "HIGH" else 0, x["table"], x["index"])):
        tsv_lines.append(
            f"{rc['table']}\t{rc['index']}\t{rc['string']}\t"
            f"0x{rc['func_addr']:08X}\t{rc['cur_name']}\t{rc['proposed_name']}\t{rc['confidence']}"
        )
    tsv_path = OUT_DIR / "dispatch_rename_candidates.tsv"
    tsv_path.write_text("\n".join(tsv_lines) + "\n")
    print(f"  Wrote {tsv_path}")

    print(f"\n{'=' * 60}")
    print("Done.")


if __name__ == "__main__":
    main()
