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

    def test_expand_bitwise_struct_local(self):
        out = sweep.expand_macros("void f(void) {\n    struct nuvec_s sp8;\n    s32 r;\n"
                                  "    g(M2C_BITWISE(f32, sp8), M2C_BITWISE(f32, r));\n}\n")
        self.assertIn("g((*(f32 *)&(sp8)), ((f32)(r)));", out)

    def test_better_prefers_match_then_exact(self):
        self.assertTrue(sweep.better({"verdict": "MATCH", "exact": 3}, {"verdict": "MISMATCH", "exact": 9}))
        self.assertTrue(sweep.better({"verdict": "MISMATCH", "exact": 9}, {"verdict": "MISMATCH", "exact": 8}))
        self.assertFalse(sweep.better({"verdict": "COMPILE-ERROR"}, {"verdict": "UNHANDLED"}))

    def test_store_in_arms(self):
        body = ("void f(void) {\n    s32 var_r0;\n\n    var_r0 = 0xA;\n    if (g() != 0) {\n"
                "        var_r0 = 8;\n    }\n    lbl_X = var_r0;\n}\n")
        self.assertEqual(sweep.store_in_arms(body),
                         "void f(void) {\n\n    if (g() != 0) {\n        lbl_X = 8;\n    } else {\n"
                         "        lbl_X = 0xA;\n    }\n}\n")
        self.assertEqual(sweep.store_in_arms("x = 1;\n"), "x = 1;\n")

    def test_const_rodata(self):
        sweep._RODATA = {"lbl_801A4C2C"}
        try:
            out = sweep.const_rodata("extern f32 lbl_801A4C2C[];\nextern f32 lbl_8040A6BC[];\n"
                                     "extern const f32 lbl_801A4C2C[];\n")
        finally:
            sweep._RODATA = None
        self.assertEqual(out, "extern const f32 lbl_801A4C2C[];\nextern f32 lbl_8040A6BC[];\n"
                              "extern const f32 lbl_801A4C2C[];\n")

    def test_fold_cror(self):
        asm = ("\tfcmpu cr0, f0, f31\n\tcror un, eq, lt\n\tbns .L_1\n"
               "\tfcmpu cr0, f1, f2\n\tcror un, eq, gt\n\tbsolr\n")
        out = sweep.fold_cror(asm)
        self.assertIn("cror cr0un, cr0eq, cr0lt", out)
        self.assertIn("\tbns .L_1\n", out)
        self.assertIn("\tbsolr\n", out)

    def test_fold_cror_leaves_other_patterns(self):
        asm = "\tcror un, eq, lt\n\tmfcr r0\n\tbns .L_1\n"
        self.assertEqual(sweep.fold_cror(asm), asm.replace("un, eq, lt", "cr0un, cr0eq, cr0lt"))

    def test_type_derefs(self):
        out = sweep.type_derefs("x = *(a + 4); y = (*(s32 *)(b)); z = a * (b);")
        self.assertEqual(out, "x = *(s32 *)(a + 4); y = (*(s32 *)(b)); z = a * (b);")

    def test_drop_decl(self):
        body = "void g(s32);\nextern s32 lbl_X;\nvoid f(void) {\n    g(lbl_X);\n}\n"
        self.assertEqual(sweep.drop_decl(body, "g", "f"), "extern s32 lbl_X;\nvoid f(void) {\n    g(lbl_X);\n}\n")
        self.assertIsNone(sweep.drop_decl(body, "f", "f"))
        self.assertIsNone(sweep.drop_decl(body, "h", "f"))

    def test_stack_arrays(self):
        body = "void f(void) {\n    f32 sp8;\n\n    g(&sp8);\n    h(sp8 + spC + sp10);\n}\n"
        self.assertEqual(sweep.stack_arrays(body),
                         "void f(void) {\n    f32 sp8[3];\n\n    g(sp8);\n    h(sp8[0] + sp8[1] + sp8[2]);\n}\n")

    def test_variadic_calls(self):
        asm = "\taddi r3, r1, 0x8\n/* x */\tcrclr cr1eq\n/* y */\tbl sprintf\n\tbl g\n"
        self.assertEqual(sweep.variadic_calls(asm), {"sprintf"})

    def test_plain_type(self):
        self.assertEqual(sweep.plain_type("struct nuvec_s* dest", True), "void *")
        self.assertEqual(sweep.plain_type("const u16 n", True), "u16")
        self.assertIsNone(sweep.plain_type("struct nuvec_s v", True))

    def test_fix_prototypes(self):
        body = ("s32 g(s32, s32);\ns32 sprintf(s32 *, s32);\nextern f32 lbl_K[];\n\n"
                "void f(void) {\n    g(1, 2);\n    g(3);\n    sprintf(0, 1);\n    k(lbl_K);\n}\n")
        old = sweep._PROTOS
        sweep._PROTOS = {"g": ("s32", ["s32"]), "k": ("void", ["f32"])}
        try:
            out = sweep.fix_prototypes(body.replace("extern f32", "void k(f32);\nextern f32"), "f",
                                       "\tcrclr cr1eq\n\tbl sprintf\n", "")
        finally:
            sweep._PROTOS = old
        self.assertIn("s32 g(s32);\n", out)
        self.assertIn("s32 sprintf();\n", out)
        self.assertIn("    g(1);\n    g(3);\n", out)
        self.assertIn("    k(lbl_K[0]);\n", out)

    def test_fix_prototypes_alias_takes_unit_params(self):
        # alias_decl kept m2c's guessed parameter for a callee the unit defines as (void):
        # the call must lose the argument (fn_800ABEB0 calling fn_800B9ED4).
        body = ("#define h ((s32 (*)(f32 *))h)\nextern const f32 lbl_K[];\n\n"
                "void f(void) {\n    x = h(lbl_K);\n}\n#undef h\n")
        out = sweep.fix_prototypes(body, "f", "\tbl h\n", "u16 h(void) {\n    return 1;\n}\n")
        self.assertIn("#define h ((s32(*)(void))h)\n", out)
        self.assertIn("    x = h();\n", out)

    def test_byte_strides(self):
        body = ("s32 f(void) {\n    s32 *p;\n    p = lbl;\n"
                "    (*(u8 *)((s8 *)(p) + (0x8B))) = 0U;\n    p += 0x9C;\n}\n")
        self.assertIn("    void *p;\n", sweep.byte_strides(body))
        deref = "void f(void) {\n    s32 *p;\n    p += 0xC;\n    *p = 1;\n}\n"
        self.assertEqual(sweep.byte_strides(deref), deref)
        indexed = "void f(void) {\n    s32 *p;\n    p += 0xC;\n    p[1] = 1;\n}\n"
        self.assertEqual(sweep.byte_strides(indexed), indexed)

    def test_base_pointer(self):
        body = ("void f(void) {\n    if (x) {\n"
                "        (*(s8 *)((s8 *)(MenuHeader) + (0x174))) = -1;\n"
                "        (*(f32 *)((s8 *)(MenuHeader) + (0x138))) = 0.0f;\n    }\n"
                "    (*(s8 *)((s8 *)(Other) + (0x4))) = 1;\n}\n")
        out = sweep.base_pointer(body)
        self.assertIn("        {\n            s8 *MenuHeader_p = (s8 *)MenuHeader;\n"
                      "            (*(s8 *)(MenuHeader_p + (0x174))) = -1;\n", out)
        self.assertIn("    (*(s8 *)((s8 *)(Other) + (0x4))) = 1;\n", out)  # a single access stays


if __name__ == "__main__":
    unittest.main()
