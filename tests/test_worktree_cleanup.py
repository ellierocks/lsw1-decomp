import contextlib
import io
import json
import os
import subprocess
import tempfile
import unittest
from pathlib import Path

from tools.worktree_cleanup import build_index, is_parked_status, main, permuter_best


def git(cwd, *args):
    subprocess.run(["git", "-c", "user.name=t", "-c", "user.email=t@t", "-c", "commit.gpgsign=false",
                    "-c", "init.defaultBranch=main", "-c", "core.hooksPath=/dev/null", *args],
                   cwd=cwd, check=True, capture_output=True)


class IndexTests(unittest.TestCase):
    def test_todo_packs_are_not_parked(self):
        self.assertFalse(is_parked_status("# Status: TODO\n\nfill in"))
        self.assertFalse(is_parked_status(""))
        self.assertTrue(is_parked_status("# Status: PARKED\nBest 82/83"))
        self.assertTrue(is_parked_status("parked 72/80 (mask address order)"))

    def test_index_lists_parked_rows_with_main_state_and_permuter(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            archive = root / "work" / "archive" / "worktrees"
            for name, text in [("fn_a", "# Status: PARKED\nBest 9/10"),
                               ("fn_b", "parked 3/4 | pipes"), ("fn_c", "# Status: TODO\n")]:
                (archive / "wt1" / name).mkdir(parents=True)
                (archive / "wt1" / name / "status.md").write_text(text)
            permute = archive / "wt1" / "permute" / "fn_a"
            (permute / "output-40-1").mkdir(parents=True)
            (permute / "output-120-1").mkdir()
            (permute / "overnight.log").write_text("base score = 300\n")
            report = root / "report.json"
            report.write_text(json.dumps({"units": [{"functions": [
                {"name": "fn_a", "fuzzy_match_percent": 97.5},
                {"name": "fn_b", "fuzzy_match_percent": 100.0}]}]}))

            self.assertEqual(permuter_best(permute), "300→40")
            self.assertEqual(build_index(archive, report), (2, 1))
            index = (archive / "INDEX.md").read_text()
            self.assertIn("| fn_a | wt1 | 97.5% | 300→40 |", index)
            self.assertIn("| fn_b | wt1 | matched |", index)
            self.assertIn("parked 3/4 / pipes", index)
            self.assertNotIn("fn_c", index)
            self.assertLess(index.index("fn_a"), index.index("fn_b"))


class CleanupTests(unittest.TestCase):
    def test_dry_run_keeps_fresh_and_unmerged_worktrees(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            origin, repo = root / "origin.git", root / "repo"
            git(root, "init", "-q", "--bare", "-b", "main", str(origin))
            git(root, "clone", "-q", str(origin), str(repo))
            git(repo, "commit", "-q", "--allow-empty", "-m", "base")
            git(repo, "push", "-q", "origin", "main")
            git(repo, "worktree", "add", "-q", "-b", "fresh", str(root / "fresh"))
            git(repo, "worktree", "add", "-q", "-b", "wip", str(root / "wip"))
            git(root / "wip", "commit", "-q", "--allow-empty", "-m", "unmerged")

            output = io.StringIO()
            cwd = os.getcwd()
            try:
                os.chdir(repo)
                with contextlib.redirect_stdout(output):
                    main([])
            finally:
                os.chdir(cwd)
            text = output.getvalue()
            self.assertRegex(text, r"keep .*fresh\]: active")
            self.assertRegex(text, r"keep .*wip\]: not merged")
            self.assertIn("dry run", text)
            self.assertTrue((root / "fresh").exists())


if __name__ == "__main__":
    unittest.main()
