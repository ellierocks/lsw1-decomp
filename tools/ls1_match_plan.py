#!/usr/bin/env python3
"""Show remaining unnamed functions in a module with struct evidence and Mac
cross-reference suggestions.

Usage:
    python tools/ls1_match_plan.py nuanim
    python tools/ls1_match_plan.py nufile --show-all
    python tools/ls1_match_plan.py 0x80016A00 0x80024000 --top 5
    python tools/ls1_match_plan.py nuanim --top 10
"""
import argparse
import csv
import struct
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOL = ROOT / "orig/GL5E4F/sys/main.dol"
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"
MAC_TSVS = [
    ROOT / "docs/symbol_donors/mac_lsw1_demo_symbols.tsv",
    ROOT / "docs/symbol_donors/mac_lsw2_symbols.tsv",
    ROOT / "docs/symbol_donors/mac_batman_symbols.tsv",
    ROOT / "docs/symbol_donors/mac_indy_symbols.tsv",
]

TEXT_START = 0x800034A0
TEXT_OFF = 0x4A0

MODULES = {
    "nucore": (0x800034A0, 0x80008000),
    "nucore_file": (0x800034A0, 0x80006000),
    "nufile": (0x800034A0, 0x80006000),
    "nucore_numem": (0x80006F74, 0x80007468),
    "numem": (0x80006F74, 0x80007468),
    "nucore_error": (0x80007468, 0x80008000),
    "error": (0x80007468, 0x80008000),
    "numath": (0x80008000, 0x80012000),
    "nu3dx": (0x80012000, 0x80090000),
    "nuanim": (0x80016A00, 0x80024000),
    "anim": (0x80016A00, 0x80024000),
    "render": (0x80024000, 0x8005C000),
    "scene": (0x8005C000, 0x80090000),
    "nusound": (0x80090000, 0x800B0000),
    "sound": (0x80090000, 0x800B0000),
    "gamelib": (0x800B0000, 0x80100000),
    "gamecode": (0x80100000, 0x8018CB00),
    "all": (0x800034A0, 0x8018CB00),
}


def load_symtab():
    funcs = []
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
            funcs.append((addr, name, size))
    funcs.sort()
    return funcs


def load_mac_symbols():
    syms = defaultdict(list)
    for tsv in MAC_TSVS:
        if not tsv.exists():
            continue
        source = tsv.stem.replace("mac_", "").replace("_symbols", "")
        with open(tsv) as f:
            for row in csv.reader(f, delimiter="\t"):
                if len(row) < 8:
                    continue
                stype = row[4].strip()
                if stype != "text":
                    continue
                clean = row[7].strip()
                dem = row[8].strip() if len(row) > 8 else ""
                syms[clean].append((source, dem))
    return syms


def analyze_struct(dol_data, addr, size):
    end = min(size, 0x100)
    data = dol_data[TEXT_OFF + (addr - TEXT_START):TEXT_OFF + (addr - TEXT_START) + end]
    float_regs, word_regs, half_regs, bl_targets = {}, {}, {}, []
    pos = 0
    while pos + 4 <= len(data):
        inst = struct.unpack_from(">I", data, pos)[0]
        op = inst >> 26
        paddr = addr + pos
        if op in (48, 52):
            rs, ra, d = (inst >> 21) & 0x1F, (inst >> 16) & 0x1F, inst & 0xFFFF
            if d >= 0x8000:
                d -= 0x10000
            if ra:
                float_regs.setdefault(ra, set()).add(d)
        elif op in (32, 36):
            r1, ra, d = (inst >> 21) & 0x1F, (inst >> 16) & 0x1F, inst & 0xFFFF
            if d >= 0x8000:
                d -= 0x10000
            if ra:
                word_regs.setdefault(ra, set()).add(d)
        elif op == 40:
            rd, ra, d = (inst >> 21) & 0x1F, (inst >> 16) & 0x1F, inst & 0xFFFF
            if d >= 0x8000:
                d -= 0x10000
            if ra:
                half_regs.setdefault(ra, set()).add(d)
        elif op == 18:
            li = inst & 0x3FFFFFF
            if li & 0x2000000:
                li |= ~0x3FFFFFF
            bl_targets.append(paddr + li * 4)
        pos += 4

    patterns = []
    for reg, offs in float_regs.items():
        kos = [o for o in offs if isinstance(o, int) and o >= 0]
        if all(o in kos for o in (0, 4, 8, 12)):
            patterns.append(f"KEY(r{reg})")
        if 0 in kos and 4 in kos and 8 in kos:
            patterns.append("VEC3")
    for reg, offs in word_regs.items():
        kos = [o for o in offs if isinstance(o, int) and 0 <= o <= 0x64]
        if all(o in kos for o in (0, 4, 8, 12)):
            patterns.append(f"CURVE(r{reg})")
        if len(kos) >= 5:
            patterns.append(f"PTR(r{reg}:0x{min(kos):X}-0x{max(kos):X})")
    for reg, offs in half_regs.items():
        if 4 in offs or 6 in offs:
            patterns.append(f"DAT2(r{reg})")
    return patterns, bl_targets


