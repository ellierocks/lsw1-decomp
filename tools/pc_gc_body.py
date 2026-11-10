#!/usr/bin/env python3
"""
Write the paired Japanese PC 1.0 x86 body of a GC function as an annotated listing.

    python3 tools/pc_gc_body.py fn_8009F0B8                 # writes work/fn_8009F0B8/jp_x86.s
    python3 tools/pc_gc_body.py 0x8009F0B8 --stdout
    python3 tools/pc_gc_body.py NAME --out-dir work/scratch # writes work/scratch/NAME/jp_x86.s

The pair comes from build/pc_analysis/gc_pc_anchors.tsv and the x86 extent from
pc_functions.tsv (both written by tools/pc_gc_anchors.py, which runs on first use,
about 25 s). The file starts with a summary: x86 calls aligned against the GC
function's own call order (disagreements are leads for inlining or PC-only code),
globals with their paired GC labels, strings, float constants and switch tables.
The listing below it is `objdump -M intel` output with local branch labels, calls
named after their paired GC functions and data operands annotated.

MSVC x86 codegen and PC struct offsets never transfer; read it for call order,
branch structure, globals and constants, confirmed against the GC assembly
(docs/research.md#pc-builds). The output is derived from the retail exe and stays
local (work/ is git-ignored). Without the exe, objdump or a pair it writes nothing
and prints a one-line note.
"""

from __future__ import annotations

import argparse
import bisect
import difflib
import re
import shutil
import struct
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import pc_gc_anchors  # noqa: E402
from pc_gc_features import nice_float, pe_layout  # noqa: E402,F401

OUT_DIR = pc_gc_anchors.OUT_DIR
GLOBALS_DOC = ROOT / "docs/symbol_donors/pc_jp_globals.tsv"
TRUSTED = {"confirmed", "strings"}
CAVEAT = ("MSVC x86 with link-time codegen: arguments may arrive in registers (pushes are a lower"
          " bound). Struct offsets and frame layout NEVER transfer (PC layouts differ); use call"
          " order, branch structure, globals and constants, confirmed against the GC assembly.")
LINE_RE = re.compile(r"^\s*([0-9a-f]+):\s+(\S+)\s*(.*?)\s*$")
TARGET_RE = re.compile(r"^0x([0-9a-f]+)$")
HEX_RE = re.compile(r"0x([0-9a-f]+)")
TABLE_RE = re.compile(r"DWORD PTR \[(\w+)\*4\+0x([0-9a-f]+)\]")
BYTE_INDEX_RE = re.compile(r"^(\w+),BYTE PTR \[(\w+)\+0x([0-9a-f]+)\]$")
CMP_RE = re.compile(r"^(\w+),0x([0-9a-f]+)$")
TERMINATORS = ("ret", "jmp")
PADDING = {"int3", "nop"}


# --- inputs ---------------------------------------------------------------------

@dataclass
class Tables:
    """Pairs, x86 extents and global pairs from build/pc_analysis and the committed donor tables."""
    gc_to_pc: dict = field(default_factory=dict)    # GC address -> (x86 address, method, evidence)
    pc_to_gc: dict = field(default_factory=dict)    # x86 address -> (GC address, method)
    extents: dict = field(default_factory=dict)     # x86 start -> size
    callers: dict = field(default_factory=dict)     # x86 start -> distinct direct callers
    starts: list = field(default_factory=list)
    globals: dict = field(default_factory=dict)     # x86 address -> (GC address, source)
    audit: dict = field(default_factory=dict)       # GC address -> audit verdict text


def read_tsv(path: Path) -> list[list[str]]:
    if not path.exists():
        return []
    return [line.split("\t") for line in path.read_text().splitlines()
            if line and not line.startswith("#") and line.split("\t")[0].startswith("0x")]


