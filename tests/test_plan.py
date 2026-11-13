"""plan.py: closure with parked functions, current stage, next item with claims and look-ahead,
stale claims, revision warnings and yield. Fixture plan/report dicts; no retail inputs."""
import json
import subprocess
import tempfile
import unittest
from pathlib import Path

from tools import plan


def fn(name, size, matched=False):
    return {"name": name, "size": str(size), **({"fuzzy_match_percent": 100.0} if matched else
                                                {"fuzzy_match_percent": 50.0})}


def report(**units):
    return {"measures": {"matched_data": "40", "total_data": "100"},
            "units": [{"name": f"main/{u}", "functions": fns} for u, fns in units.items()]}


REPORT = report(**{
    "core/a": [fn("a1", 10, True), fn("a2", 20)],
    "core/b": [fn("b1", 30), fn("b2", 40)],
    "game/c": [fn("c1", 50), fn("c2", 60), fn("c3", 70, True)],
    "data_only": [],
})

PLAN = {
    "revision": 1,
    "parked": [{"function": "b2", "reason": "best 9/10"}],
    "stage": [
        {"id": "s1", "item": [{"id": "hub", "functions": ["c1"]},
                               {"id": "core", "units": ["core/a", "core/b"]}]},
        {"id": "s2", "item": [{"id": "game", "units": ["game/c"]},
                               {"id": "stuck", "units": [], "functions": [], "blocked": "no route"}]},
        {"id": "s3", "item": [{"id": "data", "data": True}]},
    ],
}

NO_CLAIMS = {"items": {}, "stale": []}


def state_with(cl=NO_CLAIMS, plan_=PLAN, report_=REPORT, records=()):
    return plan.status(plan=plan_, report=report_, cl=cl, records=list(records))


class EvaluateTests(unittest.TestCase):
    def test_items_close_on_matched_or_parked(self):
        s = plan.evaluate(PLAN, REPORT)
        core = plan.find_item(s, "core")
        self.assertEqual((core["functions"], core["fn_left"], core["bytes_left"]), (["a2", "b1"], 2, 50))
        self.assertEqual(core["parked"], ["b2"])
        self.assertFalse(core["closed"])
        self.assertEqual(plan.find_item(s, "hub")["units"], ["game/c"])
        data = plan.find_item(s, "data")
        self.assertEqual((data["bytes_left"], data["closed"]), (60, False))
        self.assertEqual(s["current"], "s1")
        done = report(**{"core/a": [fn("a2", 20, True)], "core/b": [fn("b1", 30, True), fn("b2", 40)],
                         "game/c": [fn("c1", 50, True), fn("c2", 60)]})
        s = plan.evaluate(PLAN, done)
        self.assertTrue(plan.find_item(s, "core")["closed"])  # b2 is parked
        self.assertTrue(plan.stage_of(s, "s1")["closed"])
        self.assertEqual(s["current"], "s2")

    def test_revision_warnings_for_missing_and_uncovered(self):
        p = json.loads(json.dumps(PLAN))
        p["stage"][0]["item"][0]["functions"].append("nope")
        p["stage"][0]["item"][1]["units"] = ["core/a", "core/zz"]
        warnings = plan.evaluate(p, REPORT)["warnings"]
        self.assertIn("item hub names function nope, which the report lacks", warnings)
        self.assertIn("item core names unit core/zz, which the report lacks", warnings)
        self.assertIn("2 unmatched function(s) of core/b are in no item", warnings)
        self.assertEqual(plan.evaluate(PLAN, REPORT)["warnings"], [])


