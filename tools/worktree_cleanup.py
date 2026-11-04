#!/usr/bin/env python3
"""Archive local research from merged worktrees, then remove them and their merged branches.

    python3 tools/worktree_cleanup.py                 # dry run: what would happen
    python3 tools/worktree_cleanup.py --apply         # archive, remove worktrees, delete local branches
    python3 tools/worktree_cleanup.py --apply --remote  # also delete merged branches on origin
    python3 tools/worktree_cleanup.py --index-only    # rebuild the parked index

A worktree is removed only when its HEAD is an ancestor of origin/main, it has no tracked or
untracked changes, it has been idle for --min-idle hours, it is not the main checkout or the current worktree and it is not listed in
--keep. Before removal its ignored `work/` directory (status notes, candidates, permuter outputs)
is copied to the main checkout's `work/archive/worktrees/NAME/`, and the copy is checked by file
count. `work/archive/worktrees/INDEX.md` lists every parked or plateaued function in the
archive with its state on main. Branches whose tip is an ancestor of origin/main are deleted locally and
`git push origin --delete` for merged remote branches that no worktree uses.
"""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

PROTECTED = {"main", "HEAD"}


def git(*args: str, cwd: Path | None = None, check: bool = True) -> str:
    result = subprocess.run(["git", *args], cwd=cwd, capture_output=True, text=True)
    if check and result.returncode:
        raise RuntimeError(f"git {' '.join(args)}: {result.stderr.strip()}")
    return result.stdout.strip()


def is_merged(rev: str, cwd: Path) -> bool:
    return subprocess.run(["git", "merge-base", "--is-ancestor", rev, "origin/main"],
                          cwd=cwd, capture_output=True).returncode == 0


def worktrees(cwd: Path) -> list[dict]:
    entries, current = [], {}
    for line in git("worktree", "list", "--porcelain", cwd=cwd).splitlines() + [""]:
        if not line:
            if current:
                entries.append(current)
            current = {}
            continue
        key, _, value = line.partition(" ")
        current[key] = value or True
    for entry in entries:
        entry["path"] = Path(entry["worktree"])
        entry["branch"] = str(entry.get("branch", "")).removeprefix("refs/heads/")
    return entries


def idle_hours(path: Path) -> float:
    """Hours since the worktree's HEAD, index or work/ last changed (fresh checkouts look merged)."""
    gitdir = Path(git("rev-parse", "--path-format=absolute", "--git-dir", cwd=path))
    stamps = [item.stat().st_mtime for item in (gitdir / "HEAD", gitdir / "index",
                                                 gitdir / "logs" / "HEAD", path / "work")
              if item.exists()]
    return (time.time() - max(stamps, default=0)) / 3600


def is_parked_status(text: str) -> bool:
    """Generated task packs start as TODO; anything else records an attempt worth keeping."""
    return bool(text.strip()) and not text.startswith("# Status: TODO")


def status_summary(text: str, width: int = 160) -> str:
    lines = [line.strip("# ").strip() for line in text.splitlines() if line.strip()]
    return " ".join(lines[:2])[:width].replace("|", "/")


def permuter_best(permute_dir: Path) -> str:
    outputs = []
    for output in permute_dir.glob("output-*"):
        match = re.match(r"output-(\d+)", output.name)
        if match:
            outputs.append(int(match.group(1)))
    if not outputs:
        return ""
    base = "?"
    for log in permute_dir.glob("*.log"):
        found = re.search(r"base score = (\d+)", log.read_text(errors="replace"))
        if found:
            base = found.group(1)
            break
    return f"{base}→{min(outputs)}"


def report_percentages(report: Path | None) -> dict[str, float]:
    if not report or not report.exists():
        return {}
    data = json.loads(report.read_text())
    return {fn["name"]: fn.get("fuzzy_match_percent", 0)
            for unit in data.get("units", []) for fn in unit.get("functions", [])}


def parked_statuses(archive: Path, live_work: Path | None):
    """Yield (source label, its work dir, status.md) for the archive and, optionally, a live work/."""
    for status in sorted(archive.glob("*/**/status.md")):
        source = status.relative_to(archive).parts[0]
        yield source, archive / source, status
    if live_work and live_work.is_dir():
        for status in sorted(live_work.glob("**/status.md")):
            if archive not in status.parents:
                yield "main checkout", live_work, status


def build_index(archive: Path, report: Path | None, live_work: Path | None = None) -> tuple[int, int]:
    percent = report_percentages(report)
    rows = []
    for source, work, status in parked_statuses(archive, live_work):
        text = status.read_text(errors="replace")
        if not is_parked_status(text):
            continue
        function = status.parent.name
        permute = work / "permute" / function
        best = permuter_best(permute) if permute.is_dir() else ""
        value = percent.get(function)
        state = "?" if value is None else "matched" if value >= 100 else f"{value:.1f}%"
        rows.append((state == "matched", function, source, state, best, status_summary(text),
                     status.relative_to(archive.parent.parent)))
    rows.sort(key=lambda row: (row[0], row[1]))
    lines = ["# Parked index", "",
             f"Archived worktree research. Main state from `{report}`; permuter is base→best score "
             "(0 = match).", "",
             "| function | from | main | permuter | status | file (under work/) |",
             "|---|---|---|---|---|---|"]
    lines += [f"| {f} | {src} | {st} | {best} | {summary} | {rel} |"
              for _, f, src, st, best, summary, rel in rows]
    (archive / "INDEX.md").write_text("\n".join(lines) + "\n")
    return len(rows), sum(not row[0] for row in rows)


