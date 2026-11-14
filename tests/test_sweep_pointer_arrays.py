import unittest

from tools import sweep  # Makes the sweep repair modules importable.
from sweep_repairs.pointer_arrays import pointer_array_bytes


class PointerArrayBytesTest(unittest.TestCase):
    def test_raw_ai_object_array_cursor(self):
        # fn_80088CD8: target advances r10 by four bytes and loads at an
        # independently computed byte offset, not four pointer elements.
        body = "void fn(void **arg2) {\n    void **var_r10;\n    var_r10 += 4;\n    object = *(s32 *)(arg2 + temp_r0_2);\n}"
        repaired = pointer_array_bytes(body)
        self.assertIn("var_r10 += 1;", repaired)
        self.assertIn("*(s32 *)((u8 *)arg2 + temp_r0_2)", repaired)
        self.assertEqual(pointer_array_bytes(repaired), repaired)

    def test_signed_and_hex_steps(self):
        body = "    s32 **cursor;\n    cursor -= 0x4;\n    cursor += 1;\n    cursor += 04;\n    cursor += 16;"
        repaired = pointer_array_bytes(body)
        self.assertIn("cursor -= 1;", repaired)
        self.assertIn("cursor += 1;", repaired)
        self.assertNotIn("cursor += 04;", repaired)
        self.assertEqual(repaired.count("cursor += 1;"), 2)
        self.assertIn("cursor += 16;", repaired)

    def test_preserve_indexing_and_unrelated_arithmetic(self):
        body = "void fn(void **p, s32 *q) {\n    x = p[4];\n    y = p + i;\n    q += 4;\n    p += count;\n}"
        self.assertEqual(pointer_array_bytes(body), body)


if __name__ == "__main__":
    unittest.main()
