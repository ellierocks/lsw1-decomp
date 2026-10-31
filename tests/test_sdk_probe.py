import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import sdk_probe  # noqa: E402
import verify_fn as vf  # noqa: E402


def sym(name, value, size):
    return vf.Symbol(0, name, value, size, 2, 1, 1)


class Table:
    def __init__(self, rows):
        self.entries = {n: vf.SymEntry(".text", a, s, True) for n, a, s in rows}
        self.duplicates = set()


class AnchorTests(unittest.TestCase):
    def test_layout_follows_name_anchor_and_skips_stripped_functions(self):
        table = Table([("fn_a", 0x100, 8), ("OSKnown", 0x108, 12), ("fn_b", 0x114, 16),
                       ("fn_c", 0x124, 4)])
        funcs = [sym("Before", 0, 8), sym("OSKnown", 8, 12), sym("Stripped", 20, 32),
                 sym("After", 52, 16), sym("Last", 68, 4)]
        got = sdk_probe.anchor(funcs, table)
        self.assertEqual(got["OSKnown"], ("OSKnown", "name"))
        self.assertEqual(got["Before"][0], "fn_a")
        self.assertEqual(got["After"][0], "fn_b")
        self.assertEqual(got["Last"][0], "fn_c")
        self.assertNotIn("Stripped", got)

    def test_tiny_layout_slot_needs_compatible_code(self):
        table = Table([("fn_get", 0x100, 8), ("OSKnown", 0x108, 12)])
        funcs = [sym("Getter", 0, 8), sym("Setter", 8, 8), sym("OSKnown", 16, 12)]
        got = sdk_probe.anchor(funcs, table)
        self.assertEqual(got["Setter"][0], "fn_get")
        got = sdk_probe.anchor(funcs, table, None, lambda f, retail: f.name == "Getter")
        self.assertEqual(got["Getter"][0], "fn_get")
        self.assertNotIn("Setter", got)

    def test_oversized_lone_anchor_does_not_crash_outlier_pruning(self):
        table = Table([("OSBig", 0x100, 64)])
        got = sdk_probe.anchor([sym("OSBig", 0, 64), sym("Small", 64, 4)], table)
        self.assertEqual(got["OSBig"], ("OSBig", "name"))
        out = {"Tiny": ("OSBig", "name")}
        retail = [(0x100, "OSBig", 64)]
        sdk_probe.prune_outliers(out, retail, {"OSBig": 0}, 8)
        self.assertEqual(out, {"Tiny": ("OSBig", "name")})

    def test_no_anchor_without_a_named_function(self):
        table = Table([("fn_a", 0x100, 8)])
        self.assertEqual(sdk_probe.anchor([sym("Foo", 0, 8)], table), {})


class DtkLocalNameTests(unittest.TestCase):
    def table(self):
        t = vf.SymbolTable.__new__(vf.SymbolTable)
        t.entries = {"XYNTSC": vf.SymEntry(".data", 0x801F5B40, 0x30, False)}
        t.duplicates = set()
        return t

    def test_suffixed_local_resolves_only_at_its_own_address(self):
        t = self.table()
        self.assertEqual(t.addr("XYNTSC"), 0x801F5B40)
        self.assertEqual(t.addr("XYNTSC_801F5B40"), 0x801F5B40)
        self.assertIsNone(t.addr("XYNTSC_801F5B44"))
        self.assertIsNone(t.addr("OTHER_801F5B40"))
        t.duplicates.add("XYNTSC")
        self.assertIsNone(t.addr("XYNTSC_801F5B40"))


class ClassifyTests(unittest.TestCase):
    def test_reloc_only_differences_are_code_same(self):
        rep = {"verdict": "MISMATCH", "target_insns": 4, "mine_insns": 4,
               "differences": [{"kind": "reloc"}]}
        self.assertEqual(sdk_probe.classify(rep), "CODE_SAME")

    def test_operand_difference_is_mismatch(self):
        rep = {"verdict": "MISMATCH", "target_insns": 4, "mine_insns": 4,
               "differences": [{"kind": "reloc"}, {"kind": "operand"}]}
        self.assertEqual(sdk_probe.classify(rep), "MISMATCH")

    def test_unverified_match(self):
        self.assertEqual(sdk_probe.classify({"verdict": "MATCH (unverified references)"}),
                         "UNVERIFIED")


if __name__ == "__main__":
    unittest.main()
