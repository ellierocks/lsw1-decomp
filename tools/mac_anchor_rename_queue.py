#!/usr/bin/env python3
"""Generate GC rename candidates from Mac debug symbol order.

The matcher uses already-named GC functions as anchors.  When two adjacent GC
anchors also appear in a Mac debug binary, unnamed GC functions between those
anchors are aligned against Mac functions in the same anchor gap.
"""

from __future__ import annotations

import argparse
import csv
import re
import subprocess
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
GC_SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
OUT_DIR = ROOT / "docs" / "symbol_donors"
OUT_TSV = OUT_DIR / "mac_anchor_rename_queue.tsv"
OUT_MD = OUT_DIR / "mac_anchor_rename_queue.md"

MAC_BINS = [
    ("mac_lsw1_demo", ROOT / "orig" / "mac" / "mac-debug-symbols" / "LEGO Star Wars Demo", "ppc", 5),
    ("mac_lsw2_ppc", ROOT / "orig" / "mac" / "mac-debug-symbols" / "LEGO Star Wars II", "ppc", 3),
    ("mac_batman", ROOT / "orig" / "mac" / "mac-debug-symbols" / "LEGO Batman", "i386", 1),
    ("mac_indy", ROOT / "orig" / "mac" / "mac-debug-symbols" / "LEGOIndianaJones", "i386", 1),
]

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s*//\s*(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")
MANGLED_RE = re.compile(r"^_?Z(\d+)([A-Za-z_]\w*)")
VALID_CANDIDATE_RE = re.compile(
    r"^(Nu[A-Z][A-Za-z0-9_]*|instNu[A-Z][A-Za-z0-9_]*|AI[A-Z][A-Za-z0-9_]*|"
    r"Action_[A-Za-z0-9_]+|Condition_[A-Za-z0-9_]+|Area_[A-Za-z0-9_]+|"
    r"Menu[A-Za-z0-9_]*|DebugMenu[A-Za-z0-9_]*|PauseMenu[A-Za-z0-9_]*|"
    r"Scene[A-Za-z0-9_]*|Camera[A-Za-z0-9_]*|Cam[A-Za-z0-9_]*|Pad[A-Za-z0-9_]*|"
    r"Player_[A-Za-z0-9_]+|Sound[A-Za-z0-9_]*|Credit[A-Za-z0-9_]*|"
    r"Podrace[A-Za-z0-9_]*|Draw_[A-Za-z0-9_]+|Init[A-Za-z0-9_]*|Load[A-Za-z0-9_]*)$"
)
BAD_NAMES = {
    "__start",
    "start",
    "main",
    "atexit",
    "malloc",
    "free",
}


@dataclass(frozen=True)
class GcFunc:
    name: str
    section: str
    address: int
    size: int


@dataclass(frozen=True)
class MacFunc:
    name: str
    raw_name: str
    address: int
    size: int
    source: str
    arch: str
    weight: int


def clean_name(raw: str) -> str:
    name = raw
    if name.startswith("_"):
        name = name[1:]
    m = MANGLED_RE.match(raw)
    if m:
        length = int(m.group(1))
        body = m.group(2)
        if len(body) >= length:
            name = body[:length]
    m = MANGLED_RE.match(name)
    if m:
        length = int(m.group(1))
        body = m.group(2)
        if len(body) >= length:
            name = body[:length]
    return name


def is_auto_gc(name: str) -> bool:
    return name.startswith(("fn_", "lbl_", "jumptable_"))


def is_candidate_name(name: str) -> bool:
    return (
        name
        and name not in BAD_NAMES
        and not name.startswith((".", "$", "__", "sub_", "loc_"))
        and not name.endswith(".eh")
        and VALID_CANDIDATE_RE.match(name) is not None
    )


def parse_gc_symbols() -> list[GcFunc]:
    funcs: list[GcFunc] = []
    for line in GC_SYMBOLS.read_text(errors="replace").splitlines():
        m = SYMBOL_RE.match(line)
        if not m:
            continue
        meta = m.group("meta")
        if "type:function" not in meta:
            continue
        section = m.group("section")
        if section not in (".init", ".text"):
            continue
        sm = SIZE_RE.search(meta)
        size = int(sm.group(1), 16) if sm else 0
        funcs.append(GcFunc(m.group("name"), section, int(m.group("addr"), 16), size))
    return sorted(funcs, key=lambda f: f.address)


