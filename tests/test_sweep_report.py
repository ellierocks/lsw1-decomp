"""sweep_report classification on synthetic score reports, and --compare on synthetic JSON."""
import json
from pathlib import Path
import tempfile
import unittest
from contextlib import redirect_stdout
from io import StringIO

from tools import sweep_report as sr

# Instruction words, target first as verify_fn writes them ("TARGET vs MINE").
STWU_20, STWU_30 = "9421FFE0", "9421FFD0"          # stwu r1,-0x20(r1) / -0x30(r1)
STMW_29, STMW_28 = "BFA10014", "BF810014"          # stmw r29,0x14(r1) / stmw r28,0x14(r1)
ADDI_R3, ADDI_R5, ADDI_R3_2 = "38640001", "38A40001", "38640002"  # addi r3,r4,1 / r5 / imm 2
LWZ_8, LWZ_C = "80610008", "8061000C"              # lwz r3,0x8(r1) / 0xC(r1)


def op(kind, t, m):
    return {"kind": kind, "detail": f"{t} vs {m}"}


def rep(diffs, target=100, mine=100, mnemonic=100):
    return {"target_insns": target, "mine_insns": mine, "mnemonic_similarity": mnemonic, "differences": diffs}


LENGTH = {"kind": "length", "detail": "instruction only in mine"}


class ShapeTest(unittest.TestCase):
    def test_each_shape(self):
        cases = {
            "reloc-data-only": rep([{"kind": "reloc", "detail": "only mine has a relocation"}]),
            "frame-offsets": rep([op("operand", LWZ_8, LWZ_C), op("operand", ADDI_R3, ADDI_R5)]),
            "registers": rep([op("operand", ADDI_R3, ADDI_R5)]),
            "immediates": rep([op("operand", ADDI_R3, ADDI_R3_2), op("operand", ADDI_R3, ADDI_R5)]),
            "same-length": rep([op("opcode", ADDI_R3, LWZ_8), LENGTH, LENGTH]),
            "length-3pct": rep([LENGTH] * 3, mine=103),
            "length-10pct": rep([LENGTH] * 8, mine=108),
            "mine-longer": rep([LENGTH] * 20, mine=120),
            "mine-shorter": rep([LENGTH] * 20, mine=80),
        }
        for want, report in cases.items():
            self.assertEqual(sr.shape(report), want, want)
        self.assertEqual([k for k, _ in sr.SHAPES], list(cases))

    def test_small_functions_get_two_instructions_of_slack(self):
        self.assertEqual(sr.shape(rep([LENGTH, LENGTH], target=10, mine=12)), "length-3pct")

    def test_operand_kinds(self):
        self.assertEqual(sr.operand_kind(f"{STWU_20} vs {STWU_30}"), "frame")
        self.assertEqual(sr.operand_kind(f"{LWZ_8} vs {LWZ_C}"), "stackoff")
        self.assertEqual(sr.operand_kind(f"{ADDI_R3} vs {ADDI_R3_2}"), "imm")
        self.assertEqual(sr.operand_kind(f"{ADDI_R3} vs {ADDI_R5}"), "reg")
        self.assertEqual(sr.operand_kind("differs from the aligned neighbours"), "other")


class FrameTest(unittest.TestCase):
    def test_split(self):
        self.assertEqual(sr.frame(rep([op("operand", ADDI_R3, ADDI_R5)])), "equal")
        self.assertEqual(sr.frame(rep([op("operand", STWU_20, STWU_30)])), "bigger-same-gprs")
        self.assertEqual(sr.frame(rep([op("operand", STWU_30, STWU_20)])), "smaller-same-gprs")
        self.assertEqual(sr.frame(rep([op("operand", STWU_20, STWU_30), op("operand", STMW_29, STMW_28)])),
                         "more-gprs")
        self.assertEqual(sr.frame(rep([op("operand", STWU_30, STWU_20), op("operand", STMW_28, STMW_29)])),
                         "fewer-gprs")
        # a differing stmw with an equal frame is not a frame difference
        self.assertEqual(sr.frame(rep([op("operand", STMW_29, STMW_28)])), "equal")


class RelocTest(unittest.TestCase):
    def test_symbol_offset(self):
        refs = ("references EMB_SDA21 lbl_804097A0+0x8 (0x804097A8) but target references "
                "EMB_SDA21 lbl_804097A0 (0x804097A0)")
        self.assertTrue(sr.symbol_offset(refs))
        self.assertTrue(sr.symbol_offset("addend: target R_PPC_ADDR16_HA gFoo vs mine R_PPC_ADDR16_HA gFoo+0x10"))
        self.assertFalse(sr.symbol_offset("addend: target R_PPC_ADDR16_HA gFoo+0x4 vs mine R_PPC_ADDR16_HA gFoo+0x10"))
        self.assertFalse(sr.symbol_offset("references EMB_SDA21 a (0x1) but target references EMB_SDA21 b (0x2)"))

    def test_reloc_classes(self):
        report = rep([{"kind": "reloc", "detail": "only target has a relocation"},
                      {"kind": "reloc", "detail": "only mine has a relocation"},
                      {"kind": "reloc", "detail": "addend: target R a vs mine R a-0x4"}])
        self.assertEqual(sr.reloc_classes(report), ["reloc:symbol+offset", "reloc:only-mine", "reloc:only-target"])


