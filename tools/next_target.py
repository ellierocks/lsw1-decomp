#!/usr/bin/env python3
"""List the next small C-matching targets that need no decision from the agent.

A target is a named `.text` function in config/GL5E4F/symbols.txt that:
  - has no C definition under src/ yet,
  - has no verified hillclimb record under work/hillclimb/,
  - is not already claimed by a work/<name>/ directory, and
  - lies inside a split unit (so verify_fn.py can compare it).

By default the list is the plan target set (tools/plan.py: the item this checkout claimed,
else the plan's next item), including its functions already drafted in src/, with no size
limit; `--all` (or `--off-plan REASON`) ranks every ready target as before (default
`--max-size 0x200`), and `--by-unit` without it lists the target stage's plan items.

Ranking: fewest distinct direct callees first, then smallest size; donor name
leads, then a Japanese PC 1.0 pair (readable x86 body), break ties. Callees and
sizes come from the retail DOL. Read-only; nothing is written. The `jp` column
reads build/pc_analysis/gc_pc_anchors.tsv (`python3 tools/pc_gc_anchors.py`);
`--jp-paired` keeps only paired targets (see docs/research.md#pc-builds).

    python3 tools/next_target.py                  # the plan item's targets
    python3 tools/next_target.py --all --max-size 0x80 --limit 30 --json
    python3 tools/next_target.py --all --min-size 0x80 --max-size 0x400 --unit auto/game/ --limit 50
    python3 tools/next_target.py --all --by-unit --max-size 0x200 --limit 20
    python3 tools/next_target.py --all --jp-paired --max-size 0x400 --limit 40
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
    from . import pc_gc_anchors
except ImportError:
    import opensaga
    import pc_gc_anchors


ROOT = vf.ROOT
SYMBOLS = ROOT / "config/GL5E4F/symbols.txt"
SRC = ROOT / "src"
LEDGERS = ROOT / "work" / "hillclimb"
WORK = ROOT / "work"

SYMBOL_RE = re.compile(r"^(\S+) = \.text:0x([0-9A-Fa-f]+); // type:function size:0x([0-9A-Fa-f]+)")
DEFINITION_RE = re.compile(r"^[A-Za-z_][^;{}()=]*?\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{", re.M)
# newlib's K&R-compatible definitions: `_DEFUN (strlen, (str), ...)` / `_DEFUN_VOID (abort)`
NEWLIB_DEFUN_RE = re.compile(r"^_DEFUN(?:_VOID)?\s*\(\s*([A-Za-z_]\w*)\b", re.M)
# K&R definitions: `vprintf (fmt, ap)` then parameter declarations, then the body
KR_DEFINITION_RE = re.compile(r"^([A-Za-z_]\w*)\s*\([\w\s,]*\)\s*\n(?:[ \t]+[^;{}()\n]+;[ \t]*\n)+\{", re.M)


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
            text = path.read_text(errors="replace")
            names.update(DEFINITION_RE.findall(text))
            names.update(NEWLIB_DEFUN_RE.findall(text))
            names.update(KR_DEFINITION_RE.findall(text))
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
WAVE_SIZE = 10
SPRINT_PREFIX = "t3/sprint-"


def squash(text: str) -> str:
    return re.sub(r"[^a-z0-9]", "", text.lower())


REPO = "ellierocks767/lsw1-decomp"


def active_branches(prefix: str, remote: bool = True, include_current: bool = False) -> set[str]:
    """Names (without PREFIX) of active PREFIX* branches, excluding the current one unless
    INCLUDE_CURRENT.

    Active means an open PR (via `gh`, on REPO) or a local/`origin` branch not yet merged into
    origin/main: a sprint that is pushed but has no PR yet, or lives in another local worktree,
    still owns its target. Run `git fetch` first for an up-to-date view of other machines.
    """
    heads: set[str] = set()
    try:
        out = subprocess.run(["git", "branch", "-a", "--no-merged", "origin/main", "--format=%(refname:short)",
                              "--list", prefix + "*", "origin/" + prefix + "*"],
                             cwd=ROOT, capture_output=True, text=True, timeout=15)
        if out.returncode == 0:
            heads.update(h.removeprefix("origin/") for h in out.stdout.split())
    except (OSError, subprocess.TimeoutExpired):
        pass
    if remote:
        try:
            out = subprocess.run(["gh", "pr", "list", "--repo", REPO, "--state", "open", "--json", "headRefName",
                                  "--jq", ".[].headRefName"], cwd=ROOT, capture_output=True, text=True,
                                 timeout=15)
            heads.update(out.stdout.split() if out.returncode == 0 else [])
        except (OSError, subprocess.TimeoutExpired):
            pass
    try:
        cur = subprocess.run(["git", "branch", "--show-current"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
    except OSError:
        cur = ""
    return {h[len(prefix):] for h in heads if h.startswith(prefix) and (include_current or h != cur)}


def sprint_branches(remote: bool = True) -> set[str]:
    """Names (without the `t3/sprint-` prefix) of active sprint branches, excluding the current one."""
    return active_branches(SPRINT_PREFIX, remote=remote)


def unit_keys(unit: str) -> set[str]:
    return {squash(unit.rsplit("/", 1)[-1]), squash(unit), squash(unit.removeprefix("auto/"))}


def owned_by_sprint(unit: str, branches: set[str], units: frozenset[str] = frozenset()) -> bool:
    """A sprint branch owns a unit when its name matches the unit's leaf or its whole path. A second
    sprint on a unit (`levels-2`) owns it too, unless the suffixed name is itself a unit (`nuanim-2`)."""
    keys, taken = unit_keys(unit), {k for u in units for k in unit_keys(u)}
    for b in branches:
        base = re.sub(r"-\d+$", "", b)
        if squash(b) in keys or (base != b and squash(b) not in taken and squash(base) in keys):
            return True
    return False


def in_units(unit: str, prefixes: list[str]) -> bool:
    return not prefixes or any(unit.startswith(p) for p in prefixes)


def rank_units(rows, owned=frozenset(), limit=None):
    """(ranked [(unit, rows)], skipped owned units): unclaimed ready rows grouped per unit, by
    bytes in the first wave."""
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
        # Bound the opportunity to one wave, not the whole unit's backlog.
        return (-sum(r["size"] for r in rs[:WAVE_SIZE]),
                rs[0]["unit_left"], u)

    return sorted(units.items(), key=key)[:limit], skipped


def print_by_unit(rows, args, owned=frozenset()) -> int:
    """One line per unit: ready targets, bytes, size range and how many functions the unit has left."""
    order, skipped = rank_units(rows, owned, args.limit or None)
    if args.json:
        print(json.dumps([{"unit": u, "ready": len(rs), "left": rs[0]["unit_left"],
                           "finishable": rs[0]["unit_left"] <= FINISHABLE,
                           "wave_bytes": sum(r["size"] for r in rs[:WAVE_SIZE]), "bytes": sum(r["size"] for r in rs),
                           "min_size": min(r["size"] for r in rs), "max_size": max(r["size"] for r in rs),
                           "source": rs[0]["source"], "names": [r["name"] for r in rs]}
                          for u, rs in order], indent=2))
        return 0
    print(f"{'unit':<34}{'ready':>6}{'left':>6}{'bytes':>8}{'wave':>8}  {'sizes':<14}source")
    for unit, rs in order:
        sizes = sorted(r["size"] for r in rs)
        left = rs[0]["unit_left"]
        mark = "*" if left <= FINISHABLE else " "
        print(f"{unit:<34}{len(rs):>6}{left:>5}{mark}{sum(sizes):>#8x}{sum(r["size"] for r in rs[:WAVE_SIZE]):>#8x}  {f'{sizes[0]:#x}..{sizes[-1]:#x}':<14}"
              f"{rs[0]['source'] or '(new file)'}")
    print(f"\n* = {FINISHABLE} or fewer functions left in the unit (all sizes): code completion only; data and relocation gates still apply.")
    print(f"Ranked by bytes in the first {WAVE_SIZE} ready targets; a yield shortlist, not a time estimate.")
    if skipped:
        print(f"Skipped (open sprint PR): {', '.join(skipped)}")
    if order:
        print(f"\nSprint queue for the first unit:\n  python3 tools/next_target.py --unit {order[0][0]} "
              f"--max-size {args.max_size:#x} --limit 40")
    return 0


def print_plan_items(state: dict, scope: dict, args) -> int:
    """`--by-unit` under the plan: the target stage's items instead of units."""
    import plan
    stage = plan.stage_of(state, scope["stage_id"])
    items = stage["items"] if stage else []
    if args.json:
        print(json.dumps([{k: i[k] for k in ("id", "fn_left", "bytes_left", "closed", "blocked", "units")}
                          | {"claim": state["claims"]["items"].get(i["id"])} for i in items], indent=2))
        return 0
    print(f"Stage {scope['stage_id']} items (--all --by-unit for units, off plan):")
    print(f"  {'item':<18}{'fn left':>8}{'bytes left':>12}  claim")
    for i in items:
        c = state["claims"]["items"].get(i["id"])
        who = ("closed" if i["closed"] else "blocked" if i["blocked"] else
               ("mine" if c["mine"] else "claimed: " + c["by"]) if c else "-")
        print(f"  {i['id']:<18}{i['fn_left']:>8}{i['bytes_left']:>12,}  {who}")
    print(f"\nTarget: {scope['item']}  ->  python3 tools/next_target.py")
    return 0


