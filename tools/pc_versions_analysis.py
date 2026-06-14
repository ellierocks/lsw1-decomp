#!/usr/bin/env python3
"""
PC retail/demo analysis for cross-version symbol research.

The PC executables are stripped, but they retain useful source-file paths,
assert strings, command names, and subsystem messages.  This tool compares
those strings with the target GameCube DOL and emits conservative research
artifacts without mutating symbols.txt.
"""

from __future__ import annotations

import re
import struct
import subprocess
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
GC_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
GC_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
PC_RETAIL_CANDIDATES = [
    ROOT / "orig/pc/usa/retail/LEGO Star Wars Game/LegoStarwars.exe",
    ROOT / "orig/pc/usa/retail/program files/Giant/LEGO Star Wars Game/LegoStarwars.exe",
    ROOT / "build/pc_usa/LegoStarwars.exe",
]
PC_DEMO_CANDIDATES = [
    ROOT / "orig/pc/usa/LEGOStarWarsDemo/program files/Giant/LEGO Star Wars/LegoStarwars.exe",
    ROOT / "build/pc_usa/LegoStarwars.exe",
]
OUT_DIR = ROOT / "build/pc_analysis"


SECTION_NAMES = {
    0: ".init",
    1: ".text",
    7: ".rodata",
    8: ".data",
    9: ".sdata",
    10: ".sdata2",
}

NOISE_PREFIXES = (
    "D3D",
    "D3DX",
    "ID3DX",
    "COM",
    "CERT",
    "CRYPT",
    "TRUST",
    "WIN",
    "SH",
    "OLE",
    "SEC_",
)

KEY_PATTERNS = [
    re.compile(r"^\.\\.*\.(c|cpp|h)$", re.IGNORECASE),
    re.compile(r"^c:\\projects\\", re.IGNORECASE),
    re.compile(r"^(Nu[A-Za-z0-9_]+|instNu[A-Za-z0-9_]+)"),
    re.compile(r"^(AIPath|AISys|AIScript|AI_|AIOverride|SetAIOverride)"),
    re.compile(r"(Camera|CutScene|CUT_|SCRIPT|TRIGGER|VISIBILITY|PARAM)"),
    re.compile(r"(BufferAlloc|FixUp|cannot|Unable|No buffer|failed|internal error|implement me)"),
    re.compile(r"^(LEGO Options|Go To Scene|Go To Level|Go To Door)$"),
]


@dataclass(frozen=True)
class DolSection:
    name: str
    offset: int
    address: int
    size: int

    def contains_file_offset(self, offset: int, length: int) -> bool:
        return self.offset <= offset and offset + length <= self.offset + self.size

    def address_for_file_offset(self, offset: int) -> int:
        return self.address + (offset - self.offset)


def run_strings(path: Path, min_len: int = 5) -> set[str]:
    result = subprocess.run(
        ["strings", "-n", str(min_len), str(path)],
        check=True,
        capture_output=True,
        text=True,
        errors="replace",
    )
    return {line.strip() for line in result.stdout.splitlines() if line.strip()}


def first_existing(paths: list[Path], label: str) -> Path:
    for path in paths:
        if path.exists():
            return path
    joined = "\n  ".join(str(path.relative_to(ROOT)) for path in paths)
    raise FileNotFoundError(f"No {label} executable found. Checked:\n  {joined}")


def parse_dol_sections(data: bytes) -> list[DolSection]:
    sections: list[DolSection] = []
    for idx in range(18):
        offset = struct.unpack(">I", data[idx * 4 : idx * 4 + 4])[0]
        address = struct.unpack(">I", data[0x48 + idx * 4 : 0x4C + idx * 4])[0]
        size = struct.unpack(">I", data[0x90 + idx * 4 : 0x94 + idx * 4])[0]
        if size == 0:
            continue
        name = SECTION_NAMES.get(idx, f".dol{idx}")
        sections.append(DolSection(name, offset, address, size))
    return sections


