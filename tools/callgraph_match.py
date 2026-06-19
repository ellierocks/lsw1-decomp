#!/usr/bin/env python3
"""Match unnamed GC functions to named donor functions via call-graph topology.

For each unnamed LSW1 fn_, looks at the set of NAMED functions it calls.
Then scans donor ELFs (gcnemo, crashwoc, crashwoc_proto) to find which
named donor function calls the same set of named functions.

This is robust to:
  - paired-single size differences (body hash changes, call pattern doesn't)
  - reordering within functions
  - inline expansions (donor may call more functions than GC version)

Requires at minimum 2 named callees in GC for a confident match.

Usage:
    python tools/callgraph_match.py                         # print candidates
    python tools/callgraph_match.py -o work/cg_batch.txt   # write batch
    python tools/callgraph_match.py --min-callees 3         # stricter
"""
from __future__ import annotations

import argparse
import struct
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CALL_GRAPH_TSV = ROOT / "docs/symbol_donors/call_graph.tsv"
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"

DONOR_ELFS = [
    ("gcnemo",
     ROOT / "orig/nu2/Disney-Pixar Finding Nemo (USA)/extracted/files/GCNemo.elf"),
    ("crashwoc_retail",
     ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/extracted/files/crashwoc.elf"),
    ("crashwoc_proto",
     ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/prototype/prototype/files/crashwoc.elf"),
]


def load_gc_callgraph(tsv: Path) -> tuple[dict[str, set[str]], dict[str, set[str]]]:
    """Returns (callee_map, caller_map): name → set of callee/caller names."""
    callee_map: dict[str, set[str]] = defaultdict(set)
    caller_map: dict[str, set[str]] = defaultdict(set)
    for line in tsv.read_text(errors="replace").splitlines()[1:]:
        parts = line.split("\t")
        if len(parts) < 4:
            continue
        caller, callee = parts[0].strip(), parts[3].strip()
        callee_map[caller].add(callee)
        caller_map[callee].add(caller)
    return dict(callee_map), dict(caller_map)


def extract_elf_callgraph(elf_path: Path) -> tuple[dict[str, set[str]], dict[str, set[str]]]:
    """Extract call graph from a GC/PPC ELF by scanning for bl instructions."""
    data = elf_path.read_bytes()
    e_shoff = struct.unpack_from(">I", data, 0x20)[0]
    e_shentsize = struct.unpack_from(">H", data, 0x2E)[0]
    n_sections = struct.unpack_from(">H", data, 0x30)[0]
    sections = [
        struct.unpack_from(">IIIIIIIIII", data, e_shoff + i * e_shentsize)
        for i in range(n_sections)
    ]

    # Find symbol table section (SHT_SYMTAB=2)
    sym_sec_idx = next((i for i, s in enumerate(sections) if s[1] == 2), None)
    if sym_sec_idx is None:
        return {}, {}
    sym_sec = sections[sym_sec_idx]
    str_sec = sections[sym_sec[6]]  # sh_link = associated string table
    str_off = str_sec[4]
    sym_off = sym_sec[4]
    sym_entsize = sym_sec[9] or 16
    sym_count = sym_sec[5] // sym_entsize
    str_off = sections[16][4]

    # Build addr → name mapping
    addr_to_name: dict[int, str] = {}
    funcs: list[tuple[int, int, str]] = []  # (addr, size, name)
    for i in range(sym_count):
        so = sym_off + i * sym_entsize
        st_name, addr, size = struct.unpack_from(">III", data, so)
        info = data[so + 12]
        if (info >> 4) != 1 or size == 0:
            continue
        nm_end = data.index(b"\x00", str_off + st_name)
        name = data[str_off + st_name:nm_end].decode("latin1", errors="replace")
        addr_to_name[addr] = name
        funcs.append((addr, size, name))

    # Find section names (shstrtab)
    e_shstrndx = struct.unpack_from(">H", data, 0x32)[0]
    sh_strtab = sections[e_shstrndx]
    sh_strtab_off = sh_strtab[4]

    def sec_name(sec: tuple) -> str:
        off = sec[0]
        end = data.index(b"\x00", sh_strtab_off + off)
        return data[sh_strtab_off + off:end].decode("latin1", errors="replace")

    # Code sections in GC ELFs: .text and .init (GC uses non-standard sh_flags)
    exec_names = {".text", ".init", ".sdata"}
    text_sections: list[tuple[int, int, int]] = []  # (vaddr, offset, size)
    for sec in sections:
        if sec[1] == 1 and sec[5] > 0 and sec_name(sec) in exec_names:  # by name
            text_sections.append((sec[3], sec[4], sec[5]))

    def read_vaddr(vaddr: int, length: int) -> bytes | None:
        for v, off, sz in text_sections:
            if v <= vaddr < v + sz:
                file_off = off + (vaddr - v)
                if file_off + length <= len(data):
                    return data[file_off: file_off + length]
        return None

    callee_map: dict[str, set[str]] = defaultdict(set)
    caller_map: dict[str, set[str]] = defaultdict(set)

    for addr, size, name in funcs:
        fn_bytes = read_vaddr(addr, size)
        if fn_bytes is None:
            continue
        for i in range(0, len(fn_bytes) - 3, 4):
            instr = struct.unpack_from(">I", fn_bytes, i)[0]
            # bl = opcode 18 (0x48), LK=1, AA=0  → bits 31-26=010010, bit 0=1, bit 1=0
            if (instr & 0xFC000003) == 0x48000001:
                # branch offset: bits 25-2, sign-extended
                raw = instr & 0x03FFFFFC
                if raw & 0x02000000:
                    raw |= 0xFC000000
                offset = struct.unpack(">i", struct.pack(">I", raw))[0]
                target = addr + i + offset
                target_name = addr_to_name.get(target)
                if target_name and target_name != name:
                    callee_map[name].add(target_name)
                    caller_map[target_name].add(name)

    return dict(callee_map), dict(caller_map)


def load_gc_syms() -> tuple[set[str], set[str]]:
    unnamed, named = set(), set()
    for line in SYMTAB.read_text(errors="replace").splitlines():
        if "type:function" not in line or "=" not in line:
            continue
        n = line.split("=")[0].strip()
        if n.startswith("fn_"):
            unnamed.add(n)
        else:
            named.add(n)
    return unnamed, named


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("-o", "--output", help="Write batch file here")
    ap.add_argument("--min-callees", type=int, default=2,
                    help="Min named callees required for match (default: 2)")
    ap.add_argument("--min-callers", type=int, default=0,
                    help="Also require this many named callers to match")
    ap.add_argument("--max-candidates", type=int, default=1,
                    help="Max donor matches to consider confident (default: 1)")
    args = ap.parse_args()

    print("Loading GC call graph...", file=sys.stderr)
    gc_callees, gc_callers = load_gc_callgraph(CALL_GRAPH_TSV)
    unnamed, named = load_gc_syms()

    # Build donor call graphs
    donor_callgraph: dict[str, tuple[dict[str, set[str]], dict[str, set[str]]]] = {}
    for donor_id, elf_path in DONOR_ELFS:
        if not elf_path.exists():
            print(f"  Skipping {donor_id} (ELF not found)", file=sys.stderr)
            continue
        print(f"  Parsing {donor_id} ELF...", file=sys.stderr)
        d_callees, d_callers = extract_elf_callgraph(elf_path)
        donor_callgraph[donor_id] = (d_callees, d_callers)
        named_fns = sum(1 for k in d_callees if k and not k.startswith("fn_"))
        print(f"    {len(d_callees)} functions with outgoing calls, "
              f"{named_fns} named", file=sys.stderr)

    print("Matching unnamed GC functions...", file=sys.stderr)
    results: list[tuple[str, str, str, int, str]] = []
    seen_new_names: set[str] = set(named)

    for fn_name in sorted(unnamed):
        gc_callee_set = {c for c in gc_callees.get(fn_name, set()) if c in named}
        gc_caller_set = {c for c in gc_callers.get(fn_name, set()) if c in named}

        if len(gc_callee_set) < args.min_callees:
            continue
        if len(gc_caller_set) < args.min_callers:
            continue

        # For each donor, find functions whose callee set contains all gc named callees
        all_matches: list[tuple[str, str]] = []
        for donor_id, (d_callees, d_callers) in donor_callgraph.items():
            donor_matches = [
                dn for dn, dc in d_callees.items()
                if not dn.startswith("fn_") and gc_callee_set.issubset(dc)
            ]
            for m in donor_matches:
                all_matches.append((donor_id, m))

        # Deduplicate by new name
        unique_names = list(dict.fromkeys(m for _, m in all_matches))

        if len(unique_names) == 0 or len(unique_names) > args.max_candidates:
            continue

        new_name = unique_names[0]
        if new_name in seen_new_names:
            continue

        donor_ids = list(dict.fromkeys(d for d, m in all_matches if m == new_name))
        score = len(gc_callee_set) + len(gc_caller_set)
        results.append((fn_name, new_name, ",".join(donor_ids), score,
                        f"callees={sorted(gc_callee_set)}"))
        seen_new_names.add(new_name)

    results.sort(key=lambda r: -r[3])

    lines = [f"# callgraph_match.py --min-callees {args.min_callees} "
             f"({len(results)} candidates)\n"]
    for fn_name, new_name, donor_ids, score, context in results:
        lines.append(f"{fn_name} {new_name}  # [{donor_ids}] score={score} {context}")

    output = "\n".join(lines) + "\n"
    if args.output:
        Path(args.output).parent.mkdir(parents=True, exist_ok=True)
        Path(args.output).write_text(output)
        print(f"Wrote {len(results)} candidates to {args.output}", file=sys.stderr)
    else:
        print(output)

    print(f"Total: {len(results)} call-graph matches", file=sys.stderr)


if __name__ == "__main__":
    main()
