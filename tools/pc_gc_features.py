#!/usr/bin/env python3
"""
Per-function features of the GC DOL and the Japanese PC 1.0 exe for pairing.

Used by tools/pc_gc_anchors.py. Both sides yield, per function:
  refs    ordered distinct data addresses it reads, writes or takes
          (GC: lis pairs and r13/r2 small-data accesses, mapped to the
          containing symbol; PC: absolute operands in .rdata/.data/.bss)
  floats  floating-point constants it loads (GC lfs/lfd; PC x87 memory operands)
plus the code pointers stored in data (function-pointer tables).

The PC side disassembles with the system `objdump` (binutils with pei-i386
support), which also gives pc_gc_anchors exact function starts and calls.
Without it, PC refs and floats are empty, the steps that need them pair
nothing, and starts fall back to a byte scan.
"""

from __future__ import annotations

import bisect
import re
import shutil
import struct
import subprocess
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

from string_xrefs import DOL, SYMBOLS, parse_dol_sections, parse_symbols

SDA_BASE = 0x8040FA40   # r13
SDA2_BASE = 0x8041FA40  # r2
# D-form loads, stores and addi that take a base register.
D_FORM = set(range(32, 56)) | {14}
D_STORES = {36, 37, 38, 39, 44, 45, 47, 52, 53, 54, 55}
FLOAT_LOADS = {48: 4, 49: 4, 50: 8, 51: 8}


@dataclass
class Features:
    refs: dict = field(default_factory=dict)     # function -> [data address]
    floats: dict = field(default_factory=dict)   # function -> [value]
    pointers: list = field(default_factory=list)  # [(data address, function)]


def _s16(value: int) -> int:
    return value - 0x10000 if value & 0x8000 else value


def gc_features() -> Features:
    """Data refs, float constants and code pointers of configured GC functions."""
    data = DOL.read_bytes()
    sections = parse_dol_sections(DOL)
    text = next(s for s in sections if s.name == ".text")
    symbols = parse_symbols(SYMBOLS)
    functions = sorted((s for s in symbols if s.section == ".text" and "type:function" in s.meta),
                       key=lambda s: s.address)
    data_syms = sorted((s for s in symbols if s.section not in {".text", ".init"}), key=lambda s: s.address)
    data_starts = [s.address for s in data_syms]

    def read(addr: int, n: int) -> bytes | None:
        for s in sections:
            if s.address <= addr < s.address + s.size:
                return data[s.offset + addr - s.address : s.offset + addr - s.address + n]
        return None

    def owner(addr: int) -> int | None:
        i = bisect.bisect_right(data_starts, addr) - 1
        if i >= 0 and data_syms[i].address <= addr < data_syms[i].address + max(data_syms[i].size, 1):
            return data_syms[i].address
        return None

    out = Features()
    for f in functions:
        high: dict[int, int] = {}
        refs: list[int] = []
        floats: list[float] = []
        for addr in range(f.address, f.address + f.size, 4):
            word = struct.unpack_from(">I", data, text.offset + addr - text.address)[0]
            op, rd, ra, imm = word >> 26, (word >> 21) & 31, (word >> 16) & 31, word & 0xFFFF
            if op == 15 and ra == 0:  # lis
                high[rd] = _s16(imm) << 16
                continue
            target = None
            if op in D_FORM:
                if ra == 13:
                    target = SDA_BASE + _s16(imm)
                elif ra == 2:
                    target = SDA2_BASE + _s16(imm)
                elif ra and ra in high:
                    target = (high[ra] + _s16(imm)) & 0xFFFFFFFF
            elif op == 24 and rd in high:  # ori rA, rS(=rd field), imm
                target = (high[rd] | imm) & 0xFFFFFFFF
            if target is not None and 0x80000000 <= target < 0x81800000:
                if op in FLOAT_LOADS:
                    raw = read(target, FLOAT_LOADS[op])
                    if raw and len(raw) == FLOAT_LOADS[op]:
                        floats.append(struct.unpack(">f" if FLOAT_LOADS[op] == 4 else ">d", raw)[0])
                else:
                    sym = owner(target)
                    if sym is not None and sym not in refs:
                        refs.append(sym)
            if op in D_FORM and op not in D_STORES and rd in high and not (op == 14 and rd == ra):
                del high[rd]
        out.refs[f.name] = refs
        out.floats[f.name] = floats

    starts = {f.address: f.name for f in functions}
    for s in sections:
        if s.name in {".data", ".rodata", ".sdata"}:
            blob = data[s.offset : s.offset + s.size]
            for off in range(0, len(blob) - 3, 4):
                value = struct.unpack_from(">I", blob, off)[0]
                if value in starts:
                    out.pointers.append((s.address + off, starts[value]))
    return out