def file_count(path: Path) -> int:
    return sum(1 for item in path.rglob("*") if item.is_file() or item.is_symlink())


def archive_work(source: Path, destination: Path) -> None:
    if destination.exists():
        shutil.copytree(source, destination, symlinks=True, dirs_exist_ok=True)
    else:
        shutil.copytree(source, destination, symlinks=True)
    if file_count(destination) < file_count(source):
        raise RuntimeError(f"archive of {source} is incomplete")


def newest_report(paths: list[Path]) -> Path | None:
    reports = [path / "build/GL5E4F/report.json" for path in paths]
    reports = [report for report in reports if report.exists()]
    return max(reports, key=lambda report: report.stat().st_mtime, default=None)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--apply", action="store_true", help="act; the default is a dry run")
    parser.add_argument("--remote", action="store_true", help="also delete merged origin branches")
    parser.add_argument("--keep", action="append", default=[], metavar="PATH_OR_BRANCH",
                        help="never remove this worktree or branch (repeatable)")
    parser.add_argument("--min-idle", type=float, default=6, metavar="HOURS",
                        help="skip worktrees active more recently than this (default 6)")
    parser.add_argument("--index-only", action="store_true", help="only rebuild INDEX.md")
    parser.add_argument("--report", type=Path, help="report.json for main state (default: newest)")
    args = parser.parse_args(argv)

    here = Path.cwd()
    common = Path(git("rev-parse", "--path-format=absolute", "--git-common-dir", cwd=here))
    main_checkout = common.parent
    current = Path(git("rev-parse", "--show-toplevel", cwd=here)).resolve()
    archive = main_checkout / "work" / "archive" / "worktrees"
    trees = worktrees(main_checkout)
    report = args.report or newest_report([tree["path"] for tree in trees])

    if args.index_only:
        total, open_ = build_index(archive, report, main_checkout / "work") if archive.exists() else (0, 0)
        print(f"index: {total} parked rows, {open_} unmatched on main")
        return 0

    git("fetch", "--quiet", "--prune", "origin", cwd=main_checkout)
    keep = set(args.keep)
    used_branches = {tree["branch"] for tree in trees if tree["branch"]}
    for tree in trees:
        path = tree["path"].resolve()
        branch = tree["branch"]
        label = f"{path} [{branch or 'detached'}]"
        if path in (main_checkout.resolve(), current) or str(path) in keep or branch in keep:
            continue
        if tree.get("prunable"):
            print(f"prune   {label}")
            if args.apply:
                git("worktree", "prune", cwd=main_checkout)
            continue
        if not is_merged(tree["HEAD"], main_checkout):
            print(f"keep    {label}: not merged")
            continue
        if git("status", "--porcelain", cwd=path):
            print(f"keep    {label}: uncommitted changes")
            continue
        idle = idle_hours(path)
        if idle < args.min_idle:
            print(f"keep    {label}: active {idle:.1f} h ago")
            continue
        work = path / "work"
        print(f"remove  {label}" + (f" (archive work/ → {archive / path.name})" if work.is_dir() else ""))
        if not args.apply:
            continue
        if work.is_dir():
            archive_work(work, archive / path.name)
        git("worktree", "remove", str(path), cwd=main_checkout)
        if branch:
            used_branches.discard(branch)

    for branch in git("for-each-ref", "--format=%(refname:short)", "refs/heads", cwd=main_checkout).split():
        if branch in PROTECTED or branch in keep or branch in used_branches:
            continue
        if is_merged(f"refs/heads/{branch}", main_checkout):
            print(f"delete  local {branch}")
            if args.apply:
                # -D: -d compares with the local main, which may be stale; the ancestor check
                # against origin/main above is the merge test.
                git("branch", "-D", branch, cwd=main_checkout)

    if args.remote:
        for ref in git("for-each-ref", "--format=%(refname:short)", "refs/remotes/origin",
                       cwd=main_checkout).split():
            branch = ref.removeprefix("origin/")
            if branch in PROTECTED or branch == "origin" or branch in keep or branch in used_branches:
                continue
            if is_merged(ref, main_checkout):
                print(f"delete  origin {branch}")
                if args.apply:
                    git("push", "--quiet", "origin", "--delete", branch, cwd=main_checkout)

    if args.apply and archive.exists():
        total, open_ = build_index(archive, report, main_checkout / "work")
        print(f"index: {archive / 'INDEX.md'}: {total} parked rows, {open_} unmatched on main")
    elif not args.apply:
        print("dry run; pass --apply to act")
    return 0


if __name__ == "__main__":
    sys.exit(main())
