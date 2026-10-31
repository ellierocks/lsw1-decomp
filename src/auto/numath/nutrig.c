#include "numath/numath.h"

s32 NuAngAdd(s32 a, s32 b)
{
    s32 sum = (a + b) & 0xffff;
    if (sum > 0x7fff) {
        sum -= 0x10000;
    }
    return sum;
}

s32 NuAngSub(s32 a, s32 b)
{
    s32 diff = (a - b) & 0xffff;
    if (diff > 0x7fff) {
        diff -= 0x10000;
    }
    return diff;
}

s32 NuPower2(s32 n)
{
    s32 p;
    if (n > 127) {
        p = 128;
    } else {
        p = 1;
    }
    while (p < n) {
        p <<= 1;
    }
    return p;
}

s32 NuAng2AltSol(s32 a, s32 b)
{
    s32 diff = (a - b) & 0xffff;
    if (diff > 0x7fff) {
        diff -= 0x10000;
    }
    return diff;
}

f32 fn_8000B234(f32 x)
{
    s32 bits = *(s32*)&x;
    return bits < 0 ? -1.0f : 1.0f;
}

f32 fn_8000B2C4(f32 a, f32 b)
{
    return a - (s32)(a / b) * b;
}
