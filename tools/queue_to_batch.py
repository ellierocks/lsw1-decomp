#!/usr/bin/env python3
"""Convert rename-queue TSVs to a batch file for batch_rename.py.

Reads the pre-analysed rename queues in docs/symbol_donors/ and produces
a batch rename file filtered by confidence level.

Usage:
    python tools/queue_to_batch.py                           # MEDIUM+ only
    python tools/queue_to_batch.py --min-conf LOW            # all entries
    python tools/queue_to_batch.py -o work/batch.txt         # write to file
    python tools/queue_to_batch.py --queues sdk crash mac    # select queues
"""
import argparse
import csv
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYMTAB = ROOT / "config/GL5E4F/symbols.txt"
DONORS = ROOT / "docs/symbol_donors"

QUEUES = {
    "sdk": DONORS / "sdk_anchor_rename_queue.tsv",
    "crash": DONORS / "crashwoc_fn_rename_queue.tsv",
    "mac": DONORS / "mac_anchor_rename_queue.tsv",
    "nu2": DONORS / "nu2_fn_rename_queue.tsv",
}

CONF_ORDER = {"HIGH": 3, "MEDIUM": 2, "LOW": 1}


def load_unnamed() -> set[str]:
    unnamed = set()
    for line in SYMTAB.read_text(errors="replace").splitlines():
        if "type:function" in line and "=" in line:
            n = line.split("=")[0].strip()
            if n.startswith("fn_"):
                unnamed.add(n)
    return unnamed


def load_named_set() -> set[str]:
    named = set()
    for line in SYMTAB.read_text(errors="replace").splitlines():
        if "type:function" in line and "=" in line:
            n = line.split("=")[0].strip()
            named.add(n)
    return named


def parse_sdk_queue(path: Path, min_conf: int) -> list[tuple[str, str, str, str]]:
    rows = []
    with open(path) as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            conf = row.get("confidence", "").upper()
            if CONF_ORDER.get(conf, 0) < min_conf:
                continue
            gc_old = row.get("gc_old", "").strip()
            new_name = row.get("new_name", "").strip()
            if gc_old and new_name:
                rows.append((conf, gc_old, new_name, f"sdk [{row.get('left_anchor','')}..{row.get('right_anchor','')}]"))
    return rows


def parse_crash_queue(path: Path, min_conf: int) -> list[tuple[str, str, str, str]]:
    rows = []
    with open(path) as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            conf = row.get("confidence", "").upper()
            if CONF_ORDER.get(conf, 0) < min_conf:
                continue
            gc_fn = row.get("gc_fn", "").strip()
            crash_name = row.get("crash_name", "").strip()
            if gc_fn and crash_name:
                rows.append((conf, gc_fn, crash_name, f"crashwoc [{row.get('notes','')}]"))
    return rows


def parse_mac_queue(path: Path, min_conf: int) -> list[tuple[str, str, str, str]]:
    rows = []
    with open(path) as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            conf = row.get("confidence", "").upper()
            if CONF_ORDER.get(conf, 0) < min_conf:
                continue
            gc_old = row.get("gc_old", "").strip()
            new_name = row.get("new_name", "").strip()
            if gc_old and new_name:
                rows.append((conf, gc_old, new_name, f"mac [{row.get('mac_source','')} {row.get('left_anchor','')}..{row.get('right_anchor','')}]"))
    return rows


def parse_nu2_queue(path: Path, min_conf: int) -> list[tuple[str, str, str, str]]:
    # Same format as crash queue
    return parse_crash_queue(path, min_conf)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--min-conf", choices=["HIGH", "MEDIUM", "LOW"], default="MEDIUM",
                    help="Minimum confidence level to include (default: MEDIUM)")
    ap.add_argument("-o", "--output", help="Write batch to this file (default: stdout)")
    ap.add_argument("--queues", nargs="+", choices=list(QUEUES.keys()),
                    default=list(QUEUES.keys()),
                    help="Which queues to include (default: all)")
    args = ap.parse_args()

    min_conf = CONF_ORDER[args.min_conf]
    unnamed = load_unnamed()
    named_set = load_named_set()

    all_rows: list[tuple[str, str, str, str]] = []
    parsers = {"sdk": parse_sdk_queue, "crash": parse_crash_queue,
               "mac": parse_mac_queue, "nu2": parse_nu2_queue}

    for key in args.queues:
        path = QUEUES[key]
        if not path.exists():
            print(f"Warning: {path} not found", file=sys.stderr)
            continue
        rows = parsers[key](path, min_conf)
        all_rows.extend(rows)
        print(f"  {key}: {len(rows)} candidates", file=sys.stderr)

    # Deduplicate: prefer higher confidence, skip already-named targets
    seen_gc: dict[str, tuple[str, str, str, str]] = {}
    for conf, gc_old, new_name, note in all_rows:
        if gc_old not in unnamed:
            continue  # already named or doesn't exist
        if new_name in named_set and new_name != gc_old:
            continue  # new name already in use
        if gc_old in seen_gc:
            prev_conf = seen_gc[gc_old][0]
            if CONF_ORDER.get(conf, 0) <= CONF_ORDER.get(prev_conf, 0):
                continue  # keep higher confidence
        seen_gc[gc_old] = (conf, gc_old, new_name, note)

    # Sort by confidence desc, then by gc_old address
    sorted_rows = sorted(
        seen_gc.values(),
        key=lambda r: (-CONF_ORDER.get(r[0], 0), r[1])
    )

    lines = [f"# queue_to_batch.py --min-conf {args.min_conf} ({len(sorted_rows)} renames)\n"]
    cur_conf = None
    for conf, gc_old, new_name, note in sorted_rows:
        if conf != cur_conf:
            lines.append(f"\n# === {conf} ===")
            cur_conf = conf
        lines.append(f"{gc_old} {new_name}  # {note}")

    output = "\n".join(lines) + "\n"
    if args.output:
        Path(args.output).parent.mkdir(parents=True, exist_ok=True)
        Path(args.output).write_text(output)
        print(f"Wrote {len(sorted_rows)} renames to {args.output}", file=sys.stderr)
    else:
        print(output)

    print(f"Total: {len(sorted_rows)} renames", file=sys.stderr)


if __name__ == "__main__":
    main()
