"""Exercise pinned donor provenance and the evidence reaching generated tasks."""
import contextlib
import io
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest import mock

from tools import opensaga as saga
from tools import ls1_task_pack as packs


class OpenSagaTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.repo = self.root / 'donor'
        self.repo.mkdir()
        self.git('init', '-q')
        source = self.repo / 'src/nu2api/helper.c'
        source.parent.mkdir(parents=True)
        source.write_text('int NuExample(void) { return 7; }\n')
        functions = [{'name': 'NuExample', 'demangled_name': 'NuExample()', 'match_percent': 100}]
        (self.repo / 'matching.json').write_text(json.dumps({'units': [
            {'source': 'src/nu2api/helper.c', 'functions': functions},
            {'source': 'src/another.c', 'functions': functions},
            {'source': 'src/../../outside.c', 'functions': functions}]}))
        self.git('add', '.')
        self.git('-c', 'user.name=Test', '-c', 'user.email=test@example.invalid', 'commit', '-qm', 'test: add fixture')
        self.revision = self.git('rev-parse', 'HEAD')
        self.pin = self.root / 'pin.json'
        self.pin.write_text(json.dumps({'repository': str(self.repo), 'revision': self.revision}))
        self.patch = contextlib.ExitStack()
        self.addCleanup(self.patch.close)
        self.patch.enter_context(mock.patch.object(saga, 'PIN', self.pin))
        self.patch.enter_context(mock.patch.dict(os.environ, {'OPENSAGA': str(self.repo)}))

    def git(self, *args):
        return subprocess.check_output(['git', '-C', str(self.repo), *args], text=True,
                                       stderr=subprocess.PIPE).strip()

    def test_lookup_preserves_ambiguous_owners_and_excludes_unsafe_paths(self):
        result = saga.lookup('NuExample')
        self.assertEqual(result['revision'], self.revision)
        self.assertEqual(result['result'], 'name-lead')
        self.assertEqual(len(result['matches']), 2)
        self.assertEqual(saga.lookup('NuAbsent')['result'], 'no-name-hit')
        with contextlib.redirect_stdout(io.StringIO()) as output:
            self.assertEqual(saga.main(['search', 'return 7']), 0)
        self.assertEqual(json.loads(output.getvalue())['hits'], [
            {'source': 'src/nu2api/helper.c', 'line': 1}])

    def test_missing_dirty_and_wrong_revision_are_not_no_hit(self):
        (self.repo / 'src/nu2api/helper.c').write_text('modified')
        self.assertEqual(saga.lookup('NuExample')['status'], 'modified')
        self.assertEqual(saga.lookup('NuExample')['result'], 'unavailable')
        with contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(saga.main(['fetch']), 2)
        self.assertEqual((self.repo / 'src/nu2api/helper.c').read_text(), 'modified')
        pin = json.loads(self.pin.read_text())
        pin['revision'] = '0' * 40
        self.pin.write_text(json.dumps(pin))
        self.assertEqual(saga.lookup('NuExample')['status'], 'wrong-revision')
        with mock.patch.dict(os.environ, {'OPENSAGA': str(self.root / 'missing')}):
            self.assertEqual(saga.lookup('NuExample')['result'], 'unavailable')

    def test_fetch_checks_out_pin_even_when_remote_head_has_advanced(self):
        (self.repo / 'new').write_text('later commit')
        self.git('add', 'new')
        self.git('-c', 'user.name=Test', '-c', 'user.email=test@example.invalid', 'commit', '-qm', 'test: advance fixture')
        destination = self.root / 'cache'
        with mock.patch.dict(os.environ, {'OPENSAGA': str(destination)}), \
             contextlib.redirect_stdout(io.StringIO()):
            saga.fetch()
            self.assertEqual(saga.lookup('NuExample')['status'], 'ready')
            self.assertFalse((destination / 'new').exists())
            saga.fetch()  # Reuse the same pin without fetching/updating it.

    def test_generated_pack_contains_provenance_but_no_donor_source(self):
        output = self.root / 'tasks'
        with mock.patch.object(packs, 'extract_struct_defs', return_value=('', None)), \
             mock.patch.object(packs, 'callers_of', return_value=[]), \
             mock.patch.object(packs, 'find_data_refs', return_value=[]), \
             contextlib.redirect_stdout(io.StringIO()):
            packs.generate_pack('NuExample', 0x80000000, 4, output,
                                mac_hits=[], callee_list=[], asm='blr')
        evidence = (output / 'NuExample/donor_evidence.md').read_text()
        self.assertIn(self.revision, evidence)
        self.assertIn('src/nu2api/helper.c', evidence)
        self.assertNotIn('return 7', evidence)
        self.assertIn('--donor-note', (output / 'NuExample/prompt.md').read_text())

    def test_address_lookup_uses_current_gc_names(self):
        config = self.root / 'config/GL5E4F'
        config.mkdir(parents=True)
        (config / 'symbols.txt').write_text('NuExample = .text:0x80000000; // type:function size:0x4\n')
        with mock.patch.object(saga, 'ROOT', self.root):
            self.assertEqual(saga.resolve('0x80000000'), 'NuExample')
            with self.assertRaises(ValueError):
                saga.resolve('0x80000004')
