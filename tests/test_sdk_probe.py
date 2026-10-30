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

    def test_no_anchor_without_a_named_function(self):
        table = Table([("fn_a", 0x100, 8)])
        self.assertEqual(sdk_probe.anchor([sym("Foo", 0, 8)], table), {})


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
