import unittest
import subprocess
import sys
import tempfile
from pathlib import Path
from tools.project import Object, ProjectConfig
from tools.prodg_cc import split_flags


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
