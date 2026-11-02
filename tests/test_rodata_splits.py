"""Range proposal rules for tools/rodata_splits.py on synthetic units."""
import unittest

from tools import rodata_splits as rs

SPLITS = """Sections:
\t.text       type:code align:4

a.c:
\t.text       start:0x80000000 end:0x80000100

b_1.c:
\t.text       start:0x80000100 end:0x80000200

b_2.c:
\t.text       start:0x80000200 end:0x80000300

c.c:
\t.text       start:0x80000300 end:0x80000400

dolphin/os/OS.c:
\t.text       start:0x80000400 end:0x80000500
"""

SYMBOLS = """lbl_A0 = .rodata:0x80100000; // type:object size:0x4 align:4 data:float
lbl_A1 = .rodata:0x80100004; // type:object size:0xC data:string
lbl_B0 = .rodata:0x80100010; // type:object size:0x8
lbl_B1 = .rodata:0x80100018; // type:object size:0x8
lbl_C0 = .rodata:0x80100030; // type:object size:0x4
lbl_OS = .rodata:0x80100040; // type:object size:0x4
fn_X = .text:0x80000000; // type:function size:0x100
"""


def units(refs):
    out = rs.parse_splits(SPLITS)
    for u in out:
        u.refs = set(refs.get(u.name, ()))
    return out


class RodataSplitTests(unittest.TestCase):
    def test_spans_follow_references_and_skip_shared_tu_pieces(self):
        symbols = rs.rodata_symbols(SYMBOLS)
        game = rs.propose(units({"a.c": ["lbl_A0", "lbl_A1", "fn_X"],
                                 "b_1.c": ["lbl_B0", "lbl_B1"], "b_2.c": ["lbl_B1"],
                                 "c.c": ["lbl_C0"], "dolphin/os/OS.c": ["lbl_OS"]}),
                          symbols, matching=set())
        spans = {u.name: u.span for u in game}
        self.assertEqual(spans["a.c"], (0x80100000, 0x80100010))
        self.assertIsNone(spans["b_1.c"])
        self.assertIsNone(spans["b_2.c"])
        self.assertEqual(spans["c.c"], (0x80100030, 0x80100034))
        self.assertNotIn("dolphin/os/OS.c", spans)

    def test_matching_units_need_opt_in(self):
        symbols = rs.rodata_symbols(SYMBOLS)
        refs = {"a.c": ["lbl_A0"]}
        game = rs.propose(units(refs), symbols, matching={"a"})
        self.assertIsNone(next(u for u in game if u.name == "a.c").span)
        game = rs.propose(units(refs), symbols, matching={"a"}, include_matching=True)
        self.assertEqual(next(u for u in game if u.name == "a.c").span, (0x80100000, 0x80100004))

    def test_interleaved_spans_without_shared_literals_stay_unresolved(self):
        # Like the nufont/nugscn audit: overlapping envelopes can have no
        # shared target, so the range tool cannot infer a common TU.
        game = rs.propose(units({"a.c": ["lbl_A0", "lbl_B1"],
                                 "b_1.c": ["lbl_B0"]}),
                          rs.rodata_symbols(SYMBOLS), matching=set())
        for unit in game[:2]:
            self.assertIsNone(unit.span)
            self.assertEqual(unit.reason, "overlaps another unit (ownership unresolved)")

    def test_with_ranges_inserts_after_text(self):
        text = rs.with_ranges(SPLITS, {"c.c": (0x80100030, 0x80100034)})
        self.assertIn("c.c:\n\t.text       start:0x80000300 end:0x80000400\n"
                      "\t.rodata     start:0x80100030 end:0x80100034\n", text)
        self.assertEqual(text.count(".rodata"), 1)

    def test_relocation_targets_only_from_text(self):
        out = """Relocation section '.rela.text' at offset 0x1 contains 2 entries:
 Offset     Info    Type                Sym. Value  Symbol's Name + Addend
0000001a  00006006 R_PPC_ADDR16_HA        00000000   lbl_A0 + 0
Relocation section '.rela.data' at offset 0x2 contains 1 entries:
00000000  00000101 R_PPC_ADDR32           00000000   lbl_C0 + 0
"""
        self.assertEqual(rs.text_relocation_targets(out), {"lbl_A0"})


if __name__ == "__main__":
    unittest.main()