def load_tables(out_dir: Path = OUT_DIR, globals_doc: Path = GLOBALS_DOC) -> Tables:
    t = Tables()
    for row in read_tsv(out_dir / "gc_pc_anchors.tsv"):
        if len(row) < 4:
            continue
        kind = pc_gc_anchors.method(row[3])
        if kind == "ambiguous":
            continue
        g, p = int(row[0], 16), int(row[2], 16)
        t.gc_to_pc[g] = (p, kind, row[3])
        t.pc_to_gc[p] = (g, kind)
        if len(row) > 4 and row[4]:
            t.audit[g] = row[4]
    for row in read_tsv(out_dir / "pc_functions.tsv"):
        start = int(row[0], 16)
        t.extents[start] = int(row[1], 16)
        t.callers[start] = int(row[2]) if len(row) > 2 and row[2].isdigit() else 0
    t.starts = sorted(t.extents)
    for row in read_tsv(out_dir / "gc_pc_globals.tsv"):
        if len(row) >= 3:
            t.globals[int(row[2], 16)] = (int(row[0], 16), "inferred")
    for row in read_tsv(globals_doc):
        if len(row) >= 3:
            t.globals[int(row[2], 16)] = (int(row[0], 16), "confirmed")
    return t


def ensure_tables(exe: Path) -> str | None:
    """Generate build/pc_analysis tables when missing or older than the committed pairs.

    Returns None when ready, else a one-line reason."""
    needed = [OUT_DIR / "gc_pc_anchors.tsv", OUT_DIR / "pc_functions.tsv"]
    seed = pc_gc_anchors.CONFIRMED.stat().st_mtime if pc_gc_anchors.CONFIRMED.exists() else 0
    if all(p.exists() and p.stat().st_mtime >= seed for p in needed):
        return None
    result = subprocess.run([sys.executable, str(ROOT / "tools/pc_gc_anchors.py"), "--exe", str(exe)],
                            cwd=ROOT, capture_output=True, text=True)
    if result.returncode != 0 or not all(p.exists() for p in needed):
        return ("`python3 tools/pc_gc_anchors.py` failed: "
                + (result.stderr.strip().splitlines() or ["no output"])[-1])
    return None


class Image:
    """Read-only view of the PE file: sections, imports, strings and constants."""

    def __init__(self, data: bytes):
        self.data = data
        self.sections = pe_layout(data)
        self.imports = parse_imports(data, self.sections)

    def section_of(self, addr: int) -> str | None:
        for name, (va, vsize, _, raw_size) in self.sections.items():
            if va <= addr < va + max(vsize, raw_size):
                return name
        return None

    def read(self, addr: int, n: int) -> bytes | None:
        for va, vsize, raw_off, raw_size in self.sections.values():
            if va <= addr < va + min(vsize, raw_size):
                chunk = self.data[raw_off + addr - va: raw_off + min(addr - va + n, raw_size)]
                return chunk if len(chunk) == n else None
        return None

    def string_at(self, addr: int) -> str | None:
        """Text of a NUL-terminated ASCII or Shift-JIS string of 3+ characters at addr."""
        if self.section_of(addr) not in {".rdata", ".data"}:
            return None
        va, vsize, _, raw_size = self.sections[self.section_of(addr)]
        raw = self.read(addr, max(0, min(256, va + min(vsize, raw_size) - addr))) or b""
        raw = raw.split(b"\0", 1)[0]
        if len(raw) < 3:
            return None
        if all(0x20 <= b < 0x7F or b in (9, 10, 13) for b in raw):
            return raw.decode("ascii")
        if any(0x81 <= b <= 0x9F or 0xE0 <= b <= 0xFC for b in raw):
            try:
                text = raw.decode("cp932")
            except UnicodeDecodeError:
                return None
            # Shift-JIS text has full-width characters; half-width katakana means stray bytes.
            if all((c.isprintable() or c in "\t\r\n") and not "\uff61" <= c <= "\uff9f" for c in text) \
                    and any(ord(c) > 0x2000 for c in text):
                return text
        return None


