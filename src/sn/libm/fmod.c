/* LSW1 libm: SN exports fdlibm's __ieee754_fmod as fmod; see docs/sn_runtime.md. */
#define __ieee754_fmod fmod
#include "math/e_fmod.c"
