#!/usr/bin/env python3
"""The matching plan (config/plan.toml) joined with the objdiff report: progress, claims, next item.

    python3 tools/plan.py                 # stages, the current stage's items, claims, warnings, yield
    python3 tools/plan.py --json
    python3 tools/plan.py --next          # the next item id, its units and its functions left
    python3 tools/plan.py --item hubs     # one item's functions left
    python3 tools/plan.py --release hubs  # drop this checkout's claim record for an item
    python3 tools/plan.py --no-remote     # skip the GitHub open-PR query

A function is matched when its `fuzzy_match_percent` in build/GL5E4F/report.json is 100; an
item is closed when each of its functions is matched or listed in `[[parked]]`, a stage when
all its items are closed, and the current stage is the first one not closed and not marked
`alongside = true`. An alongside stage (stage 0, the CodeWarrior SDK residue) does not wait on
the call graph: its open items are worked beside the current stage's and come first in order. The `data` item
closes when the report's data bytes are all matched.

Claims: `claim(ID)` (run by `kickoff.py --start --item ID`) writes
`<git common dir>/lsw1-plan-claims/ID.json`, which every local worktree sees. A local or origin
branch or an open PR named `t3/plan-ID` also claims the item, as does a legacy
`t3/sprint-<unit>` branch for an item listing that unit. A record is stale (ignored) when its
worktree is gone, its branch is gone, or its branch gained commits since the claim and they
are merged into origin/main. This checkout's own claim is "mine", not taken.

`active_items()` are the open, unblocked, unclaimed items of the open alongside stages before
the current stage, then of the current stage; when none is free, those of the next open stage,
flagged as look-ahead. `next_item()` is the first of them. `kickoff.py --start --item` accepts
any active item.
`plan_targets()` is what other tools default to: this checkout's claimed item, else the next item.
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))

PLAN = ROOT / "config" / "plan.toml"
REPORT = ROOT / "build" / "GL5E4F" / "report.json"
PLAN_PREFIX = "t3/plan-"
CLAIMS = "lsw1-plan-claims"


class PlanError(Exception):
    """The plan or the report is missing or unreadable."""


def git(*args: str, cwd: Path | None = None) -> str | None:
    try:
        out = subprocess.run(["git", *args], cwd=cwd or ROOT, capture_output=True, text=True, timeout=15)
    except (OSError, subprocess.TimeoutExpired):
        return None
    return out.stdout.strip() if out.returncode == 0 else None


# --- inputs ---------------------------------------------------------------------------------

def load_plan(path: Path | None = None) -> dict:
    path = path or PLAN
    try:
        return tomllib.loads(path.read_text())
    except (OSError, tomllib.TOMLDecodeError) as exc:
        raise PlanError(f"cannot read {path}: {exc}") from exc


def load_report(path: Path | None = None) -> dict:
    path = path or REPORT
    try:
        return json.loads(path.read_text())
    except (OSError, ValueError) as exc:
        raise PlanError(f"cannot read {path} ({exc}); run bash build.sh") from exc


def index_report(report: dict) -> dict:
    """{"units": {unit: [fn]}, "by_name": {name: [fn]}, "data": (matched, total)}; fn has name,
    unit, size, matched. Unit names lose the report's `main/` prefix."""
    units: dict[str, list[dict]] = {}
    by_name: dict[str, list[dict]] = {}
    for u in report.get("units", []):
        name = u["name"].removeprefix("main/")
        fns = units.setdefault(name, [])
        for f in u.get("functions", []):
            fn = {"name": f["name"], "unit": name, "size": int(f.get("size") or 0),
                  "matched": float(f.get("fuzzy_match_percent") or 0) >= 100.0}
            fns.append(fn)
            by_name.setdefault(f["name"], []).append(fn)
    m = report.get("measures", {})
    return {"units": units, "by_name": by_name,
            "data": (int(m.get("matched_data") or 0), int(m.get("total_data") or 0))}


def parked_names(plan: dict) -> dict[str, str]:
    return {p["function"]: p.get("reason", "") for p in plan.get("parked", []) if p.get("function")}


# --- evaluation -----------------------------------------------------------------------------

def item_functions(item: dict, idx: dict) -> tuple[list[dict], list[str]]:
    """(the item's report functions, names of its units/functions the report lacks)."""
    fns, missing = [], []
    for unit in item.get("units", []):
        if unit in idx["units"]:
            fns.extend(idx["units"][unit])
        else:
            missing.append(f"unit {unit}")
    for name in item.get("functions", []):
        if name in idx["by_name"]:
            fns.extend(idx["by_name"][name])
        else:
            missing.append(f"function {name}")
    return fns, missing


