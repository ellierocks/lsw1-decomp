import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))

import pattern_library as pl  # noqa: E402

STABS = """\t.file\t"a.ii"
.stabs "src/game/a.cpp",100,0,0,.Ltext0
.text
\t.align 2
.stabs "f__Fv:F(0,1)",36,0,0,f__Fv
\t.globl f__Fv
f__Fv:
.stabn 68,0,10,.LM1-f__Fv\r
.LM1:
\tli 3,1
.stabs "include/h.h",132,0,0,.Ltext1
.stabn 68,0,4,.LM2-f__Fv
.LM2:
\tblr
.Lfe1:
g:
\tblr
"""

DTK = """.fn f__Fv, global
/* 00000000 00000034  38 60 00 01 */\tli r3, 0x1
/* 00000004 00000038  4E 80 00 20 */\tblr
.endfn f__Fv
"""


class PatternLibraryTest(unittest.TestCase):
    def test_stabs_lines_track_main_and_included_files(self):
        lines = pl.stabs_lines(STABS)
        self.assertEqual(lines["f__Fv"], [("src/game/a.cpp", 10), ("include/h.h", 4)])
        self.assertEqual(lines["g"], [("include/h.h", 4)])

    def test_dtk_functions(self):
        self.assertEqual(pl.dtk_functions(DTK), {"f__Fv": [(0, "li r3, 0x1"), (4, "blr")]})

    def test_line_directive_maps_back_to_physical_line(self):
        with tempfile.TemporaryDirectory() as tmp:
            src = Path(tmp) / "a.cpp"
            src.write_text("int a;\n#line 100 \"D:/Bio4/Prog/a.cpp\"\nint b;\nint c;\n")
            segs = pl.line_segments(src, "src/a.cpp")
            self.assertEqual(pl.physical(segs, "src/a.cpp", "D:/Bio4/Prog/a.cpp", 101), ("src/a.cpp", 4))
            self.assertEqual(pl.physical(segs, "src/a.cpp", "src/a.cpp", 1), ("src/a.cpp", 1))
            self.assertIsNone(pl.physical(segs, "src/a.cpp", "other.cpp", 1))

    def test_source_name_strips_wibo_drive(self):
        root = Path("/home/x/repo")
        self.assertEqual(pl.source_name("Z:/home/x/repo/src/a.c", root), "src/a.c")
        self.assertEqual(pl.source_name("include/h.h", root), "include/h.h")

    def test_shape_classes(self):
        self.assertEqual(pl.shape("lwz r9, lbl_80409410@sda21(r0)"), "lwz a,sym(r0)")
        self.assertEqual(pl.shape("stw r31, 0x1c(r1)"), "stw s,d(sp)")
        self.assertEqual(pl.shape("cmpwi r3, 0x0"), "cmpwi a,0")
        self.assertEqual(pl.shape("addi r4, r27, str_Bool_ON@l"), "addi a,s,sym@l")
        self.assertEqual(pl.shape("fmuls f1, f31, f0"), "fmuls f,fs,f")
        self.assertEqual(pl.mnemonic("bne+ .L_1"), "bne")


if __name__ == "__main__":
    unittest.main()
