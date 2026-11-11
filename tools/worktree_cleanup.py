#!/usr/bin/env python3
"""Archive local research from merged worktrees, then remove them and their merged branches.

    python3 tools/worktree_cleanup.py                 # dry run: what would happen
    python3 tools/worktree_cleanup.py --apply         # archive, remove worktrees, delete local branches
    python3 tools/worktree_cleanup.py --apply --remote  # also delete merged branches on origin
    python3 tools/worktree_cleanup.py --index-only    # rebuild the parked index
    python3 tools/worktree_cleanup.py --audit         # report every worktree's unfinished work

A worktree is removed only when its HEAD is an ancestor of origin/main, it has no tracked or
untracked changes, it has been idle for --min-idle hours, it is not the main checkout or the current worktree and it is not listed in
--keep. Before removal its ignored `work/` directory (status notes, candidates, permuter outputs)
is copied to the main checkout's `work/archive/worktrees/NAME/`, and the copy is checked by file
count. `work/archive/worktrees/INDEX.md` lists every parked or plateaued function in the
archive with its state on main; `--index-only` and `--audit` also index notes still in live
worktrees. `--audit` is read-only apart from INDEX.md: it lists each worktree's unmerged, unpushed
and uncommitted work and does not fetch unless given `--fetch`. Branches whose tip is an ancestor of origin/main are deleted locally and
`git push origin --delete` for merged remote branches that no worktree uses.
"""

from __future__ import annotations

import argparse
import json
import os
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


def live_statuses(work: Path, archive: Path) -> list[Path]:
    """status.md files in a live work/, skipping the archive and permuter output trees."""
    found, archive = [], archive.resolve()
    for root, dirs, files in os.walk(work):
        dirs[:] = sorted(name for name in dirs if not name.startswith("output-") and not (
            name == archive.name and Path(root, name).resolve() == archive))
        if "status.md" in files:
            found.append(Path(root, "status.md"))
    return found


def parked_statuses(archive: Path, live: list[tuple[str, Path]] = ()):
    """Yield (source label, its work dir, status.md) for the archive and each live (label, work/)."""
    for status in sorted(archive.glob("*/**/status.md")):
        source = status.relative_to(archive).parts[0]
        yield source, archive / source, status
    for label, work in live:
        if work.is_dir():
            for status in live_statuses(work, archive):
                yield label, work, status


def build_index(archive: Path, report: Path | None, live: list[tuple[str, Path]] = ()) -> tuple[int, int]:
    """Write archive/INDEX.md. Rows from the main checkout's work/ (archive.parent.parent) use paths
    relative to it; rows from other live worktrees use absolute paths."""
    percent = report_percentages(report)
    main_work = archive.parent.parent
    rows = []
    for source, work, status in parked_statuses(archive, live):
        text = status.read_text(errors="replace")
        if not is_parked_status(text):
            continue
        function = status.parent.name
        permute = work / "permute" / function
        best = permuter_best(permute) if permute.is_dir() else ""
        value = percent.get(function)
        state = "?" if value is None else "matched" if value >= 100 else f"{value:.1f}%"
        path = status.relative_to(main_work) if main_work in status.parents else status
        rows.append((state == "matched", function, source, state, best, status_summary(text), path))
    rows.sort(key=lambda row: (row[0], row[1], row[2]))
    lines = ["# Parked index", "",
             f"Archived and live worktree research (`live:NAME` rows are still in that worktree). "
             f"Main state from `{report}`; permuter is base→best score (0 = match).", "",
             "| function | from | main | permuter | status | file (under main work/, or absolute) |",
             "|---|---|---|---|---|---|"]
    lines += [f"| {f} | {src} | {st} | {best} | {summary} | {rel} |"
              for _, f, src, st, best, summary, rel in rows]
    archive.mkdir(parents=True, exist_ok=True)
    (archive / "INDEX.md").write_text("\n".join(lines) + "\n")
    return len(rows), sum(not row[0] for row in rows)