def evaluate(plan: dict, report: dict) -> dict:
    """Per-item and per-stage figures, the current stage and the plan-revision warnings that
    need only the plan and the report (claims and bench runs add more in status())."""
    idx = index_report(report)
    parked = parked_names(plan)
    stages, covered, warnings = [], set(), []
    for stage in plan.get("stage", []):
        items = []
        for item in stage.get("item", []):
            rec = {"id": item["id"], "stage": stage["id"], "units": list(item.get("units", [])),
                   "blocked": item.get("blocked"), "note": item.get("note"), "data": bool(item.get("data")),
                   "whole_units": bool(item.get("units"))}
            if rec["data"]:
                matched, total = idx["data"]
                rec.update(functions=[], fn_left=0, bytes_left=total - matched, parked=[],
                           closed=total > 0 and matched >= total)
            else:
                fns, missing = item_functions(item, idx)
                covered.update((f["unit"], f["name"]) for f in fns)
                left = [f for f in fns if not f["matched"]]
                todo = [f for f in left if f["name"] not in parked]
                rec.update(functions=[f["name"] for f in todo], fn_left=len(todo),
                           bytes_left=sum(f["size"] for f in todo),
                           parked=sorted({f["name"] for f in left if f["name"] in parked}),
                           units=rec["units"] or sorted({f["unit"] for f in fns}), closed=not todo)
                for m in missing:
                    warnings.append(f"item {item['id']} names {m}, which the report lacks")
            items.append(rec)
        stages.append({"id": stage["id"], "title": stage.get("title", ""), "items": items,
                       "alongside": bool(stage.get("alongside")),
                       "closed": all(i["closed"] for i in items),
                       "fn_left": sum(i["fn_left"] for i in items),
                       "bytes_left": sum(i["bytes_left"] for i in items),
                       "data": any(i["data"] for i in items)})
    uncovered: dict[str, int] = {}
    for unit, fns in idx["units"].items():
        for f in fns:
            if not f["matched"] and (unit, f["name"]) not in covered:
                uncovered[unit] = uncovered.get(unit, 0) + 1
    for unit, n in sorted(uncovered.items()):
        warnings.append(f"{n} unmatched function(s) of {unit} are in no item")
    current = (next((s for s in stages if not s["closed"] and not s["alongside"]), None)
               or next((s for s in stages if not s["closed"]), None))
    return {"revision": plan.get("revision"), "stages": stages, "current": current["id"] if current else None,
            "warnings": warnings, "parked": parked}


def find_item(state: dict, item_id: str) -> dict | None:
    return next((i for s in state["stages"] for i in s["items"] if i["id"] == item_id), None)


def stage_of(state: dict, stage_id: str | None) -> dict | None:
    return next((s for s in state["stages"] if s["id"] == stage_id), None)


# --- claims ---------------------------------------------------------------------------------

def claims_dir(root: Path | None = None) -> Path:
    common = git("rev-parse", "--path-format=absolute", "--git-common-dir", cwd=root or ROOT)
    return Path(common or (root or ROOT) / ".git") / CLAIMS


def current_branch(root: Path | None = None) -> str:
    return git("branch", "--show-current", cwd=root or ROOT) or ""


def claim(item_id: str, root: Path | None = None, now: float | None = None) -> Path:
    """Record this checkout's claim on ITEM_ID; returns the record path."""
    root = (root or ROOT).resolve()
    path = claims_dir(root) / f"{item_id}.json"
    path.parent.mkdir(parents=True, exist_ok=True)
    record = {"item": item_id, "branch": current_branch(root), "worktree": str(root),
              "head": git("rev-parse", "HEAD", cwd=root), "time": round(now or time.time())}
    path.write_text(json.dumps(record, indent=2) + "\n")
    return path


def release(item_id: str, root: Path | None = None) -> bool:
    path = claims_dir(root) / f"{item_id}.json"
    if path.is_file():
        path.unlink()
        return True
    return False


