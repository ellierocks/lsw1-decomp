#!/usr/bin/env python3
"""Inspect LSW1 (GL5E4F) game state from raw Dolphin MEM1 RAM dumps.

The decomp work is mostly static: we infer struct layouts, data addresses and
`fn_` purposes from ASM alone. A raw RAM dump captured from a running game turns
those guesses into ground truth -- every GC address resolves to real bytes.

The intended workflow is "big-picture snapshots": dump MEM1 at meaningful game
states (title screen, hub, shop, a specific level, a status screen), then:
  * read typed values at an address (`read`, `sym`),
  * name an address or a pointer value (`whatis`) -- e.g. resolve a vtable
    pointer to the class it belongs to,
  * DIFF two snapshots (`diff`) to discover which globals change between
    states -- the fastest way to locate live state variables,
  * scavenge ASCII strings (`strings`).

Dump format: a raw little-endian-free MEM1 image. GC MEM1 starts at
0x80000000 and is 24 MiB (0x1800000). A standard Dolphin RAM dump is exactly
that blob, so address 0x80000000 == file offset 0. The console is big-endian,
so all multi-byte values are decoded big-endian.

Dumps are large (24 MiB); keep them out of git. By default this looks for them
under ./dumps/ and supports a label manifest (dumps/manifest.txt: "label = file"
per line) so you can say `read title 0x80407B40` instead of a path.

Examples:
    python tools/ramdump.py read dumps/hub.raw 0x80407B40 --type f32 --count 4
    python tools/ramdump.py sym dumps/hub.raw NuRandSeed
    python tools/ramdump.py whatis dumps/hub.raw 0x8000C700
    python tools/ramdump.py diff dumps/title.raw dumps/level1.raw --section sdata
    python tools/ramdump.py strings dumps/shop.raw --range 0x80400000 0x80410000
"""
import argparse
import bisect
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
DUMPS_DIR = ROOT / "dumps"

MEM1_BASE = 0x80000000
MEM1_SIZE = 0x1800000  # 24 MiB

# Name = .section:0xADDR; // type:... size:0xSZ
SYM_LINE = re.compile(
    r"^([A-Za-z_$][\w$]*)\s*=\s*\.(\w+):0x([0-9A-Fa-f]+)\s*;"
    r"(?:.*?size:0x([0-9A-Fa-f]+))?"
)

# struct.unpack format + byte width per type. All big-endian ('>').
SCALARS = {
    "u8": ("B", 1), "s8": ("b", 1),
    "u16": (">H", 2), "s16": (">h", 2),
    "u32": (">I", 4), "s32": (">i", 4),
    "f32": (">f", 4), "f64": (">d", 8),
    "ptr": (">I", 4),
}


# --------------------------------------------------------------------------- #
# Symbol index
# --------------------------------------------------------------------------- #
class SymbolIndex:
    """Address-sorted view of symbols.txt for two-way name<->addr lookup."""

    def __init__(self, path=SYMBOLS):
        self.by_name = {}
        entries = []
        for line in path.read_text(errors="replace").splitlines():
            m = SYM_LINE.match(line.strip())
            if not m:
                continue
            name, section, addr_s, size_s = m.groups()
            addr = int(addr_s, 16)
            size = int(size_s, 16) if size_s else 0
            entries.append((addr, name, section, size))
            self.by_name[name] = (addr, section, size)
        entries.sort()
        self._addrs = [e[0] for e in entries]
        self._entries = entries

    def resolve(self, name):
        if name not in self.by_name:
            sys.exit(f"error: symbol '{name}' not found in symbols.txt")
        return self.by_name[name]  # (addr, section, size)

    def nearest(self, addr):
        """Return (name, section, sym_addr, size, offset) for the symbol that
        covers/precedes addr, or None."""
        i = bisect.bisect_right(self._addrs, addr) - 1
        if i < 0:
            return None
        sym_addr, name, section, size = self._entries[i]
        return (name, section, sym_addr, size, addr - sym_addr)


# --------------------------------------------------------------------------- #
# Dump access
# --------------------------------------------------------------------------- #
class Dump:
    def __init__(self, path, base=MEM1_BASE):
        self.path = path
        self.base = base
        self.data = path.read_bytes()

    def offset(self, addr):
        # Accept cached (0x80xxxxxx) and uncached (0xC0xxxxxx) MEM1 mirrors,
        # and a raw file offset (addr < base).
        if addr >= 0xC0000000:
            addr = (addr & 0x01FFFFFF) | MEM1_BASE
        off = addr - self.base if addr >= self.base else addr
        if not (0 <= off < len(self.data)):
            sys.exit(f"error: 0x{addr:08X} (offset 0x{off:X}) outside dump "
                     f"[0x{self.base:08X}..0x{self.base + len(self.data):08X})")
        return off

    def read(self, addr, n):
        off = self.offset(addr)
        if off + n > len(self.data):
            sys.exit(f"error: read of {n} bytes at 0x{addr:08X} runs past dump end")
        return self.data[off:off + n]

    def scalar(self, addr, kind):
        fmt, width = SCALARS[kind]
        return struct.unpack(fmt, self.read(addr, width))[0]


