#!/usr/bin/env python3
"""Strictly verify one compiled function against the retail target.

Compares the *bytes and relocations* of a function in your compiled object
(build/GL5E4F/src/<unit>.o) with the same function in the DTK-split target
object (build/GL5E4F/obj/**.o). When the retail DOL is present the target bytes
are additionally checked against it, and constants/data referenced through
relocations are compared against the retail data.

Usage:
    python3 tools/verify_fn.py NuRandInt
    python3 tools/verify_fn.py NuVecMtxTransformH --unit numath/nuvec --build
    python3 tools/verify_fn.py Foo --object work/Foo/source.o

Exit codes (stable, for scripts and CI):
    0  MATCH         every instruction word and relocation is identical
    1  MISMATCH      at least one instruction, relocation, length or data differs
    2  ERROR         missing symbol/object, stale object, tool failure, bad input
    3  UNVERIFIED    code is identical but some relocation targets could only be
                     mapped by consistency (only with --strict-symbols)

Mnemonic-level similarity is printed for orientation only; it never counts as a
match. See docs/code_matching_workflow.md for what is and is not verified.
"""
from __future__ import annotations

import argparse
import difflib
import hashlib
import json
import re
import struct
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional

ROOT = Path(__file__).resolve().parent.parent
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
SPLITS = ROOT / "config/GL5E4F/splits.txt"
TARGET_OBJ_DIR = ROOT / "build/GL5E4F/obj"
SRC_OBJ_DIR = ROOT / "build/GL5E4F/src"
SRC_DIR = ROOT / "src"
DTK = ROOT / "build/tools/dtk_patched"
DOL_PATH = ROOT / "orig/GL5E4F/sys/main.dol"
DOL_SHA1 = "95cca08a19224775d1a8d6cc64601fb7d0080981"

EXIT_MATCH, EXIT_MISMATCH, EXIT_ERROR, EXIT_UNVERIFIED = 0, 1, 2, 3
SUBPROCESS_TIMEOUT = 120


class ToolError(Exception):
    """Environment, input or tool failure; never a verdict about the code."""


# ---------------------------------------------------------------- ELF parsing

SHT_SYMTAB, SHT_RELA, SHT_NOBITS = 2, 4, 8
SHF_EXECINSTR = 4
STT_SECTION, STT_FUNC = 3, 2

# type -> (name, bits rewritten by the linker). Unknown types are an error so a
# new reloc kind can never be silently ignored.
RELOC_TYPES = {
    1: ("ADDR32", 0xFFFFFFFF),
    2: ("ADDR24", 0x03FFFFFC),
    3: ("ADDR16", 0x0000FFFF),
    4: ("ADDR16_LO", 0x0000FFFF),
    5: ("ADDR16_HI", 0x0000FFFF),
    6: ("ADDR16_HA", 0x0000FFFF),
    7: ("ADDR14", 0x0000FFFC),
    8: ("ADDR14_BRTAKEN", 0x0000FFFC),
    9: ("ADDR14_BRNTAKEN", 0x0000FFFC),
    10: ("REL24", 0x03FFFFFC),
    11: ("REL14", 0x0000FFFC),
    12: ("REL14_BRTAKEN", 0x0000FFFC),
    13: ("REL14_BRNTAKEN", 0x0000FFFC),
    26: ("REL32", 0xFFFFFFFF),
    32: ("SDAREL16", 0x0000FFFF),
    106: ("EMB_SDAI16", 0x0000FFFF),
    107: ("EMB_SDA2I16", 0x0000FFFF),
    108: ("EMB_SDA2REL", 0x0000FFFF),
    109: ("EMB_SDA21", 0x001FFFFF),
}
LO_TYPES = {4, 32, 106, 107, 108, 109}  # relocs on the memory-access instruction
HALFWORD_TYPES = {3, 4, 5, 6, 32, 106, 107, 108}  # patch the low half: offset is word+2
WORD_OR_HALF_TYPES = {109}  # EMB_SDA21: either convention, normalized to the word offset


@dataclass
class Section:
    index: int
    name: str
    sh_type: int
    flags: int
    offset: int
    size: int

    @property
    def is_code(self) -> bool:
        return bool(self.flags & SHF_EXECINSTR)


@dataclass
class Symbol:
    index: int
    name: str
    value: int
    size: int
    sym_type: int
    bind: int
    shndx: int


@dataclass
class RawReloc:
    offset: int
    rtype: int
    sym: int
    addend: int


