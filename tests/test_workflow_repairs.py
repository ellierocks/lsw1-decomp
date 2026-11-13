"""Regressions from the creature sprint's observed workflow failures."""
import contextlib
import io
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
from tools import draft_fn, git_origin, m2c_helpers, score_fn, sweep


class DraftRepairs(unittest.TestCase):
    def test_byte_offsets_keep_byte_units_when_arrayized(self):
        source = 'extern int data;\nint main(void) { return ((char *)(&data + 3) - (char *)&data) != 3; }\n'
        repaired = draft_fn.arrayize(source, {'data'}).replace('s8', 'char')
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp)
            (path / 'test.c').write_text(repaired + '\nint data[4];\n')
            subprocess.run(['cc', str(path / 'test.c'), '-o', str(path / 'test')], check=True,
                           capture_output=True)
            self.assertEqual(subprocess.run([str(path / 'test')]).returncode, 0)

    def test_indirect_abi_flag_and_mixed_calls(self):
        source = 's32 (*callback)(s32 *, s32 *);\ns32 (*factory(s32 *, s32))(s32 *, s32 *);\n'
        asm = 'mtlr r0\ncrclr cr1eq\nblrl\n'
        out = m2c_helpers.repair_indirect_prototypes(source, asm)
        self.assertIn('(*callback)();', out)
        self.assertIn('(*factory(s32 *, s32))();', out)
        self.assertEqual(m2c_helpers.repair_indirect_prototypes(source, asm + 'blrl\n'), source)

    def test_cr_logic_preserves_condition_bits_and_branch(self):
        asm = 'cror un, eq, gt\nbns .L1\ncror cr7un, cr7eq, cr7gt\n'
        out = m2c_helpers.prepare_asm(asm)
        self.assertIn('cror cr0un, cr0eq, cr0gt\nbns .L1', out)
        self.assertIn('cror cr7un, cr7eq, cr7gt', out)

    def test_splice_replaces_only_selected_function(self):
        source = 'int first(void) { return 1; }\nint second(void) { return 2; }\n'
        out = score_fn.splice_body(source, 'second', 'int second(void) { return 3; }')
        self.assertIn('first(void) { return 1; }', out)
        self.assertIn('second(void) { return 3; }', out)
        self.assertNotIn('return 2;', out)

    def test_splice_drops_externs_and_structs_the_unit_already_has(self):
        source = ('struct v { int x; };\nf32 helper(void *, void *);\nextern u8 table[];\n'
                  'int first(void) { return 1; }\n')
        body = ('struct v { int x; };\nstruct w { int y; };\nextern s32 helper(void *);\n'
                'extern u8 table[];\nextern void (*cb)(void);\nint first(void) { return 2; }')
        out = score_fn.splice_body(source, 'first', body)
        self.assertEqual(out.count('struct v {'), 1)
        self.assertIn('struct w { int y; };', out)
        self.assertNotIn('extern s32 helper', out)
        self.assertEqual(out.count('table[]'), 1)
        self.assertIn('extern void (*cb)(void);', out)
        self.assertIn('return 2;', out)

    def test_splice_appends_after_parked_disabled_draft(self):
        source = ('int before(void) { return 1; }\n#if 0\n'
                  'int target(void) { return 2; }\n#endif\n')
        out = score_fn.splice_body(source, 'target', 'int target(void) { return 3; }')
        self.assertIn('#if 0\nint target(void) { return 2; }\n#endif', out)
        self.assertIn('#endif\n\nint target(void) { return 3; }', out)

    def test_splice_skips_disabled_elif_and_compiles_active_replacement(self):
        source = ('#if 0\n#elif 0\nint target(void) { return 2; }\n'
                  '#else\nint target(void) { return 1; }\n#endif\n')
        out = score_fn.splice_body(source, 'target', 'int target(void) { return 3; }')
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp)
            (path / 'test.c').write_text(out + '\nint main(void) { return target() != 3; }\n')
            subprocess.run(['cc', str(path / 'test.c'), '-o', str(path / 'test')],
                           check=True, capture_output=True)
            self.assertEqual(subprocess.run([str(path / 'test')]).returncode, 0)