def record_stale(rec: dict, root: Path | None = None) -> str | None:
    """Why a claim record no longer holds, or None while it does."""
    tree = rec.get("worktree")
    if not tree or not Path(tree).exists():
        return f"worktree {tree} is gone"
    branch = rec.get("branch")
    if not branch:
        return None
    tip = git("rev-parse", "--verify", "-q", f"refs/heads/{branch}", cwd=root)
    if tip is None:
        return f"branch {branch} is gone"
    # A fresh branch is an ancestor of origin/main before its first commit; only commits made
    # after the claim and then merged end it.
    if tip != rec.get("head") and git("merge-base", "--is-ancestor", tip, "origin/main", cwd=root) is not None:
        return f"branch {branch} is merged into origin/main"
    return None


def read_records(root: Path | None = None) -> list[dict]:
    out = []
    d = claims_dir(root)
    for path in sorted(d.glob("*.json")) if d.is_dir() else []:
        try:
            rec = json.loads(path.read_text())
        except (OSError, ValueError):
            continue
        rec.setdefault("item", path.stem)
        rec["path"] = str(path)
        rec["stale"] = record_stale(rec, root)
        out.append(rec)
    return out


def plan_branches(remote: bool = True) -> dict[str, str]:
    """{item: source} for unmerged local/origin branches and open PRs named t3/plan-ITEM,
    via next_target's ownership query; the current branch is included (it is "mine")."""
    import next_target as nt
    return {name: "branch or PR" for name in nt.active_branches(PLAN_PREFIX, remote=remote, include_current=True)}


def claims(state: dict, remote: bool = True, root: Path | None = None, records: list[dict] | None = None,
           branches: dict[str, str] | None = None, sprints: set[str] | None = None) -> dict:
    """{"items": {item: {"by", "mine"}}, "stale": [records]} from claim records, t3/plan-* branches
    and PRs, and legacy t3/sprint-* branches owning one of an item's units."""
    root = (root or ROOT).resolve()
    me, branch = str(root), current_branch(root)
    records = read_records(root) if records is None else records
    branches = plan_branches(remote) if branches is None else branches
    if sprints is None:
        import next_target as nt
        sprints = nt.sprint_branches(remote=remote)
    out: dict[str, dict] = {}
    stale = []

    def add(item, by, mine):
        cur = out.get(item)
        if cur is None or (mine and not cur["mine"]):
            out[item] = {"by": by, "mine": mine}
    for rec in records:
        if rec.get("stale"):
            stale.append(rec)
            continue
        mine = rec.get("worktree") == me
        add(rec["item"], f"{rec.get('branch') or '?'} ({rec.get('worktree')})", mine)
    if branch.startswith(PLAN_PREFIX):
        add(branch[len(PLAN_PREFIX):], f"current branch {branch}", True)
    for item, src in branches.items():
        add(item, f"{PLAN_PREFIX}{item} {src}", branch == PLAN_PREFIX + item)
    if sprints:
        import next_target as nt
        whole = [i for s in state["stages"] for i in s["items"] if i["whole_units"]]
        units = frozenset(u for i in whole for u in i["units"])
        for i in whole:
            for u in i["units"]:
                if nt.owned_by_sprint(u, sprints, units):
                    add(i["id"], f"t3/sprint-* owns {u}", False)
    return {"items": out, "stale": stale}


def mine(cl: dict) -> str | None:
    return next((item for item, c in cl["items"].items() if c["mine"]), None)


def taken(cl: dict, item_id: str) -> bool:
    c = cl["items"].get(item_id)
    return bool(c and not c["mine"])


def open_items(stage: dict) -> list[dict]:
    return [i for i in stage["items"] if not i["closed"] and not i["blocked"]]


def next_item(state: dict, cl: dict) -> dict | None:
    """{"item", "stage", "lookahead"} or None: see the module docstring. This checkout's own
    claimed item comes first while it is open."""
    own = mine(cl)
    if own:
        item = find_item(state, own)
        if item and not item["closed"]:
            return {"item": item, "stage": item["stage"], "lookahead": False, "mine": True}
    active = active_items(state, cl)
    if not active:
        return None
    item, lookahead = active[0]
    return {"item": item, "stage": item["stage"], "lookahead": lookahead, "mine": False}


def active_items(state: dict, cl: dict) -> list[tuple[dict, bool]]:
    """[(item, lookahead)]: the free items of the open alongside stages up to and including the
    current stage; when none is free, those of the next open non-alongside stage (look-ahead)."""
    pending = [s for s in state["stages"] if not s["closed"]]
    free = lambda st: [i for i in open_items(st) if not taken(cl, i["id"])]
    now, rest, seen_current = [], [], False
    for stage in pending:
        if not seen_current:
            if stage["alongside"] or stage["id"] == state["current"]:
                now += free(stage)
            seen_current = stage["id"] == state["current"]
        elif not stage["alongside"]:
            rest = free(stage)
            break
    return [(i, False) for i in now] or [(i, True) for i in rest]


