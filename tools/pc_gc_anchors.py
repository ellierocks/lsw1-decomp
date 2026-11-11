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
import re
import struct
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

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


def pc_string_refs(exe: Path, min_len: int) -> tuple[dict[str, set[int]], list[int]]:
    """Return ({string text: {x86 function VA}}, sorted function starts)."""
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
    starts = {text_va}
    for i in range(len(code) - 5):
        if code[i] == 0xE8:
            target = text_va + i + 5 + struct.unpack_from("<i", code, i + 1)[0]
            if text_va <= target < text_va + text_size:
                starts.add(target)
    for match in re.finditer(rb"(?:\xcc|\x90){2,}", code):
        end = match.end()
        if end < len(code) and code[end] not in (0xCC, 0x90):
            if code[match.start() - 1] in (0xC3, 0xC2) or code[match.start()] == 0xCC:
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
    calls: dict[int, list[int]] = {}
    for n, start in enumerate(ordered):
        end = ordered[n + 1] if n + 1 < len(ordered) else text_va + text_size
        seq: list[int] = []
        body = code[start - text_va : end - text_va]
        for i in range(len(body) - 4):
            if body[i] == 0xE8:
                target = start + i + 5 + struct.unpack_from("<i", body, i + 1)[0]
                if target in start_set and target not in seq:
                    seq.append(target)
        calls[start] = seq
    return refs, calls


def propagate(pairs: dict[str, tuple[int, str]], gc_calls: dict[str, list[str]],
              pc_calls: dict[int, list[int]]) -> int:
    """Pair callees of paired functions where both call lists align.

    Already-paired callees act as fixed points; a gap between them is paired
    positionally only when both sides have the same number of callees in it
    and it holds one callee (up to three when the whole call lists have equal
    length). Looser rules dropped below 95% on held-out string pairs.
    Conflicting proposals are dropped. Returns the number of new pairs.
    """
    added = 0
    while True:
        proposals: dict[str, set[tuple[int, str]]] = defaultdict(set)
        taken = {pc for pc, _ in pairs.values()}
        for gc_fn, (pc_fn, _) in list(pairs.items()):
            a, b = gc_calls.get(gc_fn, []), pc_calls.get(pc_fn, [])
            fixed = [(-1, -1)]
            for i, g in enumerate(a):
                if g in pairs and pairs[g][0] in b:
                    j = b.index(pairs[g][0])
                    if j > fixed[-1][1]:
                        fixed.append((i, j))
            fixed.append((len(a), len(b)))
            for (i0, j0), (i1, j1) in zip(fixed, fixed[1:]):
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
                if len(by_pc[p]) == 1:
                    parent = sorted(src for _, src in props)[0]
                    pairs[g] = (p, f"callee of {parent}")
                    new += 1
        if not new:
            return added
        added += new


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
        lines.append("No pair for this function (no shared string or aligned call; see"
                     " `build/pc_analysis/gc_pc_anchors.tsv` for its callers).")
        return "\n".join(lines) + "\n"
    lines += [
        f"Paired with x86 `{row[2]}` ({row[3]}).",
        f"Read it with REA: `open_binary` on `{exe}`,",
        f"then `procedure_pseudo_code` at `{row[2]}`. MSVC output does not transfer; use it for call",
        "order, globals, constants, field offsets and branch structure, confirmed against GC assembly.",
        "Known global pairs: `docs/symbol_donors/pc_jp_globals.tsv`. After confirming the pair, add it to",
        "`docs/symbol_donors/pc_jp_pairs.tsv` so later runs propagate from it.",
    ]
    if row[3].startswith(("callee", "strings split")):
        lines.append("This pair is inferred, not confirmed: check it before relying on it.")
    return "\n".join(lines) + "\n"


# --- Pairing ------------------------------------------------------------------

