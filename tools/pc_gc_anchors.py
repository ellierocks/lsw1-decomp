#!/usr/bin/env python3
"""
Pair GameCube functions with PC x86 functions through shared string references.

The Japanese 1.0 PC exe (pc_jp_retail) is the only PC retail build without
SecuROM, so its code can be read statically. Its game code mirrors the GC
function structure closely, so a GC function and the x86 function that both
reference the same distinctive strings are very likely the same source
function. The x86 body then shows call order, globals, constants and field
offsets for the GC function (see docs/research.md#pc-builds).

GC side: lis+addi/ori/load string materialisations (tools/string_xrefs.py)
mapped to configured functions. x86 side: 32-bit immediates in .text equal to
a string's VA, attributed to the nearest preceding function start (call
targets and code after int3/nop padding). Only strings that occur once in
each binary are used.

Outputs (build/pc_analysis/, local):
  gc_pc_anchors.tsv     GC function -> x86 function, shared string count, sample
  pc_source_files.tsv   x86 source-path string -> referencing x86 functions and
                        their anchored GC functions (split evidence)

Usage:
    python3 tools/pc_gc_anchors.py [--exe PATH] [--min-len N]
"""

from __future__ import annotations

import argparse
import bisect
import hashlib
import os
import random
import re
import struct
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

from pc_gc_features import gc_features, pc_features, x86_disassembly, x86_listing  # noqa: E402
from string_xrefs import (  # noqa: E402
    DOL,
    SYMBOLS,
    Symbol,
    containing_function,
    parse_dol_sections,
    parse_symbols,
    scan_section_for_xrefs,
)

PC_EXE_CANDIDATES = [
    ROOT / "orig/pc/jpn/retail/LEGO Star Wars Game/LegoStarwars.exe",
]
PC_JP_SHA256 = "e224197c7ca58891d8bc0f3da6a3d29ed9ff213f9b8ef52e3708ec0db2f0921b"
CONFIRMED = ROOT / "docs/symbol_donors/pc_jp_pairs.tsv"
OUT_DIR = ROOT / "build/pc_analysis"
STRING_RE = re.compile(rb"[\x20-\x7e]{%d,}\x00")
SOURCE_PATH_RE = re.compile(r"^\.\\.*\.(c|cpp|h)$", re.IGNORECASE)


# --- GameCube -------------------------------------------------------------

def gc_strings(data: bytes, min_len: int) -> dict[int, str]:
    """Return {address: text} for NUL-terminated ASCII strings in GC data sections."""
    out: dict[int, str] = {}
    pattern = re.compile(STRING_RE.pattern % min_len)
    for section in parse_dol_sections(DOL):
        if section.name in {".init", ".text"} or section.index < 7:
            continue
        blob = data[section.offset : section.offset + section.size]
        for match in pattern.finditer(blob):
            out[section.address + match.start()] = match.group()[:-1].decode("ascii")
    return out


def gc_string_refs(min_len: int) -> dict[str, set[str]]:
    """Return {string text: {GC function name}} for strings referenced from code."""
    data = DOL.read_bytes()
    strings = gc_strings(data, min_len)
    symbols = parse_symbols(SYMBOLS)
    functions = sorted(
        (s for s in symbols if s.section in {".init", ".text"} and "type:function" in s.meta),
        key=lambda s: s.address,
    )
    starts = [f.address for f in functions]
    targets = {addr: Symbol(text, ".data", addr, len(text) + 1, "") for addr, text in strings.items()}
    refs: dict[str, set[str]] = defaultdict(set)
    for section in parse_dol_sections(DOL):
        if section.name not in {".init", ".text"}:
            continue
        for xref in scan_section_for_xrefs(data, section, targets, functions, starts):
            if xref.function and xref.kind != "lis-exact":
                refs[xref.string.name].add(f"{xref.function.name}@{xref.function.address:08X}")
    counts = Counter(strings.values())
    return {text: fns for text, fns in refs.items() if counts[text] == 1}


def gc_callees() -> tuple[dict[str, list[str]], dict[str, int]]:
    """Return ({GC function: ordered distinct direct callees}, {name: address})."""
    data = DOL.read_bytes()
    text = next(s for s in parse_dol_sections(DOL) if s.name == ".text")
    symbols = parse_symbols(SYMBOLS)
    functions = [s for s in symbols if s.section == ".text" and "type:function" in s.meta]
    by_addr = {f.address: f.name for f in functions}
    calls: dict[str, list[str]] = {}
    for f in functions:
        seq: list[str] = []
        for addr in range(f.address, f.address + f.size, 4):
            word = struct.unpack_from(">I", data, text.offset + addr - text.address)[0]
            if word >> 26 == 18 and word & 3 == 1:
                li = word & 0x03FFFFFC
                if li & 0x02000000:
                    li -= 0x04000000
                callee = by_addr.get((addr + li) & 0xFFFFFFFF)
                if callee and callee not in seq:
                    seq.append(callee)
        calls[f.name] = seq
    return calls, {f.name: f.address for f in functions}