def parse_imports(data: bytes, sections: dict) -> dict[int, str]:
    """{IAT slot VA: 'DLL!name'} from the PE import directory."""
    def rva_off(rva: int) -> int | None:
        for va, vsize, raw_off, raw_size in sections.values():
            if va - base <= rva < va - base + max(vsize, raw_size):
                return raw_off + rva - (va - base)
        return None

    try:
        pe = struct.unpack_from("<I", data, 0x3C)[0]
        base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
        imp_rva = struct.unpack_from("<I", data, pe + 24 + 96 + 8)[0]
        off = rva_off(imp_rva) if imp_rva else None
        out: dict[int, str] = {}
        while off is not None:
            ilt, _, _, name_rva, iat = struct.unpack_from("<5I", data, off)
            if not name_rva:
                break
            name_off = rva_off(name_rva)
            dll = data[name_off: data.index(b"\0", name_off)].decode("ascii", "replace") if name_off else "?"
            thunk = rva_off(ilt or iat)
            slot = iat
            while thunk is not None:
                value = struct.unpack_from("<I", data, thunk)[0]
                if not value:
                    break
                if value & 0x80000000:
                    sym = f"#{value & 0xFFFF}"
                else:
                    hint = rva_off(value)
                    sym = data[hint + 2: data.index(b"\0", hint + 2)].decode("ascii", "replace") if hint else "?"
                out[base + slot] = f"{dll.split('.')[0]}!{sym}"
                thunk += 4
                slot += 4
            off += 20
        return out
    except (struct.error, ValueError):
        return {}


def fmt_float(value: float) -> str:
    return f"{value:.7g}"


# --- GC side ------------------------------------------------------------------------

@dataclass
class GcFunction:
    name: str
    address: int
    size: int
    calls: list = field(default_factory=list)       # callee names, every call site in order
    globals: list = field(default_factory=list)     # data symbol addresses, first use order
    constants: set = field(default_factory=set)     # of those, read-only data (.rodata/.sdata2, strings)
    floats: list = field(default_factory=list)      # float constants loaded, first use order
    branches: tuple = (0, 0)                        # (conditional branches, backward ones)


def gc_function(target: str, symbols=None) -> GcFunction | None:
    """GC function by name or 0x address, with its call sites, globals and branch counts."""
    from string_xrefs import DOL, SYMBOLS, parse_dol_sections, parse_symbols
    symbols = symbols or parse_symbols(SYMBOLS)
    funcs = [s for s in symbols if s.section in {".init", ".text"} and "type:function" in s.meta]
    if target.lower().startswith("0x"):
        addr = int(target, 16)
        fn = next((s for s in funcs if s.address <= addr < s.address + max(s.size, 4)), None)
    else:
        fn = next((s for s in funcs if s.name == target), None)
    if fn is None:
        return None
    out = GcFunction(fn.name, fn.address, fn.size)
    if not DOL.exists():
        return out
    data = DOL.read_bytes()
    text = next((s for s in parse_dol_sections(DOL) if s.name == ".text"), None)
    by_addr = {s.address: s.name for s in funcs}
    cond = back = 0
    if text and text.address <= fn.address < text.address + text.size:
        for addr in range(fn.address, fn.address + fn.size, 4):
            word = struct.unpack_from(">I", data, text.offset + addr - text.address)[0]
            op = word >> 26
            if op == 18 and word & 3 == 1:
                li = word & 0x03FFFFFC
                if li & 0x02000000:
                    li -= 0x04000000
                dest = (addr + li) & 0xFFFFFFFF
                out.calls.append(by_addr.get(dest, f"0x{dest:08X}"))
            elif op == 16 and (word >> 21) & 0x14 != 0x14:  # bc, not branch-always
                cond += 1
                bd = word & 0xFFFC
                back += bool(bd & 0x8000)
    out.branches = (cond, back)
    try:
        from pc_gc_features import gc_features
        feat = gc_features(float_refs=True)
        out.globals = list(feat.refs.get(fn.name, []))
        for value in feat.floats.get(fn.name, []):
            if fmt_float(value) not in out.floats:
                out.floats.append(fmt_float(value))
    except (OSError, StopIteration):
        pass
    sections = {s.address: s.section for s in symbols}
    out.constants = {g for g in out.globals if sections.get(g) in {".rodata", ".sdata2"}}
    return out