class TargetError(Exception):
    """A required input (retail DOL, JP anchor table) is missing."""


def collect(max_size: int = 0x200, min_size: int = 0, prefixes=(), jp_paired: bool = False, names=None):
    """(ranked ready rows, count of in-range unmatched functions outside any split unit). NAMES
    (a plan target set) keeps only those functions and includes ones already drafted in src/ or
    recorded verified by hillclimb, marked `defined`/`ledger_verified`."""
    dol = vf.load_dol(vf.DOL_PATH, [])
    if dol is None:
        raise TargetError(f"retail DOL missing at {vf.DOL_PATH.relative_to(ROOT)}; see README.md")
    donor_state = opensaga.snapshot()
    jp_pairs = pc_gc_anchors.load_anchors()
    if jp_pairs is None:
        if jp_paired:
            raise TargetError("build/pc_analysis/gc_pc_anchors.tsv missing; run python3 tools/pc_gc_anchors.py")
        jp_pairs = {}
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
        if not size or size > max_size or size < min_size:
            continue
        if names is not None:
            if name not in names:
                continue
        elif name in done or name in verified:
            continue
        unit = unit_of[name] if name in unit_of else vf.lookup_unit(addr)
        if unit is None:
            skipped_unit += 1
            continue
        if not in_units(unit, list(prefixes)):
            continue
        jp = jp_pairs.get(addr)
        if jp_paired and jp is None:
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
            "defined": name in done, "ledger_verified": name in verified,
            "unit_left": unit_left.get(unit, 0),
            "jp": {"pc_address": f"0x{jp[0]:08X}", "method": jp[1]} if jp else None,
        })
    rows.sort(key=lambda r: (r["callees"], r["size"], not r["opensaga"]["matches"], r["jp"] is None,
                             r["address"]))
    return rows, skipped_unit


