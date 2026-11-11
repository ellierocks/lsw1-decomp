#!/usr/bin/env python3
"""Compare a unit's compiled `.rodata` layout with its retail range and say why it differs.

objdiff counts a literal relocation only when the literal's value *and* section offset match
retail (`functionRelocDiffs=all`). A function that passes `verify_fn.py --strict-symbols` can
therefore sit below 100% because its unit emits literals at other offsets. This tool measures
that gap and classifies its causes.

    python3 tools/rodata_layout.py gap [--json]          # strict-only functions by unit, with bytes
    python3 tools/rodata_layout.py diff UNIT [--json]    # retail vs built layout for one unit
    python3 tools/rodata_layout.py survey [--json]       # classify every unit that `gap` lists
    python3 tools/rodata_layout.py placeholders UNIT START END   # C for unreferenced retail bytes

`UNIT` is the split path without extension (`auto/numath/nutrig`). Run `bash build.sh` first;
nothing is rebuilt here. Retail bytes come from the dtk target object (`build/GL5E4F/obj`), so
everything this prints is derived from retail and belongs under the ignored `work/` if saved.

Items are carved from the bytes and the relocations that point at them. The instruction at a
relocation site gives the literal's width (`lfs`/`lwz` 4 bytes, `lfd` 8); `addi` marks an
address-taken object (string, table). Strings are NUL-terminated printable runs. A path string
ending in `.h` is a header copy (VuMath.h's assert strings are emitted once per including TU,
at the start of its `.rodata`, so each copy also marks a TU start); the unreferenced string after
it belongs to the copy. A `.c`/`.cpp` path is a TU's `__FILE__`.

Each retail item that the build does not reproduce at its offset is classified as:

    wrong order      the build emits the same literal elsewhere
    missing          retail code of this unit references it, the build does not emit it
    extra            the build emits it, retail has nothing equal (built-only item)
    padding          zero bytes before an 8-aligned item or a TU start
    stripped         no relocation anywhere in the DOL reaches it: a pool literal of a
                     function the SN linker dead-stripped (its code is not in the DOL)
    other TU         only code of another unit references it
    header copy      a shared header string copy (VuMath.h); `extra TU` when the unit holds
                     more than one copy, i.e. it spans several TUs

Ranges, splits and the oracle are never changed here. A classification is a lead, not proof:
confirm it against the retail bytes before acting on it (docs/build_config.md).
"""
from __future__ import annotations

import argparse
import difflib
import json
import re
import struct
import sys
from collections import Counter, defaultdict
from dataclasses import asdict, dataclass, field
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build" / "GL5E4F"
SPLITS = ROOT / "config" / "GL5E4F" / "splits.txt"
SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
REPORT = BUILD / "report.json"

STT_SECTION = 3
HEADER_RE = re.compile(rb"^\.\./[\x20-\x7e]*\.h$")
PATH_RE = re.compile(rb"^\.\./[\x20-\x7e]*\.(c|cpp)$")
LIT_DOUBLE_OPS = {50, 54, 51, 55}        # lfd, stfd, lfdu, stfdu
LIT_WORD_OPS = {48, 49, 32, 33, 52, 36}  # lfs, lfsu, lwz, lwzu, stfs, stw
ADDR_OPS = {14, 15, 24}                  # addi, addis, ori: address-taken object


# --------------------------------------------------------------------------- model

@dataclass
class Item:
    offset: int
    size: int
    kind: str          # f32 | f64 | word | str | header | path | table | zero
    data: bytes
    refs: list[str] = field(default_factory=list)  # functions of this unit
    other: list[str] = field(default_factory=list)  # units elsewhere that reference it
    dol_refs: int = 0  # relocations anywhere that reach it (-1: unknown)

    def key(self) -> tuple:
        # Referenced and unreferenced copies of one value must not align with each other:
        # a stripped 1.0f next to a used 1.0f would otherwise hide the real divergence.
        return (self.kind if self.kind != "zero" else "word", self.data, bool(self.refs))

    def text(self) -> str:
        if self.kind in ("str", "header", "path"):
            return repr(self.data.rstrip(b"\0").decode("latin-1"))
        if self.kind == "f32":
            return f"{struct.unpack('>f', self.data)[0]!r}f"
        if self.kind == "f64":
            return repr(struct.unpack(">d", self.data)[0])
        return self.data.hex()

    def as_json(self) -> dict:
        d = asdict(self)
        d["data"] = self.data.hex()
        d["value"] = self.text()
        return d


