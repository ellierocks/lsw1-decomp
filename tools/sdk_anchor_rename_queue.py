#!/usr/bin/env python3
"""Generate SDK/runtime rename candidates from local symbol-order sources."""

from __future__ import annotations

import argparse
import csv
import re
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = ROOT / "config" / "GL5E4F" / "symbols.txt"
SOURCE_DIR = ROOT / "docs" / "symbol_donors" / "sdk_symbol_sources"
OUT_DIR = ROOT / "docs" / "symbol_donors"
OUT_TSV = OUT_DIR / "sdk_anchor_rename_queue.tsv"
OUT_MD = OUT_DIR / "sdk_anchor_rename_queue.md"

SDK_START = 0x8015BFC0
SDK_END = 0x8018CB00
SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+=\s+(?P<section>\.\w+):0x(?P<addr>[0-9A-Fa-f]+);\s*//\s*(?P<meta>.*)$"
)
SIZE_RE = re.compile(r"\bsize:0x([0-9A-Fa-f]+)")
TOKEN_RE = re.compile(r"[_A-Za-z][_A-Za-z0-9@$.:]*")
VALID_SDK_RE = re.compile(
    r"^(__GX|GX|__OS|OS|DVD|__DVD|VI|__VI|PAD|__PAD|CARD|__CARD|AI|__AI|"
    r"AR|ARQ|__AR|AX|__AX|DSP|__DSP|EXI|__EXI|SI|__SI|MTX|PSMTX|C_MTX|"
    r"PPC|__|mem|str|sprintf|printf|malloc|free)[_A-Za-z0-9@$.:]*$"
)
SOURCE_GLOBS = ("*.txt", "*.lst", "*.map", "*.sym", "*.tsv")


@dataclass(frozen=True)
class GcFunc:
    name: str
    address: int
    size: int

    @property
    def is_auto(self) -> bool:
        return self.name.startswith("fn_")


def parse_gc_funcs() -> list[GcFunc]:
    funcs: list[GcFunc] = []
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        m = SYMBOL_RE.match(line)
        if not m or m.group("section") != ".text" or "type:function" not in m.group("meta"):
            continue
        addr = int(m.group("addr"), 16)
        if not (SDK_START <= addr < SDK_END):
            continue
        sm = SIZE_RE.search(m.group("meta"))
        funcs.append(GcFunc(m.group("name"), addr, int(sm.group(1), 16) if sm else 0))
    return sorted(funcs, key=lambda f: f.address)


def clean_source_symbol(token: str) -> str | None:
    token = token.strip()
    if not token or token.startswith((".", "#", "//")):
        return None
    token = token.split(";", 1)[0].strip()
    token = token.strip("`'\",()[]{}")
    if token.startswith("_") and token.startswith("__Z"):
        return None
    if token.endswith(".o") or token.endswith(".c") or token.endswith(".h"):
        return None
    if VALID_SDK_RE.match(token):
        return token
    return None


def load_source_symbols(path: Path) -> list[str]:
    symbols: list[str] = []
    seen: set[str] = set()
    for line in path.read_text(errors="replace").splitlines():
        for token in TOKEN_RE.findall(line):
            sym = clean_source_symbol(token)
            if sym and sym not in seen:
                seen.add(sym)
                symbols.append(sym)
    return symbols


def source_files(source_dir: Path) -> list[Path]:
    files: list[Path] = []
    for glob in SOURCE_GLOBS:
        files.extend(source_dir.rglob(glob))
    return sorted(path for path in files if path.name != "README.md")


def size_score(gc_size: int, left_gap_exact: bool) -> int:
    score = 0
    if left_gap_exact:
        score += 4
    if gc_size <= 0x10:
        score += 1
    return score


def confidence(exact_gap: bool, score: int) -> str:
    if exact_gap and score >= 5:
        return "HIGH"
    if exact_gap:
        return "MEDIUM"
    return "LOW"


def build_candidates(gc_funcs: list[GcFunc], src_name: str, src_symbols: list[str]) -> list[dict[str, object]]:
    src_pos: dict[str, int] = {}
    for i, sym in enumerate(src_symbols):
        src_pos.setdefault(sym, i)
    named_gc = {f.name for f in gc_funcs if not f.is_auto}
    candidates: list[dict[str, object]] = []

    for left_i, left in enumerate(gc_funcs):
        if left.is_auto or left.name not in src_pos:
            continue
        right_i = left_i + 1
        while right_i < len(gc_funcs) and gc_funcs[right_i].is_auto:
            right_i += 1
        if right_i >= len(gc_funcs):
            continue
        right = gc_funcs[right_i]
        if right.name not in src_pos:
            continue
        src_left = src_pos[left.name]
        src_right = src_pos[right.name]
        if src_right <= src_left:
            continue

        gc_gap = gc_funcs[left_i + 1:right_i]
        src_gap = [s for s in src_symbols[src_left + 1:src_right] if s not in named_gc and VALID_SDK_RE.match(s)]
        if not gc_gap or not src_gap:
            continue
        if len(gc_gap) > 64 or len(src_gap) > 64:
            continue

        exact = len(gc_gap) == len(src_gap)
        pair_count = min(len(gc_gap), len(src_gap))
        for off in range(pair_count):
            gc = gc_gap[off]
            new_name = src_gap[off]
            if not gc.is_auto or new_name in named_gc:
                continue
            score = size_score(gc.size, exact)
            candidates.append(
                {
                    "confidence": confidence(exact, score),
                    "score": score,
                    "gc_old": gc.name,
                    "gc_addr": f"0x{gc.address:08X}",
                    "gc_size": f"0x{gc.size:X}",
                    "new_name": new_name,
                    "source": src_name,
                    "left_anchor": left.name,
                    "right_anchor": right.name,
                    "gap": f"{len(gc_gap)}:{len(src_gap)}",
                }
            )
    return candidates