class Elf:
    def __init__(self, path: Path):
        self.path = Path(path)
        try:
            self.data = self.path.read_bytes()
        except OSError as e:
            raise ToolError(f"cannot read {path}: {e}")
        d = self.data
        if len(d) < 52 or d[:4] != b"\x7fELF" or d[4] != 1 or d[5] != 2:
            raise ToolError(f"{path}: not a 32-bit big-endian ELF object")
        if struct.unpack(">H", d[18:20])[0] != 20:
            raise ToolError(f"{path}: not a PowerPC object")
        shoff, = struct.unpack(">I", d[32:36])
        shentsize, shnum, shstrndx = struct.unpack(">HHH", d[46:52])
        try:
            raw = [struct.unpack(">10I", d[shoff + i * shentsize: shoff + i * shentsize + 40])
                   for i in range(shnum)]
        except struct.error:
            raise ToolError(f"{path}: truncated section header table")
        strtab = raw[shstrndx]
        self.sections: list[Section] = []
        for i, (name, typ, flags, _addr, off, size, *_rest) in enumerate(raw):
            self.sections.append(Section(i, self._cstr(strtab[4] + name), typ, flags, off, size))
        self.symbols: list[Symbol] = []
        self._function_symbols: Optional[list[Symbol]] = None
        self.relocs: dict[int, list[RawReloc]] = {}
        for i, (_n, typ, _f, _a, off, size, link, info, _al, _es) in enumerate(raw):
            if typ == SHT_SYMTAB:
                self._read_symbols(off, size, raw[link][4])
            elif typ == SHT_RELA:
                rl = []
                for p in range(off, off + size, 12):
                    r_off, r_info, r_add = struct.unpack(">IIi", d[p:p + 12])
                    rl.append(RawReloc(r_off, r_info & 0xFF, r_info >> 8, r_add))
                self.relocs.setdefault(info, []).extend(rl)

    def _cstr(self, pos: int) -> str:
        end = self.data.index(b"\0", pos)
        return self.data[pos:end].decode("latin-1")

    def _read_symbols(self, off: int, size: int, stroff: int) -> None:
        for n, p in enumerate(range(off, off + size, 16)):
            name, value, sz, info, _other, shndx = struct.unpack(">IIIBBH", self.data[p:p + 16])
            self.symbols.append(Symbol(n, self._cstr(stroff + name), value, sz,
                                       info & 0xF, info >> 4, shndx))

    def section_bytes(self, sec: Section) -> bytes:
        if sec.sh_type == SHT_NOBITS:
            return bytes(sec.size)
        return self.data[sec.offset: sec.offset + sec.size]

    def function_symbols(self) -> list[Symbol]:
        if self._function_symbols is None:
            self._function_symbols = self._build_function_symbols()
        return self._function_symbols

    def _build_function_symbols(self) -> list[Symbol]:
        return [s for s in self.symbols
                if s.shndx and s.shndx < len(self.sections)
                and self.sections[s.shndx].is_code
                and s.sym_type != STT_SECTION and s.name and not s.name.startswith(".L")
                and s.name != "gcc2_compiled."]

    def find_function(self, name: str) -> Optional[Symbol]:
        hits = [s for s in self.function_symbols() if s.name == name]
        if len(hits) > 1:
            raise ToolError(f"{self.path}: symbol '{name}' is defined {len(hits)} times")
        return hits[0] if hits else None

    def symbol_extent(self, sym: Symbol) -> int:
        """Function size in bytes; falls back to the distance to the next symbol."""
        if sym.size:
            return sym.size
        sec = self.sections[sym.shndx]
        later = [s.value for s in self.function_symbols()
                 if s.shndx == sym.shndx and s.value > sym.value]
        return (min(later) if later else sec.size) - sym.value


# ------------------------------------------------------------ project inputs

_SYMBOL_LINE = re.compile(r"^(\S+)\s*=\s*(\.\w+):0x([0-9A-Fa-f]+);(.*)$")
_DTK_LOCAL = re.compile(r"^(.+)_([0-9A-F]{8})$")
_SIZE = re.compile(r"size:0x([0-9A-Fa-f]+)")
CODE_SECTIONS = (".text", ".init")


@dataclass
class SymEntry:
    section: str
    addr: int
    size: int
    is_function: bool


class SymbolTable:
    def __init__(self, path: Optional[Path] = None):
        path = SYMBOLS if path is None else path
        self.entries: dict[str, SymEntry] = {}
        self.duplicates: set[str] = set()
        try:
            lines = Path(path).read_text(errors="replace").splitlines()
        except OSError as e:
            raise ToolError(f"cannot read symbols file {path}: {e}")
        for line in lines:
            m = _SYMBOL_LINE.match(line)
            if not m:
                continue
            name, sec, addr, rest = m.group(1), m.group(2), int(m.group(3), 16), m.group(4)
            sz = _SIZE.search(rest)
            entry = SymEntry(sec, addr, int(sz.group(1), 16) if sz else 0,
                             "type:function" in rest)
            if name in self.entries:
                self.duplicates.add(name)
                continue
            self.entries[name] = entry

    def function(self, name: str) -> SymEntry:
        if name in self.duplicates:
            raise ToolError(f"'{name}' is defined more than once in {SYMBOLS.name}")
        e = self.entries.get(name)
        if not e or e.section not in CODE_SECTIONS:
            raise ToolError(f"'{name}' is not a code symbol in {SYMBOLS.relative_to(ROOT)}"
                            " (check the spelling; verify_fn needs a named function)")
        if not e.size:
            raise ToolError(f"'{name}' has no size in {SYMBOLS.name}; cannot bound the function")
        return e

    def addr(self, name: str) -> Optional[int]:
        e = self.entries.get(name)
        if e is None:
            e = self._dtk_local(name)
        return e.addr if e and name not in self.duplicates else None

    def _dtk_local(self, name: str) -> Optional[SymEntry]:
        """DTK renames a local symbol to NAME_<address> in split objects.

        Accept the suffixed name only when the unsuffixed symbol is in
        symbols.txt at exactly that address.
        """
        m = _DTK_LOCAL.match(name)
        if not m or m.group(1) in self.duplicates:
            return None
        e = self.entries.get(m.group(1))
        return e if e and e.addr == int(m.group(2), 16) else None


class Dol:
    """Retail DOL image (file-backed sections only; .bss is not present)."""

    def __init__(self, path: Path):
        self.data = Path(path).read_bytes()
        hdr = struct.unpack(">54I", self.data[:216])
        self.sections = [(hdr[i], hdr[18 + i], hdr[36 + i]) for i in range(18) if hdr[36 + i]]

    def read(self, addr: int, n: int) -> Optional[bytes]:
        for off, base, size in self.sections:
            if base <= addr and addr + n <= base + size:
                return self.data[off + addr - base: off + addr - base + n]
        return None