# --- x86 side ----------------------------------------------------------------------

@dataclass
class Insn:
    addr: int
    mnem: str
    ops: str


def objdump_range(exe: Path, start: int, stop: int) -> list[Insn]:
    objdump = shutil.which("objdump")
    if objdump is None or stop <= start:
        return []
    r = subprocess.run([objdump, "-d", "-M", "intel", "--no-show-raw-insn", "-j", ".text",
                        f"--start-address=0x{start:x}", f"--stop-address=0x{stop:x}", str(exe)],
                       capture_output=True, text=True)
    return parse_objdump(r.stdout) if r.returncode == 0 else []


def parse_objdump(text: str) -> list[Insn]:
    out = []
    for line in text.splitlines():
        m = LINE_RE.match(line)
        if m:
            out.append(Insn(int(m.group(1), 16), m.group(2), m.group(3)))
    return out


def branch_target(insn: Insn) -> int | None:
    if insn.mnem.startswith("j") or insn.mnem.startswith("loop"):
        m = TARGET_RE.match(insn.ops)
        if m:
            return int(m.group(1), 16)
    return None


@dataclass
class Switch:
    site: int
    table: int
    cases: int            # dword entries
    targets: list         # entry targets in table order
    bound: int | None     # cmp bound + 1 (index values handled), when found
    index_table: int | None = None
    index_len: int = 0
    first_case: int = 0   # value subtracted before the bound check


def find_switches(insns: list[Insn], image: Image, lo: int, hi: int) -> list[Switch]:
    """`jmp DWORD PTR [reg*4+table]` dispatches, with MSVC's optional byte index table."""
    out = []
    for i, insn in enumerate(insns):
        if insn.mnem != "jmp":
            continue
        m = TABLE_RE.search(insn.ops)
        if not m:
            continue
        reg, table = m.group(1), int(m.group(2), 16)
        bound = index_table = None
        first_case = 0
        index_reg = reg
        for prev in reversed(insns[max(0, i - 8): i]):
            bm = BYTE_INDEX_RE.match(prev.ops)
            if prev.mnem.startswith("mov") and bm and bm.group(1) in (reg, "e" + reg[-2:]) and index_table is None:
                index_table, index_reg = int(bm.group(3), 16), bm.group(2)
                continue
            cm = CMP_RE.match(prev.ops)
            if prev.mnem == "cmp" and cm and cm.group(1) == index_reg:
                bound = int(cm.group(2), 16) + 1
                j = insns.index(prev)
                for bias in reversed(insns[max(0, j - 3): j]):
                    am = CMP_RE.match(bias.ops)
                    if am and am.group(1) == index_reg and bias.mnem in ("add", "sub"):
                        value = int(am.group(2), 16)
                        first_case = (-value if value < 0x80000000 else 0x100000000 - value) \
                            if bias.mnem == "add" else value
                        break
                break
        index_len = 0
        entries = None
        if index_table is not None and bound:
            raw = image.read(index_table, bound)
            if raw:
                index_len = bound
                entries = max(raw) + 1
        limit = entries or bound or 512
        targets = []
        for k in range(limit):
            raw = image.read(table + 4 * k, 4)
            value = struct.unpack("<I", raw)[0] if raw else None
            if value is None or not lo <= value < hi:
                break
            targets.append(value)
        out.append(Switch(insn.addr, table, len(targets), targets, bound, index_table, index_len, first_case))
    return out


@dataclass
class Body:
    start: int
    end: int
    insns: list
    switches: list
    note: str = ""


