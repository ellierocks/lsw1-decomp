#!/usr/bin/env python3
"""Identify likely Dolphin SDK / runtime islands in the GC executable.

This is deliberately conservative: it maps island boundaries and evidence so a
known SDK source/symbol-order pass can be layered on top later.
"""

from __future__ import annotations

import csv
import re
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
CALLGRAPH = ROOT / "docs" / "symbol_donors" / "call_graph.tsv"
OUT_DIR = ROOT / "docs" / "symbol_donors"
OUT_TSV = OUT_DIR / "sdk_islands.tsv"
OUT_MD = OUT_DIR / "sdk_islands.md"

SDK_START = 0x8015BFC0
SDK_END = 0x8018CB00

SDK_PREFIXES = {
    "GX": ("GX", "__GX"),
    "OS": ("OS", "__OS"),
    "DVD": ("DVD", "__DVD"),
    "VI": ("VI", "__VI"),
    "PAD": ("PAD", "__PAD"),
    "CARD": ("CARD", "__CARD"),
    "AI": ("AI", "__AI"),
    "AR": ("AR", "ARQ", "__AR"),
    "AX": ("AX", "__AX"),
    "DSP": ("DSP", "__DSP"),
    "EXI": ("EXI", "__EXI"),
    "SI": ("SI", "__SI"),
    "MTX": ("MTX", "PSMTX", "C_MTX"),
    "PPC": ("PPC",),
    "runtime": ("__", "mem", "str", "sprintf", "printf", "malloc", "free"),
}

MANUAL_RANGES = [
    ("runtime/PPC", 0x8015BFC0, 0x8015BFD8, "confirmed PPC helpers"),
    ("sdk/OS", 0x8015CAA8, 0x80161040, "OS anchors and reset registration"),
    ("sdk/GX", 0x8017CA2C, 0x80182708, "dense GX anchor island"),
    ("sdk/GX-or-video-tail", 0x80182708, 0x8018CB00, "post-GX tail, needs symbol order source"),
]

SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s*//\s*(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")


@dataclass(frozen=True)
class Func:
    name: str
    address: int
    size: int

    @property
    def end(self) -> int:
        return self.address + self.size

    @property
    def is_named(self) -> bool:
        return not self.name.startswith("fn_")


def parse_symbols() -> list[Func]:
    funcs: list[Func] = []
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        m = SYMBOL_RE.match(line)
        if not m or "type:function" not in m.group("meta") or m.group("section") != ".text":
            continue
        sm = SIZE_RE.search(m.group("meta"))
        funcs.append(
            Func(
                name=m.group("name"),
                address=int(m.group("addr"), 16),
                size=int(sm.group(1), 16) if sm else 0,
            )
        )
    return sorted(funcs, key=lambda f: f.address)


def family_for_name(name: str) -> str | None:
    for family, prefixes in SDK_PREFIXES.items():
        if any(name.startswith(prefix) for prefix in prefixes):
            if family == "runtime" and name.startswith("__GX"):
                continue
            return family
    return None


def parse_callgraph() -> tuple[dict[str, set[str]], dict[str, set[str]]]:
    callees: dict[str, set[str]] = {}
    callers: dict[str, set[str]] = {}
    if not CALLGRAPH.exists():
        return callees, callers
    for line in CALLGRAPH.read_text(errors="replace").splitlines():
        if not line or line.startswith("#"):
            continue
        parts = line.split("\t")
        if len(parts) < 4:
            continue
        caller = parts[0].strip()
        callee = parts[3].strip()
        callees.setdefault(caller, set()).add(callee)
        callers.setdefault(callee, set()).add(caller)
    return callees, callers


def summarize_range(
    funcs: list[Func],
    start: int,
    end: int,
    family: str,
    note: str,
    callees: dict[str, set[str]],
    callers: dict[str, set[str]],
) -> dict[str, object]:
    in_range = [f for f in funcs if start <= f.address < end]
    named = [f for f in in_range if f.is_named]
    unnamed = [f for f in in_range if not f.is_named]
    anchors_by_family: dict[str, int] = {}
    for f in named:
        fam = family_for_name(f.name)
        if fam:
            anchors_by_family[fam] = anchors_by_family.get(fam, 0) + 1

    incoming_named: set[str] = set()
    outgoing_named: set[str] = set()
    for f in in_range:
        for c in callers.get(f.name, set()):
            if not c.startswith("fn_"):
                incoming_named.add(c)
        for c in callees.get(f.name, set()):
            if not c.startswith("fn_"):
                outgoing_named.add(c)

    bytes_total = max(0, min(end, in_range[-1].end if in_range else end) - start)
    bytes_unnamed = sum(f.size for f in unnamed)
    priority = "HIGH" if family == "sdk/GX" else "MEDIUM" if named else "REVIEW"
    if family.startswith("sdk/GX") and len(unnamed) > 0:
        priority = "HIGH"

    return {
        "family": family,
        "start": f"0x{start:08X}",
        "end": f"0x{end:08X}",
        "bytes": f"0x{bytes_total:X}",
        "functions": len(in_range),
        "named": len(named),
        "unnamed": len(unnamed),
        "unnamed_bytes": f"0x{bytes_unnamed:X}",
        "anchors": ", ".join(f.name for f in named[:16]),
        "anchor_families": ", ".join(f"{k}:{v}" for k, v in sorted(anchors_by_family.items())),
        "incoming_named": ", ".join(sorted(incoming_named)[:12]),
        "outgoing_named": ", ".join(sorted(outgoing_named)[:12]),
        "priority": priority,
        "note": note,
    }


