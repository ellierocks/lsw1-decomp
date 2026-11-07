"""Exercise the permuter work-dir generator and the verifier-backed objdump scorer."""
import hashlib
import json
import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest import mock

from tools import permute_objdump as po
from tools import permute_setup as ps

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "tools" / "permuter_shim"))
import toml  # noqa: E402


def ref(desc):
    return SimpleNamespace(describe=lambda verbose: desc)


class SetupTests(unittest.TestCase):
    def test_compile_script_runs_cc1_as_and_fixup_directly(self):
        text = ps.compile_script("fn_8000A64C")
        self.assertTrue(text.startswith("#!/usr/bin/env bash\n"))
        self.assertIn('cc1.exe -quiet -O2 "$1" -o "$tmp/c.s"', text)
        self.assertIn('powerpc-eabi-as -mgekko -o "$3" "$tmp/c.s"', text)
        self.assertIn('dtk_patched elf fixup "$3" "$3"', text)

    def test_fallback_script_wraps_score_fn_compile_only(self):
        text = ps.score_compile_script("fn_8000A64C")
        self.assertIn('score_fn.py fn_8000A64C "$1" --compile-only -o "$3"', text)

    def test_focus_markers_become_perm_macros_or_vanish(self):
        src = ("void f(int *p) {\n/* permute: begin */\np[0] = 1;\n/* permute: lineswap */\n"
               "p[1] = 2;\np[2] = 3;\n/* permute: end */\n/* permute: end */\n}\n")
        marked, n = ps.focus(src, keep=True)
        self.assertEqual(n, 2)
        self.assertIn("PERM_RANDOMIZE(\np[0] = 1;\nPERM_LINESWAP(\np[1] = 2;", marked)
        self.assertEqual(marked.count(")\n"), 2)
        plain, _ = ps.focus(src, keep=False)
        self.assertNotIn("PERM_", plain)
        self.assertNotIn("permute:", plain)

    def test_unbalanced_focus_markers_are_rejected(self):
        for bad in ("/* permute: begin */ x;", "x; /* permute: end */"):
            with self.assertRaises(ps.vf.ToolError):
                ps.focus(bad, keep=True)

    def test_m2c_names_only_counts_the_target_function(self):
        text = "int g(void) { int temp_r3 = 0; return temp_r3; }\n" \
               "int f(int var_r31) { int temp_r4 = var_r31; f32 var_f1; return temp_r4; }\n"
        self.assertEqual(ps.m2c_names(text, "f"), ["temp_r4", "var_f1", "var_r31"])
        self.assertEqual(ps.m2c_names(text, "missing"), [])

    def test_warnings_flag_structural_diffs_m2c_names_and_no_focus(self):
        rep = {"verdict": "MISMATCH", "target_insns": 100, "mine_insns": 98, "exact_insns": 95}
        meta = {"m2c_names": ["temp_r3"], "focus_regions": 0, "fast_compile": True}
        got = " | ".join(ps.warnings(rep, meta))
        self.assertIn("structural diff", got)
        self.assertIn("m2c names", got)
        self.assertIn("no /* permute", got)
        near = {"verdict": "MISMATCH", "target_insns": 100, "mine_insns": 100, "exact_insns": 98}
        self.assertEqual(ps.warnings(near, {"m2c_names": [], "focus_regions": 1, "fast_compile": True}), [])

    def test_settings_parse_with_shim_and_point_at_dumper(self):
        settings = toml.loads(ps.settings_toml("NuFdiv", Path("/w/permute/NuFdiv")))
        self.assertEqual(settings["func_name"], "NuFdiv")
        self.assertEqual(settings["compiler_type"], "gcc")
        self.assertTrue(settings["objdump_command"].endswith("permute_objdump.py --dir /w/permute/NuFdiv"))

    def test_body_is_spliced_into_unit_source_with_focus_markers(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            unit = tmp / "u.c"
            unit.write_text("int g(void);\nint f(int a)\n{\n    return a;\n}\nint h(void) { return 1; }\n")
            body = tmp / "try.c"
            body.write_text("int f(int a)\n{\n    /* permute: begin */\n    return a + 1;\n"
                            "    /* permute: end */\n}\n")
            target = tmp / "t.o"
            target.write_bytes(b"")
            seen = {}

            def fake_preprocess(src, out, include_dir):
                seen[out.name] = src.read_text()
                out.write_text(src.read_text())
                return None

            entry = SimpleNamespace(addr=0x80000000, size=4)
            with mock.patch.object(ps.vf, "SymbolTable", return_value=mock.Mock(function=lambda n: entry)), \
                    mock.patch.object(ps.vf, "lookup_unit", return_value="u"), \
                    mock.patch.object(ps.vf, "find_target_object", return_value=target), \
                    mock.patch.object(ps.vf, "source_for_unit", return_value=unit), \
                    mock.patch.object(ps, "ROOT", tmp), \
                    mock.patch.object(ps, "preprocess", side_effect=fake_preprocess), \
                    mock.patch.object(ps, "fast_compile_agrees", return_value=True):
                meta = ps.setup("f", tmp / "out", None, None, tmp / "ledger", False, body)
            self.assertEqual(meta["focus_regions"], 1)
            self.assertIn("--body try.c", meta["base_source"])
            self.assertIn("PERM_RANDOMIZE", seen["base.c"])
            self.assertIn("return a + 1;", seen["base_plain.c"])
            self.assertIn("int h(void)", seen["base_plain.c"])
            self.assertNotIn("return a;", seen["base_plain.c"])

    def test_ledger_best_takes_lowest_score_with_source_snapshot(self):
        with tempfile.TemporaryDirectory() as tmp:
            ledger = Path(tmp)
            key = hashlib.sha256(json.dumps({"function": "f", "unit": "u", "compiled_symbol": None},
                                            sort_keys=True).encode()).hexdigest()[:20]
            (ledger / key).mkdir()
            recs = [{"score": [1, 3], "sources": {"src/u.c": {"text": "a"}}},
                    {"score": [1, 1], "sources": {"src/u.c": {"text": "b"}}},
                    {"score": [0, 0], "sources": {}},
                    {"score": None, "sources": {"src/u.c": {"text": "c"}}}]
            for i, rec in enumerate(recs):
                (ledger / key / f"{i:04d}.json").write_text(json.dumps(rec))
            with mock.patch.object(ps, "ROOT", ledger):
                text, prov = ps.ledger_best("f", "u", None, "src/u.c", ledger)
            self.assertEqual(text, "b")
            self.assertIn("score [1, 1]", prov)


class ObjdumpTests(unittest.TestCase):
    def test_accepted_requires_verified_mappings(self):
        mine = SimpleNamespace(insns=[SimpleNamespace(ref=ref(".rodata+0x8"))])
        rep = SimpleNamespace(mappings=[(".rodata+0x8 -> lbl_8018F150", "verified bytes")])
        self.assertTrue(po.accepted(rep, (0, 0, "ok", ""), None, mine))
        self.assertTrue(po.accepted(rep, (0, 0, "mapped", ""), None, mine))
        rep.mappings.append((".rodata+0x8 -> lbl_8018F154", "unverified"))
        self.assertFalse(po.accepted(rep, (0, 0, "mapped", ""), None, mine))
        self.assertFalse(po.accepted(rep, (0, 0, "operand", ""), None, mine))
        self.assertFalse(po.accepted(rep, (None, 0, "length", ""), None, mine))

    def test_lines_use_function_relative_offsets(self):
        text = po.lines(["lwz r0, 0x34(r31)", "blr"])
        self.assertEqual(text, "\n00000000 <func>:\n       0:\t00 00 00 00 \tlwz r0, 0x34(r31)\n"
                               "       4:\t00 00 00 00 \tblr\n")

    def test_sentinel_only_when_not_strict_match(self):
        meta = {"name": "f", "compiled_symbol": None}
        with mock.patch.object(po, "candidate_rows", return_value=(["blr"], "MATCH")):
            self.assertNotIn(po.SENTINEL, po.dump(meta, Path("c.o"), False))
        with mock.patch.object(po, "candidate_rows", return_value=(["blr"], "MATCH (unverified references)")):
            self.assertIn("verify_fn MATCH_(unverified_references)", po.dump(meta, Path("c.o"), False))


if __name__ == "__main__":
    unittest.main()
