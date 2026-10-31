#include "types.h"

extern s32 lbl_80407B40;

f32 NuRandIntSeeded(s32* seed)
{
    union {
        s32 i;
        f32 f;
    } u;
    s32 v;
    v = *seed * 1664525 + 1013904223;
    *seed = v;
    u.i = (v & 0x7fffff) | 0x3f800000;
    return u.f - 1.0f;
}

f32 fn_8000C038(void)
{
    union {
        s32 i;
        f32 f;
    } u;
    s32 v;
    v = lbl_80407B40 * 1664525 + 1013904223;
    lbl_80407B40 = v;
    u.i = (v & 0x7fffff) | 0x3f800000;
    return u.f - 1.0f;
}

void fn_8000BF0C(f32* dst, f32* a, f32* b, f32* t)
{
    f32 d;
    if (t != 0) {
        dst[0] = *t;
    } else {
        dst[0] = 0.0f;
    }
    d = dst[0] - a[0];
    dst[1] = a[1] + d * b[1] / b[0];
    dst[2] = a[2] + d * b[2] / b[0];
}

void fn_8000BF70(f32* dst, f32* a, f32* b, f32* t)
{
    f32 d;
    if (t != 0) {
        dst[1] = *t;
    } else {
        dst[1] = 0.0f;
    }
    d = dst[1] - a[1];
    dst[0] = a[0] + d * b[0] / b[1];
    dst[2] = a[2] + d * b[2] / b[1];
}

void fn_8000BFD4(f32* dst, f32* a, f32* b, f32* t)
{
    f32 d;
    if (t != 0) {
        dst[2] = *t;
    } else {
        dst[2] = 0.0f;
    }
    d = dst[2] - a[2];
    dst[0] = a[0] + d * b[0] / b[2];
    dst[1] = a[1] + d * b[1] / b[2];
}