def find_string_addresses(needle: str, data: bytes, sections: list[DolSection]) -> list[tuple[str, int]]:
    try:
        raw = needle.encode("ascii") + b"\x00"
    except UnicodeEncodeError:
        return []

    results: list[tuple[str, int]] = []
    start = 0
    while True:
        pos = data.find(raw, start)
        if pos < 0:
            break
        for section in sections:
            if section.contains_file_offset(pos, len(raw)):
                results.append((section.name, section.address_for_file_offset(pos)))
                break
        start = pos + 1
    return results


def load_symbols(path: Path) -> dict[tuple[str, int], str]:
    symbols: dict[tuple[str, int], str] = {}
    if not path.exists():
        return symbols

    pattern = re.compile(r"^(\w+)\s*=\s*(\.\w+):0x([0-9A-Fa-f]+);")
    for line in path.read_text().splitlines():
        match = pattern.match(line.strip())
        if not match:
            continue
        name, section, address = match.groups()
        symbols[(section, int(address, 16))] = name
    return symbols


def is_noise(s: str) -> bool:
    if len(s) > 180:
        return True
    return s.startswith(NOISE_PREFIXES)


def is_key_string(s: str) -> bool:
    if is_noise(s):
        return False
    return any(pattern.search(s) for pattern in KEY_PATTERNS)


def safe_symbol_suffix(s: str, max_len: int = 72) -> str:
    s = s.replace("\\", "/")
    if s.lower().endswith((".c", ".cpp", ".h")):
        stem = Path(s).stem
        return "SourceFile_" + re.sub(r"[^A-Za-z0-9]+", "_", stem).strip("_")

    if ":" in s:
        stem = re.sub(r"[^A-Za-z0-9]+", "_", s).strip("_")
        return "Assert_" + stem[:max_len]

    stem = re.sub(r"[^A-Za-z0-9]+", "_", s).strip("_")
    if not stem:
        stem = "PCShared"
    if stem[0].isdigit():
        stem = "_" + stem
    return stem[:max_len]


def classify_string(s: str) -> str:
    lower = s.lower()
    if lower.endswith((".c", ".cpp", ".h")) or lower.startswith("c:\\projects\\"):
        return "source_paths"
    if lower.startswith("nu") or lower.startswith("instnu"):
        return "nu2"
    if lower.startswith("ai") or "ai" in lower and ("bufferalloc" in lower or "path" in lower):
        return "ai"
    if "cutscene" in lower or "cut_" in s or "camera" in lower:
        return "camera_cutscene"
    if any(token in s for token in ("SCRIPT", "TRIGGER", "VISIBILITY", "PARAM")):
        return "script"
    if "lego options" in lower or lower.startswith("go to "):
        return "debug_menu"
    if any(token in lower for token in ("cannot", "unable", "failed", "internal error", "no buffer")):
        return "assert_errors"
    return "other"


def write_list(path: Path, strings: list[str]) -> None:
    path.write_text("".join(f"{s}\n" for s in strings))