def extract_body(exe: Path, image: Image, start: int, size: int, disasm=objdump_range) -> Body:
    """Instructions of the function at start, cut before jump tables and unreached code after padding."""
    end = start + max(size, 1)
    insns = disasm(exe, start, end)
    switches = find_switches(insns, image, start, end)
    regions = []
    for s in switches:
        if start <= s.table < end and s.cases:
            regions.append((s.table, s.table + 4 * s.cases))
        if s.index_table is not None and start <= s.index_table < end:
            regions.append((s.index_table, s.index_table + s.index_len))
    if regions:
        code: list[Insn] = []
        cursor = start
        for lo, hi in sorted(regions):
            if lo > cursor:
                code += disasm(exe, cursor, lo)
            cursor = max(cursor, hi)
        insns = code + (disasm(exe, cursor, end) if cursor < end else [])
    note = ""
    targets: set[int] = set()
    for s in switches:
        targets.update(s.targets)
    # Code after an int3 run that nothing earlier branches to is another (pointer-only) function.
    for i, insn in enumerate(insns):
        t = branch_target(insn)
        if t is not None:
            targets.add(t)
        if insn.mnem != "int3" or i == 0:
            continue
        prev = next((p for p in reversed(insns[:i]) if p.mnem not in PADDING), None)
        nxt = next((n for n in insns[i:] if n.mnem not in PADDING), None)
        if prev is None or nxt is None or not prev.mnem.startswith(TERMINATORS):
            continue
        later = {branch_target(x) for x in insns if x.addr < nxt.addr}
        if nxt.addr not in later and nxt.addr not in targets:
            note = (f"the x86 extent continues at 0x{nxt.addr:08X}, which nothing above reaches"
                    " (probably a function only reached through a pointer); listing stops there")
            insns = [x for x in insns if x.addr < nxt.addr]
            end = nxt.addr
            break
    while insns and insns[-1].mnem in PADDING:
        insns.pop()
    return Body(start, end, insns, switches, note)


# --- rendering -----------------------------------------------------------------------------

class Namer:
    def __init__(self, tables: Tables, gc_names: dict[int, str]):
        self.t, self.gc_names = tables, gc_names

    def gc_name(self, gc_addr: int) -> str:
        return self.gc_names.get(gc_addr, f"fn_{gc_addr:08X}")

    def function(self, pc: int) -> tuple[str, str]:
        """(display name, comment) for an x86 function address."""
        if pc in self.t.pc_to_gc:
            g, kind = self.t.pc_to_gc[pc]
            flag = "" if kind in TRUSTED else f" inferred:{kind}"
            weak = self.t.audit.get(g, "")
            if weak.startswith("weak"):
                flag += " WEAK"
            return self.gc_name(g), f"x86 0x{pc:X}{flag}"
        helper = self.t.callers.get(pc, 0) >= pc_gc_anchors.HELPER_CALLERS
        return f"sub_{pc:X}", "unpaired" + (f", {self.t.callers[pc]} callers (runtime helper?)" if helper else "")

    def data(self, addr: int, image: Image, insn: Insn) -> str | None:
        if addr in image.imports:
            return image.imports[addr]
        if addr in self.t.globals:
            g, source = self.t.globals[addr]
            return self.gc_name_data(g) + ("" if source == "confirmed" else " (global pair)")
        text = image.string_at(addr)
        if text is not None:
            return '"' + text.encode("unicode_escape").decode("ascii")[:70] + '"'
        width = 8 if "QWORD PTR" in insn.ops else 4 if "DWORD PTR" in insn.ops else 0
        if width and image.section_of(addr) == ".rdata" and (
                insn.mnem.startswith("f") or insn.mnem.endswith(("ss", "sd"))):
            raw = image.read(addr, width)
            if raw:
                return f"={fmt_float(struct.unpack('<f' if width == 4 else '<d', raw)[0])}"
        return None

    def gc_name_data(self, g: int) -> str:
        return self.gc_names.get(g, f"lbl_{g:08X}")


