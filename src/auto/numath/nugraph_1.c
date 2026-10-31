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