# --- PC x86 ----------------------------------------------------------------

def pe_sections(data: bytes) -> tuple[int, list[tuple[str, int, int, int]]]:
    """Return (image base, [(name, va, raw offset, raw size)])."""
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe : pe + 4] != b"PE\0\0":
        raise SystemExit("not a PE file")
    count = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    image_base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    table = pe + 24 + opt_size
    sections = []
    for i in range(count):
        entry = table + 40 * i
        name = data[entry : entry + 8].rstrip(b"\0").decode("ascii", "replace")
        vsize, rva, raw_size, raw_off = struct.unpack_from("<IIII", data, entry + 8)
        sections.append((name, image_base + rva, raw_off, min(raw_size, vsize or raw_size)))
    return image_base, sections


def pc_string_refs(exe: Path, min_len: int, listing=None) -> tuple[dict[str, set[int]], dict[int, list[int]]]:
    """Return ({string text: {x86 function VA}}, {x86 function VA: [callee VA]}).

    `listing` is (instruction addresses, [(call site, target)]) from
    pc_gc_features.x86_listing; None falls back to a byte scan.
    """
    data = exe.read_bytes()
    _, sections = pe_sections(data)
    names = [s[0] for s in sections]
    if ".securom" in names:
        raise SystemExit(f"{exe} is SecuROM-wrapped; its .text is encrypted on disk")
    text = next(s for s in sections if s[0] == ".text")
    _, text_va, text_off, text_size = text
    code = data[text_off : text_off + text_size]

    pattern = re.compile(STRING_RE.pattern % min_len)
    strings: dict[int, str] = {}
    for name, va, off, size in sections:
        if name in {".rdata", ".data"}:
            for match in pattern.finditer(data[off : off + size]):
                strings[va + match.start()] = match.group()[:-1].decode("ascii")
    counts = Counter(strings.values())

    # Function starts: direct call targets, plus code following int3/nop padding.
    # With a disassembly, only instruction boundaries count and calls are real
    # call instructions; without one, E8 bytes are scanned (some false starts).
    insns = calls_at = None
    if listing is not None:
        insns, calls_at = listing
    starts = {text_va}
    if calls_at is not None:
        starts.update(t for _, t in calls_at if text_va <= t < text_va + text_size and t in insns)
    else:
        for i in range(len(code) - 5):
            if code[i] == 0xE8:
                target = text_va + i + 5 + struct.unpack_from("<i", code, i + 1)[0]
                if text_va <= target < text_va + text_size:
                    starts.add(target)
    for match in re.finditer(rb"(?:\xcc|\x90){2,}", code):
        end = match.end()
        if end < len(code) and code[end] not in (0xCC, 0x90):
            if code[match.start() - 1] in (0xC3, 0xC2) or code[match.start()] == 0xCC:
                if insns is None or text_va + end in insns:
                    starts.add(text_va + end)
    ordered = sorted(starts)

    refs: dict[str, set[int]] = defaultdict(set)
    for addr, string in strings.items():
        if counts[string] != 1:
            continue
        needle = struct.pack("<I", addr)
        pos = code.find(needle)
        while pos >= 0:
            index = bisect.bisect_right(ordered, text_va + pos) - 1
            refs[string].add(ordered[index])
            pos = code.find(needle, pos + 1)

    # Ordered distinct direct callees per function.
    start_set = set(ordered)
    calls: dict[int, list[int]] = {start: [] for start in ordered}
    if calls_at is not None:
        for site, target in calls_at:
            if target in start_set and text_va <= site:
                seq = calls[ordered[bisect.bisect_right(ordered, site) - 1]]
                if target not in seq:
                    seq.append(target)
        return refs, calls
    for n, start in enumerate(ordered):
        end = ordered[n + 1] if n + 1 < len(ordered) else text_va + text_size
        seq = calls[start]
        body = code[start - text_va : end - text_va]
        for i in range(len(body) - 4):
            if body[i] == 0xE8:
                target = start + i + 5 + struct.unpack_from("<i", body, i + 1)[0]
                if target in start_set and target not in seq:
                    seq.append(target)
    return refs, calls


