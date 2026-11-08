#!/usr/bin/env python3
"""Mark the shared toolchain outputs as current in this checkout's .ninja_log.

A fresh worktree links build/tools, build/compilers and build/binutils to the shared toolchain,
but its checkout gives tools/download_tool.py a newer mtime than any log entry copied from the
primary checkout. Ninja 1.12+ compares the *recorded* output mtime with its inputs, so copied
entries still look stale and the next ninja build re-downloads the tools into the shared
directories, which fails with "Text file busy" while other worktrees run them.

This appends one log entry per shared tool output, with the primary checkout's command hash (a
changed pin still differs and triggers a fetch) and the output's current mtime. It is idempotent:
entries are only appended when the latest recorded mtime is older than an input.
"""
from __future__ import annotations

import argparse
import os
from pathlib import Path

TOOLS = ("build/tools/sjiswrap.exe", "build/tools/wibo", "build/tools/objdiff-cli",
         "build/compilers", "build/binutils")
INPUTS = ("tools/download_tool.py",)
HEADER = "# ninja log v7\n"


def read_log(path: Path) -> dict[str, list[str]]:
    """Last entry per output: [start, end, mtime, output, hash]."""
    entries: dict[str, list[str]] = {}
    if not path.is_file():
        return entries
    for line in path.read_text(errors="replace").splitlines():
        if line.startswith("#"):
            continue
        parts = line.split("\t")
        if len(parts) == 5:
            entries[parts[3]] = parts
    return entries


def mark(repo: Path, primary: Path) -> list[str]:
    """Append current entries for stale shared tools; return the outputs marked."""
    log = repo / ".ninja_log"
    ours = read_log(log)
    theirs = read_log(primary / ".ninja_log")
    newest_input = max((os.stat(repo / i).st_mtime_ns for i in INPUTS if (repo / i).exists()),
                       default=0)
    lines: list[str] = []
    for out in TOOLS:
        target = repo / out
        if not target.exists() or out not in theirs:
            continue
        cur = ours.get(out)
        if cur and cur[4] == theirs[out][4] and int(cur[2]) >= newest_input:
            continue
        mtime = os.stat(target).st_mtime_ns
        if mtime < newest_input:
            os.utime(target)  # follows the symlink into the shared toolchain
            mtime = os.stat(target).st_mtime_ns
        lines.append("\t".join(["0", "0", str(mtime), out, theirs[out][4]]))
    if lines:
        text = log.read_text(errors="replace") if log.is_file() else ""
        if not text.startswith("# ninja log"):
            text = HEADER + text
        if text and not text.endswith("\n"):
            text += "\n"
        log.write_text(text + "\n".join(lines) + "\n")
    return [line.split("\t")[3] for line in lines]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--primary", type=Path, required=True, help="primary checkout with a .ninja_log")
    ap.add_argument("--repo", type=Path, default=Path("."))
    args = ap.parse_args()
    if args.primary.resolve() == args.repo.resolve():
        return 0
    for out in mark(args.repo, args.primary):
        print(f"marked current: {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
