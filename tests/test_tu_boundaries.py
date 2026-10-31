import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import tu_boundaries as tb  # noqa: E402


def lis(rd, hi):
    return (15 << 26) | (rd << 21) | (hi & 0xFFFF)


def addi(rd, ra, lo):
    return (14 << 26) | (rd << 21) | (ra << 16) | (lo & 0xFFFF)


def lwz(rd, ra, off):
    return (32 << 26) | (rd << 21) | (ra << 16) | (off & 0xFFFF)


BL = (18 << 26) | 1


class UnitNameTests(unittest.TestCase):
    def test_nu2api_path(self):
        self.assertEqual(tb.unit_name("../nu2api.legogc/nucore/ngc/ngc_obj/../nufile_ngc.c"),
                         "nucore/ngc/nufile_ngc.c")

    def test_gameapi_path(self):
        self.assertEqual(tb.unit_name("../gameapi.legogc/gamelib/ngc_obj/../terrain.c"), "gamelib/terrain.c")

    def test_game_path(self):
        self.assertEqual(tb.unit_name("ngc_obj/../main.c"), "game/main.c")


class ScanRefsTests(unittest.TestCase):
    def test_lis_addi_pair(self):
        self.assertEqual(tb.scan_refs([lis(3, 0x8020), addi(3, 3, -0x10)]), [0x801FFFF0])

    def test_load_through_tracked_base(self):
        self.assertEqual(tb.scan_refs([lis(4, 0x8020), lwz(5, 4, 0x18)]), [0x80200018])

    def test_call_clobbers_volatile(self):
        self.assertEqual(tb.scan_refs([lis(4, 0x8020), BL, lwz(5, 4, 0x18)]), [])

    def test_call_keeps_nonvolatile(self):
        self.assertEqual(tb.scan_refs([lis(31, 0x8020), BL, lwz(5, 31, 4)]), [0x80200004])

    def test_load_overwrites_base(self):
        self.assertEqual(tb.scan_refs([lis(4, 0x8020), lwz(4, 4, 0), lwz(5, 4, 8)]), [0x80200000])


class AlignTests(unittest.TestCase):
    def test_follows_strong_evidence(self):
        cost = [[0, 5, 5], [0, 5, 5], [5, 0, 5], [5, 5, 0]]
        self.assertEqual(tb.align(cost, [False] * 4), [0, 0, 1, 2])

    def test_mac_adjacency_holds_pair_together(self):
        cost = [[0, 1], [1, 0.5]]
        self.assertEqual(tb.align(cost, [False, False]), [0, 1])
        self.assertEqual(tb.align(cost, [False, True]), [0, 0])

    def test_slack_spans_evidence_free_functions(self):
        cost = [[0, 9], [0, 0], [0, 0], [9, 0]]
        path = tb.align(cost, [False] * 4)
        segs = tb.segments(path, cost, [False] * 4)
        self.assertEqual(len(segs), 2)
        self.assertEqual(segs[1].first - segs[1].slack[0], 1)
        self.assertEqual(segs[1].first + segs[1].slack[1], 3)


class ProposedRangeTests(unittest.TestCase):
    def test_cuts_around_existing_text_split(self):
        functions = [tb.Function("a", 0x100, 0x10), tb.Function("b", 0x110, 0x10),
                     tb.Function("c", 0x120, 0x10), tb.Function("d", 0x130, 0x10)]
        anchors = [tb.Anchor(0x9000, "../a.c", "game/a.c")]
        segs = [tb.Segment(1, 0, 3)]
        units = [tb.Unit("game/b.c", {".text": (0x110, 0x120)})]
        self.assertEqual(tb.proposed_ranges(segs, anchors, functions, units, 0x140),
                         [("auto/game/a_1.c", 0x100, 0x110), ("auto/game/a_2.c", 0x120, 0x140)])

    def test_parse_splits_round_trip(self):
        text = tb.splits_with("Sections:\n\t.text type:code\n", [("auto/game/a.c", 0x100, 0x140)])
        units = tb.parse_splits(text)
        self.assertEqual([(u.name, u.sections) for u in units], [("auto/game/a.c", {".text": (0x100, 0x140)})])


if __name__ == "__main__":
    unittest.main()
