#!/usr/bin/env python3
"""
Cross-version lineage analysis for LSW1/Nu2 reference builds.

This compares the target US GameCube DOL against:
- PAL GameCube LSW1 retail (`GL5P4F`)
- LSW2 PS2 prototype (`SLUS_999.99`, Jun 13 2006 print preview)
- TCS Wii prototype (`RLGP64-387`) DATA partition

The output is evidence for docs and symbol review.  It does not mutate
config/GL5E4F/symbols.txt.
"""

from __future__ import annotations

import re
import struct
import subprocess
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
US_GC_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
PAL_GC_ISO = ROOT / "orig/GL5P4F/LEGO Star Wars - The Video Game (United KIngdom).iso"
PAL_GC_DOL = ROOT / "build/region/GL5P4F/main.dol"
LSW2_PS2_BIN = ROOT / "orig/lsw2-ps2-prototype/PS2 - Lego Star Wars II Print Preview.bin"
LSW2_PS2_ELF = ROOT / "build/lineage/lsw2_ps2/SLUS_999.99"
TCS_WII_ISO = ROOT / "orig/tcs-wii-prototype/RLGP64-387.iso"
TCS_WII_DUMPINFO = ROOT / "orig/tcs-wii-prototype/RLGP64-387-dumpinfo.txt"
TCS_WII_DOL = ROOT / "orig/tcs-wii-prototype/DATA/sys/main.dol"
TCS_WII_FILES = ROOT / "orig/tcs-wii-prototype/DATA/files"
OUT_DIR = ROOT / "build/lineage"


KEY_RE = re.compile(
    r"(Nu[A-Z]|instNu|AIPath|AISys|AIScript|Camera|SCRIPT|TRIGGER|"
    r"Go To|LEGO Options|\.c$|\.cpp$|gameapi|nu2api|Debug|debug)"
)


@dataclass(frozen=True)
class DolSection:
    index: int
    name: str
    offset: int
    address: int
    size: int


SECTION_NAMES = {
    0: ".init",
    1: ".text",
    7: ".rodata",
    8: ".data",
    9: ".sdata",
    10: ".sdata2",
}


def run_strings(path: Path, min_len: int = 5) -> set[str]:
    result = subprocess.run(
        ["strings", "-n", str(min_len), str(path)],
        check=True,
        capture_output=True,
        text=True,
        errors="replace",
    )
    return {line.strip() for line in result.stdout.splitlines() if line.strip()}


def extract_gc_main_dol(iso: Path, output: Path) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    with iso.open("rb") as f:
        f.seek(0x420)
        dol_offset = struct.unpack(">I", f.read(4))[0]
        f.seek(dol_offset)
        header = f.read(0x100)
        max_end = 0
        for i in range(18):
            offset = struct.unpack(">I", header[i * 4 : i * 4 + 4])[0]
            size = struct.unpack(">I", header[0x90 + i * 4 : 0x94 + i * 4])[0]
            if size:
                max_end = max(max_end, offset + size)
        f.seek(dol_offset)
        output.write_bytes(f.read(max_end))


def extract_lsw2_elf() -> None:
    LSW2_PS2_ELF.parent.mkdir(parents=True, exist_ok=True)
    if LSW2_PS2_ELF.exists():
        return
    subprocess.run(
        [
            "7z",
            "e",
            "-y",
            f"-o{LSW2_PS2_ELF.parent}",
            str(LSW2_PS2_BIN),
            "SLUS_999.99",
            "SYSTEM.CNF",
        ],
        check=True,
        capture_output=True,
        text=True,
    )


def parse_dol_sections(path: Path) -> list[DolSection]:
    data = path.read_bytes()[:0x100]
    sections: list[DolSection] = []
    for i in range(18):
        offset = struct.unpack(">I", data[i * 4 : i * 4 + 4])[0]
        address = struct.unpack(">I", data[0x48 + i * 4 : 0x4C + i * 4])[0]
        size = struct.unpack(">I", data[0x90 + i * 4 : 0x94 + i * 4])[0]
        if size:
            sections.append(DolSection(i, SECTION_NAMES.get(i, f".dol{i}"), offset, address, size))
    return sections