def render(gc: GcFunction, pc: int, method_name: str, evidence: str, body: Body, image: Image,
           tables: Tables, gc_names: dict[int, str]) -> str:
    namer = Namer(tables, gc_names)
    lo, hi = body.start, body.end
    out = [
        f"; Japanese PC 1.0 (x86) body paired with GC {gc.name} @ 0x{gc.address:08X} (0x{gc.size:X} bytes)",
        f"; x86 0x{pc:08X}..0x{hi:08X} (0x{hi - pc:X} bytes), pairing method: {method_name}",
        f";   evidence: {evidence[:120]}",
    ]
    audit = tables.audit.get(gc.address, "")
    if method_name not in TRUSTED:
        out.append(f"; !!! INFERRED PAIR ({method_name}): not confirmed. Check that the calls, globals and"
                   " constants below agree with the GC assembly before relying on it.")
    if audit:
        out.append(f"; independent check (pc_gc_anchors.py --audit): {audit}")
    out += [f"; {CAVEAT}", "; Derived from the retail exe: keep local, never commit or paste into source."]
    if body.note:
        out.append(f"; note: {body.note}")

    # Calls, aligned.
    calls: list[tuple[int, str]] = []   # (x86 target or IAT slot, token)
    for insn in body.insns:
        if insn.mnem == "jmp":   # tail call to another function
            t = branch_target(insn)
            if t is not None and not lo <= t < hi and (t in tables.extents or t in tables.pc_to_gc):
                calls.append((t, namer.function(t)[0]))
            continue
        if insn.mnem != "call":
            continue
        m = TARGET_RE.match(insn.ops)
        if m:
            t = int(m.group(1), 16)
            calls.append((t, namer.function(t)[0]))
        else:
            dm = re.search(r"ds:0x([0-9a-f]+)", insn.ops)
            if dm and int(dm.group(1), 16) in image.imports:
                calls.append((int(dm.group(1), 16), image.imports[int(dm.group(1), 16)]))
    xs = [name for _, name in calls]
    gs = list(gc.calls)
    out += ["", f"; == Calls in order: x86 {len(xs)} | GC {len(gs)} =="]
    rows = []
    sm = difflib.SequenceMatcher(None, xs, gs, autojunk=False)
    for tag, i0, i1, j0, j1 in sm.get_opcodes():
        if tag == "equal":
            rows += [(f"{xs[i]}", gs[j0 + k], "") for k, i in enumerate(range(i0, i1))]
            continue
        n = max(i1 - i0, j1 - j0)
        for k in range(n):
            x = xs[i0 + k] if i0 + k < i1 else "-"
            g = gs[j0 + k] if j0 + k < j1 else "-"
            if x == "-":
                why = "GC only (inlined on x86, or a GC-only call)"
            elif g == "-":
                why = "x86 only (PC-only code, inlined on GC, or a runtime helper)"
            elif x.startswith("sub_"):
                why = "unpaired x86 callee at this GC call's position (pair lead)"
            else:
                why = "DIFFERS"
            rows.append((x, g, why))
    if rows:
        width = max(len(r[0]) for r in rows) + 2
        out.append(f";   {'x86':<{width}}{'GC':<{max(len(r[1]) for r in rows) + 2}}")
        gw = max(len(r[1]) for r in rows) + 2
        for x, g, why in rows:
            out.append(f";   {x:<{width}}{g:<{gw}}{('<- ' + why) if why else ''}".rstrip())
        agree = sum(1 for r in rows if not r[2])
        out.append(f";   {agree} of {len(rows)} aligned call sites agree")
    else:
        out.append(";   (no direct calls on either side)")

    # Globals, strings, floats, immediates.
    x_globals: list[int] = []
    strings: list[str] = []
    floats: list[str] = []
    data_lo = min((v[0] for k, v in image.sections.items() if k in {".rdata", ".data"}), default=0)
    notes: dict[int, list[str]] = {}
    for insn in body.insns:
        if insn.mnem.startswith("j") or insn.mnem == "call":
            continue
        for h in HEX_RE.findall(insn.ops):
            value = int(h, 16)
            sect = image.section_of(value)
            if sect in {".rdata", ".data"} and value >= data_lo:
                label = namer.data(value, image, insn)
                if label:
                    notes.setdefault(insn.addr, []).append(label)
                    if label.startswith('"'):
                        if label not in strings:
                            strings.append(label)
                    elif label.startswith("="):
                        if label[1:] not in floats:
                            floats.append(label[1:])
                if not (label or "").startswith(('"', "=")) and value not in image.imports and value not in x_globals:
                    x_globals.append(value)
            elif len(h) >= 8 and sect is None:
                f = nice_float(value)
                if f is not None:
                    notes.setdefault(insn.addr, []).append(f"0x{value:x}={fmt_float(f)}f?")
                    if fmt_float(f) + "?" not in floats:
                        floats.append(fmt_float(f) + "?")
    out += ["", f"; == Globals: x86 {len(x_globals)} | GC {len(gc.globals)} =="]
    used_gc = set()
    for x in x_globals:
        if x in tables.globals:
            g, source = tables.globals[x]
            used_gc.add(g)
            seen = " (GC uses it here too)" if g in gc.globals else " (GC function does not reference it)"
            out.append(f";   0x{x:08X} = {namer.gc_name_data(g)} [{source}]{seen}")
        else:
            out.append(f";   0x{x:08X} (no GC pair)")
    missing = [g for g in gc.globals if g not in used_gc and g not in gc.constants]
    if missing:
        out.append(";   GC globals with no paired x86 operand here: "
                   + ", ".join(namer.gc_name_data(g) for g in missing[:24])
                   + (" ..." if len(missing) > 24 else ""))
    if strings:
        out += ["", "; == Strings =="] + [f";   {s}" for s in strings]
    if floats or gc.floats:
        out += ["", "; == Float constants ('?' = x86 32-bit immediate read as a float) ==",
                ";   x86: " + (", ".join(floats) or "-"), ";   GC:  " + (", ".join(gc.floats) or "-")]
    x_cond = sum(1 for i in body.insns if i.mnem.startswith("j") and i.mnem != "jmp" and branch_target(i))
    x_back = sum(1 for i in body.insns if i.mnem.startswith("j") and i.mnem != "jmp"
                 and (branch_target(i) or 1 << 32) <= i.addr)
    out += ["", f"; == Conditional branches: x86 {x_cond} ({x_back} backward) | GC {gc.branches[0]}"
                f" ({gc.branches[1]} backward) =="]
    if body.switches:
        out += ["", "; == Switch tables =="]
        for s in body.switches:
            distinct = len(set(s.targets))
            extra = f", byte index table 0x{s.index_table:08X} ({s.index_len} values)" if s.index_table else ""
            bound = (f", case values {s.first_case}..{s.first_case + s.bound - 1}"
                     f" (anything else goes to the default)") if s.bound else ""
            out.append(f";   jmp at 0x{s.site:08X}: table 0x{s.table:08X}, {s.cases} entries,"
                       f" {distinct} distinct targets{bound}{extra}")

    # Listing.
    labels = {t for i in body.insns if (t := branch_target(i)) is not None and lo <= t < hi}
    for s in body.switches:
        labels.update(t for t in s.targets if lo <= t < hi)
    case_of: dict[int, list[str]] = {}
    for n, s in enumerate(body.switches):
        for k, t in enumerate(s.targets):
            case_of.setdefault(t, []).append(f"switch{n} case {k}" if s.index_table is None else f"switch{n} entry {k}")
    out += ["", "; == Listing (objdump -M intel; .Lxxxxxx = local labels) ==", ""]
    for insn in body.insns:
        if insn.addr in labels:
            extra = f"   ; {', '.join(case_of[insn.addr][:6])}" if insn.addr in case_of else ""
            out.append(f".L{insn.addr:x}:{extra}")
        ops, comment = insn.ops, list(notes.get(insn.addr, []))
        t = branch_target(insn)
        if t is not None:
            if lo <= t < hi:
                ops = f".L{t:x}"
            else:
                name, why = namer.function(t) if t in tables.extents or t in tables.pc_to_gc else (f"0x{t:x}", "")
                ops = name
                comment.append(("tail call, " if insn.mnem == "jmp" else "outside, ") + why if why else "outside")
        elif insn.mnem == "call":
            m = TARGET_RE.match(insn.ops)
            if m:
                name, why = namer.function(int(m.group(1), 16))
                ops = name
                comment.append(why)
            else:
                dm = re.search(r"ds:0x([0-9a-f]+)", insn.ops)
                if dm and int(dm.group(1), 16) in image.imports:
                    comment.append(image.imports[int(dm.group(1), 16)])
        elif insn.mnem == "jmp" and TABLE_RE.search(insn.ops):
            n = next((k for k, s in enumerate(body.switches) if s.site == insn.addr), None)
            if n is not None:
                comment.append(f"switch{n}: {body.switches[n].cases} entries")
        line = f"  {insn.addr:06x}  {insn.mnem:<7}{ops}"
        if comment:
            line = f"{line:<56} ; {'; '.join(comment)}"
        out.append(line.rstrip())
    return "\n".join(out) + "\n"