def _string_at(data: bytes, off: int) -> int:
    """Length of a NUL-terminated printable string at off (without the NUL), or 0."""
    end = off
    while end < len(data) and (0x20 <= data[end] < 0x7F or data[end] in (9, 10, 13)):
        end += 1
    if end - off >= 4 and end < len(data) and data[end] == 0:
        return end - off
    return 0


def carve(data: bytes, base: int, refs: dict[int, tuple[int, set[str]]] | None = None,
          reached: set[int] | None = None) -> list[Item]:
    """Split a `.rodata` blob into literal items.

    `refs` maps an offset to (width hint, referencing functions); the width hint is 8 for a
    double load, 4 for a word, 0 for an address-taken object. `base` is the absolute address
    of offset 0, needed for 8-byte alignment. `reached` is the set of offsets any relocation
    in the DOL reaches; with it, unreferenced items get `dol_refs` 0, otherwise -1 (unknown).
    """
    refs = refs or {}
    items: list[Item] = []
    off = 0
    starts = sorted(refs)
    while off < len(data):
        width, fns = refs.get(off, (None, set()))
        n = _string_at(data, off) if off % 4 == 0 else 0
        if n and n < 8 and width != 0:
            n = 0  # a short printable run is usually a float (0x3F666666 reads "?fff")
        if n:
            raw = data[off:off + n + 1]
            body = raw[:-1]
            kind = "header" if HEADER_RE.match(body) else "path" if PATH_RE.match(body) else "str"
            items.append(Item(off, n + 1, kind, raw, sorted(fns)))
            off += n + 1
            pad = (-off) % 4
            if pad and data[off:off + pad] == bytes(pad):
                items.append(Item(off, pad, "zero", data[off:off + pad]))
                off += pad
            continue
        if off % 4:
            items.append(Item(off, 1, "word", data[off:off + 1], sorted(fns)))
            off += 1
            continue
        nxt = next((s for s in starts if s > off), len(data))
        if width == 0 and nxt - off > 8:
            size = nxt - off
            items.append(Item(off, size, "table", data[off:off + size], sorted(fns)))
            off += size
            continue
        word = data[off:off + 4]
        is_double = ((base + off) % 8 == 0 and off + 8 <= len(data)
                     and (width == 8 or (width is None and word == b"\x43\x30\x00\x00"))
                     and (off + 4) not in refs)
        if is_double:
            items.append(Item(off, 8, "f64", data[off:off + 8], sorted(fns)))
            off += 8
            continue
        if len(word) < 4:
            items.append(Item(off, len(word), "word", word, sorted(fns)))
            break
        kind = "zero" if word == bytes(4) and not fns else "f32" if width in (4, None) else "word"
        items.append(Item(off, 4, kind, word, sorted(fns)))
        off += 4
    # A header path and the unreferenced string right after it form one copy.
    for i, it in enumerate(items):
        if it.kind == "header":
            j = i + 1
            while j < len(items) and items[j].kind == "zero":
                j += 1
            if j < len(items) and items[j].kind == "str" and not items[j].refs:
                items[j].kind = "header"
    if reached is not None:
        for it in items:
            it.dol_refs = sum(1 for o in reached if it.offset <= o < it.offset + it.size)
    return items


def needs_eight(items: list[Item], index: int, base: int) -> bool:
    """True when the item after a zero run at `index` starts 8-aligned and needs it."""
    for it in items[index + 1:]:
        if it.kind == "zero":
            continue
        return (base + it.offset) % 8 == 0 and it.kind in ("f64", "header", "path")
    return False


