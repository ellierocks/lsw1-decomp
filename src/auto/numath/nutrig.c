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

void NuTexGetDDS(void)
{
}

void NuTexSetTextureDirect(void)
{
}

void fn_8000AFB8(void)
{
}

void fn_8000BCB0(f32* dst, f32* a, f32* b, f32 t)
{
    dst[0] = (1.0f - t) * a[0] + t * b[0];
    dst[1] = (1.0f - t) * a[1] + t * b[1];
    dst[2] = (1.0f - t) * a[2] + t * b[2];
    dst[3] = (1.0f - t) * a[3] + t * b[3];
}

f32 fn_8000B264(f32 x)
{
    f32 f = (f32)(s32)x;
    if (f > x) {
        return f - 1.0f;
    }
    return f;
}
