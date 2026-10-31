import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import sdk_import_symbols as sym  # noqa: E402
import verify_fn as vf  # noqa: E402


def table(rows):
    """rows: (name, section, addr, size[, is_function])"""
    t = vf.SymbolTable.__new__(vf.SymbolTable)
    t.entries = {r[0]: vf.SymEntry(r[1], r[2], r[3], r[4] if len(r) > 4 else r[1] == ".text") for r in rows}
    t.duplicates = set()
    return t


def prop(addr, retail, donor, size=0x40, identical=True, **kw):
    return sym.Proposal(addr, size, retail, donor, "code", identical, **kw)


class ProposalTests(unittest.TestCase):
    def test_description_names_both_symbols_and_the_evidence(self):
        p = prop(0x80175F7C, "__CARDSyncCallback", "__CARDExtHandler", 0xD8)
        self.assertEqual(p.describe(), "retail __CARDSyncCallback at 80175F7C vs donor __CARDExtHandler, "
                                       "same size (0xd8), code identical")

    def test_code_evidence_is_required(self):
        t = table([("Old", ".text", 0x100, 0x40)])
        p = prop(0x100, "Old", "New", identical=False)
        sym.resolve_proposals([p], t, {})
        self.assertEqual(p.status, "unproven")
        self.assertEqual(sym.proposal_renames([p]), {})

    def test_rejected_pair_is_never_accepted(self):
        t = table([("Old", ".text", 0x100, 0x40)])
        p = prop(0x100, "Old", "New")
        sym.resolve_proposals([p], t, {0x100: {"New"}})
        self.assertEqual(p.status, "rejected")
        self.assertEqual(sym.proposal_renames([p]), {})

    def test_free_name_is_accepted_without_demotion(self):
        t = table([("Old", ".text", 0x100, 0x40)])
        p = prop(0x100, "Old", "New")
        sym.resolve_proposals([p], t, {})
        self.assertEqual((p.status, p.demote), ("ok", None))
        self.assertEqual(sym.proposal_renames([p]), {"Old": "New"})

    def test_shifted_run_of_names_moves_together(self):
        t = table([("A", ".text", 0x100, 0x40), ("B", ".text", 0x140, 0x40), ("C", ".text", 0x180, 0x40)])
        props = [prop(0x100, "A", "B"), prop(0x140, "B", "C"), prop(0x180, "C", "D")]
        sym.resolve_proposals(props, t, {})
        self.assertEqual([p.status for p in props], ["ok", "ok", "ok"])
        self.assertEqual(sym.proposal_renames(props), {"A": "B", "B": "C", "C": "D"})

    def test_a_rejected_link_leaves_its_holder_to_be_demoted(self):
        t = table([("A", ".text", 0x100, 0x40), ("B", ".text", 0x140, 0x40)])
        props = [prop(0x100, "A", "B"), prop(0x140, "B", "C")]
        sym.resolve_proposals(props, t, {0x140: {"C"}})
        self.assertEqual([p.status for p in props], ["ok", "rejected"])
        self.assertEqual(sym.proposal_renames(props), {"A": "B", "B": "fn_00000140"})

    def test_wrongly_named_game_function_is_demoted_to_a_placeholder(self):
        t = table([("fn_80173404", ".text", 0x80173404, 0x1B8), ("__AXOutNewFrame", ".text", 0x80140504, 0x188)])
        p = prop(0x80173404, "fn_80173404", "__AXOutNewFrame", 0x1B8, matches=lambda n: False)
        sym.resolve_proposals([p], t, {}, (0x80173404, 0x80173C10))
        self.assertEqual((p.status, p.demote), ("ok", "fn_80140504"))
        self.assertEqual(sym.proposal_renames([p]), {"fn_80173404": "__AXOutNewFrame",
                                                     "__AXOutNewFrame": "fn_80140504"})

    def test_same_named_function_in_another_split_is_not_demoted(self):
        t = table([("Old", ".text", 0x100, 0x50), ("OnReset", ".text", 0x80160BB8, 0x3C)])
        p = prop(0x100, "Old", "OnReset", 0x50, matches=lambda n: False)
        sym.resolve_proposals([p], t, {}, None, lambda a: "dolphin/os/OSMemory" if a == 0x80160BB8 else None)
        self.assertEqual(p.status, "blocked")
        self.assertIn("different function with the same name", p.detail)

    def test_identical_copy_elsewhere_and_data_holders_block(self):
        t = table([("Old", ".text", 0x100, 0x40), ("Twin", ".text", 0x200, 0x40), ("Var", ".bss", 0x300, 4, False)])
        twin = prop(0x100, "Old", "Twin", matches=lambda n: True)
        data = prop(0x100, "Old", "Var")
        sym.resolve_proposals([twin], t, {})
        sym.resolve_proposals([data], t, {})
        self.assertEqual((twin.status, data.status), ("blocked", "blocked"))

    def test_two_proposals_for_one_donor_name_and_a_holder_inside_the_unit_block(self):
        t = table([("A", ".text", 0x100, 0x40), ("B", ".text", 0x140, 0x40), ("In", ".text", 0x180, 0x40)])
        both = [prop(0x100, "A", "X"), prop(0x140, "B", "X")]
        sym.resolve_proposals(both, t, {})
        self.assertEqual([p.status for p in both], ["ok", "blocked"])
        inside = prop(0x100, "A", "In")
        sym.resolve_proposals([inside], t, {}, (0x100, 0x1C0))
        self.assertEqual(inside.status, "blocked")

    def test_invalid_identifier_is_blocked(self):
        t = table([("Old", ".text", 0x100, 0x40)])
        p = prop(0x100, "Old", "1bad")
        sym.resolve_proposals([p], t, {})
        self.assertEqual(p.status, "blocked")