def load_dol(path: Optional[Path], notes: list[str]) -> Optional[Dol]:
    if path is None or not Path(path).is_file():
        notes.append("retail DOL not available: target bytes come from the split object only "
                     "and referenced data cannot be compared")
        return None
    data = Path(path).read_bytes()
    if hashlib.sha1(data).hexdigest() != DOL_SHA1:
        raise ToolError(f"{path} does not have the retail SHA-1 {DOL_SHA1}; refusing to use it")
    return Dol(path)


def lookup_unit(addr: int) -> Optional[str]:
    """Return the unit path (e.g. 'numath/nuvec') whose split covers addr."""
    cur = None
    start_re = re.compile(r"start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)")
    try:
        text = SPLITS.read_text()
    except OSError as e:
        raise ToolError(f"cannot read {SPLITS}: {e}")
    for raw in text.splitlines():
        s = raw.strip()
        if not s or s.startswith("#"):
            continue
        if not raw.startswith((" ", "\t")):
            name = s.rstrip(":").strip()
            cur = name if not name.startswith(".") else cur
            continue
        m = start_re.search(s)
        if m and cur and s.startswith(".text"):
            if int(m.group(1), 16) <= addr < int(m.group(2), 16):
                return cur.rsplit(".", 1)[0]
    return None


# ------------------------------------------------------ function extraction

PRIMARY_LOAD_STORE_WIDTH = {32: 4, 33: 4, 34: 1, 35: 1, 36: 4, 37: 4, 38: 1, 39: 1,
                            40: 2, 41: 2, 42: 2, 43: 2, 44: 2, 45: 2,
                            48: 4, 49: 4, 50: 8, 51: 8, 52: 4, 53: 4, 54: 8, 55: 8}


@dataclass(frozen=True)
class Ref:
    rtype: int
    name: str            # referenced symbol, or section name for section-relative
    addend: int
    is_section: bool     # section-relative reference (name is the section)
    loc: Optional[tuple]  # (section name, offset) when defined in this object
    loc_is_code: bool = False

    @property
    def type_name(self) -> str:
        return RELOC_TYPES[self.rtype][0]

    def describe(self, with_type: bool = True) -> str:
        base = f"{self.name}{self.addend:+#x}" if self.addend or self.is_section else self.name
        return f"{self.type_name} {base}" if with_type else base


@dataclass
class Insn:
    word: int
    mask: int
    ref: Optional[Ref]

    @property
    def cmp(self) -> int:
        return self.word & ~self.mask & 0xFFFFFFFF


@dataclass
class Func:
    name: str
    elf: Elf
    section: Section
    start: int
    size: int
    insns: list[Insn]
    data_widths: dict = field(default_factory=dict)  # loc -> access width


def _containing_symbol(elf: Elf, shndx: int, off: int) -> Optional[Symbol]:
    best = None
    for s in elf.function_symbols():
        if s.shndx == shndx and s.value <= off < s.value + max(s.size, 1):
            if best is None or s.value > best.value:
                best = s
    return best


def extract_function(elf: Elf, name: str, sym: Optional[Symbol] = None) -> Func:
    sym = sym or elf.find_function(name)
    if sym is None:
        raise ToolError(f"{elf.path}: no function symbol '{name}'")
    sec = elf.sections[sym.shndx]
    size = elf.symbol_extent(sym)
    if size % 4 or size <= 0:
        raise ToolError(f"{elf.path}: '{name}' has size {size}; expected a positive multiple of 4")
    code = elf.section_bytes(sec)[sym.value: sym.value + size]
    if len(code) != size:
        raise ToolError(f"{elf.path}: '{name}' extends past the end of {sec.name}")
    words = struct.unpack(f">{size // 4}I", code)
    by_off = {}
    for r in elf.relocs.get(sec.index, []):
        if sym.value <= r.offset < sym.value + size:
            if r.rtype not in RELOC_TYPES:
                raise ToolError(f"{elf.path}: unsupported relocation type {r.rtype} in '{name}'")
            lowhalf = 2 if r.rtype in HALFWORD_TYPES else 0
            if r.rtype in WORD_OR_HALF_TYPES and (r.offset - sym.value) % 4 == 2:
                lowhalf = 2  # MWCC places SDA21 on the halfword; GNU as and DTK on the word
            if (r.offset - sym.value) % 4 != lowhalf:
                raise ToolError(f"{elf.path}: misaligned {RELOC_TYPES[r.rtype][0]} relocation "
                                f"at +0x{r.offset - sym.value:X} in '{name}'")
            by_off[r.offset - sym.value - lowhalf] = r
    insns = []
    func = Func(name, elf, sec, sym.value, size, insns)
    for i, w in enumerate(words):
        off = i * 4
        ref = None
        mask = 0
        r = by_off.get(off)
        if r is not None:
            ref = _make_ref(elf, r)
            mask = RELOC_TYPES[r.rtype][1]
        else:
            ref, mask = _implicit_branch(elf, sec, sym, w, off, size)
        insns.append(Insn(w, mask, ref))
        if ref is not None and ref.rtype in LO_TYPES and ref.loc and not ref.loc_is_code:
            width = PRIMARY_LOAD_STORE_WIDTH.get(w >> 26)
            if width:
                func.data_widths[ref.loc] = max(width, func.data_widths.get(ref.loc, 0))
            else:
                func.data_widths.setdefault(ref.loc, 0)
    return func


