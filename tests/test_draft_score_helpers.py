"""Draft extern arrays, compact assembly and the score line's sprint timer suffix."""
import json
from pathlib import Path
import tempfile
import time
import unittest

from tools import draft_fn, score_fn


class ArrayizeTest(unittest.TestCase):
    def test_absolute_scalars_become_arrays(self):
        asm = ("lis r9, lbl_A@ha\nlfs f0, lbl_A@l(r9)\nlfs f1, lbl_S@sda21(r0)\n"
               "lis r3, str_X@ha\naddi r3, r3, str_X@l\n")
        text = ("extern f32 lbl_A;\nextern f32 lbl_S;\nextern char str_X[];\n\n"
                "void f(void) {\n    g(lbl_A + lbl_S, &lbl_A, str_X);\n}\n")
        out = draft_fn.arrayize(text, draft_fn.absolute_symbols(asm))
        self.assertIn("extern f32 lbl_A[];", out)
        self.assertIn("extern f32 lbl_S;", out)
        self.assertIn("extern char str_X[];", out)
        self.assertIn("g(lbl_A[0] + lbl_S, lbl_A, str_X);", out)

    def test_compact_asm_drops_byte_column_and_markers(self):
        asm = (".fn f, global\n/* 8013EFD8 0013BFD8  81 63 0E EC */\tlwz r11, 0xeec(r3)\n"
               ".L_1:\n.endfn f\n")
        self.assertEqual(draft_fn.compact_asm(asm), "\tlwz r11, 0xeec(r3)\n.L_1:")


class TimerSuffixTest(unittest.TestCase):
    def suffix(self, state):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "timing.json"
            path.write_text(json.dumps(state))
            return score_fn.timer_suffix(path)

    def test_running_timer_reports_active_seconds_without_a_countdown(self):
        self.assertEqual(self.suffix({"seconds": {"setup": 100}, "active": "matching",
                                      "since": time.time() - 200}), "  [active 300s]")
        self.assertEqual(self.suffix({"seconds": {"matching": 900}, "active": "matching",
                                      "since": time.time()}), "  [active 900s]")

    def test_paused_or_missing_timer_is_silent(self):
        self.assertEqual(self.suffix({"seconds": {}, "active": "paused", "since": 0}), "")
        self.assertEqual(score_fn.timer_suffix(Path("/nonexistent/timing.json")), "")


if __name__ == "__main__":
    unittest.main()


class ErrorTailTest(unittest.TestCase):
    def test_temporary_directory_is_dropped_before_the_cut(self):
        line = "Z:/tmp/score_fn_5uq_brh/obstacle.c:4848: incompatible type"
        out = score_fn.error_tail("x" * 1490 + "\n" + line)
        self.assertEqual(out, "obstacle.c:4848: incompatible type")
        self.assertNotIn("5uq_brh", out)
        self.assertEqual(score_fn.error_tail("a.c:1: x\n" * 400).splitlines()[0], "a.c:1: x")


class DropRedeclaredTest(unittest.TestCase):
    def test_function_pointer_extern_names_the_pointer(self):
        context = "extern void (*cb)(void);\nextern s32 n;\n"
        body = "extern void (*cb)(void);\nextern s32 n;\nextern void (*other)(s32);\nvoid f(void) {}\n"
        out, dropped = score_fn.drop_redeclared(context, body)
        self.assertEqual(dropped, ["cb", "n"])
        self.assertIn("extern void (*other)(s32);", out)


class StaticHelperTest(unittest.TestCase):
    def test_body_helper_replaces_unit_definition_in_place(self):
        text = ("static inline s32 H(s32 x)\n{\n    return x;\n}\n\nvoid g(void) { H(1); }\n\n"
                "void f(void)\n{\n    H(2);\n}\n")
        body = "static inline s32 H(s32 x)\n{\n    return x + 1;\n}\n\nvoid f(void)\n{\n    H(3);\n}\n"
        out = score_fn.splice_body(text, "f", body)
        self.assertEqual(out.count("static inline s32 H"), 1)
        self.assertLess(out.index("return x + 1;"), out.index("void g(void)"))
        self.assertIn("H(3);", out)
        self.assertNotIn("H(2);", out)

    def test_new_helper_stays_in_body(self):
        text = "void f(void)\n{\n}\n"
        body = "static inline s32 H(void)\n{\n    return 0;\n}\n\nvoid f(void)\n{\n    H();\n}\n"
        out = score_fn.splice_body(text, "f", body)
        self.assertEqual(out.count("static inline s32 H"), 1)
        self.assertLess(out.index("static inline s32 H"), out.index("void f(void)"))


class FunctionExternTest(unittest.TestCase):
    def test_array_extern_becomes_the_definitions_prototype(self):
        text = "extern s32 fn_A[];\nvoid g(void) { h(fn_A); }\n"
        out = score_fn.splice_body(text, "fn_A", "s32 fn_A(s32 a, f32 x) {\n    return 0;\n}\n")
        self.assertTrue(out.startswith("s32 fn_A(s32 a, f32 x);\n"))
        self.assertNotIn("fn_A[]", out)

    def test_struct_parameters_stay_unprototyped(self):
        out = score_fn.function_extern("extern s32 fn_A[];\n", "fn_A", "void fn_A(struct S *p) {\n}\n")
        self.assertEqual(out, "void fn_A();\n")


