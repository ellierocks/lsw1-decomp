#!/usr/bin/env python3
"""Auto-generate SDK rename candidates from crashwoc ELF size matching.

For each unnamed fn_ in symbols.txt, looks up functions at the same size in
the crashwoc ELF. If exactly one match exists with the right section prefix
(GX, OS, AX, DVD, etc.), outputs a confident rename candidate.

Output is a batch file suitable for batch_rename.py.

Usage:
    python tools/sdk_automatch.py                        # print candidates
    python tools/sdk_automatch.py -o work/sdk_batch.txt  # write to file
    python tools/sdk_automatch.py --apply                # apply directly
"""
import argparse
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"
DONOR_ELF = ROOT / "orig/nu2/Crash Bandicoot - The Wrath of Cortex (USA)/extracted/files/crashwoc.elf"

# Section prefix filters: map island note keyword → valid name prefixes in crashwoc
PREFIX_FILTERS = {
    "GX": ("GX", "__GX"),
    "OS": ("OS", "__OS"),
    "AX": ("AX", "__AX"),
    "DVD": ("DVD", "__DVD", "DVDLow"),
    "AR": ("AR", "__AR"),
    "VI": ("VI", "__VI"),
    "SI": ("SI", "__SI"),
    "CARD": ("CARD", "__CARD"),
    "AI": ("AI", "__AI"),
    "EXI": ("EXI", "__EXI"),
    "PAD": ("PAD", "__PAD"),
    "MTX": ("MTX", "C_MTX", "PSMTXConcat", "MTXConcat"),
    "NuAnim": ("NuAnim",),
    "NuAudio": ("NuAudio",),
    "NuContPak": ("NuContPak",),
    "NuFont": ("NuFont",),
    "Nu": ("Nu",),
}


def load_crashwoc_syms(elf_path: Path) -> dict[int, list[str]]:
    """Returns size → [name, ...] mapping."""
    data = elf_path.read_bytes()
    e_shoff = struct.unpack_from(">I", data, 0x20)[0]
    e_shentsize = struct.unpack_from(">H", data, 0x2E)[0]
    n_sections = struct.unpack_from(">H", data, 0x30)[0]
    sections = [
        struct.unpack_from(">IIIIIIIIII", data, e_shoff + i * e_shentsize)
        for i in range(n_sections)
    ]
    sym_off = sections[15][4]
    sym_entsize = sections[15][9]
    sym_count = sections[15][5] // sym_entsize
    str_off = sections[16][4]

    by_size: dict[int, list[str]] = {}
    for i in range(sym_count):
        so = sym_off + i * sym_entsize
        st_name, addr, size = struct.unpack_from(">III", data, so)
        info = data[so + 12]
        if (info >> 4) != 1 or size == 0:
            continue
        nm_end = data.index(b"\x00", str_off + st_name)
        name = data[str_off + st_name:nm_end].decode("latin1", errors="replace")
        by_size.setdefault(size, []).append(name)
    return by_size


def parse_symbols() -> list[tuple[int, str, int]]:
    """Returns [(addr, name, size)] for all functions."""
    result = []
    for line in SYMTAB.read_text(errors="replace").splitlines():
        if "type:function" not in line or "=" not in line:
            continue
        name = line.split("=")[0].strip()
        addr_str = line.split(":0x")[1].split(";")[0].strip() if ":0x" in line else ""
        size_str = line.split("size:")[1].split(";")[0].strip() if "size:" in line else "0"
        try:
            addr = int(addr_str, 16)
            size = int(size_str, 16)
        except ValueError:
            continue
        result.append((addr, name, size))
    result.sort()
    return result


