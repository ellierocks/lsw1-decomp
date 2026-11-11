"""Synthetic .rodata layouts for tools/rodata_layout.py; no retail bytes."""
import struct
import unittest

from tools import rodata_layout as rl

HDR = b"../nu2api.legogc/numath/ngc/VuMath.h\0"
MSG = b"VuMtxInitRot NOT fully implemented!\0"
BASE = 0x80100000


def f32(v):
    return struct.pack(">f", v)


def pad4(b):
    return b + bytes(-len(b) % 4)


def header_copy():
    return pad4(HDR) + pad4(MSG)


class CarveTests(unittest.TestCase):
    def test_floats_doubles_and_strings(self):
        path = b"../nu2api.legogc/numath/ngc_obj/../foo.c\0"
        data = f32(1.0) + bytes(4) + b"\x43\x30\x00\x00\x80\x00\x00\x00" + pad4(path)
        items = rl.carve(data, BASE, {0: (4, {"A"}), 8: (8, {"A"})})
        self.assertEqual([i.kind for i in items], ["f32", "zero", "f64", "path", "zero"])
        self.assertEqual(items[0].refs, ["A"])

    def test_short_printable_word_is_a_float(self):
        # 0.9f is 0x3F666666, which reads "?fff" followed by a zero byte of the next word.
        data = f32(0.9) + bytes(4)
        items = rl.carve(data, BASE, {0: (4, {"A"})})
        self.assertEqual(items[0].kind, "f32")

    def test_header_copy_marks_both_strings(self):
        items = rl.carve(header_copy(), BASE)
        self.assertEqual([i.kind for i in items if i.kind != "zero"], ["header", "header"])

    def test_dol_refs(self):
        data = f32(1.0) + f32(2.0)
        items = rl.carve(data, BASE, {0: (4, {"A"})}, reached={0})
        self.assertEqual([i.dol_refs for i in items], [1, 0])


class ClassifyTests(unittest.TestCase):
    def test_stripped_literal_is_not_wrong_order(self):
        # Retail: used 1.0f, stripped 1.0f, used 0.0f. The build lacks the stripped word.
        retail = rl.carve(f32(1.0) + f32(1.0) + f32(0.0), BASE,
                          {0: (4, {"A"}), 8: (4, {"B"})}, reached={0, 8})
        built = rl.carve(f32(1.0) + f32(0.0), BASE, {0: (4, {"A"}), 4: (4, {"B"})})
        d = rl.classify(retail, built, BASE, "numath/foo")
        self.assertEqual(d["first_divergence"], 4)
        self.assertEqual(d["bytes_by_class"], {"stripped": 4})

    def test_extra_header_copy_is_extra_tu(self):
        data = f32(1.0) + header_copy() + f32(2.0) + header_copy()
        retail = rl.carve(data, BASE, {0: (4, {"A"})}, reached={0})
        d = rl.classify(retail, rl.carve(f32(1.0), BASE, {0: (4, {"A"})}), BASE)
        self.assertEqual(d["header_copies"], 2)
        self.assertIn("header copy", d["bytes_by_class"])
        self.assertIn("extra TU", d["bytes_by_class"])

    def test_identical_layout(self):
        data = f32(1.0) + f32(0.0)
        refs = {0: (4, {"A"}), 4: (4, {"A"})}
        d = rl.classify(rl.carve(data, BASE, refs), rl.carve(data, BASE, refs), BASE)
        self.assertIsNone(d["first_divergence"])
        self.assertEqual(d["mismatches"], [])


class SegmentTests(unittest.TestCase):
    def test_cut_at_padding_before_header(self):
        data = f32(1.0) + bytes(4) + header_copy() + f32(2.0)
        items = rl.carve(data, BASE, {0: (4, {"A"}), len(data) - 4: (4, {"B"})})
        segs = rl.tu_segments(items)
        self.assertEqual([(s["start"], s["functions"], s["header"]) for s in segs],
                         [(0, ["A"], False), (4, ["B"], True)])

    def test_function_offsets(self):
        rows = rl.function_offsets({"A": [0, 4], "B": [8]}, {"A": [0, 4], "B": [4]})
        self.assertEqual({r["function"]: r["aligned"] for r in rows}, {"A": True, "B": False})


class PlaceholderTests(unittest.TestCase):
    def test_words_strings_and_section_attribute(self):
        data = f32(1.0) + pad4(b"../x/foo.c\0") + f32(1.0) + f32(2.0) + f32(3.0)
        decls = rl.placeholder_decls(data, BASE, 0, len(data))
        self.assertEqual(decls, [
            'static const u32 unk_80100000[1] __attribute__((section(".rodata"))) = { 0x3F800000 };',
            'static const char unk_80100004[] = "../x/foo.c";',
            "static const u32 unk_80100010[3] = { 0x3F800000, 0x40000000, 0x40400000 };",
        ])

    def test_rejects_nonzero_string_padding(self):
        data = b"abcdefghi\0\x01\x00"
        with self.assertRaises(ValueError):
            rl.placeholder_decls(data, BASE, 0, len(data))

    def test_escapes(self):
        self.assertEqual(rl._c_string(b'a"b\\c\n'), '"a\\"b\\\\c\\n"')


if __name__ == "__main__":
    unittest.main()
