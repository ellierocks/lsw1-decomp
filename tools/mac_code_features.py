"""Compiler-independent features of PowerPC functions, for GC (ProDG) and Mac (CodeWarrior) alike.

Both builds are PowerPC from the same source, so the *code* differs (registers, scheduling,
PIC addressing) but the facts the source dictates survive: which functions are called, the
integer immediates, the struct-field offsets loaded and stored, the float literals, the
strings and the named globals referenced.  `scan` decodes one function's words with a
linear constant-propagation over `lis/addis/addi/ori` (and the Mac `bcl; mflr` PIC base),
which resolves every data reference to an absolute address, and returns those facts.

Used by tools/mac_code_match.py; has no repo dependencies.
"""
from __future__ import annotations

import math
import re
import struct
import subprocess
from dataclasses import dataclass, field
from pathlib import Path

MASK = 0xFFFFFFFF


@dataclass
class Section:
    name: str
    addr: int
    size: int
    off: int  # file offset; -1 for zero-fill
    kind: str  # text | data | bss


class Image:
    """A loaded address space: sections plus byte lookup."""

    def __init__(self, blob: bytes, sections: list[Section]):
        self.blob = blob
        self.sections = sorted(sections, key=lambda s: s.addr)

    def section_of(self, addr: int) -> Section | None:
        for s in self.sections:
            if s.addr <= addr < s.addr + s.size:
                return s
        return None

    def read(self, addr: int, n: int) -> bytes | None:
        s = self.section_of(addr)
        if s is None or s.off < 0 or addr + n > s.addr + s.size:
            return None
        o = s.off + addr - s.addr
        return self.blob[o:o + n]

    def cstring(self, addr: int) -> str | None:
        """The C string at ADDR in a data section (3-200 printable chars), else None."""
        s = self.section_of(addr)
        if s is None or s.kind != "data" or s.off < 0:
            return None
        o = s.off + addr - s.addr
        end = self.blob.find(b"\0", o, min(o + 200, s.off + s.size))
        if end < o + 3:
            return None
        raw = self.blob[o:end]
        if all(32 <= c < 127 or c in (9, 10) for c in raw):
            return raw.decode("ascii")
        return None


def load_dol(path: Path) -> tuple[Image, int, int]:
    """(image, r13 SDA base, r2 SDA2 base) of a GameCube DOL."""
    blob = path.read_bytes()
    w = struct.unpack(">64I", blob[:0x100])
    secs = []
    for i in range(18):
        off, addr, size = w[i], w[18 + i], w[36 + i]
        if size:
            secs.append(Section(f"{'text' if i < 7 else 'data'}{i}", addr, size, off, "text" if i < 7 else "data"))
    bss_addr, bss_size = w[54], w[55]
    secs.append(Section("bss", bss_addr, bss_size, -1, "bss"))
    img = Image(blob, secs)
    # __start loads the SDA bases with lis/ori pairs: r13 first, then r2.
    consts: dict[int, int] = {}
    entry = w[56]
    for i in range(40):
        raw = img.read(entry + 4 * i, 4)
        if not raw:
            break
        v = struct.unpack(">I", raw)[0]
        op, rt, ra, imm = v >> 26, (v >> 21) & 31, (v >> 16) & 31, v & 0xFFFF
        if op == 15 and ra == 0:
            consts[rt] = imm << 16
        elif op == 24 and rt in consts and ra == rt:
            consts[ra] = consts[rt] | imm
    return img, consts[13], consts[2]


def load_macho(path: Path) -> Image:
    """A 32-bit big-endian Mach-O (thin PPC) as an Image."""
    blob = path.read_bytes()
    magic, _, _, _, ncmds, _, _ = struct.unpack(">7I", blob[:28])
    if magic != 0xFEEDFACE:
        raise SystemExit(f"{path}: not a thin 32-bit big-endian Mach-O")
    off, secs = 28, []
    for _ in range(ncmds):
        cmd, size = struct.unpack(">II", blob[off:off + 8])
        if cmd == 1:  # LC_SEGMENT
            nsects = struct.unpack(">I", blob[off + 48:off + 52])[0]
            for k in range(nsects):
                o = off + 56 + 68 * k
                name = blob[o:o + 16].rstrip(b"\0").decode()
                addr, ssize, foff = struct.unpack(">III", blob[o + 32:o + 44])
                flags = struct.unpack(">I", blob[o + 56:o + 60])[0]
                typ = flags & 0xFF
                if name == "__text":
                    kind = "text"
                elif typ in (1, 12):  # zerofill sections
                    kind = "bss"
                elif "stub" in name:
                    kind = "text"
                else:
                    kind = "data"
                secs.append(Section(name, addr, ssize, -1 if kind == "bss" else foff, kind))
        off += size
    return Image(blob, secs)


