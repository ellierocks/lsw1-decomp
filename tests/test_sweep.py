"""The unattended m2c sweep's text rewrites: macro expansion, cror folding, derefs, decls."""
import unittest

from tools import sweep


class SweepRewriteTest(unittest.TestCase):
    def test_split_args_nests(self):
        args, end = sweep.split_args("M(a, f(b, c), (d))x", 1)
        self.assertEqual(args, ["a", "f(b, c)", "(d)"])
        self.assertEqual(end, 18)

    def test_expand_macros(self):
        out = sweep.expand_macros("M2C_UNK32 x = M2C_FIELD(M2C_FIELD(p, s8 **, 4), s32 *, 0x10);")
        self.assertEqual(out, "s32 x = (*(s32 *)((s8 *)((*(s8 **)((s8 *)(p) + (4)))) + (0x10)));")

    def test_fold_cror(self):
        asm = ("\tfcmpu cr0, f0, f31\n\tcror un, eq, lt\n\tbns .L_1\n"
               "\tfcmpu cr0, f1, f2\n\tcror un, eq, gt\n\tbsolr\n")
        out = sweep.fold_cror(asm)
        self.assertNotIn("cror", out)
        self.assertIn("\tbgt .L_1\n", out)
        self.assertIn("\tbgelr\n", out)

    def test_fold_cror_leaves_other_patterns(self):
        asm = "\tcror un, eq, lt\n\tmfcr r0\n\tbns .L_1\n"
        self.assertEqual(sweep.fold_cror(asm), asm)

    def test_type_derefs(self):
        out = sweep.type_derefs("x = *(a + 4); y = (*(s32 *)(b)); z = a * (b);")
        self.assertEqual(out, "x = *(s32 *)(a + 4); y = (*(s32 *)(b)); z = a * (b);")

    def test_drop_decl(self):
        body = "void g(s32);\nextern s32 lbl_X;\nvoid f(void) {\n    g(lbl_X);\n}\n"
        self.assertEqual(sweep.drop_decl(body, "g", "f"), "extern s32 lbl_X;\nvoid f(void) {\n    g(lbl_X);\n}\n")
        self.assertIsNone(sweep.drop_decl(body, "f", "f"))
        self.assertIsNone(sweep.drop_decl(body, "h", "f"))


if __name__ == "__main__":
    unittest.main()
