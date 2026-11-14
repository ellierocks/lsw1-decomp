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

    def test_address_taken_locals_get_a_volatile_option(self):
        src = "void h(void)\n{\n    s32 a;\n    s32 b;\n    s32 c;\n    a = 1;\n    g(&b, &a, c);\n}\n"
        fn = vs.find_function(src, "h")
        dims, lines = vs.build_dimensions(fn)
        vol = {d.name: d for d in dims if d.name.startswith("volatile-local")}
        self.assertEqual(sorted(vol), ["volatile-local@1", "volatile-local@2"])
        self.assertEqual(vol["volatile-local@1"].options[1], {1: "    volatile s32 a;"})

    def test_initialized_locals_join_the_decl_order_and_long_lists_are_capped(self):
        decls = "\n".join(f"    s32 v{i} = {i};" for i in range(vs.MAX_DECLS + 2))
        fn = vs.find_function(f"void h(void)\n{{\n{decls}\n    g();\n}}\n", "h")
        dims, _ = vs.build_dimensions(fn)
        order = [d for d in dims if d.kind == "order"]
        self.assertEqual(len(order), 1)
        self.assertEqual(len(order[0].options[0]), vs.MAX_DECLS)

    def test_assignment_after_texture_locals_is_not_a_declaration(self):
        src = ("void NuMtlRender(s32 fh)\n{\n"
               "    s32 count;\n    s32 i;\n    s32 k;\n"
               "    struct nughgtex_s *tex;\n"
               "    count = NuFileReadInt(fh);\n    use(tex, count);\n}\n")
        fn = vs.find_function(src, "NuMtlRender")
        dims, lines = vs.build_dimensions(fn)
        self.assertEqual(vs.leading_decls(lines), (1, 5))
        for choice in vs.combos(dims, 1):
            out = vs.render(fn, dims, lines, choice)
            self.assertLess(out.index("*tex;"), out.index("count = NuFileReadInt(fh);"))

    def test_declarations_require_a_type_name_separator(self):
        for line in ("    count = read();", "    value = 0;", "    texture = NULL;"):
            with self.subTest(line=line):
                self.assertIsNone(vs.DECL_RE.match(line))
        for line in ("    s32 count = read();", "    s32*p;", "    struct tex *p;"):
            with self.subTest(line=line):
                self.assertIsNotNone(vs.DECL_RE.match(line))

    def test_combos_try_fewest_changes_first_and_respect_the_cap(self):
        fn = vs.find_function(SOURCE, "f")
        dims, _ = vs.build_dimensions(fn)
        first = next(iter(vs.combos(dims, 2)))
        self.assertEqual(first, (0,) * len(dims))
        self.assertEqual(sum(1 for _ in vs.combos(dims, 2)), vs.count(dims, 2))
        self.assertLess(vs.count(dims, 1), vs.count(dims, 2))


IF_SOURCE = """void k(s32 a)
{
    if (a) {
        g(1);
    } else if (a > 2) {
        g(2);
    } else {
        g(3);
    }
    if (a) {
        if (a == 4) {
            g(4);
        }
    }
    if (a) g(5);
}
"""


class DoWhileTests(unittest.TestCase):
    def test_braced_ifs_and_else_chains_are_found(self):
        lines = vs.find_function(IF_SOURCE, "k").body.split("\n")
        blocks = vs.if_blocks(lines)
        self.assertEqual([(lines[i].strip(), lines[j].strip()) for i, j in blocks],
                         [("if (a) {", "}"), ("if (a) {", "}"), ("if (a == 4) {", "}")])
        self.assertEqual(blocks[0][1] - blocks[0][0], 6)

    def test_wrapping_puts_the_whole_chain_in_a_do_while(self):
        fn = vs.find_function(IF_SOURCE, "k")
        dims, lines = vs.build_dimensions(fn)
        i = next(n for n, d in enumerate(dims) if d.name.startswith("do-while"))
        choice = [0] * len(dims)
        choice[i] = 1
        out = vs.render(fn, dims, lines, tuple(choice))
        self.assertIn("    do {\n        if (a) {\n        g(1);", out)
        self.assertIn("        g(3);\n        }\n    } while (0);\n    if (a) {", out)
        self.assertEqual(out.count("while (0)"), 1)

    def test_binary_dimensions_come_before_the_declaration_order(self):
        fn = vs.find_function("void k(s32 a)\n{\n    s32 x;\n    s32 y;\n" + IF_SOURCE.split("{\n", 1)[1], "k")
        dims, _ = vs.build_dimensions(fn)
        kinds = [d.kind for d in dims]
        self.assertEqual(kinds[-1], "order")
        self.assertEqual(kinds.count("order"), 1)
        self.assertTrue(any(d.name.startswith("do-while") for d in dims))


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

    def test_searches_over_the_limit_are_truncated_not_refused(self):
        result = vs.search("f", SOURCE, lambda t, i: report("MISMATCH", 0), jobs=1, limit=2)
        self.assertTrue(result["truncated"])
        self.assertEqual(result["tried"], 2)

    def test_compile_errors_are_counted_not_scored(self):
        calls = {"n": 0}

        def scorer(text, index):
            calls["n"] += 1
            return {"verdict": "ERROR", "error": "x"} if calls["n"] == 2 else report("MISMATCH", 1)

        result = vs.search("f", SOURCE, scorer, jobs=1, limit=1000)
        self.assertEqual(result["errors"], 1)

    def test_rewrite_that_fails_alone_is_not_combined(self):
        scored = []

        def scorer(text, index):
            scored.append(text)
            return {"verdict": "ERROR", "error": "e"} if "u32 a" in text else report("MISMATCH", 1)

        result = vs.search("f", SOURCE, scorer, jobs=1, limit=1000)
        self.assertEqual(sum("u32 a" in t for t in scored), 1)
        self.assertGreater(result["pruned"], 0)
        self.assertEqual(result["tried"] + result["pruned"], result["total"])

    def test_uncompilable_seed_is_refused(self):
        with self.assertRaisesRegex(vs.VariantError, "does not compile"):
            vs.search("f", SOURCE, lambda t, i: {"verdict": "ERROR", "error": "e"}, jobs=2, limit=100)

    def test_default_case_dimension(self):
        # An extra label on the default arm rebalances GCC's case tree (fn_80063F80).
        lines = ["    switch (x) {", "    case 2:", "        y = 1;", "        break;", "    case 7:",
                 "        break;", "    default:", "        y = 0;", "    }"]
        dims = vs.default_case_dims(lines)
        self.assertEqual(len(dims), 1)
        self.assertEqual(dims[0].options[1], {6: "    case 0:\n    default:"})
        values = [int(o[6].split()[1].rstrip(":")) for o in dims[0].options[1:]]
        self.assertEqual(values, [0, 1, -1, 8, 9, 3, 4, 5])
        self.assertEqual(vs.default_case_dims(["    default:"]), [])


if __name__ == "__main__":
    unittest.main()