def _make_ref(elf: Elf, r: RawReloc) -> Ref:
    if r.sym >= len(elf.symbols):
        raise ToolError(f"{elf.path}: relocation references missing symbol {r.sym}")
    s = elf.symbols[r.sym]
    if s.shndx == 0:
        return Ref(r.rtype, s.name, r.addend, False, None)
    if s.shndx >= len(elf.sections):
        raise ToolError(f"{elf.path}: symbol '{s.name}' has unsupported section index {s.shndx}")
    sec = elf.sections[s.shndx]
    if s.sym_type == STT_SECTION:
        return Ref(r.rtype, sec.name, r.addend, True, (sec.name, s.value + r.addend), sec.is_code)
    return Ref(r.rtype, s.name, r.addend, False, (sec.name, s.value + r.addend), sec.is_code)


def _implicit_branch(elf, sec, sym, word, off, size):
    """Branch resolved at assembly time but leaving the function (no reloc)."""
    op = word >> 26
    if op not in (16, 18) or word & 2:  # not b/bc, or absolute
        return None, 0
    if op == 18:
        disp, mask, rtype = word & 0x03FFFFFC, 0x03FFFFFC, 10
        disp = disp - 0x04000000 if disp & 0x02000000 else disp
    else:
        disp, mask, rtype = word & 0xFFFC, 0xFFFC, 11
        disp = disp - 0x10000 if disp & 0x8000 else disp
    tgt = off + disp
    if 0 <= tgt < size:
        return None, 0  # intra-function: compared as raw bytes
    abs_tgt = sym.value + tgt
    cont = _containing_symbol(elf, sec.index, abs_tgt)
    if cont is not None:
        return Ref(rtype, cont.name, abs_tgt - cont.value, False, (sec.name, abs_tgt), True), mask
    return Ref(rtype, sec.name, abs_tgt, True, (sec.name, abs_tgt), True), mask


# ---------------------------------------------------------------- comparison

def _same_opcode(a: int, b: int) -> bool:
    if a >> 26 != b >> 26:
        return False
    op = a >> 26
    if op in (4, 19, 31):
        return (a >> 1) & 0x3FF == (b >> 1) & 0x3FF
    if op in (59, 63):  # A-form: bits 6-10 are frC, only the low 5 XO bits are fixed
        return (a >> 1) & 0x1F == (b >> 1) & 0x1F
    return True


@dataclass
class Diff:
    kind: str       # opcode | operand | reloc | length | data
    index_t: Optional[int]
    index_m: Optional[int]
    detail: str


@dataclass
class Report:
    name: str
    verdict: str = "ERROR"
    exit_code: int = EXIT_ERROR
    target_insns: int = 0
    mine_insns: int = 0
    exact_insns: int = 0
    mnemonic_similarity: int = 0
    diffs: list = field(default_factory=list)
    mappings: list = field(default_factory=list)   # (mine, target, status)
    notes: list = field(default_factory=list)
    rows: list = field(default_factory=list)