class SweepRepairTest(unittest.TestCase):
    def setUp(self):
        import sys
        sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "tools"))
        import sweep
        self.sweep = sweep

    def test_unit_sweep_keeps_other_units_rows(self):
        prev = [{"name": "a", "unit": "auto/game/obstacle"}, {"name": "b", "unit": "auto/game/main"},
                {"name": "c", "unit": "auto/game/levels", "applied": True}]
        kept = self.sweep.kept_rows(prev, "auto/game/obstacle", [{"name": "a"}])
        self.assertEqual([r["name"] for r in kept], ["b"])

    def test_adopt_own_prototype_keeps_m2c_names(self):
        body = "extern s32 x;\n\ns32 f(void *arg0, void *arg1) {\n    return 0;\n}\n"
        out = self.sweep.adopt_own_prototype(body, "f", "u8 *f(void *, char *, s32 *);\n")
        self.assertIn("u8 *f(void *arg0, char *arg1, s32 *pad2) {", out)

    def test_adopt_own_prototype_ignores_unspecified_arguments(self):
        body = "s32 f(void *arg0) {\n}\n"
        self.assertEqual(self.sweep.adopt_own_prototype(body, "f", "s32 f();\n"), body)

    def test_declare_unset_names_saved_registers(self):
        out = self.sweep.declare_unset("void f(void) {\n    g(saved_reg_r28);\n}\n", "f")
        self.assertIn("    s32 saved_reg_r28;\n", out)

    def test_stack_array_folds_zero_indexed_address(self):
        body = "void f(void) {\n    s32 sp8;\n    g(&sp8);\n    h(&sp20[0]);\n}\n"
        out = self.sweep.stack_arrays(body)
        self.assertIn("s32 sp8[7];", out)
        self.assertIn("h(&sp8[6]);", out)

    def test_find_function_skips_calls(self):
        from variant_search import find_function
        text = "void g(void) {\n    if (f(1) != 0) {\n    }\n}\n\nint f(int a) {\n    return a;\n}\n"
        self.assertEqual(find_function(text, "f").body.strip(), "return a;")

    def test_moves_to_args_orders_parallel_moves(self):
        self.assertEqual(self.sweep.moves_to_args(["r4", "r3", "r5"]),
                         ["\tmr r0, r3", "\tmr r3, r4", "\tmr r4, r0", "\tmr r5, r5"])
        self.assertEqual(self.sweep.moves_to_args(["r30", "r31", "r29"]),
                         ["\tmr r3, r30", "\tmr r4, r31", "\tmr r5, r29"])

    def test_fold_ps_replaces_add_block_with_call(self):
        block = ["psq_l f1, 0x0(r4), 0, qr0", "psq_l f3, 0x0(r5), 0, qr0",
                 "psq_l f2, 0x8(r4), 1, qr0", "psq_l f4, 0x8(r5), 1, qr0",
                 "ps_add f1, f1, f3", "ps_add f2, f2, f4",
                 "psq_st f1, 0x0(r3), 0, qr0", "psq_st f2, 0x8(r3), 1, qr0"]
        asm = "\n".join(f"/* 80000000 00000000  00 00 00 00 */\t{ln}" for ln in ["li r0, 1", *block, "blr"])
        out = self.sweep.fold_ps(asm)
        self.assertIn("bl NuVecAddPS", out)
        self.assertNotIn("psq_", out)
        self.assertIn("blr", out)
        self.assertIn("bl NuVecSubPS", self.sweep.fold_ps(asm.replace("ps_add", "ps_sub")))

    def test_fix_ps_calls_pads_trims_and_drops_value(self):
        body = "void f(void) {\n    temp_r31 = NuVecSubPS(&sp8, a);\n    NuVecAddPS(x, y, z, w);\n}\n"
        out = self.sweep.fix_ps_calls(body)
        self.assertIn("    NuVecSubPS(&sp8, a, 0);\n    temp_r31 = &sp8;", out)
        self.assertIn("NuVecAddPS(x, y, z);", out)


class UnitMwSettingsTest(unittest.TestCase):
    def test_reads_the_units_pinned_compiler_and_flags(self):
        with tempfile.TemporaryDirectory() as tmp:
            ninja = Path(tmp) / "build.ninja"
            ninja.write_text(
                "build build/GL5E4F/src/dolphin/x/a.o: mwcc $\n"
                "    src/dolphin/x/a.c | build/compilers $\n    build/tools/wibo\n"
                "  mw_version = GC/1.2.5\n"
                "  cflags = -O4,p -pragma \"cats off\" $\n      -I- -i include -lang=c\n"
                "  basedir = build\n"
                "build build/GL5E4F/src/dolphin/x/b.o: mwcc src/dolphin/x/b.c\n"
                "  mw_version = GC/1.2.5n\n  cflags = -O4,p\n")
            got = score_fn.unit_mw_settings(score_fn.ROOT / "src/dolphin/x/a.c", ninja)
            self.assertEqual(got, ("1.2.5", ["-O4,p", "-pragma", "cats off", "-I-", "-i", "include",
                                             "-lang=c"]))
            self.assertEqual(score_fn.unit_mw_settings(score_fn.ROOT / "src/dolphin/x/b.c", ninja)[0],
                             "1.2.5n")
            self.assertIsNone(score_fn.unit_mw_settings(score_fn.ROOT / "src/dolphin/x/c.c", ninja))