# --------------------------------------------------------------------------- comparison

@dataclass
class Mismatch:
    offset: int
    size: int
    side: str          # retail | built
    cls: str
    value: str
    refs: list[str]


def classify(retail: list[Item], built: list[Item], base: int, unit_path: str = "") -> dict:
    """Align the two item lists and classify each retail/built item that does not line up."""
    rk, bk = [i.key() for i in retail], [i.key() for i in built]
    sm = difflib.SequenceMatcher(a=rk, b=bk, autojunk=False)
    out: list[Mismatch] = []
    built_keys = Counter(bk)
    retail_keys = Counter(rk)
    headers = [i for i in retail if i.kind == "header" and HEADER_RE.match(i.data.rstrip(b"\0"))]
    first_header = headers[0].offset if headers else None
    stem = Path(unit_path).name
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            continue
        for it in retail[i1:i2]:
            if it.kind == "zero" and not it.refs:
                cls = "padding" if needs_eight(retail, retail.index(it), base) else (
                    "stripped" if it.dol_refs == 0 else "other TU" if it.other else "padding")
            elif it.kind == "header":
                cls = "header copy" if it.offset == first_header or (
                    first_header is not None and _same_copy(retail, first_header, it.offset)) \
                    else "extra TU"
            elif it.kind == "path" and stem and not it.data.rstrip(b"\0").endswith(
                    (f"/{stem}.c".encode(), f"/{stem}.cpp".encode())) and not it.refs:
                cls = "other TU" if it.other else "stripped" if it.dol_refs == 0 else "other TU"
            elif not it.refs and it.dol_refs == 0:
                cls = "stripped"
            elif not it.refs and it.other:
                cls = "other TU"
            elif built_keys[it.key()]:
                cls = "wrong order"
            else:
                cls = "missing"
            out.append(Mismatch(it.offset, it.size, "retail", cls, it.text(), it.refs))
        for it in built[j1:j2]:
            if it.kind == "zero" and not it.refs:
                cls = "padding"
            elif retail_keys[it.key()]:
                cls = "wrong order"
            else:
                cls = "extra"
            out.append(Mismatch(it.offset, it.size, "built", cls, it.text(), it.refs))
    rb = b"".join(i.data for i in retail)
    bb = b"".join(i.data for i in built)
    first = next((k for k in range(min(len(rb), len(bb))) if rb[k] != bb[k]),
                 None if len(rb) == len(bb) else min(len(rb), len(bb)))
    counts: dict[str, int] = defaultdict(int)
    for m in out:
        if m.side == "retail" or m.cls in ("extra", "wrong order"):
            counts[m.cls] += m.size
    return {"first_divergence": first, "retail_size": len(rb), "built_size": len(bb),
            "mismatches": [asdict(m) for m in out], "bytes_by_class": dict(counts),
            "header_copies": len({_copy_start(retail, h.offset) for h in headers})}


def _copy_start(items: list[Item], off: int) -> int:
    idx = next(i for i, it in enumerate(items) if it.offset == off)
    while idx > 0 and not HEADER_RE.match(items[idx].data.rstrip(b"\0")):
        idx -= 1
    return items[idx].offset


def _same_copy(items: list[Item], first: int, off: int) -> bool:
    return _copy_start(items, off) == _copy_start(items, first)


def function_offsets(retail_fn_refs: dict[str, list[int]], built_fn_refs: dict[str, list[int]]):
    """Per function: retail literal offsets (in relocation order) vs built ones.

    A function whose lists are equal has every literal at its retail section offset, which
    is what objdiff needs to count it at 100%.
    """
    rows = []
    for name in sorted(set(retail_fn_refs) | set(built_fn_refs)):
        r, b = retail_fn_refs.get(name), built_fn_refs.get(name)
        rows.append({"function": name, "retail": r, "built": b,
                     "aligned": r is not None and r == b})
    return rows


