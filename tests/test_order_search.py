"""Exercise order-block parsing, variant rendering and the bounded, early-stopping search."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest

from tools import order_search as os_

SOURCE = """void f(struct s* p)
{
    /* order: decl */
    /* order: begin */
    p->a = 0;
    p->b = -1;
    if (p->c) { p->d = 1; } else { p->d = 2; }
    p->e = -1;
    /* order: end */
    g(p);
}
"""


def report(verdict, exact, returncode=1):
    diffs = 0 if verdict == "MATCH" else 4 - exact
    return {"verdict": verdict, "score": [0 if verdict == "MATCH" else 1, diffs, 0, -exact, 0],
            "exact_insns": exact, "target_insns": 4, "returncode": returncode}


class ParseTests(unittest.TestCase):
    def test_splits_top_level_statements_and_braced_groups(self):
        t = os_.parse(SOURCE)
        self.assertEqual(len(t.blocks), 1)
        self.assertEqual(t.blocks[0].statements,
                         ["p->a = 0;", "p->b = -1;", "if (p->c) { p->d = 1; } else { p->d = 2; }",
                          "p->e = -1;"])
        self.assertIsNotNone(t.decl)

    def test_comments_strings_and_do_while_stay_inside_one_statement(self):
        stmts = os_.split_statements('x = ";"; /* a; b */ y = 1;\ndo { z(); } while (q);\n')
        self.assertEqual(stmts, ['x = ";";', "/* a; b */ y = 1;", "do { z(); } while (q);"])

    def test_rejects_malformed_markers(self):
        for text in ("/* order: begin */\nx = 1;\n", "/* order: end */\n", "int x;\n",
                     "/* order: begin */\n/* order: end */\n",
                     "/* order: begin */\nx = (1;\n/* order: end */\n"):
            with self.subTest(text=text), self.assertRaises(os_.SearchError):
                os_.parse(text)

    def test_shared_constants_need_two_whole_rhs_uses(self):
        stmts = ["p->a = 0;", "p->b = -1;", "p->c += -1;", "p->d = -1;", "p->e = x == 0;", "p->f = 0x10;"]
        self.assertEqual(os_.shared_constants(stmts), ["-1"])


class RenderTests(unittest.TestCase):
    def test_source_order_without_locals_drops_markers(self):
        t = os_.parse(SOURCE)
        text = os_.render(t, ((0, 1, 2, 3),), ())
        self.assertNotIn("order:", text)
        self.assertIn("    p->a = 0;\n    p->b = -1;\n", text)
        self.assertTrue(text.rstrip().endswith("g(p);\n}"))

    def test_permutation_and_shared_local_at_decl_marker(self):
        t = os_.parse(SOURCE)
        text = os_.render(t, ((3, 0, 2, 1),), ((0, "-1"),))
        self.assertIn("    s32 order_k0_m1 = -1;\n", text)
        body = text.split("order_k0_m1 = -1;\n", 1)[1]
        self.assertLess(body.index("p->e = order_k0_m1;"), body.index("p->a = 0;"))
        self.assertLess(body.index("p->a = 0;"), body.index("p->b = order_k0_m1;"))

    def test_shared_local_wraps_block_without_decl_marker(self):
        t = os_.parse(SOURCE.replace("    /* order: decl */\n", ""))
        text = os_.render(t, ((0, 1, 2, 3),), ((0, "-1"),))
        self.assertIn("    {\n        s32 order_k0_m1 = -1;\n        p->a = 0;\n", text)

    def test_variant_count_is_product_of_permutations_and_locals(self):
        t = os_.parse(SOURCE)
        self.assertEqual(os_.count(t, False), 24)
        self.assertEqual(os_.count(t, True), 48)
        self.assertEqual(len(list(os_.candidates(t, True))), 48)


class SearchTests(unittest.TestCase):
    def test_keeps_best_and_stops_on_strict_match(self):
        t = os_.parse(SOURCE)
        seen = []

        def scorer(text, index):
            seen.append(text)
            if "p->e = -1;\n    p->a = 0;\n    p->b = -1;\n    if" in text:
                return report("MATCH", 4, returncode=0)
            return report("MISMATCH", 2 if "p->e" in text.split("p->a")[0] else 1)

        res = os_.search(t, scorer, const_locals=False, jobs=1, limit=100)
        self.assertEqual(res["best"]["report"]["verdict"], "MATCH")
        self.assertEqual(res["best"]["orders"], ((3, 0, 1, 2),))
        self.assertLess(res["tried"], res["total"])
        self.assertLessEqual(len(seen), res["tried"])

    def test_unverified_match_does_not_stop_search(self):
        t = os_.parse(SOURCE)
        res = os_.search(t, lambda text, i: report("MATCH (unverified references)", 4, returncode=3),
                         const_locals=False, jobs=2, limit=100)
        self.assertEqual(res["tried"], 24)

    def test_compile_errors_are_counted_not_scored(self):
        t = os_.parse(SOURCE)
        res = os_.search(t, lambda text, i: {"verdict": "ERROR", "error": "x"},
                         const_locals=False, jobs=1, limit=100)
        self.assertEqual(res["errors"], 24)
        self.assertIsNone(res["best"])

    def test_refuses_search_over_limit(self):
        t = os_.parse(SOURCE)
        with self.assertRaises(os_.SearchError):
            os_.search(t, lambda text, i: report("MISMATCH", 1), const_locals=True, jobs=1, limit=47)

    def test_dry_run_reports_count(self):
        with tempfile.TemporaryDirectory() as tmp:
            src = Path(tmp) / "c.c"
            src.write_text(SOURCE)
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                self.assertEqual(os_.main(["f", str(src), "--const-locals", "--dry-run"]), 0)
            self.assertIn("48 variants", out.getvalue())


if __name__ == "__main__":
    unittest.main()
