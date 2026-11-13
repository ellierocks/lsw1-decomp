"""Exercise tools/workflow_metrics.py: segments, conversions, hypothesis counts and comparisons."""
import contextlib
import io
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import order_search, score_fn, sprint_bench as sb
from tools import sprint_metrics, workflow_metrics as wm


def score(t, name, verdict, exact=0, target=10, **extra):
    return dict(kind="score", t=t, name=name, verdict=verdict, exact=exact, target=target, **extra)


START = {"kind": "start", "t": 1000, "run_id": "run-a", "model": "m", "item": "i"}


class SegmentTests(unittest.TestCase):
    def test_window_and_capped_active_time(self):
        events = [START, score(1100, "a", "MISMATCH"), score(5000, "a", "MATCH", 10), score(9000, "b", "ERROR")]
        whole = wm.Segment.of("run-a", events)
        self.assertEqual(whole.origin, 1000)
        self.assertEqual(whole.active_seconds(), 100 + 900 + 900)  # two gaps capped at 15 min
        cut = wm.Segment.of("run-a", events, since=4000, until=6000)
        self.assertEqual([e["t"] for e in cut.events], [5000])
        self.assertEqual(cut.origin, 4000)
        self.assertEqual(cut.active_seconds(), 900)

    def test_test_stub_scores_are_not_attempts(self):
        seg = wm.Segment.of("run-a", [START, score(1100, "f", "MATCH", 0, 0), score(1200, "g", "ERROR", target=0)])
        self.assertEqual([e["name"] for e in seg.scores()], ["g"])


class FunctionStatsTests(unittest.TestCase):
    def test_reverification_is_not_a_conversion(self):
        stats = wm.function_stats([score(1, "new", "MISMATCH", 5), score(2, "new", "MATCH", 10),
                                   score(3, "old", "MATCH", 10), score(4, "left", "MISMATCH", 3)])
        self.assertEqual(wm.converted(stats), ["new"])
        self.assertEqual(wm.unfinished(stats), ["left"])

    def test_error_only_function_is_not_unfinished(self):
        stats = wm.function_stats([score(1, "left", "MISMATCH", 3, codegen="x"),
                                   score(2, "broken", "ERROR"), score(3, "broken", "ERROR")])
        self.assertEqual(wm.unfinished(stats), ["left"])

    def test_distinct_candidates_prefers_codegen_hashes(self):
        hashed = wm.function_stats([score(1, "a", "MISMATCH", 5, codegen="x"),
                                    score(2, "a", "MISMATCH", 5, codegen="y"),
                                    score(3, "a", "MISMATCH", 5, codegen="x"),
                                    score(4, "a", "ERROR")])
        self.assertEqual(wm.distinct_candidates(hashed["a"]), (2, True))
        proxy = wm.function_stats([score(1, "a", "MISMATCH", 5, codegen="x"), score(2, "a", "MISMATCH", 6)])
        self.assertEqual(wm.distinct_candidates(proxy["a"]), (2, False))


class PlateauTests(unittest.TestCase):
    def test_plateau_needs_the_floor_and_a_flat_window(self):
        def run(exacts):
            return [score(i, "f", "MISMATCH", x, 50, codegen=f"h{i}") for i, x in enumerate(exacts)]
        self.assertIsNone(sprint_metrics.plateau(run([40] * 20), "f"))  # under the floor
        flat = run(list(range(10, 25)) + [24] * 15)  # best 24 first at candidate 14 of 30
        self.assertEqual(sprint_metrics.plateau(flat, "f"), {"distinct": 30, "best": 24, "target": 50, "since": 15})
        self.assertIsNone(sprint_metrics.plateau(run(list(range(30))), "f"))  # still improving
        dup = flat + [score(99, "f", "MISMATCH", 24, 50, codegen="h0")] * 5 + [score(98, "f", "ERROR")]
        self.assertEqual(sprint_metrics.plateau(dup, "f")["distinct"], 30)  # repeats and errors do not count
        self.assertIsNone(sprint_metrics.plateau(flat + [score(100, "f", "MATCH", 50, 50, codegen="m")], "f"))