def find_dol_string(path: Path, needle: str) -> list[tuple[str, int]]:
    data = path.read_bytes()
    raw = needle.encode("ascii", errors="ignore") + b"\0"
    sections = parse_dol_sections(path)
    hits: list[tuple[str, int]] = []
    start = 0
    while True:
        pos = data.find(raw, start)
        if pos < 0:
            return hits
        for section in sections:
            if section.offset <= pos < section.offset + section.size:
                hits.append((section.name, section.address + pos - section.offset))
                break
        start = pos + 1


def wii_partition_summary(path: Path) -> list[str]:
    if not path.exists():
        return ["TCS Wii ISO not present."]
    lines: list[str] = []
    with path.open("rb") as f:
        header = f.read(0x50000)
    lines.append(f"Game ID: {header[:6].decode('ascii', errors='replace')}")
    part_count = struct.unpack(">I", header[0x40000:0x40004])[0]
    table_offset = struct.unpack(">I", header[0x40004:0x40008])[0] << 2
    lines.append(f"Partition groups: {part_count}, table offset: 0x{table_offset:X}")
    if 0 < table_offset < len(header):
        for i in range(part_count):
            entry = table_offset + i * 8
            part_offset = struct.unpack(">I", header[entry:entry + 4])[0] << 2
            part_type = struct.unpack(">I", header[entry + 4:entry + 8])[0]
            lines.append(f"Partition {i}: type={part_type} offset=0x{part_offset:X}")
    return lines


def count_files(root: Path) -> tuple[int, int]:
    files = 0
    dirs = 0
    if not root.exists():
        return files, dirs
    for path in root.rglob("*"):
        if path.is_dir():
            dirs += 1
        elif path.is_file():
            files += 1
    return files, dirs


def read_short_text(path: Path, limit: int = 80) -> list[str]:
    if not path.exists():
        return []
    lines = path.read_text(errors="replace").splitlines()
    return lines[:limit]


def write_list(path: Path, values: list[str]) -> None:
    path.write_text("".join(f"{value}\n" for value in values))


