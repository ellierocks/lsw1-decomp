#!/usr/bin/env python3
"""Optional Ghidra helper for exporting decompiler output.

This is intentionally outside the normal build. It creates/uses a local Ghidra
project, applies names from symbols.txt, and exports C-like decompiler output
for one function at a time.

Ghidra cannot import DOL files directly. Run `convert-dol` once first:
    python tools/ghidra_decompile.py convert-dol
    GHIDRA_INSTALL_DIR=/opt/ghidra python tools/ghidra_decompile.py import-symbols
"""
import argparse
import os
import shutil
import struct
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
DEFAULT_DOL = ROOT / "orig/GL5E4F/sys/main.dol"
DEFAULT_ELF = ROOT / "ghidra_out/main.elf"
DEFAULT_PROJECT_DIR = ROOT / "ghidra_out/project"
DEFAULT_OUT_DIR = ROOT / "ghidra_out/decomp"
DEFAULT_SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
SCRIPTS_DIR = ROOT / "tools/ghidra_scripts"
PROJECT_NAME = "lsw1_ghidra"


def find_analyze_headless() -> str:
    tool = shutil.which("analyzeHeadless")
    if tool:
        return tool

    install_dir = os.environ.get("GHIDRA_INSTALL_DIR")
    if install_dir:
        candidate = Path(install_dir) / "support" / "analyzeHeadless"
        if candidate.exists():
            return str(candidate)

    raise SystemExit(
        "Could not find analyzeHeadless. Put it on PATH or set GHIDRA_INSTALL_DIR."
    )


def run_headless(args: list[str]) -> None:
    analyze = find_analyze_headless()
    result = subprocess.run([analyze, *args], cwd=ROOT)
    if result.returncode != 0:
        raise SystemExit(result.returncode)


def base_args(project_dir: Path, dol: Path) -> list[str]:
    args = [
        str(project_dir),
        PROJECT_NAME,
        "-scriptPath",
        str(SCRIPTS_DIR),
    ]

    if not project_dir.exists():
        project_dir.mkdir(parents=True, exist_ok=True)

    # Import on first use. Later runs process the existing project.
    project_marker = project_dir / f"{PROJECT_NAME}.gpr"
    if project_marker.exists():
        args.extend(["-process", dol.name])
    else:
        args.extend(["-import", str(dol)])

    return args


def command_convert_dol(args: argparse.Namespace) -> None:
    """Convert main.dol to a PowerPC ELF so Ghidra can import it."""
    dol = args.dol.read_bytes()
    text_offsets = struct.unpack_from(">7I", dol, 0x00)
    data_offsets = struct.unpack_from(">11I", dol, 0x1C)
    text_addrs   = struct.unpack_from(">7I", dol, 0x48)
    data_addrs   = struct.unpack_from(">11I", dol, 0x64)
    text_sizes   = struct.unpack_from(">7I", dol, 0x90)
    data_sizes   = struct.unpack_from(">11I", dol, 0xAC)
    bss_addr, bss_size, entry = struct.unpack_from(">III", dol, 0xD8)

    segs = []
    for i in range(7):
        if text_offsets[i] and text_sizes[i]:
            segs.append((text_addrs[i], text_offsets[i], text_sizes[i], 5, False))
    for i in range(11):
        if data_offsets[i] and data_sizes[i]:
            segs.append((data_addrs[i], data_offsets[i], data_sizes[i], 6, False))
    if bss_size:
        segs.append((bss_addr, 0, bss_size, 6, True))

    e_phentsize = 32
    e_ehsize = 52
    e_phoff = e_ehsize
    ph_count = len(segs)
    headers_size = e_ehsize + ph_count * e_phentsize

    current_offset = headers_size
    real_offsets = []
    seg_data = []
    for vaddr, file_off, size, flags, is_bss in segs:
        if is_bss:
            real_offsets.append(current_offset)
            seg_data.append(b"")
        else:
            current_offset = (current_offset + 3) & ~3
            real_offsets.append(current_offset)
            seg_data.append(dol[file_off : file_off + size])
            current_offset += size

    elf_hdr = struct.pack(">4sBBBBxxxxxxxx", b"\x7fELF", 1, 2, 1, 0)
    elf_hdr += struct.pack(">HHIIIIIHHHHHH",
        2, 20, 1, entry, e_phoff, 0, 0,
        e_ehsize, e_phentsize, ph_count, 0x28, 0, 0)

    result = bytearray(elf_hdr)
    for i, (vaddr, file_off, size, flags, is_bss) in enumerate(segs):
        filesz = 0 if is_bss else size
        result.extend(struct.pack(">IIIIIIII",
            1, real_offsets[i], vaddr, vaddr, filesz, size, flags, 4))

    for i, (vaddr, file_off, size, flags, is_bss) in enumerate(segs):
        if not is_bss:
            while len(result) % 4:
                result.append(0)
            result.extend(seg_data[i])

    out = args.elf
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(result)
    print(f"Wrote {len(result):,} bytes → {out}")


def command_import_symbols(args: argparse.Namespace) -> None:
    elf = args.elf
    if not elf.exists():
        raise SystemExit(f"ELF not found: {elf}\nRun: python tools/ghidra_decompile.py convert-dol")
    run_headless([
        *base_args(args.project_dir, elf),
        "-postScript",
        "ApplySymbolsTxt.java",
        str(args.symbols),
        "-noanalysis",
    ])


def command_decompile(args: argparse.Namespace) -> None:
    elf = args.elf
    if not elf.exists():
        raise SystemExit(f"ELF not found: {elf}\nRun: python tools/ghidra_decompile.py convert-dol")
    args.out_dir.mkdir(parents=True, exist_ok=True)
    for target in args.targets:
        run_headless([
            *base_args(args.project_dir, elf),
            "-postScript",
            "ExportFunctionDecomp.java",
            target,
            str(args.out_dir),
            str(args.timeout),
            "-noanalysis",
        ])


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Run optional Ghidra symbol import / function decompile helpers."
    )
    parser.add_argument("--dol", type=Path, default=DEFAULT_DOL)
    parser.add_argument("--elf", type=Path, default=DEFAULT_ELF)
    parser.add_argument("--project-dir", type=Path, default=DEFAULT_PROJECT_DIR)

    sub = parser.add_subparsers(dest="command", required=True)

    convert_parser = sub.add_parser("convert-dol",
        help="Convert main.dol → ghidra_out/main.elf for Ghidra import")
    convert_parser.set_defaults(func=command_convert_dol)

    import_parser = sub.add_parser("import-symbols")
    import_parser.add_argument("--symbols", type=Path, default=DEFAULT_SYMBOLS)
    import_parser.set_defaults(func=command_import_symbols)

    decomp_parser = sub.add_parser("decompile")
    decomp_parser.add_argument(
        "targets",
        nargs="+",
        help="Function names or addresses, e.g. Shop_UpdateSubMenu 0x801486FC",
    )
    decomp_parser.add_argument("--out-dir", type=Path, default=DEFAULT_OUT_DIR)
    decomp_parser.add_argument("--timeout", type=int, default=60)
    decomp_parser.set_defaults(func=command_decompile)

    args = parser.parse_args()
    if args.command != "convert-dol" and not args.dol.exists():
        raise SystemExit(f"DOL not found: {args.dol}")
    args.func(args)


if __name__ == "__main__":
    main()