class MeasureTests(unittest.TestCase):
    def test_cohort_metrics(self):
        events = [START,
                  score(1060, "a", "ERROR", source="variant_search"),
                  score(1120, "a", "MISMATCH", 6, source="manual", seconds=2.0),
                  score(1180, "a", "MATCH", 10, source="manual", seconds=4.0),
                  score(1240, "b", "MISMATCH", 2, source="manual", seconds=3.0),
                  {"kind": "time", "t": 1300, "label": "finish-build", "seconds": 7.5}]
        later = [dict(START, run_id="run-b", t=2000), score(2100, "b", "MATCH", 10)]
        runs = {"run-a": events, "run-b": later}
        m = wm.measure([wm.Segment.of("run-a", events)], runs)
        self.assertEqual((m["attempts"], m["compile_errors"], m["compile_error_rate"]), (4, 1, 25.0))
        self.assertEqual(m["by_source"]["variant_search"], {"attempts": 1, "errors": 1, "error_rate": 1.0})
        self.assertEqual((m["converted_names"], m["converted_bytes"]), (["a"], 40))
        self.assertEqual(m["exact_gain"], 4)
        self.assertEqual(m["first_compile_seconds"], 120)
        self.assertEqual((m["unfinished"], m["recovered"], m["park_recovery_rate"]), (1, 1, 100.0))
        self.assertEqual(m["score_seconds"], 3.0)
        self.assertEqual(m["build_seconds"], 7.5)
        self.assertEqual(m["matched_bytes_per_hour"], round(40 / (300 / 3600)))

    def test_function_left_by_two_segments_counts_once(self):
        events = [START, score(1100, "b", "MISMATCH", 2), score(1200, "b", "MISMATCH", 3)]
        segs = [wm.Segment.of("run-a", events, until=1150), wm.Segment.of("run-a", events, since=1150)]
        self.assertEqual(wm.measure(segs, {"run-a": events})["unfinished"], 1)

    def test_unknown_metrics_stay_unknown(self):
        m = wm.measure([wm.Segment.of("run-a", [START])])
        self.assertIsNone(m["compile_error_rate"])
        self.assertIsNone(m["score_seconds"])
        self.assertIsNone(m["park_recovery_rate"])


class ComparisonTests(unittest.TestCase):
    def test_multiplier_direction(self):
        self.assertEqual(wm.multiplier(wm.BY_KEY["matched_bytes_per_hour"], 100, 400), 4)
        self.assertEqual(wm.multiplier(wm.BY_KEY["compile_error_rate"], 60.0, 3.0), 20)
        self.assertIsNone(wm.multiplier(wm.BY_KEY["compile_error_rate"], 60.0, None))
        self.assertIsNone(wm.multiplier(wm.BY_KEY["matched_bytes_per_hour"], 0, 5))
        self.assertEqual((wm.fmt_mult(39.1), wm.fmt_mult(4.26), wm.fmt_mult(None)), ("39×", "4.3×", "–"))

    def test_markdown_and_svg(self):
        rows = wm.compare_rows({"compile_error_rate": 69.4, "matched_bytes_per_hour": 374},
                               {"compile_error_rate": 4.1, "matched_bytes_per_hour": 14481},
                               ["matched_bytes_per_hour", "compile_error_rate"])
        text = wm.markdown("T", rows)
        self.assertIn("374 B/h → **14,481 B/h** | 39×", text)
        self.assertIn("69.4% → **4.1%** | 17×", text)
        card = wm.svg("A & B", "sub", rows)
        self.assertTrue(card.startswith("<svg"))
        self.assertIn("A &amp; B", card)
        self.assertIn("39×", card)


