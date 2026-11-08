import os
import shutil
import subprocess
import tempfile
import time
import unittest
from pathlib import Path

from tools import ninja_mark_tools

NINJA = """rule tool
  command = echo fetched > $out.fetched
build build/binutils: tool tools/download_tool.py
"""


class NinjaMarkToolsTests(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp())
        self.addCleanup(shutil.rmtree, self.tmp)
        self.primary = self.tmp / 'primary'
        self.repo = self.tmp / 'repo'
        shared = self.tmp / 'shared'
        (shared / 'binutils').mkdir(parents=True)
        for root in (self.primary, self.repo):
            (root / 'tools').mkdir(parents=True)
            (root / 'build').mkdir()
            (root / 'build.ninja').write_text(NINJA)
        (self.primary / 'tools/download_tool.py').write_text('')
        (self.primary / 'build/binutils').symlink_to(shared / 'binutils')

    def primary_log(self):
        if not shutil.which('ninja'):
            self.skipTest('needs ninja')
        subprocess.run(['ninja', 'build/binutils'], cwd=self.primary, check=True,
                       capture_output=True)
        return (self.primary / '.ninja_log').read_text()

    def dirty(self):
        out = subprocess.run(['ninja', '-n', 'build/binutils'], cwd=self.repo,
                             check=True, capture_output=True, text=True)
        return 'no work to do' not in out.stdout

    def test_fresh_worktree_is_current_after_mark(self):
        self.primary_log()
        time.sleep(0.02)
        # The worktree's checkout is newer than anything the primary recorded.
        (self.repo / 'tools/download_tool.py').write_text('')
        (self.repo / 'build/binutils').symlink_to(self.tmp / 'shared' / 'binutils')
        # Copying the primary's log lines (the old behaviour) still leaves the tool dirty.
        (self.repo / '.ninja_log').write_text((self.primary / '.ninja_log').read_text())
        self.assertTrue(self.dirty())
        marked = ninja_mark_tools.mark(self.repo, self.primary)
        self.assertEqual(marked, ['build/binutils'])
        self.assertFalse(self.dirty())
        # Idempotent: nothing more to mark.
        self.assertEqual(ninja_mark_tools.mark(self.repo, self.primary), [])

    def test_changed_command_hash_is_not_masked(self):
        self.primary_log()
        (self.repo / 'tools/download_tool.py').write_text('')
        (self.repo / 'build/binutils').symlink_to(self.tmp / 'shared' / 'binutils')
        (self.repo / 'build.ninja').write_text(NINJA.replace('fetched', 'pin2'))
        ninja_mark_tools.mark(self.repo, self.primary)
        self.assertTrue(self.dirty())


if __name__ == '__main__':
    unittest.main()
