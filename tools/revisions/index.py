#!/usr/bin/env python3
"""
Build index — generate hashes.tsv for all files in each available build.

For disc images: hashes the disc and the extracted executable.
For extracted directories (xbox_demo_oxm045): hashes all files.

Output: research/revisions/hashes.tsv

Usage:
    python3 tools/revisions/index.py
"""
from __future__ import annotations

import csv
import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILDS_TSV = ROOT / "research" / "revisions" / "builds.tsv"
HASHES_TSV = ROOT / "research" / "revisions" / "hashes.tsv"

HASH_COLS = ["build_id", "file_role", "rel_path", "size", "md5", "sha256"]


def hash_file(path: Path) -> tuple[str, str]:
    md5 = hashlib.md5()
    sha = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            md5.update(chunk)
            sha.update(chunk)
    return md5.hexdigest(), sha.hexdigest()


def index_build(row: dict) -> list[dict]:
    build_id = row["build_id"]
    if row.get("available", "").strip() != "yes":
        return []

    entries = []

    def add(role: str, path: Path):
        if not path.exists():
            return
        md5, sha = hash_file(path)
        entries.append({
            "build_id": build_id,
            "file_role": role,
            "rel_path": str(path.relative_to(ROOT)),
            "size": str(path.stat().st_size),
            "md5": md5,
            "sha256": sha,
        })

    # Disc image
    disc_raw = row.get("disc_path", "").strip()
    if disc_raw:
        disc_path = ROOT / disc_raw
        if disc_path.exists() and disc_path.is_file():
            add("disc", disc_path)

    # Main executable
    exe_raw = row.get("exe_path", "").strip()
    if exe_raw:
        exe_path = ROOT / exe_raw
        if exe_path.exists() and exe_path.is_file():
            add("exe", exe_path)

    # For extracted builds (xbox demo): hash all loose files
    disc_fmt = row.get("disc_format", "").strip()
    if disc_fmt == "extracted" and disc_raw:
        disc_dir = ROOT / disc_raw
        if disc_dir.exists() and disc_dir.is_dir():
            for f in sorted(disc_dir.rglob("*")):
                if f.is_file() and f != (ROOT / exe_raw if exe_raw else None):
                    md5, sha = hash_file(f)
                    entries.append({
                        "build_id": build_id,
                        "file_role": "asset",
                        "rel_path": str(f.relative_to(ROOT)),
                        "size": str(f.stat().st_size),
                        "md5": md5,
                        "sha256": sha,
                    })

    return entries


def main():
    builds = []
    with open(BUILDS_TSV, newline="", encoding="utf-8") as f:
        builds = list(csv.DictReader(f, delimiter="\t"))

    all_entries = []
    for row in builds:
        build_id = row["build_id"]
        print(f"  indexing {build_id} ...")
        entries = index_build(row)
        all_entries.extend(entries)
        print(f"    {len(entries)} file(s)")

    HASHES_TSV.parent.mkdir(parents=True, exist_ok=True)
    with open(HASHES_TSV, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=HASH_COLS, delimiter="\t")
        writer.writeheader()
        writer.writerows(all_entries)

    print(f"\nWrote {len(all_entries)} entries to {HASHES_TSV}")


if __name__ == "__main__":
    main()
