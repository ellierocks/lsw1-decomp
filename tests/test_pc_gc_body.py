"""JP PC x86 body, audit and Ghidra-name helpers on synthetic inputs (no exe or DOL)."""
import struct
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))

import binary_mining_pipeline  # noqa: E402
import ghidra_decompile  # noqa: E402
import pc_gc_anchors  # noqa: E402
import pc_gc_body  # noqa: E402
import pc_gc_features  # noqa: E402
from pc_gc_body import Body, GcFunction, Insn, Tables  # noqa: E402
from pc_gc_features import Features  # noqa: E402


class FakeImage(pc_gc_body.Image):
    """An Image over a flat buffer: {name: (va, bytes)} sections, no PE header."""

    def __init__(self, sections: dict, imports: dict | None = None):
        self.data = b""
        self.sections = {}
        for name, (va, blob) in sections.items():
            self.sections[name] = (va, len(blob), len(self.data), len(blob))
            self.data += blob
        self.imports = imports or {}


def objdump(text: str) -> list[Insn]:
    return pc_gc_body.parse_objdump(text)


class SwitchTest(unittest.TestCase):
    def test_bounded_jump_table_with_bias(self):
        table = 0x401100
        image = FakeImage({".text": (0x401000, struct.pack("<3I", 0x401020, 0x401030, 0x401020)
                                     .rjust(0x100 + 12, b"\xcc"))})
        insns = objdump("""
  401000:	sub    eax,0x5
  401003:	cmp    eax,0x2
  401006:	ja     0x401040
  401008:	jmp    DWORD PTR [eax*4+0x401100]
""")
        [s] = pc_gc_body.find_switches(insns, image, 0x401000, 0x401200)
        self.assertEqual((s.table, s.cases, s.bound, s.first_case), (table, 3, 3, 5))
        self.assertEqual(s.targets, [0x401020, 0x401030, 0x401020])

    def test_table_stops_at_first_entry_outside_function(self):
        image = FakeImage({".text": (0x401000, (struct.pack("<I", 0x401010) + struct.pack("<I", 0x999999))
                                     .rjust(0x108, b"\xcc"))})
        insns = objdump("  401008:\tjmp    DWORD PTR [ecx*4+0x401100]\n")
        [s] = pc_gc_body.find_switches(insns, image, 0x401000, 0x401200)
        self.assertEqual(s.cases, 1)
        self.assertIsNone(s.bound)


class ImageTest(unittest.TestCase):
    def test_string_at_ascii_and_shift_jis(self):
        image = FakeImage({".rdata": (0x600000, b"hello\0ab\0" + "テスト".encode("cp932") + b"\0"),
                           ".text": (0x401000, b"code\0")})
        self.assertEqual(image.string_at(0x600000), "hello")
        self.assertIsNone(image.string_at(0x600006))      # too short
        self.assertEqual(image.string_at(0x600009), "テスト")
        self.assertIsNone(image.string_at(0x401000))      # not a data section


class FeatureTest(unittest.TestCase):
    def test_nice_float(self):
        self.assertEqual(pc_gc_features.nice_float(0x3F800000), 1.0)
        self.assertEqual(pc_gc_features.nice_float(0xC2C80000), -100.0)
        self.assertIsNone(pc_gc_features.nice_float(0x3F800001))   # not a short decimal
        self.assertIsNone(pc_gc_features.nice_float(0x00001234))   # an integer, not a float

    def test_distinctive(self):
        self.assertFalse(pc_gc_features.distinctive(8))
        self.assertFalse(pc_gc_features.distinctive(0x80401234))   # GC address
        self.assertFalse(pc_gc_features.distinctive(0x00612345))   # PC image address
        self.assertTrue(pc_gc_features.distinctive(0x1234))
        self.assertTrue(pc_gc_features.distinctive(-500))

    LISTING = """
  401000:	mov    eax,0x1234
  401005:	cmp    DWORD PTR [ecx+0x9999],0x2710
  40100c:	sub    esp,0x5000
  401012:	push   0x612345
  401017:	mov    ecx,0xfffffe0c
  40101c:	jmp    0x401100
  401021:	jmp    0x401030
  401100:	mov    eax,0x7
  401105:	jmp    0x401000
"""

    def test_pc_immediates_skip_stack_memory_and_addresses(self):
        imms = pc_gc_features.pc_immediates(self.LISTING, [0x401000, 0x401100])
        self.assertEqual(imms, {0x401000: {0x1234, 0x2710, -500}})

    def test_pc_tail_calls(self):
        tails = pc_gc_features.pc_tail_calls(self.LISTING, [0x401000, 0x401100])
        self.assertEqual(tails, {0x401000: {0x401100}, 0x401100: {0x401000}})


