"""Cached globals: preserve conversions and reject writes and local sources."""
import unittest
from tools import sweep  # make tools imports available
from sweep_repairs.globals import inline_globals, inline_globals_after_calls


class GlobalsTest(unittest.TestCase):
    BODY = ("void f(void) {\n    s32 temp_r9;\n\n"
            "    temp_r9 = lbl_X;\n    use(temp_r9);\n    use(temp_r9);\n}\n")

    def test_reload_and_remove_local(self):
        out = inline_globals(self.BODY)
        self.assertNotIn("temp_r9", out)
        self.assertEqual(out.count("use(((s32) lbl_X))"), 2)
        self.assertEqual(inline_globals(out), out)

    def test_after_call_preserves_first_use(self):
        out = inline_globals_after_calls(self.BODY)
        self.assertIn("temp_r9 = lbl_X;\n    use(temp_r9);", out)
        self.assertEqual(out.count("use(((s32) lbl_X))"), 1)

    def test_cast_keeps_assignment_conversion(self):
        out = inline_globals(self.BODY.replace("= lbl_X", "= (u8) lbl_X"))
        self.assertIn("use(((s32) (u8) lbl_X))", out)

    def test_writes_and_address_taking_are_left_alone(self):
        for statement in ("temp_r9 = lbl_Y;", "temp_r9 += 1;", "++temp_r9;",
                          "temp_r9--;", "use(&temp_r9);"):
            body = self.BODY.replace("    use(temp_r9);", "    " + statement, 1)
            self.assertEqual(inline_globals(body), body, statement)

    def test_local_or_parameter_is_not_global(self):
        for body in (self.BODY.replace("void f(void)", "void f(s32 lbl_X)"),
                     self.BODY.replace("    s32 temp_r9;", "    s32 lbl_X;\n    s32 temp_r9;")):
            self.assertEqual(inline_globals(body), body)

    def test_no_intervening_call(self):
        body = self.BODY.replace("use(temp_r9);", "out = temp_r9;")
        self.assertEqual(inline_globals_after_calls(body), body)


if __name__ == "__main__":
    unittest.main()