def main() -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    pc_retail_exe = first_existing(PC_RETAIL_CANDIDATES, "PC retail")
    pc_demo_exe = first_existing(PC_DEMO_CANDIDATES, "PC demo")

    retail = run_strings(pc_retail_exe)
    demo = run_strings(pc_demo_exe)
    gc_strings = run_strings(GC_DOL)
    gc_data = GC_DOL.read_bytes()
    sections = parse_dol_sections(gc_data)
    symbols = load_symbols(GC_SYMBOLS)

    shared_retail_gc = sorted(retail & gc_strings)
    shared_demo_gc = sorted(demo & gc_strings)
    shared_all = sorted(retail & demo & gc_strings)
    retail_unique = sorted(retail - demo)
    demo_unique = sorted(demo - retail)

    write_list(OUT_DIR / "retail_gc_shared_strings.txt", shared_retail_gc)
    write_list(OUT_DIR / "demo_gc_shared_strings.txt", shared_demo_gc)
    write_list(OUT_DIR / "pc_gc_shared_strings.txt", shared_all)
    write_list(OUT_DIR / "retail_unique_strings.txt", retail_unique)
    write_list(OUT_DIR / "demo_unique_strings.txt", demo_unique)

    categorized: dict[str, list[str]] = {}
    for s in shared_all:
        if is_key_string(s):
            categorized.setdefault(classify_string(s), []).append(s)

    shared_entries: list[tuple[str, str, int, str, str]] = []
    for s in sorted({item for values in categorized.values() for item in values}):
        for section, address in find_string_addresses(s, gc_data, sections):
            current = symbols.get((section, address), "")
            shared_entries.append((s, section, address, current, "retail+demo+gc"))

    proposal_lines = [
        "# PC Retail/Demo Symbol Proposals",
        "# Generated by tools/pc_versions_analysis.py",
        "# Review before applying to config/GL5E4F/symbols.txt.",
        "",
    ]
    for s, section, address, current, evidence in shared_entries:
        proposed = "str_PC_" + safe_symbol_suffix(s)
        if current and not current.startswith(("lbl_", "jumptable_", "string_")):
            proposed = current
        size = len(s.encode("ascii", errors="ignore")) + 1
        proposal_lines.append(f"# {evidence}: {s}")
        if current:
            proposal_lines.append(f"# Current: {current}")
        proposal_lines.append(f"{proposed} = {section}:0x{address:08X}; // type:object size:0x{size:X} data:string")
        proposal_lines.append("")
    (OUT_DIR / "pc_symbol_proposals.txt").write_text("\n".join(proposal_lines))

    report_lines = [
        "=== PC Retail/Demo Analysis ===",
        "",
        f"Retail executable: {pc_retail_exe.relative_to(ROOT)}",
        f"Demo executable: {pc_demo_exe.relative_to(ROOT)}",
        f"Retail strings: {len(retail)}",
        f"Demo strings: {len(demo)}",
        f"GameCube DOL strings: {len(gc_strings)}",
        f"Retail strings shared with GameCube: {len(shared_retail_gc)}",
        f"Demo strings shared with GameCube: {len(shared_demo_gc)}",
        f"Strings shared by retail, demo, and GameCube: {len(shared_all)}",
        f"Retail-only strings: {len(retail_unique)}",
        f"Demo-only strings: {len(demo_unique)}",
        "",
        "=== DOL Sections ===",
    ]
    for section in sections:
        report_lines.append(
            f"{section.name}: file 0x{section.offset:X}-0x{section.offset + section.size:X}, "
            f"addr 0x{section.address:08X}-0x{section.address + section.size:08X}"
        )

    for category in sorted(categorized):
        report_lines.extend(["", f"=== {category} ({len(categorized[category])}) ==="])
        report_lines.extend(categorized[category][:200])

    report_lines.extend(["", "=== Shared string addresses ==="])
    for s, section, address, current, evidence in shared_entries:
        name = current or "<unnamed>"
        report_lines.append(f"{section}:0x{address:08X} {name}: {s}")

    report_lines.extend(["", "=== Demo-only key strings ==="])
    report_lines.extend([s for s in demo_unique if is_key_string(s)][:300])

    report_lines.extend(["", "=== Retail-only key strings ==="])
    report_lines.extend([s for s in retail_unique if is_key_string(s)][:300])

    (OUT_DIR / "pc_analysis.txt").write_text("\n".join(report_lines) + "\n")

    print(f"Retail strings: {len(retail)}")
    print(f"Demo strings: {len(demo)}")
    print(f"Shared retail/demo/GameCube strings: {len(shared_all)}")
    print(f"High-value shared strings with DOL addresses: {len(shared_entries)}")
    print(f"Wrote {OUT_DIR.relative_to(ROOT) / 'pc_analysis.txt'}")
    print(f"Wrote {OUT_DIR.relative_to(ROOT) / 'pc_symbol_proposals.txt'}")


if __name__ == "__main__":
    main()