def fake_pairer(**kw) -> pc_gc_anchors.Pairer:
    p = pc_gc_anchors.Pairer({}, {}, kw.get("gc_calls", {}), kw.get("pc_calls", {0x100: [], 0x200: []}),
                             {"f": 0x80001000}, {"f": 0x40}, kw.get("gc_feat", Features()),
                             kw.get("pc_feat", Features()))
    p.gc_imms, p.pc_imms, p.pc_tails = kw.get("gc_imms", {}), kw.get("pc_imms", {}), kw.get("pc_tails", {})
    return p


class AuditTest(unittest.TestCase):
    def test_shared_constants_support(self):
        p = fake_pairer(gc_imms={"f": {0x1234, 0x2710}}, pc_imms={0x100: {0x1234, 0x2710}})
        self.assertTrue(pc_gc_anchors.audit(p, "f", 0x100, {}).startswith("supported: 2 constants"))

    def test_disjoint_constants_are_weak(self):
        p = fake_pairer(gc_imms={"f": {0x1234, 0x2710}}, pc_imms={0x100: {0x4321, 0x3000}})
        self.assertTrue(pc_gc_anchors.audit(p, "f", 0x100, {}).startswith("weak"))

    def test_float_immediate_counts_as_float(self):
        gc_feat = Features()
        gc_feat.floats["f"] = [123.5]
        p = fake_pairer(gc_feat=gc_feat, pc_imms={0x100: {0x42F70000}})   # 123.5f
        self.assertIn("1 floats", pc_gc_anchors.audit(p, "f", 0x100, {}))

    def test_callees_through_tail_calls_agree(self):
        pairs = {"g1": (0x300, "strings"), "g2": (0x400, "strings")}
        p = fake_pairer(gc_calls={"f": ["g1", "g2"]}, pc_calls={0x100: [0x300], 0x300: [], 0x400: []},
                        pc_tails={0x100: {0x400}})
        self.assertEqual(pc_gc_anchors.audit(p, "f", 0x100, pairs),
                         "supported: 0 constants, 0 floats, callees 2 agree 0 differ")

    def test_contradicting_callees_are_weak(self):
        pairs = {"g1": (0x300, "strings")}
        p = fake_pairer(gc_calls={"f": ["g1"]}, pc_calls={0x100: [0x500], 0x300: [], 0x500: []})
        self.assertTrue(pc_gc_anchors.audit(p, "f", 0x100, pairs).startswith("weak"))


class VetoTest(unittest.TestCase):
    def test_vetoed_function_is_never_accepted(self):
        from collections import Counter
        p = fake_pairer()
        votes = {"f": Counter({0x100: 1})}
        p.vetoed = frozenset({"f"})
        self.assertEqual(p.accept(votes, {}, check=False), 0)
        p.vetoed = frozenset()
        pairs: dict = {}
        self.assertEqual(p.accept(votes, pairs, check=False), 1)
        self.assertEqual(pairs["f"][0], 0x100)


class TablesTest(unittest.TestCase):
    def test_load_tables_reads_audit_column_and_confirmed_globals(self):
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp)
            (out / "gc_pc_anchors.tsv").write_text(
                "gc_addr\tgc_function\tpc_function\tevidence\taudit\n"
                "0x80001000\tf\t0x00401000\tstrings: 2 shared\tsupported: 1 constants\n"
                "0x80002000\tg\t0x00402000\tshape, callee of f\tweak: 0 constants\n"
                "0x80003000\th\t0x00403000\tstrings split: two candidates\t\n")
            (out / "pc_functions.tsv").write_text("pc_addr\tsize\n0x00401000\t0x40\t3\tf\tstrings\n")
            (out / "gc_pc_globals.tsv").write_text("0x80400000\tlbl\t0x00700000\tinferred\n")
            doc = out / "globals_doc.tsv"
            doc.write_text("0x80400004\tlbl2\t0x00700004\tconfirmed\n")
            t = pc_gc_body.load_tables(out, doc)
        self.assertEqual(t.gc_to_pc[0x80001000][:2], (0x401000, "strings"))
        self.assertEqual(t.gc_to_pc[0x80002000][1], "shape-callee")
        self.assertNotIn(0x80003000, t.gc_to_pc)
        self.assertTrue(t.audit[0x80002000].startswith("weak"))
        self.assertEqual((t.extents[0x401000], t.callers[0x401000]), (0x40, 3))
        self.assertEqual(t.globals, {0x700000: (0x80400000, "inferred"), 0x700004: (0x80400004, "confirmed")})

    def test_pc_names_marks_inferred(self):
        t = Tables(gc_to_pc={0x80001000: (0x401000, "strings", ""), 0x80002000: (0x402000, "shape", "")},
                   globals={0x700000: (0x80400000, "inferred"), 0x700004: (0x80400004, "confirmed")})
        text = ghidra_decompile.pc_names(t, {0x80001000: "Foo", 0x80400004: "gBar"})
        self.assertIn("0x00401000\tFoo\tfunction", text)
        self.assertIn("0x00402000\tfn_80002000_jpinferred\tfunction", text)
        self.assertIn("0x00700000\tlbl_80400000_jpinferred\tlabel", text)
        self.assertIn("0x00700004\tgBar\tlabel", text)