def write_atomic(path: Path, text: str) -> None:
    """Write via rename so parallel task packs never read a partial file."""
    tmp = path.with_name(f".{path.name}.{os.getpid()}")
    tmp.write_text(text)
    tmp.replace(path)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--exe", type=Path, help="PC exe (default: pc_jp_retail)")
    parser.add_argument("--min-len", type=int, default=6, help="minimum string length (default 6)")
    args = parser.parse_args()

    exe = args.exe or find_exe()
    if exe is None or not exe.exists():
        raise SystemExit("No unwrapped PC exe found; see docs/research.md#pc-builds")

    gc_refs = gc_string_refs(args.min_len)
    pc_refs, pc_calls = pc_string_refs(exe, args.min_len)
    gc_calls, gc_addrs = gc_callees()

    # Vote: each shared string referenced by exactly one function on each side.
    votes: dict[str, Counter] = defaultdict(Counter)
    samples: dict[tuple[str, int], list[str]] = defaultdict(list)
    for string, gc_fns in gc_refs.items():
        pc_fns = pc_refs.get(string)
        if not pc_fns or len(gc_fns) != 1 or len(pc_fns) != 1:
            continue
        gc_fn = next(iter(gc_fns))
        pc_fn = next(iter(pc_fns))
        votes[gc_fn][pc_fn] += 1
        samples[(gc_fn, pc_fn)].append(string)

    # pairs: GC function name -> (x86 function, evidence); confirmed pairs first.
    pairs: dict[str, tuple[int, str]] = {}
    pc_owner: dict[int, str] = {}
    by_addr = {addr: name for name, addr in gc_addrs.items()}
    if CONFIRMED.exists() and hashlib.sha256(exe.read_bytes()).hexdigest() == PC_JP_SHA256:
        for line in CONFIRMED.read_text().splitlines():
            if line.startswith(("#", "gc_addr")) or not line.strip():
                continue
            gc_addr, pc_addr, note = line.split("\t", 2)
            name = by_addr.get(int(gc_addr, 16))
            if name:
                pairs[name] = (int(pc_addr, 16), f"confirmed: {note[:60]}")
                pc_owner[int(pc_addr, 16)] = name
    confirmed = len(pairs)
    split_rows = []
    for gc_fn, counter in sorted(votes.items(), key=lambda kv: -sum(kv[1].values())):
        (pc_fn, count), *rest = counter.most_common()
        name = gc_fn.split("@")[0]
        sample = samples[(gc_fn, pc_fn)][0][:60]
        if name in pairs:
            continue
        if rest or pc_fn in pc_owner:
            split_rows.append((name, pc_fn, f"strings split {count}/{sum(counter.values())}: {sample}"))
            continue
        pairs[name] = (pc_fn, f"strings {count}: {sample}")
        pc_owner[pc_fn] = name
    seeded = len(pairs) - confirmed
    propagated = propagate(pairs, gc_calls, pc_calls)

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
    for string in sorted(s for s in pc_refs if SOURCE_PATH_RE.match(s)):
        pc_fns = sorted(pc_refs[string])
        gc_fns = sorted({g for f in pc_fns for g in pc_to_gc.get(f, [])})
        src_lines.append(
            f"{string}\t{','.join(f'0x{f:08X}' for f in pc_fns)}\t{','.join(gc_fns)}"
        )
    write_atomic(OUT_DIR / "pc_source_files.tsv", "\n".join(src_lines) + "\n")

    print(f"GC functions paired: {len(pairs)} ({confirmed} confirmed, {seeded} by strings, "
          f"{propagated} by call order); "
          f"{len(split_rows)} ambiguous string votes listed")
    print(f"PC source paths with code refs: {len(src_lines) - 2}")
    print(f"Wrote {(OUT_DIR / 'gc_pc_anchors.tsv').relative_to(ROOT)}")
    print(f"Wrote {(OUT_DIR / 'pc_source_files.tsv').relative_to(ROOT)}")


if __name__ == "__main__":
    main()
