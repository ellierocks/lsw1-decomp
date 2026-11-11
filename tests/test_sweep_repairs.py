"""The sweep repair registry: stage order, slot collisions and drop-in modules."""
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path

from tools import sweep  # noqa: F401  (puts tools/ on sys.path)
import sweep_repairs


class RegistryTest(unittest.TestCase):
    def test_stage_order_matches_sweep(self):
        # The order sweep.py hard-coded before the split. A new repair may take any free
        # slot without editing this list; these keep their relative order.
        before = {
            "frame": ["empty_frame", "frame_buffer"],
            "scored": ["byte_strides", "base_pointer", "loop_guard", "index_loops", "countdown_loops",
                       "positive_guards", "switch_chain"],
            "variant": ["store_in_arms", "const_rodata", "both_rewrites", "wide_return", "void_callees",
                        "sorted_stores"],
            "variants": ["swap_stores", "switch_pivots", "float_positions", "volatile_locals", "int_callees",
                         "float_blocks"],
        }
        self.assertEqual(set(sweep_repairs.STAGES), set(before))
        for stage, names in before.items():
            got = [f.__name__ for f in sweep_repairs.stage(stage)]
            self.assertEqual([n for n in got if n in names], names, stage)
            self.assertEqual(sweep_repairs.stage(stage), sweep_repairs.stage(stage))

    def test_taken_slot_or_name_raises(self):
        reg = sweep_repairs.Registry()

        @reg.repair("scored", 10)
        def first(body):
            return body

        with self.assertRaisesRegex(ValueError, r"\('scored', 10\) is taken by first"):
            @reg.repair("scored", 10)
            def second(body):
                return body
        with self.assertRaisesRegex(ValueError, "already registered"):
            reg.repair("variant", 10)(first)
        with self.assertRaisesRegex(ValueError, "unknown repair stage"):
            reg.repair("nope", 10)
        self.assertEqual(reg.stage("scored"), (first,))

    def test_new_module_is_picked_up(self):
        with tempfile.TemporaryDirectory() as tmp:
            pkg = Path(tmp) / "fake_repairs"
            pkg.mkdir()
            (pkg / "__init__.py").write_text(textwrap.dedent("""\
                import sweep_repairs
                REGISTRY = sweep_repairs.Registry()
                repair = REGISTRY.repair
                REGISTRY.load(__path__, __name__)
            """))
            (pkg / "_helper.py").write_text("raise RuntimeError('helpers are not auto-imported')\n")
            (pkg / "late.py").write_text("from . import repair\n\n\n@repair('scored', 20)\n"
                                         "def late(body):\n    return body\n")
            (pkg / "early.py").write_text("from . import repair\n\n\n@repair('scored', 10)\n"
                                          "def early(body):\n    return body\n")
            sys.path.insert(0, tmp)
            try:
                import fake_repairs
                self.assertEqual([f.__name__ for f in fake_repairs.REGISTRY.stage("scored")], ["early", "late"])
            finally:
                sys.path.remove(tmp)
                for mod in [m for m in sys.modules if m == "fake_repairs" or m.startswith("fake_repairs.")]:
                    del sys.modules[mod]


if __name__ == "__main__":
    unittest.main()
