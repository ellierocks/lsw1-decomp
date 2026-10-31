import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import sdk_import_unit as imp  # noqa: E402
import sdk_probe  # noqa: E402
import verify_fn as vf  # noqa: E402

SPLITS = """Sections:
\t.text       type:code align:4

dolphin/si/SISamplingRateBSS.c:
\t.sbss       start:0x8040A920 end:0x8040A924

dolphin/ax/AX.c:
\t.text       start:0x80171000 end:0x8017237C
\t.data       start:0x801F6C60 end:0x801F6CA4

dolphin/ax/AXProf.c:
\t.text       start:0x801753CC end:0x80175414
"""

CONFIGURE = '''cflags_mw_sdk = [
    "-i src/dolphin/gx", "-i src/dolphin/ax", "-i src/dolphin/card",
]
config.libs = [
    DolphinLib("os", [
        SDKObject("os/OSAlloc", Matching),
        SDKObject("os/OSLink", Matching),
    ]),
    DolphinLib("ax", [SDKObject("ax/AX", Matching), SDKObject("ax/AXProf", Matching)]),
    DolphinLib("pad", [SDKObject("pad/Padclamp", Matching)]),
]
'''


def with_splits(*units):
    text = SPLITS
    for unit, start, end in units:
        text = imp.insert_split_block(text, unit, start, end)
    return text


class SplitTests(unittest.TestCase):
    def test_block_goes_after_the_next_lower_text_start(self):
        text = imp.insert_split_block(SPLITS, "ax/AXAux", 0x80172844, 0x80172CCC)
        self.assertIn("dolphin/ax/AX.c:", text)
        order = [h for h, _, _ in imp.split_blocks(text)]
        self.assertEqual(order.index("dolphin/ax/AXAux.c:"), order.index("dolphin/ax/AX.c:") + 1)
        self.assertLess(order.index("dolphin/ax/AXAux.c:"), order.index("dolphin/ax/AXProf.c:"))
        self.assertIn("\t.text       start:0x80172844 end:0x80172CCC\n\ndolphin/ax/AXProf.c:", text)

    def test_existing_block_and_missing_predecessor_are_errors(self):
        with self.assertRaises(imp.ImportError_):
            imp.insert_split_block(SPLITS, "ax/AX", 0x80172844, 0x80172CCC)
        with self.assertRaises(imp.ImportError_):
            imp.insert_split_block(SPLITS, "ax/First", 0x100, 0x200)


class ConfigureTests(unittest.TestCase):
    def test_object_inserted_in_address_order_in_single_line_lib(self):
        splits = with_splits(("ax/AXAux", 0x80172844, 0x80172CCC))
        text = imp.insert_sdk_object(CONFIGURE, splits, "ax/AXAux", True)
        self.assertIn('SDKObject("ax/AX", Matching), SDKObject("ax/AXAux", Matching, external_bss=True), '
                      'SDKObject("ax/AXProf", Matching)', text)

    def test_object_appended_to_multiline_lib_keeps_indentation(self):
        splits = with_splits(("os/OSFatal", 0x8018BE6C, 0x8018CAFC), ("os/OSAlloc", 0x80180000, 0x80180100),
                             ("os/OSLink", 0x80181000, 0x80181100))
        text = imp.insert_sdk_object(CONFIGURE, splits, "os/OSFatal", False)
        self.assertIn('        SDKObject("os/OSLink", Matching),\n        SDKObject("os/OSFatal", Matching)', text)
        before = imp.insert_sdk_object(CONFIGURE, with_splits(("os/OSAlloc", 0x80180000, 0x80180100),
                                                              ("os/OSLink", 0x80181000, 0x80181100),
                                                              ("os/OSError", 0x80180800, 0x80180900)),
                                       "os/OSError", False)
        self.assertIn('        SDKObject("os/OSAlloc", Matching),\n        SDKObject("os/OSError", Matching),\n'
                      '        SDKObject("os/OSLink", Matching),', before)

    def test_new_lib_is_placed_between_neighbours(self):
        splits = with_splits(("ax/AXAux", 0x80172844, 0x80172CCC), ("pad/Padclamp", 0x80190000, 0x80190100),
                             ("vi/vi", 0x80185000, 0x80185200))
        text = imp.insert_sdk_object(CONFIGURE, splits, "vi/vi", False)
        lines = text.splitlines()
        vi = next(i for i, l in enumerate(lines) if 'DolphinLib("vi"' in l)
        self.assertIn('DolphinLib("ax"', lines[vi - 1])
        self.assertIn('DolphinLib("pad"', lines[vi + 1])

    def test_include_flag_added_once(self):
        text = imp.add_include_flag(CONFIGURE, "vi")
        self.assertIn('"-i src/dolphin/card", "-i src/dolphin/vi",', text)
        self.assertEqual(imp.add_include_flag(text, "vi"), text)