def propagate(pairs: dict[str, tuple[int, str]], gc_calls: dict[str, list[str]],
              pc_calls: dict[int, list[int]], check=None) -> int:
    """Pair callees of paired functions where both call lists align.

    Already-paired callees act as fixed points; a gap between them is paired
    positionally only when both sides have the same number of callees in it
    and it holds one callee (up to three when the whole call lists have equal
    length). Looser rules dropped below 95% on held-out string pairs.
    Conflicting proposals are dropped. `check(gc, pc, pairs)`, when given,
    vetoes a proposal (the full pipeline passes Pairer.consistent). Returns the
    number of new pairs.
    """
    added = 0
    while True:
        proposals: dict[str, set[tuple[int, str]]] = defaultdict(set)
        taken = {pc for pc, _ in pairs.values()}
        for gc_fn, (pc_fn, _) in list(pairs.items()):
            a, b = gc_calls.get(gc_fn, []), pc_calls.get(pc_fn, [])
            for (i0, j0), (i1, j1) in call_gaps(a, b, pairs):
                gap = i1 - i0 - 1
                if i1 - i0 != j1 - j0 or gap > (3 if len(a) == len(b) else 1):
                    continue
                for k in range(1, i1 - i0):
                    g, p = a[i0 + k], b[j0 + k]
                    if g not in pairs and p not in taken:
                        proposals[g].add((p, gc_fn))
        by_pc: dict[int, set[str]] = defaultdict(set)
        for g, props in proposals.items():
            for p, _ in props:
                by_pc[p].add(g)
        new = 0
        for g, props in proposals.items():
            pcs = {p for p, _ in props}
            if len(pcs) == 1:
                p = next(iter(pcs))
                if len(by_pc[p]) == 1 and (check is None or check(g, p, pairs)):
                    parent = sorted(src for _, src in props)[0]
                    pairs[g] = (p, f"callee of {parent}")
                    new += 1
        if not new:
            return added
        added += new


def call_gaps(a: list, b: list, pairs: dict) -> list[tuple[tuple[int, int], tuple[int, int]]]:
    """Return consecutive (GC index, PC index) fixed points of two call lists."""
    fixed = [(-1, -1)]
    for i, g in enumerate(a):
        if g in pairs and pairs[g][0] in b:
            j = b.index(pairs[g][0])
            if j > fixed[-1][1]:
                fixed.append((i, j))
    fixed.append((len(a), len(b)))
    return list(zip(fixed, fixed[1:]))


# Float constants too common to tell functions apart.
COMMON_FLOATS = {0.0, 1.0, -1.0, 0.5, -0.5, 2.0, 0.25, 3.0, 4.0, 10.0, 30.0, 60.0, 100.0, 255.0, 0.1, 0.01}
HELPER_CALLERS = 150   # callees with this many callers (memcpy, ftol...) are ignored when counting calls
LAYOUT_SPAN = 0x8000   # adjacent pairs whose x86 functions are further apart are not a layout gap


def f32(value: float) -> float:
    try:
        return struct.unpack("<f", struct.pack("<f", value))[0]
    except OverflowError:
        return value


def message_strings(strings) -> set[str]:
    """Strings other than source paths (assert paths differ between GC and PC)."""
    return {s for s in strings if not s.lower().endswith((".c", ".cpp", ".h"))}


