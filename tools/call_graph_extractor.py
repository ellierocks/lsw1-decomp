#!/usr/bin/env python3
"""
Extract call graph from GC DOL .text section and suggest names for unnamed
functions based on call context from known named functions.

Outputs:
  docs/symbol_donors/call_graph.tsv  — full call graph
  docs/symbol_donors/call_graph_rename_suggestions.md — naming suggestions
"""

import re
import struct
from collections import defaultdict, Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GC_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
OUT_DIR = ROOT / "docs/symbol_donors"

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s*//\s*(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"size:0x([0-9A-Fa-f]+)")

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


def read_dol():
    return open(GC_DOL, "rb").read()


def parse_symbols():
    """Load all symbols, separate functions by address."""
    funcs = {}  # addr -> {name, size}
    lbls = {}   # addr -> name
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
        if sec == ".text":
            if not name.startswith("lbl_"):
                sm = SIZE_RE.search(meta)
                size = int(sm.group(1), 16) if sm else 0
                funcs[addr] = {"name": name, "size": size}
        elif not name.startswith("fn_") and sec in (".data", ".rodata", ".bss", ".sbss", ".sdata", ".sdata2"):
            lbls[addr] = name
    return funcs, lbls


def resolve_func(addr, funcs):
    """Find function name for an address (may be inside a function)."""
    for f_addr in sorted(funcs.keys(), reverse=True):
        if f_addr <= addr:
            off = addr - f_addr
            if off < 0x20000:  # max plausible function size
                return funcs[f_addr]["name"], off
            break
    return f"fn_{addr:08X}", 0


def find_func_by_name(name, funcs):
    for addr, info in funcs.items():
        if info["name"] == name:
            return addr
    return None


def is_bl(inst):
    """Check if instruction is a `bl` (branch and link)."""
    opcode = (inst >> 26) & 0x3F
    return opcode == 18


def extract_bl_target(inst, ins_addr):
    """Extract target address from a `bl` instruction."""
    li = inst & 0x03FFFFFF
    if li & 0x02000000:
        li |= ~0x03FFFFFF  # sign extend
    target = ins_addr + li * 4
    return target


def is_b(inst):
    """Check if instruction is an unconditional branch `b`."""
    opcode = (inst >> 26) & 0x3F
    return opcode == 16 and ((inst >> 1) & 0x3) == 0  # B-form, AA=0, LK=0


def extract_b_target(inst, ins_addr):
    """Extract target from a branch instruction."""
    li = inst & 0x03FFFFFF
    if li & 0x02000000:
        li |= ~0x03FFFFFF
    return ins_addr + li * 4


