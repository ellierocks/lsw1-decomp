"""kickoff.py: lane table, sweep staleness, recommendation rule and --start idempotence.

No compiler, build or network: next_target, the audit and the snapshot are stubbed.
"""
import argparse
import contextlib
import io
import json
import os
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import kickoff as ko
from tools import plan

sb, sf = ko.sb, ko.sf  # kickoff imports its siblings by bare name


def row(name, unit, size, exact, target, verdict="MISMATCH", **kw):
    return {"name": name, "unit": unit, "source": f"src/{unit}.c", "size": size, "verdict": verdict,
            "exact": exact, "target": target, "closeness": exact / target if target else 0.0, **kw}


ROWS = [row("a", "u1", 0x100, 95, 100), row("b", "u1", 0x80, 90, 100), row("c", "u2", 0x40, 99, 100),
        row("d", "u2", 0x200, 85, 100), row("e", "u3", 0x10, 40, 100), row("f", "u1", 0x20, 70, 100),
        row("m", "u1", 0x30, 10, 10, verdict="MATCH", applied=True), row("x", "u3", 0x30, 0, 0, verdict="COMPILE-ERROR")]


class LaneTableTests(unittest.TestCase):
    def state(self, ready, owned=frozenset(), blockers=None, rows=ROWS):
        return {"rows": rows, "ready": ready, "owned": set(owned), "sweep_dir": Path("work/sweep"),
                "sweep": {"state": "fresh", "why": "no commits", "age_hours": 1.0}, "ready_error": None,
                "units": [{"unit": "u9", "ready": 5, "left": 7, "wave_bytes": 900}],
                "blockers": blockers, "worktrees": "3 worktrees: none need attention; 0 parked notes"}

    def test_bands_split_mismatches_by_ratio(self):
        self.assertEqual(ko.bands(ROWS), [(">=90", 3, 0x1C0), ("80-90", 1, 0x200),
                                          ("50-80", 1, 0x20), ("<50", 1, 0x10)])
        self.assertEqual(ko.verdict_mix(ROWS), {"MISMATCH": 6, "MATCH": 1, "COMPILE-ERROR": 1})

    def test_near_misses_skip_done_claimed_and_owned(self):
        lines, near, unit, blocker = ko.lane_table(self.state({"a", "b", "c", "d"}, owned={"u2"}))
        self.assertEqual(near, {"functions": 2, "bytes": 0x180})
        self.assertIn("2 fn, 384 B at >=90%", lines[1])
        self.assertIn("drafts 80-90 0 fn", lines[2])  # d is in an owned unit
        self.assertEqual(unit["unit"], "u9")
        self.assertIsNone(blocker)
        self.assertIn("run python3 tools/sweep_report.py", "\n".join(lines))
        _, near, _, _ = ko.lane_table(self.state(ready=None))
        self.assertEqual(near["functions"], 3)  # no ready list: unfiltered

    def test_blockers_table_is_parsed_defensively(self):
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / "b.md"
            self.assertIsNone(ko.read_blockers(path))
            path.write_text("# Sweep blockers\n\nNo table here.\n")
            self.assertIsNone(ko.read_blockers(path))
            path.write_text("## Ranked by bytes\n\nTop 20.\n\n| class | functions | bytes | bytes >=80% | example |\n"
                            "| --- | ---: | ---: | ---: | --- |\n| `frame:x` | 12 | 9,000 | 4,096 | f |\n"
                            "| `unswept:no-source` | 3 | 200 | - | g |\n\n## Next\n| x |\n")
            rows = ko.read_blockers(path)
            self.assertEqual([r["name"] for r in rows], ["frame:x", "unswept:no-source"])
            self.assertEqual((rows[0]["functions"], rows[0]["bytes"], rows[0]["bytes80"]), (12, 9000, 4096))
            self.assertIsNone(rows[1]["bytes80"])
            text = "\n".join(ko.lane_table(self.state({"a"}, blockers=rows))[0])
            self.assertIn("workflow #1  frame:x: 12 fn, 9,000 B, 4,096 B at >=80%", text)
            self.assertIn("workflow #2  unswept:no-source: 3 fn, 200 B  ->", text)


class StalenessTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.git("init", "-q", "-b", "main")
        self.commit("src/u1.c", "base")
        patch = mock.patch.object(ko, "ROOT", self.root)
        patch.start()
        self.addCleanup(patch.stop)

    def git(self, *args):
        return subprocess.run(["git", "-C", str(self.root), "-c", "user.name=t", "-c", "user.email=t@t",
                               "-c", "commit.gpgsign=false", "-c", "core.hooksPath=/dev/null", *args],
                              check=True, capture_output=True, text=True).stdout.strip()

    def commit(self, path, text):
        p = self.root / path
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text)
        self.git("add", path)
        self.git("commit", "-qm", path + text)

    def test_missing_and_no_asm(self):
        self.assertEqual(ko.sweep_status(None, None, None)["state"], "missing")
        state = ko.sweep_status([row("a", "u1", 4, 0, 0, verdict="NO-ASM")], None, 1.0)
        self.assertEqual(state["state"], "stale")

    def test_swept_commit_counts_only_input_changes(self):
        meta = {"head": self.git("rev-parse", "HEAD")}
        rows = [row("a", "u1", 4, 1, 2), row("m", "u2", 4, 1, 1, verdict="MATCH", applied=True)]
        self.assertEqual(ko.sweep_status(rows, meta, 1.0)["state"], "fresh")
        self.commit("docs/x.md", "doc")
        self.assertEqual(ko.sweep_status(rows, meta, 1.0)["state"], "fresh")
        self.commit("src/u2.c", "the sweep's own applied match")
        self.assertEqual(ko.sweep_status(rows, meta, 1.0)["state"], "fresh")
        self.commit("tools/sweep_repairs/new.py", "repair")
        state = ko.sweep_status(rows, meta, 1.0)
        self.assertEqual(state["state"], "stale")
        self.assertIn("1 commit", state["why"])

    def test_mtime_fallback_and_unknown_commit(self):
        rows = [row("a", "u1", 4, 1, 2)]
        self.assertEqual(ko.sweep_status(rows, None, 4e9)["state"], "fresh")
        self.assertEqual(ko.sweep_status(rows, None, 1.0)["state"], "stale")  # base commit is newer
        self.assertEqual(ko.sweep_status(rows, {"head": "0" * 40}, 4e9)["state"], "fresh")  # falls back


class RecommendTests(unittest.TestCase):
    fresh = {"state": "fresh", "why": "-"}
    unit = {"unit": "u9", "wave_bytes": 900}

    def test_stale_or_missing_sweep_comes_first(self):
        for state in ("stale", "missing"):
            lane, _, _ = ko.recommend({"state": state, "why": "x"}, {"functions": 3, "bytes": 99}, self.unit, None, {})
            self.assertEqual(lane, "sweep")

    def test_near_misses_then_unit_then_workflow(self):
        near = {"functions": 2, "bytes": 300}
        none = {"functions": 0, "bytes": 0}
        self.assertEqual(ko.recommend(self.fresh, near, self.unit, None, {})[0], "near-misses")
        self.assertEqual(ko.recommend(self.fresh, none, self.unit, None, {})[0], "unit")
        self.assertEqual(ko.recommend(self.fresh, none, None, {"cells": ["b"], "bytes80": 1}, {})[0], "workflow")
        self.assertEqual(ko.recommend(self.fresh, none, None, None, {})[0], "none")

    def test_measured_rates_override_and_blocker_alternative(self):
        near = {"functions": 2, "bytes": 300}
        lane, why, _ = ko.recommend(self.fresh, near, self.unit, None, {"near-misses": 100.0, "unit": 500.0})
        self.assertEqual(lane, "unit")
        self.assertIn("measured", why)
        self.assertEqual(ko.recommend(self.fresh, near, self.unit, None, {"unit": 500.0})[0], "near-misses")
        _, _, alt = ko.recommend(self.fresh, near, self.unit, {"cells": ["void *"], "bytes80": 301}, {})
        self.assertIn("void *", alt)
        self.assertIsNone(ko.recommend(self.fresh, near, self.unit, {"cells": ["v"], "bytes80": 300}, {})[2])

    def test_rates_need_verified_bytes_and_old_records_stay_readable(self):
        old = {"unit": "src/u.c", "verified": 29, "active_seconds": 3600, "code_bytes_delta": 3680}
        new = {"lane": "near-misses", "verified": 4, "verified_bytes": 800, "active_seconds": 1800}
        self.assertEqual(ko.lane_rates([old, new]), {"near-misses": 1600.0})
        lines = ko.yield_lines([old, new])
        self.assertIn("unit src/u.c", lines[0])
        self.assertIn("verified bytes not recorded", lines[0])
        self.assertIn("800 B = 1,600 B/h", lines[1])
        self.assertEqual(ko.yield_lines([]), ["no measurement (no finished bench runs in ~/.cache/lsw1/bench)"])


class StartTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        root = self.root = Path(self.tmp.name)
        build = root / "build" / "GL5E4F"
        (build / "asm").mkdir(parents=True)
        (build / "asm" / "u.s").write_text("")
        (build / "report.json").write_text("{}")
        (build / "main.dol").write_text("")
        (root / "work" / "sweep").mkdir(parents=True)
        (root / "work" / "sweep" / "results.json").write_text(json.dumps(
            [row("near1", "auto/u", 0x40, 19, 20), row("other", "auto/v", 0x40, 19, 20)]))
        ready = [{"name": n, "unit": u, "size": 0x40, "claimed": False, "unit_left": 3}
                 for n, u in (("near1", "auto/u"), ("t1", "auto/u"), ("t2", "auto/u"), ("other", "auto/v"))]
        self.behind = 0
        self.owned = set()
        self.claimed = []
        self.plan_claims = {"items": {}, "stale": []}
        fixture_plan = {"revision": 1, "stage": [
            {"id": "s1", "item": [{"id": "first", "functions": ["t2", "t1"]}, {"id": "second", "units": ["auto/u"]}]},
            {"id": "s2", "item": [{"id": "later", "units": ["auto/v"]}]}]}
        fixture_report = {"measures": {}, "units": [
            {"name": "main/auto/u", "functions": [{"name": n, "size": "64"} for n in ("near1", "t1", "t2")]},
            {"name": "main/auto/v", "functions": [{"name": "other", "size": "64"}]}]}
        snap = {"functions": 1, "code": 2, "strict_only_functions": 0, "strict_only_bytes": 0,
                "dol_sha1": "x", "head": "abc"}
        patches = [mock.patch.object(ko, "ROOT", root), mock.patch.object(ko, "BUILD", build),
                   mock.patch.object(sf, "ROOT", root), mock.patch.object(sf, "BASELINE", root / "work" / "sprint_baseline.json"),
                   mock.patch.object(sf, "snapshot", return_value=snap),
                   mock.patch.object(sf, "behind_origin", side_effect=lambda: self.behind),
                   mock.patch.object(ko.nt, "collect", return_value=(ready, 0)),
                   mock.patch.object(ko.nt, "owned_units", side_effect=lambda rows, remote=True: self.owned),
                   mock.patch.object(ko, "worktree_summary", return_value=None),
                   mock.patch.object(ko, "plan_status", side_effect=lambda remote: plan.status(
                       plan=fixture_plan, report=fixture_report, cl=self.plan_claims, records=[])),
                   mock.patch.object(ko.plan, "claim", side_effect=lambda item: self.claimed.append(item) or item),
                   mock.patch.object(sb, "ROOT", root), mock.patch.object(sb, "EVENTS", root / "work" / "sprint_bench.jsonl"),
                   mock.patch.dict(os.environ, {"LSW1_CACHE": str(root / "cache")})]
        for p in patches:
            p.start()
            self.addCleanup(p.stop)

    def run_main(self, *argv):
        out, err = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            code = ko.main(["--no-remote", "--sweep-dir", str(self.root / "work" / "sweep"), "--model", "m", *argv])
        return code, out.getvalue() + err.getvalue()

    def events(self):
        return [json.loads(l) for l in (self.root / "work" / "sprint_bench.jsonl").read_text().splitlines()]

    def test_start_is_idempotent(self):
        code, out = self.run_main("--start", "src/auto/u.c", "--off-plan", "test")
        self.assertEqual(code, 0, out)
        base = json.loads(sf.BASELINE.read_text())
        self.assertEqual(base["queue"], ["near1", "t1", "t2"])
        self.assertEqual((base["lane"], base["unit"]), ("unit", "auto/u"))
        self.assertEqual([e["kind"] for e in self.events()], ["start"])
        self.assertEqual((self.events()[0]["lane"], self.events()[0]["unit"]), ("unit", "auto/u"))
        self.assertTrue((self.root / "work" / "sprint_timing.json").exists())
        code, out = self.run_main("--start", "auto/u", "t9", "--off-plan", "test")
        self.assertEqual(code, 0, out)
        self.assertIn("baseline: kept", out)
        self.assertIn("already open", out)
        self.assertEqual(json.loads(sf.BASELINE.read_text())["queue"], ["near1", "t1", "t2", "t9"])
        self.assertEqual(len(self.events()), 1)

    def test_other_lane_needs_restart(self):
        self.assertEqual(self.run_main("--start", "auto/u", "--off-plan", "test")[0], 0)
        code, out = self.run_main("--start-lane", "near-misses", "--off-plan", "test")
        self.assertEqual(code, 2)
        self.assertIn("--restart", out)
        code, out = self.run_main("--start-lane", "near-misses", "--restart", "--off-plan", "test", "--all")
        self.assertEqual(code, 0, out)
        self.assertEqual(json.loads(sf.BASELINE.read_text())["queue"], ["near1", "other"])
        self.assertEqual(self.events()[0]["lane"], "near-misses")

    def test_refusals_behind_missing_build_and_owned(self):
        off = ("--off-plan", "test")
        self.behind = 2
        code, out = self.run_main("--start", "auto/u", *off)
        self.assertEqual(code, 2)
        self.assertIn("2 commits behind", out)
        self.behind = 0
        self.owned = {"auto/u"}
        self.assertEqual(self.run_main("--start", "auto/u", *off)[0], 2)
        self.assertEqual(self.run_main("--start", "auto/u", "--force", *off)[0], 0)
        (ko.BUILD / "report.json").unlink()
        code, out = self.run_main("--start", "auto/u", "--restart", *off)
        self.assertEqual(code, 2)
        self.assertIn("run bash build.sh", out)


    def test_off_plan_starts_refuse_without_reason(self):
        for argv in (("--start", "auto/u"), ("--start-lane", "near-misses"), ("--start-lane", "sweep"),
                     ("--start", "--item", "later")):
            code, out = self.run_main(*argv)
            self.assertEqual(code, 2, argv)
            self.assertIn("--off-plan", out)
            self.assertIn("next item is first", out)
        self.assertFalse(sf.BASELINE.exists())
        self.assertEqual(self.claimed, [])
        code, out = self.run_main("--start", "--item", "second")  # any item of the current stage
        self.assertEqual(code, 0, out)
        self.assertEqual(self.claimed, ["second"])
        code, out = self.run_main("--start", "auto/u", "--off-plan", "runtime evidence", "--restart")
        self.assertEqual(code, 0, out)
        self.assertEqual(json.loads(sf.BASELINE.read_text())["off_plan"], "runtime evidence")
        self.assertEqual(self.events()[0]["off_plan"], "runtime evidence")

    def test_item_start_claims_and_queues_the_item(self):
        code, out = self.run_main("--start", "--item", "first")
        self.assertEqual(code, 0, out)
        self.assertEqual(self.claimed, ["first"])
        base = json.loads(sf.BASELINE.read_text())
        self.assertEqual((base["lane"], base["item"], base["queue"]), ("plan", "first", ["t2", "t1"]))
        self.assertNotIn("off_plan", base)
        self.assertEqual((self.events()[0]["lane"], self.events()[0]["item"]), ("plan", "first"))
        self.assertEqual(self.run_main("--start", "--item", "first")[0], 0)  # idempotent
        self.assertIn("already open", self.run_main("--start", "--item", "first")[1])
        self.plan_claims = {"items": {"first": {"by": "elsewhere", "mine": False}}, "stale": []}
        code, out = self.run_main("--start", "--item", "first", "--restart")
        self.assertEqual(code, 2)
        self.assertIn("claimed by elsewhere", out)
        code, out = self.run_main("--start", "--item", "second", "--restart")  # now the next item
        self.assertEqual(code, 0, out)

    def test_near_miss_lane_defaults_to_the_plan_scope(self):
        rows = [row("t1", "auto/u", 0x10, 19, 20), row("near1", "auto/u", 0x80, 19, 20),
                row("other", "auto/v", 0x400, 19, 20)]
        (self.root / "work" / "sweep" / "results.json").write_text(json.dumps(rows))
        code, out = self.run_main("--start-lane", "near-misses", "--off-plan", "permuter burst")
        self.assertEqual(code, 0, out)
        self.assertEqual(json.loads(sf.BASELINE.read_text())["queue"], ["t1", "near1"])


class FinishHelperTests(unittest.TestCase):
    def test_return_type_on_previous_line(self):
        text = ("static s32\nhelper(void)\n{\n}\nvoid *\nfn_A(int a,\n    int b) {\n}\n"
                "__attribute__((noinline)) static void\nfn_B(void) {\n}\nint x;\nvoid c(void) {\n"
                "    if (x)\n    {\n    }\n}\nsize_t\n_DEFUN (strlen, (s), const char *s)\n{\n}\n")
        self.assertEqual(sf.defined_functions(text), ["helper", "fn_A", "fn_B", "c"])
        self.assertEqual(sf.global_functions(text), ["fn_A", "c"])


if __name__ == "__main__":
    unittest.main()