class CliTests(unittest.TestCase):
    def test_cohorts_compare_and_dedupe(self):
        with tempfile.TemporaryDirectory() as tmp:
            d = Path(tmp)
            short = [START, score(1100, "a", "MISMATCH", 5)]
            full = short + [score(1200, "a", "MATCH", 10)]
            (d / "short.jsonl").write_text("".join(json.dumps(e) + "\n" for e in short))
            (d / "full.jsonl").write_text("".join(json.dumps(e) + "\n" for e in full))
            runs = wm.load_runs([d / "short.jsonl", d / "full.jsonl"])
            self.assertEqual(len(runs["run-a"]), 3)
            (d / "c.toml").write_text(
                '[[cohort]]\nname = "x"\nsegments = [{ run = "run-a", until = 1150 }]\n'
                '[[cohort]]\nname = "y"\nsegments = [{ run = "run-a" }, { run = "gone" }]\n'
                '[[comparison]]\ntitle = "t"\nbefore = "x"\nafter = "y"\nmetrics = ["exact_gain_per_hour"]\n')
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                code = wm.main(["--config", str(d / "c.toml"), "--logs", str(d / "short.jsonl"),
                                "--logs", str(d / "full.jsonl"), "compare", "--json", "--svg", str(d / "card.svg")])
            self.assertEqual(code, 0)
            rows = json.loads(out.getvalue())[0]["rows"]
            self.assertEqual(rows[0]["key"], "exact_gain_per_hour")
            self.assertEqual(rows[0]["before"], 0)
            self.assertTrue((d / "card.svg").is_file())


class LoggingTests(unittest.TestCase):
    def test_bench_note_records_source_latency_and_codegen(self):
        fake = mock.Mock()
        with mock.patch.dict(sys.modules, {"sprint_bench": fake}), mock.patch.dict(os.environ, {}, clear=False):
            os.environ.pop(score_fn.BENCH_SOURCE_ENV, None)
            score_fn.bench_note("a", "MISMATCH", 3, 4, seconds=1.234, codegen="abc")
            os.environ[score_fn.BENCH_SOURCE_ENV] = "variant_search"
            score_fn.bench_note("a", "ERROR")
        first, second = fake.append.call_args_list
        self.assertEqual(first.kwargs, dict(name="a", verdict="MISMATCH", exact=3, target=4,
                                            source="manual", seconds=1.23, codegen="abc"))
        self.assertEqual(second.kwargs["source"], "variant_search")
        self.assertNotIn("codegen", second.kwargs)

    def test_codegen_hash_is_none_for_unreadable_objects(self):
        self.assertIsNone(score_fn.codegen_hash(Path("/nonexistent.o"), "f"))

    def test_order_search_tags_its_scores(self):
        done = mock.Mock(returncode=1, stdout='{"verdict": "MISMATCH"}', stderr="")
        with tempfile.TemporaryDirectory() as tmp, \
             mock.patch.object(order_search.subprocess, "run", return_value=done) as run:
            order_search.score_source("f", "void f(void) {}", [], Path(tmp), 0, source="variant_search")
        self.assertEqual(run.call_args.kwargs["env"]["LSW1_BENCH_SOURCE"], "variant_search")

    def test_archive_keeps_the_longer_copy(self):
        with tempfile.TemporaryDirectory() as tmp:
            d = Path(tmp)
            log = d / "sprint_bench.jsonl"
            log.write_text(json.dumps(START) + "\n")
            out = sb.archive_events(log, d / "events")
            self.assertEqual(out, d / "events" / "run-a.jsonl")
            out.write_text(out.read_text() + json.dumps(score(1100, "a", "MATCH")) + "\n")
            sb.archive_events(log, d / "events")
            self.assertEqual(len(out.read_text().splitlines()), 2)
            self.assertIsNone(sb.archive_events(d / "missing.jsonl", d / "events"))


if __name__ == "__main__":
    unittest.main()