def main():
    parser = argparse.ArgumentParser(
        description="Show remaining unnamed functions with struct evidence and Mac hints."
    )
    parser.add_argument("target", nargs="+", help="Module name or start end addresses")
    parser.add_argument("--show-all", action="store_true", help="Show all unnamed functions (no limit)")
    parser.add_argument("--top", type=int, default=20, help="Show top N candidates (default: 20)")
    args = parser.parse_args()

    show_all = args.show_all
    top_n = args.top
    raw_args = args.target

    if len(raw_args) == 1 and raw_args[0].lower() in MODULES:
        raw = raw_args[0].lower()
        start, end = MODULES[raw]
    elif len(raw_args) == 2 and raw_args[0].startswith("0x"):
        start = int(raw_args[0], 16)
        end = int(raw_args[1], 16)
        raw = f"0x{start:08X}-0x{end:08X}"
    else:
        print(f"Unknown module: {' '.join(raw_args)}")
        print(f"Modules: {', '.join(sorted(MODULES))}")
        sys.exit(1)

    with open(DOL, "rb") as f:
        dol_data = f.read()
    funcs = load_symtab()
    mac_syms = load_mac_symbols()

    named_by_addr = {a: n for a, n, s in funcs if not n.startswith("fn_")}
    funcs_in_range = [f for f in funcs if start <= f[0] < end]
    unnamed = [f for f in funcs_in_range if f[1].startswith("fn_") and f[2] > 0]
    named_range = [f for f in funcs_in_range if not f[1].startswith("fn_")]

    print(f"Module: {raw}")
    print(f"Named: {len(named_range)}, Unnamed: {len(unnamed)}, Total: {len(funcs_in_range)}")
    print()

    # Build call graph cache: for each named function, what unnamed does it call?
    callers_of_unnamed = defaultdict(list)
    for nf in named_range:
        _, bls = analyze_struct(dol_data, nf[0], nf[2])
        for t in bls:
            if t in {u[0] for u in unnamed}:
                callers_of_unnamed[t].append(nf[1])

    # Build neighbor context
    named_addrs = sorted(named_by_addr.keys())

    def prev_named(addr):
        before = [a for a in named_addrs if a < addr]
        return named_by_addr.get(before[-1]) if before else None

    def next_named(addr):
        after = [a for a in named_addrs if a > addr]
        return named_by_addr.get(after[0]) if after else None

    # Score and display candidates
    scored = []
    for addr, name, size in unnamed:
        patterns, bls = analyze_struct(dol_data, addr, size)
        called_named = {named_by_addr[t] for t in bls if t in named_by_addr}
        callers = callers_of_unnamed.get(addr, [])
        key_score = sum(1 for p in patterns if p.startswith(("KEY", "CURVE", "DAT2")))

        prev_n = prev_named(addr)
        next_n = next_named(addr)
        mac_hints = []
        for cname, entries in mac_syms.items():
            for src, dem in entries:
                if prev_n and prev_n in cname:
                    mac_hints.append(cname)
                elif next_n and next_n in cname:
                    mac_hints.append(cname)
        mac_hints = list(set(mac_hints))[:3]

        scored.append((
            key_score, len(patterns), addr, name, size, patterns, called_named,
            callers, prev_n, next_n, mac_hints
        ))

    scored.sort(key=lambda x: (-x[0], -x[1], x[2]))

    top = len(scored) if show_all else min(top_n, len(scored))
    for item in scored[:top]:
        ks, pc, addr, name, size, patterns, called_named, callers, prev_n, next_n, mac_hints = item
        pat_str = " | ".join(set(patterns[:4])) if patterns else "(no struct pattern)"
        has_struct = any(p.startswith(("KEY", "CURVE", "DAT2")) for p in patterns)
        tag = "★ STRUCT MATCH" if has_struct else ""
        print(f"  0x{addr:08X}  {name:30s}  size=0x{size:X}  {tag}")
        print(f"    Struct: {pat_str}")
        if callers:
            print(f"    Called from: {', '.join(callers[:4])}")
        if called_named:
            print(f"    Calls: {', '.join(sorted(called_named)[:4])}")
        if prev_n or next_n:
            print(f"    Neighbors: {prev_n or '(start)'} → ... → {next_n or '(end)'}")
        if mac_hints:
            print(f"    Mac hints: {', '.join(mac_hints)}")
        print()

    # Pattern summary
    print("## Pattern summary\n")
    pc = defaultdict(int)
    for _, _, _, _, _, patterns, _, _, _, _, _ in scored:
        for p in set(patterns):
            pc[p.split("(")[0]] += 1
    print(f"{'Pattern':<16} {'Count':>6}")
    print("-" * 24)
    for p, c in sorted(pc.items(), key=lambda x: -x[1]):
        print(f"{p:<16} {c:>6}")
    print()

    # Mac hint summary
    all_hints = defaultdict(int)
    for item in scored:
        for h in item[10]:
            all_hints[h] += 1
    if all_hints:
        print("## Mac symbol hints in range\n")
        for h, c in sorted(all_hints.items(), key=lambda x: -x[1])[:10]:
            print(f"  {h} (appears {c}x)")
        print()


if __name__ == "__main__":
    main()
