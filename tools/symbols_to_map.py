#!/usr/bin/env python3
"""Generate a Dolphin-compatible .map file from symbols.txt.

Reads config/GL5E4F/symbols.txt and outputs a .map file that Dolphin can load
via Symbols → Load Symbol Map.

Usage:
    python tools/symbols_to_map.py
    python tools/symbols_to_map.py -o path/to/GL5E4F.map
    python tools/symbols_to_map.py --all   # include unnamed functions as zz_ placeholders
"""
import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"
DEFAULT_OUT = ROOT / "config/GL5E4F/GL5E4F.map"


def parse_symbols(path: Path, include_unnamed: bool) -> list[tuple[int, int, str]]:
    funcs = []
    for line in path.read_text(errors="replace").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        if "type:function" not in line or "=" not in line:
            continue
        name = line.split("=")[0].strip()
        if not include_unnamed and name.startswith("fn_"):
            continue
        addr_str = line.split(":0x")[1].split(";")[0].strip() if ":0x" in line else ""
        size_str = line.split("size:")[1].split(";")[0].strip() if "size:" in line else "0"
        try:
            addr = int(addr_str, 16)
            size = int(size_str, 16)
        except ValueError:
            continue
        if size == 0:
            continue
        # Dolphin sorts symbols by name in several views. Keep anonymous
        # functions visible without interleaving them with recovered names.
        # This matches the zz_ convention used by the external map supplied
        # for this build.
        if include_unnamed and name.startswith("fn_"):
            name = f"zz_{addr:08x}_"
        funcs.append((addr, size, name))
    funcs.sort()
    return funcs


def write_map(funcs: list[tuple[int, int, str]], out: Path) -> None:
    lines = [".text section layout\n"]
    for addr, size, name in funcs:
        lines.append(f"{addr:08x} {size:06x} {addr:08x} 0 {name}\n")
    lines.append("\n.data section layout\n\n.note section layout\n")
    out.write_text("".join(lines))


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("-o", "--output", default=str(DEFAULT_OUT),
                    help=f"Output .map path (default: {DEFAULT_OUT})")
    ap.add_argument("--all", action="store_true",
                    help="Include unnamed fn_ placeholders as zz_<address>_ entries")
    ap.add_argument("--input", default=str(SYMTAB),
                    help=f"Input symbols.txt (default: {SYMTAB})")
    args = ap.parse_args()

    src = Path(args.input)
    dst = Path(args.output)

    funcs = parse_symbols(src, include_unnamed=args.all)
    named = sum(1 for _, _, n in funcs if not n.startswith("zz_"))
    write_map(funcs, dst)

    total = len(funcs)
    print(f"Wrote {total} functions ({named} named) to {dst}", file=sys.stderr)


if __name__ == "__main__":
    main()
