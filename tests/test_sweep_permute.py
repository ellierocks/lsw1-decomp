"""Near-miss queue, draft splicing and permuter output selection for sweep_permute."""
from pathlib import Path
import tempfile
import unittest

from tools import sweep_permute as sp


class SweepPermuteTest(unittest.TestCase):
    def test_queue_ranks_by_size_times_ratio(self):
        rows = [{"name": "a", "verdict": "MISMATCH", "size": 100, "exact": 95, "target": 100},
                {"name": "b", "verdict": "MISMATCH", "size": 400, "exact": 92, "target": 100},
                {"name": "c", "verdict": "MISMATCH", "size": 900, "exact": 50, "target": 100},
                {"name": "d", "verdict": "MATCH", "size": 900, "exact": 100, "target": 100}]
        self.assertEqual([r["name"] for r in sp.queue(rows, 0.9, 10)], ["b", "a"])
        self.assertEqual([r["name"] for r in sp.queue(rows, 0.9, 1)], ["b"])
        rows.append({"name": "e", "verdict": "MISMATCH", "size": 50, "exact": 99, "target": 100})
        scope = {"functions": ["e"], "stage": ["a", "e"]}  # the plan item first, then its stage
        self.assertEqual([r["name"] for r in sp.queue(rows, 0.9, 10, scope)], ["e", "a"])

    def test_splice_replaces_or_appends(self):
        text = "int g;\nvoid f(void) {\n    g = 1;\n}\nvoid h(void) {}\n"
        self.assertEqual(sp.splice(text, "f", "void f(void) {\n    g = 2;\n}\n"),
                         "int g;\nvoid f(void) {\n    g = 2;\n}\nvoid h(void) {}\n")
        self.assertTrue(sp.splice(text, "k", "void k(void) {}").endswith("void h(void) {}\n\nvoid k(void) {}\n"))
        self.assertEqual(sp.definition(text, "f"), "void f(void) {\n    g = 1;\n}")

    def test_best_output_picks_lowest_score(self):
        with tempfile.TemporaryDirectory() as tmp:
            for name, score in (("output-40-1", 40), ("output-0-2", 0), ("output-9-1", 9)):
                d = Path(tmp) / name
                d.mkdir()
                (d / "score.txt").write_text(f"{score}\n")
                (d / "source.c").write_text("")
            (Path(tmp) / "output-bad").mkdir()
            score, path = sp.best_output(Path(tmp))
            self.assertEqual((score, path.name), (0, "output-0-2"))

    def test_apply_saved_writes_recorded_matches(self):
        from unittest import mock
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp)
            (out / "f").mkdir()
            (out / "f" / "permuted.c").write_text("void f(void) {}\n")
            (out / "permute_results.json").write_text(
                '[{"name": "f", "verdict": "MATCH", "size": 4}, {"name": "g", "verdict": "IMPROVED", "size": 8}]')
            calls = []
            with mock.patch.object(sp.sweep, "OUT", out), mock.patch.object(
                    sp.sweep, "score", lambda n, src, body, apply=False: calls.append((n, src, apply)) or {"verdict": "MATCH"}):
                self.assertEqual(sp.apply_saved([{"name": "f", "source": "src/u.c"}]), 0)
            self.assertEqual(calls, [("f", "src/u.c", True)])
            self.assertIn('"APPLIED"', (out / "permute_results.json").read_text())


if __name__ == "__main__":
    unittest.main()
