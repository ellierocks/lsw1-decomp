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

    def test_fix_prototypes_places_args_by_class(self):
        # m2c lists r3..r10 then f1..; the real prototype interleaves them and passes a ninth
        # integer on the stack, which m2c leaves as `sp8 = x;` (fn_80077038 calling fn_8007B298).
        body = ("s32 g(s32 *, s32, s32, s32, s32, s32, s32, s32, f32, f32);\n\n"
                "void f(void) {\n    s32 sp8;\n\n    sp8 = lbl_A;\n"
                "    g(&b, 1, 2, 3, 4, 5, 6, 7, x, y);\n}\n")
        old = sweep._PROTOS
        sweep._PROTOS = {"g": ("void", ["void *", "f32", "s32", "s32", "s32", "s32", "f32",
                                        "s32", "s32", "s32", "s32"])}
        try:
            out = sweep.fix_prototypes(body, "f", "\tbl g\n", "")
        finally:
            sweep._PROTOS = old
        self.assertIn("    g(&b, x, 1, 2, 3, 4, y, 5, 6, 7, lbl_A);\n", out)
        self.assertNotIn("sp8 = lbl_A", out)

    def test_fix_prototypes_passes_unwritten_args_through(self):
        # A thin wrapper leaves r3/r4 as its own arguments and sets only r5: m2c and the
        # arity repair fill the untouched registers with 0 (fn_80064A04 calling fn_800631DC).
        body = "s32 g(s32, s32);\n\nvoid f(void) {\n    g(0, 0);\n}\n"
        asm = ("/* 0 */\tstwu r1, -0x8(r1)\n/* 4 */\tmflr r0\n/* 8 */\tstw r0, 0xc(r1)\n"
               "/* c */\tli r5, 0x0\n/* 10 */\tbl g\n")
        out = sweep.fix_prototypes(body, "f", asm, "s32 g(void *, s8, s32);\n")
        self.assertIn("void f(void *arg0, s8 arg1) {\n    g(arg0, arg1, 0);\n", out)

    def test_fix_prototypes_spreads_compacted_args_by_register(self):
        # m2c lists only the registers it saw written: a call setting r6..r8 reads as
        # g(0, 0, arg3), and one setting r10 plus two stack words as g(0x1000) after the
        # stores (fn_80063E14, fn_8005A3F8).
        body = "s32 g(s32, s32, s32);\n\nvoid f(s32 arg3) {\n    g(0, 0, arg3);\n}\n"
        asm = "\tmr r8, r6\n\tli r7, 0x0\n\tli r6, 0x0\n\tbl g\n"
        out = sweep.fix_prototypes(body, "f", asm, "s32 g(void *, s32, void *, void *, void *, s32);\n")
        self.assertIn("void f(void *arg0, s32 arg1, void *arg2, s32 arg3) {\n"
                      "    g(arg0, arg1, arg2, 0, 0, arg3);\n", out)
        body = ("s32 h(s32);\n\nvoid f(void) {\n    s32 sp8;\n    s32 spC;\n\n"
                "    sp8 = 0xC00;\n    spC = 0x100;\n    h(0x1000);\n}\n")
        asm = ("\tli r0, 0xc00\n\tli r11, 0x100\n\tstw r0, 0x8(r1)\n\tli r10, 0x1000\n"
               "\tstw r11, 0xc(r1)\n\tbl h\n")
        out = sweep.fix_prototypes(body, "f", asm, "s32 h(" + ", ".join(["s32"] * 10) + ");\n")
        self.assertIn("    h(arg0, arg1, arg2, arg3, arg4, arg5, arg6, 0x1000, 0xC00, 0x100);\n", out)
        self.assertNotIn("sp8 = 0xC00", out)

    def test_passthrough_needs_the_first_call(self):
        self.assertIsNone(sweep.passthrough_regs("\tbl h\n\tbl g\n", "g"))
        gprs, fprs = sweep.passthrough_regs("\tli r4, 0x0\n\tfmr f1, f2\n\tstw r3, 0x8(r1)\n\tbl g\n", "g")
        self.assertEqual((gprs, fprs), ({0, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7}))
        body = "void g(s32, f32);\n\nvoid f(s32 arg1) {\n    g(0, 0);\n}\n"
        out = sweep.fix_prototypes(body, "f", "\tli r3, 0x0\n\tbl g\n", "void g(s32, f32);\n")
        self.assertIn("void f(s32 pad0, s32 arg1, f32 farg0) {\n    g(0, farg0);\n", out)

    def test_place_args_needs_the_stack_store(self):
        self.assertIsNone(sweep.place_args(["a"] * 8, ["s32"] * 8, ["s32"] * 9, "    x = 1;\n    "))

    def test_fix_prototypes_alias_takes_unit_params(self):
        # alias_decl kept m2c's guessed parameter for a callee the unit defines as (void):
        # the call must lose the argument (fn_800ABEB0 calling fn_800B9ED4).
        body = ("#define h ((s32 (*)(f32 *))h)\nextern const f32 lbl_K[];\n\n"
                "void f(void) {\n    x = h(lbl_K);\n}\n#undef h\n")
        out = sweep.fix_prototypes(body, "f", "\tbl h\n", "u16 h(void) {\n    return 1;\n}\n")
        self.assertIn("#define h ((s32(*)(void))h)\n", out)
        self.assertIn("    x = h();\n", out)

    def test_fix_prototypes_alias_places_args_by_class(self):
        # An aliased callee keeps m2c's class-sorted parameters in the `#define`; the call's
        # arguments must still move to the unit's interleaved order (fn_80077340).
        body = ("#define g ((s32 (*)(s32 *, s32, f32))g)\n\n"
                "void f(void) {\n    g(&b, 1, x);\n}\n#undef g\n")
        out = sweep.fix_prototypes(body, "f", "\tbl g\n", "void g(void *, f32, s32) {\n}\n")
        self.assertIn("    g(&b, x, 1);\n", out)

    def test_sorted_stores(self):
        # GCC schedules float stores first; the source wrote the fields in layout order
        # (fn_80064860).
        f = "(*(%s *)((s8 *)(p) + (%s))) = %s;"
        body = "    " + "\n    ".join([f % ("f32", "0x264", "b"), f % ("s16", "0x25C", "x"),
                                       f % ("f32", "0x260", "a")]) + "\n    q = 1;\n"
        self.assertEqual(sweep.sorted_stores(body), "    " + "\n    ".join(
            [f % ("s16", "0x25C", "x"), f % ("f32", "0x260", "a"), f % ("f32", "0x264", "b")]) + "\n    q = 1;\n")
        dep = "    " + f % ("s32", "8", "1") + "\n    " + f % ("s32", "4", "(*(s32 *)((s8 *)(p) + (8)))") + "\n"
        self.assertEqual(sweep.sorted_stores(dep), dep)

    def test_loop_guard(self):
        # GCC keeps a for loop's entry test as `cmpw i, n` (fn_80064B40).
        body = ("    var_r30 = 0;\n    if ((s32) lbl_N > 0) {\nloop_2:\n        var_r30 += 1;\n"
                "        if (var_r30 >= (s32) lbl_N) {\n")
        self.assertIn("    if (var_r30 < (s32) lbl_N) {\n", sweep.loop_guard(body))
        unrelated = body.replace("var_r30 >= (s32) lbl_N", "var_r30 >= (s32) lbl_M")
        self.assertEqual(sweep.loop_guard(unrelated), unrelated)

    def test_byte_strides(self):
        body = ("s32 f(void) {\n    s32 *p;\n    p = lbl;\n"
                "    (*(u8 *)((s8 *)(p) + (0x8B))) = 0U;\n    p += 0x9C;\n}\n")
        self.assertIn("    void *p;\n", sweep.byte_strides(body))
        deref = "void f(void) {\n    s32 *p;\n    p += 0xC;\n    *p = 1;\n}\n"
        self.assertEqual(sweep.byte_strides(deref), deref.replace("p += 0xC;", "p += 3;"))
        odd = "void f(void) {\n    s32 *p;\n    p += 6;\n    *p = 1;\n}\n"
        self.assertEqual(sweep.byte_strides(odd), odd)
        indexed = "void f(void) {\n    s16 *p;\n    p += 2;\n    p[1] = 1;\n}\n"
        self.assertIn("p += 1;", sweep.byte_strides(indexed))

    def test_float_ge_bare_return(self):
        self.assertEqual(sweep.float_ge("    return (t == lbl_1[0]) || (t > lbl_1[0]);"), "    return (t >= lbl_1[0]);")
        self.assertEqual(sweep.float_ge("if (((a == f(b)) || (a < f(b))) != 0)"), "if ((a <= f(b)))")

    def test_swap_stores(self):
        body = "void f(s32 *p) {\n    *(s32 *)(p + 1) = 0;\n    p->a = 2;\n    x = p->a;\n    p->b = p->a;\n}\n"
        self.assertEqual(sweep.swap_stores(body),
                         ["void f(s32 *p) {\n    p->a = 2;\n    *(s32 *)(p + 1) = 0;\n    x = p->a;\n    p->b = p->a;\n}\n"])

    def test_swap_stores_extern_globals_and_rotations(self):
        body = ("extern s32 ga;\nextern s32 gb;\nextern s32 gc;\nvoid f(void) {\n"
                "    ga = 0;\n    gb = 1;\n    gc = 0;\n}\n")
        outs = sweep.swap_stores(body)
        self.assertIn(body.replace("    ga = 0;\n    gb = 1;\n    gc = 0;", "    gc = 0;\n    ga = 0;\n    gb = 1;"), outs)
        self.assertIn(body.replace("    ga = 0;\n    gb = 1;", "    gb = 1;\n    ga = 0;"), outs)
        local = "void f(void) {\n    s32 x;\n    x = 0;\n    y = 1;\n}\n"
        self.assertEqual(sweep.swap_stores(local), [])

    def test_index_loops_pointer_bound(self):
        body = ("void f(u32 arg0) {\n    void *var_r31;\n\n    var_r31 = lbl_A;\n    do {\n"
                "        g(var_r31);\n        var_r31 += 0xC;\n"
                "    } while (var_r31 <= (void *) (((s8 *)lbl_A) + 0x18));\n}\n")
        self.assertEqual(sweep.index_loops(body),
                         "void f(u32 arg0) {\n    s32 i_var_r31;\n    void *var_r31;\n\n    i_var_r31 = 0;\n"
                         "    do {\n        var_r31 = (void *) ((s8 *) (lbl_A) + i_var_r31 * 12);\n"
                         "        g(var_r31);\n        i_var_r31 += 1;\n    } while (i_var_r31 <= 2);\n}\n")

    def test_index_loops_counter(self):
        body = ("void f(void) {\n    void *p;\n    s32 c;\n\n    c = 0;\n    if (n > 0) {\n"
                "        p = lbl_A;\n        do {\n            g(p);\n            c += 1;\n"
                "            p += 0xC;\n        } while (c < n);\n    }\n}\n")
        out = sweep.index_loops(body)
        self.assertIn("            p = (void *) ((s8 *) (lbl_A) + c * 12);\n            g(p);\n", out)
        self.assertNotIn("p = lbl_A;", out)
        self.assertNotIn("p += 0xC", out)
        used_after = body.replace("    }\n}\n", "    }\n    h(p);\n}\n")
        self.assertEqual(sweep.index_loops(used_after), used_after)

    def test_countdown_loops(self):
        body = ("void f(s32 arg3, s32 arg4) {\n    s32 var_r28;\n    s32 var_r29;\n    s8 *p;\n\n"
                "    if (arg4 > 0) {\n        var_r28 = arg4;\n        var_r29 = 0;\n        p = lbl_A;\n"
                "        do {\n            g(*(s32 *)(var_r29 + arg3), p);\n            var_r29 += 4;\n"
                "            p += 0xC;\n            var_r28 -= 1;\n        } while (var_r28 != 0);\n    }\n}\n")
        self.assertEqual(sweep.countdown_loops(body),
                         "void f(s32 arg3, s32 arg4) {\n    s32 var_r28;\n    s32 var_r29;\n    s8 *p;\n\n"
                         "    for (var_r28 = 0; var_r28 < arg4; var_r28++) {\n        var_r29 = var_r28 * 4;\n"
                         "        p = (void *) ((s8 *) (lbl_A) + var_r28 * 12);\n"
                         "        g(*(s32 *)(var_r29 + arg3), p);\n    }\n}\n")
        used = body.replace("    }\n}\n", "    }\n    h(p);\n}\n")
        self.assertEqual(sweep.countdown_loops(used), used)

    def test_empty_frame(self):
        asm = ("/* 0 0  94 21 FF E8 */\tstwu r1, -0x18(r1)\n/* 4 4  38 21 00 18 */\taddi r1, r1, 0x18\n"
               "/* 8 8  4E 80 00 20 */\tblr\n")
        self.assertEqual(sweep.empty_frame("void f(void) {\n\n}\n", asm),
                         "void f(void) {\n    volatile s32 pad[4];\n}\n")
        self.assertEqual(sweep.empty_frame("void f(void) {\n    g = 1;\n}\n", asm), "void f(void) {\n    g = 1;\n}\n")
        self.assertEqual(sweep.empty_frame("void f(void) {\n}\n", "/* 0 0  4E 80 00 20 */\tblr\n"), "void f(void) {\n}\n")

    def test_base_pointer(self):
        body = ("void f(void) {\n    if (x) {\n"
                "        (*(s8 *)((s8 *)(MenuHeader) + (0x174))) = -1;\n"
                "        (*(f32 *)((s8 *)(MenuHeader) + (0x138))) = 0.0f;\n    }\n"
                "    (*(s8 *)((s8 *)(Other) + (0x4))) = 1;\n}\n")
        out = sweep.base_pointer(body)
        self.assertIn("        {\n            s8 *MenuHeader_p = (s8 *)MenuHeader;\n"
                      "            (*(s8 *)(MenuHeader_p + (0x174))) = -1;\n", out)
        self.assertIn("    (*(s8 *)((s8 *)(Other) + (0x4))) = 1;\n", out)  # a single access stays

    def test_float_positions(self):
        body = ("s32 g(u8, s32, s32, f32, f32);\n\nvoid f(void) {\n"
                "    g(a, b, -1, x, y);\n}\n")
        outs = sweep.float_positions(body)
        self.assertEqual(len(outs), 9)  # C(5,2) placements minus the trailing one
        self.assertIn("s32 g(u8, f32, s32, f32, s32);\n\nvoid f(void) {\n    g(a, x, b, y, -1);\n}\n", outs)
        self.assertEqual(sweep.float_positions(body.replace("f32, f32", "s32, s32")), [])
        self.assertEqual(len(sweep.float_positions(body, limit=3)), 3)

    def test_switch_pivots(self):
        body = ("void f(void) {\n    switch (x) {\n    case 0:\n        a();\n        break;\n"
                "    case 1:\n        b();\n        break;\n    case 2:\n        break;\n    }\n}\n")
        outs = sweep.switch_pivots(body)
        self.assertEqual(len(outs), 2)
        self.assertIn("    case 3:\n        break;\n    }\n}\n", outs[0])
        self.assertIn("    case -1:\n        break;\n    }\n}\n", outs[1])
        self.assertEqual(sweep.switch_pivots(body.replace("    case 2:\n        break;\n", "")), [])

    def test_positive_guards(self):
        body = ("s32 f(void) {\n    if (((s32) x != 0x1C) || (y == 0) || !(a > b) || !((c) < (d - e))) {\n"
                "        return 0;\n    }\n    return 1;\n}\n")
        self.assertEqual(sweep.positive_guards(body),
                         "s32 f(void) {\n    if ((s32) x == 0x1C && y != 0 && a > b && (c) < (d - e)) {\n"
                         "        return 1;\n    }\n    return 0;\n}\n")
        # A lone conjunction, or a float comparison without `!`, is not flipped unsafely.
        same = body.replace("((s32) x != 0x1C) || (y == 0) || !(a > b) || !((c) < (d - e))", "(a && b)")
        self.assertEqual(sweep.positive_guards(same), same)
        self.assertEqual(sweep.negate("a < b"), "!(a < b)")
        self.assertEqual(sweep.negate("!(a) && b"), "!(!(a) && b)")


class WideReturnTests(unittest.TestCase):
    def test_wide_return(self):
        body = "extern void *g;\n\nu8 f(void) {\n    if (g != NULL) {\n        return *(u8 *)g;\n    }\n    return -1U;\n}\n"
        out = sweep.wide_return(body)
        self.assertIn("s32 f(void) {", out)
        self.assertIn("return -1;", out)
        self.assertEqual(sweep.wide_return(body.replace("-1U", "0")), body.replace("-1U", "0"))


if __name__ == "__main__":
    unittest.main()