def guess_section(addr: int) -> str:
    """Guess the SDK section from address (rough heuristics from known islands)."""
    # These ranges were established from sdk_islands.tsv analysis
    ranges = [
        (0x80150000, 0x80158000, "OS"),
        (0x80158000, 0x80168000, "OS"),
        (0x80168000, 0x80170000, "DVD"),
        (0x80170000, 0x80176000, "AX"),
        (0x80176000, 0x801A0000, "GX"),
        (0x801A0000, 0x801B0000, "NuAudio"),
        (0x80000000, 0x80020000, "Nu"),
    ]
    for start, end, label in ranges:
        if start <= addr < end:
            return label
    return ""


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("-o", "--output", help="Write batch file here instead of stdout")
    ap.add_argument("--apply", action="store_true", help="Apply via batch_rename.py after generating")
    ap.add_argument("--section", help="Only consider this section (e.g. GX, OS)")
    ap.add_argument("--min-size", type=lambda x: int(x, 0), default=0x4, help="Min function size")
    ap.add_argument("--max-size", type=lambda x: int(x, 0), default=0x400, help="Max function size")
    args = ap.parse_args()

    if not DONOR_ELF.exists():
        print(f"Donor ELF not found: {DONOR_ELF}", file=sys.stderr)
        sys.exit(1)

    print("Loading crashwoc ELF...", file=sys.stderr)
    by_size = load_crashwoc_syms(DONOR_ELF)
    print("Loading symbols.txt...", file=sys.stderr)
    funcs = parse_symbols()

    # Build set of already-named functions to skip
    named = {name for _, name, _ in funcs if not name.startswith("fn_")}

    lines = []
    skipped_conflict = 0
    skipped_multi = 0
    skipped_none = 0

    # Nu2 prefixes are safe anywhere; SDK prefixes only in SDK address range
    NU_PREFIXES = PREFIX_FILTERS.get("Nu", ()) + PREFIX_FILTERS.get("NuAnim", ()) + \
                  PREFIX_FILTERS.get("NuAudio", ()) + PREFIX_FILTERS.get("NuFont", ()) + \
                  PREFIX_FILTERS.get("NuContPak", ())
    SDK_PREFIXES = tuple(p for k, prefs in PREFIX_FILTERS.items()
                         if k not in ("Nu", "NuAnim", "NuAudio", "NuFont", "NuContPak")
                         for p in prefs)
    SDK_ADDR_RANGE = (0x80140000, 0x80200000)

    for addr, name, size in funcs:
        if not name.startswith("fn_"):
            continue
        if size < args.min_size or size > args.max_size:
            continue

        section = guess_section(addr)
        if args.section and section != args.section:
            continue

        candidates = by_size.get(size, [])

        # Filter candidates: Nu2 names allowed anywhere; SDK names only in SDK addr range
        in_sdk_range = SDK_ADDR_RANGE[0] <= addr < SDK_ADDR_RANGE[1]
        if in_sdk_range:
            prefixes = PREFIX_FILTERS.get(section, SDK_PREFIXES)
            candidates = [c for c in candidates if any(c.startswith(p) for p in prefixes)]
        else:
            # Game code range: only Nu2 names (SDK names at these addresses = false positive)
            candidates = [c for c in candidates if any(c.startswith(p) for p in NU_PREFIXES)]

        if len(candidates) == 0:
            skipped_none += 1
            continue
        if len(candidates) > 1:
            skipped_multi += 1
            continue

        new_name = candidates[0]
        if new_name in named:
            skipped_conflict += 1
            continue

        named.add(new_name)
        lines.append(f"{name} {new_name}  # 0x{addr:08X} size=0x{size:X} [{section}]")

    output = "\n".join(
        ["# SDK auto-match candidates (single crashwoc size match per section)"]
        + lines
    ) + "\n"

    if args.output:
        Path(args.output).parent.mkdir(parents=True, exist_ok=True)
        Path(args.output).write_text(output)
        print(f"Wrote {len(lines)} candidates to {args.output}", file=sys.stderr)
    else:
        print(output)

    print(
        f"Stats: {len(lines)} candidates | "
        f"{skipped_none} no match | {skipped_multi} ambiguous | {skipped_conflict} conflict",
        file=sys.stderr,
    )

    if args.apply and args.output and lines:
        print("Applying via batch_rename.py...", file=sys.stderr)
        result = subprocess.run(
            [sys.executable, str(ROOT / "tools/batch_rename.py"), args.output],
            cwd=ROOT,
        )
        sys.exit(result.returncode)


if __name__ == "__main__":
    main()