# --- bench yield ----------------------------------------------------------------------------

def bench_records() -> list[dict]:
    try:
        import sprint_bench as sb
        return sb.compare()
    except Exception:
        return []


def stage_yield(state: dict, records: list[dict]) -> dict[str, dict]:
    """{stage: {runs, verified, verified_bytes, active_hours, fn_per_hour, bytes_per_hour}} over
    finished bench runs that carry an item id."""
    out: dict[str, dict] = {}
    for rec in records:
        item = find_item(state, rec.get("item") or "")
        if not item or not rec.get("active_seconds") or rec.get("new_verified") is None:
            continue
        y = out.setdefault(item["stage"], {"runs": 0, "verified": 0, "verified_bytes": 0, "active_hours": 0.0})
        y["runs"] += 1
        y["verified"] += rec["new_verified"] or 0
        y["verified_bytes"] += rec.get("new_verified_bytes") or 0
        y["active_hours"] += rec["active_seconds"] / 3600
    for y in out.values():
        y["fn_per_hour"] = y["verified"] / y["active_hours"]
        y["bytes_per_hour"] = y["verified_bytes"] / y["active_hours"]
    return out


# --- the joined view ------------------------------------------------------------------------

def status(remote: bool = True, plan: dict | None = None, report: dict | None = None,
           cl: dict | None = None, records: list[dict] | None = None) -> dict:
    state = evaluate(plan if plan is not None else load_plan(), report if report is not None else load_report())
    cl = claims(state, remote=remote) if cl is None else cl
    records = bench_records() if records is None else records
    state["claims"] = cl
    state["next"] = next_item(state, cl)
    state["mine"] = mine(cl)
    cur = stage_of(state, state["current"])
    if cur and not [i for i in open_items(cur) if not taken(cl, i["id"])]:
        state["warnings"].append(f"stage {cur['id']} is not closed but has no recommendable item "
                                 "(what is left is claimed, parked or blocked)")
    for rec in records:
        if rec.get("item") and rec.get("new_verified") == 0:
            state["warnings"].append(f"bench run {rec.get('run_id')} on item {rec['item']} added no new strict matches")
    state["yield"] = stage_yield(state, records)
    return state


def target_item(state: dict) -> dict | None:
    """This checkout's claimed item, else the next item."""
    if state.get("mine"):
        item = find_item(state, state["mine"])
        if item:
            return item
    return state["next"]["item"] if state.get("next") else None


def functions_for(item_id: str, state: dict | None = None) -> list[str]:
    """ITEM_ID's unmatched, unparked function names."""
    state = state or evaluate(load_plan(), load_report())
    item = find_item(state, item_id)
    if item is None:
        raise PlanError(f"no item {item_id} in {PLAN.relative_to(ROOT)}")
    return list(item["functions"])


def plan_targets(remote: bool = False, state: dict | None = None) -> dict:
    """{"item": id or None, "mine": bool, "functions": [...], "stage": [names of the current
    stage's open items]}: the default target set for other tools. Never raises; an unreadable
    plan or report gives an empty set with an "error"."""
    try:
        state = state or status(remote=remote, records=[])
    except PlanError as exc:
        return {"item": None, "mine": False, "functions": [], "stage": [], "error": str(exc)}
    item = target_item(state)
    cur = stage_of(state, item["stage"] if item else state["current"])
    stage = [n for i in (open_items(cur) if cur else []) for n in i["functions"]]
    return {"item": item["id"] if item else None, "stage_id": cur["id"] if cur else None,
            "mine": bool(item and item["id"] == state.get("mine")),
            "functions": list(item["functions"]) if item else [], "stage": stage}


# --- CLI ------------------------------------------------------------------------------------

def warning_lines(state: dict) -> list[str]:
    return [f"plan needs revision: {w}" for w in state["warnings"]]


def describe_next(state: dict) -> str:
    nxt = state["next"]
    if not nxt:
        return "no recommendable item"
    i = nxt["item"]
    tag = " (yours)" if nxt.get("mine") else (" (look-ahead: every open item of the current stage is claimed)"
                                               if nxt["lookahead"] else "")
    left = (f"{i['bytes_left']:,} data bytes left" if i["data"]
            else f"{i['fn_left']} functions, {i['bytes_left']:,} bytes left")
    return f"stage {nxt['stage']}, item {i['id']}{tag}: {left}"