class RewriteTests(unittest.TestCase):
    def test_swaps_and_chains_are_simultaneous_and_whole_word(self):
        got = sym.rewrite_identifiers("A(); B(); AB(); C();", {"A": "B", "B": "A", "C": "D"})
        self.assertEqual(got, "B(); A(); AB(); D();")

    def test_empty_map_is_identity(self):
        self.assertEqual(sym.rewrite_identifiers("x", {}), "x")


SYMBOLS = [
    ("lbl_80000100", ".sbss", 0x80000100, 4, False), ("lbl_80000104", ".sbss", 0x80000104, 4, False),
    ("lbl_80000200", ".bss", 0x80000200, 0x30, False),
    ("Named", ".bss", 0x80000300, 0x10, False),
]


class LayoutTests(unittest.TestCase):
    def plan(self, section, objs, refs, rows=SYMBOLS, referenced=lambda n: False):
        return sym.plan_layout(section, objs, refs, table(rows), referenced)

    def test_two_labels_of_one_donor_object_are_merged(self):
        objs = [sym.DonorObject("Time", ".sbss", 0, 8)]
        got = self.plan(".sbss", objs, [("Time", 0, "lbl_80000100", 0), ("Time", 4, "lbl_80000104", 0)])
        self.assertEqual(got.replace, {"lbl_80000100": [("Time", 0x80000100, 8)], "lbl_80000104": []})
        self.assertEqual(got.merged, {"Time"})
        self.assertEqual(got.manual, [])

    def test_covering_symbol_is_split_when_the_objects_tile_it(self):
        objs = [sym.DonorObject("A", ".bss", 0, 0x10), sym.DonorObject("B", ".bss", 0x10, 0x20)]
        got = self.plan(".bss", objs, [("A", 0, "lbl_80000200", 0), ("B", 0, "lbl_80000200", 0x10)])
        self.assertEqual(got.replace, {"lbl_80000200": [("A", 0x80000200, 0x10), ("B", 0x80000210, 0x20)]})

    def test_objects_that_do_not_tile_the_symbol_are_reported_not_applied(self):
        objs = [sym.DonorObject("A", ".bss", 0, 0x10)]
        got = self.plan(".bss", objs, [("A", 0, "lbl_80000200", 0)])
        self.assertEqual(got.replace, {})
        self.assertIn("do not tile it", got.manual[0])
        self.assertIn("A@0x0+0x10", got.manual[0])

    def test_named_or_used_symbols_are_left_alone(self):
        objs = [sym.DonorObject("A", ".bss", 0, 8), sym.DonorObject("B", ".bss", 8, 8)]
        refs = [("A", 0, "Named", 0)]
        got = self.plan(".bss", objs, refs)
        self.assertEqual(got.replace, {})
        self.assertIn("Named is named", got.manual[0])
        used = self.plan(".bss", [sym.DonorObject("A", ".bss", 0, 0x30)], [("A", 0, "lbl_80000200", 0)],
                         referenced=lambda n: n == "lbl_80000200")
        self.assertEqual(used.replace, {})

    def test_merge_needs_a_relocation_naming_the_object(self):
        objs = [sym.DonorObject("Time", ".sbss", 0, 8), sym.DonorObject("Other", ".sbss", 0x40, 4)]
        got = self.plan(".sbss", objs, [("Other", 0, "lbl_80000100", 0)])
        self.assertEqual(got.replace, {})

    def test_inconsistent_bias_is_reported(self):
        objs = [sym.DonorObject("A", ".sbss", 0, 4), sym.DonorObject("B", ".sbss", 4, 4)]
        got = self.plan(".sbss", objs, [("A", 0, "lbl_80000100", 0), ("B", 0, "lbl_80000100", 0)])
        self.assertEqual(got.replace, {})
        self.assertIn("inconsistent", got.manual[0])

    def test_objects_without_a_symbol_are_reported(self):
        objs = [sym.DonorObject("A", ".sbss", 0, 4), sym.DonorObject("Gone", ".sbss", 0x100, 4)]
        got = self.plan(".sbss", objs, [("A", 0, "lbl_80000100", 0)])
        self.assertIn("Gone", got.manual[0])
        self.assertIn("has no symbol", got.manual[0])


