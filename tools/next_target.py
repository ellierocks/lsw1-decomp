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
    python3 tools/next_target.py --min-size 0x80 --max-size 0x400 --unit auto/game/ --limit 50
    python3 tools/next_target.py --by-unit --max-size 0x200 --limit 20
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
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


FINISHABLE = 10
SPRINT_PREFIX = "t3/sprint-"


def squash(text: str) -> str:
    return re.sub(r"[^a-z0-9]", "", text.lower())


def sprint_branches(remote: bool = True) -> set[str]:
    """Names (without the `t3/sprint-` prefix) of active sprint branches, excluding the current one.

    Active means an open PR (via `gh`) or a local/`origin` sprint branch not yet merged into
    origin/main: a sprint that is pushed but has no PR yet, or lives in another local worktree,
    still owns its unit. Run `git fetch` first for an up-to-date view of other machines.
    """
    heads: set[str] = set()
    try:
        out = subprocess.run(["git", "branch", "-a", "--no-merged", "origin/main", "--format=%(refname:short)",
                              "--list", SPRINT_PREFIX + "*", "origin/" + SPRINT_PREFIX + "*"],
                             cwd=ROOT, capture_output=True, text=True, timeout=15)
        if out.returncode == 0:
            heads.update(h.removeprefix("origin/") for h in out.stdout.split())
    except (OSError, subprocess.TimeoutExpired):
        pass
    if remote:
        try:
            out = subprocess.run(["gh", "pr", "list", "--state", "open", "--json", "headRefName",
                                  "--jq", ".[].headRefName"], cwd=ROOT, capture_output=True, text=True,
                                 timeout=15)
            heads.update(out.stdout.split() if out.returncode == 0 else [])
        except (OSError, subprocess.TimeoutExpired):
            pass
    try:
        cur = subprocess.run(["git", "branch", "--show-current"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
    except OSError:
        cur = ""
    return {h[len(SPRINT_PREFIX):] for h in heads if h.startswith(SPRINT_PREFIX) and h != cur}


def owned_by_sprint(unit: str, branches: set[str]) -> bool:
    """A sprint branch owns a unit when its name matches the unit's leaf or its whole path."""
    keys = {squash(unit.rsplit("/", 1)[-1]), squash(unit), squash(unit.removeprefix("auto/"))}
    return any(squash(b) in keys for b in branches)


def in_units(unit: str, prefixes: list[str]) -> bool:
    return not prefixes or any(unit.startswith(p) for p in prefixes)


def print_by_unit(rows, args, owned=frozenset()) -> int:
    """One line per unit: ready targets, bytes, size range and how many functions the unit has left."""
    units = {}
    for r in rows:
        if r["claimed"]:
            continue
        units.setdefault(r["unit"], []).append(r)
    skipped = sorted(u for u in units if u in owned)
    for u in skipped:
        del units[u]

    def key(kv):
        u, rs = kv
        finish = rs[0]["unit_left"] <= FINISHABLE
        return (not finish, -len(rs), sum(r["size"] for r in rs), u)

    order = sorted(units.items(), key=key)[:args.limit]
    if args.json:
        print(json.dumps([{"unit": u, "ready": len(rs), "left": rs[0]["unit_left"],
                           "finishable": rs[0]["unit_left"] <= FINISHABLE, "bytes": sum(r["size"] for r in rs),
                           "min_size": min(r["size"] for r in rs), "max_size": max(r["size"] for r in rs),
                           "source": rs[0]["source"], "names": [r["name"] for r in rs]}
                          for u, rs in order], indent=2))
        return 0
    print(f"{'unit':<34}{'ready':>6}{'left':>6}{'bytes':>8}  {'sizes':<14}source")
    for unit, rs in order:
        sizes = sorted(r["size"] for r in rs)
        left = rs[0]["unit_left"]
        mark = "*" if left <= FINISHABLE else " "
        print(f"{unit:<34}{len(rs):>6}{left:>5}{mark}{sum(sizes):>#8x}  {f'{sizes[0]:#x}..{sizes[-1]:#x}':<14}"
              f"{rs[0]['source'] or '(new file)'}")
    print(f"\n* = {FINISHABLE} or fewer functions left in the unit (all sizes): finishing it allows promotion.")
    if skipped:
        print(f"Skipped (open sprint PR): {', '.join(skipped)}")
    if order:
        print(f"\nSprint queue for the first unit:\n  python3 tools/next_target.py --unit {order[0][0]} "
              f"--max-size {args.max_size:#x} --limit 40")
    return 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--max-size", type=lambda s: int(s, 0), default=0x200, help="largest size in bytes (default 0x200)")
    ap.add_argument("--min-size", type=lambda s: int(s, 0), default=0, help="smallest size in bytes (default 0)")
    ap.add_argument("--unit", action="append", default=[], metavar="PREFIX",
                    help="only units starting with PREFIX, e.g. auto/game/ (repeatable)")
    ap.add_argument("--limit", type=int, default=15)
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--by-unit", action="store_true",
                    help="summarize ready targets per unit to scope a sprint (finishable units first, "
                         "then by ready count; units owned by an open t3/sprint-* PR are skipped)")
    ap.add_argument("--no-remote", action="store_true", help="do not ask GitHub for open sprint PRs")
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

    unit_left: dict[str, int] = {}
    unit_of: dict[str, str | None] = {}
    for name, addr, size in symbols:
        if size and name not in done and name not in verified:
            u = vf.lookup_unit(addr)
            unit_of[name] = u
            if u is not None:
                unit_left[u] = unit_left.get(u, 0) + 1

    rows, skipped_unit = [], 0
    for name, addr, size in symbols:
        if not size or size > args.max_size or size < args.min_size or name in done or name in verified:
            continue
        unit = unit_of[name]
        if unit is None:
            skipped_unit += 1
            continue
        if not in_units(unit, args.unit):
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
            "source_path": str(source.relative_to(ROOT)) if source else f"src/{unit}.c",
            "claimed": name in claimed, "attempts": attempts.get(name, 0),
            "unit_left": unit_left.get(unit, 0),
        })
    rows.sort(key=lambda r: (r["callees"], r["size"], not r["opensaga"]["matches"], r["address"]))
    rows_all = rows
    if args.by_unit:
        branches = sprint_branches(remote=not args.no_remote)
        owned = {r["unit"] for r in rows_all if owned_by_sprint(r["unit"], branches)}
        return print_by_unit(rows_all, args, owned)
    rows = rows[:args.limit]

    if args.json:
        print(json.dumps(rows, indent=2))
        return 0
    uw = max([18] + [len(r["unit"]) + 2 for r in rows])
    sw = max([28] + [len(r["source"] or "(new file)") + 2 for r in rows])
    print(f"{'name':<22}{'address':<12}{'size':>6} {'callees':>7}  {'unit':<{uw}}{'source':<{sw}}{'saga':<14}state")
    for r in rows:
        state = "claimed" if r["claimed"] else ("attempted" if r["attempts"] else "ready")
        print(f"{r['name']:<22}{r['address']:<12}{r['size']:>#6x} {r['callees']:>7}  {r['unit']:<{uw}}"
              f"{(r['source'] or '(new file)'):<{sw}}{r['opensaga']['result']:<14}{state}")
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
