"""`score_fn.py --apply` says why it wrote nothing (gap 24)."""
import contextlib
import io
import json
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools import score_fn


class ApplyTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.source = Path(tmp.name) / "u.c"
        self.source.write_text("void f(void) {\n}\n")
        self.body = Path(tmp.name) / "body.c"
        self.body.write_text("void f(void) {\n    g();\n}\n")

    def run_score(self, verdict, *extra, compile_error=None):
        def verify(args):
            if "--json" in args:
                print(json.dumps({"verdict": verdict, "differences": [], "exact_insns": 3, "target_insns": 4,
                                  "mine_insns": 4}))
            return 0 if verdict == "MATCH" else 1
        out, err = io.StringIO(), io.StringIO()
        with mock.patch.object(score_fn, "missing_toolchain", return_value=[]), \
             mock.patch.object(score_fn, "compile_source", return_value=compile_error), \
             mock.patch.object(score_fn.vf, "main", side_effect=verify), \
             mock.patch.object(score_fn, "score", return_value=7), \
             contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            score_fn.main(["f", str(self.source), "--body", str(self.body), "--apply", "--no-queue", *extra])
        return out.getvalue() + err.getvalue()

    def test_mismatch_names_verdict_and_score(self):
        text = self.run_score("MISMATCH")
        self.assertIn(f"not applied to {self.source}: verdict MISMATCH, score=7, exact=3/4", text)
        self.assertNotIn("g();", self.source.read_text())

    def test_compile_error_says_not_applied(self):
        text = self.run_score("MATCH", compile_error="u.c:2: parse error")
        self.assertIn("not applied", text)
        self.assertIn("does not compile", text)
        self.assertNotIn("g();", self.source.read_text())

    def test_match_applies_with_and_without_json(self):
        self.assertIn("applied to", self.run_score("MATCH"))
        self.assertIn("g();", self.source.read_text())
        self.source.write_text("void f(void) {\n}\n")
        text = self.run_score("MATCH", "--json")
        self.assertIn("g();", self.source.read_text())
        self.assertEqual(json.loads(text[:text.rindex("}") + 1])["verdict"], "MATCH")


if __name__ == "__main__":
    unittest.main()
