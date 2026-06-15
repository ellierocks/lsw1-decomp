#!/usr/bin/env python3
"""
Compute pairwise similarity between build executables.

Methods:
  positional_byte  — byte-by-byte comparison at same offsets (same-arch pairs)
  chunk_hash       — divide into 4KB chunks, count matching chunk hashes
  string_jaccard   — Jaccard similarity of extracted printable strings (>=8 chars)
  key_string_jaccard — Jaccard on the Nu-engine / debug string subset

Output: research/revisions/similarity_matrix.tsv

Usage:
    python3 tools/revisions/similarity.py [--rebuild]
"""
from __future__ import annotations

import csv
import hashlib
import itertools
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILDS_TSV = ROOT / "research" / "revisions" / "builds.tsv"
MATRIX_TSV = ROOT / "research" / "revisions" / "similarity_matrix.tsv"

MATRIX_COLS = [
    "build_a", "build_b", "arch_a", "arch_b", "same_arch",
    "method", "score", "matched", "total", "notes",
]

CHUNK_SIZE = 4096
MIN_STRING_LEN = 8

KEY_RE = re.compile(
    r"(Nu[A-Z]|instNu|AIPath|AISys|AIScript|Camera|SCRIPT|TRIGGER|"
    r"LEGO Options|Go To|Debug|debug|assert|\.c$|\.cpp$|gameapi|nu2api|"
    r"nu3d|cannot|Unable|failed|internal error)",
    re.IGNORECASE,
)

PLATFORM_ARCH = {
    "ps2": "mips",
    "gc": "ppc",
    "xbox": "x86",
    "pc": "x86",
    "mac": "ppc",
}


def extract_strings(path: Path) -> set[str]:
    result = subprocess.run(
        ["strings", "-n", str(MIN_STRING_LEN), str(path)],
        capture_output=True, text=True,
    )
    return set(result.stdout.splitlines())


def positional_byte(a: Path, b: Path) -> tuple[float, int, int]:
    """Compare bytes at same offsets up to min(len(a), len(b))."""
    size = min(a.stat().st_size, b.stat().st_size)
    matched = 0
    with open(a, "rb") as fa, open(b, "rb") as fb:
        remaining = size
        while remaining > 0:
            n = min(remaining, 1 << 20)
            ca, cb = fa.read(n), fb.read(n)
            matched += sum(x == y for x, y in zip(ca, cb))
            remaining -= n
    score = matched / size if size else 0.0
    return score, matched, size


def chunk_hash(a: Path, b: Path) -> tuple[float, int, int]:
    """Count matching 4KB chunk hashes."""
    def get_chunks(p: Path) -> set[bytes]:
        chunks = set()
        with open(p, "rb") as f:
            while True:
                c = f.read(CHUNK_SIZE)
                if not c:
                    break
                if len(c) == CHUNK_SIZE:
                    chunks.add(hashlib.md5(c).digest())
        return chunks

    ca, cb = get_chunks(a), get_chunks(b)
    intersection = len(ca & cb)
    union = len(ca | cb)
    score = intersection / union if union else 0.0
    return score, intersection, union


def string_jaccard(sa: set[str], sb: set[str]) -> tuple[float, int, int]:
    intersection = len(sa & sb)
    union = len(sa | sb)
    score = intersection / union if union else 0.0
    return score, intersection, union


