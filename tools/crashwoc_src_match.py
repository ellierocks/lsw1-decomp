#!/usr/bin/env python3
"""Cross-reference crashwoc-decomp C source names against LSW1 unnamed functions.

Reads the cloned crashwoc-decomp repo, extracts function names from:
  1. Embedded map comment blocks (addr size addr align name type)
  2. C function definition signatures

Then chains: crashwoc_name -> crashwoc_addr -> [body match in nu2_fn_matches.tsv] -> lsw1_fn_

Usage:
    python3 tools/crashwoc_src_match.py /tmp/crashwoc-decomp
    python3 tools/crashwoc_src_match.py /tmp/crashwoc-decomp --confidence HIGH
"""

import re
import sys
import csv
import argparse
from pathlib import Path
from collections import defaultdict, Counter

ROOT = Path(__file__).resolve().parent.parent
NU2_MATCHES = ROOT / "docs/symbol_donors/nu2_fn_matches.tsv"
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
OUT_TSV = ROOT / "docs/symbol_donors/crashwoc_src_rename_queue.tsv"
OUT_MD  = ROOT / "docs/symbol_donors/crashwoc_src_rename_queue.md"

# Modules that are safe to apply (Nu2 engine, not Crash-specific gamecode)
SAFE_MODULES = {"nu3dx", "nucore", "numath", "nuraster", "nusound", "system", "runtime",
                "gamelib"}
GAMECODE_MODULES = {"gamecode", "nuxbox"}

# Regex to match map comment entries: addr size addr align name type
MAP_COMMENT_LINE = re.compile(
    r'^\s*([0-9a-fA-F]{8})\s+([0-9a-fA-F]{4,8})\s+[0-9a-fA-F]{8}\s+\d+\s+(\S+)\s+(Global|Static)')

# Regex for C function definition: return_type funcname(args) [or just funcname(]
# Match lines like: void NuAnimUpdate(...)  or  struct foo* MyFunc(
C_FUNC_DEF = re.compile(
    r'^(?:(?:static\s+|inline\s+|const\s+)*)'     # optional qualifiers
    r'(?:[a-zA-Z_][a-zA-Z0-9_*\s]+ )'             # return type
    r'(\*?[a-zA-Z_][a-zA-Z0-9_]*)'                # function name (group 1)
    r'\s*\('                                        # opening paren
)


def extract_map_entries(src_dir: Path) -> dict[int, tuple[str, str, int]]:
    """Return {crashwoc_addr: (name, module, size)} from map comment blocks."""
    entries: dict[int, tuple[str, str, int]] = {}
    for c_file in src_dir.rglob("*.c"):
        module = c_file.parent.name
        in_comment = False
        for line in c_file.read_text(errors="replace").splitlines():
            stripped = line.strip()
            if "/*" in stripped:
                in_comment = True
            if in_comment:
                m = MAP_COMMENT_LINE.match(line)
                if m:
                    addr = int(m.group(1), 16)
                    size = int(m.group(2), 16)
                    name = m.group(3)
                    if addr not in entries:  # first occurrence wins
                        entries[addr] = (name, module, size)
            if "*/" in stripped:
                in_comment = False
    return entries


def extract_c_func_names(src_dir: Path) -> dict[str, str]:
    """Return {funcname: module} for all C function definitions found."""
    names: dict[str, str] = {}
    skip_prefixes = ("if", "for", "while", "switch", "return", "else", "do", "sizeof",
                     "typedef", "struct", "union", "enum", "#")
    for c_file in src_dir.rglob("*.c"):
        module = c_file.parent.name
        for line in c_file.read_text(errors="replace").splitlines():
            stripped = line.strip()
            if not stripped or stripped.startswith(skip_prefixes):
                continue
            m = C_FUNC_DEF.match(stripped)
            if m:
                name = m.group(1).lstrip("*")
                if len(name) > 2 and not name[0].isdigit():
                    names.setdefault(name, module)
    return names


def load_nu2_matches() -> dict[int, list[tuple[str, int, str, int]]]:
    """Return {crashwoc_addr: [(lsw1_fn, lsw1_size, match_type, crash_size)]} from nu2_fn_matches.tsv."""
    result: dict[int, list] = defaultdict(list)
    with NU2_MATCHES.open() as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            try:
                ca = int(row["crash_addr"], 16)
                ls = int(row["lsw1_size"], 16)
                cs = int(row["crash_size"], 16)
                result[ca].append((row["lsw1_fn"], ls, row["match_type"], cs))
            except (KeyError, ValueError):
                pass
    return result


def load_lsw1_symbols() -> tuple[dict[int, tuple[str, int]], set[str]]:
    """Return ({addr: (name, size)}, named_set)."""
    syms: dict[int, tuple[str, int]] = {}
    named: set[str] = set()
    for line in SYMBOLS.read_text(errors="replace").splitlines():
        if "type:function" not in line or "=" not in line:
            continue
        name = line.split("=")[0].strip()
        try:
            addr = int(line.split(":0x")[1].split(";")[0].strip(), 16)
            size = int(line.split("size:")[1].split(";")[0].strip(), 16)
        except (IndexError, ValueError):
            continue
        syms[addr] = (name, size)
        if not name.startswith("fn_"):
            named.add(name)
    return syms, named


