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
    def test_compile_script_wraps_score_fn_compile_only(self):
        text = ps.compile_script("fn_8000A64C")
        self.assertTrue(text.startswith("#!/usr/bin/env bash\n"))
        self.assertIn('score_fn.py fn_8000A64C "$1" --compile-only -o "$3"', text)

    def test_settings_parse_with_shim_and_point_at_dumper(self):
        settings = toml.loads(ps.settings_toml("NuFdiv", Path("/w/permute/NuFdiv")))
        self.assertEqual(settings["func_name"], "NuFdiv")
        self.assertEqual(settings["compiler_type"], "gcc")
        self.assertTrue(settings["objdump_command"].endswith("permute_objdump.py --dir /w/permute/NuFdiv"))

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