def load_builds() -> list[dict]:
    with open(BUILDS_TSV, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def main():
    rebuild = "--rebuild" in sys.argv
    builds = [b for b in load_builds() if b.get("available", "").strip() == "yes"]

    # Load existing matrix to avoid recomputing
    existing: set[tuple[str, str, str]] = set()
    existing_rows: list[dict] = []
    if MATRIX_TSV.exists() and not rebuild:
        with open(MATRIX_TSV, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f, delimiter="\t"):
                existing.add((r["build_a"], r["build_b"], r["method"]))
                existing_rows.append(r)

    new_rows: list[dict] = []

    # Cache strings per build
    string_cache: dict[str, set[str]] = {}
    key_string_cache: dict[str, set[str]] = {}

    def get_strings(b: dict) -> tuple[set[str], set[str]]:
        bid = b["build_id"]
        if bid not in string_cache:
            exe_raw = b.get("exe_path", "").strip()
            if not exe_raw:
                string_cache[bid] = set()
                key_string_cache[bid] = set()
            else:
                exe = ROOT / exe_raw
                if exe.exists():
                    strs = extract_strings(exe)
                    string_cache[bid] = strs
                    key_string_cache[bid] = {s for s in strs if KEY_RE.search(s)}
                else:
                    string_cache[bid] = set()
                    key_string_cache[bid] = set()
        return string_cache[bid], key_string_cache[bid]

    pairs = list(itertools.combinations(builds, 2))
    print(f"Computing {len(pairs)} pairs × 4 methods ...")

    for a, b in pairs:
        aid, bid = a["build_id"], b["build_id"]
        arch_a = PLATFORM_ARCH.get(a["platform"], "?")
        arch_b = PLATFORM_ARCH.get(b["platform"], "?")
        same_arch = "yes" if arch_a == arch_b else "no"

        exe_a_raw = a.get("exe_path", "").strip()
        exe_b_raw = b.get("exe_path", "").strip()
        exe_a = ROOT / exe_a_raw if exe_a_raw else None
        exe_b = ROOT / exe_b_raw if exe_b_raw else None

        sa, ksa = get_strings(a)
        sb, ksb = get_strings(b)

        # String Jaccard (all platforms)
        key = (aid, bid, "string_jaccard")
        if key not in existing and sa and sb:
            score, matched, total = string_jaccard(sa, sb)
            new_rows.append({
                "build_a": aid, "build_b": bid,
                "arch_a": arch_a, "arch_b": arch_b, "same_arch": same_arch,
                "method": "string_jaccard", "score": f"{score:.4f}",
                "matched": str(matched), "total": str(total), "notes": "",
            })

        # Key-string Jaccard
        key = (aid, bid, "key_string_jaccard")
        if key not in existing and ksa and ksb:
            score, matched, total = string_jaccard(ksa, ksb)
            new_rows.append({
                "build_a": aid, "build_b": bid,
                "arch_a": arch_a, "arch_b": arch_b, "same_arch": same_arch,
                "method": "key_string_jaccard", "score": f"{score:.4f}",
                "matched": str(matched), "total": str(total), "notes": "",
            })

        if exe_a and exe_b and exe_a.exists() and exe_b.exists() and same_arch == "yes":
            # Positional byte match
            key = (aid, bid, "positional_byte")
            if key not in existing:
                print(f"  positional_byte {aid} vs {bid} ...")
                score, matched, total = positional_byte(exe_a, exe_b)
                new_rows.append({
                    "build_a": aid, "build_b": bid,
                    "arch_a": arch_a, "arch_b": arch_b, "same_arch": same_arch,
                    "method": "positional_byte", "score": f"{score:.4f}",
                    "matched": str(matched), "total": str(total),
                    "notes": f"vs min({exe_a.stat().st_size},{exe_b.stat().st_size})",
                })

            # Chunk hash
            key = (aid, bid, "chunk_hash")
            if key not in existing:
                print(f"  chunk_hash {aid} vs {bid} ...")
                score, matched, total = chunk_hash(exe_a, exe_b)
                new_rows.append({
                    "build_a": aid, "build_b": bid,
                    "arch_a": arch_a, "arch_b": arch_b, "same_arch": same_arch,
                    "method": "chunk_hash", "score": f"{score:.4f}",
                    "matched": str(matched), "total": str(total), "notes": "4KB chunks",
                })

    all_rows = existing_rows + new_rows
    MATRIX_TSV.parent.mkdir(parents=True, exist_ok=True)
    with open(MATRIX_TSV, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=MATRIX_COLS, delimiter="\t")
        writer.writeheader()
        writer.writerows(all_rows)

    print(f"\nWrote {len(all_rows)} rows ({len(new_rows)} new) to {MATRIX_TSV}")


if __name__ == "__main__":
    main()