class Comparer:
    def __init__(self, symbols: SymbolTable, dol: Optional[Dol], unit_proof=None):
        self.unit_proof = unit_proof
        self.symbols = symbols
        self.dol = dol
        self.name_map: dict = {}       # mine key -> target key
        self.rev_map: dict = {}
        self.pending_data: dict = {}   # (mine loc, target addr) -> width
        self.mappings: dict = {}       # description -> status

    def ref_status(self, t: Ref, m: Ref, mine: Func) -> tuple[str, str]:
        """('same'|'alias'|'mapped'|'diff', detail) for an otherwise equal pair."""
        if t.rtype != m.rtype:
            return "diff", f"relocation type: target {t.type_name} vs mine {m.type_name}"
        if (not m.is_section and not t.is_section and m.name == t.name and m.addend == t.addend
                and (m.loc is None or m.loc_is_code)):
            return "same", ""
        taddr = None if t.is_section else self.symbols.addr(t.name)
        if m.loc is not None and not m.loc_is_code:
            if taddr is None:
                return "diff", (f"target references {t.describe()} which is not in "
                                f"symbols.txt; cannot map mine {m.describe()}")
            return self._bind(("loc",) + m.loc, taddr + t.addend,
                              m.describe(False), f"{t.name}{t.addend:+#x}", m.loc)
        maddr = None if m.is_section else self.symbols.addr(m.name)
        if maddr is not None and taddr is not None:
            if maddr + m.addend == taddr + t.addend:
                self.mappings[f"{m.name} = {t.name}"] = "verified: same address in symbols.txt"
                return "alias", ""
            return "diff", (f"references {m.describe()} (0x{maddr + m.addend:08X}) "
                            f"but target references {t.describe()} (0x{taddr + t.addend:08X})")
        if m.addend != t.addend:
            return "diff", f"addend: target {t.describe()} vs mine {m.describe()}"
        return self._bind(("sym", m.name), ("sym", t.name),
                          m.describe(False), t.describe(False), None)

    def _bind(self, mkey, tkey, mdesc, tdesc, mloc) -> tuple[str, str]:
        tkey = tkey if isinstance(tkey, tuple) else ("addr", tkey)
        if self.name_map.setdefault(mkey, tkey) != tkey:
            return "diff", f"{mdesc} was already mapped to a different target ({tdesc} now)"
        if self.rev_map.setdefault(tkey, mkey) != mkey:
            return "diff", f"{tdesc} is already referenced by a different symbol in mine"
        key = f"{mdesc} -> {tdesc}"
        self.mappings.setdefault(key, "unverified: names differ, mapped by consistency")
        if mloc is not None:
            self.pending_data[(key, mloc, tkey[1])] = 0
        return "mapped", ""

    def unit_proof_covers(self, mloc, sec, width: int, taddr: int) -> bool:
        return bool(self.unit_proof and 0 <= mloc[1] and mloc[1] + width <= sec.size
                    and self.unit_proof.get(mloc[0], -1) + mloc[1] == taddr)

    def verify_data(self, mine: Func, report: Report) -> None:
        for (key, mloc, taddr), _ in list(self.pending_data.items()):
            width = mine.data_widths.get(mloc, 0)
            sec = next(s for s in mine.elf.sections if s.name == mloc[0])
            if not width:
                if self.unit_proof_covers(mloc, sec, 1, taddr):
                    self.mappings[key] = "verified: full unit sections and relocations match retail"
                    continue
                self.mappings[key] = "unverified: address-taken or unsized, contents not compared"
                continue
            if self.dol is None:
                self.mappings[key] = "unverified: retail DOL not available"
                continue
            mine_bytes = mine.elf.section_bytes(sec)[mloc[1]: mloc[1] + width]
            target_bytes = self.dol.read(taddr, width)
            if len(mine_bytes) != width:
                report.diffs.append(Diff("data", None, None,
                                         f"{key}: mine {sec.name} ends before {width} bytes"))
                self.mappings[key] = "MISMATCH: referenced data runs past the end of mine"
            elif target_bytes is None:
                if sec.sh_type == SHT_NOBITS:
                    self.mappings[key] = "unverified: both uninitialised (bss); layout not compared"
                else:
                    self.mappings[key] = ("unverified: retail address 0x%08X is not file-backed "
                                          "(bss) but mine is initialised" % taddr)
            elif mine_bytes != target_bytes and self.unit_proof_covers(mloc, sec, width, taddr):
                # Pointer words are zero in the object; the complete-unit proof
                # resolves their relocations against retail.
                self.mappings[key] = "verified: full unit sections and relocations match retail"
            elif mine_bytes != target_bytes:
                report.diffs.append(Diff(
                    "data", None, None,
                    f"{key}: mine {mine_bytes.hex()} vs retail {target_bytes.hex()}"))
                self.mappings[key] = f"MISMATCH: data {mine_bytes.hex()} != retail {target_bytes.hex()}"
            else:
                if sec.sh_type == SHT_NOBITS:
                    self.mappings[key] = (f"unverified: mine is zero-filled bss ({sec.name}) but "
                                          f"retail 0x{taddr:08X} is initialised data (value matches, "
                                          "section placement does not)")
                else:
                    self.mappings[key] = (f"verified: {width} bytes identical to retail "
                                          f"({mine_bytes.hex()})")


def _self_entry_branch(t: Insn, m: Insn, index: int, mine: Func) -> bool:
    """DTK relocates a branch back to its own function entry; the assembler
    resolves it. Accept only when mine's assembled branch lands on offset 0."""
    ref = t.ref
    if ref is None or m.ref is not None or ref.is_section or ref.name != mine.name or ref.addend:
        return False
    opcode = {10: 18, 11: 16}.get(ref.rtype)
    if opcode is None or m.word >> 26 != opcode or m.word & 3:
        return False
    bits, mask = (26, 0x03FFFFFC) if ref.rtype == 10 else (16, 0xFFFC)
    disp = m.word & mask
    disp -= (disp & (1 << (bits - 1))) << 1
    return index * 4 + disp == 0


def _align_key(insn: Insn, index: int, func: Func, is_target: bool):
    ref = insn.ref
    if is_target:
        entry = (ref is not None and not ref.is_section and ref.name == func.name
                 and not ref.addend and ref.rtype in (10, 11))
        rtype = ref.rtype if entry else None
    else:
        rtype = {18: 10, 16: 11}.get(insn.word >> 26) if ref is None and not insn.word & 3 else None
        if rtype is not None:
            bits, mask = (26, 0x03FFFFFC) if rtype == 10 else (16, 0xFFFC)
            disp = insn.word & mask
            disp -= (disp & (1 << (bits - 1))) << 1
            rtype = rtype if index * 4 + disp == 0 else None
    if rtype is None:
        return (insn.cmp, ref and ref.rtype)
    return (insn.word & ~RELOC_TYPES[rtype][1] & 0xFFFFFFFF, "entry-branch", rtype)


def _row_diff(t: Insn, m: Insn, cmp: Comparer, mine: Func, mi: Optional[int] = None) -> tuple[str, str]:
    """Classify one aligned pair: ('ok'|'mapped'|diff-kind, detail)."""
    union = t.mask | m.mask
    if (t.word & ~union) & 0xFFFFFFFF != (m.word & ~union) & 0xFFFFFFFF:
        kind = "operand" if _same_opcode(t.word, m.word) else "opcode"
        return kind, f"{t.word:08X} vs {m.word:08X}"
    if mi is not None and _self_entry_branch(t, m, mi, mine):
        return "ok", ""
    if (t.ref is None) != (m.ref is None):
        side = "target" if t.ref else "mine"
        return "reloc", f"only {side} has a relocation"
    if t.ref is None:
        return "ok", ""
    status, detail = cmp.ref_status(t.ref, m.ref, mine)
    if status == "diff":
        return "reloc", detail
    return ("ok" if status in ("same", "alias") else "mapped"), ""