def tu_segments(items: list[Item]) -> list[dict]:
    """Cut the range at each header copy and report which functions reference each piece."""
    cuts = [0]
    for i, it in enumerate(items):
        prev = next((p for p in reversed(items[:i]) if p.kind != "zero"), None)
        if it.kind == "header" and (prev is None or prev.kind != "header"):
            j = i
            while (j > 0 and items[j - 1].kind == "zero" and not items[j - 1].refs
                   and items[j - 1].offset % 4 == 0):
                j -= 1  # alignment zeros before a TU's first copy end the previous TU;
                # a string's NUL padding (unaligned start) stays with the string
            if items[j].offset:
                cuts.append(items[j].offset)
    end = items[-1].offset + items[-1].size if items else 0
    cuts = sorted(set(cuts)) + [end]
    segs = []
    for a, b in zip(cuts, cuts[1:]):
        if a == b:
            continue
        part = [it for it in items if a <= it.offset < b]
        fns: list[str] = []
        for it in part:
            for f in it.refs:
                if f not in fns:
                    fns.append(f)
        segs.append({"start": a, "end": b, "functions": fns,
                     "paths": [it.text() for it in part if it.kind == "path"],
                     "stripped_bytes": sum(it.size for it in part
                                           if it.dol_refs == 0 and it.kind not in ("header",)
                                           and not (it.kind == "zero" and needs_eight(items, items.index(it), 0))),
                     "header": any(it.kind == "header" for it in part)})
    return segs


# --------------------------------------------------------------------------- objects

def split_ranges(unit: str) -> dict[str, tuple[int, int]]:
    import verify_unit
    return verify_unit.split_ranges(SPLITS.read_text(), unit) or {}


def _symbol_sections() -> dict[str, tuple[str, int]]:
    out = {}
    for line in SYMBOLS.read_text().splitlines():
        m = re.match(r"^(\S+) = (\.\w+):0x([0-9A-Fa-f]+);", line)
        if m:
            out[m[1]] = (m[2], int(m[3], 16))
    return out


def object_refs(path: Path, base: int | None, symbols: dict[str, tuple[str, int]]):
    """(function -> [(abs address, width)] in relocation order, all absolute .rodata targets).

    `base` is the absolute address of this object's `.rodata` (None when it has none).
    """
    import verify_fn as vf
    elf = vf.Elf(path)
    ro = next((s for s in elf.sections if s.name == ".rodata"), None)
    fns = sorted((s.value, s.value + (s.size or 4), s.name, s.shndx)
                 for s in elf.function_symbols())
    per_fn: dict[str, list[tuple[int, int]]] = defaultdict(list)
    targets: list[int] = []
    for shndx, rels in elf.relocs.items():
        sec = elf.sections[shndx] if shndx < len(elf.sections) else None
        target_sec = sec  # the relocated section
        if target_sec is None:
            continue
        code = target_sec.is_code
        body = elf.section_bytes(target_sec) if code else b""
        for r in rels:
            if r.sym >= len(elf.symbols):
                continue
            sym = elf.symbols[r.sym]
            addr = None
            if ro is not None and base is not None and sym.shndx == ro.index:
                addr = base + (0 if sym.sym_type == STT_SECTION else sym.value) + r.addend
            elif sym.shndx == 0 and sym.name in symbols and symbols[sym.name][0] == ".rodata":
                addr = symbols[sym.name][1] + r.addend
            if addr is None:
                continue
            targets.append(addr)
            if not code:
                continue
            site = r.offset & ~3
            op = struct.unpack(">I", body[site:site + 4])[0] >> 26 if site + 4 <= len(body) else -1
            width = 8 if op in LIT_DOUBLE_OPS else 4 if op in LIT_WORD_OPS else 0 if op in ADDR_OPS else 4
            owner = next((n for a, e, n, ix in fns if ix == shndx and a <= r.offset < e), "?")
            if r.rtype in (4, 6) or op in LIT_DOUBLE_OPS | LIT_WORD_OPS:
                # keep one entry per load: the @l half (or the only half) carries the width
                if r.rtype == 6 and op not in LIT_DOUBLE_OPS | LIT_WORD_OPS:
                    continue
                per_fn[owner].append((addr, width))
    return per_fn, targets


