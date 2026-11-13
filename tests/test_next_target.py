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

    def test_newlib_and_kr_definitions_count_as_defined(self):
        text = ("_DEFUN (strlen, (str),\n\t_CONST char *str)\n{\n}\n"
                "int\nvprintf (fmt, ap)\n     char *fmt;\n     va_list ap;\n{\n}\n"
                "int\nproto (int a);\n")
        found = set(nt.NEWLIB_DEFUN_RE.findall(text)) | set(nt.KR_DEFINITION_RE.findall(text))
        self.assertEqual(found, {"strlen", "vprintf"})

    def test_unit_prefixes_filter_and_empty_means_all(self):
        self.assertTrue(nt.in_units("auto/game/cut", []))
        self.assertTrue(nt.in_units("auto/game/cut", ["auto/ai/", "auto/game/"]))
        self.assertFalse(nt.in_units("auto/gamelib/glutils", ["auto/game/"]))


def _rows(n):
    return [{"opensaga": {"matches": False, "result": "-"}, "name": f"fn_{i:02d}", "address": f"0x{i:08X}",
             "size": 4 * (i + 1), "unit": "auto/nu3d/nuportal_gen", "callees": 0, "callee_names": [],
             "source": None, "source_path": "src/x.c", "claimed": False, "attempts": 0, "defined": False,
             "ledger_verified": False, "unit_left": n, "jp": None} for i in range(n)]


class PlanListingTests(unittest.TestCase):
    """The nuportal item had 22 unmatched functions; the default --limit 15 printed 15 (gap 22)."""

    def run_main(self, argv, scope):
        import plan
        out = io.StringIO()
        with mock.patch.object(plan, "status", return_value={}), \
             mock.patch.object(plan, "plan_targets", return_value=scope), \
             mock.patch.object(nt, "collect", return_value=(_rows(22), 0)), \
             contextlib.redirect_stdout(out), contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(nt.main(argv), 0)
        return out.getvalue()

    def scope(self):
        return {"item": "nuportal", "stage_id": "s2-nu3d", "mine": True,
                "functions": [f"fn_{i:02d}" for i in range(22)], "stage": []}

    def test_plan_item_lists_every_function_by_default(self):
        listed = json.loads(self.run_main(["--json", "--no-remote"], self.scope()))
        self.assertEqual(len(listed), 22)
        text = self.run_main(["--no-remote"], self.scope())
        self.assertEqual(sum(f"fn_{i:02d} " in text for i in range(22)), 22)
        self.assertNotIn("more not shown", text)

    def test_explicit_limit_says_what_it_cut(self):
        text = self.run_main(["--no-remote", "--limit", "5"], self.scope())
        self.assertIn("17 more not shown (--limit 5)", text)

    def test_global_ranking_keeps_default_of_15(self):
        text = self.run_main(["--all", "--no-remote"], None)
        self.assertEqual(sum(f"fn_{i:02d} " in text for i in range(22)), 15)
        self.assertIn("7 more not shown", text)


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

    def test_numbered_sprint_owns_its_unit(self):
        units = frozenset({"auto/game/levels", "auto/nu3d/nuanim", "auto/nu3d/nuanim_2"})
        self.assertTrue(nt.owned_by_sprint("auto/game/levels", {"levels-2"}, units))
        self.assertTrue(nt.owned_by_sprint("auto/nu3d/nuanim_2", {"nuanim-2"}, units))
        self.assertFalse(nt.owned_by_sprint("auto/nu3d/nuanim", {"nuanim-2"}, units))

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
