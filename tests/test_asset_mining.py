"""Synthetic file-format and provenance tests; no proprietary fixtures."""
import importlib.util
import struct
import subprocess
import sys
import json
import tempfile
import unittest
from pathlib import Path

SPEC = importlib.util.spec_from_file_location('asset_mining', Path(__file__).resolve().parents[1] / 'tools/asset_mining.py')
a = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(a)


class AssetMiningTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def disc(self):
        raw = bytearray(0x1000)
        struct.pack_into('>I', raw, 0x1C, 0xC2339F3D)
        names = b'folder\0test.scp\0empty\0'
        fst = b''.join(struct.pack('>III', *entry) for entry in [
            (0x1000000, 0, 4), (0x1000000, 0, 3), (7, 0x800, 3), (16, 0x803, 0)]) + names
        struct.pack_into('>II', raw, 0x424, 0x500, len(fst))
        raw[0x500:0x500 + len(fst)] = fst
        raw[0x800:0x803] = b'Go\n'
        path = self.root / 'disc.iso'
        path.write_bytes(raw)
        return path

    def test_gc_inventory_nested_and_empty(self):
        path = self.disc()
        manifest = a.inventory(path, 'synthetic', 'gc')
        self.assertEqual([r['member'] for r in manifest['files']], ['folder/test.scp', 'empty'])
        self.assertEqual(manifest['files'][0]['offset'], 0x800)
        indexed = a.index_scripts(manifest, ['.scp'], {'commands': {'Go': [{'address': '0x80001234'}]}})
        token = indexed['scripts'][0]['records'][0]
        self.assertEqual(token['tokens'][0]['byte'], 0)
        self.assertEqual(token['candidates'][0]['handlers'][0]['address'], '0x80001234')

    def test_gc_rejects_bad_ranges_parent_and_paths(self):
        for field, value in [(0x500 + 24 + 4, 0xFFFF), (0x500 + 12 + 4, 2),
                             (0x500 + 12 + 8, 5)]:
            path = self.disc()
            raw = bytearray(path.read_bytes())
            struct.pack_into('>I', raw, field, value)
            path.write_bytes(raw)
            with self.assertRaises(ValueError):
                a.gc_members(path)
        path = self.disc()
        raw = path.read_bytes().replace(b'test.scp', b'../a.scp')
        path.write_bytes(raw)
        with self.assertRaises(ValueError):
            a.gc_members(path)

    def test_hash_mismatch_and_loose_symlinks(self):
        corpus = self.root / 'corpus'
        corpus.mkdir()
        file = corpus / 'test.scp'
        file.write_bytes(b'Go')
        manifest = a.inventory(corpus, 'test', 'loose')
        file.write_bytes(b'No')
        with self.assertRaisesRegex(ValueError, 'changed'):
            a.index_scripts(manifest, ['.scp'], {})
        (corpus / 'link').symlink_to(file)
        with self.assertRaisesRegex(ValueError, 'Symlink'):
            a.inventory(corpus, 'test', 'loose')

    def test_encoding_comments_quotes_and_ambiguous_commands(self):
        text = 'STATE Foo {\r\n  IF Go "Go // not a comment" // Go\r\n Unknown 1\n}\n'
        result = a.script_records(b'\xff\xfe' + text.encode('utf-16-le'), {'Go': [{'address': 'x'}]})
        records = result['records']
        usage = records[1]
        self.assertEqual(usage['scope'][0]['label'], 'STATE Foo')
        self.assertEqual(len(usage['candidates']), 1)
        token = usage['tokens'][1]
        self.assertEqual(token['line'], 2)
        self.assertEqual(token['byte'], 2 + text.index('Go') * 2)
        self.assertEqual(records[2]['tokens'][0]['text'], 'Unknown')
        self.assertFalse(result['diagnostics'])
        self.assertEqual(a.script_records(b'\xff', {})['status'], 'unparsed')
        separate = a.script_records(b'STATE Foo\n{\nGo\n}', {})
        self.assertEqual(separate['records'][1]['scope'][0]['label'], 'STATE Foo')
        for raw in [b'Go "unterminated', b'Go /* unfinished', b'}', b'{']:
            self.assertTrue(a.script_records(raw, {})['diagnostics'], raw)

    def test_cli_inventory_index_and_refuse_overwrite(self):
        project = Path(__file__).resolve().parents[1]
        (project / 'work').mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=project / 'work') as output_dir:
            output = Path(output_dir)
            corpus = self.root / 'corpus'
            corpus.mkdir()
            (corpus / 'test.scp').write_text('STATE Foo {\nUnknown 1\n}\n')
            cli = [sys.executable, str(project / 'tools/asset_mining.py')]
            manifest = output / 'inventory.json'
            run = subprocess.run(cli + ['inventory', str(corpus), '--kind', 'loose',
                                        '--build', 'fixture', '--output', str(manifest)], capture_output=True)
            self.assertEqual(run.returncode, 0, run.stderr)
            result = output / 'scripts.json'
            run = subprocess.run(cli + ['scripts', str(manifest), '--output', str(result)], capture_output=True)
            self.assertEqual(run.returncode, 0, run.stderr)
            report = json.loads(result.read_text())
            self.assertEqual(report['build'], 'fixture')
            self.assertEqual(report['scripts'][0]['records'][1]['tokens'][0]['text'], 'Unknown')
            original = result.read_bytes()
            run = subprocess.run(cli + ['scripts', str(manifest), '--output', str(result)], capture_output=True)
            self.assertNotEqual(run.returncode, 0)
            self.assertEqual(result.read_bytes(), original)
            run = subprocess.run(cli + ['scripts', str(manifest), '--output', str(self.root / 'outside.json')], capture_output=True)
            self.assertNotEqual(run.returncode, 0)
            self.assertFalse((self.root / 'outside.json').exists())

    def test_dispatch_uses_real_sections_and_keeps_collisions(self):
        raw = bytearray(0x100 + 0x300000)
        struct.pack_into('>III', raw, 0, 0x100, 0, 0)
        struct.pack_into('>I', raw, 0x48, 0x80000000)
        struct.pack_into('>I', raw, 0x90, len(raw) - 0x100)
        for base, stride, _ in a.TABLES:
            off = base - 0x80000000 + 0x100
            struct.pack_into('>II', raw, off, 0x80280000, 0x80001000)
        raw[0x280100:0x280103] = b'Go\0'
        dol = self.root / 'main.bin'
        dol.write_bytes(raw)
        symbols = self.root / 'symbols'
        symbols.write_text('Action_Go = .text:0x80001000; // type:function\n')
        result = a.dispatch(dol, symbols)
        self.assertEqual(len(result['commands']['Go']), len(a.TABLES))
        self.assertEqual(result['commands']['Go'][0]['symbol'], 'Action_Go')
        raw[0x280100:0x280200] = b'a' * 256
        dol.write_bytes(raw)
        with self.assertRaisesRegex(ValueError, 'Unterminated'):
            a.dispatch(dol, symbols)


if __name__ == '__main__':
    unittest.main()
