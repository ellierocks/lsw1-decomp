import os
import unittest
import subprocess
import sys
import tempfile
from pathlib import Path
from tools.project import Object, ProjectConfig
from tools.prodg_cc import NGCCC_TARGET_DEFS, split_flags


class ToolchainTests(unittest.TestCase):
    def test_rtl_diagnostics_preserve_real_prodg_object(self):
        root = Path(__file__).resolve().parents[1]
        required = ['build/tools/wibo', 'build/tools/dtk_patched',
                    'build/binutils/powerpc-eabi-as', 'build/compilers/ProDG/3.9.3/cc1.exe']
        if not all((root / path).is_file() for path in required):
            self.skipTest('needs bootstrapped ProDG tools')
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            command = [sys.executable, str(root / 'tools/score_fn.py'), 'NuRandInt',
                       str(root / 'src/numath/nurand.c'), '--compile-only']
            for name, extra in [('plain', []), ('rtl', ['--diagnostics-dir', str(tmp / 'dumps')])]:
                result = subprocess.run(command + ['-o', str(tmp / (name + '.o'))] + extra,
                                        cwd=root, capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertEqual((tmp / 'plain.o').read_bytes(), (tmp / 'rtl.o').read_bytes())
            for suffix in ['greg', 'lreg', 'sched', 'sched2', 'loop', 'gcse']:
                self.assertTrue((tmp / 'dumps' / ('candidate.i.' + suffix)).stat().st_size)
            self.assertIn('SN BUILD', (tmp / 'dumps/compiler.log').read_text())

    def test_prodg_does_not_depend_on_linker_version(self):
        config = ProjectConfig()
        config.linker_version = 'GC/1.3.2'
        obj = Object(False, 'test.c', compiler='prodg', compiler_version='ProDG/3.9.3')
        resolved = obj.resolve(config, {})
        self.assertEqual(resolved.options['compiler'], 'prodg')
        self.assertEqual(resolved.options['compiler_version'], 'ProDG/3.9.3')

    def test_unknown_backend_fails(self):
        with self.assertRaises(ValueError):
            Object(False, 'test.c', compiler='typo').resolve(ProjectConfig(), {})

    def test_preprocessor_operands_are_not_source_files(self):
        cpp, cc1, source, output = split_flags(
            ['-I', 'include dir', '-DVALUE=1', '-O2', '-c', 'source.c', '-o', 'out.o'])
        self.assertEqual(cpp, ['-I', 'include dir', '-DVALUE=1'])
        self.assertEqual(cc1, ['-O2'])
        self.assertEqual((source, output), ('source.c', 'out.o'))

    def test_small_data_threshold_is_joined_for_cc1(self):
        cpp, cc1, source, output = split_flags(['-G', '1024', '-O2', '-c', 'source.c', '-o', 'out.o'])
        self.assertEqual((cpp, cc1), ([], ['-G1024', '-O2']))
        self.assertEqual((source, output), ('source.c', 'out.o'))

    def test_ngccc_defines_are_opt_in_and_use_the_driver_target_macros(self):
        root = Path(__file__).resolve().parents[1]
        source = (root / 'tools/prodg_cc.py').read_text()
        self.assertIn('add_argument("--ngccc-defines", action="store_true"', source)
        self.assertIn('-D__GNUC__=2', NGCCC_TARGET_DEFS)
        self.assertIn('-D__CHAR_UNSIGNED__', NGCCC_TARGET_DEFS)
        self.assertEqual(len(NGCCC_TARGET_DEFS), len(set(NGCCC_TARGET_DEFS)))

    def test_default_ninja_command_does_not_name_native_or_evaluation_flags(self):
        config = ProjectConfig()
        self.assertEqual((config.prodg_extra_args, config.prodg_extra_inputs), ([], []))

    def test_diagnostics_build_does_not_change_object(self):
        root = Path(__file__).resolve().parents[1]
        required = ['build/tools/wibo', 'build/tools/dtk_patched', 'build/binutils/powerpc-eabi-as',
                    'build/sn-gcc/v179/cc1', 'build/sn-gcc/v179-diag/cc1']
        if not all((root / path).is_file() for path in required):
            self.skipTest('needs tools/sn-gcc/build.sh outputs (v179, v179-diag)')
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            objs = {}
            for name, cc1, env in [('plain', 'v179', {}), ('diag', 'v179-diag', {'GDBG': '1', 'LADBG': '1'})]:
                result = subprocess.run(
                    [sys.executable, str(root / 'tools/score_fn.py'), 'NuRandInt', str(root / 'src/numath/nurand.c'),
                     '--compile-only', '--cc1-native', str(root / 'build/sn-gcc' / cc1 / 'cc1'),
                     '--diagnostics-dir', str(tmp / name), '-o', str(tmp / (name + '.o'))],
                    cwd=root, capture_output=True, text=True, env={**os.environ, **env})
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objs[name] = (tmp / (name + '.o')).read_bytes()
            self.assertEqual(objs['plain'], objs['diag'])
            self.assertIn('LADBG', (tmp / 'diag/compiler.log').read_text())
            self.assertNotIn('LADBG', (tmp / 'plain/compiler.log').read_text())