def dedupe(candidates: list[dict[str, object]]) -> list[dict[str, object]]:
    rank = {"HIGH": 3, "MEDIUM": 2, "LOW": 1}
    best: dict[str, dict[str, object]] = {}
    for cand in candidates:
        key = str(cand["gc_old"])
        prev = best.get(key)
        if prev is None or (rank[str(cand["confidence"])], int(cand["score"])) > (
            rank[str(prev["confidence"])],
            int(prev["score"]),
        ):
            best[key] = cand
    return sorted(
        best.values(),
        key=lambda c: ({"HIGH": 0, "MEDIUM": 1, "LOW": 2}[str(c["confidence"])], int(str(c["gc_addr"]), 16)),
    )


def write_outputs(candidates: list[dict[str, object]], sources: list[tuple[Path, int]]) -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    fields = [
        "confidence",
        "score",
        "gc_old",
        "gc_addr",
        "gc_size",
        "new_name",
        "source",
        "left_anchor",
        "right_anchor",
        "gap",
    ]
    with OUT_TSV.open("w", newline="") as f:
        writer = csv.DictWriter(f, delimiter="\t", fieldnames=fields)
        writer.writeheader()
        writer.writerows(candidates)

    lines = [
        "# SDK Anchor Rename Queue",
        "",
        "Generated by `python tools/sdk_anchor_rename_queue.py`.",
        "",
        "This queue is source-driven. Add local symbol-order files under `docs/symbol_donors/sdk_symbol_sources/`.",
        "",
        "## Sources",
    ]
    if sources:
        for path, count in sources:
            lines.append(f"- `{path.relative_to(ROOT)}`: {count} symbols")
    else:
        lines.append("- No SDK symbol-order sources found.")
    lines.extend(["", "## Summary"])
    for conf in ("HIGH", "MEDIUM", "LOW"):
        lines.append(f"- {conf}: {sum(1 for c in candidates if c['confidence'] == conf)}")

    if not sources:
        lines.extend(
            [
                "",
                "## Blocked",
                "",
                "Drop a CrashWOC/Dolphin SDK map or symbol dump into `docs/symbol_donors/sdk_symbol_sources/`, or normalize it with `tools/sdk_symbol_source_import.py`, then rerun:",
                "",
                "```sh",
                "python tools/binary_mining_pipeline.py sdk-anchor-queue",
                "```",
            ]
        )
    else:
        lines.extend(["", "## Candidates", ""])
        lines.append("| Conf | GC | New name | Source | Anchors | Gap |")
        lines.append("|---|---:|---|---|---|---:|")
        for cand in candidates[:300]:
            lines.append(
                f"| {cand['confidence']} | `{cand['gc_old']}` @ `{cand['gc_addr']}` | "
                f"`{cand['new_name']}` | `{cand['source']}` | "
                f"`{cand['left_anchor']}` .. `{cand['right_anchor']}` | {cand['gap']} |"
            )
    OUT_MD.write_text("\n".join(lines) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-dir", type=Path, default=SOURCE_DIR)
    args = parser.parse_args()

    source_dir = args.source_dir
    source_dir.mkdir(parents=True, exist_ok=True)

    gc_funcs = parse_gc_funcs()
    sources: list[tuple[Path, int]] = []
    all_candidates: list[dict[str, object]] = []
    for path in source_files(source_dir):
        symbols = load_source_symbols(path)
        sources.append((path, len(symbols)))
        all_candidates.extend(build_candidates(gc_funcs, path.name, symbols))

    candidates = dedupe(all_candidates)
    write_outputs(candidates, sources)
    print(f"Wrote {OUT_TSV.relative_to(ROOT)}")
    print(f"Wrote {OUT_MD.relative_to(ROOT)}")
    print(
        "Candidates: "
        + ", ".join(f"{c}={sum(1 for row in candidates if row['confidence'] == c)}" for c in ("HIGH", "MEDIUM", "LOW"))
    )


if __name__ == "__main__":
    main()
