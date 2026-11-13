import unittest
from tools import sweep
from sweep_repairs.pointer_differences import pointer_byte_differences


class PointerDifferenceTest(unittest.TestCase):
    def test_integer_base_load_before_reciprocal_multiply(self):
        body = "    void *node;\n    index = (s32) ((node - (*(s32 *)((s8 *)(path) + (0x78)))) * 0xBA2E8BA3) >> 3;"
        repaired = pointer_byte_differences(body)
        self.assertIn("((u32) node - (*(s32 *)", repaired)
        self.assertEqual(pointer_byte_differences(repaired), repaired)

    def test_leave_valid_or_ambiguous_arithmetic(self):
        for expression in (
            "(node - base) * 0xBA2E8BA3",
            "(node - (*(void **)(path + 0x78))) * 0xBA2E8BA3",
            "(node - (*(s32 *)(path + 0x78))) + 4",
            "(integer - (*(s32 *)(path + 0x78))) * 0xBA2E8BA3",
        ):
            body = "    void *node;\n    void *base;\n    index = " + expression + ";"
            self.assertEqual(pointer_byte_differences(body), body)

    def test_multiple_displacements(self):
        body = "    void *a;\n    void *b;\n    x = (a - (*(s32 *)p)) * 0x11;\n    y = (b - (*(s32 *)q)) * 0x13;"
        self.assertEqual(pointer_byte_differences(body).count("(u32) a"), 1)
        self.assertEqual(pointer_byte_differences(body).count("(u32) b"), 1)


if __name__ == "__main__":
    unittest.main()