def print_status(state: dict) -> None:
    print(f"Plan revision {state['revision']} (config/plan.toml); current stage: {state['current'] or 'none'}")
    print(f"\n{'stage':<20}{'items':>6}{'open':>6}{'fn left':>9}{'bytes left':>12}  state")
    for s in state["stages"]:
        n_open = sum(1 for i in s["items"] if not i["closed"])
        mark = "closed" if s["closed"] else ("current" if s["id"] == state["current"]
                                             else "alongside" if s["alongside"] else "")
        mark += " (data bytes)" if s["data"] else ""
        print(f"{s['id']:<20}{len(s['items']):>6}{n_open:>6}{s['fn_left']:>9}{s['bytes_left']:>12,}  {mark}")
    shown = [s for s in state["stages"] if not s["closed"] and s["alongside"]
             and state["stages"].index(s) < state["stages"].index(stage_of(state, state["current"]))]
    for cur in shown + [s for s in [stage_of(state, state["current"])] if s]:
        print(f"\nStage {cur['id']} ({cur['title']}{', alongside' if cur['alongside'] else ''}):")
        print(f"  {'item':<18}{'fn left':>8}{'bytes left':>12}  claim")
        for i in cur["items"]:
            c = state["claims"]["items"].get(i["id"])
            if i["closed"]:
                who = "closed"
            elif i["blocked"]:
                who = f"blocked: {i['blocked']}"
            elif c:
                who = ("mine: " if c["mine"] else "claimed: ") + c["by"]
            else:
                who = "-"
            extra = f" ({len(i['parked'])} parked)" if i["parked"] else ""
            print(f"  {i['id']:<18}{i['fn_left']:>8}{i['bytes_left']:>12,}  {who}{extra}")
    for rec in state["claims"]["stale"]:
        print(f"stale claim ignored: {rec['item']} ({rec['stale']}; {rec['path']})")
    print(f"\nNext: {describe_next(state)}")
    if state["next"]:
        print(f"  python3 tools/kickoff.py --start --item {state['next']['item']['id']}")
    for line in warning_lines(state):
        print(line)
    print("\nYield per stage (finished item bench runs):")
    if not state["yield"]:
        print("  no measurement yet (kickoff.py --start --item ID opens a bench run)")
    for sid, y in state["yield"].items():
        print(f"  {sid}: {y['runs']} run(s), {y['verified']} fn / {y['verified_bytes']:,} B in "
              f"{y['active_hours']:.1f} active h = {y['fn_per_hour']:.1f} fn/h, {y['bytes_per_hour']:,.0f} B/h")


def jsonable(state: dict) -> dict:
    out = dict(state)
    out["claims"] = {"items": state["claims"]["items"], "stale": state["claims"]["stale"]}
    return out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--next", action="store_true", help="print the next item and its targets")
    ap.add_argument("--item", help="print one item's targets")
    ap.add_argument("--release", metavar="ID", help="remove the claim record for ID")
    ap.add_argument("--no-remote", action="store_true", help="do not ask GitHub for open t3/plan-* PRs")
    args = ap.parse_args(argv)
    if args.release:
        print(f"released {args.release}" if release(args.release) else f"no claim record for {args.release}")
        return 0
    try:
        state = status(remote=not args.no_remote)
    except PlanError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2
    if args.next or args.item:
        item = state["next"]["item"] if args.next and state["next"] else find_item(state, args.item or "")
        if item is None:
            print("error: " + (f"no item {args.item}" if args.item else "no recommendable item"), file=sys.stderr)
            return 2
        if args.json:
            print(json.dumps(item, indent=2))
            return 0
        print(describe_next(state) if args.next else f"item {item['id']} (stage {item['stage']})")
        if item["units"]:
            print("units: " + " ".join(item["units"]))
        if item["blocked"]:
            print(f"blocked: {item['blocked']}")
        print(f"functions left ({item['fn_left']}): " + " ".join(item["functions"]))
        if item["parked"]:
            print("parked: " + " ".join(item["parked"]))
        return 0
    if args.json:
        print(json.dumps(jsonable(state), indent=2, default=str))
        return 0
    print_status(state)
    return 0


if __name__ == "__main__":
    sys.exit(main())
