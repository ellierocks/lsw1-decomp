"""Exercise variant discovery, rendering and the bounded, early-stopping search."""
import unittest

from tools import variant_search as vs

SOURCE = """#include "types.h"
s32 other(s32 a)
{
    return a;
}

void f(s32 a, struct s* p)
{
    s32 x;
    s32 y;
    x = p->a ? p->b : p->c;
    y = p->d ? p->e : p->f;
    g(x, y, a);
}
"""


def report(verdict, exact):
    return {"verdict": verdict, "score": [0 if verdict == "MATCH" else 1, 0, 0, -exact, 0],
            "exact_insns": exact, "target_insns": 4, "returncode": 0 if verdict == "MATCH" else 1}


class DiscoveryTests(unittest.TestCase):
    def test_finds_only_the_named_function(self):
        fn = vs.find_function(SOURCE, "f")
        self.assertIn("g(x, y, a);", fn.body)
        self.assertNotIn("return a;", fn.body)
        self.assertEqual(fn.signature, "s32 a, struct s* p")

    def test_missing_function_is_an_error(self):
        with self.assertRaises(vs.VariantError):
            vs.find_function(SOURCE, "nope")

    def test_dimensions_cover_decl_order_ternaries_and_signs(self):
        fn = vs.find_function(SOURCE, "f")
        dims, _ = vs.build_dimensions(fn)
        dims += vs.sign_param_dims(fn)
        names = [d.name for d in dims]
        self.assertTrue(any(n.startswith("decl-order") for n in names))
        self.assertIn("ternary-all", names)
        self.assertTrue(any(n.startswith("sign-param") for n in names))

    def test_combos_try_fewest_changes_first_and_respect_the_cap(self):
        fn = vs.find_function(SOURCE, "f")
        dims, _ = vs.build_dimensions(fn)
        first = next(iter(vs.combos(dims, 2)))
        self.assertEqual(first, (0,) * len(dims))
        self.assertEqual(sum(1 for _ in vs.combos(dims, 2)), vs.count(dims, 2))
        self.assertLess(vs.count(dims, 1), vs.count(dims, 2))


class RenderTests(unittest.TestCase):
    def setUp(self):
        self.fn = vs.find_function(SOURCE, "f")
        self.dims, self.lines = vs.build_dimensions(self.fn)
        self.dims += vs.sign_param_dims(self.fn)

    def pick(self, prefix):
        return next(i for i, d in enumerate(self.dims) if d.name.startswith(prefix))

    def test_original_choice_round_trips(self):
        self.assertEqual(vs.render(self.fn, self.dims, self.lines, (0,) * len(self.dims)), SOURCE)

    def test_ternary_all_becomes_if_else_in_both_places(self):
        choice = [0] * len(self.dims)
        choice[self.pick("ternary-all")] = 1
        out = vs.render(self.fn, self.dims, self.lines, tuple(choice))
        self.assertIn("if (p->a) {\n        x = p->b;\n    } else {\n        x = p->c;", out)
        self.assertIn("y = p->e;", out)
        self.assertNotIn("?", out)

    def test_declaration_permutation_reorders_locals_only(self):
        i = self.pick("decl-order")
        k = next(k for k, perm in enumerate(self.dims[i].options) if perm == (1, 0))
        choice = [0] * len(self.dims)
        choice[i] = k
        out = vs.render(self.fn, self.dims, self.lines, tuple(choice))
        self.assertLess(out.index("s32 y;"), out.index("s32 x;"))

    def test_param_sign_flip_keeps_offsets(self):
        i = self.pick("sign-param")
        choice = [0] * len(self.dims)
        choice[i] = 1
        out = vs.render(self.fn, self.dims, self.lines, tuple(choice))
        self.assertIn("void f(u32 a, struct s* p)", out)


class SearchTests(unittest.TestCase):
    def test_stops_at_first_match_and_reports_changes(self):
        seen = []

        def scorer(text, index):
            seen.append(text)
            return report("MATCH", 4) if "} else {" in text else report("MISMATCH", 2)

        result = vs.search("f", SOURCE, scorer, jobs=1, limit=1000)
        self.assertEqual(result["best"]["report"]["verdict"], "MATCH")
        self.assertTrue(result["best"]["changes"][0].startswith("ternary"))
        self.assertLess(result["tried"], result["total"])

    def test_refuses_searches_over_the_limit(self):
        with self.assertRaises(vs.VariantError):
            vs.search("f", SOURCE, lambda t, i: report("MISMATCH", 0), jobs=1, limit=2)

    def test_compile_errors_are_counted_not_scored(self):
        calls = {"n": 0}

        def scorer(text, index):
            calls["n"] += 1
            return {"verdict": "ERROR", "error": "x"} if calls["n"] == 2 else report("MISMATCH", 1)

        result = vs.search("f", SOURCE, scorer, jobs=1, limit=1000)
        self.assertEqual(result["errors"], 1)


if __name__ == "__main__":
    unittest.main()
