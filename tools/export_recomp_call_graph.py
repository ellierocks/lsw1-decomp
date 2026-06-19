#!/usr/bin/env python3
"""Export direct PPC branch-with-link call edges for lsw1-recomp review.

This intentionally does not infer calls from address proximity. Each row is
derived from a PPC branch instruction with LK=1 in the target DOL's .text.
"""

from __future__ import annotations

import csv
import re
import struct
from bisect import bisect_right
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOL = ROOT / "orig" / "GL5E4F" / "sys" / "main.dol"
SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
OUTPUT = ROOT / "docs" / "recomp_exports"
TEXT_FILE_OFFSET = 0x4A0
TEXT_ADDRESS = 0x800034A0
TEXT_SIZE = 0x00189660
SYMBOL_RE = re.compile(r"^(\S+)\s+=\s+\.text:0x([0-9A-Fa-f]+);\s*//\s*(.*)$")
SIZE_RE = re.compile(r"size:0x([0-9A-Fa-f]+)")
PLATFORM_PREFIXES = ("OS", "DVD", "PAD", "SI", "VI", "GX", "AX", "AI", "AR", "CARD", "EXI",
                     "NuFile", "NuDat", "NuRndr", "NuTex", "NuSound")


def parse_functions() -> dict[int, tuple[str, int]]:
    functions: dict[int, tuple[str, int]] = {}
    for line in SYMBOLS.read_text(encoding="utf-8").splitlines():
        match = SYMBOL_RE.match(line)
        if not match:
            continue
        size = SIZE_RE.search(match.group(3))
        if size:
            functions[int(match.group(2), 16)] = (match.group(1), int(size.group(1), 16))
    return functions


def resolve(address: int, functions: dict[int, tuple[str, int]], starts: list[int]) -> tuple[str, int] | None:
    index = bisect_right(starts, address) - 1
    if index < 0:
        return None
    start = starts[index]
    name, size = functions[start]
    if size and address < start + size:
        return name, start
    return None


def branch_target(instruction: int, address: int) -> int:
    displacement = instruction & 0x03FFFFFC  # LI already includes its two low zero bits.
    if displacement & 0x02000000:
        displacement -= 0x04000000
    absolute = bool(instruction & 0x2)
    return (displacement if absolute else address + displacement) & 0xFFFFFFFF


def is_direct_call(instruction: int) -> bool:
    return (instruction >> 26) == 18 and bool(instruction & 1)  # opcode b/bl, LK=1


def is_platform(name: str) -> bool:
    return name.startswith(PLATFORM_PREFIXES)


def main() -> int:
    if not DOL.is_file():
        raise SystemExit(f"missing target DOL: {DOL}")
    functions = parse_functions()
    starts = sorted(functions)
    data = DOL.read_bytes()[TEXT_FILE_OFFSET:TEXT_FILE_OFFSET + TEXT_SIZE]
    rows: list[dict[str, str]] = []
    for offset in range(0, len(data) - 3, 4):
        instruction = struct.unpack_from(">I", data, offset)[0]
        if not is_direct_call(instruction):
            continue
        callsite = TEXT_ADDRESS + offset
        caller = resolve(callsite, functions, starts)
        callee = resolve(branch_target(instruction, callsite), functions, starts)
        if not caller or not callee:
            continue
        caller_name, caller_address = caller
        callee_name, callee_address = callee
        rows.append({
            "caller_address": f"0x{caller_address:08X}", "caller_name": caller_name,
            "callee_address": f"0x{callee_address:08X}", "callee_name": callee_name,
            "callsite_address": f"0x{callsite:08X}",
            "evidence_source": "PPC opcode 18 with LK=1 (bl/bla)",
            "evidence": f"instruction=0x{instruction:08X}; direct_target=0x{branch_target(instruction, callsite):08X}",
        })
    OUTPUT.mkdir(parents=True, exist_ok=True)
    graph = OUTPUT / "call_graph.tsv"
    with graph.open("w", newline="", encoding="utf-8") as output:
        writer = csv.DictWriter(output, fieldnames=("caller_address", "caller_name", "callee_address", "callee_name", "callsite_address", "evidence_source", "evidence"), delimiter="\t")
        writer.writeheader()
        writer.writerows(rows)
    boundary = [row for row in rows if is_platform(row["caller_name"]) or is_platform(row["callee_name"])]
    families = Counter(prefix for row in boundary for name in (row["caller_name"], row["callee_name"])
                       for prefix in PLATFORM_PREFIXES if name.startswith(prefix))
    report = OUTPUT / "call_graph_summary.md"
    report.write_text(
        "# Recomp call graph export\n\n"
        f"- Direct `bl`/`bla` edges: **{len(rows)}**\n"
        f"- Platform-boundary edges: **{len(boundary)}**\n"
        "- Evidence: PPC opcode 18 with LK=1; caller/callee are resolved only when each address falls within a sized `.text` symbol.\n"
        "- Limitations: indirect calls, tail branches, unresolved targets, and symbols without usable sizes are excluded. This is call-edge evidence, not call order.\n\n"
        "## Platform families present\n\n" + "\n".join(f"- `{family}`: {count}" for family, count in sorted(families.items())) + "\n",
        encoding="utf-8",
    )
    print(f"wrote {graph}: {len(rows)} direct edges; {len(boundary)} platform-boundary edges")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
