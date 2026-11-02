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
