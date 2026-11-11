"""Exercise the sprint benchmark event log and summaries."""
import json
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import sprint_bench as sb


class BenchTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)
        self.events = self.dir / "work" / "sprint_bench.jsonl"
        self.patches = [mock.patch.object(sb, "EVENTS", self.events),
                        mock.patch.object(sb, "ROOT", self.dir),
                        mock.patch.dict("os.environ", {"LSW1_CACHE": str(self.dir / "cache")})]
        for p in self.patches:
            p.start()

    def tearDown(self):
        for p in self.patches:
            p.stop()
        self.tmp.cleanup()

    def test_events_before_start_are_dropped(self):
        self.assertIsNone(sb.append("score", path=self.events, name="f", verdict="MATCH"))
        self.assertFalse(self.events.exists())

    def test_startup_split_and_throughput(self):
        git_file = self.dir / ".git"
        git_file.write_text("gitdir: x\n")
        import os
        os.utime(git_file, (1030, 1030))
        sb.start("model-x", "run1", launched_at=1000, now=1100)
        sb.append("time", path=self.events, now=1200, label="build", seconds=90, returncode=0)
        sb.append("score", path=self.events, now=1300, name="a", verdict="MISMATCH", exact=3, target=4)
        sb.append("score", path=self.events, now=1400, name="a", verdict="MATCH", exact=4, target=4)
        sb.append("score", path=self.events, now=1500, name="b", verdict="ERROR")
        s = sb.summarize(sb.load(self.events))
        self.assertEqual(s["startup"], {"worktree_seconds": 30.0, "agent_boot_seconds": 70.0,
                                        "launch_to_start_seconds": 100.0})
        self.assertEqual(s["steps_seconds"], {"build": 90})
        self.assertEqual((s["attempts"], s["compile_errors"], s["attempt_matches"]), (3, 1, 1))
        self.assertEqual(s["first_score_seconds"], 200.0)
        self.assertEqual(s["first_match_seconds"], 300.0)

    def test_finish_writes_a_comparable_summary(self):
        sb.start("model-x", "run2", launched_at=None, now=1000)
        out = sb.finish({"unit": "u", "verified": 6, "parked": 1, "active_seconds": 1800})
        self.assertTrue(out.is_file())
        saved = json.loads(out.read_text())
        self.assertEqual(saved["verified_per_active_hour"], 12.0)
        rows = sb.compare(out.parent)
        self.assertEqual(rows[0]["model"], "model-x")

    def test_lane_bytes_and_open_run(self):
        self.assertIsNone(sb.current_start())
        sb.start("m", "run3", launched_at=None, now=1000, lane="near-misses", unit="u")
        self.assertEqual(sb.current_start()["lane"], "near-misses")
        out = sb.finish({"verified": 2, "verified_bytes": 400, "active_seconds": 1800})
        self.assertIsNone(sb.current_start())
        row = sb.compare(out.parent)[0]
        self.assertEqual((row["lane"], row["unit"], row["verified_bytes_per_active_hour"]), ("near-misses", "u", 800))
        (out.parent / "old.json").write_text(json.dumps({"run_id": "old", "unit": "v", "started_at": 1}))
        (out.parent / "bad.json").write_text("{")
        rows = sb.compare(out.parent)
        self.assertEqual([r["run_id"] for r in rows], ["old", "run3"])
        self.assertIsNone(rows[0]["lane"])


if __name__ == "__main__":
    unittest.main()
