import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))

import pc_gc_anchors  # noqa: E402
import pc_gc_features  # noqa: E402
import pc_gc_splits  # noqa: E402
from pc_gc_features import Features  # noqa: E402


def make_pairer(gc_calls=None, pc_calls=None, gc_addrs=None, gc_sizes=None, gc_strs=None, pc_strs=None,
                gc_feat=None, pc_feat=None):
    pc_calls = pc_calls or {}
    gc_addrs = gc_addrs or {}
    return pc_gc_anchors.Pairer(gc_strs or {}, pc_strs or {}, gc_calls or {}, pc_calls, gc_addrs,
                                gc_sizes or {g: 0x40 for g in gc_addrs}, gc_feat or Features(),
                                pc_feat or Features())


class PropagateTest(unittest.TestCase):
    def test_single_gap_between_fixed_points_is_paired(self):
        pairs = {"parent": (0x100, "seed"), "a": (0x200, "seed"), "c": (0x400, "seed")}
        gc_calls = {"parent": ["a", "b", "c", "d", "e"]}
        pc_calls = {0x100: [0x200, 0x300, 0x400, 0x500, 0x600, 0x700]}
        added = pc_gc_anchors.propagate(pairs, gc_calls, pc_calls)
        self.assertEqual(pairs["b"][0], 0x300)
        self.assertTrue(pairs["b"][1].startswith("callee of parent"))
        # The tail gap differs in length (2 vs 3), so d and e stay unpaired.
        self.assertNotIn("d", pairs)
        self.assertEqual(added, 1)

    def test_wider_gap_needs_equal_call_lists(self):
        pairs = {"parent": (0x100, "seed")}
        pc_calls = {0x100: [0x200, 0x300, 0x400]}
        pc_gc_anchors.propagate(pairs, {"parent": ["a", "b", "c"]}, pc_calls)
        self.assertEqual(pairs["b"][0], 0x300)

        pairs = {"parent": (0x100, "seed")}
        pc_calls = {0x100: [0x200, 0x300, 0x400, 0x500]}
        pc_gc_anchors.propagate(pairs, {"parent": ["a", "b", "c"]}, pc_calls)
        self.assertNotIn("b", pairs)

    def test_conflicting_proposals_are_dropped(self):
        pairs = {"p1": (0x100, "seed"), "p2": (0x110, "seed")}
        gc_calls = {"p1": ["x"], "p2": ["x"]}
        pc_calls = {0x100: [0x200], 0x110: [0x300]}
        self.assertEqual(pc_gc_anchors.propagate(pairs, gc_calls, pc_calls), 0)
        self.assertNotIn("x", pairs)


class PairerTest(unittest.TestCase):
    def test_layout_pairs_single_gap_between_adjacent_pairs(self):
        gc_addrs = {"a": 0x1000, "b": 0x1040, "c": 0x1080}
        pc_calls = {0x100: [], 0x140: [], 0x180: []}
        pairer = make_pairer(gc_addrs=gc_addrs, pc_calls=pc_calls)
        pairs = {"a": (0x100, "seed"), "c": (0x180, "seed")}
        self.assertEqual(pairer.step_layout(pairs), 1)
        self.assertEqual(pairs["b"][0], 0x140)
        self.assertEqual(pc_gc_anchors.method(pairs["b"][1]), "layout")

    def test_disjoint_message_strings_veto_but_source_paths_do_not(self):
        pairer = make_pairer(gc_addrs={"g": 0x1000}, pc_calls={0x100: [], 0x140: [], 0x180: []},
                             gc_strs={"Coin (Blue)": {"g"}, "../ngc_obj/../a.c": {"g"}},
                             pc_strs={"d3dSetVertexShader": {0x100}, ".\\a.c": {0x140}})
        self.assertFalse(pairer.consistent("g", 0x100, {}))
        self.assertTrue(pairer.consistent("g", 0x140, {}))
        pairer.hidden = frozenset({"g"})  # held out: strings cannot be consulted
        self.assertTrue(pairer.consistent("g", 0x100, {}))

    def test_table_slots_pair_when_two_slots_agree(self):
        gc = Features(pointers=[(0x80200000, "a"), (0x80200004, "b"), (0x80200008, "c")])
        pc = Features(pointers=[(0x700000, 0x100), (0x700004, 0x140), (0x700008, 0x180)])
        pairer = make_pairer(gc_addrs={"a": 0x1000, "b": 0x2000, "c": 0x3000},
                             pc_calls={0x100: [], 0x140: [], 0x180: []}, gc_feat=gc, pc_feat=pc)
        pairs = {"a": (0x100, "seed")}
        self.assertEqual(pairer.step_tables(pairs), 0)  # one agreeing slot is not enough
        pairs["c"] = (0x180, "seed")
        self.assertEqual(pairer.step_tables(pairs), 1)
        self.assertEqual(pairs["b"][0], 0x140)

    def test_run_seeds_from_unique_strings_and_reports_splits(self):
        gc_strs = {"hello world": {"g"}, "split me": {"h"}}
        pc_strs = {"hello world": {0x100}, "split me": {0x100}}
        pairer = make_pairer(gc_addrs={"g": 0x1000, "h": 0x2000}, pc_calls={0x100: []},
                             gc_strs=gc_strs, pc_strs=pc_strs)
        pairs, split = pairer.run({})
        self.assertEqual(len(pairs), 1)
        self.assertEqual(len(split), 1)
        self.assertEqual(pc_gc_anchors.method(split[0][2]), "ambiguous")

    def test_method_names(self):
        for evidence, name in [("strings 2: x", "strings"), ("callee of f", "callee"),
                               ("table slot at 0x80200000", "table"),
                               ("sole other user of global 0x80400000 (x86 0x00700000)", "global"),
                               ("layout between a and b", "layout"), ("shape, callee of f", "shape-callee"),
                               ("shape, layout between a and b", "shape-layout")]:
            self.assertEqual(pc_gc_anchors.method(evidence), name)

    def test_load_anchors_skips_ambiguous_rows(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp, "a.tsv")
            path.write_text("# header\ngc_addr\tgc_function\tpc_function\tevidence\n"
                            "0x80001000\tf\t0x00401000\tcallee of g\n"
                            "0x80002000\th\t0x00402000\tstrings split 1/2: x\n")
            self.assertEqual(pc_gc_anchors.load_anchors(path), {0x80001000: (0x401000, "callee")})
            self.assertIsNone(pc_gc_anchors.load_anchors(Path(tmp, "missing.tsv")))


