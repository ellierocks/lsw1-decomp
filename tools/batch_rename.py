#!/usr/bin/env python3
"""Apply many symbol renames at once with a single build verification.

File format: one rename per line, "old_name new_name".
Lines starting with # are comments. Blank lines ignored.

Usage:
    python tools/batch_rename.py renames.txt
    python tools/batch_rename.py --no-build renames.txt
    python tools/batch_rename.py --dry-run renames.txt
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

try:
    from tools.rejected_renames import load_rejected
except ImportError:
    from rejected_renames import load_rejected

ROOT = Path(__file__).resolve().parent.parent
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("rename_file", help="File with 'old_name new_name' pairs")
    ap.add_argument("--no-build", action="store_true", help="Skip build verification")
    ap.add_argument("--dry-run", action="store_true", help="Show changes without applying")
    args = ap.parse_args()

    pairs = []
    with open(args.rename_file) as f:
        for lineno, line in enumerate(f, 1):
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) < 2:
                print(f"Warning: malformed line {lineno}: {line!r}")
                continue
            pairs.append((parts[0], parts[1]))

    if not pairs:
        print("No renames to apply.")
        return

    original = SYMTAB.read_text()
    result = original
    existing_names = set()
    for line in original.splitlines():
        if "=" in line and not line.strip().startswith("#"):
            existing_names.add(line.split("=")[0].strip())

    rejected = load_rejected()
    applied = []
    errors = []
    for old, new in pairs:
        target = f"{old} ="
        old_line = next((ln for ln in result.splitlines() if ln.startswith(target)), None)
        if old_line is None:
            errors.append(f"NOT FOUND: {old}")
            continue
        addr = re.search(r":0x([0-9A-Fa-f]+);", old_line)
        if addr and new in rejected.get(int(addr.group(1), 16), ()):
            errors.append(f"REJECTED: {new} is listed in rejected_renames.tsv for {old}")
            continue
        if new in existing_names and new != old:
            errors.append(f"CONFLICT: {new} already exists (trying to rename {old})")
            continue
        existing_names.discard(old)
        existing_names.add(new)
        # Replace in text (symbols.txt uses `name = .section:addr` format)
        result = result.replace(f"{old} =", f"{new} =", 1)
        applied.append((old, new))

    if errors:
        for e in errors:
            print(f"Error: {e}", file=sys.stderr)
        if not applied:
            sys.exit(1)
        print(f"Warning: {len(errors)} error(s), proceeding with {len(applied)} valid renames.")

    if args.dry_run:
        for old, new in applied:
            print(f"  {old} → {new}")
        print(f"\n{len(applied)} renames would be applied.")
        return

    SYMTAB.write_text(result)
    print(f"Applied {len(applied)} renames.")

    if args.no_build:
        return

    print("Verifying build...")
    res = subprocess.run(
        ["bash", "build.sh"], cwd=ROOT, capture_output=True, text=True, timeout=300
    )
    if res.returncode == 0:
        print("Build: OK")
    else:
        print("Build FAILED — reverting all renames...")
        print(res.stderr[-800:])
        SYMTAB.write_text(original)
        print(f"Reverted {len(applied)} renames.")
        sys.exit(1)


if __name__ == "__main__":
    main()