def live_works(trees: list[dict], main_checkout: Path) -> list[tuple[str, Path]]:
    """(label, work/) for every worktree whose directory exists, the main checkout first."""
    works = []
    for tree in trees:
        path = tree["path"]
        if not path.is_dir():
            continue
        if path.resolve() == main_checkout.resolve():
            works.insert(0, ("main checkout", path / "work"))
        else:
            works.append((f"live:{path.name}", path / "work"))
    return works


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


def outstanding(rev: str, cwd: Path) -> int:
    """Commits on rev not yet on origin/main. Ancestry, then patch-id (`git cherry`, which covers
    cherry-picks and one-commit squashes), then content: if every file rev changed since the
    merge base is identical on origin/main, a multi-commit squash merge landed it."""
    if is_merged(rev, cwd):
        return 0
    ahead = sum(line.startswith("+") for line in git("cherry", "origin/main", rev, cwd=cwd).splitlines())
    base = git("merge-base", "origin/main", rev, cwd=cwd, check=False)
    if ahead and base:
        changed = [name for name in subprocess.run(
            ["git", "diff", "-z", "--name-only", "--no-renames", base, rev],
            cwd=cwd, capture_output=True, text=True).stdout.split("\0") if name]
        same = subprocess.run(["git", "diff", "--quiet", "origin/main", rev, "--", *changed],
                              cwd=cwd, capture_output=True).returncode == 0
        if changed and same:
            return 0
    return ahead


def upstreams(cwd: Path) -> dict[str, tuple[str, str]]:
    """branch → (upstream, track), where track is e.g. "[gone]" or "[ahead 1]"."""
    result = {}
    for line in git("for-each-ref", "--format=%(refname:short)\t%(upstream:short)\t%(upstream:track)",
                    "refs/heads", cwd=cwd).splitlines():
        branch, upstream, track = (line.split("\t") + ["", ""])[:3]
        result[branch] = (upstream, track)
    return result


def push_state(branch: str, rev: str, ups: dict, cwd: Path) -> tuple[str, int | None]:
    """(label, commits not on the upstream). An unset upstream falls back to origin/BRANCH."""
    if not branch:
        return "detached", None
    upstream, track = ups.get(branch, ("", ""))
    if "gone" in track:
        return "gone", None
    if not upstream and git("rev-parse", "--verify", "--quiet", f"refs/remotes/origin/{branch}",
                            cwd=cwd, check=False):
        upstream = f"origin/{branch}"
    if not upstream:
        return "no upstream", None
    count = int(git("rev-list", "--count", f"{upstream}..{rev}", cwd=cwd))
    return str(count), count