def resolve_dump_path(arg):
    """A path, or a label looked up in dumps/manifest.txt."""
    p = Path(arg)
    if p.is_file():
        return p
    cand = DUMPS_DIR / arg
    if cand.is_file():
        return cand
    manifest = DUMPS_DIR / "manifest.txt"
    if manifest.is_file():
        for line in manifest.read_text().splitlines():
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            label, _, path = line.partition("=")
            if label.strip() == arg:
                mp = Path(path.strip())
                return mp if mp.is_absolute() else (DUMPS_DIR / mp)
    sys.exit(f"error: dump '{arg}' not found (no such file, and no matching "
             f"label in {manifest})")


def parse_addr(s):
    try:
        return int(s, 0)
    except ValueError:
        sys.exit(f"error: bad address '{s}'")


# --------------------------------------------------------------------------- #
# Formatting
# --------------------------------------------------------------------------- #
def annotate(syms, value):
    """If a 32-bit value looks like a MEM1 pointer, name what it points at."""
    if syms is None or not (MEM1_BASE <= value < MEM1_BASE + MEM1_SIZE):
        return ""
    near = syms.nearest(value)
    if not near:
        return ""
    name, section, _, size, off = near
    within = size == 0 or off < size
    if not within:
        return ""
    return f"  -> {name}{f'+0x{off:X}' if off else ''} ({section})"


def fmt_scalar(kind, v):
    if kind == "f32" or kind == "f64":
        return f"{v:.6g}"
    if kind == "ptr" or kind.startswith("u"):
        width = SCALARS[kind][1]
        return f"0x{v:0{width * 2}X}"
    return str(v)


# --------------------------------------------------------------------------- #
# Subcommands
# --------------------------------------------------------------------------- #
def read_typed(dump, syms, addr, kind, count):
    """Read scalars / vec3 / vec4 / mtx and print with pointer annotations."""
    if kind in ("vec3", "vec4"):
        n = 3 if kind == "vec3" else 4
        for i in range(count):
            base = addr + i * n * 4
            comps = [dump.scalar(base + j * 4, "f32") for j in range(n)]
            print(f"0x{base:08X}  {kind}  (" +
                  ", ".join(f"{c:.6g}" for c in comps) + ")")
        return
    if kind in ("mtx", "mtx44", "mtx34"):
        rows = 3 if kind == "mtx34" else 4
        for r in range(rows):
            base = addr + r * 16
            cols = [dump.scalar(base + c * 4, "f32") for c in range(4)]
            print(f"0x{base:08X}  [" +
                  "  ".join(f"{c:9.4f}" for c in cols) + "]")
        return
    if kind == "cstr":
        raw = dump.read(addr, 256)
        s = raw.split(b"\x00", 1)[0]
        print(f"0x{addr:08X}  cstr  {s.decode('latin-1')!r}")
        return
    if kind == "hex":
        hexdump(dump, addr, count if count > 1 else 64)
        return
    # scalar
    _, width = SCALARS[kind]
    for i in range(count):
        a = addr + i * width
        v = dump.scalar(a, kind)
        note = annotate(syms, v) if kind == "ptr" else ""
        print(f"0x{a:08X}  {kind:<4} {fmt_scalar(kind, v)}{note}")


def hexdump(dump, addr, length):
    data = dump.read(addr, length)
    for i in range(0, len(data), 16):
        chunk = data[i:i + 16]
        hexs = " ".join(f"{b:02X}" for b in chunk)
        ascii_ = "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)
        print(f"0x{addr + i:08X}  {hexs:<47}  {ascii_}")


def cmd_read(args, syms):
    dump = Dump(resolve_dump_path(args.dump), base=args.base)
    read_typed(dump, syms, parse_addr(args.addr), args.type, args.count)


def cmd_dump(args, syms):
    dump = Dump(resolve_dump_path(args.dump), base=args.base)
    hexdump(dump, parse_addr(args.addr), args.len)


def cmd_sym(args, syms):
    dump = Dump(resolve_dump_path(args.dump), base=args.base)
    addr, section, size = syms.resolve(args.symbol)
    print(f"{args.symbol} = 0x{addr:08X}  (.{section}, size 0x{size:X})")
    if args.type:
        read_typed(dump, syms, addr, args.type, args.count)
    else:
        hexdump(dump, addr, size or 64)


def cmd_whatis(args, syms):
    addr = parse_addr(args.addr)
    near = syms.nearest(addr)
    if not near:
        print(f"0x{addr:08X}: no preceding symbol")
        return
    name, section, sym_addr, size, off = near
    within = "" if (size and off < size) else "  (PAST symbol end -- gap/unknown)"
    print(f"0x{addr:08X} = {name}{f'+0x{off:X}' if off else ''}  "
          f"(.{section}, sym @0x{sym_addr:08X}, size 0x{size:X}){within}")


