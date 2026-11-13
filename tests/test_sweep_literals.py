import unittest
from tools import sweep  # noqa: F401  (puts tools/ on sys.path)
from sweep_repairs.literals import c_literal, float_literals

FLOATS = {"lbl_A": -1.0, "lbl_B": 0.2, "lbl_C": 10430.378, "lbl_T": 1.0}


class LiteralTest(unittest.TestCase):
    def test_shortest_round_trip(self):
        self.assertEqual(c_literal(1.0), "1.0f")
        self.assertEqual(c_literal(-1.0), "-1.0f")
        self.assertEqual(c_literal(0.2), "0.2f")
        self.assertEqual(c_literal(10430.378), "10430.378f")
        self.assertEqual(c_literal(0.0), "0.0f")
        self.assertEqual(c_literal(3.4028235e38), "3.4028234e+38f")
        self.assertEqual(c_literal(-3.4028235e38), "-3.4028234e+38f")
        self.assertIsNone(c_literal(1e39))
        self.assertIsNone(c_literal(float("nan")))

    def test_all_then_each(self):
        body = ("extern f32 lbl_A[];\nextern const f32 lbl_B[];\n"
                "void f(void) {\n    g(lbl_A[0], lbl_B[0]);\n}\n")
        alts = float_literals(body, floats=FLOATS)
        self.assertEqual(alts[0], "void f(void) {\n    g(-1.0f, 0.2f);\n}\n")
        self.assertEqual(len(alts), 3)
        self.assertIn("g(-1.0f, lbl_B[0])", alts[1])
        self.assertIn("g(lbl_A[0], 0.2f)", alts[2])

    def test_tables_and_unknown_labels_stay(self):
        body = ("extern f32 lbl_T[];\nextern f32 lbl_U[];\n"
                "void f(void) {\n    g(lbl_T[1], &lbl_T, lbl_U[0]);\n}\n")
        self.assertEqual(float_literals(body, floats=FLOATS), [])


if __name__ == '__main__':
    unittest.main()
