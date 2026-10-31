#!/usr/bin/env python3
"""List the next small C-matching targets that need no decision from the agent.

A target is a named `.text` function in config/GL5E4F/symbols.txt that:
  - has no C definition under src/ yet,
  - has no verified hillclimb record under work/hillclimb/,
  - is not already claimed by a work/<name>/ directory, and
  - lies inside a split unit (so verify_fn.py can compare it).

Ranking: fewest distinct direct callees first, then smallest size; donor name
leads break ties. Callees and
sizes come from the retail DOL. Read-only; nothing is written.

    python3 tools/next_target.py
    python3 tools/next_target.py --max-size 0x80 --limit 30 --json
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import verify_fn as vf  # noqa: E402

try:
    from . import opensaga
except ImportError:
    import opensaga


ROOT = vf.ROOT
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
SRC = ROOT / "src"
LEDGERS = ROOT / "work" / "hillclimb"
WORK = ROOT / "work"

SYMBOL_RE = re.compile(r"^(\S+) = \.text:0x([0-9A-Fa-f]+); // type:function size:0x([0-9A-Fa-f]+)")
DEFINITION_RE = re.compile(r"^[A-Za-z_][^;{}()=]*?\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{", re.M)


def read_symbols() -> list[tuple[str, int, int]]:
    out = []
    for line in SYMBOLS.read_text().splitlines():
        m = SYMBOL_RE.match(line)
        if m:
            out.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return out


def defined_in_src() -> set[str]:
    names: set[str] = set()
    for path in SRC.rglob("*"):
        if path.suffix in (".c", ".cpp") and path.is_file():
            names.update(DEFINITION_RE.findall(path.read_text(errors="replace")))
    return names


def ledger_state() -> tuple[set[str], dict[str, int]]:
    verified: set[str] = set()
    attempts: dict[str, int] = {}
    for record in LEDGERS.glob("*/*.json") if LEDGERS.is_dir() else []:
        try:
            data = json.loads(record.read_text())
        except (OSError, ValueError):
            continue
        name = data.get("function")
        if not name:
            continue
        attempts[name] = attempts.get(name, 0) + 1
        if data.get("returncode") == 0:
            verified.add(name)
    return verified, attempts


def direct_callees(code: bytes, addr: int) -> set[int]:
    targets = set()
    for i in range(0, len(code) - len(code) % 4, 4):
        word = int.from_bytes(code[i:i + 4], "big")
        if (word >> 26) == 18 and word & 1 and not word & 2:
            offset = word & 0x03FFFFFC
            if offset & 0x02000000:
                offset -= 0x04000000
            targets.add(addr + i + offset)
    return targets


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--max-size", type=lambda s: int(s, 0), default=0x200, help="largest size in bytes (default 0x200)")
    ap.add_argument("--limit", type=int, default=15)
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args(argv)

    dol = vf.load_dol(vf.DOL_PATH, [])
    if dol is None:
        print(f"error: retail DOL missing at {vf.DOL_PATH.relative_to(ROOT)}; see README.md", file=sys.stderr)
        return vf.EXIT_ERROR
    donor_state = opensaga.snapshot()
    symbols = read_symbols()
    done = defined_in_src()
    verified, attempts = ledger_state()
    claimed = {p.name for p in WORK.iterdir() if p.is_dir()} if WORK.is_dir() else set()
    names_at = {addr: name for name, addr, _ in symbols}

    rows, skipped_unit = [], 0
    for name, addr, size in symbols:
        if not size or size > args.max_size or name in done or name in verified:
            continue
        unit = vf.lookup_unit(addr)
        if unit is None:
            skipped_unit += 1
            continue
        code = dol.read(addr, size)
        if code is None:
            continue
        callees = direct_callees(code, addr)
        source = vf.source_for_unit(unit)
        rows.append({
            "opensaga": opensaga.lookup(name, donor_state),
            "name": name, "address": f"0x{addr:08X}", "size": size, "unit": unit,
            "callees": len(callees), "callee_names": sorted(names_at.get(t, f"0x{t:08X}") for t in callees),
            "source": str(source.relative_to(ROOT)) if source else None,
            "claimed": name in claimed, "attempts": attempts.get(name, 0),
        })
    rows.sort(key=lambda r: (r["callees"], r["size"], not r["opensaga"]["matches"], r["address"]))
    rows_all = rows
    rows = rows[:args.limit]

    if args.json:
        print(json.dumps(rows, indent=2))
        return 0
    print(f"{'name':<22}{'address':<12}{'size':>6} {'callees':>7}  {'unit':<18}{'source':<28}{'saga':<14}state")
    for r in rows:
        state = "claimed" if r["claimed"] else ("attempted" if r["attempts"] else "ready")
        print(f"{r['name']:<22}{r['address']:<12}{r['size']:>#6x} {r['callees']:>7}  {r['unit']:<18}"
              f"{(r['source'] or '(new file)'):<28}{r['opensaga']['result']:<14}{state}")
    if rows:
        top = rows[0]
        src = top["source"] or f"src/{top['unit']}.c"
        print(f"\nNext for {top['name']}:")
        print(f"  python3 tools/opensaga.py lookup {top['name']}")
        print(f"  python3 tools/ls1_task_pack.py {top['name']} --out-dir work")
        print(f"  python3 tools/score_fn.py {top['name']} {src}")
        print(f"  python3 tools/hillclimb.py try {top['name']} --unit {top['unit']} --source {src} "
              "--hypothesis '<one falsifiable change>' --donor-note '<GC-grounded donor review or limit>'")
    print(f"\n{len(rows_all)} ready targets lie in split units. {skipped_unit} unmatched functions within "
          f"--max-size are outside any split unit and cannot be verified yet (see Phase 2 in docs/strategy.md).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