class Pairer:
    """Pair GC and x86 functions from shared evidence (see main() for the stages).

    Every step proposes (GC function, x86 function) votes; `accept` keeps a
    proposal only if it is the GC function's sole candidate, no other GC
    function wants that x86 function, and (for inferred steps) the pair is
    `consistent`. Precision is measured by `evaluate` on held-out string pairs.
    """

    def __init__(self, gc_strs, pc_strs, gc_calls, pc_calls, gc_addrs, gc_sizes,
                 gc_feat, pc_feat):
        self.gc_calls, self.pc_calls = gc_calls, pc_calls
        self.gc_addrs, self.gc_sizes = gc_addrs, gc_sizes
        self.notes: dict[tuple[str, int], str] = {}
        self.hidden: frozenset = frozenset()  # held-out GC functions (see run)
        self.gc_feat, self.pc_feat = gc_feat, pc_feat
        self.pc_starts = sorted(pc_calls)
        self.pc_sizes = {s: (self.pc_starts[i + 1] - s if i + 1 < len(self.pc_starts) else 0)
                         for i, s in enumerate(self.pc_starts)}
        order = sorted((a, n) for n, a in gc_addrs.items())
        self.gc_order = [n for _, n in order]
        self.gc_order_addr = [a for a, _ in order]
        self.gc_strs: dict[str, set[str]] = defaultdict(set)
        for text, fns in gc_strs.items():
            for fn in fns:
                self.gc_strs[fn.split("@")[0]].add(text)
        self.pc_strs: dict[int, set[str]] = defaultdict(set)
        for text, fns in pc_strs.items():
            for fn in fns:
                self.pc_strs[fn].add(text)
        self.gc_callers = Counter(c for cs in gc_calls.values() for c in cs)
        self.pc_callers = Counter(c for cs in pc_calls.values() for c in cs)
        self.string_votes: dict[str, Counter] = defaultdict(Counter)
        self.string_samples: dict[tuple[str, int], list[str]] = defaultdict(list)
        for text, gc_fns in gc_strs.items():
            pc_fns = pc_strs.get(text)
            if pc_fns and len(gc_fns) == 1 and len(pc_fns) == 1:
                g, p = next(iter(gc_fns)).split("@")[0], next(iter(pc_fns))
                self.string_votes[g][p] += 1
                self.string_samples[(g, p)].append(text)

    # -- checks ------------------------------------------------------------

    def size_ratio(self, g: str, p: int) -> float:
        return self.pc_sizes.get(p, 0) / max(self.gc_sizes.get(g, 1), 1)

    def callee_agreement(self, g: str, p: int, pairs: dict) -> tuple[int, int]:
        """Return (paired GC callees whose x86 partner p calls, those it does not)."""
        pc = set(self.pc_calls.get(p, []))
        agree = bad = 0
        for c in self.gc_calls.get(g, []):
            if c in pairs:
                if pairs[c][0] in pc:
                    agree += 1
                else:
                    bad += 1
        return agree, bad

    def consistent(self, g: str, p: int, pairs: dict, low: float = 0.1, high: float = 8.0) -> bool:
        """No more contradicting than agreeing paired callees, a sane size ratio and no
        disjoint message strings (source paths aside, which differ between builds)."""
        agree, bad = self.callee_agreement(g, p, pairs)
        if not (bad == 0 or agree > bad) or not low <= self.size_ratio(g, p) <= high:
            return False
        sa, sb = self.strings(g, p)
        sa, sb = message_strings(sa), message_strings(sb)
        return not (sa and sb and not sa & sb)

    def strings(self, g: str, p: int) -> tuple[set[str], set[str]]:
        """String sets of a candidate; empty for held-out functions so checks cannot peek."""
        if g in self.hidden:
            return set(), set()
        return self.gc_strs.get(g, set()), self.pc_strs.get(p, set())

    def floats(self, feat, fn) -> set[float]:
        return {f32(v) for v in feat.floats.get(fn, []) if f32(v) not in COMMON_FLOATS}

    def similar(self, g: str, p: int, pairs: dict) -> bool:
        """Shape test for a candidate inside a gap: size, call count, floats, strings."""
        if not self.consistent(g, p, pairs, 0.4, 2.0):
            return False
        a = [c for c in self.gc_calls.get(g, []) if self.gc_callers[c] < HELPER_CALLERS]
        b = [c for c in self.pc_calls.get(p, []) if self.pc_callers[c] < HELPER_CALLERS]
        if abs(len(a) - len(b)) > 1 + max(len(a), len(b)) // 4:
            return False
        fa, fb = self.floats(self.gc_feat, g), self.floats(self.pc_feat, p)
        if (fa or fb) and not fa & fb and max(len(fa), len(fb)) >= 2:
            return False
        sa, sb = self.strings(g, p)
        return not ((sa or sb) and not sa & sb)

    # -- acceptance --------------------------------------------------------

    def accept(self, votes: dict[str, Counter], pairs: dict, check: bool = True) -> int:
        taken = {p for p, _ in pairs.values()}
        by_pc: dict[int, set[str]] = defaultdict(set)
        for g, counter in votes.items():
            for p in counter:
                by_pc[p].add(g)
        added = 0
        for g, counter in votes.items():
            if g in pairs:
                continue
            (p, _), *rest = counter.most_common()
            if rest or p in taken or len(by_pc[p]) != 1:
                continue
            if check and not self.consistent(g, p, pairs):
                continue
            pairs[g] = (p, self.notes.get((g, p), "inferred"))
            taken.add(p)
            added += 1
        return added

    def vote(self, votes: dict, g: str, p: int, note: str) -> None:
        votes[g][p] += 1
        self.notes.setdefault((g, p), note)

    # -- steps ---------------------------------------------------------------

    def step_callees(self, pairs: dict) -> int:
        return propagate(pairs, self.gc_calls, self.pc_calls, self.consistent)

    def step_tables(self, pairs: dict) -> int:
        """Function-pointer tables of equal length that agree at two or more paired slots."""
        def group(pointers):
            tables, cur = [], []
            for addr, fn in sorted(pointers):
                if cur and addr - cur[-1][0] > 16:
                    tables.append(cur)
                    cur = []
                cur.append((addr, fn))
            return tables + ([cur] if cur else [])
        gt, pt = group(self.gc_feat.pointers), group(self.pc_feat.pointers)
        where: dict[int, list[tuple[int, int]]] = defaultdict(list)
        for ti, table in enumerate(pt):
            for i, (_, fn) in enumerate(table):
                where[fn].append((ti, i))
        hits = Counter()
        for gi, table in enumerate(gt):
            for i, (_, g) in enumerate(table):
                if g in pairs:
                    for ti, j in where.get(pairs[g][0], []):
                        if i == j and len(table) == len(pt[ti]):
                            hits[(gi, ti)] += 1
        votes: dict[str, Counter] = defaultdict(Counter)
        for (gi, ti), count in hits.items():
            a, b = gt[gi], pt[ti]
            if count < 2 or any(g in pairs and pairs[g][0] != p for (_, g), (_, p) in zip(a, b)):
                continue
            for (_, g), (_, p) in zip(a, b):
                if g not in pairs:
                    self.vote(votes, g, p, f"table slot at 0x{a[0][0]:08X}")
        return self.accept(votes, pairs)

    def global_pairs(self, pairs: dict) -> dict[int, int]:
        """GC data symbol -> x86 address, where they co-occur in every pair that uses either (2+)."""
        co, gcount, pcount = Counter(), Counter(), Counter()
        for g, (p, _) in pairs.items():
            gs, ps = set(self.gc_feat.refs.get(g, [])), set(self.pc_feat.refs.get(p, []))
            gcount.update(gs)
            pcount.update(ps)
            co.update((x, y) for x in gs for y in ps)
        best: dict[int, list[int]] = defaultdict(list)
        for (x, y), count in co.items():
            if count >= 2 and count == gcount[x] == pcount[y]:
                best[x].append(y)
        single = {x: ys[0] for x, ys in best.items() if len(ys) == 1}
        users = Counter(single.values())
        return {x: y for x, y in single.items() if users[y] == 1}

    def step_globals(self, pairs: dict) -> int:
        """The one unpaired user of a paired global on each side."""
        gusers: dict[int, set[str]] = defaultdict(set)
        pusers: dict[int, set[int]] = defaultdict(set)
        for g, refs in self.gc_feat.refs.items():
            for x in refs:
                gusers[x].add(g)
        for p, refs in self.pc_feat.refs.items():
            for y in refs:
                pusers[y].add(p)
        taken = {p for p, _ in pairs.values()}
        votes: dict[str, Counter] = defaultdict(Counter)
        for x, y in self.global_pairs(pairs).items():
            a = [g for g in gusers[x] if g not in pairs]
            b = [p for p in pusers[y] if p not in taken]
            if len(a) == 1 and len(b) == 1 and self.similar(a[0], b[0], pairs):
                self.vote(votes, a[0], b[0], f"sole other user of global 0x{x:08X} (x86 0x{y:08X})")
        return self.accept(votes, pairs)

    def layout_gaps(self, pairs: dict):
        """Yield (GC functions, x86 functions, note) strictly between address-adjacent pairs."""
        seq = sorted((self.gc_addrs[g], p, g) for g, (p, _) in pairs.items() if g in self.gc_addrs)
        for (ga, pa, gn), (gb, pb, gm) in zip(seq, seq[1:]):
            if not 0 < pb - pa < LAYOUT_SPAN:
                continue
            i0, i1 = bisect.bisect_left(self.gc_order_addr, ga), bisect.bisect_left(self.gc_order_addr, gb)
            j0, j1 = bisect.bisect_left(self.pc_starts, pa), bisect.bisect_left(self.pc_starts, pb)
            yield self.gc_order[i0 + 1 : i1], self.pc_starts[j0 + 1 : j1], f"between {gn} and {gm}"

    def step_layout(self, pairs: dict) -> int:
        """Positional pairing in equal layout gaps of at most two functions."""
        votes: dict[str, Counter] = defaultdict(Counter)
        for a, b, note in self.layout_gaps(pairs):
            if len(a) == len(b) and 1 <= len(a) <= 2:
                for g, p in zip(a, b):
                    self.vote(votes, g, p, f"layout {note}")
        return self.accept(votes, pairs)

    def fuzzy(self, gaps, pairs: dict, max_gc: int, max_pc: int) -> dict[str, Counter]:
        taken = {p for p, _ in pairs.values()}
        votes: dict[str, Counter] = defaultdict(Counter)
        for a, b, note in gaps:
            a = [g for g in a if g not in pairs]
            b = [p for p in b if p not in taken]
            if not a or not b or len(a) > max_gc or len(b) > max_pc:
                continue
            cand = {g: [p for p in b if self.similar(g, p, pairs)] for g in a}
            for g, ps in cand.items():
                if len(ps) == 1 and sum(ps[0] in cand[h] for h in a) == 1:
                    self.vote(votes, g, ps[0], note)
        return votes

    def step_fuzzy_callees(self, pairs: dict) -> int:
        """Unequal call-list gaps: the one candidate of matching shape."""
        def gaps():
            for gc_fn, (pc_fn, _) in list(pairs.items()):
                a, b = self.gc_calls.get(gc_fn, []), self.pc_calls.get(pc_fn, [])
                for (i0, j0), (i1, j1) in call_gaps(a, b, pairs):
                    yield a[i0 + 1 : i1], b[j0 + 1 : j1], f"shape, callee of {gc_fn}"
        return self.accept(self.fuzzy(gaps(), pairs, 6, 8), pairs)

    def step_fuzzy_layout(self, pairs: dict) -> int:
        """Unequal layout gaps: the one candidate of matching shape."""
        gaps = ((a, b, f"shape, layout {note}") for a, b, note in self.layout_gaps(pairs))
        return self.accept(self.fuzzy(gaps, pairs, 8, 12), pairs)

    # -- driver --------------------------------------------------------------

    def run(self, confirmed: dict[str, tuple[int, str]], holdout: frozenset = frozenset()):
        """Return (pairs, ambiguous string rows). Held-out functions get no string votes."""
        self.notes: dict[tuple[str, int], str] = {}
        self.hidden = holdout
        pairs = dict(confirmed)
        owner = {p: g for g, (p, _) in pairs.items()}
        split_rows = []
        for g, counter in sorted(self.string_votes.items(), key=lambda kv: -sum(kv[1].values())):
            if g in pairs or g in holdout:
                continue
            (p, count), *rest = counter.most_common()
            sample = self.string_samples[(g, p)][0][:60]
            if rest or p in owner:
                split_rows.append((g, p, f"strings split {count}/{sum(counter.values())}: {sample}"))
                continue
            pairs[g] = (p, f"strings {count}: {sample}")
            owner[p] = g
        # Exact steps first; each later stage reruns all earlier steps to a fixed point.
        stages = [[self.step_callees, self.step_tables, self.step_globals],
                  [self.step_layout], [self.step_fuzzy_callees], [self.step_fuzzy_layout]]
        steps: list = []
        for stage in stages:
            steps = stage + steps
            while sum(step(pairs) for step in steps):
                pass
        return pairs, split_rows

    def evaluate(self, confirmed: dict, seeds: int = 20, fraction: float = 0.2) -> Counter:
        """Hold out string pairs and count (method, correct) for the ones re-derived."""
        truth = {g: p for g, (p, e) in self.run(confirmed)[0].items() if e.startswith("strings")}
        names = sorted(truth)
        tally: Counter = Counter()
        for seed in range(seeds):
            hold = frozenset(random.Random(seed).sample(names, int(len(names) * fraction)))
            pairs, _ = self.run(confirmed, hold)
            for g in hold:
                if g in pairs:
                    tally[(method(pairs[g][1]), pairs[g][0] == truth[g])] += 1
                else:
                    tally[("unpaired", None)] += 1
        return tally


