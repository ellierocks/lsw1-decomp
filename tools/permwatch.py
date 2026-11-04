#!/usr/bin/env python3
"""Read-only terminal monitor for a permute_long.sh queue.

    python3 tools/permwatch.py                    # TUI over the permute_long.sh queue
    python3 tools/permwatch.py --once             # print one snapshot and exit
    python3 tools/permwatch.py --targets A B,C    # explicit targets instead of the queue
    python3 tools/permwatch.py --unit NAME        # queue launched with permute_long.sh --unit NAME

The header reads `N done · K matched · R running · B pending`. Targets come from the
queue log (everything launched or skipped since its last `start` line) plus the pending
`backlog.txt`, so targets added with `permute_long.sh --add` appear; `queue_targets.txt`
is the fallback for a queue that has not started, and `--targets` overrides all of it.
The state of each is read from its `overnight.log`, `output-*` directories, the queue
log's `finish` lines and the running `permute_setup.py --run` processes. Keys: d details,
q quit. Colours are the terminal theme's ANSI green and cyan.
"""
import argparse
import curses
import glob
import os
import re
import sys
import time

DEFAULT_ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "work", "permute")
DEFAULT_UNIT = "lsw1-permute"
ITER = re.compile(rb"iteration (\d+), (\d+) errors,[^\r\n]*?score = (\d+)")
BASE = re.compile(rb"base score = (\d+)")


def queue_path(root, unit, stem, ext="txt"):
    suffix = "" if unit == DEFAULT_UNIT else f"-{unit}"
    return os.path.join(root, f"{stem}{suffix}.{ext}")


def queue_file(root, unit):
    return queue_path(root, unit, "queue_targets")


def read_lines(path):
    try:
        with open(path, errors="replace") as f:
            return [line.rstrip("\n") for line in f]
    except OSError:
        return []


def queue_log(root, unit):
    """(ordered targets, finish info, skipped) from the queue log since its last start line."""
    lines = read_lines(queue_path(root, unit, "queue", "log"))
    start = max((i for i, line in enumerate(lines) if line.startswith("start ")), default=-1)
    order, finish, skipped = [], {}, set()
    for line in lines[start + 1:]:
        w = line.split()
        if len(w) < 2 or w[0] not in ("launch", "finish", "skip", "requeue"):
            continue
        if w[0] == "requeue":
            if w[1] in order:
                order.remove(w[1])
            continue
        if w[1] not in order:
            order.append(w[1])
        if w[0] == "skip":
            skipped.add(w[1])
        elif w[0] == "finish":
            finish[w[1]] = dict(p.split("=", 1) for p in w[2:] if "=" in p)
    return order, finish, skipped


def queue_alive(root, unit):
    try:
        pid = int(read(queue_path(root, unit, "queue", "pid")).strip() or 0)
        return pid > 0 and os.path.exists(f"/proc/{pid}")
    except (ValueError, OSError):
        return False


def load_queue(root, unit, explicit):
    """Targets plus the queue-log facts. Explicit targets ignore the log and backlog."""
    if explicit:
        names = [n for item in explicit for n in item.split(",") if n]
        return {"targets": list(dict.fromkeys(names)), "finish": {}, "skipped": set(), "pending": set(),
                "launched": set(), "alive": False}
    order, finish, skipped = queue_log(root, unit)
    pending = [n.strip() for n in read_lines(queue_path(root, unit, "backlog")) if n.strip()]
    pending = [n for n in dict.fromkeys(pending) if n not in order]
    if not order and not pending:
        pending = list(dict.fromkeys(n.strip() for n in read_lines(queue_file(root, unit)) if n.strip()))
    return {"targets": order + pending, "finish": finish, "skipped": skipped, "pending": set(pending),
            "launched": set(order), "alive": queue_alive(root, unit)}


def running(targets):
    """Map running target -> process start time."""
    live = {}
    for cmd in glob.glob("/proc/[0-9]*/cmdline"):
        try:
            with open(cmd, "rb") as f:
                c = f.read()
            if b"permute_setup.py" not in c or b"--run" not in c:
                continue
            start = os.stat(os.path.dirname(cmd)).st_ctime
        except OSError:
            continue
        for t in targets:
            if t.encode() + b"\0" in c:
                live[t] = min(live.get(t, start), start)
    return live


def read(path, tail=False, n=8192):
    try:
        with open(path, "rb") as f:
            if tail:
                f.seek(0, 2)
                f.seek(max(0, f.tell() - n))
            return f.read(n)
    except OSError:
        return b""


def target(root, t, live, now, info=None):
    d = os.path.join(root, t)
    log = os.path.join(d, "overnight.log")
    m = BASE.search(read(log))
    base = int(m.group(1)) if m else None
    its = ITER.findall(read(log, tail=True))
    scores = [int(m.group(1)) for o in glob.glob(os.path.join(d, "output-*"))
              if (m := re.match(r"output-(\d+)-\d+$", os.path.basename(o)))]
    best = min(scores) if scores else None
    if best is not None and base is not None:
        best = min(best, base)
    info = info or {"finish": {}, "skipped": set(), "pending": set(), "launched": set(), "alive": False}
    fin = info["finish"].get(t)
    if fin:
        if base is None and fin.get("base", "?").isdigit():
            base = int(fin["base"])
        if fin.get("best", "none").isdigit():
            best = int(fin["best"]) if best is None else min(best, int(fin["best"]))
    if best == 0:
        state = "match"
    elif t in live:
        state = "running"
    elif t in info["pending"]:
        state = "queued"
    elif t in info["launched"] and fin is None and t not in info["skipped"]:
        state = "running" if info["alive"] else "done"
    elif fin is not None or t in info["skipped"] or os.path.exists(log):
        state = "done"
    else:
        state = "queued"
    elapsed = int(now - live[t]) if t in live else 0
    return {"name": t, "state": state, "base": base, "best": best if best is not None else base,
            "iters": int(its[-1][0]) if its else 0, "elapsed": elapsed}


