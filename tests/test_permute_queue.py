"""Drive tools/permute_long.sh --inner with fake jobs in a temp repo-like root, plus
permute_backlog.py ranking and the permwatch header."""
import json
import os
import shutil
import signal
import subprocess
import tempfile
import time
import unittest
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent

FAKE_SETUP = """#!/usr/bin/env python3
import sys
from pathlib import Path
name = sys.argv[1]
if name.startswith("badsetup"):
    print("error: cannot set up " + name, file=sys.stderr)
    sys.exit(2)
Path("work/permute", name).mkdir(parents=True, exist_ok=True)
print("base: 100")
"""

# Behaviour by target prefix: match* -> output-0, improve* -> output-50, fail* -> exit 3,
# slow* -> sleep long, anything else -> sleep briefly with no output.
FAKE_JOB = """#!/usr/bin/env bash
name=$1
dir=work/permute/$name
mkdir -p "$dir"
echo "$(date +%s.%N) start $name" >>events.txt
echo "base score = 100"
case $name in
    match*) sleep "${JOB_SECS:-0.3}"; mkdir -p "$dir/output-0-1" ;;
    improve*) sleep "${JOB_SECS:-0.3}"; mkdir -p "$dir/output-50-1" ;;
    fail*) sleep 0.1; echo "boom"; echo "$(date +%s.%N) end $name" >>events.txt; exit 3 ;;
    slow*) exec sleep "${SLOW_SECS:-30}" ;;
    *) sleep "${JOB_SECS:-0.3}" ;;
esac
echo "$(date +%s.%N) end $name" >>events.txt
"""


