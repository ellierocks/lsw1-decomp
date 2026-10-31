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

f32 NuASin(s32 ang)
{
    f32 x = (f32)(ang & 0xffff) * 9.58738019e-05f;
    f32 sign = 1.0f;
    f32 x2;
    if (x > 3.14159274f) {
        x = 6.28318548f - x;
    }
    if (x > 1.57079637f) {
        x = 3.14159274f - x;
        sign = -1.0f;
    }
    x2 = x * x;
    return (((((-2.60500002e-07f * x2 + 2.47609005e-05f) * x2 - 0.00138883968f) * x2 + 0.0416666418f) * x2 - 0.5f) * x2 + 1.0f) * sign;
}

f32 NuAngDiff(s32 ang)
{
    f32 x = (f32)(ang & 0xffff) * 9.58738019e-05f;
    f32 sign = 1.0f;
    f32 x2;
    if (x > 3.14159274f) {
        x -= 3.14159274f;
        sign = -1.0f;
    }
    if (x > 1.57079637f) {
        x = 3.14159274f - x;
    }
    x2 = x * x;
    return ((((((-2.39000002e-08f * x2 + 2.75260004e-06f) * x2 - 0.000198409005f) * x2 + 0.00833333191f) * x2 - 0.166666672f) * x2 + 1.0f) * x) * sign;
}

extern s16 fn_8000AEC4(s32 ang);

s16 NuACos(s32 ang)
{
    return 0x4000 - fn_8000AEC4(ang);
}

struct nusem_s {
    s32 pad[3];
};

extern struct nusem_s lbl_8020A140[];
extern void OSWaitSemaphore(struct nusem_s* sem);

void fn_8000A834(s32 id)
{
    if (id >= 0) {
        OSWaitSemaphore(&lbl_8020A140[id]);
    }
}

extern void OSSignalSemaphore(struct nusem_s* sem);

void fn_8000A86C(s32 id)
{
    if (id >= 0) {
        OSSignalSemaphore(&lbl_8020A140[id]);
    }
}

extern s32 fn_8000ABDC(s32 ang);

f32 fn_8000AFBC(s32 ang)
{
    return (f32)fn_8000ABDC(ang) * 9.58738019e-05f;
}
