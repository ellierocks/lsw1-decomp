import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import sdk_import_symbols as sym  # noqa: E402
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


class AnchorNameVersusCodeTests(unittest.TestCase):
    def setUp(self):
        rows = [("A", 0x1000, 0x40), ("B", 0x1040, 0x40), ("C", 0x1080, 0x40)]
        self.table = type("T", (), {})()
        self.table.entries = {n: vf.SymEntry(".text", a, s, True) for n, a, s in rows}
        self.table.duplicates = set()

    def funcs(self, *names):
        return [vf.Symbol(i, n, i * 0x40, 0x40, 2, 1, 1) for i, n in enumerate(names)]

    def test_identical_code_beats_a_name_that_sits_on_a_neighbour(self):
        # retail names are shifted by one: donor B's code is at retail A, donor C's at retail B
        got = sdk_probe.anchor(self.funcs("B", "C"), self.table, {"B": "A", "C": "B"})
        self.assertEqual({k: v for k, v in got.items()}, {"B": ("A", "code"), "C": ("B", "code")})

    def test_a_retail_function_is_never_claimed_twice(self):
        got = sdk_probe.anchor(self.funcs("A", "B", "X"), self.table, {"X": "A"})
        owners = [r for r, _ in got.values()]
        self.assertEqual(len(owners), len(set(owners)))
        self.assertEqual(got["X"], ("A", "code"))

    def test_matching_name_and_code_stay_a_name_anchor(self):
        got = sdk_probe.anchor(self.funcs("A", "B"), self.table, {"A": "A", "B": "B"})
        self.assertEqual(got, {"A": ("A", "name"), "B": ("B", "name")})


class AnchorPruneTests(unittest.TestCase):
    def test_stray_code_anchor_outside_the_unit_footprint_is_dropped(self):
        rows = [("a", 0x1000, 16), ("b", 0x1010, 16), ("c", 0x1020, 16), ("far", 0x9000, 8)]
        table = type("T", (), {})()
        table.entries = {n: vf.SymEntry(".text", a, s, True) for n, a, s in rows}
        table.duplicates = set()
        funcs = [vf.Symbol(0, n, i * 16, 16, 2, 1, 1) for i, n in enumerate(["x", "y", "z", "w"])]
        got = sdk_probe.anchor(funcs, table, {"x": "a", "y": "b", "w": "far"})
        self.assertEqual({k: v[0] for k, v in got.items()}, {"x": "a", "y": "b", "z": "c"})


def symtable(rows):
    t = vf.SymbolTable.__new__(vf.SymbolTable)
    t.entries = {n: vf.SymEntry(".text", a, s, True) for n, a, s in rows}
    t.duplicates = set()
    return t


