"""Exercise reconstructed SN va_arg overflow and register-index rules on the host."""

import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Construct the list directly: va_start is a target compiler builtin. Native
# long may be 8 bytes, so seed GP slots through the header's register-save type.
FIXTURE = r'''
#define _STDARG_H
#include "va-ppc.h"
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
int main(void)
{
    __va_list ap;
    __va_regsave_t saved;
    char stack[64] __attribute__((aligned(16)));
    double d = 2.5;
    long long q = 0x123456789abcdefLL;
    ap->reg_save_area = (char *)&saved;
    ap->overflow_arg_area = stack + 4;
    saved.__fp_save[7] = 1.25;
    __builtin_memcpy(stack + 4, &d, 8);
    __builtin_memcpy(stack + 12, &d, 8);
#ifndef ONLY_INTEGER
    ap->fpr = 7;
    CHECK(va_arg(ap, double) == 1.25);
    CHECK(ap->fpr == 8 && ap->overflow_arg_area == stack + 4);
    CHECK(va_arg(ap, double) == 2.5);
    CHECK(ap->fpr == 8 && ap->overflow_arg_area == stack + 12);
    CHECK(va_arg(ap, double) == 2.5);
    CHECK(ap->fpr == 8 && ap->overflow_arg_area == stack + 20);

    ap->fpr = -1;
    CHECK(ap->fpr < 0);
#endif

    ap->overflow_arg_area = stack + 20;
    __builtin_memcpy(&saved.__gp_save[2], &q, 8);
    ap->gpr = 1;
    CHECK(va_arg(ap, long long) == q);
    CHECK(ap->gpr == 4 && ap->overflow_arg_area == stack + 20);
    __builtin_memcpy(&saved.__gp_save[6], &q, 8);
    ap->gpr = 6;
    CHECK(va_arg(ap, long long) == q);
    CHECK(ap->gpr == 8 && ap->overflow_arg_area == stack + 20);

    __builtin_memcpy(stack + 4, &q, 8);
    __builtin_memcpy(stack + 12, &q, 8);
    ap->overflow_arg_area = stack + 4;
    ap->gpr = 7;
    CHECK(va_arg(ap, long long) == q);
    CHECK(ap->gpr == 8 && ap->overflow_arg_area == stack + 12);
    CHECK(va_arg(ap, long long) == q);
    CHECK(ap->gpr == 8 && ap->overflow_arg_area == stack + 20);
    return 0;
}
'''


@unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
class SnVarargs(unittest.TestCase):
    def check_arguments(self, defines=()):
        with tempfile.TemporaryDirectory() as tmp:
            source = Path(tmp) / "check.c"
            binary = Path(tmp) / "check"
            source.write_text(FIXTURE)
            subprocess.run([
                "cc", "-std=gnu99", "-O2", "-fsigned-char",
                "-fno-strict-aliasing", *defines, "-I", str(ROOT / "src/sn/include"),
                str(source), "-o", str(binary),
            ], check=True, capture_output=True, text=True)
            result = subprocess.run([str(binary)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, f"va_arg assertion at C line {result.returncode}")

    def test_float_register_exhaustion_and_unaligned_overflow(self):
        self.check_arguments()

    def test_integer_register_alignment_and_overflow(self):
        self.check_arguments(("-DONLY_INTEGER",))
