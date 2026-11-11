import unittest
from tools import sweep
from sweep_repairs.unprototyped import unprototyped_calls


class UnprototypedTest(unittest.TestCase):
    ASM = '/* 0 */\tcrclr cr1eq\n/* 4 */\tbl memset\n'

    def test_alias_keeps_return_type(self):
        body = '#define memset ((s32 (*)(void *, s32, u32))memset)\nvoid f(void) {\n    memset(p, 0, 12);\n}\n#undef memset\n'
        out = unprototyped_calls(body, 'f', self.ASM, '')
        self.assertIn('#define memset ((s32 (*)())memset)', out)
        self.assertIn('    memset(p, 0, 12);', out)
        self.assertEqual(unprototyped_calls(out, 'f', self.ASM, ''), out)

    def test_unit_prototype_stays_unchanged(self):
        body = 'void f(void) {\n    memset(p, 0, 12);\n}\n'
        out = unprototyped_calls(body, 'f', self.ASM, 'extern void *memset(void *, int, unsigned int);')
        self.assertIn('((void * (*)())(memset))(p, 0, 12);', out)

    def test_cr6_numeric_and_three_operand_spellings(self):
        body = 's32 memset(s32, s32, s32);\nvoid f(void) {\n    memset(p, 0, 12);\n}\n'
        for insn in ('crclr 6', 'crset 6', 'crxor 6, 6, 6',
                     'creqv cr1eq, cr1eq, cr1eq'):
            out = unprototyped_calls(body, 'f', self.ASM.replace('crclr cr1eq', insn), '')
            self.assertIn('((s32 (*)())(memset))(', out, insn)

    def test_no_evidence_or_return_type_leaves_body(self):
        body = 'void f(void) {\n    memset(p, 0, 12);\n}\n'
        self.assertEqual(unprototyped_calls(body, 'f', self.ASM, ''), body)
        self.assertEqual(unprototyped_calls(body, 'f', '\tbl memset\n', 'void memset();'), body)

    def test_declarations_and_other_calls_are_unchanged(self):
        body = 's32 memset(s32, s32, s32);\nvoid f(void) {\n    other();\n    memset(p, 0, 12);\n}\n'
        out = unprototyped_calls(body, 'f', self.ASM, '')
        self.assertTrue(out.startswith('s32 memset(s32, s32, s32);'))
        self.assertIn('    other();', out)


if __name__ == '__main__':
    unittest.main()
