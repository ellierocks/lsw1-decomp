#!/usr/bin/env python3
"""
Extract main executables from each known build.

Reads builds.tsv, processes all builds with available=yes and a disc_path,
and extracts the main executable to the exe_path if not already present.
Updates builds.tsv in-place with exe_size and exe_sha256.

Usage:
    python3 tools/revisions/extract.py [--force]
"""
from __future__ import annotations

import csv
import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILDS_TSV = ROOT / "research" / "revisions" / "builds.tsv"

sys.path.insert(0, str(ROOT / "tools"))
from revisions.formats.ps2_bin import extract_ps2_exe
from revisions.formats.xiso import extract_xbe
from revisions.formats.gc_iso import extract_gc_dol


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def extract_build(row: dict, force: bool = False) -> dict:
    build_id = row["build_id"]
    platform = row["platform"]
    disc_fmt = row["disc_format"]
    disc_raw = row.get("disc_path", "").strip()
    exe_raw = row.get("exe_path", "").strip()

    if not disc_raw and not exe_raw:
        return row
    if row.get("available", "no").strip() != "yes":
        return row

    exe_path = ROOT / exe_raw if exe_raw else None

    # If exe already exists and has a hash, skip unless forced
    if exe_path and exe_path.exists() and row.get("exe_sha256", "").strip() and not force:
        return row

    # If exe already exists, just fill in hash/size
    if exe_path and exe_path.exists():
        row["exe_size"] = str(exe_path.stat().st_size)
        row["exe_sha256"] = sha256(exe_path)
        print(f"  [{build_id}] hashed existing {exe_path.name}")
        return row

    # Need to extract
    if not disc_raw:
        return row
    disc_path = ROOT / disc_raw
    if not disc_path.exists():
        print(f"  [{build_id}] disc not found: {disc_path}", file=sys.stderr)
        return row

    print(f"  [{build_id}] extracting from {disc_path.name} ...")
    extracted = None

    if disc_fmt == "dol":
        # Already a DOL file or the disc_path is a DOL
        if disc_path.suffix.lower() == ".dol":
            exe_path = disc_path
        elif disc_path.suffix.lower() in (".iso",):
            exe_path = ROOT / "build" / "region" / build_id / "main.dol"
            extracted = extract_gc_dol(disc_path, exe_path)
    elif disc_fmt == "iso9660" and platform == "gc":
        exe_path = ROOT / "build" / "region" / build_id / "main.dol"
        extracted = extract_gc_dol(disc_path, exe_path)
    elif disc_fmt in ("bin_cue", "iso9660") and platform == "ps2":
        if not exe_path:
            exe_path = ROOT / "build" / "revisions" / build_id / "main.elf"
        extracted = extract_ps2_exe(disc_path, exe_path)
    elif disc_fmt == "xiso":
        if not exe_path:
            exe_path = ROOT / "build" / "revisions" / build_id / "default.xbe"
        extracted = extract_xbe(disc_path, exe_path)
    elif disc_fmt == "raw_exe":
        if exe_path and exe_path.exists():
            extracted = exe_path
    elif disc_fmt in ("mach_o", "extracted"):
        if exe_path and exe_path.exists():
            extracted = exe_path

    if extracted and extracted.exists():
        row["exe_path"] = str(extracted.relative_to(ROOT))
        row["exe_size"] = str(extracted.stat().st_size)
        row["exe_sha256"] = sha256(extracted)
        print(f"  [{build_id}] -> {extracted.name} ({extracted.stat().st_size:,} bytes)")
    elif exe_path and exe_path.exists():
        row["exe_path"] = str(exe_path.relative_to(ROOT))
        row["exe_size"] = str(exe_path.stat().st_size)
        row["exe_sha256"] = sha256(exe_path)
        print(f"  [{build_id}] -> {exe_path.name} ({exe_path.stat().st_size:,} bytes)")
    else:
        print(f"  [{build_id}] extraction failed", file=sys.stderr)

    return row


def main():
    force = "--force" in sys.argv
    rows = []
    with open(BUILDS_TSV, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        fieldnames = reader.fieldnames
        for row in reader:
            updated = extract_build(row, force=force)
            rows.append(updated)

    with open(BUILDS_TSV, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames, delimiter="\t")
        writer.writeheader()
        for row in rows:
            writer.writerow({k: v for k, v in row.items() if k in fieldnames})

    print("builds.tsv updated.")


if __name__ == "__main__":
    main()