class ProposalPlanTests(unittest.TestCase):
    ROWS = [("OldA", 0x1000, 0x40), ("OldB", 0x1040, 0x40), ("fn_80001080", 0x1080, 0x40),
            ("TinyOld", 0x10C0, 0xC), ("Same", 0x10D0, 0x40)]

    def test_named_retail_function_with_other_donor_name_becomes_a_proposal(self):
        anchors = {"NewA": ("OldA", "code"), "NewB": ("OldB", "layout from NewA"), "Same": ("Same", "name"),
                   "NewC": ("fn_80001080", "code")}
        props = imp.name_proposals(anchors, symtable(self.ROWS), lambda c, r: True)
        self.assertEqual([(p.retail, p.donor, p.identical) for p in props],
                         [("OldA", "NewA", True), ("OldB", "NewB", True)])

    def test_unnamed_retail_is_a_proposal_only_when_the_donor_name_is_taken_elsewhere(self):
        rows = self.ROWS + [("Taken", 0x2000, 0x20)]
        anchors = {"Taken": ("fn_80001080", "code"), "Free": ("fn_80001080", "code")}
        props = imp.name_proposals(anchors, symtable(rows), lambda c, r: True)
        self.assertEqual([(p.retail, p.donor) for p in props], [("fn_80001080", "Taken")])

    def test_differing_code_is_unproven_evidence(self):
        props = imp.name_proposals({"NewA": ("OldA", "layout from X")}, symtable(self.ROWS), lambda c, r: False)
        sym.resolve_proposals(props, symtable(self.ROWS), {})
        self.assertEqual(props[0].status, "unproven")
        self.assertIn("code differs", props[0].describe())

    def test_tiny_function_needs_a_unique_code_match_or_a_proven_neighbour(self):
        rows = self.ROWS
        weak = imp.name_proposals({"New": ("TinyOld", "layout from Same")}, symtable(rows),
                                  lambda c, r: c != "Same")
        self.assertFalse(weak[0].identical)
        strong = imp.name_proposals({"New": ("TinyOld", "layout from NewA"), "NewA": ("OldA", "code")},
                                    symtable(rows), lambda c, r: True)
        self.assertTrue(next(p for p in strong if p.donor == "New").identical)
        unique = imp.name_proposals({"New": ("TinyOld", "code")}, symtable(rows), lambda c, r: True)
        self.assertTrue(unique[0].identical)

    def test_chain_collects_the_run_a_seed_belongs_to(self):
        a, b, c, other = (sym.Proposal(0, 4, r, d, "", True) for r, d in
                          [("B", "A"), ("C", "B"), ("D", "C"), ("Q", "R")])
        self.assertEqual(imp.chain([a, b, c, other], a), [a, b, c])
        self.assertEqual(imp.chain([a, b, c, other], c), [c, b, a])

    def test_neighbour_code_confirms_a_tiny_function_through_a_larger_neighbour(self):
        funcs = [vf.Symbol(0, "Tiny", 0, 12, 2, 1, 1), vf.Symbol(1, "Stripped", 12, 116, 2, 1, 1),
                 vf.Symbol(2, "Big", 128, 0x40, 2, 1, 1)]
        t = symtable([("OldTiny", 0x1000, 12), ("OldBig", 0x100C, 0x40)])
        entry = t.entries["OldTiny"]
        self.assertTrue(imp.neighbour_code(funcs, "Tiny", entry, t, lambda c, r: (c, r) == ("Big", "OldBig")))
        self.assertFalse(imp.neighbour_code(funcs, "Tiny", entry, t, lambda c, r: False))

    def test_pairing_records_accesses_and_named_conflicts_for_the_importer(self):
        symbols = symtable([("lbl_80000000", 0x80000000, 4), ("Retail", 0x1000, 8)])
        out = imp.Pairing()
        imp.pair_refs([(0, 109, "Time", 4), (4, 10, "Donor", 0)],
                      [(0, 109, "lbl_80000000", 0), (4, 10, "Retail", 0)], {}, symbols, "f", out)
        self.assertEqual(out.refs, [("Time", 4, "lbl_80000000", 0), ("Donor", 0, "Retail", 0)])
        self.assertEqual(out.named, {"Retail": {"Donor"}})
        self.assertEqual(list(out.addend_conflicts.values()), [("Time", 4, "lbl_80000000", 0)])
        self.assertEqual(list(out.named_text.values()), [("Retail", "Donor")])


