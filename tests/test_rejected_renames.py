"""Rename-suggestion tools must not re-propose names rejected for an address."""
import re
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import batch_rename, ls1_rename, mac_anchor_rename_queue as maq
from tools.rejected_renames import REJECTED_TSV, ROOT, load_rejected

SYMBOL_RE = re.compile(r"^(\S+)\s+=\s+\.\w+:0x([0-9A-Fa-f]+);")


class RejectedRenamesTests(unittest.TestCase):
    def test_list_parses_and_covers_pr5_addresses(self):
        rejected = load_rejected()
        self.assertEqual(rejected[0x80019634], {"NuQuatSlerp"})
        self.assertIn("NuAnimCurve2SetApplyToJoint", rejected[0x80017EA0])
        self.assertGreaterEqual(len(rejected), 16)

    def test_rejected_names_are_not_in_symbols_txt_at_that_address(self):
        rejected = load_rejected()
        for line in (ROOT / "config/GL5E4F/symbols.txt").read_text().splitlines():
            m = SYMBOL_RE.match(line)
            if m and ".text" in line:
                addr = int(m.group(2), 16)
                self.assertNotIn(m.group(1), rejected.get(addr, ()), line)

    def test_missing_file_is_empty(self):
        self.assertEqual(load_rejected(Path("/nonexistent/rejected.tsv")), {})

    def test_mac_anchor_queue_drops_rejected_candidate(self):
        gc = [
            maq.GcFunc("NuLeft", ".text", 0x1000, 0x40),
            maq.GcFunc("fn_00001040", ".text", 0x1040, 0x40),
            maq.GcFunc("NuRight", ".text", 0x1080, 0x40),
        ]
        mac = [
            maq.MacFunc("NuLeft", "NuLeft", 0x10, 0x40, "mac_lsw1_demo", "ppc", 5),
            maq.MacFunc("NuWrong", "NuWrong", 0x50, 0x40, "mac_lsw1_demo", "ppc", 5),
            maq.MacFunc("NuRight", "NuRight", 0x90, 0x40, "mac_lsw1_demo", "ppc", 5),
        ]
        kept = maq.build_candidates(gc, mac, {})
        self.assertEqual([c["new_name"] for c in kept], ["NuWrong"])
        dropped = maq.build_candidates(gc, mac, {0x1040: {"NuWrong"}})
        self.assertEqual(dropped, [])
        other_addr = maq.build_candidates(gc, mac, {0x2000: {"NuWrong"}})
        self.assertEqual(len(other_addr), 1)

    def test_suggest_filters_rejected_name_for_that_address_only(self):
        funcs = [(0x10, "NuAnimCurveCalcVal2", 4, ""), (0x20, "fn_00000020", 4, ""),
                 (0x30, "NuAnimDataLoad", 4, ""), (0x40, "fn_00000040", 4, "")]
        mac = {"NuAnimDataLoadBogus": "", "NuAnimDataLoadGood": ""}
        with tempfile.TemporaryDirectory() as tmp:
            tsv = Path(tmp) / "mac.tsv"
            tsv.write_text("\n".join(
                "\t".join(["0", "0", "0", "0", "text", "0", "0", n, d]) for n, d in mac.items()))
            with mock.patch.object(ls1_rename, "load_symtab_funcs", return_value=funcs), \
                    mock.patch.object(ls1_rename, "MAC_TSVS", [tsv]), \
                    mock.patch.object(ls1_rename, "load_rejected", return_value={0x20: {"NuAnimDataLoadBogus"}}):
                at_rejected = [n for n, _ in ls1_rename.find_mac_suggestions(0x20)]
                elsewhere = [n for n, _ in ls1_rename.find_mac_suggestions(0x40)]
        self.assertEqual(at_rejected, ["NuAnimDataLoadGood"])
        self.assertEqual(elsewhere, ["NuAnimDataLoadBogus", "NuAnimDataLoadGood"])


if __name__ == "__main__":
    unittest.main()