def parse_mac_symbols(source: str, path: Path, arch: str, weight: int) -> list[MacFunc]:
    if not path.exists():
        return []
    cmd = ["llvm-nm", "-n", "-a"]
    if arch:
        cmd.extend(["-arch", arch])
    cmd.append(str(path))
    result = subprocess.run(cmd, check=False, capture_output=True, text=True, errors="replace")
    by_addr: dict[int, tuple[str, str]] = {}
    for line in result.stdout.splitlines():
        parts = line.split(None, 2)
        if len(parts) < 3:
            continue
        try:
            addr = int(parts[0], 16)
        except ValueError:
            continue
        sym_type = parts[1]
        if sym_type not in ("T", "t"):
            continue
        raw = parts[2].strip()
        name = clean_name(raw)
        if not is_candidate_name(name):
            continue
        by_addr.setdefault(addr, (name, raw))

    funcs: list[MacFunc] = []
    addrs = sorted(by_addr)
    for i, addr in enumerate(addrs):
        next_addr = addrs[i + 1] if i + 1 < len(addrs) else addr
        size = max(0, next_addr - addr)
        name, raw = by_addr[addr]
        funcs.append(MacFunc(name, raw, addr, size, source, arch, weight))
    return funcs


def module_for_addr(addr: int) -> str:
    modules = [
        ("nucore/file", 0x800034A0, 0x80006000),
        ("nucore/numem", 0x80006F74, 0x80007468),
        ("nucore/error", 0x80007468, 0x80008000),
        ("numath", 0x80008000, 0x80012000),
        ("nu3dx/anim", 0x80016A00, 0x80024000),
        ("nu3dx/render", 0x80024000, 0x8005C000),
        ("nu3dx/scene", 0x8005C000, 0x80090000),
        ("nusound", 0x80090000, 0x800B0000),
        ("gamelib", 0x800B0000, 0x80100000),
        ("gamecode", 0x80100000, 0x8018CB00),
    ]
    for name, start, end in modules:
        if start <= addr < end:
            return name
    return "unknown"


def size_score(gc_size: int, mac_size: int) -> int:
    if not gc_size or not mac_size:
        return 0
    delta = abs(gc_size - mac_size)
    if delta == 0:
        return 4
    if delta <= 8:
        return 3
    if delta <= 0x20:
        return 2
    if delta <= 0x80:
        return 1
    return 0


def confidence(score: int, gap_exact: bool, source: str) -> str:
    if gap_exact and source == "mac_lsw1_demo" and score >= 9:
        return "HIGH"
    if gap_exact and score >= 7:
        return "MEDIUM"
    if score >= 6:
        return "LOW"
    return "REVIEW"


def build_candidates(gc_funcs: list[GcFunc], mac_funcs: list[MacFunc]) -> list[dict[str, object]]:
    gc_named = {f.name for f in gc_funcs if not is_auto_gc(f.name)}
    mac_by_name: dict[str, list[int]] = {}
    for i, f in enumerate(mac_funcs):
        mac_by_name.setdefault(f.name, []).append(i)

    candidates: list[dict[str, object]] = []
    for left_i, left in enumerate(gc_funcs):
        if is_auto_gc(left.name) or left.name not in mac_by_name:
            continue
        right_i = left_i + 1
        while right_i < len(gc_funcs) and is_auto_gc(gc_funcs[right_i].name):
            right_i += 1
        if right_i >= len(gc_funcs):
            continue
        right = gc_funcs[right_i]
        if right.name not in mac_by_name:
            continue

        gc_gap = gc_funcs[left_i + 1:right_i]
        if not gc_gap:
            continue

        for mac_left_i in mac_by_name[left.name]:
            right_positions = [i for i in mac_by_name[right.name] if i > mac_left_i]
            if not right_positions:
                continue
            mac_right_i = right_positions[0]
            mac_gap = [
                f for f in mac_funcs[mac_left_i + 1:mac_right_i]
                if f.name not in gc_named and is_candidate_name(f.name)
            ]
            if not mac_gap:
                continue

            gap_exact = len(gc_gap) == len(mac_gap)
            pair_count = min(len(gc_gap), len(mac_gap))
            if pair_count == 0 or pair_count > 24:
                continue

            for offset in range(pair_count):
                gc = gc_gap[offset]
                mac = mac_gap[offset]
                if not is_auto_gc(gc.name) or mac.name in gc_named:
                    continue
                score = mac.weight
                score += 3 if gap_exact else 0
                score += size_score(gc.size, mac.size)
                if left.name.startswith("Nu") and mac.name.startswith("Nu"):
                    score += 1
                conf = confidence(score, gap_exact, mac.source)
                if conf == "REVIEW":
                    continue
                candidates.append(
                    {
                        "confidence": conf,
                        "score": score,
                        "gc_old": gc.name,
                        "gc_addr": f"0x{gc.address:08X}",
                        "gc_size": f"0x{gc.size:X}",
                        "new_name": mac.name,
                        "mac_source": mac.source,
                        "mac_arch": mac.arch,
                        "mac_addr": f"0x{mac.address:X}",
                        "mac_size": f"0x{mac.size:X}",
                        "left_anchor": left.name,
                        "right_anchor": right.name,
                        "gap": f"{len(gc_gap)}:{len(mac_gap)}",
                        "module": module_for_addr(gc.address),
                    }
                )
            break
    return candidates


