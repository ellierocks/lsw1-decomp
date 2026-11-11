import unittest
from tools import sweep
from sweep_repairs.arithmetic import commuted_adds


class ArithmeticTest(unittest.TestCase):
    def test_bare_assignment_and_parenthesized_expression(self):
        self.assertEqual(commuted_adds('    p = var_r3 + lbl_X;'), ['    p = lbl_X + var_r3;'])
        self.assertEqual(commuted_adds('    f((var_r3 + lbl_X));'), ['    f((lbl_X + var_r3));'])

    def test_do_not_change_binding_or_evaluation(self):
        for expr in ('-var_r3 + lbl_X', 'var_r3 + lbl_X * 4', 'var_r3 + lbl_X->x',
                     '(u8) var_r3 + lbl_X', '*var_r3 + lbl_X', 'var_r3 + lbl_X[0]',
                     'var_r3 + call()', 'var_r3 + temp_r4', 'global_a + global_b'):
            self.assertEqual(commuted_adds('    p = ' + expr + ';'), [], expr)

    def test_bounded_independent_alternatives(self):
        body = '\n'.join('    p = var_r3 + lbl_%d;' % i for i in range(12))
        alts = commuted_adds(body)
        self.assertEqual(len(alts), 8)
        self.assertTrue(all(a.count('= lbl_') == 1 for a in alts))


if __name__ == '__main__':
    unittest.main()