# --- driver ---------------------------------------------------------------------------------

def body_for(target: str, exe: Path | None = None) -> tuple[str | None, str]:
    """Return (listing text or None, one-line status) for a GC function name or address."""
    exe = exe or pc_gc_anchors.find_exe()
    if exe is None:
        return None, "no x86 body: the Japanese PC exe is not present here or in the primary checkout"
    if shutil.which("objdump") is None:
        return None, "no x86 body: system objdump (binutils with pei-i386) is not installed"
    problem = ensure_tables(exe)
    if problem:
        return None, f"no x86 body: {problem}"
    from string_xrefs import SYMBOLS, parse_symbols
    symbols = parse_symbols(SYMBOLS)
    gc = gc_function(target, symbols)
    if gc is None:
        return None, f"no x86 body: {target} is not a GC function in symbols.txt"
    tables = load_tables()
    if gc.address not in tables.gc_to_pc:
        return None, f"no x86 body: {gc.name} has no JP PC pair in build/pc_analysis/gc_pc_anchors.tsv"
    pc, kind, evidence = tables.gc_to_pc[gc.address]
    size = tables.extents.get(pc)
    if not size:
        i = bisect.bisect_right(tables.starts, pc)
        size = (tables.starts[i] - pc) if i < len(tables.starts) else 0x400
    image = Image(exe.read_bytes())
    body = extract_body(exe, image, pc, size)
    if not body.insns:
        return None, f"no x86 body: objdump produced nothing for 0x{pc:08X}"
    gc_names = {s.address: s.name for s in symbols}
    text = render(gc, pc, kind, evidence, body, image, tables, gc_names)
    status = f"x86 0x{pc:08X} ({kind}{', inferred' if kind not in TRUSTED else ''}), {len(body.insns)} instructions"
    return text, status