class ErrorTest(unittest.TestCase):
    def test_normalise(self):
        self.assertEqual(sr.normalise_error("Z:/tmp/score_fn_x/levels.c:5371: `sp10' undeclared (first use in this function)"),
                         "X undeclared (first use in this function)")
        self.assertEqual(sr.normalise_error("Z:/tmp/a/b.c:12: incompatible type for argument 2 of `fn_80001234'"),
                         "incompatible type for argument N of X")
        self.assertEqual(sr.normalise_error("Z:/tmp/a/b.c:12: invalid operands to binary *"),
                         "invalid operands to binary *")
        self.assertEqual(sr.normalise_error(""), "(no error text)")


class ClassifyTest(unittest.TestCase):
    def test_mismatch_classes(self):
        row = {"name": "f", "verdict": "MISMATCH", "closeness": 0.85, "size": 40}
        draft = ("void f(void *arg0) {\n    s32 sp10;\n    *(s32 *)((s8 *)arg0 + 4) = 0;\n"
                 "    switch (sp10) { default: goto end; }\nend:;\n}\n")
        classes = sr.classify(row, rep([op("operand", STWU_30, STWU_20)], mnemonic=96), draft)
        self.assertEqual(classes, ["verdict:MISMATCH", "band:80-90", "shape:frame-offsets",
                                   "frame:smaller-same-gprs", "mnemonic:>=95", "draft:casted-deref",
                                   "draft:void-param", "draft:goto", "draft:sp-local", "draft:switch"])

    def test_compile_error_and_failed_rescore(self):
        self.assertEqual(sr.classify({"name": "g", "verdict": "COMPILE-ERROR", "error": "a.c:1: parse error before `*'"}),
                         ["verdict:COMPILE-ERROR", "error:parse error before X"])
        self.assertIn("rescore:failed", sr.classify({"name": "h", "verdict": "MISMATCH", "closeness": 0.2}))


def fn(name, size, verdict, closeness, classes):
    return {"name": name, "unit": "u", "size": size, "verdict": verdict, "closeness": closeness,
            "classes": [f"verdict:{verdict}", *classes]}


class CompareTest(unittest.TestCase):
    def setUp(self):
        self.old = {"sweep": "aaa", "rows": 3, "functions": [
            fn("a", 100, "MISMATCH", 0.85, ["band:80-90", "reloc:symbol+offset"]),
            fn("b", 200, "MISMATCH", 0.5, ["band:50-80", "reloc:symbol+offset"]),
            fn("c", 40, "COMPILE-ERROR", 0.0, ["error:X undeclared"]),
        ]}
        self.new = {"sweep": "bbb", "rows": 3, "functions": [
            fn("b", 200, "MISMATCH", 0.95, ["band:>=90"]),
            fn("c", 40, "MISMATCH", 0.6, ["band:50-80"]),
            fn("d", 8, "NOT-SWEPT", 0.0, ["unswept:no-source"]),
        ]}

    def run_compare(self):
        with tempfile.TemporaryDirectory() as tmp:
            old, new = Path(tmp) / "old.json", Path(tmp) / "new.json"
            old.write_text(json.dumps(self.old))
            new.write_text(json.dumps(self.new))
            buf = StringIO()
            with redirect_stdout(buf):
                self.assertEqual(sr.main(["--compare", str(old), "--against", str(new)]), 0)
            return buf.getvalue()

    def row(self, out, cls):
        for line in out.splitlines():
            nums, _, name = line.rpartition("  ")
            if name == cls and line[:1] == " ":
                return [int(x.replace(",", "")) for x in nums.split()]
        return None

    def test_movement(self):
        out = self.run_compare()
        self.assertIn("matched: 0 -> 1 functions (+100 B); 1 left the listing (100 B", out)
        self.assertIn("new in listing: 1", out)
        self.assertIn("verdict changes: 1", out)
        # a left the listing, b's reloc fixed: both leave symbol+offset
        self.assertEqual(self.row(out, "reloc:symbol+offset"), [0, 0, 2, 300, 300, 0])
        self.assertEqual(self.row(out, "band:80-90"), [0, 0, 1, 100, 100, 0])
        self.assertEqual(self.row(out, "band:>=90"), [1, 200, 0, 0, 0, 200])
        # b leaves 50-80 while c enters it
        self.assertEqual(self.row(out, "band:50-80"), [1, 40, 1, 200, 200, 40])
        self.assertEqual(self.row(out, "error:X undeclared"), [0, 0, 1, 40, 40, 0])
        self.assertEqual(self.row(out, "verdict:COMPILE-ERROR"), [0, 0, 1, 40, 40, 0])
        self.assertEqual(self.row(out, "verdict:GONE"), [1, 100, 0, 0, 0, 0])
        self.assertEqual(self.row(out, "unswept:no-source"), [1, 8, 0, 0, 0, 8])


class MarkdownTest(unittest.TestCase):
    def test_deterministic_and_ranked(self):
        data = {"sweep": "abc1234", "rows": 3, "functions": [
            fn("a", 100, "MISMATCH", 0.85, ["band:80-90", "reloc:symbol+offset", "frame:equal"]),
            fn("b", 200, "COMPILE-ERROR", 0.0, ["error:X undeclared"]),
            fn("c", 8, "NOT-SWEPT", 0.0, ["unswept:no-source"]),
        ]}
        text = sr.markdown(data)
        self.assertEqual(text, sr.markdown(json.loads(json.dumps(data))))
        self.assertIn("Generated from sweep of abc1234, 3 rows.", text)
        ranked = text.split("## Ranked by bytes")[1]
        self.assertLess(ranked.index("X undeclared"), ranked.index("reloc:symbol+offset"))
        self.assertNotIn("frame:equal", ranked)


if __name__ == "__main__":
    unittest.main()
