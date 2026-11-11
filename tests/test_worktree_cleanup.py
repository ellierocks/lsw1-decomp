import contextlib
import io
import json
import os
import shutil
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


def run_main(cwd, *args):
    output, previous = io.StringIO(), os.getcwd()
    try:
        os.chdir(cwd)
        with contextlib.redirect_stdout(output):
            main(list(args))
    finally:
        os.chdir(previous)
    return output.getvalue()


def snapshot(root, skip):
    """Every file and symlink under root with its size and mtime, except skip."""
    return {(str(path), path.lstat().st_size, path.lstat().st_mtime_ns)
            for path in root.rglob("*") if path != skip and not path.is_dir()}


class AuditTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        root = self.root = Path(self.directory.name)
        origin, repo = root / "origin.git", root / "repo"
        git(root, "init", "-q", "--bare", "-b", "main", str(origin))
        git(root, "clone", "-q", str(origin), str(repo))
        (repo / ".gitignore").write_text("work/\n")  # as in the real repo
        git(repo, "add", ".gitignore")
        git(repo, "commit", "-q", "-m", "base")
        git(repo, "push", "-q", "origin", "main")
        for name in ("clean", "dirty", "wip", "squash", "gone", "lost"):
            git(repo, "worktree", "add", "-q", "-b", name, str(root / name))
        (root / "dirty" / "notes.txt").write_text("unsaved\n")
        git(root / "wip", "commit", "-q", "--allow-empty", "-m", "unpushed")
        for name in ("a", "b"):  # two commits, landed on main as one squash commit
            (root / "squash" / name).write_text(name)
            git(root / "squash", "add", name)
            git(root / "squash", "commit", "-q", "-m", f"add {name}")
        for name in ("a", "b"):
            (repo / name).write_text(name)
        git(repo, "add", "a", "b")
        git(repo, "commit", "-q", "-m", "squash merge")
        git(repo, "push", "-q", "origin", "main")
        (root / "gone" / "g").write_text("g")
        git(root / "gone", "add", "g")
        git(root / "gone", "commit", "-q", "-m", "pushed then deleted")
        git(root / "gone", "push", "-q", "-u", "origin", "gone")
        git(repo, "push", "-q", "origin", "--delete", "gone")
        git(repo, "fetch", "-q", "--prune", "origin")
        shutil.rmtree(root / "lost")
        parked = root / "clean" / "work" / "fn_live"
        parked.mkdir(parents=True)
        (parked / "status.md").write_text("# Status: PARKED\nBest 5/6")
        (root / "clean" / "work" / "fn_todo").mkdir()
        (root / "clean" / "work" / "fn_todo" / "status.md").write_text("# Status: TODO\n")

    def tearDown(self):
        self.directory.cleanup()

    def rows(self, text):
        return {cells[0]: cells for cells in
                ([cell.strip() for cell in line.strip("|").split("|")]
                 for line in text.splitlines() if line.startswith("| ") and "worktree" not in line)}

    def test_audit_rows_and_attention(self):
        text = run_main(self.root / "repo", "--audit")
        rows = self.rows(text)
        # worktree, branch, ahead/behind, dirty, unpushed, idle, parked, note
        self.assertEqual([rows["clean"][i] for i in (1, 2, 3, 4, 6)], ["clean", "0/1", "0", "no upstream", "1"])
        self.assertEqual([rows["dirty"][i] for i in (2, 3)], ["0/1", "1"])
        self.assertEqual([rows["wip"][i] for i in (2, 3, 4)], ["1/1", "0", "no upstream"])
        self.assertEqual(rows["squash"][2], "0/1")  # squash-merged: not outstanding
        self.assertEqual([rows["gone"][i] for i in (2, 4)], ["1/1", "gone"])
        self.assertEqual([rows["lost"][i] for i in (3, 5, 6)], ["-", "-", "-"])
        self.assertIn("missing", rows["lost"][7])
        self.assertEqual(rows["repo"][7], "main, current")
        attention = text.split("Needs attention:")[1]
        self.assertIn("dirty: dirty [dirty] 1 paths", attention)
        self.assertIn("unpushed: wip [wip] 1 unmerged commit(s), no upstream", attention)
        self.assertIn("upstream gone: gone 1 unmerged commit(s)", attention)
        self.assertIn("missing: lost", attention)
        self.assertNotIn("squash", attention)
        self.assertNotIn("clean", attention)

    def test_audit_writes_only_the_index(self):
        index = self.root / "repo" / "work" / "archive" / "worktrees" / "INDEX.md"
        refs = subprocess.run(["git", "for-each-ref"], cwd=self.root / "repo",
                              capture_output=True, text=True).stdout
        before = snapshot(self.root, index)
        run_main(self.root / "repo", "--audit")
        self.assertEqual(snapshot(self.root, index), before)
        self.assertEqual(subprocess.run(["git", "for-each-ref"], cwd=self.root / "repo",
                                        capture_output=True, text=True).stdout, refs)
        self.assertTrue(index.exists())

    def test_index_covers_live_worktrees(self):
        text = run_main(self.root / "wip", "--index-only")
        self.assertIn("1 parked rows", text)
        index = (self.root / "repo" / "work" / "archive" / "worktrees" / "INDEX.md").read_text()
        status = self.root / "clean" / "work" / "fn_live" / "status.md"
        self.assertIn(f"| fn_live | live:clean | ? |  | Status: PARKED Best 5/6 | {status} |", index)
        self.assertNotIn("fn_todo", index)


if __name__ == "__main__":
    unittest.main()
