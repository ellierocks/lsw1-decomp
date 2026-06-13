#!/usr/bin/env python3
"""
Find DOL text references to named strings in config/GL5E4F/symbols.txt.

This is intentionally lightweight: it scans PowerPC address materialization
patterns that the Metrowerks compiler emits for string pointers, then maps the
instruction address back to the containing configured function symbol.
"""

from __future__ import annotations

import bisect
import re
import struct
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOL = ROOT / "orig/GL5E4F/sys/main.dol"
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
OUT_DIR = ROOT / "build/xrefs"

SECTION_NAMES = {
    0: ".init",
    1: ".text",
    7: ".rodata",
    8: ".data",
    9: ".sdata",
    10: ".sdata2",
}

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s+//\s+(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")


@dataclass(frozen=True)
class DolSection:
    index: int
    name: str
    offset: int
    address: int
    size: int


@dataclass(frozen=True)
class Symbol:
    name: str
    section: str
    address: int
    size: int
    meta: str


@dataclass(frozen=True)
class Xref:
    string: Symbol
    function: Symbol | None
    instruction_address: int
    instruction: int
    kind: str


def sign16(value: int) -> int:
    return value - 0x10000 if value & 0x8000 else value


def parse_dol_sections(path: Path) -> list[DolSection]:
    header = path.read_bytes()[:0x100]
    sections: list[DolSection] = []
    for i in range(18):
        offset = struct.unpack(">I", header[i * 4 : i * 4 + 4])[0]
        address = struct.unpack(">I", header[0x48 + i * 4 : 0x4C + i * 4])[0]
        size = struct.unpack(">I", header[0x90 + i * 4 : 0x94 + i * 4])[0]
        if size:
            sections.append(DolSection(i, SECTION_NAMES.get(i, f".dol{i}"), offset, address, size))
    return sections


def parse_symbols(path: Path) -> list[Symbol]:
    symbols: list[Symbol] = []
    for line in path.read_text().splitlines():
        match = SYMBOL_RE.match(line)
        if not match:
            continue
        size_match = SIZE_RE.search(match.group("meta"))
        symbols.append(
            Symbol(
                match.group("name"),
                match.group("section"),
                int(match.group("addr"), 16),
                int(size_match.group(1), 16) if size_match else 0,
                match.group("meta"),
            )
        )
    return symbols


def containing_function(functions: list[Symbol], starts: list[int], address: int) -> Symbol | None:
    index = bisect.bisect_right(starts, address) - 1
    if index < 0:
        return None
    function = functions[index]
    if function.address <= address < function.address + function.size:
        return function
    return None


def scan_section_for_xrefs(
    data: bytes,
    section: DolSection,
    targets: dict[int, Symbol],
    functions: list[Symbol],
    function_starts: list[int],
) -> list[Xref]:
    section_data = data[section.offset : section.offset + section.size]
    xrefs: list[Xref] = []

    for offset in range(0, len(section_data) - 4, 4):
        instruction = struct.unpack(">I", section_data[offset : offset + 4])[0]
        opcode = instruction >> 26
        ra = (instruction >> 16) & 0x1F
        rd = (instruction >> 21) & 0x1F
        imm = instruction & 0xFFFF

        # lis rD, imm is encoded as addis rD, r0, imm.
        if opcode != 15 or ra != 0:
            continue

        high = sign16(imm) << 16
        base_address = section.address + offset
        lookahead_end = min(len(section_data), offset + 4 + 32 * 4)
        for use_offset in range(offset + 4, lookahead_end, 4):
            use = struct.unpack(">I", section_data[use_offset : use_offset + 4])[0]
            use_opcode = use >> 26
            use_rd = (use >> 21) & 0x1F
            use_ra = (use >> 16) & 0x1F
            use_imm = use & 0xFFFF
            use_address = section.address + use_offset

            if use_ra == rd and use_opcode in {14, 32, 34, 35, 36, 37, 38, 40, 42, 43, 44, 45, 46, 47}:
                target_address = (high + sign16(use_imm)) & 0xFFFFFFFF
                target = targets.get(target_address)
                if target:
                    function = containing_function(functions, function_starts, use_address)
                    xrefs.append(Xref(target, function, use_address, use, f"lis+op{use_opcode}"))
            elif use_ra == rd and use_opcode == 24:
                target_address = (high | use_imm) & 0xFFFFFFFF
                target = targets.get(target_address)
                if target:
                    function = containing_function(functions, function_starts, use_address)
                    xrefs.append(Xref(target, function, use_address, use, "lis+ori"))

            if use_offset != offset + 4 and use_rd == rd:
                break

        # Also catch the rare exact high-half anchor as diagnostic evidence.
        if high in targets:
            function = containing_function(functions, function_starts, base_address)
            xrefs.append(Xref(targets[high], function, base_address, instruction, "lis-exact"))

    return xrefs


def format_xref(xref: Xref) -> str:
    function = xref.function.name if xref.function else "<unknown>"
    function_addr = f"0x{xref.function.address:08X}" if xref.function else "????????"
    return (
        f"{xref.string.name:<56} 0x{xref.string.address:08X}  "
        f"{function:<32} {function_addr}  "
        f"at 0x{xref.instruction_address:08X}  {xref.kind}"
    )


def main() -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    symbols = parse_symbols(SYMBOLS)
    functions = sorted(
        (
            symbol
            for symbol in symbols
            if symbol.section in {".init", ".text"} and "type:function" in symbol.meta
        ),
        key=lambda symbol: symbol.address,
    )
    function_starts = [symbol.address for symbol in functions]

    strings = sorted(
        (
            symbol
            for symbol in symbols
            if symbol.name.startswith("str_")
            and symbol.section in {".rodata", ".data", ".sdata", ".sdata2"}
        ),
        key=lambda symbol: symbol.address,
    )
    targets = {symbol.address: symbol for symbol in strings}

    data = DOL.read_bytes()
    sections = [section for section in parse_dol_sections(DOL) if section.name in {".init", ".text"}]
    xrefs: list[Xref] = []
    for section in sections:
        xrefs.extend(scan_section_for_xrefs(data, section, targets, functions, function_starts))

    xrefs.sort(key=lambda xref: (xref.string.address, xref.instruction_address))
    all_lines = [
        "String xrefs for GL5E4F main.dol",
        f"functions: {len(functions)}",
        f"strings: {len(strings)}",
        f"xrefs: {len(xrefs)}",
        "",
    ]
    all_lines.extend(format_xref(xref) for xref in xrefs)
    (OUT_DIR / "string_xrefs.txt").write_text("\n".join(all_lines) + "\n")

    focus_re = re.compile(r"NuFile|NuDat|NuPP|NuFPar|NuMem|NuDebug|NuSpecial")
    focus = [xref for xref in xrefs if focus_re.search(xref.string.name)]
    focus_lines = [
        "Focused Nu2 file/debug string xrefs",
        f"xrefs: {len(focus)}",
        "",
    ]
    focus_lines.extend(format_xref(xref) for xref in focus)
    (OUT_DIR / "nufile_string_xrefs.txt").write_text("\n".join(focus_lines) + "\n")

    print(f"Wrote {len(xrefs)} xrefs for {len(strings)} strings.")
    print(f"Focused NuFile/NuDat/NuPP/NuDebug/NuSpecial xrefs: {len(focus)}")


if __name__ == "__main__":
    main()