class MergeHalvesTests(unittest.TestCase):
    ROWS = [("OSStart", ".sbss", 0x8040A858, 4, False), ("lbl_8040A85C", ".sbss", 0x8040A85C, 4, False)]

    def test_second_word_access_merges_the_next_label(self):
        got = sym.merge_halves([("OSStart", 4, "lbl_8040A85C", 0)], table(self.ROWS), lambda n: False)
        self.assertEqual(got.replace, {"OSStart": [("OSStart", 0x8040A858, 8)], "lbl_8040A85C": []})
        self.assertEqual(got.merged, {"OSStart"})

    def test_first_half_may_still_be_an_unnamed_label_being_renamed(self):
        rows = [("lbl_8040A858", ".sbss", 0x8040A858, 4, False), self.ROWS[1]]
        got = sym.merge_halves([("OSStart", 4, "lbl_8040A85C", 0)], table(rows), lambda n: False,
                               {"lbl_8040A858": "OSStart"})
        self.assertEqual(got.replace["lbl_8040A858"], [("OSStart", 0x8040A858, 8)])
        self.assertIn("OSStart", got.touched)

    def test_other_shapes_are_not_merged(self):
        t = table(self.ROWS)
        for access in [("OSStart", 8, "lbl_8040A85C", 0), ("OSStart", 4, "lbl_8040A85C", 4),
                       ("OSStart", 4, "OSStart", 0)]:
            self.assertEqual(sym.merge_halves([access], t, lambda n: False).replace, {})
        self.assertEqual(sym.merge_halves([("OSStart", 4, "lbl_8040A85C", 0)], t, lambda n: True).replace, {})
        odd = table([("X", ".sbss", 0x8040A85C, 4, False), ("lbl_8040A860", ".sbss", 0x8040A860, 4, False)])
        self.assertEqual(sym.merge_halves([("X", 4, "lbl_8040A860", 0)], odd, lambda n: False).replace, {})


class ReplaceObjectsTests(unittest.TestCase):
    TEXT = ("A = .sbss:0x80000000; // type:object size:0x4 data:4byte\n"
            "lbl_B = .sbss:0x80000004; // type:object size:0x4 data:4byte\n"
            "C = .bss:0x80000100; // type:object size:0x30 align:32 data:4byte\n"
            "other = .text:0x1; // type:function size:0x4\n")

    def test_merge_widens_and_deletes_the_second_label(self):
        got = sym.replace_objects(self.TEXT, {"A": [("T", 0x80000000, 8)], "lbl_B": []}, {"T"})
        self.assertEqual(got.splitlines()[0], "T = .sbss:0x80000000; // type:object size:0x8 data:8byte")
        self.assertNotIn("lbl_B", got)

    def test_split_keeps_attributes_that_still_fit_and_drops_those_that_do_not(self):
        got = sym.replace_objects(self.TEXT, {"C": [("X", 0x80000100, 0x11), ("Y", 0x80000111, 0x1F)]})
        lines = [l for l in got.splitlines() if l.startswith(("X", "Y"))]
        self.assertEqual(lines[0], "X = .bss:0x80000100; // type:object size:0x11 align:32")
        self.assertEqual(lines[1], "Y = .bss:0x80000111; // type:object size:0x1F")
        self.assertIn("other = .text", got)


if __name__ == "__main__":
    unittest.main()
