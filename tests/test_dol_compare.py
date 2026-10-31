import contextlib
import io
import json
import struct
import tempfile
import unittest
from pathlib import Path

from tools import dol_compare

_quiet = None


def setUpModule():
    """Keep the tools' progress and usage output out of the test log."""
    global _quiet
    _quiet = contextlib.ExitStack()
    _quiet.enter_context(contextlib.redirect_stdout(io.StringIO()))
    _quiet.enter_context(contextlib.redirect_stderr(io.StringIO()))


def tearDownModule():
    _quiet.close()


def make_dol(text: bytes, data: bytes = b"", text_addr: int = 0x80003100, data_addr: int = 0x80400000) -> bytes:
    offsets = [0] * 18
    addrs = [0] * 18
    sizes = [0] * 18
    body = b""
    cursor = 0x100
    for idx, (blob, addr) in ((0, (text, text_addr)), (7, (data, data_addr))):
        if not blob:
            continue
        offsets[idx], addrs[idx], sizes[idx] = cursor, addr, len(blob)
        body += blob
        cursor += len(blob)
    header = struct.pack(">18I18I18I", *offsets, *addrs, *sizes).ljust(0x100, b"\0")
    return header + body


def word(opcode: int, rd: int, ra: int, imm: int) -> int:
    return (opcode << 26) | (rd << 21) | (ra << 16) | (imm & 0xFFFF)


class DolCompareTests(unittest.TestCase):
    def test_parse_reads_sections(self):
        dol = dol_compare.parse_dol(make_dol(b"\x01\x02\x03\x04", b"\xAA\xBB"))
        self.assertEqual(dol.read(0x80003100, 4), b"\x01\x02\x03\x04")
        self.assertEqual(dol.read(0x80400000, 2), b"\xAA\xBB")
        with self.assertRaises(ValueError):
            dol.read(0x80003102, 4)

    def test_identical_images_have_no_diffs(self):
        a = dol_compare.parse_dol(make_dol(b"\0" * 8, b"\1" * 4))
        self.assertEqual(dol_compare.diff_bytes(a, a), [])

    def test_diff_addresses_and_range_check(self):
        a = dol_compare.parse_dol(make_dol(b"\0" * 8, b"\1" * 4))
        b = dol_compare.parse_dol(make_dol(b"\0\0\0\0\0\0\0\xFF", b"\1" * 4))
        diffs = dol_compare.diff_bytes(a, b)
        self.assertEqual(diffs, [0x80003107])
        self.assertEqual(dol_compare.check_ranges(a, b, diffs, [(0x80003100, 0x80003104)]), [])
        self.assertEqual(dol_compare.check_ranges(a, b, diffs, [(0x80003104, 0x80003108)]), diffs)

    def test_classifies_sdata2_base_register(self):
        built = word(32, 0, 2, -0x7B28)
        ref = word(32, 0, 13, -0x4268)
        self.assertTrue(dol_compare.classify_word(built, ref).startswith("sdata2-via-r2"))
        self.assertEqual(dol_compare.classify_word(word(32, 0, 2, 4), word(32, 1, 13, 4)), "other")
        self.assertEqual(dol_compare.classify_word(word(15, 0, 2, 4), word(15, 0, 13, 4)), "other")

    def test_layout_mismatch_is_a_failure(self):
        with tempfile.TemporaryDirectory() as tmp:
            a, b = Path(tmp, "a.dol"), Path(tmp, "b.dol")
            a.write_bytes(make_dol(b"\0" * 8))
            b.write_bytes(make_dol(b"\0" * 12))
            self.assertEqual(dol_compare.main([str(a), str(b)]), 1)

    def test_main_fails_only_when_checked_range_differs(self):
        with tempfile.TemporaryDirectory() as tmp:
            a, b = Path(tmp, "a.dol"), Path(tmp, "b.dol")
            a.write_bytes(make_dol(b"\0" * 8))
            b.write_bytes(make_dol(b"\0\0\0\0\0\0\0\xFF"))
            self.assertEqual(dol_compare.main([str(a), str(b), "--range", "0x80003100:0x80003104"]), 0)
            self.assertEqual(dol_compare.main([str(a), str(b), "--range", "0x80003104:0x80003108"]), 1)

    def test_parse_splits(self):
        text = (
            "Sections:\n\t.text type:code align:4\n\n"
            "numath/nurand.c:\n\t.text start:0x8000C0C0 end:0x8000C1B8\n\t.sdata start:0x80407B40 end:0x80407B64\n"
        )
        self.assertEqual(
            dol_compare.parse_splits(text),
            {"numath/nurand.c": [(".text", 0x8000C0C0, 0x8000C1B8), (".sdata", 0x80407B40, 0x80407B64)]},
        )

    def test_linked_units_come_from_complete_objdiff_units(self):
        with tempfile.TemporaryDirectory() as tmp:
            objdiff = Path(tmp, "objdiff.json")
            splits = Path(tmp, "splits.txt")
            objdiff.write_text(json.dumps({"units": [
                {"name": "main/numath/nurand", "metadata": {"complete": True, "source_path": "src/numath/nurand.c"}},
                {"name": "main/numath/nuvec", "metadata": {"complete": False, "source_path": "src/numath/nuvec.c"}},
                {"name": "main/auto", "metadata": {"complete": True}},
            ]}))
            splits.write_text("numath/nurand.c:\n\t.text start:0x10 end:0x20\nnumath/nuvec.c:\n\t.text start:0x30 end:0x40\n")
            self.assertEqual(dol_compare.linked_unit_ranges(objdiff, splits), {"numath/nurand.c": [(0x10, 0x20)]})


if __name__ == "__main__":
    unittest.main()