# --- PowerPC decoding ---------------------------------------------------------------------

# X-form (primary opcode 31) extended opcodes by where the result lands.
NO_DEST = {0, 32, 4, 144, 146, 467, 598, 150, 151, 215, 407, 727, 663, 247, 183, 439, 695, 759, 662, 918,
           661, 725, 983, 86, 54, 246, 278, 470, 982, 1014, 214, 566, 854, 310}
RA_DEST = {28, 60, 124, 284, 316, 412, 444, 476, 24, 536, 792, 824, 26, 922, 954}
# D-form: load width/kind by primary opcode.
LOADS = {32: ("w", 4), 33: ("w", 4), 34: ("b", 1), 35: ("b", 1), 40: ("h", 2), 41: ("h", 2),
         42: ("h", 2), 43: ("h", 2), 48: ("fs", 4), 49: ("fs", 4), 50: ("fd", 8), 51: ("fd", 8)}
STORES = {36: ("w", 4), 37: ("w", 4), 38: ("b", 1), 39: ("b", 1), 44: ("h", 2), 45: ("h", 2),
          52: ("fs", 4), 53: ("fs", 4), 54: ("fd", 8), 55: ("fd", 8)}
UPDATE = {33, 35, 37, 39, 41, 43, 45, 49, 51, 53, 55}
FP_LOADS = {48, 49, 50, 51}


def sext16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


@dataclass
class Facts:
    """What one function does, in compiler-independent terms."""
    calls: list[int] = field(default_factory=list)  # bl targets in code order
    tails: list[int] = field(default_factory=list)  # plain `b` targets outside the function
    imms: set[int] = field(default_factory=set)
    fields: set[tuple[str, int]] = field(default_factory=set)
    floats: set[str] = field(default_factory=set)
    strings: set[str] = field(default_factory=set)
    dsyms: set[str] = field(default_factory=set)  # named globals referenced
    shifts: set[tuple[int, int, int]] = field(default_factory=set)
    ninsn: int = 0


def float_key(v: float) -> str:
    if v != v or math.isinf(v):
        return "nan"
    return f"{v:.6g}"