class HeaderResolutionTests(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmp.cleanup)
        self.root = Path(self._tmp.name) / "repo"
        self.sdk = Path(self._tmp.name) / "sdk"
        for d in ("include", "src/dolphin/os", "src/dolphin/card"):
            (self.root / d).mkdir(parents=True)
        (self.sdk / "src/os").mkdir(parents=True)
        (self.sdk / "src/card").mkdir(parents=True)
        self._root = imp.ROOT
        imp.ROOT = self.root
        self.addCleanup(setattr, imp, "ROOT", self._root)

    def plan(self, source, flag_dirs, has_root=False):
        plan = imp.Plan("card/CARDMount", "card")
        imp.plan_headers(plan, self.sdk, source, flag_dirs, has_root)
        return plan

    def test_donor_local_directory_include_is_found_through_the_src_dolphin_root(self):
        (self.root / "src/dolphin/os/__os.h").write_text("")
        plan = self.plan('#include "os/__os.h"\n', {"os", "card"})
        self.assertEqual((plan.errors, plan.headers, plan.new_root_flag), ([], [], True))
        self.assertFalse(self.plan('#include "os/__os.h"\n', {"os", "card"}, True).new_root_flag)

    def test_directory_include_missing_from_the_repo_is_copied_from_the_donor_tree(self):
        (self.sdk / "src/os/__os.h").write_text("")
        plan = self.plan('#include "os/__os.h"\n', {"card"})
        self.assertEqual(plan.errors, [])
        self.assertEqual(plan.headers, [(self.sdk / "src/os/__os.h", self.root / "src/dolphin/os/__os.h")])
        self.assertTrue(plan.new_root_flag)
        self.assertFalse(plan.new_include_dir)

    def test_bare_local_header_is_still_copied_into_the_unit_directory(self):
        (self.sdk / "src/card/__card.h").write_text("")
        plan = self.plan('#include "__card.h"\n', set())
        self.assertEqual(plan.headers, [(self.sdk / "src/card/__card.h", self.root / "src/dolphin/card/__card.h")])
        self.assertTrue(plan.new_include_dir)
        self.assertFalse(plan.new_root_flag)

    def test_unknown_include_is_an_error_that_names_it(self):
        plan = self.plan('#include "os/nope.h"\n#include <gone.h>\n', {"os"})
        self.assertEqual(len(plan.errors), 2)
        self.assertIn('"os/nope.h"', plan.errors[0])
        self.assertIn("<gone.h>", plan.errors[1])

    def test_root_flag_is_added_once_after_the_library_flags(self):
        text = imp.add_root_flag(CONFIGURE)
        self.assertIn('"-i src/dolphin/card", "-i src/dolphin",', text)
        self.assertEqual(imp.add_root_flag(text), text)


class DonorSideProposalTests(unittest.TestCase):
    def test_retail_names_the_sdk_uses_are_not_rewritten_in_donor_copies(self):
        Prop = type("Prop", (), {})
        def prop(retail, donor):
            p = Prop()
            p.retail, p.donor = retail, donor
            return p
        with tempfile.TemporaryDirectory() as d:
            sdk = Path(d)
            (sdk / "src/dvd").mkdir(parents=True)
            (sdk / "include").mkdir()
            (sdk / "src/dvd/dvdfs.c").write_text("BOOL DVDPrepareStreamAsync(void) { return 0; }\n")
            got = imp.donor_side_proposals([prop("DVDPrepareStreamAsync", "cbForStateMotorStopped"),
                                            prop("fn_8016A2B0", "DVDCancelAsync"),
                                            prop("DVDCancelAsync", "fn_801435B8")], sdk)
        self.assertEqual(got, {"fn_8016A2B0": "DVDCancelAsync"})


class FailureSummaryTests(unittest.TestCase):
    def test_the_explaining_lines_come_first_and_are_not_buried_by_the_tail(self):
        noise = [f"[{i}/90] MWCC build/x{i}.o" for i in range(60)]
        out = "\n".join(["strip_unused: .text still references removed symbol F", "FAILED: build/a.o"]
                        + noise + ["ninja: build stopped"])
        got = imp.failure_summary(out)
        self.assertEqual(got.splitlines()[:2], ["strip_unused: .text still references removed symbol F",
                                                "FAILED: build/a.o"])
        self.assertIn("--- last 30 lines ---", got)
        self.assertTrue(got.endswith("ninja: build stopped"))

    def test_error_and_missing_lines_count_and_plain_output_is_just_the_tail(self):
        got = imp.failure_summary("noise\n" * 50 + "Error: x\nMissing configuration for y\n")
        self.assertTrue(got.startswith("Error: x\nMissing configuration for y\n--- last"))
        self.assertEqual(imp.failure_summary("a\nb\nc"), "a\nb\nc")


if __name__ == "__main__":
    unittest.main()
