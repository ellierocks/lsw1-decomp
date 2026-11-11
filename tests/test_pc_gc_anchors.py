import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))

import pc_gc_anchors  # noqa: E402


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
