import tempfile
from pathlib import Path
import subprocess
import unittest
from unittest.mock import patch

from tools.export_progress import export
from tools.publish_progress import merge, publish


class PublishProgressTests(unittest.TestCase):
    def snapshot(self, revision='a', time='2026-11-10T00:00:00+00:00'):
        return export({'measures': {}, 'units': []}, revision=revision*40, report_at=time)

    def test_history_and_repeated_build(self):
        first = self.snapshot()
        second = self.snapshot('b', '2026-11-11T00:00:00+00:00')
        result = merge(second, first)
        self.assertEqual(len(result['history']), 2)
        self.assertEqual(len(merge(second, result)['history']), 2)
        with self.assertRaises(ValueError):
            merge(first, second)
        with self.assertRaises(ValueError):
            merge(export({'measures': {}}, report_at='2026-11-10T00:00:00Z'), None)

    def test_real_git_publication_preserves_checkout_and_history(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            remote, checkout = root/'remote.git', root/'checkout'
            subprocess.run(['git', 'init', '--bare', str(remote)], check=True, capture_output=True)
            subprocess.run(['git', 'init', '-b', 'main', str(checkout)], check=True, capture_output=True)
            def git(*args):
                return subprocess.check_output(['git', '-C', str(checkout), *args], text=True).strip()
            git('config', 'user.email', 'test@example.com')
            git('config', 'user.name', 'Test')
            git('remote', 'add', 'origin', str(remote))
            (checkout/'keep').write_text('original')
            git('add', 'keep')
            git('commit', '-m', 'chore: initial commit')
            (checkout/'keep').write_text('other session work')
            before = git('status', '--porcelain')
            # check_output/run share cwd, while the caller stays in its original directory.
            original_output, original_run = subprocess.check_output, subprocess.run
            def output(*args, **kwargs):
                kwargs['cwd'] = checkout
                return original_output(*args, **kwargs)
            def run(*args, **kwargs):
                kwargs['cwd'] = checkout
                return original_run(*args, **kwargs)
            with patch('tools.publish_progress.subprocess.check_output', side_effect=output), patch('tools.publish_progress.subprocess.run', side_effect=run):
                publish(self.snapshot())
                publish(self.snapshot('b', '2026-11-11T00:00:00+00:00'))
            self.assertEqual(git('branch', '--show-current'), 'main')
            self.assertEqual(git('status', '--porcelain'), before)
            git('fetch', 'origin', 'progress-data')
            self.assertEqual(git('ls-tree', '--name-only', 'FETCH_HEAD'), 'progress.json')
            import json
            self.assertEqual(len(json.loads(git('show', 'FETCH_HEAD:progress.json'))['history']), 2)
