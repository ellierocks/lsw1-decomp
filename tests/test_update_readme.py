import sys
import unittest
from pathlib import Path
import os
import subprocess
import tempfile
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import update_readme as readme


class ReadmeTests(unittest.TestCase):
    def test_render_uses_counts_and_keeps_naming_separate(self):
        symbols = {'sections': {s: {'named': 1, 'total': 4} for s in ('text', 'rodata', 'bss')}}
        block = readme.render(symbols, {'matched_code': '1920', 'total_code': '1569532',
                                      'matched_functions': 22, 'total_functions': 4147})
        self.assertIn('| Code bytes | 1,920 | 1,569,532 | 0.12% |', block)
        self.assertIn('| `.text` (functions) | 1 | 4 | 25.0% |', block)
        original = 'intro\n' + readme.START + '\nstale\n' + readme.END + '\noutro\n'
        result = readme.update(original, block)
        self.assertEqual(result, 'intro\n' + block + '\noutro\n')
        self.assertEqual(readme.update(result, block), result)

    def test_missing_or_invalid_evidence_fails(self):
        for text in ('no markers', readme.END + readme.START, readme.START * 2 + readme.END):
            with self.assertRaises(ValueError):
                readme.update(text, 'replacement')
        for measures in ({}, {'matched_code': 2, 'total_code': 1}, {'matched_code': 0, 'total_code': 0}):
            with self.assertRaises((KeyError, ValueError)):
                readme.render({}, measures)

class FakeGitHub:
    """GitHub API double for the branch head and workflow dispatch."""

    def __init__(self, origin):
        self.origin, self.calls = origin, []

    def request(self, method, path, data=None):
        self.calls.append((method, path, data))
        if (method, path) == ('GET', '/branches/main'):
            return {'commit': {'sha': git(self.origin, 'rev-parse', 'main')}}
        if method == 'POST' and path == '/actions/workflows/build.yml/dispatches':
            return None
        raise AssertionError(f'unexpected API call {method} {path}')


def git(cwd, *args):
    return subprocess.run(['git', *args], cwd=cwd, check=True, capture_output=True, text=True).stdout.strip()