def scan(img: Image, start: int, size: int, *, sda: tuple[int, int] | None = None, symname=None) -> Facts:
    """Decode the SIZE bytes at START.  SDA is (r13, r2) for GC code; Mac PIC bases come from bcl/mflr.
    SYMNAME(addr) -> name of the global containing addr, or None."""
    f = Facts()
    raw = img.read(start, size)
    if raw is None:
        return f
    n = len(raw) // 4
    f.ninsn = n
    words = struct.unpack(f">{n}I", raw[:4 * n])
    reg: dict[int, int] = {}
    if sda:
        reg[13], reg[2] = sda
    pic_next = None
    end = start + 4 * n

    def finish(r: int, val: int):
        """REG just became VAL via a composed (low-part) instruction: note strings and globals."""
        reg[r] = val & MASK
        v = val & MASK
        s = img.cstring(v)
        if s is not None:
            f.strings.add(s)
        if symname is not None and (nm := symname(v)):
            f.dsyms.add(nm)

    for i, w in enumerate(words):
        addr = start + 4 * i
        op = w >> 26
        rt, ra, rb = (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
        imm = w & 0xFFFF
        simm = sext16(imm)
        if op == 18:  # b/bl/ba
            li = w & 0x03FFFFFC
            if li & 0x02000000:
                li -= 0x04000000
            target = li if w & 2 else (addr + li) & MASK
            if w & 1:
                f.calls.append(target)
            elif not (start <= target < end):
                f.tails.append(target)
        elif op == 16:  # bc; `bcl 20,31,.+4` fetches the PC
            if w & 1 and rt == 20 and ra == 31:
                pic_next = addr + 4
        elif op == 14:  # addi / li
            if ra == 0:
                reg[rt] = simm & MASK
                f.imms.add(simm)
            elif ra in reg:
                finish(rt, reg[ra] + simm)
            else:
                reg.pop(rt, None)
                if ra != 1 and simm:
                    f.imms.add(simm)
        elif op == 15:  # addis / lis
            if ra == 0:
                reg[rt] = (simm << 16) & MASK
            elif ra in reg:
                reg[rt] = (reg[ra] + (simm << 16)) & MASK
            else:
                reg.pop(rt, None)
        elif op in (24, 25):  # ori / oris (dest ra)
            if rt in reg:
                finish(ra, reg[rt] | (imm << (16 if op == 25 else 0)))
            else:
                reg.pop(ra, None)
                f.imms.add(imm if op == 24 else imm << 16)
        elif op in (26, 27, 28, 29):  # xori xoris andi. andis.
            reg.pop(ra, None)
            if op in (26, 28):
                f.imms.add(imm)
        elif op in (10, 11):  # cmpli / cmpi
            f.imms.add(simm if op == 11 else imm)
        elif op in (7, 8, 12, 13):  # mulli subfic addic addic.
            reg.pop(rt, None)
            if op == 7:
                f.imms.add(simm)
        elif op in LOADS or op in STORES:
            kind, width = (LOADS.get(op) or STORES.get(op))
            if ra == 1:
                pass  # stack slot
            elif ra in reg and not (ra == 0):
                eff = (reg[ra] + simm) & MASK
                if op in FP_LOADS:
                    data = img.read(eff, width)
                    if data:
                        v = struct.unpack(">f" if width == 4 else ">d", data)[0]
                        f.floats.add(float_key(v))
                if symname is not None and (nm := symname(eff)):
                    f.dsyms.add(nm)
            elif ra != 0:
                f.fields.add((kind, simm))
            if op in LOADS:
                reg.pop(rt, None)
            if op in UPDATE:
                reg.pop(ra, None)
        elif op in (46, 47):  # lmw / stmw
            if op == 46:
                for r in range(rt, 32):
                    reg.pop(r, None)
        elif op in (20, 21, 23):  # rlwimi rlwinm rlwnm
            reg.pop(ra, None)
            if op == 21:
                f.shifts.add((rb, (w >> 6) & 31, (w >> 1) & 31))
        elif op == 31:
            xo = (w >> 1) & 0x3FF
            if xo == 339:  # mfspr
                spr = ((w >> 16) & 31) | (((w >> 11) & 31) << 5)
                if spr == 8 and pic_next is not None:
                    reg[rt] = pic_next
                    pic_next = None
                else:
                    reg.pop(rt, None)
            elif xo == 444 and rt == rb:  # mr ra,rs
                if rt in reg:
                    reg[ra] = reg[rt]
                else:
                    reg.pop(ra, None)
            elif xo in RA_DEST:
                reg.pop(ra, None)
            elif xo not in NO_DEST:
                reg.pop(rt, None)
        elif op in (56, 57, 60, 61, 4, 59, 63, 19, 17, 2, 3):
            pass
        else:
            pass
    return f


def demangle_all(names: list[str]) -> list[str]:
    """llvm-cxxfilt on NAMES (one per line); falls back to the input on failure."""
    r = subprocess.run(["llvm-cxxfilt"], input="\n".join(names) + "\n", capture_output=True, text=True)
    out = r.stdout.splitlines()
    return out if len(out) == len(names) else list(names)


MANGLED = re.compile(r"^_?_Z(\d+)([A-Za-z_]\w*)$")
IDENT = re.compile(r"^[A-Za-z_]\w*$")


def base_name(raw: str) -> str | None:
    """The C-level name of a Mac symbol (`__Z15MusicSeekOffsetif` -> MusicSeekOffset), or None for
    class members, templates and system symbols that no GC function can share."""
    m = MANGLED.match(raw)
    if m:
        length = int(m.group(1))
        body = m.group(2)
        return body[:length] if len(body) >= length else None
    if raw.startswith("__Z") or raw.startswith("_Z"):
        return None
    name = raw[1:] if raw.startswith("_") else raw
    if raw.startswith("__") or not IDENT.match(name):
        return None
    return name
