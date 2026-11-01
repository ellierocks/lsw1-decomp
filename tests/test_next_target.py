import contextlib
import io
import json
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import next_target as nt
from tools import score_fn


def bl(offset: int) -> bytes:
    return (0x48000001 | (offset & 0x03FFFFFC)).to_bytes(4, "big")


class NextTargetTests(unittest.TestCase):
    def test_direct_callees_decodes_relative_bl_targets(self):
        code = bl(0x10) + bl(0x0C) + bl(-0x20) + (0x48000000).to_bytes(4, "big")
        self.assertEqual(nt.direct_callees(code, 0x80001000), {0x80001010, 0x80000FE8})

    def test_ledger_state_reads_verified_and_attempted_records(self):
        with tempfile.TemporaryDirectory() as tmp:
            ledger = Path(tmp) / "abc"
            ledger.mkdir()
            (ledger / "0001.json").write_text(json.dumps({"function": "Foo", "returncode": 1}))
            (ledger / "0002.json").write_text(json.dumps({"function": "Bar", "returncode": 0}))
            with mock.patch.object(nt, "LEDGERS", Path(tmp)):
                verified, attempts = nt.ledger_state()
        self.assertEqual(verified, {"Bar"})
        self.assertEqual(attempts, {"Foo": 1, "Bar": 1})

    def test_definition_regex_ignores_prototypes(self):
        source = "s32 Proto(s32 a);\ns32 Defined(s32 a)\n{\n    return a;\n}\n"
        self.assertEqual(set(nt.DEFINITION_RE.findall(source)), {"Defined"})

    def test_unit_prefixes_filter_and_empty_means_all(self):
        self.assertTrue(nt.in_units("auto/game/cut", []))
        self.assertTrue(nt.in_units("auto/game/cut", ["auto/ai/", "auto/game/"]))
        self.assertFalse(nt.in_units("auto/gamelib/glutils", ["auto/game/"]))


class ScoreFnPreflightTests(unittest.TestCase):
    def test_missing_toolchain_names_each_absent_prodg_tool(self):
        with tempfile.TemporaryDirectory() as tmp:
            with mock.patch.object(score_fn, "BUILD", Path(tmp)):
                missing = score_fn.missing_toolchain("prodg")
        self.assertIn(Path(tmp) / "tools" / "wibo", missing)
        self.assertIn(Path(tmp) / "binutils" / "powerpc-eabi-as", missing)

    def test_main_reports_bootstrap_instead_of_traceback(self):
        with tempfile.TemporaryDirectory() as tmp:
            source = Path(tmp) / "x.c"
            source.write_text("int x;\n")
            err = io.StringIO()
            with mock.patch.object(score_fn, "BUILD", Path(tmp)), contextlib.redirect_stderr(err):
                code = score_fn.main(["Foo", str(source)])
        self.assertEqual(code, 2)
        self.assertIn("bash tools/bootstrap.sh", err.getvalue())


if __name__ == "__main__":
    unittest.main()