def pe_layout(data: bytes) -> dict[str, tuple[int, int, int, int]]:
    """Return {section: (va, virtual size, raw offset, raw size)}."""
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    count = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    out = {}
    for i in range(count):
        entry = pe + 24 + opt_size + 40 * i
        name = data[entry : entry + 8].rstrip(b"\0").decode("ascii", "replace")
        vsize, rva, raw_size, raw_off = struct.unpack_from("<IIII", data, entry + 8)
        out[name] = (base + rva, vsize, raw_off, raw_size)
    return out


LINE_RE = re.compile(r"^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$")
HEX_RE = re.compile(r"0x([0-9a-f]+)")


def parse_x86(listing: str, starts: list[int], data_lo: int, data_hi: int, rdata: tuple[int, int],
              read) -> tuple[dict, dict]:
    """Return ({function: [data address]}, {function: [float]}) from objdump -M intel text."""
    refs: dict[int, list[int]] = defaultdict(list)
    floats: dict[int, list[float]] = defaultdict(list)
    for line in listing.splitlines():
        m = LINE_RE.match(line)
        if not m:
            continue
        mnemonic, operands = m.group(2), m.group(3)
        if mnemonic.startswith(("call", "j")):
            continue
        i = bisect.bisect_right(starts, int(m.group(1), 16)) - 1
        if i < 0:
            continue
        fn = starts[i]
        for h in HEX_RE.findall(operands):
            value = int(h, 16)
            if not data_lo <= value < data_hi:
                continue
            width = 8 if "QWORD PTR" in operands else 4 if "DWORD PTR" in operands else 0
            if mnemonic.startswith("f") and width and rdata[0] <= value < rdata[1]:
                raw = read(value, width)
                if raw and len(raw) == width:
                    floats[fn].append(struct.unpack("<f" if width == 4 else "<d", raw)[0])
            elif value not in refs[fn]:
                refs[fn].append(value)
    return dict(refs), dict(floats)


def x86_disassembly(exe: Path) -> str | None:
    """objdump -M intel listing of .text, or None without a usable objdump."""
    objdump = shutil.which("objdump")
    if objdump is None:
        return None
    result = subprocess.run([objdump, "-d", "-M", "intel", "--no-show-raw-insn", "-j", ".text", str(exe)],
                            capture_output=True, text=True)
    return result.stdout if result.returncode == 0 and result.stdout else None


CALL_RE = re.compile(r"^\s*([0-9a-f]+):\s+call\s+(?:0x)?([0-9a-f]+)\s*$")


def x86_listing(listing: str) -> tuple[set[int], list[tuple[int, int]]]:
    """Return (instruction addresses, [(call site, direct call target)])."""
    insns: set[int] = set()
    calls: list[tuple[int, int]] = []
    for line in listing.splitlines():
        m = LINE_RE.match(line)
        if not m:
            continue
        insns.add(int(m.group(1), 16))
        c = CALL_RE.match(line)
        if c:
            calls.append((int(c.group(1), 16), int(c.group(2), 16)))
    return insns, calls


def pc_features(exe: Path, starts: list[int], listing: str | None = None) -> Features:
    """Data refs, x87 float constants and code pointers of x86 functions."""
    data = exe.read_bytes()
    sections = pe_layout(data)

    def read(addr: int, n: int) -> bytes | None:
        for va, vsize, raw_off, raw_size in sections.values():
            if va <= addr < va + min(vsize, raw_size):
                return data[raw_off + addr - va : raw_off + addr - va + n]
        return None

    out = Features()
    start_set = set(starts)
    for name in (".rdata", ".data"):
        if name not in sections:
            continue
        va, vsize, raw_off, raw_size = sections[name]
        blob = data[raw_off : raw_off + min(vsize, raw_size)]
        for off in range(0, len(blob) - 3, 4):
            value = struct.unpack_from("<I", blob, off)[0]
            if value in start_set:
                out.pointers.append((va + off, value))

    if listing is None or ".rdata" not in sections or ".data" not in sections:
        return out
    rdata_va, rdata_size = sections[".rdata"][:2]
    data_va, data_size = sections[".data"][:2]
    out.refs, out.floats = parse_x86(listing, starts, rdata_va, data_va + data_size,
                                     (rdata_va, rdata_va + rdata_size), read)
    return out