class QueueCase(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        (self.root / "tools").mkdir()
        shutil.copy(REPO / "tools" / "permute_long.sh", self.root / "tools")
        (self.root / "tools" / "permute_setup.py").write_text(FAKE_SETUP)
        job = self.root / "job.sh"
        job.write_text(FAKE_JOB)
        job.chmod(0o755)
        self.procs = []
        self.env = {**os.environ, "PERMUTE_LONG_JOB_CMD": str(job), "PERMUTE_LONG_HOUR_SECS": "1",
                    "PERMUTE_LONG_POLL": "0.05"}

    def tearDown(self):
        for p in self.procs:
            try:
                os.killpg(p.pid, signal.SIGKILL)
            except OSError:
                pass
            p.wait()
        self.tmp.cleanup()

    @property
    def pdir(self):
        return self.root / "work" / "permute"

    def start(self, *args):
        p = subprocess.Popen(["bash", "tools/permute_long.sh", "--inner", *args], cwd=self.root, env=self.env,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
                             start_new_session=True)
        self.procs.append(p)
        return p

    def run_queue(self, *args, timeout=20):
        p = self.start(*args)
        out, _ = p.communicate(timeout=timeout)
        return p.returncode, out

    def tool(self, *args):
        return subprocess.run(["bash", "tools/permute_long.sh", *args], cwd=self.root, env=self.env,
                              capture_output=True, text=True, timeout=20)

    def log(self):
        path = self.pdir / "queue.log"
        return path.read_text() if path.exists() else ""

    def wait_for(self, text, timeout=5):
        end = time.time() + timeout
        while time.time() < end:
            if text in self.log():
                return
            time.sleep(0.02)
        self.fail(f"{text!r} not in queue log:\n{self.log()}")

    def events(self):
        rows = []
        for line in (self.root / "events.txt").read_text().splitlines():
            t, kind, name = line.split()
            rows.append((float(t), kind, name))
        return sorted(rows)

    def summary(self):
        return (self.pdir / "queue_summary.txt").read_text()


class SlotTests(QueueCase):
    def test_slots_stay_full_while_backlog_remains(self):
        names = [f"t{i}" for i in range(6)]
        rc, _ = self.run_queue("--unit", "lsw1-permute", "2", "5", *names)
        self.assertEqual(rc, 0)
        events = self.events()
        running = peak = 0
        for _, kind, _ in events:
            running += 1 if kind == "start" else -1
            peak = max(peak, running)
        self.assertEqual(peak, 2)
        starts = [t for t, k, _ in events if k == "start"]
        ends = [t for t, k, _ in events if k == "end"]
        self.assertEqual(len(starts), 6)
        # Each of the first four ends is followed by a new start within 0.25 s.
        for end in sorted(ends)[:4]:
            gap = min(s - end for s in starts if s >= end - 0.01)
            self.assertLess(gap, 0.25)
        self.assertEqual(self.log().count("finish "), 6)
        self.assertIn("unchanged (6):", self.summary())
        self.assertIn("not started (0):", self.summary())

    def test_early_matches_pull_more_targets(self):
        names = [f"match{i}" for i in range(6)]
        # Total 100 "hours" with 2 slots: matches end after 0.3 s, far before the deadline.
        rc, _ = self.run_queue("--total", "100", "2", "50", *names)
        self.assertEqual(rc, 0)
        log = self.log()
        for n in names:
            self.assertIn(f"finish {n} rc=0 base=100 best=0 secs=", log)
        self.assertIn("matched (6):", self.summary())
        self.assertIn("unchanged (0):", self.summary())
        self.assertEqual(self.events()[0][1], "start")

    def test_add_while_running_is_picked_up(self):
        self.env["JOB_SECS"] = "1.2"
        p = self.start("1", "5", "first")
        self.wait_for("launch first")
        out = self.tool("--add", "extra1", "first", "extra1", "extra2")
        self.assertEqual(out.returncode, 0, out.stderr)
        self.assertIn("added extra1", out.stdout)
        self.assertIn("added extra2", out.stdout)
        self.assertIn("skipped first (running or already ran", out.stdout)
        self.assertEqual(out.stdout.count("extra1"), 1)  # repeated argument is added once
        again = self.tool("--add", "extra2")
        self.assertIn("skipped extra2 (already queued)", again.stdout)
        p.communicate(timeout=20)
        log = self.log()
        for n in ("first", "extra1", "extra2"):
            self.assertIn(f"finish {n} rc=0", log)

    def test_add_skips_matched(self):
        self.env["JOB_SECS"] = "0.8"
        (self.pdir / "done1" / "output-0-1").mkdir(parents=True)
        p = self.start("1", "5", "first")
        self.wait_for("launch first")
        out = self.tool("--add", "done1")
        self.assertIn("skipped done1 (already matched)", out.stdout)
        p.communicate(timeout=20)

    def test_add_when_not_running_fails(self):
        out = self.tool("--add", "x")
        self.assertEqual(out.returncode, 1)
        self.assertIn("not running", out.stderr)
        self.assertFalse((self.pdir / "backlog.txt").exists())

    def test_total_deadline_stops_launches_and_keeps_backlog(self):
        self.env["JOB_SECS"] = "0.7"
        names = [f"t{i}" for i in range(8)]
        started = time.time()
        rc, _ = self.run_queue("--total", "2", "1", "5", *names)
        self.assertEqual(rc, 0)
        self.assertLess(time.time() - started, 5)
        launched = self.log().count("launch ")
        self.assertTrue(2 <= launched < 8, self.log())
        left = (self.pdir / "backlog.txt").read_text().split()
        self.assertEqual(left, names[launched:])
        self.assertIn(f"not started ({len(left)}):", self.summary())
        self.assertIn(f"  {names[-1]}", self.summary())

    def test_timeout_is_capped_by_time_left(self):
        started = time.time()
        rc, _ = self.run_queue("--total", "1.5", "1", "100", "slow1")
        self.assertEqual(rc, 0)
        self.assertLess(time.time() - started, 4)
        self.assertIn("finish slow1 rc=124 base=100 best=none", self.log())
        self.assertIn("unchanged (1):", self.summary())

    def test_failures_never_stop_the_queue(self):
        rc, _ = self.run_queue("1", "5", "badsetup1", "fail1", "ok1", "improve1", "match1")
        self.assertEqual(rc, 0)
        log = self.log()
        self.assertIn("skip badsetup1 setup-failed", log)
        self.assertIn("finish fail1 rc=3 base=100 best=none", log)
        self.assertIn("finish ok1 rc=0", log)
        self.assertIn("finish improve1 rc=0 base=100 best=50", log)
        summary = self.summary()
        self.assertIn("matched (1):", summary)
        self.assertIn("improved (1):\n  improve1  100→50", summary)
        self.assertIn("unchanged (1):\n  ok1", summary)
        self.assertIn("failed (2):\n  badsetup1  setup-failed\n  fail1  100→none  rc=3", summary)
        self.assertTrue((self.pdir / "ok1").is_dir())  # setup ran for the missing dir

    def test_matched_dir_skipped_unless_rerun(self):
        (self.pdir / "m1" / "output-0-3").mkdir(parents=True)
        self.run_queue("1", "5", "m1", "ok1")
        self.assertIn("skip m1 matched", self.log())
        self.assertNotIn("finish m1", self.log())
        self.assertIn("matched (1):\n  m1  (already matched)", self.summary())
        self.run_queue("--rerun", "1", "5", "m1")
        self.assertIn("finish m1 rc=0", self.log())

    def test_sigterm_writes_summary_and_keeps_backlog(self):
        p = self.start("1", "5", "slow1", "slow2", "slow3")
        self.wait_for("launch slow1")
        time.sleep(0.3)
        p.send_signal(signal.SIGTERM)
        out, _ = p.communicate(timeout=8)
        summary = self.summary()
        self.assertIn("stopped (signal)", summary)
        self.assertIn("not started (2):\n  slow2\n  slow3", summary)
        self.assertEqual((self.pdir / "backlog.txt").read_text().split(), ["slow2", "slow3"])
        self.assertRegex(self.log(), r"finish slow1 rc=\d+ base=100 best=none")
        self.assertNotIn("launch slow2", self.log())
        self.assertFalse((self.pdir / "queue.pid").exists())

    def test_backlog_is_deduped_in_order(self):
        self.run_queue("2", "5", "b", "a", "b", "c", "a")
        self.assertEqual(self.log().count("launch "), 3)
        launched = [l.split()[1] for l in self.log().splitlines() if l.startswith("launch ")]
        self.assertEqual(launched, ["b", "a", "c"])

    def test_dry_run_changes_nothing(self):
        out = self.tool("--dry-run", "2", "1", "a", "b", "a")
        self.assertEqual(out.returncode, 0, out.stderr)
        self.assertIn("targets=2", out.stdout)
        self.assertFalse(self.pdir.exists())

    def test_options_may_follow_slots_and_hours(self):
        out = self.tool("--dry-run", "7", "2", "--total", "8", "a", "b")
        self.assertEqual(out.returncode, 0, out.stderr)
        self.assertIn("slots=7 hours=2 total=8", out.stdout)
        self.assertIn("targets=2", out.stdout)

    def test_non_default_unit_uses_suffixed_files(self):
        self.run_queue("--unit", "other", "1", "5", "ok1")
        self.assertTrue((self.pdir / "queue-other.log").exists())
        self.assertTrue((self.pdir / "queue_summary-other.txt").exists())
        self.assertFalse((self.pdir / "queue.log").exists())


class BacklogTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        (self.root / "work" / "sweep").mkdir(parents=True)
        (self.root / "build" / "GL5E4F").mkdir(parents=True)

    def tearDown(self):
        self.tmp.cleanup()

    def sweep(self, rows):
        (self.root / "work" / "sweep" / "results.json").write_text(json.dumps(rows))

    def run_tool(self, *args):
        p = subprocess.run(["python3", str(REPO / "tools" / "permute_backlog.py"), "--root", str(self.root),
                            *args], capture_output=True, text=True, timeout=20)
        self.assertEqual(p.returncode, 0, p.stderr)
        return p.stdout.split(), p.stderr

    @staticmethod
    def row(name, exact, size, verdict="MISMATCH"):
        return {"name": name, "unit": "u", "size": size, "verdict": verdict, "exact": exact, "target": 100,
                "closeness": exact / 100}

    def test_rank_filter_and_dedupe(self):
        self.sweep([self.row("low", 50, 900), self.row("small_hi", 99, 40), self.row("big_hi", 99, 400),
                    self.row("mid", 95, 100), self.row("done", 100, 10, "MATCH"),
                    {"name": "broken", "unit": "u", "size": 5, "verdict": "ERROR"},
                    self.row("small_hi", 99, 40)])
        names, _ = self.run_tool()
        self.assertEqual(names, ["big_hi", "small_hi", "mid"])
        self.assertEqual(self.run_tool("--limit", "2")[0], ["big_hi", "small_hi"])
        self.assertEqual(self.run_tool("--min-ratio", "0.4")[0], ["big_hi", "small_hi", "mid", "low"])

    def test_excludes_report_matched_and_permuter_output_zero(self):
        self.sweep([self.row("a", 99, 10), self.row("b", 98, 10), self.row("c", 97, 10), self.row("d", 96, 10)])
        (self.root / "build" / "GL5E4F" / "report.json").write_text(json.dumps(
            {"units": [{"functions": [{"name": "a", "fuzzy_match_percent": 100},
                                      {"name": "b", "fuzzy_match_percent": 97.5}]}]}))
        (self.root / "work" / "permute" / "c" / "output-0-2").mkdir(parents=True)
        (self.root / "work" / "permute" / "d" / "output-12-2").mkdir(parents=True)
        self.assertEqual(self.run_tool()[0], ["b", "d"])

    def test_index_rows_added_unless_matched_on_main(self):
        self.sweep([self.row("a", 99, 10)])
        index = self.root / "INDEX.md"
        index.write_text("# Parked index\n\n| function | from | main | permuter | status | file (under work/) |\n"
                         "|---|---|---|---|---|---|\n"
                         "| parked1 | wt | 91.2% | 100→40 | stuck | w/parked1/status.md |\n"
                         "| gone | wt | matched | 100→0 | ok | w/gone/status.md |\n"
                         "| a | wt | 99.0% |  | dup | w/a/status.md |\n")
        names, _ = self.run_tool("--index", str(index))
        self.assertEqual(names, ["a", "parked1"])

    def test_size_recommendation_goes_to_stderr(self):
        self.sweep([self.row("a", 99, 10)])
        names, err = self.run_tool("--slots", "7", "--hours", "8", "--per", "2", "--limit", "10")
        self.assertEqual(names, ["a"])
        self.assertIn("recommended backlog size: 56", err)
        self.assertIn("--limit 10 is smaller", err)
        self.assertIn("only 1 candidates", err)


class PermwatchTests(unittest.TestCase):
    def test_header_counts_done_matched_running_pending(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "m1" / "output-0-1").mkdir(parents=True)
            (root / "i1" / "output-40-1").mkdir(parents=True)
            (root / "queue.log").write_text(
                "start 2026-01-01 unit=lsw1-permute slots=2\n"
                "launch m1 t\nfinish m1 rc=0 base=90 best=0 secs=5\n"
                "launch i1 t\nfinish i1 rc=0 base=100 best=40 secs=9\n"
                "launch u1 t\nskip u1 setup-failed\n")
            (root / "backlog.txt").write_text("p1\np2\n")
            out = subprocess.run(["python3", str(REPO / "tools" / "permwatch.py"), "--once", "--root", str(root)],
                                 capture_output=True, text=True, timeout=20)
            self.assertEqual(out.returncode, 0, out.stderr)
            self.assertIn("Permuter  3 done · 1 matched · 0 running · 2 pending", out.stdout)
            for name in ("m1", "i1", "u1", "p1", "p2"):
                self.assertIn(name, out.stdout)
            self.assertIn("100→40", out.stdout)


if __name__ == "__main__":
    unittest.main()