class RenderTest(unittest.TestCase):
    def test_calls_align_with_gc_including_tail_call(self):
        tables = Tables(pc_to_gc={0x402000: (0x80002000, "strings"), 0x403000: (0x80003000, "shape")},
                        extents={0x402000: 0x10, 0x403000: 0x10, 0x404000: 0x10},
                        audit={0x80003000: "weak: 0 constants"})
        body = Body(0x401000, 0x401020, objdump("""
  401000:	call   0x402000
  401005:	call   0x404000
  40100a:	jne    0x401000
  40100c:	jmp    0x403000
"""), [])
        gc = GcFunction("Caller", 0x80001000, 0x40, calls=["Callee", "fn_80005000", "Other"])
        image = FakeImage({".text": (0x401000, b"\xcc" * 0x20)})
        text = pc_gc_body.render(gc, 0x401000, "callee", "callee of X", body, image, tables,
                                 {0x80002000: "Callee", 0x80003000: "Other"})
        self.assertIn("INFERRED PAIR", text)
        self.assertIn("x86 3 | GC 3", text)
        self.assertIn("pair lead", text)               # sub_404000 sits where GC calls fn_80005000
        self.assertIn("2 of 3 aligned call sites agree", text)
        self.assertIn("tail call, x86 0x403000 inferred:shape WEAK", text)
        self.assertIn("Conditional branches: x86 1 (1 backward)", text)


class WriteTaskBodyTest(unittest.TestCase):
    def test_failures_degrade_to_status(self):
        with tempfile.TemporaryDirectory() as tmp:
            with mock.patch.object(pc_gc_body, "body_for", side_effect=RuntimeError("boom")):
                path, status = pc_gc_body.write_task_body("f", Path(tmp))
            self.assertIsNone(path)
            self.assertIn("RuntimeError: boom", status)
            with mock.patch.object(pc_gc_body, "body_for", return_value=(None, "no x86 body: no JP PC pair")):
                self.assertEqual(pc_gc_body.write_task_body("f", Path(tmp))[0], None)
            with mock.patch.object(pc_gc_body, "body_for", return_value=("; listing\n", "ok")):
                path, _ = pc_gc_body.write_task_body("f", Path(tmp) / "f")
            self.assertEqual(path.read_text(), "; listing\n")


class FunctionsTsvTest(unittest.TestCase):
    def test_rows_carry_pair_and_method(self):
        p = fake_pairer(pc_calls={0x100: [0x200], 0x200: []})
        text = pc_gc_anchors.functions_tsv(p, {"f": (0x200, "strings: 2 shared")})
        self.assertIn("0x00000100\t0x100\t", text)
        self.assertTrue(text.rstrip().endswith("\tf\tstrings"))


class PipelineTest(unittest.TestCase):
    def test_primary_fallback_finds_inputs_in_primary_checkout(self):
        with tempfile.TemporaryDirectory() as a, tempfile.TemporaryDirectory() as b:
            (Path(b) / "input.bin").write_text("x")
            stage = binary_mining_pipeline.Stage("s", [], [], [Path("input.bin")])
            with mock.patch.object(binary_mining_pipeline, "ROOT", Path(a)), \
                    mock.patch.object(binary_mining_pipeline, "primary_root", return_value=Path(b)):
                self.assertEqual(binary_mining_pipeline.missing_inputs(stage), [Path("input.bin")])
                stage = binary_mining_pipeline.Stage("s", [], [], [Path("input.bin")], primary_fallback=True)
                self.assertEqual(binary_mining_pipeline.missing_inputs(stage), [])


if __name__ == "__main__":
    unittest.main()