def method(evidence: str) -> str:
    """Short name of the step that produced an evidence string."""
    for prefix, name in (("confirmed", "confirmed"), ("strings split", "ambiguous"), ("strings", "strings"),
                         ("callee", "callee"), ("table", "table"), ("sole other user", "global"),
                         ("layout", "layout"), ("shape, callee", "shape-callee"),
                         ("shape, layout", "shape-layout")):
        if evidence.startswith(prefix):
            return name
    return "other"


# --- Task-pack evidence -----------------------------------------------------------

def primary_root() -> Path:
    """Return the primary checkout (worktrees have no orig/pc of their own)."""
    try:
        common = subprocess.run(
            ["git", "rev-parse", "--path-format=absolute", "--git-common-dir"],
            cwd=ROOT, capture_output=True, text=True, check=True).stdout.strip()
        return Path(common).parent
    except (OSError, subprocess.CalledProcessError):
        return ROOT


def find_exe() -> Path | None:
    """Return the pc_jp_retail exe from this checkout or the primary one."""
    for root in (ROOT, primary_root()):
        for path in PC_EXE_CANDIDATES:
            candidate = root / path.relative_to(ROOT)
            if candidate.exists():
                return candidate
    return None


def load_anchors(path: Path | None = None) -> dict[int, tuple[int, str]] | None:
    """{GC address: (x86 address, method)} from an existing gc_pc_anchors.tsv, without
    regenerating it; ambiguous string votes are left out. None if it was never generated."""
    path = path or OUT_DIR / "gc_pc_anchors.tsv"
    if not path.exists():
        return None
    out = {}
    for line in path.read_text().splitlines():
        cols = line.split("\t")
        if len(cols) < 4 or not cols[0].startswith("0x"):
            continue
        kind = method(cols[3])
        if kind != "ambiguous":
            out[int(cols[0], 16)] = (int(cols[2], 16), kind)
    return out