def assign_confidence(module: str, match_type: str, size_diff: int, crash_size: int) -> str:
    if module in GAMECODE_MODULES:
        return "LOW"
    if match_type == "exact" and size_diff == 0:
        return "HIGH"
    if size_diff <= 8:
        return "MEDIUM"
    return "LOW"


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("src_dir", help="Path to cloned crashwoc-decomp repo")
    ap.add_argument("--confidence", choices=["HIGH", "MEDIUM", "LOW"], default="LOW",
                    help="Minimum confidence to include (default: LOW)")
    ap.add_argument("--gamecode", action="store_true",
                    help="Include gamecode module matches (default: skip)")
    args = ap.parse_args()

    src_dir = Path(args.src_dir)
    if not src_dir.exists():
        sys.exit(f"Not found: {src_dir}")

    print("Extracting map comment entries...", flush=True)
    map_entries = extract_map_entries(src_dir / "src")
    print(f"  {len(map_entries)} crashwoc addresses with names")

    print("Extracting C function definitions...", flush=True)
    c_names = extract_c_func_names(src_dir / "src")
    print(f"  {len(c_names)} C function names")

    print("Loading LSW1 symbols and body matches...", flush=True)
    lsw1_syms, lsw1_named = load_lsw1_symbols()
    nu2_map = load_nu2_matches()
    print(f"  {len(nu2_map)} crashwoc addresses with LSW1 body matches")
    print(f"  {len(lsw1_named)} already-named LSW1 functions")

    # Build candidate list: for each crashwoc addr with a C source name,
    # find LSW1 functions that body-match it
    CONF_ORDER = {"HIGH": 3, "MEDIUM": 2, "LOW": 1}
    min_conf = CONF_ORDER[args.confidence]

    candidates = []
    # Also track which names appear multiple times in C source (ambiguous)
    name_counts = Counter(n for n, _, _s in map_entries.values())

    seen_lsw1 = set()
    for crash_addr, (c_name, module, c_src_size) in map_entries.items():
        if not args.gamecode and module in GAMECODE_MODULES:
            continue
        if crash_addr not in nu2_map:
            continue
        if name_counts[c_name] > 1:
            continue  # same name at multiple crashwoc addrs = ambiguous

        for lsw1_fn, lsw1_size, match_type, crash_size in nu2_map[crash_addr]:
            cur_name, _ = lsw1_syms.get(int(lsw1_fn.replace("fn_", "0x"), 16), (lsw1_fn, 0))
            if not cur_name.startswith("fn_"):
                continue  # already named
            if lsw1_fn in seen_lsw1:
                continue
            if c_name in lsw1_named:
                continue  # name already used in LSW1 (would be CONFLICT)

            lsw1_addr = int(lsw1_fn.replace("fn_", "0x"), 16)
            size_diff = abs(lsw1_size - crash_size)
            conf = assign_confidence(module, match_type, size_diff, crash_size)
            if CONF_ORDER[conf] < min_conf:
                continue

            candidates.append({
                "confidence": conf,
                "module": module,
                "crash_name": c_name,
                "lsw1_fn": lsw1_fn,
                "lsw1_addr": hex(lsw1_addr),
                "lsw1_size": hex(lsw1_size),
                "crash_addr": hex(crash_addr),
                "crash_size": hex(crash_size),
                "size_diff": size_diff,
                "match_type": match_type,
            })
            seen_lsw1.add(lsw1_fn)

    # Sort by confidence desc, then module (safe first), then addr
    def sort_key(c):
        return (-CONF_ORDER[c["confidence"]],
                0 if c["module"] in SAFE_MODULES else 1,
                int(c["lsw1_addr"], 16))
    candidates.sort(key=sort_key)

    # Write TSV
    fields = ["confidence", "module", "crash_name", "lsw1_fn", "lsw1_addr",
              "lsw1_size", "crash_addr", "crash_size", "size_diff", "match_type"]
    with OUT_TSV.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields, delimiter="\t")
        w.writeheader()
        w.writerows(candidates)

    # Write markdown summary
    by_conf = defaultdict(list)
    for c in candidates:
        by_conf[c["confidence"]].append(c)

    lines = [f"# crashwoc-decomp C source rename candidates\n\n"]
    lines.append(f"Total: {len(candidates)} candidates "
                 f"(HIGH={len(by_conf['HIGH'])}, MEDIUM={len(by_conf['MEDIUM'])}, LOW={len(by_conf['LOW'])})\n\n")

    for conf in ("HIGH", "MEDIUM", "LOW"):
        group = by_conf[conf]
        if not group:
            continue
        lines.append(f"## {conf} ({len(group)})\n\n")
        lines.append(f"| Module | crash_name | lsw1_fn | lsw1_size | crash_size | diff | match |\n")
        lines.append(f"|---|---|---|---|---|---|---|\n")
        for c in group:
            lines.append(f"| {c['module']} | {c['crash_name']} | {c['lsw1_fn']} | "
                         f"{c['lsw1_size']} | {c['crash_size']} | {c['size_diff']} | {c['match_type']} |\n")
        lines.append("\n")

    OUT_MD.write_text("".join(lines))

    print(f"\nResults: {len(candidates)} candidates")
    print(f"  HIGH={len(by_conf['HIGH'])}  MEDIUM={len(by_conf['MEDIUM'])}  LOW={len(by_conf['LOW'])}")
    print(f"Wrote: {OUT_TSV}")
    print(f"Wrote: {OUT_MD}")


if __name__ == "__main__":
    main()
