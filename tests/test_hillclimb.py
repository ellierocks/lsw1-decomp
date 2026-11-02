"""Exercise experiment retention, strict acceptance, errors and stop budgets."""
import contextlib
import io
import json
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest import mock

from tools import hillclimb as hc
from tools import ls1_task_pack as packs

_quiet = None


def setUpModule():
    """Keep the tools' progress and usage output out of the test log."""
    global _quiet
    _quiet = contextlib.ExitStack()
    _quiet.enter_context(contextlib.redirect_stdout(io.StringIO()))
    _quiet.enter_context(contextlib.redirect_stderr(io.StringIO()))


def tearDownModule():
    _quiet.close()


def report(verdict='MISMATCH', differences=2, exact=2):
    return dict(verdict=verdict, differences=[{}] * differences,
                target_insns=4, mine_insns=4, exact_insns=exact, references=[])


class HillclimbTests(unittest.TestCase):
    def test_strict_match_beats_unverified_and_partial(self):
        partial = hc.score(report())
        unknown = hc.score(report('MATCH (unverified references)', 0, 4))
        verified = hc.score(report('MATCH', 0, 4))
        self.assertLess(unknown, partial)
        self.assertLess(verified, unknown)

    def test_errors_and_regressions_do_not_replace_best(self):
        attempts = [dict(attempt=1, score=[1, 2], returncode=1),
                    dict(attempt=2, score=None, returncode=2),
                    dict(attempt=3, score=[1, 3], returncode=1)]
        result = hc.summarize(attempts, 2)
        self.assertEqual(result['state'], 'PARK')
        self.assertEqual(result['best_attempt'], 1)

    def test_no_name_hit_records_limit_without_note(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / 'source.c').write_text('int foo(void) { return 1; }')
            for name in ('configure.py', 'tools/prodg_cc.py', 'tools/verify_fn.py',
                         'config/GL5E4F/symbols.txt', 'config/GL5E4F/splits.txt'):
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text('fixture')
            proc = subprocess.CompletedProcess([], 1, json.dumps(report()), '')
            no_hit = dict(result='no-name-hit', revision='r1', matches=[])
            with mock.patch.object(hc, 'ROOT', root), \
                 mock.patch.object(hc.opensaga, 'lookup', return_value=no_hit), \
                 mock.patch.object(hc.subprocess, 'run', return_value=proc), \
                 mock.patch.object(hc.subprocess, 'check_output', return_value='head\n'), \
                 mock.patch('builtins.print'):
                self.assertEqual(hc.main(['try', 'foo', '--unit', 'foo', '--source', 'source.c',
                                          '--hypothesis', 'baseline']), 1)
            record = json.loads(next((root / 'work/hillclimb').glob('*/*.json')).read_text())
            self.assertIn('no OpenSaga name hit at r1', record['opensaga']['review_note'])

    def test_real_recording_path_preserves_source_and_error(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / 'source.c').write_text('int foo(void) { return 1; }')
            for name in ('configure.py', 'tools/prodg_cc.py', 'tools/verify_fn.py',
                         'config/GL5E4F/symbols.txt', 'config/GL5E4F/splits.txt'):
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text('fixture')
            args = ['try', 'foo', '--unit', 'foo', '--source', 'source.c',
                    '--hypothesis', 'baseline', '--patience', '1',
                    '--donor-note', 'unavailable in fixture; GC baseline only']
            proc = subprocess.CompletedProcess([], 1, json.dumps(report()), '')
            unavailable = dict(result='unavailable', revision='r1', matches=[])
            with mock.patch.object(hc, 'ROOT', root), \
                 mock.patch.object(hc.opensaga, 'lookup', return_value=unavailable), \
                 mock.patch.object(hc.subprocess, 'run', return_value=proc) as run, \
                 mock.patch.object(hc.subprocess, 'check_output', return_value='head\n'), \
                 mock.patch('builtins.print'):
                with self.assertRaises(SystemExit):
                    hc.main(args[:-2])
                run.assert_not_called()
                self.assertEqual(hc.main(args), 1)
                self.assertIn('--strict-symbols', run.call_args.args[0])
                self.assertIn('--build', run.call_args.args[0])
                run.return_value = subprocess.CompletedProcess([], 2, '{}', 'compile failed')
                self.assertEqual(hc.main(args[:-2]), 2)
                with self.assertRaises(SystemExit):
                    hc.main(args)
            records = sorted((root / 'work/hillclimb').glob('*/*.json'))
            self.assertEqual(len(records), 2)
            first = json.loads(records[0].read_text())
            self.assertEqual(first['sources']['source.c']['text'], (root / 'source.c').read_text())
            self.assertEqual(first['opensaga']['review_note'], 'unavailable in fixture; GC baseline only')
            second = json.loads(records[1].read_text())
            self.assertEqual(first['opensaga'], second['opensaga'])
            self.assertIsNone(second['score'])
            self.assertEqual(second['stderr'], 'compile failed')
            (root / 'configure.py').write_text('changed compiler flags')
            with mock.patch.object(hc, 'ROOT', root), self.assertRaises(SystemExit):
                hc.main(args)
            self.assertEqual(len(list((root / 'work/hillclimb').glob('*/*.json'))), 2)

    def test_task_script_uses_strict_oracle(self):
        script = packs.gen_verify('Foo', 0x80000000, 8, 'numath')
        self.assertIn('--strict-symbols', script)
        self.assertIn('--build', script)
        self.assertNotIn('screenshot', script)
        subprocess.run(['bash', '-n'], input=script, text=True, check=True)

    def test_regeneration_preserves_existing_candidate(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            target = root / 'Foo'
            target.mkdir()
            source = target / 'candidate.c'
            source.write_text('valuable candidate')
            with self.assertRaises(SystemExit):
                packs.generate_pack('Foo', 0x80000000, 4, root, asm='blr')
            self.assertEqual(source.read_text(), 'valuable candidate')

    def test_split_assembly_lookup_uses_exact_symbol(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / 'numath').mkdir()
            (root / 'numath/unit.s').write_text(
                '.fn FooExtra, global\nwrong\n.endfn FooExtra\n'
                '.fn Foo, global\nblr\n.endfn Foo\n')
            with mock.patch.object(packs, 'ASM_FILE', root / 'missing.s'):
                self.assertEqual(packs.extract_asm(0x80000000, 4, 'Foo'),
                                 '.fn Foo, global\nblr\n.endfn Foo\n')
