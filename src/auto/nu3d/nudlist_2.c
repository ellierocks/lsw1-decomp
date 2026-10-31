#include "numath/numath.h"

struct fn_800127D8_inner {
    u8 pad0[0x8];
    s32 f8;
    u8 padC[0x8];
    s32 f14;
    u8 pad18[0x6];
    s16 f1e;
    s32 f20;
    s16 f24;
    s16 f26;
    s16 f28;
};

struct fn_800127D8_outer {
    u8 pad0[0x8];
    struct fn_800127D8_inner* inner;
    u8 padC[0x4];
    s32 f10;
    s32 f14;
};

f32 fn_8000ED40(f32 x)
{
    if (x < 0.0f) {
        x = -x;
    }
    return x;
}

f32 NuFdiv(f32 a, f32 b)
{
    if (b == 0.0f || a == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

void fn_800127D8(struct fn_800127D8_outer* o)
{
    struct fn_800127D8_inner* p = o->inner;
    s32 m = -1;
    p->f8 = 0;
    p->f14 = m;
    p->f20 = m;
    p->f24 = m;
    p->f26 = m;
    p->f28 = m;
    p->f1e = m;
    o->f14 = o->f10;
}
