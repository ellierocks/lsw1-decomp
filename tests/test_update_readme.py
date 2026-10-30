import sys
import unittest
from pathlib import Path
from unittest.mock import patch
import base64

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import update_readme as readme


class ReadmeTests(unittest.TestCase):
    def assert_no_internal_pushes(self, forge):
        for call in forge.return_value.request.call_args_list:
            readme.check_api_call(*call.args[:2])

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

    def test_no_change_does_not_write_to_forge(self):
        text = 'current README'
        with patch.object(readme, 'Forgejo') as forge, patch.object(readme.Path, 'read_text', return_value='{}'), patch.dict(readme.os.environ, {
            'GITHUB_EVENT_PATH': '/event.json', 'GITHUB_EVENT_NAME': 'push',
            'GITHUB_REF': 'refs/heads/main', 'GITHUB_SHA': 'abc'}):
            forge.return_value.request.side_effect = [
                {'commit': {'id': 'abc'}}, {'content': base64.b64encode(text.encode()).decode()}]
            readme.publish(text)
            self.assertTrue(all(call.args[0] == 'GET' for call in forge.return_value.request.call_args_list))

    def test_stale_main_does_not_publish(self):
        with patch.object(readme, 'Forgejo') as forge, patch.object(readme.Path, 'read_text', return_value='{}'), patch.dict(readme.os.environ, {
            'GITHUB_EVENT_PATH': '/event.json', 'GITHUB_EVENT_NAME': 'push',
            'GITHUB_REF': 'refs/heads/main', 'GITHUB_SHA': 'abc'}):
            forge.return_value.request.return_value = {'commit': {'id': 'newer'}}
            readme.publish('README')
            forge.return_value.request.assert_called_once_with('GET', '/branches/main')

    def test_api_refuses_writes_that_push_internally(self):
        with patch.dict(readme.os.environ, {'GITHUB_SERVER_URL': 'http://forge', 'GITHUB_REPOSITORY': 'o/r',
                                            'FORGEJO_TOKEN': 't'}), patch.object(readme.urllib.request, 'urlopen') as urlopen:
            api = readme.Forgejo()
            for method, path in [('POST', '/branches'), ('DELETE', '/branches/x'), ('PUT', '/contents/README.md'),
                                 ('POST', '/pulls/9/merge'), ('POST', '/pulls/9/update')]:
                with self.assertRaises(ValueError):
                    api.request(method, path, {})
            urlopen.assert_not_called()
            urlopen.return_value.__enter__.return_value.read.return_value = b''
            for method, path in [('GET', '/contents/README.md?ref=x'), ('POST', '/pulls'),
                                 ('PATCH', '/pulls/9'), ('POST', '/actions/workflows/build.yml/dispatches')]:
                api.request(method, path, {})
            self.assertEqual(urlopen.call_count, 4)

    def merge_fixture(self, files=None, actual=None):
        block = readme.START + '\ncurrent\n' + readme.END
        text = 'intro\n' + block + '\noutro'
        base = 'intro\n' + readme.START + '\nstale\n' + readme.END + '\noutro'
        pr = {'number': 9, 'head': {'ref': readme.BRANCH_PREFIX + 'source', 'sha': 'checked', 'repo': {'id': 1}},
              'base': {'ref': 'main', 'sha': 'source', 'repo': {'id': 1}}, 'html_url': 'http://forge/pr/9'}
        with patch.object(readme, 'Forgejo') as forge, patch.object(readme, 'git') as git, patch.object(readme.Path, 'read_text', return_value=actual or text), patch.dict(readme.os.environ, {
            'GITHUB_EVENT_NAME': 'workflow_dispatch', 'GITHUB_REF': 'refs/heads/' + pr['head']['ref'], 'GITHUB_SHA': 'checked'}):
            forge.return_value.request.side_effect = [[pr], pr, files or [{'filename': 'README.md'}],
                {'content': base64.b64encode(base.encode()).decode()}, {'merged': False}, None]
            if actual or files:
                with self.assertRaises(ValueError):
                    readme.publish(text)
                self.assertTrue(all(c.args[0] == 'GET' for c in forge.return_value.request.call_args_list))
                git.assert_not_called()
            else:
                readme.publish(text)
                self.assert_no_internal_pushes(forge)
                self.assertEqual(git.call_args_list[0].args, ('push', 'origin', 'checked:refs/heads/main'))
                self.assertEqual(forge.return_value.request.call_args_list[-1].args,
                                 ('PATCH', '/pulls/9', {'state': 'closed'}))
                self.assertEqual(git.call_args_list[-1].args,
                                 ('push', 'origin', '--delete', 'refs/heads/' + pr['head']['ref']))

    def test_merge_guards_checked_head(self):
        self.merge_fixture()

    def test_reject_other_files(self):
        self.merge_fixture(files=[{'filename': 'README.md'}, {'filename': 'configure.py'}])

    def test_reject_extra_readme_edits(self):
        self.merge_fixture(actual='unreviewed README edits')

    def test_publish_pins_source_and_explicitly_dispatches_checks(self):
        encoded = base64.b64encode(b'old README').decode()
        branch = readme.BRANCH_PREFIX + 'source'
        with patch.object(readme, 'Forgejo') as forge, patch.object(readme, 'git') as git, \
                patch.object(readme.Path, 'write_text') as write, patch.dict(readme.os.environ, {
            'GITHUB_EVENT_NAME': 'push', 'GITHUB_REF': 'refs/heads/main', 'GITHUB_SHA': 'source'}):
            forge.return_value.request.side_effect = [
                {'commit': {'id': 'source'}}, {'content': encoded, 'sha': 'blob'}, [],
                {'html_url': 'http://forge/pr/9'}, None]
            git.side_effect = lambda *args: 'source' if args[0] == 'rev-parse' else ''
            readme.publish('new README')
            self.assert_no_internal_pushes(forge)
            write.assert_called_once_with('new README')
            self.assertEqual(git.call_args_list[-1].args, ('push', 'origin', 'HEAD:refs/heads/' + branch))
            calls = forge.return_value.request.call_args_list
            self.assertEqual(calls[3].args[:2], ('POST', '/pulls'))
            self.assertEqual(calls[3].args[2]['head'], branch)
            self.assertEqual(calls[-1].args, ('POST', '/actions/workflows/build.yml/dispatches', {'ref': branch}))

    def test_publish_refuses_checkout_other_than_source(self):
        with patch.object(readme, 'Forgejo') as forge, patch.object(readme, 'git') as git, \
                patch.object(readme.Path, 'write_text') as write, patch.dict(readme.os.environ, {
            'GITHUB_EVENT_NAME': 'push', 'GITHUB_REF': 'refs/heads/main', 'GITHUB_SHA': 'source'}):
            forge.return_value.request.side_effect = [
                {'commit': {'id': 'source'}}, {'content': base64.b64encode(b'old').decode()}, []]
            git.side_effect = lambda *args: 'other' if args[0] == 'rev-parse' else ''
            with self.assertRaises(ValueError):
                readme.publish('new README')
            write.assert_not_called()
