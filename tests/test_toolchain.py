import unittest
from tools.project import Object, ProjectConfig
from tools.prodg_cc import split_flags


class ToolchainTests(unittest.TestCase):
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
