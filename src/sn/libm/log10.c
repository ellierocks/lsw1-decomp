/* LSW1 libm: SN exports fdlibm's __ieee754_log10 as log10, and its call to
   __ieee754_log as log; see docs/sn_runtime.md. */
#define __ieee754_log10 log10
#define __ieee754_log log
#include "math/e_log10.c"