def owned_units(rows, remote: bool = True) -> set[str]:
    """Units of ROWS owned by another active sprint branch (see sprint_branches)."""
    branches = sprint_branches(remote=remote)
    units = frozenset(r["unit"] for r in rows)
    return {r["unit"] for r in rows if owned_by_sprint(r["unit"], branches, units)}


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--max-size", type=lambda s: int(s, 0), default=None,
                    help="largest size in bytes (default 0x200 with --all, else no limit)")
    ap.add_argument("--min-size", type=lambda s: int(s, 0), default=0, help="smallest size in bytes (default 0)")
    ap.add_argument("--unit", action="append", default=[], metavar="PREFIX",
                    help="only units starting with PREFIX, e.g. auto/game/ (repeatable)")
    ap.add_argument("--limit", type=int, default=None,
                    help="rows to print (default: every plan target; 15 with --all)")
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--by-unit", action="store_true",
                    help="summarize ready targets per unit to scope a sprint (first-wave bytes descending; units owned by an open t3/sprint-* PR are skipped)")
    ap.add_argument("--no-remote", action="store_true", help="do not ask GitHub for open sprint PRs")
    ap.add_argument("--jp-paired", action="store_true",
                    help="only targets paired with a Japanese PC 1.0 x86 function (gc_pc_anchors.tsv)")
    ap.add_argument("--all", action="store_true", help="rank every ready target, not the plan item's (off plan)")
    ap.add_argument("--off-plan", metavar="REASON", help="as --all, stating why")
    args = ap.parse_args(argv)

    scope = state = None
    if not (args.all or args.off_plan):
        import plan
        try:
            state = plan.status(remote=not args.no_remote, records=[])
        except plan.PlanError as exc:
            print(f"plan unavailable ({exc}); showing the global ranking", file=sys.stderr)
        else:
            scope = plan.plan_targets(state=state)
            if scope["item"] is None:
                print("plan has no recommendable item; showing the global ranking (python3 tools/plan.py)",
                      file=sys.stderr)
                scope = None
    elif args.off_plan:
        print(f"off plan: {args.off_plan}", file=sys.stderr)
    if args.max_size is None:
        args.max_size = 0x200 if scope is None else 0x100000
    if args.limit is None:
        # A plan item is the child's whole work list: never cut it silently (gap 22).
        args.limit = 15 if scope is None else 0
    if scope is not None and args.by_unit:
        return print_plan_items(state, scope, args)
    try:
        rows, skipped_unit = collect(args.max_size, args.min_size, args.unit, args.jp_paired,
                                     names=set(scope["functions"]) if scope else None)
    except TargetError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return vf.EXIT_ERROR
    rows_all = rows
    if args.by_unit:
        return print_by_unit(rows_all, args, owned_units(rows_all, remote=not args.no_remote))
    rows = rows[:args.limit] if args.limit > 0 else rows
    if scope is not None and not args.json:
        whose = "claimed by this checkout" if scope["mine"] else "the plan's next item"
        print(f"Plan item {scope['item']} (stage {scope['stage_id']}, {whose}): {len(scope['functions'])} functions "
              f"left, {len(rows_all)} listed; --all for the global ranking (off plan)\n")

    if args.json:
        print(json.dumps(rows, indent=2))
        return 0
    nw = max([22] + [len(r["name"]) + 2 for r in rows])
    uw = max([18] + [len(r["unit"]) + 2 for r in rows])
    sw = max([28] + [len(r["source"] or "(new file)") + 2 for r in rows])
    print(f"{'name':<{nw}}{'address':<12}{'size':>6} {'callees':>7}  {'unit':<{uw}}{'source':<{sw}}{'saga':<14}"
          f"{'jp':<12}state")
    for r in rows:
        state = ("claimed" if r["claimed"] else "in src" if r.get("defined") else
                 "attempted" if r["attempts"] else "ready")
        jp = r["jp"]["pc_address"] if r["jp"] else "-"
        print(f"{r['name']:<{nw}}{r['address']:<12}{r['size']:>#6x} {r['callees']:>7}  {r['unit']:<{uw}}"
              f"{(r['source'] or '(new file)'):<{sw}}{r['opensaga']['result']:<14}{jp:<12}{state}")
    if rows:
        top = rows[0]
        src = top["source"] or f"src/{top['unit']}.c"
        print(f"\nNext for {top['name']}:")
        print(f"  python3 tools/opensaga.py lookup {top['name']}")
        print(f"  python3 tools/ls1_task_pack.py {top['name']} --out-dir work")
        print(f"  python3 tools/score_fn.py {top['name']} {src}")
        print(f"  python3 tools/hillclimb.py try {top['name']} --unit {top['unit']} --source {src} "
              "--hypothesis '<one falsifiable change>' --donor-note '<GC-grounded donor review or limit>'")
    if len(rows) < len(rows_all):
        print(f"\n{len(rows_all) - len(rows)} more not shown (--limit {args.limit}); --limit 0 lists all.")
    print(f"\n{len(rows_all)} {'plan' if scope else 'ready'} targets lie in split units. {skipped_unit} unmatched "
          f"functions within --max-size are outside any split unit and cannot be verified yet "
          f"(docs/workflow_gaps.md).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