def auto_anchor_gaps(funcs: list[Func]) -> list[tuple[str, int, int, str]]:
    anchors = [f for f in funcs if SDK_START <= f.address < SDK_END and f.is_named and family_for_name(f.name)]
    ranges: list[tuple[str, int, int, str]] = []
    for i, anchor in enumerate(anchors):
        fam = family_for_name(anchor.name) or "sdk"
        next_anchor = anchors[i + 1] if i + 1 < len(anchors) else None
        if next_anchor is None:
            continue
        next_fam = family_for_name(next_anchor.name)
        if next_fam != fam:
            continue
        end = next_anchor.address
        if end <= anchor.address:
            continue
        if end - anchor.address > 0x3000:
            continue
        note = f"anchor gap from {anchor.name}" + (f" to {next_anchor.name}" if next_anchor else "")
        ranges.append((f"sdk/{fam}", anchor.address, end, note))
    return ranges


def write_outputs(rows: list[dict[str, object]]) -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    fields = [
        "priority",
        "family",
        "start",
        "end",
        "bytes",
        "functions",
        "named",
        "unnamed",
        "unnamed_bytes",
        "anchors",
        "anchor_families",
        "incoming_named",
        "outgoing_named",
        "note",
    ]
    with OUT_TSV.open("w", newline="") as f:
        writer = csv.DictWriter(f, delimiter="\t", fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)

    lines = [
        "# SDK Island Analysis",
        "",
        "Generated by `python tools/sdk_island_analysis.py`.",
        "",
        "This is a conservative map for carving out Dolphin SDK, MSL, and Metrowerks runtime code. It does not apply renames.",
        "",
        "## Summary",
    ]
    for priority in ("HIGH", "MEDIUM", "REVIEW"):
        lines.append(f"- {priority}: {sum(1 for r in rows if r['priority'] == priority)} islands")
    lines.extend(["", "## Islands", ""])
    lines.append("| Priority | Family | Range | Funcs | Unnamed | Anchors | Note |")
    lines.append("|---|---|---:|---:|---:|---|---|")
    for row in rows:
        lines.append(
            f"| {row['priority']} | {row['family']} | `{row['start']}-{row['end']}` "
            f"({row['bytes']}) | {row['functions']} | {row['unnamed']} "
            f"({row['unnamed_bytes']}) | {row['anchors']} | {row['note']} |"
        )
    lines.extend(
        [
            "",
            "## Next Steps",
            "",
            "1. Add a local Dolphin SDK / Crash SDK symbol-order source and compare against the HIGH GX ranges.",
            "2. Promote exact anchor-gap matches into `sdk_anchor_rename_queue.tsv`.",
            "3. Split confirmed SDK objects before attempting game/Nu2 C matching in the same tail region.",
        ]
    )
    OUT_MD.write_text("\n".join(lines) + "\n")


def main() -> None:
    funcs = parse_symbols()
    callees, callers = parse_callgraph()

    ranges = list(MANUAL_RANGES)
    ranges.extend(auto_anchor_gaps(funcs))

    seen: set[tuple[int, int, str]] = set()
    rows: list[dict[str, object]] = []
    for family, start, end, note in ranges:
        key = (start, end, family)
        if key in seen:
            continue
        seen.add(key)
        if end <= start:
            continue
        rows.append(summarize_range(funcs, start, end, family, note, callees, callers))

    rows.sort(key=lambda r: ({"HIGH": 0, "MEDIUM": 1, "REVIEW": 2}[str(r["priority"])], int(str(r["start"]), 16)))
    write_outputs(rows)
    print(f"Wrote {OUT_TSV.relative_to(ROOT)}")
    print(f"Wrote {OUT_MD.relative_to(ROOT)}")
    print(
        "Islands: "
        + ", ".join(f"{p}={sum(1 for r in rows if r['priority'] == p)}" for p in ("HIGH", "MEDIUM", "REVIEW"))
    )


if __name__ == "__main__":
    main()