class Dol:
    """Every relocation target in all dtk target objects, with the unit that owns it."""

    def __init__(self):
        self.symbols = _symbol_sections()
        self.reached: dict[int, set[str]] = defaultdict(set)
        text = SPLITS.read_text()
        units = [l.strip().rstrip(":").rsplit(".", 1)[0] for l in text.splitlines()
                 if l and not l[0].isspace() and l.strip().endswith(":") and l.strip() != "Sections:"]
        for unit in units:
            obj = BUILD / "obj" / f"{unit}.o"
            if not obj.is_file():
                continue
            base = split_ranges(unit).get(".rodata", (None, None))[0]
            try:
                _, targets = object_refs(obj, base, self.symbols)
            except Exception:
                continue
            for t in targets:
                self.reached[t].add(unit)


def unit_layout(unit: str, dol: Dol | None = None) -> dict:
    ranges = split_ranges(unit)
    if ".rodata" not in ranges:
        return {"unit": unit, "error": "no .rodata range in splits.txt"}
    start, end = ranges[".rodata"]
    target, built = BUILD / "obj" / f"{unit}.o", BUILD / "src" / f"{unit}.o"
    if not target.is_file():
        return {"unit": unit, "error": f"missing {target.relative_to(ROOT)}; run bash build.sh"}
    import verify_fn as vf
    dol = dol or Dol()

    def section(path):
        elf = vf.Elf(path)
        ro = next((s for s in elf.sections if s.name == ".rodata"), None)
        return elf.section_bytes(ro) if ro else b""

    def refmap(per_fn):
        m: dict[int, tuple[int, set[str]]] = {}
        for fn, lst in per_fn.items():
            for addr, width in lst:
                w, fs = m.get(addr - start, (width, set()))
                fs.add(fn)
                m[addr - start] = (max(w, width) if w else width, fs)
        return m

    r_fn, _ = object_refs(target, start, dol.symbols)
    reached = {a - start for a in dol.reached if start <= a < end}
    retail = carve(section(target), start, refmap(r_fn), reached)
    for it in retail:
        it.other = sorted({u for a, us in dol.reached.items()
                           if start + it.offset <= a < start + it.offset + it.size
                           for u in us if u != unit})
    out = {"unit": unit, "rodata": [start, end], "retail": [i.as_json() for i in retail],
           "segments": tu_segments(retail)}
    if not built.is_file():
        out["error"] = "no compiled object (unit has no source or is not built)"
        return out
    b_fn, _ = object_refs(built, start, dol.symbols)
    built_items = carve(section(built), start, refmap(b_fn))
    out["built"] = [i.as_json() for i in built_items]
    out.update(classify(retail, built_items, start, unit))
    out["functions"] = function_offsets(
        {f: [a - start for a, _ in v] for f, v in r_fn.items()},
        {f: [a - start for a, _ in v] for f, v in b_fn.items()})
    out["text_order"] = _text_order(target, built)
    return out


def _text_order(target: Path, built: Path) -> dict:
    import verify_fn as vf
    t = [s.name for s in sorted(vf.Elf(target).function_symbols(), key=lambda s: s.value)]
    b = [s.name for s in sorted(vf.Elf(built).function_symbols(), key=lambda s: s.value)]
    common = [n for n in b if n in t]
    return {"same": [n for n in t if n in common] == common, "retail": t, "built": b}


# --------------------------------------------------------------------------- gap

def gap_rows() -> list[dict]:
    """Strict-only functions (strict verify passes, report below 100%) with their unit."""
    import strict_gap
    report = json.loads(REPORT.read_text())
    rows = [r for r in strict_gap.candidates(report, 0.0) if strict_gap.strict_match(r["name"])]
    for r in rows:
        r["unit"] = r["unit"].split("/", 1)[1] if r["unit"].startswith("main/") else r["unit"]
    return rows