def main() -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    if PAL_GC_ISO.exists():
        extract_gc_main_dol(PAL_GC_ISO, PAL_GC_DOL)
    if LSW2_PS2_BIN.exists():
        extract_lsw2_elf()

    us = run_strings(US_GC_DOL)
    pal = run_strings(PAL_GC_DOL) if PAL_GC_DOL.exists() else set()
    lsw2 = run_strings(LSW2_PS2_ELF) if LSW2_PS2_ELF.exists() else set()
    tcs = run_strings(TCS_WII_DOL) if TCS_WII_DOL.exists() else set()

    us_pal_shared = sorted(us & pal)
    pal_unique = sorted(pal - us)
    us_unique = sorted(us - pal)
    us_lsw2_shared = sorted(us & lsw2)
    us_tcs_shared = sorted(us & tcs)
    lsw2_tcs_shared = sorted(lsw2 & tcs)
    lsw2_key = sorted(s for s in lsw2 if KEY_RE.search(s))
    tcs_key = sorted(s for s in tcs if KEY_RE.search(s))
    lsw2_shared_key = sorted(s for s in us_lsw2_shared if KEY_RE.search(s))
    tcs_shared_key = sorted(s for s in us_tcs_shared if KEY_RE.search(s))
    pal_unique_key = sorted(s for s in pal_unique if KEY_RE.search(s))

    write_list(OUT_DIR / "us_gc_strings.txt", sorted(us))
    write_list(OUT_DIR / "pal_gc_strings.txt", sorted(pal))
    write_list(OUT_DIR / "lsw2_ps2_strings.txt", sorted(lsw2))
    write_list(OUT_DIR / "tcs_wii_strings.txt", sorted(tcs))
    write_list(OUT_DIR / "us_pal_shared.txt", us_pal_shared)
    write_list(OUT_DIR / "pal_unique_vs_us.txt", pal_unique)
    write_list(OUT_DIR / "us_unique_vs_pal.txt", us_unique)
    write_list(OUT_DIR / "us_lsw2_shared.txt", us_lsw2_shared)
    write_list(OUT_DIR / "us_tcs_shared.txt", us_tcs_shared)
    write_list(OUT_DIR / "lsw2_tcs_shared.txt", lsw2_tcs_shared)
    write_list(OUT_DIR / "lsw2_key_strings.txt", lsw2_key)
    write_list(OUT_DIR / "tcs_key_strings.txt", tcs_key)

    report = [
        "=== Cross-Version Lineage Analysis ===",
        "",
        f"US GameCube strings: {len(us)}",
        f"PAL GameCube strings: {len(pal)}",
        f"LSW2 PS2 prototype strings: {len(lsw2)}",
        f"TCS Wii prototype strings: {len(tcs)}",
        f"US/PAL shared strings: {len(us_pal_shared)}",
        f"PAL-only strings vs US: {len(pal_unique)}",
        f"US-only strings vs PAL: {len(us_unique)}",
        f"US/LSW2 shared strings: {len(us_lsw2_shared)}",
        f"US/TCS shared strings: {len(us_tcs_shared)}",
        f"LSW2/TCS shared strings: {len(lsw2_tcs_shared)}",
        "",
        "=== PAL DOL Sections ===",
    ]
    for section in parse_dol_sections(PAL_GC_DOL):
        report.append(
            f"{section.name}: file 0x{section.offset:X}-0x{section.offset + section.size:X}, "
            f"addr 0x{section.address:08X}-0x{section.address + section.size:08X}"
        )

    report.extend(["", "=== PAL-only key strings ==="])
    report.extend(pal_unique_key[:200])

    report.extend(["", "=== LSW2 shared key strings with US DOL addresses ==="])
    for s in lsw2_shared_key[:300]:
        hits = find_dol_string(US_GC_DOL, s)
        if hits:
            for section, address in hits:
                report.append(f"{section}:0x{address:08X}: {s}")
        else:
            report.append(f"<no-address>: {s}")

    report.extend(["", "=== LSW2 lineage key strings ==="])
    report.extend(lsw2_key[:500])

    report.extend(["", "=== TCS shared key strings with US DOL addresses ==="])
    for s in tcs_shared_key[:300]:
        hits = find_dol_string(US_GC_DOL, s)
        if hits:
            for section, address in hits:
                report.append(f"{section}:0x{address:08X}: {s}")
        else:
            report.append(f"<no-address>: {s}")

    report.extend(["", "=== TCS lineage key strings ==="])
    report.extend(tcs_key[:500])

    report.extend(["", "=== TCS Wii Prototype DATA Partition ==="])
    if TCS_WII_DUMPINFO.exists():
        report.extend(TCS_WII_DUMPINFO.read_text(errors="replace").splitlines())
    report.extend(wii_partition_summary(TCS_WII_ISO))
    if TCS_WII_DOL.exists():
        report.append(f"Extracted main.dol: {TCS_WII_DOL.relative_to(ROOT)}")
        report.append("TCS DOL sections:")
        for section in parse_dol_sections(TCS_WII_DOL):
            report.append(
                f"{section.name}: file 0x{section.offset:X}-0x{section.offset + section.size:X}, "
                f"addr 0x{section.address:08X}-0x{section.address + section.size:08X}"
            )
    files, dirs = count_files(TCS_WII_FILES)
    report.append(f"DATA/files directories: {dirs}")
    report.append(f"DATA/files files: {files}")
    for name in ["Levels/levels.txt", "Levels/areas.txt", "Levels/missions.txt", "Scripts/SCRIPT.TXT"]:
        rel = Path(name)
        lines = read_short_text(TCS_WII_FILES / rel, limit=30)
        report.extend(["", f"--- {rel} ({len(lines)} preview lines) ---"])
        report.extend(lines)

    (OUT_DIR / "lineage_analysis.txt").write_text("\n".join(report) + "\n")

    print(f"US GameCube strings: {len(us)}")
    print(f"PAL GameCube strings: {len(pal)}")
    print(f"LSW2 PS2 prototype strings: {len(lsw2)}")
    print(f"TCS Wii prototype strings: {len(tcs)}")
    print(f"US/PAL shared strings: {len(us_pal_shared)}")
    print(f"US/LSW2 shared strings: {len(us_lsw2_shared)}")
    print(f"US/TCS shared strings: {len(us_tcs_shared)}")
    print(f"Wrote {OUT_DIR.relative_to(ROOT) / 'lineage_analysis.txt'}")


if __name__ == "__main__":
    main()
