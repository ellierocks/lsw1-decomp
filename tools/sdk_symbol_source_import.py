#!/usr/bin/env python3
"""Normalize SDK symbol-order sources into ordered TSV files.

Use this when you have a linker map or symbol dump from a CrashWOC, Dolphin,
or other GC/Wii SDK donor and want a clean one-symbol-per-line source for the
SDK anchor queue.
"""

from __future__ import annotations

import argparse
import csv
import re
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUT_DIR = ROOT / "docs" / "symbol_donors" / "sdk_symbol_sources" / "imported"
SOURCE_GLOBS = ("*.txt", "*.lst", "*.map", "*.sym", "*.tsv")
TOKEN_RE = re.compile(r"[_A-Za-z][_A-Za-z0-9@$.:]*")
VALID_SDK_RE = re.compile(
    r"^(__GX|GX|__OS|OS|DVD|__DVD|VI|__VI|PAD|__PAD|CARD|__CARD|AI|__AI|"
    r"AR|ARQ|__AR|AX|__AX|DSP|__DSP|EXI|__EXI|SI|__SI|MTX|PSMTX|C_MTX|"
    r"PPC|__|mem|str|sprintf|printf|malloc|free)[_A-Za-z0-9@$.:]*$"
)
HEADER_FIELDS = {"index", "order", "symbol", "name", "source_file", "line_no", "raw"}


@dataclass(frozen=True)
class SymbolHit:
    symbol: str
    line_no: int
    raw: str


def clean_symbol(token: str) -> str | None:
    token = token.strip()
    if not token or token.startswith((".", "#", "//", ";")):
        return None
    token = token.strip("`'\",()[]{}")
    token = token.split(";", 1)[0].strip()
    if token.endswith((".o", ".c", ".cpp", ".h")):
        return None
    if VALID_SDK_RE.match(token):
        return token
    return None


def line_symbols(text: str) -> list[str]:
    seen: set[str] = set()
    symbols: list[str] = []
    for token in TOKEN_RE.findall(text):
        sym = clean_symbol(token)
        if sym and sym not in seen:
            seen.add(sym)
            symbols.append(sym)
    return symbols


def is_header_row(row: list[str]) -> bool:
    lowered = [cell.strip().lower() for cell in row if cell.strip()]
    if not lowered:
        return False
    if not any(cell in HEADER_FIELDS for cell in lowered):
        return False
    return not any(clean_symbol(cell) for cell in row)


def parse_text_source(path: Path) -> list[SymbolHit]:
    hits: list[SymbolHit] = []
    seen: set[str] = set()
    for line_no, raw in enumerate(path.read_text(errors="replace").splitlines(), start=1):
        stripped = raw.strip()
        if not stripped or stripped.startswith(("#", "//", ";", "/*", "*")):
            continue
        for sym in line_symbols(stripped):
            if sym in seen:
                continue
            seen.add(sym)
            hits.append(SymbolHit(sym, line_no, raw.rstrip("\n")))
    return hits


def parse_tsv_source(path: Path) -> list[SymbolHit]:
    hits: list[SymbolHit] = []
    seen: set[str] = set()
    with path.open(newline="", errors="replace") as f:
        reader = csv.reader(f, delimiter="\t")
        for line_no, row in enumerate(reader, start=1):
            if not row:
                continue
            if line_no == 1 and is_header_row(row):
                continue
            raw = "\t".join(row)
            for sym in line_symbols(raw):
                if sym in seen:
                    continue
                seen.add(sym)
                hits.append(SymbolHit(sym, line_no, raw))
    return hits


def parse_source(path: Path) -> list[SymbolHit]:
    if path.suffix.lower() == ".tsv":
        return parse_tsv_source(path)
    return parse_text_source(path)


def source_files(root: Path, recursive: bool) -> list[Path]:
    if root.is_file():
        return [root]
    files: list[Path] = []
    iterator = root.rglob("*") if recursive else root.iterdir()
    for path in iterator:
        if path.is_file() and path.suffix.lower() in {".txt", ".lst", ".map", ".sym", ".tsv"}:
            files.append(path)
    return sorted(files)


def safe_output_stem(path: Path) -> Path:
    try:
        rel = path.resolve().relative_to(ROOT)
    except ValueError:
        parts = [
            re.sub(r"[^A-Za-z0-9._-]+", "_", part)
            for part in path.resolve().parts
            if part not in ("", path.anchor)
        ]
        rel = Path(*parts)
    return rel


def output_path(source_root: Path, file_path: Path, out_dir: Path, source_is_dir: bool) -> Path:
    if source_is_dir:
        rel = file_path.relative_to(source_root)
        return (out_dir / source_root.name / rel).with_suffix(".order.tsv")
    return (out_dir / file_path.stem).with_suffix(".order.tsv")


def write_tsv(path: Path, hits: list[SymbolHit], source_path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as f:
        writer = csv.writer(f, delimiter="\t")
        writer.writerow(["index", "symbol", "source_file", "line_no", "raw"])
        for index, hit in enumerate(hits):
            writer.writerow([index, hit.symbol, source_path.as_posix(), hit.line_no, hit.raw])


def display_path(path: Path) -> str:
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return path.as_posix()


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sources", nargs="+", type=Path, help="Map file, symbol dump, or directory to normalize")
    parser.add_argument("--out-dir", type=Path, default=DEFAULT_OUT_DIR)
    parser.add_argument("--recursive", action="store_true", help="Recurse into input directories")
    args = parser.parse_args()

    out_dir = args.out_dir
    total_sources = 0
    total_symbols = 0
    written: list[Path] = []

    for source in args.sources:
        source_is_dir = source.is_dir()
        source_root = source if source_is_dir else source.parent
        for file_path in source_files(source, args.recursive):
            hits = parse_source(file_path)
            if not hits:
                continue
            out_path = output_path(source_root, file_path, out_dir, source_is_dir)
            write_tsv(out_path, hits, file_path)
            written.append(out_path)
            total_sources += 1
            total_symbols += len(hits)

    for path in written:
        print(f"Wrote {display_path(path)}")
    print(f"Sources: {total_sources}")
    print(f"Symbols: {total_symbols}")


if __name__ == "__main__":
    main()