def group(rows: list[dict]) -> list[dict]:
    units: dict[str, dict] = {}
    for r in rows:
        u = units.setdefault(r["unit"], {"unit": r["unit"], "functions": [], "bytes": 0})
        u["functions"].append(r["name"])
        u["bytes"] += r["size"]
    return sorted(units.values(), key=lambda u: -u["bytes"])


def unit_completeness(report: dict, unit: str, strict_only: set[str]) -> dict:
    for u in report.get("units", []):
        if u["name"] in (unit, f"main/{unit}"):
            fns = u.get("functions", [])
            full = sum(1 for f in fns if f.get("fuzzy_match_percent") == 100)
            left = [f["name"] for f in fns
                    if f.get("fuzzy_match_percent") != 100 and f["name"] not in strict_only]
            return {"functions": len(fns), "full": full, "unmatched": len(left)}
    return {"functions": 0, "full": 0, "unmatched": 0}


# --------------------------------------------------------------------------- CLI

# --------------------------------------------------------------------------- placeholders

def _c_string(raw: bytes) -> str:
    out = []
    for b in raw:
        c = chr(b)
        out.append({"\\": "\\\\", '"': '\\"', "\n": "\\n", "\t": "\\t", "\r": "\\r"}.get(c, c))
    return '"' + "".join(out) + '"'


def placeholder_decls(data: bytes, base: int, start: int, end: int) -> list[str]:
    """C declarations that reproduce data[start:end] verbatim under ProDG's `.rodata` rules.

    For unreferenced retail bytes only (stripped pools, extra header copies). ProDG emits a
    file-scope `static const` object where it is defined, between the surrounding functions'
    constant pools; arrays over 8 bytes go to `.rodata`, smaller ones need the section
    attribute (or they land in `.sdata2`); char arrays and words are 4-aligned, so the NUL
    padding after a string is reproduced and is checked here. 8-byte values are written as
    two words, which keeps them at their retail offsets as long as `start` is.
    """
    if start % 4 or end % 4:
        raise ValueError("placeholder range must be word-aligned")
    decls: list[str] = []
    words: list[int] = []
    run_at = start

    def flush(at: int) -> None:
        nonlocal words
        if words:
            attr = ' __attribute__((section(".rodata")))' if len(words) * 4 <= 8 else ""
            body = ", ".join("0" if w == 0 else f"0x{w:08X}" for w in words)
            decls.append(f"static const u32 unk_{base + run_at:08X}[{len(words)}]{attr} = {{ {body} }};")
        words = []

    off = start
    while off < end:
        n = _string_at(data, off)
        if n >= 8:
            flush(off)
            size = n + 1
            pad_end = min(end, (off + size + 3) & ~3)
            if any(data[off + size:pad_end]):
                raise ValueError(f"non-zero padding after string at +0x{off:X}")
            decls.append(f"static const char unk_{base + off:08X}[] = {_c_string(data[off:off + n])};")
            off = pad_end
            run_at = off
            continue
        if not words:
            run_at = off
        words.append(struct.unpack_from(">I", data, off)[0])
        off += 4
    flush(off)
    return decls