class NextItemTests(unittest.TestCase):
    def test_first_open_item_then_lookahead_and_mine(self):
        s = state_with()
        self.assertEqual((s["next"]["item"]["id"], s["next"]["lookahead"]), ("hub", False))
        cl = {"items": {"hub": {"by": "x", "mine": False}}, "stale": []}
        self.assertEqual(state_with(cl)["next"]["item"]["id"], "core")
        cl["items"]["core"] = {"by": "y", "mine": False}
        s = state_with(cl)
        self.assertEqual((s["next"]["item"]["id"], s["next"]["stage"], s["next"]["lookahead"]), ("game", "s2", True))
        self.assertTrue(any("stage s1 is not closed but has no recommendable item" in w for w in s["warnings"]))
        cl["items"]["core"] = {"by": "me", "mine": True}
        s = state_with(cl)
        self.assertEqual((s["next"]["item"]["id"], s["next"]["mine"]), ("core", True))
        targets = plan.plan_targets(state=s)
        self.assertEqual((targets["item"], targets["mine"], targets["functions"]), ("core", True, ["a2", "b1"]))
        self.assertEqual(targets["stage"], ["c1", "a2", "b1"])

    def test_alongside_stage_runs_beside_the_current_one(self):
        p = json.loads(json.dumps(PLAN))
        p["stage"].insert(0, {"id": "s0", "alongside": True, "item": [{"id": "side", "units": ["game/c"]}]})
        s = state_with(plan_=p)
        self.assertEqual(s["current"], "s1")
        self.assertEqual(s["next"]["item"]["id"], "side")
        active = [i["id"] for i, la in plan.active_items(s, NO_CLAIMS)]
        self.assertEqual(active, ["side", "hub", "core"])
        cl = {"items": {k: {"by": "x", "mine": False} for k in ("side", "hub", "core")}, "stale": []}
        self.assertEqual([(i["id"], la) for i, la in plan.active_items(s, cl)], [("game", True)])
        done = report(**{"core/a": [], "core/b": [], "game/c": [fn("c1", 50, True), fn("c2", 60)]})
        s = state_with(plan_=p, report_=done)
        self.assertEqual(s["current"], "s2")
        self.assertEqual(s["next"]["item"]["id"], "side")  # still open, still worked

    def test_blocked_items_are_never_recommended(self):
        p = json.loads(json.dumps(PLAN))
        p["stage"][1]["item"][1] = {"id": "stuck", "units": ["game/c"], "blocked": "no route"}
        p["stage"][1]["item"].pop(0)
        done = report(**{"core/a": [], "core/b": [], "game/c": [fn("c1", 50, True), fn("c2", 60)]})
        s = state_with(plan_=p, report_=done)
        self.assertEqual(s["current"], "s2")
        self.assertEqual(s["next"]["item"]["id"], "data")  # look-ahead past the blocked stage
        self.assertTrue(s["next"]["lookahead"])
        self.assertTrue(any("stage s2" in w for w in s["warnings"]))

    def test_zero_verified_bench_run_and_yield(self):
        records = [{"run_id": "r1", "item": "core", "verified": 0, "verified_bytes": 0, "new_verified": 0, "new_verified_bytes": 0, "active_seconds": 1800},
                   {"run_id": "r2", "item": "hub", "verified": 4, "verified_bytes": 400, "new_verified": 4, "new_verified_bytes": 400, "active_seconds": 1800},
                   {"run_id": "r3", "unit": "old", "verified": 0, "active_seconds": 60}]
        s = state_with(records=records)
        self.assertIn("bench run r1 on item core added no new strict matches", s["warnings"])
        self.assertEqual(len([w for w in s["warnings"] if "bench run" in w]), 1)
        y = s["yield"]["s1"]
        self.assertEqual((y["runs"], y["verified"], y["fn_per_hour"], y["bytes_per_hour"]), (2, 4, 4.0, 400.0))


class ClaimTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name) / "repo"
        self.root.mkdir()
        self.git("init", "-q", "-b", "main")
        self.git("commit", "-q", "--allow-empty", "-m", "base")
        self.git("update-ref", "refs/remotes/origin/main", "HEAD")
        self.git("checkout", "-q", "-b", "t3/work")
        self.state = plan.evaluate(PLAN, REPORT)

    def git(self, *args):
        subprocess.run(["git", "-c", "user.email=t@t", "-c", "user.name=t", "-c", "commit.gpgsign=false",
                        "-c", "core.hooksPath=/dev/null", *args], cwd=self.root,
                       check=True, capture_output=True)

    def claims(self, root=None):
        return plan.claims(self.state, root=root or self.root, branches={}, sprints=set())

    def test_claim_record_is_mine_here_and_taken_elsewhere(self):
        path = plan.claim("hub", root=self.root)
        self.assertEqual(path.parent, self.root / ".git" / plan.CLAIMS)
        rec = json.loads(path.read_text())
        self.assertEqual((rec["item"], rec["branch"], rec["worktree"]), ("hub", "t3/work", str(self.root.resolve())))
        cl = self.claims()
        self.assertTrue(cl["items"]["hub"]["mine"])
        other = Path(self.tmp.name) / "other"
        self.git("worktree", "add", "-q", "-b", "t3/other", str(other))
        cl = self.claims(root=other)
        self.assertTrue(plan.taken(cl, "hub"))
        self.assertEqual(plan.next_item(self.state, cl)["item"]["id"], "core")
        self.assertTrue(plan.release("hub", root=self.root))
        self.assertEqual(self.claims()["items"], {})

    def test_stale_claims_are_ignored(self):
        plan.claim("hub", root=self.root)
        self.assertIsNone(plan.read_records(self.root)[0]["stale"])  # fresh branch, no commits yet
        self.git("commit", "-q", "--allow-empty", "-m", "work")
        self.assertIsNone(plan.read_records(self.root)[0]["stale"])  # commits not merged
        self.git("update-ref", "refs/remotes/origin/main", "HEAD")
        cl = self.claims()
        self.assertEqual(cl["items"], {})
        self.assertIn("merged into origin/main", cl["stale"][0]["stale"])
        rec = {"item": "core", "branch": "t3/x", "worktree": str(Path(self.tmp.name) / "gone")}
        self.assertIn("is gone", plan.record_stale(rec, self.root))

    def test_plan_branches_and_legacy_sprints_claim(self):
        cl = plan.claims(self.state, root=self.root, records=[], branches={"core": "branch or PR"},
                         sprints={"c"})
        self.assertTrue(plan.taken(cl, "core"))
        self.assertTrue(plan.taken(cl, "game"))  # t3/sprint-c owns unit game/c
        self.assertNotIn("hub", cl["items"])  # a functions item is not owned by a unit sprint
        self.git("checkout", "-q", "-b", "t3/plan-hub")
        cl = plan.claims(self.state, root=self.root, records=[], branches={}, sprints=set())
        self.assertTrue(cl["items"]["hub"]["mine"])


if __name__ == "__main__":
    unittest.main()