class X86ListingTest(unittest.TestCase):
    LISTING = """
  401000:\tpush   ebp
  401001:\tfld    DWORD PTR ds:0x646010
  401007:\tmov    eax,DWORD PTR ds:0x750000
  40100c:\tcall   0x401020
  401011:\tjmp    0x750004
  401016:\tret
  401020:\tmov    DWORD PTR ds:0x750000,eax
  401025:\tret
"""

    def test_calls_and_instruction_boundaries(self):
        insns, calls = pc_gc_features.x86_listing(self.LISTING)
        self.assertIn(0x401020, insns)
        self.assertNotIn(0x401021, insns)
        self.assertEqual(calls, [(0x40100C, 0x401020)])

    def test_refs_and_floats_per_function(self):
        def read(addr, n):
            return b"\x00\x00\x80\x3f" if addr == 0x646010 else None
        refs, floats = pc_gc_features.parse_x86(self.LISTING, [0x401000, 0x401020], 0x646000, 0x760000,
                                                (0x646000, 0x650000), read)
        self.assertEqual(refs, {0x401000: [0x750000], 0x401020: [0x750000]})  # jmp operand skipped
        self.assertEqual(floats, {0x401000: [1.0]})


class SplitLeadsTest(unittest.TestCase):
    def test_lead_inside_one_unit_conflicts(self):
        units = pc_gc_splits.text_units(
            "Sections:\n\t.text type:code\n\na.c:\n\t.text       start:0x80001000 end:0x80003000\n"
            "b.c:\n\t.text       start:0x80003000 end:0x80004000\n")
        self.assertEqual(units, [(0x80001000, 0x80003000, "a.c"), (0x80003000, 0x80004000, "b.c")])
        extents = {".\\a.c": (0x100, 0x1FF), ".\\b.c": (0x200, 0x2FF)}
        pairs = {0x80001000: (0x110, "x"), 0x80002000: (0x210, "x"), 0x80003100: (0x220, "x")}
        (lead,) = pc_gc_splits.leads(pairs, extents, units)
        self.assertEqual((lead["after"]["last"], lead["before"]["first"]), (0x80001000, 0x80002000))
        self.assertEqual(lead["conflict"], "inside a.c")
        pairs[0x80002000] = (0x120, "x")
        (lead,) = pc_gc_splits.leads(pairs, extents, units)
        self.assertEqual(lead["conflict"], "")


class FindExeTest(unittest.TestCase):
    def test_worktree_falls_back_to_primary_checkout(self):
        with tempfile.TemporaryDirectory() as tmp:
            worktree, primary = Path(tmp, "wt"), Path(tmp, "primary")
            rel = pc_gc_anchors.PC_EXE_CANDIDATES[0].relative_to(pc_gc_anchors.ROOT)
            (primary / rel).parent.mkdir(parents=True)
            (primary / rel).write_bytes(b"MZ")
            candidates = [worktree / rel]
            with mock.patch.object(pc_gc_anchors, "ROOT", worktree), \
                    mock.patch.object(pc_gc_anchors, "PC_EXE_CANDIDATES", candidates), \
                    mock.patch.object(pc_gc_anchors, "primary_root", return_value=primary):
                self.assertEqual(pc_gc_anchors.find_exe(), primary / rel)
                (primary / rel).unlink()
                self.assertIsNone(pc_gc_anchors.find_exe())


if __name__ == "__main__":
    unittest.main()