def donor_markdown(addr: int) -> str:
    """Return the pc_jp_retail section for a task pack's donor_evidence.md.

    Generates build/pc_analysis/gc_pc_anchors.tsv on first use (about 20 s).
    """
    exe = find_exe()
    anchors = OUT_DIR / "gc_pc_anchors.tsv"
    lines = ["", "# Japanese PC 1.0 (x86) evidence", ""]
    if exe is None:
        lines.append("Unavailable: `orig/pc/jpn/retail` is not present here or in the primary"
                     " checkout (see docs/research.md#pc-builds). Record this limit.")
        return "\n".join(lines) + "\n"
    if not anchors.exists() or anchors.stat().st_mtime < CONFIRMED.stat().st_mtime:
        result = subprocess.run([sys.executable, str(Path(__file__).resolve()), "--exe", str(exe)],
                                cwd=ROOT, capture_output=True, text=True)
        if result.returncode != 0 or not anchors.exists():
            lines.append("Not generated: `python3 tools/pc_gc_anchors.py` failed: "
                         + (result.stderr.strip().splitlines() or ["no output"])[-1])
            return "\n".join(lines) + "\n"
    key = f"0x{addr:08X}"
    row = next((line.split("\t") for line in anchors.read_text().splitlines()
                if line.startswith(key + "\t")), None)
    if row is None:
        lines.append("No pair for this function (no shared string, aligned call, table slot, global"
                     " or layout gap; see `build/pc_analysis/gc_pc_anchors.tsv` for its callers).")
        return "\n".join(lines) + "\n"
    lines += [
        f"Paired with x86 `{row[2]}` ({row[3]}).",
        f"Read it with REA: `open_binary` on `{exe}`,",
        f"then `procedure_pseudo_code` at `{row[2]}`. MSVC output does not transfer; use it for call",
        "order, globals, constants, field offsets and branch structure, confirmed against GC assembly.",
        "Known global pairs: `docs/symbol_donors/pc_jp_globals.tsv`. After confirming the pair, add it to",
        "`docs/symbol_donors/pc_jp_pairs.tsv` so later runs propagate from it.",
    ]
    if method(row[3]) not in {"confirmed", "strings"}:
        lines.append("This pair is inferred, not confirmed: check it before relying on it.")
    return "\n".join(lines) + "\n"