class GitTransport(unittest.TestCase):
    def invoke(self, results):
        with patch.object(git_origin.subprocess, 'run', side_effect=results) as run, \
             contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            code = git_origin.run(['push', 'origin', 'HEAD'])
        return code, run.call_args_list

    def test_public_key_failure_retries_same_repo_without_persisting_config(self):
        result = lambda code, out='', err='': subprocess.CompletedProcess([], code, out, err)
        code, calls = self.invoke([result(128, err='Permission denied (publickey)'),
                                  result(0, 'git@github.com:ellierocks/lsw1-decomp.git\n'), result(0)])
        self.assertEqual(code, 0)
        args = calls[2].args[0]
        self.assertIn('url.https://github.com/ellierocks/lsw1-decomp.git.insteadOf=git@github.com:ellierocks/lsw1-decomp.git', args)
        self.assertIn('credential.helper=!gh auth git-credential', args)
        self.assertEqual(args[-3:], ['push', 'origin', 'HEAD'])
        self.assertNotIn('config', args)

    def test_other_errors_are_not_retried(self):
        code, calls = self.invoke([subprocess.CompletedProcess([], 1, '', 'non-fast-forward')])
        self.assertEqual(code, 1)
        self.assertEqual(len(calls), 1)

    def test_non_github_remote_is_not_redirected(self):
        code, calls = self.invoke([subprocess.CompletedProcess([], 128, '', 'Permission denied (publickey)'),
                                  subprocess.CompletedProcess([], 0, 'git@example.org:repo.git', '')])
        self.assertEqual(code, 128)
        self.assertEqual(len(calls), 2)


class ScanIsolation(unittest.TestCase):
    def test_sweep_scoring_never_claims_queue_entries(self):
        result = subprocess.CompletedProcess([], 0, '{"verdict":"MATCH", "target":[], "compiled":[], "differences":[]}', '')
        for apply in (False, True):
            with patch.object(sweep.subprocess, 'run', return_value=result) as run:
                sweep.score('outside_sprint', 'source.c', 'void outside_sprint(void) {}', apply)
            self.assertIn('--no-queue', run.call_args.args[0])

    def test_no_queue_skips_queue_mutation_after_verifying(self):
        import json
        def verify(args):
            print(json.dumps({'verdict': 'MATCH', 'differences': [], 'target': [], 'compiled': []}))
            return 0
        with patch.object(score_fn, 'missing_toolchain', return_value=[]), \
             patch.object(score_fn, 'compile_source', return_value=None), \
             patch.object(score_fn.vf, 'main', side_effect=verify), \
             patch.object(score_fn, 'score', return_value=[]), \
             patch.object(score_fn, 'note_queue') as queue, \
             contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(score_fn.main(['f', 'scratch.c', '--json', '--no-queue']), 0)
            queue.assert_not_called()
            self.assertEqual(score_fn.main(['f', 'scratch.c', '--json']), 0)
            queue.assert_called_once_with('f')


class SweepPreflight(unittest.TestCase):
    def test_missing_assembly_fails_before_target_scan(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / 'm2c.py').touch()
            with patch.object(sweep, 'ROOT', root), \
                 patch.object(sweep.draft_fn, 'm2c_dir', return_value=root), \
                 patch.object(sweep, 'targets') as targets, \
                 contextlib.redirect_stderr(io.StringIO()) as errors:
                self.assertEqual(sweep.main([]), 2)
            targets.assert_not_called()
            self.assertIn('bash build.sh', errors.getvalue())

    def test_interactive_ps_draft_uses_three_argument_helper(self):
        source = 's32 NuVecAddPS(s32, s32, s32, s32);\nvoid f(void) {\n    r = NuVecAddPS(a, b, c, d);\n}\n'
        out = draft_fn.clean(source, 'f')
        self.assertIn('#include "numath/nuvec_ps.h"', out)
        self.assertIn('NuVecAddPS(a, b, c);', out)
        self.assertIn('r = a;', out)
        self.assertNotIn('s32 NuVecAddPS(', out)


class AtomicToolDownload(unittest.TestCase):
    def test_replaces_running_executable_without_text_file_busy(self):
        import shutil
        from tools import download_tool
        with tempfile.TemporaryDirectory() as tmp:
            tool = Path(tmp) / 'tool'
            shutil.copyfile('/bin/sleep', tool)
            tool.chmod(0o755)
            proc = subprocess.Popen([str(tool), '30'])
            try:
                download_tool.download('https://example.test/tool', io.BytesIO(b'new binary'), tool)
                self.assertEqual(tool.read_bytes(), b'new binary')
                self.assertTrue(tool.stat().st_mode & 0o100)
                self.assertIsNone(proc.poll())
            finally:
                proc.terminate()
                proc.wait()

    def test_interrupted_download_preserves_installed_tool_and_removes_temp(self):
        from tools import download_tool
        class Interrupted:
            def read(self, size):
                raise OSError('download interrupted')
        with tempfile.TemporaryDirectory() as tmp:
            tool = Path(tmp) / 'tool'
            tool.write_bytes(b'old binary')
            with self.assertRaisesRegex(OSError, 'interrupted'):
                download_tool.download('https://example.test/tool', Interrupted(), tool)
            self.assertEqual(tool.read_bytes(), b'old binary')
            self.assertEqual(list(Path(tmp).iterdir()), [tool])