class PublishFlowTests(unittest.TestCase):
    """Run publish against a real git origin, as the GitHub job does."""

    STALE = 'intro\n' + readme.START + '\nstale\n' + readme.END + '\noutro\n'
    FRESH = 'intro\n' + readme.START + '\nfresh\n' + readme.END + '\noutro\n'

    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name)
        env = patch.dict(os.environ, {
            'GIT_CONFIG_GLOBAL': os.devnull, 'GIT_CONFIG_NOSYSTEM': '1',
            'GIT_AUTHOR_NAME': 't', 'GIT_AUTHOR_EMAIL': 't@t', 'GIT_COMMITTER_NAME': 't', 'GIT_COMMITTER_EMAIL': 't@t'})
        env.start()
        self.addCleanup(env.stop)
        self.origin = self.root / 'origin.git'
        git(self.root, 'init', '-q', '--bare', '-b', 'main', str(self.origin))
        seed = self.root / 'seed'
        git(self.root, 'clone', '-q', str(self.origin), str(seed))
        (seed / 'README.md').write_text(self.STALE)
        git(seed, 'add', 'README.md')
        git(seed, 'commit', '-q', '-m', 'seed')
        git(seed, 'push', '-q', 'origin', 'HEAD:main')
        self.source = git(seed, 'rev-parse', 'HEAD')
        self.branch = readme.BRANCH_PREFIX + self.source
        self.forge = FakeGitHub(self.origin)

    def checkout(self, name, ref):
        work = self.root / name
        git(self.root, 'clone', '-q', '--depth=2', '--branch', ref, 'file://' + str(self.origin), str(work))
        return work

    def run_job(self, work, event, ref, text):
        cwd = os.getcwd()
        os.chdir(work)
        try:
            with patch.object(readme, 'GitHub', return_value=self.forge), patch.dict(os.environ, {
                    'GITHUB_EVENT_NAME': event, 'GITHUB_REF': ref, 'GITHUB_SHA': git(work, 'rev-parse', 'HEAD')}):
                readme.publish(text)
        finally:
            os.chdir(cwd)

    def publish_branch(self):
        self.run_job(self.checkout('main-run', 'main'), 'push', 'refs/heads/main', self.FRESH)
        return self.checkout('dispatch-run', self.branch)

    def test_push_publishes_branch_then_dispatch_fast_forwards_main(self):
        work = self.publish_branch()
        self.assertEqual(self.forge.calls[-1], ('POST', '/actions/workflows/build.yml/dispatches', {'ref': self.branch}))
        self.assertEqual(git(self.origin, 'rev-parse', 'main'), self.source)
        head = git(work, 'rev-parse', 'HEAD')
        self.run_job(work, 'workflow_dispatch', 'refs/heads/' + self.branch, self.FRESH)
        self.assertEqual(git(self.origin, 'rev-parse', 'main'), head)
        self.assertEqual(git(self.origin, 'show', 'main:README.md') + '\n', self.FRESH)
        self.assertEqual(git(self.origin, 'branch', '--list', self.branch), '')

    def test_retry_reuses_existing_branch(self):
        self.publish_branch()
        self.run_job(self.checkout('retry-run', 'main'), 'push', 'refs/heads/main', self.FRESH)
        dispatches = [c for c in self.forge.calls if c[0] == 'POST']
        self.assertEqual(len(dispatches), 2)

    def test_current_readme_does_nothing(self):
        self.run_job(self.checkout('main-run', 'main'), 'push', 'refs/heads/main', self.STALE)
        self.assertEqual([c[0] for c in self.forge.calls], ['GET'])
        self.assertEqual(git(self.origin, 'branch', '--list', self.branch), '')

    def test_dispatch_rejects_other_files(self):
        work = self.publish_branch()
        (work / 'configure.py').write_text('x')
        git(work, 'add', 'configure.py')
        git(work, 'commit', '-q', '--amend', '--no-edit')
        with self.assertRaises(ValueError):
            self.run_job(work, 'workflow_dispatch', 'refs/heads/' + self.branch, self.FRESH)
        self.assertEqual(git(self.origin, 'rev-parse', 'main'), self.source)

    def test_dispatch_rejects_figures_that_do_not_reproduce(self):
        work = self.publish_branch()
        other = self.FRESH.replace('fresh', 'different')
        with self.assertRaises(ValueError):
            self.run_job(work, 'workflow_dispatch', 'refs/heads/' + self.branch, other)
        self.assertEqual(git(self.origin, 'rev-parse', 'main'), self.source)

    def test_dispatch_skips_when_main_advanced(self):
        work = self.publish_branch()
        other = self.checkout('other', 'main')
        (other / 'x').write_text('x')
        git(other, 'add', 'x')
        git(other, 'commit', '-q', '-m', 'advance')
        git(other, 'push', '-q', 'origin', 'HEAD:main')
        advanced = git(other, 'rev-parse', 'HEAD')
        self.run_job(work, 'workflow_dispatch', 'refs/heads/' + self.branch, self.FRESH)
        self.assertEqual(git(self.origin, 'rev-parse', 'main'), advanced)

    def test_stale_push_run_does_not_publish(self):
        work = self.checkout('main-run', 'main')
        other = self.checkout('other', 'main')
        (other / 'x').write_text('x')
        git(other, 'add', 'x')
        git(other, 'commit', '-q', '-m', 'advance')
        git(other, 'push', '-q', 'origin', 'HEAD:main')
        self.run_job(work, 'push', 'refs/heads/main', self.FRESH)
        self.assertEqual(git(self.origin, 'branch', '--list', self.branch), '')