def main():
    print("=" * 60)
    print("Call Graph Extractor")
    print("=" * 60)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    data = read_dol()
    funcs, lbls = parse_symbols()
    print(f"[1] Loaded {len(funcs)} functions")

    # Build address->name lookup for named functions
    named_funcs = {addr: info for addr, info in funcs.items()
                   if not info["name"].startswith("fn_")}
    unnamed = {addr: info for addr, info in funcs.items()
               if info["name"].startswith("fn_")}
    print(f"    Named: {len(named_funcs)}, Unnamed: {len(unnamed)}")

    # Scan .text for bl instructions, build call graph
    text_base = TEXT_START
    text_off = DOL_SECTIONS[1][0]
    text_size = DOL_SECTIONS[1][2]
    text_data = data[text_off:text_off + text_size]

    callers_of = defaultdict(set)    # callee -> set of caller names
    callees_of = defaultdict(set)    # caller -> set of callee names
    bl_instructions = []             # (caller_name, caller_addr, callee_addr, callee_name)

    print(f"[2] Scanning {len(text_data):,} bytes of .text for bl instructions...")
    for off in range(0, len(text_data) - 4, 4):
        inst = struct.unpack_from(">I", text_data, off)[0]
        if not is_bl(inst):
            continue
        ins_addr = text_base + off
        target = extract_bl_target(inst, ins_addr)

        caller_name, _ = resolve_func(ins_addr, funcs)
        callee_name, _ = resolve_func(target, funcs)

        callers_of[callee_name].add(caller_name)
        callees_of[caller_name].add(callee_name)
        bl_instructions.append((caller_name, ins_addr, target, callee_name))

    print(f"    Found {len(bl_instructions)} bl instructions")
    print(f"    {len(callers_of)} unique callees, {len(callees_of)} unique callers")

    # --- Generate rename suggestions ---
    print("[3] Generating rename suggestions for unnamed functions...")

    suggestions = []
    for addr in sorted(unnamed.keys()):
        info = unnamed[addr]
        name = info["name"]
        callers = callers_of.get(name, set())
        callees = callees_of.get(name, set())

        named_callers = [c for c in callers if not c.startswith("fn_")]
        named_callees = [c for c in callees if not c.startswith("fn_")]

        if not named_callers and not named_callees:
            continue  # skip functions with no named context

        suggestions.append({
            "addr": addr,
            "name": name,
            "size": info["size"],
            "named_callers": named_callers[:10],
            "named_callees": named_callees[:10],
            "total_callers": len(callers),
            "total_callees": len(callees),
        })

    print(f"    {len(suggestions)} unnamed functions with named call context")

    # --- Write call graph TSV ---
    print("[4] Writing reports...")
    tsv_lines = ["# caller_name\tcaller_addr\tcallee_addr\tcallee_name"]
    for caller, ins_addr, target, callee in bl_instructions:
        tsv_lines.append(f"{caller}\t0x{ins_addr:08X}\t0x{target:08X}\t{callee}")
    tsv_path = OUT_DIR / "call_graph.tsv"
    tsv_path.write_text("\n".join(tsv_lines) + "\n")
    print(f"    Wrote {tsv_path} ({len(tsv_lines)} lines)")

    # --- Write suggestions report ---
    lines = [
        "# Call Graph Rename Suggestions",
        "",
        f"Extracted {len(bl_instructions)} call sites from {len(funcs)} functions.",
        f"{len(suggestions)} unnamed functions have named call context.",
        "",
        "---",
        "## Suggestions (by named callee count descending)",
        "",
        "| Function | Size | Named Callers | Named Callees | Suggested Context |",
        "|----------|------|---------------|---------------|-------------------|",
    ]

    # Score by most named context
    suggestions.sort(key=lambda s: -(len(s["named_callers"]) + len(s["named_callees"])))

    for s in suggestions[:100]:
        named_callers_str = ", ".join(s["named_callers"][:5])
        named_callees_str = ", ".join(s["named_callees"][:5])

        # Suggest context based on callers' subsystems
        all_named = s["named_callers"] + s["named_callees"]
        subs = Counter()
        for fn in all_named:
            if fn.startswith("Action_"): subs["AI_Actions"] += 1
            elif fn.startswith("Condition_"): subs["AI_Conditions"] += 1
            elif fn.startswith("Menu_"): subs["Menu"] += 1
            elif fn.startswith("Nu"): subs["Nu2"] += 1
            elif fn.startswith("AIScript"): subs["AIScript"] += 1
            elif "Script" in fn: subs["Script"] += 1
            elif "Anim" in fn: subs["Anim"] += 1
            elif "Player" in fn: subs["Player"] += 1
            elif "Pad" in fn or "Key" in fn: subs["Input"] += 1
            elif "Cam" in fn: subs["Camera"] += 1
        context = ", ".join(f"{c}({n})" for c, n in subs.most_common(3))

        lines.append(
            f"| {s['name']} | 0x{s['size']:X} | "
            f"{named_callers_str[:60]} | {named_callees_str[:60]} | "
            f"{context[:60]} |"
        )

    lines.extend(["", "---", "## Stats", "",
        "| Metric | Value |",
        "|--------|-------|",
        f"| Total functions | {len(funcs)} |",
        f"| Named | {len(named_funcs)} |",
        f"| Unnamed | {len(unnamed)} |",
        f"| Unnamed with named context | {len(suggestions)} |",
        f"| bl instructions | {len(bl_instructions)} |",
    ])

    report_path = OUT_DIR / "call_graph_rename_suggestions.md"
    report_path.write_text("\n".join(lines) + "\n")
    print(f"    Wrote {report_path}")

    # Also save a simpler: for each named function, list its unnamed callees
    print("[5] Writing unnamed-callee-per-named-function report...")
    named_callee_lines = ["# Named Functions -> Their Unnamed Callees", ""]
    for f_addr in sorted(named_funcs.keys()):
        f_info = named_funcs[f_addr]
        f_name = f_info["name"]
        unnamed_callees = [c for c in callees_of.get(f_name, set()) if c.startswith("fn_")]
        if not unnamed_callees:
            continue
        named_callee_lines.append(f"### {f_name}")
        for c in sorted(unnamed_callees):
            named_callee_lines.append(f"  - {c}")

    named_callee_lines.extend(["", "---", f"Total: {len(named_callee_lines) - 3} entries"])
    uc_path = OUT_DIR / "named_to_unnamed_callees.md"
    uc_path.write_text("\n".join(named_callee_lines) + "\n")
    print(f"    Wrote {uc_path}")

    print(f"\n{'=' * 60}")
    print("Done.")


if __name__ == "__main__":
    main()