def audit(trees: list[dict], main_checkout: Path, current: Path, archive: Path) -> list[str]:
    """Markdown report of every worktree's unfinished work, then what needs attention."""
    ups = upstreams(main_checkout)
    lines = ["| worktree | branch | ahead/behind main | dirty | unpushed | idle h | parked | note |",
             "|---|---|---|---|---|---|---|---|"]
    attention = []
    for tree in trees:
        path, branch, rev = tree["path"], tree["branch"], tree.get("HEAD", "")
        notes = [note for note, flag in (("main", path.resolve() == main_checkout.resolve()),
                                         ("current", path.resolve() == current),
                                         ("prunable", tree.get("prunable")),
                                         ("locked", tree.get("locked"))) if flag]
        exists = path.is_dir()
        if not exists:
            notes.append("missing")
            attention.append(f"missing: {path.name} [{branch or 'detached'}] (directory gone)")
        ahead = outstanding(rev, main_checkout) if rev else 0
        behind = git("rev-list", "--count", f"{rev}..origin/main", cwd=main_checkout) if rev else "?"
        push, unpushed = push_state(branch, rev, ups, main_checkout) if rev else ("?", None)
        dirty = idle = parked = "-"
        if exists:
            # --no-optional-locks: status must not refresh the worktree's index.
            dirty = len(git("--no-optional-locks", "status", "--porcelain", cwd=path,
                            check=False).splitlines())
            try:
                idle = f"{idle_hours(path):.0f}"
            except RuntimeError:
                idle = "?"
            parked = sum(is_parked_status(status.read_text(errors="replace"))
                         for status in live_statuses(path / "work", archive))
            if dirty:
                attention.append(f"dirty: {path.name} [{branch or 'detached'}] {dirty} paths")
        if ahead and push != "gone" and (unpushed is None or unpushed):
            attention.append(f"unpushed: {path.name} [{branch or 'detached'}] {ahead} unmerged "
                             f"commit(s), {push if unpushed is None else f'{unpushed} not on upstream'}")
        lines.append(f"| {path.name} | {branch or 'detached'} | {ahead}/{behind} | {dirty} | {push} | "
                     f"{idle} | {parked} | {', '.join(notes)} |")
    used = {tree["branch"] for tree in trees if tree["branch"]}
    for branch, (upstream, track) in sorted(ups.items()):
        if branch in PROTECTED:
            continue
        if "gone" in track:
            ahead = outstanding(f"refs/heads/{branch}", main_checkout)
            if ahead:
                attention.append(f"upstream gone: {branch} {ahead} unmerged commit(s)")
        elif branch not in used:
            push, unpushed = push_state(branch, f"refs/heads/{branch}", ups, main_checkout)
            if unpushed is None or unpushed:
                ahead = outstanding(f"refs/heads/{branch}", main_checkout)
                if ahead:
                    attention.append(f"unpushed: branch {branch} (no worktree) {ahead} unmerged "
                                     f"commit(s), {push if unpushed is None else f'{unpushed} not on upstream'}")
    lines += ["", "Needs attention:" if attention else "Needs attention: none"]
    lines += [f"- {item}" for item in attention]
    return lines


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--apply", action="store_true", help="act; the default is a dry run")
    parser.add_argument("--remote", action="store_true", help="also delete merged origin branches")
    parser.add_argument("--keep", action="append", default=[], metavar="PATH_OR_BRANCH",
                        help="never remove this worktree or branch (repeatable)")
    parser.add_argument("--min-idle", type=float, default=6, metavar="HOURS",
                        help="skip worktrees active more recently than this (default 6)")
    parser.add_argument("--index-only", action="store_true", help="only rebuild INDEX.md")
    parser.add_argument("--audit", action="store_true",
                        help="read-only report of every worktree's unfinished work; rebuilds INDEX.md")
    parser.add_argument("--fetch", action="store_true", help="with --audit: fetch origin first")
    parser.add_argument("--report", type=Path, help="report.json for main state (default: newest)")
    args = parser.parse_args(argv)

    here = Path.cwd()
    common = Path(git("rev-parse", "--path-format=absolute", "--git-common-dir", cwd=here))
    main_checkout = common.parent
    current = Path(git("rev-parse", "--show-toplevel", cwd=here)).resolve()
    archive = main_checkout / "work" / "archive" / "worktrees"
    trees = worktrees(main_checkout)
    report = args.report or newest_report([tree["path"] for tree in trees])

    if args.audit:
        if args.fetch:
            fetched = subprocess.run([sys.executable, str(Path(__file__).with_name("git_origin.py")),
                                      "fetch", "--quiet", "--prune", "origin"], cwd=main_checkout)
            if fetched.returncode:
                print("warning: fetch failed; using the last fetched origin/main")
        try:
            print("\n".join(audit(trees, main_checkout, current, archive)))
        except RuntimeError as error:
            print(f"audit incomplete: {error}")
    if args.index_only or args.audit:
        total, open_ = build_index(archive, report, live_works(trees, main_checkout))
        print(f"index: {archive / 'INDEX.md'}: {total} parked rows, {open_} unmatched on main")
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
        total, open_ = build_index(archive, report, live_works(trees, main_checkout))
        print(f"index: {archive / 'INDEX.md'}: {total} parked rows, {open_} unmatched on main")
    elif not args.apply:
        print("dry run; pass --apply to act")
    return 0


if __name__ == "__main__":
    sys.exit(main())
