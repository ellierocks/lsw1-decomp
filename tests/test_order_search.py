"""Exercise order-block parsing, variant rendering and the bounded, early-stopping search."""
import contextlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest import mock

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


class CompilerSettingsTests(unittest.TestCase):
    def test_sdk_search_keeps_original_unit_compiler_and_quoted_flags(self):
        from tools import score_fn
        with mock.patch.object(score_fn, "unit_mw_settings",
                               return_value=("1.2.5", ["-lang=c", "-i include/dolphin",
                                                       "-pragma cats off"])) as settings:
            args = os_.compiler_args("sdk", Path("src/dolphin/hvqm4/hvqm4dec.c"))
        settings.assert_called_once_with(Path("src/dolphin/hvqm4/hvqm4dec.c"))
        self.assertEqual(args[:4], ["--compiler", "sdk", "--mw-version", "1.2.5"])
        import shlex
        self.assertEqual(shlex.split(args[5]), ["-lang=c", "-i include/dolphin", "-pragma cats off"])

    def test_other_compilers_keep_their_existing_defaults(self):
        self.assertEqual(os_.compiler_args(None, Path("x.c")), [])
        self.assertEqual(os_.compiler_args("prodg", Path("x.c")), ["--compiler", "prodg"])


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
        res = os_.search(t, lambda text, i: report("MISMATCH", 1) if i == 0 else
                         {"verdict": "ERROR", "error": "c.c:9:3: error: 'q' undeclared"},
                         const_locals=False, jobs=1, limit=100)
        self.assertEqual((res["tried"], res["errors"]), (24, 23))
        self.assertEqual(res["best"]["orders"], ((0, 1, 2, 3),))
        self.assertIn("23/24 variants did not compile", res["warning"])
        self.assertIn("(23x): error: 'q' undeclared", res["warning"])

    def test_uncompilable_seed_is_refused(self):
        # nuportal 2026-11-12: 684 of 724 NuAnimCurveEval scores were ERROR (workflow gap 11).
        t = os_.parse(SOURCE)
        calls = []
        with self.assertRaisesRegex(os_.SearchError, "does not compile.*error: bad type"):
            os_.search(t, lambda text, i: calls.append(i) or {"verdict": "ERROR", "error": "/tmp/x/c.c:3:5: error: bad type"},
                       const_locals=False, jobs=4, limit=100)
        self.assertEqual(calls, [0])
        with self.assertRaisesRegex(os_.SearchError, "does not compile"):
            os_.climb(t, lambda text, i: {"verdict": "ERROR", "error": "e"}, jobs=1, budget=10)

    def test_a_run_of_errors_stops_the_search(self):
        t = os_.parse(SOURCE)
        scorer = lambda text, i: report("MISMATCH", 1) if i == 0 else {"verdict": "ERROR", "error": "e"}
        res = os_.search(t, scorer, const_locals=False, jobs=1, limit=100, max_error_run=5)
        self.assertEqual(res["stopped"], "5 compile errors in a row")
        self.assertLess(res["tried"], 24)
        res = os_.search(t, scorer, const_locals=False, jobs=1, limit=100, max_error_run=0)
        self.assertIsNone(res["stopped"])
        res = os_.climb(t, scorer, jobs=1, budget=100, max_error_run=5)
        self.assertEqual(res["stopped"], "5 compile errors in a row")

    def test_diagnostic_strips_location(self):
        self.assertEqual(os_.diagnostic({"error": "In function f:\n/t/c.c:12: error: parse error before `}'\n"}),
                         "error: parse error before `}'")
        self.assertEqual(os_.diagnostic({}), "")

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

    def test_body_is_spliced_into_source(self):
        unit = "int g(void);\nvoid f(struct s* p)\n{\n    p->a = 0;\n}\nint h(void) { return 1; }\n"
        with tempfile.TemporaryDirectory() as tmp:
            src = Path(tmp) / "u.c"
            src.write_text(unit)
            body = Path(tmp) / "try.c"
            body.write_text(SOURCE)
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                self.assertEqual(os_.main(["f", str(src), "--body", str(body), "--const-locals",
                                           "--dry-run"]), 0)
            self.assertIn("48 variants", out.getvalue())
            self.assertEqual(src.read_text(), unit)


BIG = "void f(int* x)\n{\n    /* order: begin */\n" + "".join(
    f"    x[{k}] = {k};\n" for k in range(9)) + "    /* order: end */\n}\n"
GOAL = [4, 0, 8, 2, 6, 1, 7, 3, 5]


def inversions_scorer(text, index):
    """Score by the number of pairs out of GOAL's relative order; 0 is a strict MATCH."""
    pos = [text.index(f"x[{k}] =") for k in range(9)]
    order = sorted(range(9), key=lambda k: pos[k])
    rank = {k: i for i, k in enumerate(GOAL)}
    inv = sum(rank[a] > rank[b] for i, a in enumerate(order) for b in order[i + 1:])
    if inv == 0:
        return report("MATCH", 4, returncode=0)
    return {"verdict": "MISMATCH", "score": [1, inv, 0, 0, 0], "exact_insns": 0, "target_insns": 4,
            "returncode": 1}


class ClimbTests(unittest.TestCase):
    def test_moves_are_distinct_single_relocations(self):
        start = ((0, 1, 2),)
        got = list(os_.moves(start))
        self.assertEqual(len(got), len(set(got)))
        self.assertEqual(len(got), 4)   # 3*2 moves; two adjacent swaps coincide
        self.assertNotIn(start, got)

    def test_climb_finds_order_beyond_exhaustive_limit(self):
        t = os_.parse(BIG)
        with self.assertRaises(os_.SearchError):
            os_.search(t, inversions_scorer, const_locals=False, jobs=1, limit=os_.DEFAULT_LIMIT)
        res = os_.climb(t, inversions_scorer, jobs=4, budget=3000)
        self.assertEqual(res["best"]["report"]["verdict"], "MATCH")
        self.assertEqual(res["best"]["orders"], (tuple(GOAL),))
        self.assertLess(res["tried"], 3000)

    def test_climb_respects_budget_and_never_rescores(self):
        t = os_.parse(BIG)
        texts = []

        def flat(text, index):
            texts.append(text)
            return report("MISMATCH", 1)

        res = os_.climb(t, flat, jobs=2, budget=150, seed=1)
        self.assertEqual(res["tried"], 150)
        self.assertEqual(len(texts), len(set(texts)))

    def test_climb_rejects_const_locals(self):
        with tempfile.TemporaryDirectory() as tmp:
            src = Path(tmp) / "c.c"
            src.write_text(SOURCE)
            with contextlib.redirect_stderr(io.StringIO()), self.assertRaises(SystemExit):
                os_.main(["f", str(src), "--climb", "--const-locals"])


if __name__ == "__main__":
    unittest.main()