def cmd_diff(args, syms):
    a = Dump(resolve_dump_path(args.a), base=args.base)
    b = Dump(resolve_dump_path(args.b), base=args.base)
    lo, hi = _diff_range(args, syms)
    width = {"u8": 1, "u16": 2, "u32": 4, "f32": 4}[args.type]
    fmt = SCALARS[args.type][0]

    print(f"diff {a.path.name} vs {b.path.name}  "
          f"[0x{lo:08X}..0x{hi:08X}) as {args.type}")
    changes = 0
    addr = lo
    last_sym = None
    while addr + width <= hi:
        oa, ob = a.offset(addr), b.offset(addr)
        ba, bb = a.data[oa:oa + width], b.data[ob:ob + width]
        if ba != bb:
            va = struct.unpack(fmt, ba)[0]
            vb = struct.unpack(fmt, bb)[0]
            near = syms.nearest(addr)
            label = ""
            if near:
                nm, _sec, sa, sz, off = near
                if sz == 0 or off < sz:
                    label = f"  {nm}{f'+0x{off:X}' if off else ''}"
                    if nm != last_sym:
                        last_sym = nm
            changes += 1
            if changes <= args.limit:
                print(f"  0x{addr:08X}  {fmt_scalar(args.type, va):>14} -> "
                      f"{fmt_scalar(args.type, vb):<14}{label}")
        addr += width
    extra = f" (showing first {args.limit})" if changes > args.limit else ""
    print(f"\n{changes} differing {args.type} cells{extra}")


def _diff_range(args, syms):
    if args.range:
        return parse_addr(args.range[0]), parse_addr(args.range[1])
    if args.section:
        # bound the diff to the address span of symbols in a given section
        addrs = [a for a, _n, s, _sz in syms._entries if s == args.section]
        if not addrs:
            sys.exit(f"error: no symbols in section .{args.section}")
        return min(addrs), max(addrs) + 0x40
    return MEM1_BASE, MEM1_BASE + MEM1_SIZE


def cmd_strings(args, syms):
    dump = Dump(resolve_dump_path(args.dump), base=args.base)
    lo = parse_addr(args.range[0]) if args.range else dump.base
    hi = parse_addr(args.range[1]) if args.range else dump.base + len(dump.data)
    data = dump.data
    start = None
    run = bytearray()
    addr = lo
    while addr < hi:
        b = data[dump.offset(addr)]
        if 32 <= b < 127:
            if start is None:
                start = addr
            run.append(b)
        else:
            if start is not None and len(run) >= args.min:
                print(f"0x{start:08X}  {run.decode('latin-1')!r}")
            start, run = None, bytearray()
        addr += 1
    if start is not None and len(run) >= args.min:
        print(f"0x{start:08X}  {run.decode('latin-1')!r}")


# --------------------------------------------------------------------------- #
def main():
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--base", type=lambda s: int(s, 0), default=MEM1_BASE,
                    help="MEM1 base address (default 0x80000000)")
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("read", help="read typed value(s) at an address")
    p.add_argument("dump"); p.add_argument("addr")
    p.add_argument("--type", default="hex",
                   choices=list(SCALARS) + ["vec3", "vec4", "mtx", "mtx44",
                                            "mtx34", "cstr", "hex"])
    p.add_argument("--count", type=int, default=1)
    p.set_defaults(func=cmd_read)

    p = sub.add_parser("dump", help="hexdump a region")
    p.add_argument("dump"); p.add_argument("addr")
    p.add_argument("--len", type=lambda s: int(s, 0), default=64)
    p.set_defaults(func=cmd_dump)

    p = sub.add_parser("sym", help="resolve a symbol name and read it")
    p.add_argument("dump"); p.add_argument("symbol")
    p.add_argument("--type", choices=list(SCALARS) + ["vec3", "vec4", "mtx",
                   "mtx44", "mtx34", "cstr", "hex"])
    p.add_argument("--count", type=int, default=1)
    p.set_defaults(func=cmd_sym)

    p = sub.add_parser("whatis", help="name the symbol covering an address")
    p.add_argument("addr")
    p.set_defaults(func=cmd_whatis)

    p = sub.add_parser("diff", help="find cells that differ between two dumps")
    p.add_argument("a"); p.add_argument("b")
    p.add_argument("--type", default="u32", choices=["u8", "u16", "u32", "f32"])
    p.add_argument("--range", nargs=2, metavar=("LO", "HI"))
    p.add_argument("--section", help="bound diff to a symbols.txt section "
                   "(e.g. sdata, data, bss, sbss)")
    p.add_argument("--limit", type=int, default=200)
    p.set_defaults(func=cmd_diff)

    p = sub.add_parser("strings", help="list ASCII strings in a region")
    p.add_argument("dump")
    p.add_argument("--range", nargs=2, metavar=("LO", "HI"))
    p.add_argument("--min", type=int, default=4)
    p.set_defaults(func=cmd_strings)

    args = ap.parse_args()
    syms = SymbolIndex() if SYMBOLS.is_file() else None
    if syms is None and args.cmd in ("sym", "whatis"):
        sys.exit("error: symbols.txt not found")
    args.func(args, syms)


if __name__ == "__main__":
    main()