# --- Pairing ------------------------------------------------------------------

def write_atomic(path: Path, text: str) -> None:
    """Write via rename so parallel task packs never read a partial file."""
    tmp = path.with_name(f".{path.name}.{os.getpid()}")
    tmp.write_text(text)
    tmp.replace(path)


def build_pairer(exe: Path, min_len: int = 6) -> Pairer:
    """Collect both sides' evidence for `exe` (one objdump run, about 20 s in all)."""
    gc_refs = gc_string_refs(min_len)
    text = x86_disassembly(exe)
    pc_refs, pc_calls = pc_string_refs(exe, min_len, x86_listing(text) if text else None)
    gc_calls, gc_addrs = gc_callees()
    gc_sizes = {s.name: s.size for s in parse_symbols(SYMBOLS) if "type:function" in s.meta}
    pc_feat = pc_features(exe, sorted(pc_calls), text)
    if not pc_feat.refs:
        print("warning: no x86 operands (objdump with pei-i386 missing?); global and shape steps"
              " are weakened and starts come from a byte scan", file=sys.stderr)
    pairer = Pairer(gc_refs, pc_refs, gc_calls, pc_calls, gc_addrs, gc_sizes, gc_features(), pc_feat)
    pairer.pc_refs = pc_refs
    return pairer


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--exe", type=Path, help="PC exe (default: pc_jp_retail)")
    parser.add_argument("--min-len", type=int, default=6, help="minimum string length (default 6)")
    parser.add_argument("--evaluate", type=int, nargs="?", const=20, metavar="SEEDS",
                        help="measure precision: hold out 20%% of string pairs per seed (default 20 seeds)"
                             " and report how each step re-derives them; writes nothing")
    args = parser.parse_args()

    exe = args.exe or find_exe()
    if exe is None or not exe.exists():
        raise SystemExit("No unwrapped PC exe found; see docs/research.md#pc-builds")

    pairer = build_pairer(exe, args.min_len)
    gc_addrs = pairer.gc_addrs

    # Confirmed pairs seed every run; they are only valid for this exact exe.
    confirmed: dict[str, tuple[int, str]] = {}
    by_addr = {addr: name for name, addr in gc_addrs.items()}
    if CONFIRMED.exists() and hashlib.sha256(exe.read_bytes()).hexdigest() == PC_JP_SHA256:
        for line in CONFIRMED.read_text().splitlines():
            if line.startswith(("#", "gc_addr")) or not line.strip():
                continue
            gc_addr, pc_addr, note = line.split("\t", 2)
            name = by_addr.get(int(gc_addr, 16))
            if name:
                confirmed[name] = (int(pc_addr, 16), f"confirmed: {note[:60]}")

    if args.evaluate:
        tally = pairer.evaluate(confirmed, seeds=args.evaluate)
        print(f"Held-out string pairs re-derived ({args.evaluate} seeds x 20%):")
        for (name, ok), count in sorted(tally.items(), key=lambda kv: (kv[0][0], str(kv[0][1]))):
            print(f"  {name:<14}{'' if ok is None else 'correct' if ok else 'WRONG':<9}{count}")
        made = sum(c for (n, ok), c in tally.items() if ok is not None)
        right = sum(c for (n, ok), c in tally.items() if ok)
        print(f"  precision {right}/{made}" + (f" = {right / made:.1%}" if made else ""))
        return

    pairs, split_rows = pairer.run(confirmed)
    by_method = Counter(method(e) for _, e in pairs.values())

    pc_to_gc: dict[int, list[str]] = defaultdict(list)
    rows = []
    for name, (pc_fn, evidence) in pairs.items():
        rows.append((gc_addrs.get(name, 0), name, pc_fn, evidence))
        pc_to_gc[pc_fn].append(name)
    for name, pc_fn, evidence in split_rows:
        rows.append((gc_addrs.get(name, 0), name, pc_fn, evidence))
    rows.sort()

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    lines = [
        f"# Generated by tools/pc_gc_anchors.py from {exe.relative_to(ROOT) if exe.is_relative_to(ROOT) else exe.name}",
        "# Evidence only: confirm against GC assembly before renaming. Renames are not matching progress.",
        "gc_addr\tgc_function\tpc_function\tevidence",
    ]
    for addr, name, pc_fn, evidence in rows:
        lines.append(f"0x{addr:08X}\t{name}\t0x{pc_fn:08X}\t{evidence}")
    write_atomic(OUT_DIR / "gc_pc_anchors.tsv", "\n".join(lines) + "\n")

    src_lines = [
        "# Generated by tools/pc_gc_anchors.py",
        "source_path\tpc_functions\tanchored_gc_functions",
    ]
    pc_refs = pairer.pc_refs
    for string in sorted(s for s in pc_refs if SOURCE_PATH_RE.match(s)):
        pc_fns = sorted(pc_refs[string])
        gc_fns = sorted({g for f in pc_fns for g in pc_to_gc.get(f, [])})
        src_lines.append(
            f"{string}\t{','.join(f'0x{f:08X}' for f in pc_fns)}\t{','.join(gc_fns)}"
        )
    write_atomic(OUT_DIR / "pc_source_files.tsv", "\n".join(src_lines) + "\n")

    global_lines = [
        "# Generated by tools/pc_gc_anchors.py: GC data symbol <-> x86 address used together by every",
        "# paired function that uses either (two or more). Confirm against GC accesses before use.",
        "gc_addr\tgc_label\tpc_addr\tpaired_users",
    ]
    labels = {s.address: s.name for s in parse_symbols(SYMBOLS)}
    users = Counter(x for g in pairs for x in set(pairer.gc_feat.refs.get(g, [])))
    for x, y in sorted(pairer.global_pairs(pairs).items()):
        global_lines.append(f"0x{x:08X}\t{labels.get(x, '')}\t0x{y:08X}\t{users[x]}")
    write_atomic(OUT_DIR / "gc_pc_globals.tsv", "\n".join(global_lines) + "\n")

    print(f"GC functions paired: {len(pairs)} ("
          + ", ".join(f"{n} {name}" for name, n in by_method.most_common())
          + f"); {len(split_rows)} ambiguous string votes listed")
    print(f"Global pairs: {len(global_lines) - 3}")
    print(f"PC source paths with code refs: {len(src_lines) - 2}")
    print(f"Wrote {(OUT_DIR / 'gc_pc_anchors.tsv').relative_to(ROOT)}")
    print(f"Wrote {(OUT_DIR / 'pc_source_files.tsv').relative_to(ROOT)}")
    print(f"Wrote {(OUT_DIR / 'gc_pc_globals.tsv').relative_to(ROOT)}")


if __name__ == "__main__":
    main()
