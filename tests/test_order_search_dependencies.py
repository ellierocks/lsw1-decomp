"""Captured aisys-2b auto-search candidates must preserve scalar and reader order."""
import ctypes
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

from tools import order_search as search


class DependencyTests(unittest.TestCase):
    def test_captured_loader_result_is_not_erased(self):
        # The original winning fn_80086A04 variant moved the zero after allocation.
        text = """int load(void)
{
    int result;
    int count;
    result = 0;
    result = allocate();
    count = 3;
    output->result = result;
    output->count = count;
    return result;
}
"""
        blocks, _, _ = search.auto_blocks(text, "load")
        template = search.Template(text, blocks, None)
        variants = [search.render(template, orders, variant)
                    for orders, variant in search.candidates(template, False)]
        self.assertTrue(all(rendered.index("result = 0;") < rendered.index("result = allocate();")
                            for rendered in variants), "allocation result erased by an automatic candidate")
        for rendered in variants:
            self.assertLess(rendered.index("count = 3;"), rendered.index("output->count = count;"))

    def test_readers_and_updates_are_barriers_but_fields_still_permute(self):
        text = """void load(void)
{
    p->a = read();
    p->b = read();
    p->c = 1;
    p->d = 2;
    i++;
    p->e = i;
    p->f = i;
}
"""
        blocks, _, _ = search.auto_blocks(text, "load")
        self.assertEqual([b.statements for b in blocks],
                         [["p->c = 1;", "p->d = 2;"], ["p->e = i;", "p->f = i;"]])

    def test_compound_writes_and_read_dependencies(self):
        for left, right in (("n = 3;", "p->x = n;"), ("n += 1;", "n = 0;"),
                            ("n >>= 1;", "p->x = n;"), ("p->x = n;", "n = 3;")):
            with self.subTest(left=left, right=right):
                self.assertTrue(search.scalar_dependency(left, right))
        self.assertFalse(search.scalar_dependency("p->a = n;", "p->b = n;"))

    def test_indirect_calls_are_barriers_without_blocking_raw_casts(self):
        for statement in ("(*reader)();", "(reader)(1);", "x = ((int (*)(int))reader)(1);",
                          "x = read();"):
            with self.subTest(statement=statement):
                self.assertTrue(search.has_call(statement))
        for statement in ("(*(s32 *)((s8 *)(p) + (8))) = flags;", "x = (u32)(n + 1);",
                          "x = (struct node *)(base + offset);"):
            with self.subTest(statement=statement):
                self.assertFalse(search.has_call(statement))

    def test_captured_path_loader_copy_stays_after_its_store(self):
        # Guarded replay exposed the same error through m2c's raw memory syntax.
        store = "(*(s32 *)((s8 *)(connection) + (8))) = flags;"
        copy = "(*(s32 *)((s8 *)(connection) + (0x10))) = (s32) (*(s32 *)((s8 *)(connection) + (8)));"
        self.assertTrue(search.memory_dependency(store, copy))
        self.assertTrue(search.memory_dependency("p->a = 3;", "p->b = p -> a;"))
        text = "void load(void)\n{\n    " + store + "\n    " + copy + "\n    x = 1;\n    y = 2;\n}\n"
        blocks, _, _ = search.auto_blocks(text, "load")
        template = search.Template(text, blocks, None)
        for orders, variant in search.candidates(template, False):
            rendered = search.render(template, orders, variant)
            self.assertLess(rendered.index(store), rendered.index(copy))

    @unittest.skipUnless(shutil.which("cc"), "host C compiler required")
    def test_every_generated_variant_executes_the_same_allocation(self):
        # Execute real C as well as checking the source ordering. Field stores remain
        # movable, so this exercises generated candidates rather than an empty search.
        text = """int load(void)
{
    int result;
    result = 0;
    result = allocate();
    out.a = 1;
    out.b = 2;
    return result + out.a + out.b;
}
"""
        blocks, _, _ = search.auto_blocks(text, "load")
        template = search.Template(text, blocks, None)
        variants = [search.render(template, orders, variant).replace("load(void)", f"load{i}(void)")
                    for i, (orders, variant) in enumerate(search.candidates(template, False))]
        self.assertEqual(len(variants), 2)
        with tempfile.TemporaryDirectory() as tmp:
            src, lib = Path(tmp) / "replay.c", Path(tmp) / "replay.so"
            src.write_text("struct { int a, b; } out;\nint allocate(void) { return 64; }\n" +
                           "\n".join(variants))
            subprocess.run(["cc", "-shared", "-fPIC", str(src), "-o", str(lib)], check=True,
                           capture_output=True, text=True)
            loaded = ctypes.CDLL(str(lib))
            for i in range(len(variants)):
                self.assertEqual(getattr(loaded, f"load{i}")(), 67)