class NameTests(unittest.TestCase):
    def table(self, rows, rejected=()):
        t = vf.SymbolTable.__new__(vf.SymbolTable)
        t.entries = {n: vf.SymEntry(".text", a, 4, True) for n, a in rows}
        t.duplicates = set()
        return t

    def test_rename_only_changes_the_symbol_column(self):
        text = "fn_80172844 = .text:0x80172844; // type:function size:0x10\nOther = .text:0x1; // fn_80172844\n"
        got = imp.rename_symbols(text, {"fn_80172844": "__AXAuxInit"})
        self.assertEqual(got, "__AXAuxInit = .text:0x80172844; // type:function size:0x10\n"
                              "Other = .text:0x1; // fn_80172844\n")

    def test_donor_includes(self):
        src = '#include <dolphin.h>\n#include "__ax.h"\n  # include <dolphin/ax.h>\n// #include "no.h"\n'
        self.assertEqual(imp.donor_includes(src), [("<", "dolphin.h"), ('"', "__ax.h"), ("<", "dolphin/ax.h")])

    def test_pairing_proposes_donor_names_for_unnamed_symbols_only(self):
        symbols = self.table([("fn_80174AB0", 0x80174AB0), ("lbl_8040AB88", 0x8040AB88), ("OSReport", 0x80100000)])
        out = imp.Pairing()
        mine = [(0, 10, "__AXSetPBDefault", 0), (4, 109, "__AXClMode", 0), (8, 10, "OSReport", 0),
                (12, 10, "Donor", 0)]
        target = [(0, 10, "fn_80174AB0", 0), (4, 109, "lbl_8040AB88", 0), (8, 10, "OSReport", 0),
                  (12, 10, "Retail", 0)]
        imp.pair_refs(mine, target, {}, symbols, "f", out)
        self.assertEqual(out.renames, {"fn_80174AB0": "__AXSetPBDefault", "lbl_8040AB88": "__AXClMode"})
        self.assertEqual(len(out.conflicts), 1)
        self.assertIn("both named", out.conflicts[0])

    def test_pairing_resolves_dtk_local_suffix_and_anchored_functions(self):
        symbols = self.table([("helper", 0x80100000)])
        out = imp.Pairing()
        imp.pair_refs([(0, 10, "helper", 0), (4, 10, "Compiled", 0)], [(0, 10, "helper_80100000", 0),
                      (4, 10, "Retail", 0)], {"Compiled": "Retail"}, symbols, "f", out)
        self.assertEqual((out.renames, out.conflicts), ({}, []))

    def test_pairing_rejects_different_layouts_and_addends(self):
        symbols = self.table([])
        out = imp.Pairing()
        imp.pair_refs([(0, 10, "A", 0)], [(0, 10, "fn_80000000", 0), (4, 10, "x", 0)], {}, symbols, "f", out)
        imp.pair_refs([(0, 109, "A", 4)], [(0, 109, "lbl_80000000", 8)], {}, symbols, "g", out)
        self.assertEqual(out.renames, {})
        self.assertEqual(len(out.conflicts), 2)

    def test_check_renames_blocks_existing_rejected_and_invalid_names(self):
        symbols = self.table([("fn_1", 0x1), ("fn_2", 0x2), ("fn_3", 0x3), ("fn_4", 0x4), ("Used", 0x5)])
        pairing = imp.Pairing(renames={"fn_1": "Good", "fn_2": "Used", "fn_3": "Bad", "fn_4": "1x"})
        accepted = imp.check_renames(pairing, symbols, {0x3: {"Bad"}})
        self.assertEqual(accepted, {"fn_1": "Good"})
        self.assertEqual(len(pairing.conflicts), 3)


class AnchorPruneTests(unittest.TestCase):
    def test_stray_code_anchor_outside_the_unit_footprint_is_dropped(self):
        rows = [("a", 0x1000, 16), ("b", 0x1010, 16), ("c", 0x1020, 16), ("far", 0x9000, 8)]
        table = type("T", (), {})()
        table.entries = {n: vf.SymEntry(".text", a, s, True) for n, a, s in rows}
        table.duplicates = set()
        funcs = [vf.Symbol(0, n, i * 16, 16, 2, 1, 1) for i, n in enumerate(["x", "y", "z", "w"])]
        got = sdk_probe.anchor(funcs, table, {"x": "a", "y": "b", "w": "far"})
        self.assertEqual({k: v[0] for k, v in got.items()}, {"x": "a", "y": "b", "z": "c"})


if __name__ == "__main__":
    unittest.main()