def dedupe(candidates: list[dict[str, object]]) -> list[dict[str, object]]:
    best: dict[str, dict[str, object]] = {}
    rank = {"HIGH": 3, "MEDIUM": 2, "LOW": 1}
    for cand in candidates:
        key = str(cand["gc_old"])
        prev = best.get(key)
        if prev is None:
            best[key] = cand
            continue
        prev_key = (rank[str(prev["confidence"])], int(prev["score"]))
        cand_key = (rank[str(cand["confidence"])], int(cand["score"]))
        if cand_key > prev_key:
            best[key] = cand
    return sorted(
        best.values(),
        key=lambda c: (
            {"HIGH": 0, "MEDIUM": 1, "LOW": 2}[str(c["confidence"])],
            str(c["module"]),
            int(str(c["gc_addr"]), 16),
        ),
    )


def write_outputs(candidates: list[dict[str, object]]) -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    fields = [
        "confidence",
        "score",
        "module",
        "gc_old",
        "gc_addr",
        "gc_size",
        "new_name",
        "mac_source",
        "mac_arch",
        "mac_addr",
        "mac_size",
        "left_anchor",
        "right_anchor",
        "gap",
    ]
    with OUT_TSV.open("w", newline="") as f:
        writer = csv.DictWriter(f, delimiter="\t", fieldnames=fields)
        writer.writeheader()
        writer.writerows(candidates)

    lines = [
        "# Mac Anchor Rename Queue",
        "",
        "Generated by `python tools/mac_anchor_rename_queue.py`.",
        "",
        "Confidence uses named GC functions as left/right anchors, aligns unnamed functions in the anchor gap against Mac debug symbols, and scores source priority plus size similarity.",
        "",
        "## Summary",
    ]
    for conf in ("HIGH", "MEDIUM", "LOW"):
        lines.append(f"- {conf}: {sum(1 for c in candidates if c['confidence'] == conf)}")
    lines.extend(["", "## Candidates", ""])
    lines.append("| Conf | GC | New name | Module | Source | Anchors | Size |")
    lines.append("|---|---:|---|---|---|---|---:|")
    for cand in candidates[:300]:
        lines.append(
            f"| {cand['confidence']} | `{cand['gc_old']}` @ `{cand['gc_addr']}` | "
            f"`{cand['new_name']}` | {cand['module']} | {cand['mac_source']} | "
            f"`{cand['left_anchor']}` .. `{cand['right_anchor']}` ({cand['gap']}) | "
            f"{cand['gc_size']}/{cand['mac_size']} |"
        )
    lines.extend(["", "## Apply Preview", ""])
    for cand in candidates:
        if cand["confidence"] != "HIGH":
            continue
        lines.append(f"python tools/ls1_rename.py {cand['gc_old']} {cand['new_name']}")
    OUT_MD.write_text("\n".join(lines) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--min-confidence", choices=["HIGH", "MEDIUM", "LOW"], default="LOW")
    args = parser.parse_args()

    min_rank = {"HIGH": 3, "MEDIUM": 2, "LOW": 1}[args.min_confidence]
    gc_funcs = parse_gc_symbols()
    all_candidates: list[dict[str, object]] = []
    for source, path, arch, weight in MAC_BINS:
        mac_funcs = parse_mac_symbols(source, path, arch, weight)
        all_candidates.extend(build_candidates(gc_funcs, mac_funcs))

    candidates = dedupe(all_candidates)
    candidates = [c for c in candidates if {"HIGH": 3, "MEDIUM": 2, "LOW": 1}[str(c["confidence"])] >= min_rank]
    write_outputs(candidates)

    print(f"Wrote {OUT_TSV.relative_to(ROOT)}")
    print(f"Wrote {OUT_MD.relative_to(ROOT)}")
    print(
        "Candidates: "
        + ", ".join(f"{conf}={sum(1 for c in candidates if c['confidence'] == conf)}" for conf in ("HIGH", "MEDIUM", "LOW"))
    )


if __name__ == "__main__":
    main()
