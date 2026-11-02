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


class SprintOwnershipTests(unittest.TestCase):
    def test_branch_names_match_unit_leaf_or_path(self):
        self.assertTrue(nt.owned_by_sprint("auto/nu3d/nuanim_2", {"nuanim-2"}))
        self.assertTrue(nt.owned_by_sprint("auto/nu3d/nuanim_2", {"auto-nu3d-nuanim-2"}))
        self.assertTrue(nt.owned_by_sprint("auto/game/turret", {"turret"}))
        self.assertFalse(nt.owned_by_sprint("auto/game/turret", {"nuanim-2"}))
        self.assertFalse(nt.owned_by_sprint("auto/game/turret", set()))

    def test_larger_wave_beats_tiny_finishable_unit(self):
        def row(unit, left, size=0x20):
            return {"unit": unit, "unit_left": left, "size": size, "claimed": False, "source": f"src/{unit}.c",
                    "name": unit}
        rows = [row("big", 30)] * 9 + [row("small", 4)]
        args = type("A", (), {"limit": 5, "json": True, "max_size": 0x200})()
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            nt.print_by_unit(rows, args)
        listed = json.loads(buf.getvalue())
        self.assertEqual([u["unit"] for u in listed], ["big", "small"])
        self.assertTrue(listed[1]["finishable"])

    def test_backlog_beyond_first_wave_does_not_inflate_rank(self):
        def row(unit, size):
            return {"unit": unit, "unit_left": 100, "size": size,
                    "claimed": False, "source": None, "name": unit}
        rows = [row("backlog", 4)] * 100 + [row("useful", 64)] * 3
        args = type("A", (), {"limit": 5, "json": True})()
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            nt.print_by_unit(rows, args)
        listed = json.loads(buf.getvalue())
        self.assertEqual([u["unit"] for u in listed], ["useful", "backlog"])
        self.assertEqual(listed[1]["wave_bytes"], 40)

    def test_owned_units_are_skipped(self):
        rows = [{"unit": "a", "unit_left": 3, "size": 4, "claimed": False, "source": None, "name": "x"}]
        args = type("A", (), {"limit": 5, "json": True, "max_size": 0x200})()
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            nt.print_by_unit(rows, args, owned={"a"})
        self.assertEqual(json.loads(buf.getvalue()), [])