def write_task_body(name: str, task_dir: Path, exe: Path | None = None) -> tuple[Path | None, str]:
    """Write task_dir/jp_x86.s; never raises. Returns (path or None, one-line status)."""
    try:
        text, status = body_for(name, exe)
    except Exception as exc:  # optional evidence must never break a task pack
        return None, f"no x86 body: {type(exc).__name__}: {exc}"
    if text is None:
        return None, status
    path = task_dir / "jp_x86.s"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text)
    return path, status


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("targets", nargs="+", help="GC function names or 0x addresses")
    ap.add_argument("--out-dir", type=Path, default=ROOT / "work", help="writes OUT_DIR/NAME/jp_x86.s")
    ap.add_argument("--stdout", action="store_true", help="print instead of writing files")
    ap.add_argument("--exe", type=Path, help="Japanese PC exe (default: found like pc_gc_anchors.py)")
    args = ap.parse_args(argv)
    for target in args.targets:
        if args.stdout:
            text, status = body_for(target, args.exe)
            print(text if text else status)
            continue
        from string_xrefs import SYMBOLS, parse_symbols
        gc = gc_function(target, parse_symbols(SYMBOLS))
        name = gc.name if gc else target
        path, status = write_task_body(name, args.out_dir / name, args.exe)
        shown = path.relative_to(ROOT) if path and path.is_relative_to(ROOT) else path
        print(f"wrote {shown}: {status}" if path else f"{name}: {status}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