def _print_diff(d: dict) -> None:
    if "error" in d and "retail" not in d:
        print(f"{d['unit']}: {d['error']}")
        return
    s, e = d["rodata"]
    print(f"{d['unit']}  .rodata 0x{s:08X}..0x{e:08X} (0x{e - s:X} bytes)")
    print("retail TU pieces (cut at header copies):")
    for seg in d["segments"]:
        print(f"  +0x{seg['start']:03X}..+0x{seg['end']:03X}  header={'y' if seg['header'] else 'n'}"
              f"  stripped={seg['stripped_bytes']:#x}  paths={','.join(seg['paths']) or '-'}"
              f"  fns={' '.join(seg['functions']) or '-'}")
    if "built" not in d:
        print(f"  {d.get('error')}")
        return
    fd = d["first_divergence"]
    print(f"built 0x{d['built_size']:X} bytes vs retail 0x{d['retail_size']:X}; first divergence "
          + ("none" if fd is None else f"+0x{fd:X} (0x{s + fd:08X})"))
    print("bytes by class: " + ", ".join(f"{k} {v:#x}" for k, v in sorted(d["bytes_by_class"].items())))
    if not d["text_order"]["same"]:
        print("text order: built definition order differs from retail .text order")
    al = [f for f in d["functions"] if f["retail"]]
    print(f"functions with literals: {sum(f['aligned'] for f in al)}/{len(al)} at retail offsets")
    for m in d["mismatches"]:
        print(f"  {m['side']:6} +0x{m['offset']:03X} {m['size']:#5x}  {m['cls']:<12} {m['value'][:48]}"
              + (f"  <- {' '.join(m['refs'])}" if m["refs"] else ""))


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("gap").add_argument("--json", action="store_true")
    p = sub.add_parser("diff")
    p.add_argument("unit")
    p.add_argument("--json", action="store_true")
    sub.add_parser("survey").add_argument("--json", action="store_true")
    p = sub.add_parser("placeholders", help="C declarations for unreferenced retail bytes")
    p.add_argument("unit")
    p.add_argument("start", type=lambda v: int(v, 16), help="unit-relative hex offset")
    p.add_argument("end", type=lambda v: int(v, 16), help="unit-relative hex offset (exclusive)")
    args = ap.parse_args(argv)
    if args.cmd in ("gap", "survey") and not REPORT.is_file():
        print("error: no build/GL5E4F/report.json; run bash build.sh", file=sys.stderr)
        return 2
    if args.cmd == "gap":
        units = group(gap_rows())
        if args.json:
            print(json.dumps(units, indent=2))
        else:
            for u in units:
                print(f"{u['bytes']:7,} {len(u['functions']):4}  {u['unit']}")
            print(f"{sum(u['bytes'] for u in units):7,} {sum(len(u['functions']) for u in units):4}  total")
        return 0
    if args.cmd == "diff":
        d = unit_layout(args.unit)
        print(json.dumps(d, indent=2)) if args.json else _print_diff(d)
        return 0
    if args.cmd == "placeholders":
        start, _ = split_ranges(args.unit)[".rodata"]
        import verify_fn as vf
        elf = vf.Elf(BUILD / "obj" / f"{args.unit}.o")
        data = elf.section_bytes(next(s for s in elf.sections if s.name == ".rodata"))
        print("\n".join(placeholder_decls(data, start, args.start, args.end)))
        return 0
    rows = gap_rows()
    report = json.loads(REPORT.read_text())
    strict = {r["name"] for r in rows}
    dol = Dol()
    summary = []
    for u in group(rows):
        d = unit_layout(u["unit"], dol)
        comp = unit_completeness(report, u["unit"], strict)
        rec = {"unit": u["unit"], "strict_only": len(u["functions"]), "bytes": u["bytes"],
               "complete": comp["unmatched"] == 0, "unmatched": comp["unmatched"]}
        if "retail" in d:
            rec["header_copies"] = sum(1 for s in d["segments"] if s["header"])
            rec["stripped_bytes"] = sum(s["stripped_bytes"] for s in d["segments"])
        if "bytes_by_class" in d:
            rec["classes"] = d["bytes_by_class"]
            rec["text_order_same"] = d["text_order"]["same"]
            rec["aligned_functions"] = sum(f["aligned"] for f in d["functions"] if f["retail"])
        else:
            rec["error"] = d.get("error")
        summary.append(rec)
    if args.json:
        print(json.dumps(summary, indent=2))
        return 0
    for r in summary:
        cls = ", ".join(f"{k} {v:#x}" for k, v in sorted(r.get("classes", {}).items())) or r.get("error", "")
        state = "complete" if r["complete"] else f"{r['unmatched']} left"
        print(f"{r['bytes']:6,} {r['strict_only']:3} {state:>9}"
              f"  hdr={r.get('header_copies', '?')} strip={r.get('stripped_bytes', 0):#x}"
              f" order={'ok' if r.get('text_order_same') else 'diff'}  {r['unit']}: {cls}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