def compare_functions(target: Func, mine: Func, symbols: SymbolTable,
                      dol: Optional[Dol], unit_proof=None) -> Report:
    rep = Report(target.name, target_insns=len(target.insns), mine_insns=len(mine.insns))
    cmp = Comparer(symbols, dol, unit_proof)
    # Alignment keys use each side's own mask; differing reloc types still show
    # up as 'reloc' diffs because aligned pairs are re-checked with a union mask.
    tk = [_align_key(i, k, target, True) for k, i in enumerate(target.insns)]
    mk = [_align_key(i, k, mine, False) for k, i in enumerate(mine.insns)]
    sm = difflib.SequenceMatcher(a=tk, b=mk, autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for k in range(i2 - i1):
                ti, mi = i1 + k, j1 + k
                kind, detail = _row_diff(target.insns[ti], mine.insns[mi], cmp, mine, mi)
                _record(rep, kind, detail, ti, mi)
            continue
        span = max(i2 - i1, j2 - j1)
        for k in range(span):
            ti = i1 + k if i1 + k < i2 else None
            mi = j1 + k if j1 + k < j2 else None
            if ti is None or mi is None:
                kind, detail = "length", "instruction only in " + ("target" if mi is None else "mine")
            else:
                kind, detail = _row_diff(target.insns[ti], mine.insns[mi], cmp, mine, mi)
                if kind in ("ok", "mapped"):
                    kind, detail = "operand", "differs from the aligned neighbours"
            _record(rep, kind, detail, ti, mi)
    cmp.verify_data(mine, rep)
    rep.mappings = [(k, v) for k, v in cmp.mappings.items()]
    rep.exact_insns = sum(1 for r in rep.rows if r[2] in ("ok", "mapped"))
    rep.mnemonic_similarity = sum(
        1 for r in rep.rows
        if r[0] is not None and r[1] is not None
        and _same_opcode(target.insns[r[0]].word, mine.insns[r[1]].word))
    if rep.diffs:
        rep.verdict, rep.exit_code = "MISMATCH", EXIT_MISMATCH
    elif any(v.startswith("unverified") for _, v in rep.mappings):
        rep.verdict, rep.exit_code = "MATCH (unverified references)", EXIT_MATCH
    else:
        rep.verdict, rep.exit_code = "MATCH", EXIT_MATCH
    return rep


def _record(rep: Report, kind: str, detail: str, ti, mi) -> None:
    rep.rows.append((ti, mi, kind, detail))
    if kind not in ("ok", "mapped"):
        rep.diffs.append(Diff(kind, ti, mi, detail))


# -------------------------------------------------------------- disassembly

_DTK_LINE = re.compile(
    r"^\s*/\*\s*([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+((?:[0-9A-Fa-f]{2}\s){4})\*/\s*(.*?)\s*$")
_LABEL = re.compile(r"\.L_([0-9A-Fa-f]+)")


def disassemble(obj: Path, func: Func, notes: list[str], label: str) -> Optional[list[str]]:
    """Per-instruction text from `dtk elf disasm`, display only; None if unavailable."""
    if not DTK.is_file():
        notes.append(f"{label}: dtk not found, showing raw words only")
        return None
    with tempfile.TemporaryDirectory(prefix="verify_fn_") as tmp:
        out = Path(tmp) / "disasm.s"
        try:
            proc = subprocess.run([str(DTK), "elf", "disasm", str(obj), str(out)],
                                  capture_output=True, text=True, timeout=SUBPROCESS_TIMEOUT)
        except (OSError, subprocess.TimeoutExpired) as e:
            notes.append(f"{label}: dtk disasm failed ({e}), showing raw words only")
            return None
        if proc.returncode != 0 or not out.is_file():
            notes.append(f"{label}: dtk disasm exited {proc.returncode}, showing raw words only")
            return None
        text = out.read_text(errors="replace")
    lines, grab, base = [], False, None
    for line in text.splitlines():
        s = line.strip()
        if s.startswith(".fn ") and s.split()[1].rstrip(",") == func.name:
            grab = True
            continue
        if grab and s.startswith(".endfn"):
            break
        m = _DTK_LINE.match(line) if grab else None
        if m:
            addr = int(m.group(1), 16)
            base = addr if base is None else base
            lines.append((int(m.group(2).replace(" ", ""), 16), addr, m.group(3)))
    if len(lines) != len(func.insns) or any(w != i.word for (w, _a, _t), i in zip(lines, func.insns)):
        notes.append(f"{label}: dtk listing does not line up with the ELF bytes, showing raw words only")
        return None
    return [_LABEL.sub(lambda mm: f"loc_{int(mm.group(1), 16) - base:X}", t) for _w, _a, t in lines]


# --------------------------------------------------------- locating objects

def find_target_object(name: str, addr: int, size: int, unit: Optional[str]) -> Path:
    if not TARGET_OBJ_DIR.is_dir():
        raise ToolError("no target objects under build/GL5E4F/obj; run `bash build.sh` first")
    if unit:
        cand = TARGET_OBJ_DIR / f"{unit}.o"
        try:
            if cand.is_file() and Elf(cand).find_function(name) is not None:
                return cand
        except ToolError:
            pass
    hits = []
    for obj in sorted(TARGET_OBJ_DIR.rglob("*.o")):
        try:
            elf = Elf(obj)
            s = elf.find_function(name)
        except ToolError:
            continue
        if s is not None:
            hits.append(obj)
    if not hits:
        raise ToolError(f"no target object under build/GL5E4F/obj defines '{name}' "
                        f"(0x{addr:08X}); rerun `bash build.sh` to re-split the DOL")
    if len(hits) > 1:
        raise ToolError(f"'{name}' is defined in several target objects: "
                        + ", ".join(str(h.relative_to(ROOT)) for h in hits))
    return hits[0]


def source_for_unit(unit: str) -> Optional[Path]:
    for ext in (".c", ".cpp", ".s"):
        p = SRC_DIR / f"{unit}{ext}"
        if p.is_file():
            return p
    return None


def ensure_fresh(obj: Path, unit: str, build: bool, allow_stale: bool) -> None:
    rel = obj.relative_to(ROOT) if obj.is_relative_to(ROOT) else obj
    ninja_file = ROOT / "build.ninja"
    if build:
        if not ninja_file.is_file():
            raise ToolError("build.ninja is missing; run `python3 configure.py` first")
        try:
            proc = subprocess.run(["ninja", str(rel)], cwd=ROOT, capture_output=True,
                                  text=True, timeout=600)
        except (OSError, subprocess.TimeoutExpired) as e:
            raise ToolError(f"could not run ninja: {e}")
        if proc.returncode != 0:
            tail = "\n".join((proc.stdout + proc.stderr).strip().splitlines()[-40:])
            raise ToolError(f"ninja failed to build {rel} (exit {proc.returncode}):\n{tail}")
        return
    if allow_stale:
        return
    if not obj.is_file():
        raise ToolError(f"{rel} is not built. Run: ninja {rel}   (or pass --build)")
    stale = None
    if ninja_file.is_file():
        try:
            proc = subprocess.run(["ninja", "-n", str(rel)], cwd=ROOT, capture_output=True,
                                  text=True, timeout=SUBPROCESS_TIMEOUT)
            if proc.returncode == 0:
                stale = "no work to do" not in proc.stdout
        except (OSError, subprocess.TimeoutExpired):
            pass
    if stale is None:
        src = source_for_unit(unit)
        stale = bool(src and src.stat().st_mtime > obj.stat().st_mtime)
    if stale:
        raise ToolError(f"{rel} is stale (sources or flags changed since it was built). "
                        f"Run: ninja {rel}   (or pass --build, or --allow-stale)")


# ------------------------------------------------------------------ output

def render(rep: Report, target: Func, mine: Func, tdis, mdis, full: bool) -> list[str]:
    def text(func: Func, dis, i: Optional[int]) -> str:
        if i is None:
            return ""
        if dis is not None:
            return dis[i]
        return f"{func.insns[i].word:08X}"

    out = [f"   {'TARGET':<38} {'MINE':<38}"]
    marks = {"ok": " ", "mapped": "~"}
    near = set()
    for n, r in enumerate(rep.rows):
        if r[2] == "ok":
            continue
        near.update(range(max(0, n - 2), min(len(rep.rows), n + 3)))
    gap = False
    for n, (ti, mi, kind, _d) in enumerate(rep.rows):
        if not full and n not in near:
            gap = True
            continue
        if gap:
            out.append("   ...")
            gap = False
        mark = marks.get(kind, "!!")
        out.append(f"{mark:<2} {text(target, tdis, ti)[:38]:<38} {text(mine, mdis, mi)[:38]:<38}"
                   + (f"  [{kind}]" if kind not in marks else ""))
    if gap:
        out.append("   ...")
    return out


def format_report(rep: Report, args, target: Func, mine: Func, tdis, mdis,
                  tpath, mpath, dol_checked: bool) -> str:
    out = [f"{rep.name}  target {tpath}  mine {mpath}",
           f"target: {rep.target_insns} insns   mine: {rep.mine_insns} insns"
           f"   retail DOL byte check: {'yes' if dol_checked else 'no'}", ""]
    if not args.quiet:
        out += render(rep, target, mine, tdis, mdis, args.full)
        out.append("")
    if rep.mappings:
        out.append("references:")
        out += [f"  {k}: {v}" for k, v in rep.mappings]
        out.append("")
    out += [f"  {d.kind}: " + (f"target #{d.index_t} " if d.index_t is not None else "")
            + (f"mine #{d.index_m} " if d.index_m is not None else "") + d.detail
            for d in rep.diffs[: args.max_diffs]]
    if len(rep.diffs) > args.max_diffs:
        out.append(f"  ... {len(rep.diffs) - args.max_diffs} more differences")
    if rep.diffs:
        out.append("")
    out.append(f"exact instructions: {rep.exact_insns}/{rep.target_insns}   "
               f"(informational mnemonic-order similarity: {rep.mnemonic_similarity}/"
               f"{rep.target_insns}, not a match)")
    out += [f"note: {n}" for n in rep.notes]
    out.append(f"RESULT: {rep.verdict}")
    return "\n".join(out)


def to_json(rep: Report) -> str:
    return json.dumps({
        "function": rep.name, "verdict": rep.verdict, "exit_code": rep.exit_code,
        "target_insns": rep.target_insns, "mine_insns": rep.mine_insns,
        "exact_insns": rep.exact_insns, "mnemonic_similarity": rep.mnemonic_similarity,
        "differences": [d.__dict__ for d in rep.diffs],
        "references": [{"reference": k, "status": v} for k, v in rep.mappings],
        "notes": rep.notes}, indent=2)


# --------------------------------------------------------------------- main

def verify(args) -> tuple[Report, Func, Func, Optional[list], Optional[list], Path, Path, bool]:
    symbols = SymbolTable()
    entry = symbols.function(args.name)
    notes: list[str] = []
    unit = args.unit or lookup_unit(entry.addr)
    if args.object:
        mine_path = Path(args.object).resolve()
        if not mine_path.is_file():
            raise ToolError(f"{args.object} does not exist")
    else:
        if not unit:
            raise ToolError(f"no split unit covers 0x{entry.addr:08X}; pass --unit or --object")
        mine_path = SRC_OBJ_DIR / f"{unit}.o"
        ensure_fresh(mine_path, unit, args.build, args.allow_stale)

    tpath = Path(args.target_object).resolve() if args.target_object else \
        find_target_object(args.name, entry.addr, entry.size, unit)
    telf, melf = Elf(tpath), Elf(mine_path)

    tsym = telf.find_function(args.name)
    if tsym is None:
        raise ToolError(f"{tpath} does not define '{args.name}'")
    csym_name = args.compiled_symbol or args.name
    msym = melf.find_function(csym_name)
    if msym is None:
        names = [s.name for s in melf.function_symbols()]
        close = difflib.get_close_matches(csym_name, names, n=5)
        hint = f" Close matches: {', '.join(close)}." if close else ""
        have = f" Functions in the object: {', '.join(names[:15])}" + (
            " ..." if len(names) > 15 else "") if names else " The object defines no functions."
        raise ToolError(f"'{csym_name}' is not defined in {mine_path.name} (check "
                        f"--compiled-symbol, or whether the function is #if'd out).{hint}{have}")

    target = extract_function(telf, args.name, tsym)
    mine = extract_function(melf, csym_name, msym)
    if target.size != entry.size:
        raise ToolError(f"target object says '{args.name}' is 0x{target.size:X} bytes but "
                        f"symbols.txt says 0x{entry.size:X}; rebuild the split objects")

    dol = None if args.no_dol else load_dol(DOL_PATH if args.dol is None else Path(args.dol), notes)
    dol_checked = False
    if dol is not None:
        retail = dol.read(entry.addr, entry.size)
        if retail is None:
            raise ToolError(f"0x{entry.addr:08X} is not inside the retail DOL text sections")
        words = struct.unpack(f">{entry.size // 4}I", retail)
        bad = [n for n, (rw, ti) in enumerate(zip(words, target.insns))
               if rw & ~ti.mask & 0xFFFFFFFF != ti.cmp]
        if bad:
            raise ToolError(f"target object differs from the retail DOL at 0x{entry.addr:08X}"
                            f" (first at +0x{bad[0] * 4:X}) outside relocated fields; the "
                            "split objects are stale, rerun `bash build.sh`")
        dol_checked = True

    unit_proof = None
    actual_unit = lookup_unit(entry.addr)
    if dol is not None and actual_unit and unit == actual_unit and tpath.resolve() == (TARGET_OBJ_DIR / f"{actual_unit}.o").resolve():
        try:
            from tools.verify_unit import prove_unit, split_ranges
        except ModuleNotFoundError:
            from verify_unit import prove_unit, split_ranges
        ranges = split_ranges(SPLITS.read_text(), actual_unit)
        unit_proof = prove_unit(telf, melf, ranges, symbols, dol)
    rep = compare_functions(target, mine, symbols, dol, unit_proof)
    rep.notes = notes
    tdis = disassemble(tpath, target, rep.notes, "target") if not args.quiet else None
    mdis = disassemble(mine_path, mine, rep.notes, "mine") if not args.quiet else None
    return rep, target, mine, tdis, mdis, tpath, mine_path, dol_checked


def rel(p: Path) -> str:
    try:
        return str(p.relative_to(ROOT))
    except ValueError:
        return str(p)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("name", help="function name as it appears in config/GL5E4F/symbols.txt")
    ap.add_argument("--unit", help="unit path of your compiled object, e.g. numath/nuvec")
    ap.add_argument("--compiled-symbol", help="symbol emitted by your object if it differs")
    ap.add_argument("--object", help="compare this compiled object instead of build/GL5E4F/src/<unit>.o")
    ap.add_argument("--target-object", help="override the DTK-split target object")
    ap.add_argument("--build", action="store_true", help="run ninja on the object first")
    ap.add_argument("--allow-stale", action="store_true", help="skip the up-to-date check")
    ap.add_argument("--dol", help="retail DOL path (default orig/GL5E4F/sys/main.dol)")
    ap.add_argument("--no-dol", action="store_true", help="do not use the retail DOL")
    ap.add_argument("--strict-symbols", action="store_true",
                    help="exit 3 when code matches but references are only mapped by consistency")
    ap.add_argument("--full", action="store_true", help="print every instruction row")
    ap.add_argument("--quiet", action="store_true", help="skip the side-by-side listing")
    ap.add_argument("--json", action="store_true", help="machine-readable output")
    ap.add_argument("--max-diffs", type=int, default=25)
    args = ap.parse_args(argv)
    try:
        rep, target, mine, tdis, mdis, tpath, mpath, dol_checked = verify(args)
    except (ToolError, struct.error, ValueError, IndexError, OSError) as e:
        print(f"error: {e}", file=sys.stderr)
        if args.json:
            print(json.dumps({"function": args.name, "verdict": "ERROR", "exit_code": EXIT_ERROR,
                              "error": str(e)}, indent=2))
        return EXIT_ERROR
    code = rep.exit_code
    if args.strict_symbols and code == EXIT_MATCH and "unverified" in rep.verdict:
        code = EXIT_UNVERIFIED
    if args.json:
        print(to_json(rep))
    else:
        print(format_report(rep, args, target, mine, tdis, mdis, rel(tpath), rel(mpath), dol_checked))
    return code


if __name__ == "__main__":
    sys.exit(main())