def hm(seconds):
    return f"{seconds // 3600}h{seconds // 60 % 60:02d}m"


def bar(done, total, width):
    width = max(4, width)
    filled = int(width * done / total) if total else 0
    return "━" * filled + "─" * (width - filled)


def progress(root, info):
    now = time.time()
    targets = info["targets"]
    live = running(targets)
    rows = [target(root, t, live, now, info) for t in targets]
    finished = sum(r["state"] in ("match", "done") for r in rows)
    matched = sum(r["state"] == "match" for r in rows)
    active = sum(r["state"] == "running" for r in rows)
    pending = sum(r["state"] == "queued" for r in rows)
    improved = [r for r in rows if r["state"] != "match" and r["base"] and r["best"] < r["base"]]
    lead = max(improved, key=lambda r: 1 - r["best"] / r["base"], default=None)
    status = f"{active} running"
    if lead:
        status += f" · best gain {lead['name']} {lead['base']}→{lead['best']}"
    if not active and not pending:
        status = "All targets finished"
    return finished, matched, len(rows), status, rows, active, pending


def headline(finished, matched, active, pending):
    return f"{finished} done · {matched} matched · {active} running · {pending} pending"


def line(r, details):
    glyph = {"match": "✓", "running": "●", "done": "·", "queued": "○"}[r["state"]]
    score = f"{r['base']}→{r['best']}" if r["base"] is not None else ("no score" if r["state"] == "done" else "queued")
    s = f"{glyph} {r['name'][:32]:<32} {score:>11}"
    if details and r["state"] == "running":
        s += f"  {r['iters']:>7,} it  {hm(r['elapsed'])}"
    return s


def draw(screen, root, unit, explicit):
    curses.curs_set(0)
    accent = normal = 0
    if curses.has_colors():
        curses.start_color()
        curses.use_default_colors()
        # ANSI palette entries inherit the terminal's active theme, including light mode.
        curses.init_pair(1, curses.COLOR_GREEN, -1)
        curses.init_pair(2, curses.COLOR_CYAN, -1)
        accent, normal = curses.color_pair(1), curses.color_pair(2)
    screen.bkgd(" ", curses.color_pair(0))
    screen.timeout(2000)
    show_details = False
    style = {"match": accent | curses.A_BOLD, "running": normal, "done": curses.A_DIM, "queued": curses.A_DIM}
    order = {"match": 0, "running": 1, "queued": 2, "done": 3}
    while True:
        screen.erase()
        height, width = screen.getmaxyx()
        finished, matched, total, status, rows, active, pending = progress(root, load_queue(root, unit, explicit))
        content_width = min(76, max(4, width - 8))
        left = max(0, (width - content_width) // 2)
        lines = [("Permuter", curses.A_BOLD), ("", 0),
                 (bar(finished, total, content_width), accent),
                 (headline(finished, matched, active, pending), 0), (status, normal), ("", 0)]
        lines += [(line(r, show_details), style[r["state"]])
                  for r in sorted(rows, key=lambda r: order[r["state"]])]
        lines += [("", 0), ("d hide details · q quit" if show_details else "d details · q quit", 0)]
        top = max(0, (height - len(lines)) // 2)
        for offset, (text, attr) in enumerate(lines):
            if top + offset >= height - 1:
                break
            try:
                screen.addnstr(top + offset, left, text, min(content_width, max(0, width - left - 1)), attr)
            except curses.error:
                pass
        screen.refresh()
        key = screen.getch()
        if key in (ord("q"), ord("Q"), 27):
            return
        if key in (ord("d"), ord("D")):
            show_details = not show_details


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--once", action="store_true", help="print a snapshot without opening a TUI")
    parser.add_argument("--targets", nargs="+", metavar="NAME", help="targets to watch (space or comma separated)")
    parser.add_argument("--unit", default=DEFAULT_UNIT, help="permute_long.sh --unit name whose queue to read")
    parser.add_argument("--root", default=DEFAULT_ROOT, help="permute work directory (default work/permute)")
    args = parser.parse_args(argv)
    root = os.path.normpath(args.root)
    info = load_queue(root, args.unit, args.targets)
    if not info["targets"]:
        print(f"permwatch: no targets; {queue_file(root, args.unit)} is missing or empty. "
              "Launch with tools/permute_long.sh or pass --targets.", file=sys.stderr)
        return 2
    if args.once:
        finished, matched, total, status, rows, active, pending = progress(root, info)
        print(f"Permuter  {headline(finished, matched, active, pending)}")
        print(bar(finished, total, 40))
        print(status)
        for r in rows:
            print(line(r, True))
    else:
        try:
            curses.wrapper(draw, root, args.unit, args.targets)
        except KeyboardInterrupt:
            pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
