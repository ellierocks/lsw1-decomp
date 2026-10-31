#include "types.h"

void fn_8000CE8C(f32* dst, f32* a, f32* b, f32 t)
{
    dst[0] = a[0] * t + b[0] * (1.0f - t);
    dst[1] = a[1] * t + b[1] * (1.0f - t);
    dst[2] = a[2] * t + b[2] * (1.0f - t);
}

void fn_8000DC80(f32* m, f32* pos)
{
    m[0x30 / 4] = pos[0];
    m[0x34 / 4] = pos[1];
    m[0x0 / 4] = 1.0f;
    m[0x38 / 4] = pos[2];
    m[0x4 / 4] = 0.0f;
    m[0x2c / 4] = 0.0f;
    m[0x24 / 4] = 0.0f;
    m[0x20 / 4] = 0.0f;
    m[0x1c / 4] = 0.0f;
    m[0x18 / 4] = 0.0f;
    m[0x10 / 4] = 0.0f;
    m[0xc / 4] = 0.0f;
    m[0x8 / 4] = 0.0f;
    m[0x3c / 4] = 1.0f;
    m[0x28 / 4] = 1.0f;
    m[0x14 / 4] = 1.0f;
}

struct nugraph3_blk_s {
    u8 b[0x40];
};

extern struct nugraph3_blk_s lbl_801B0F48;
extern struct nugraph3_blk_s lbl_801B0F08;

void fn_8000DBA0(struct nugraph3_blk_s* dst)
{
    *dst = lbl_801B0F48;
}

void fn_8000DC10(struct nugraph3_blk_s* dst)
{
    *dst = lbl_801B0F08;
}

extern f32 NuVecMagSqr(f32* v);
extern f32 NuFsqrt(f32 x);

f32 fn_8000CB50(f32* v)
{
    return NuFsqrt(NuVecMagSqr(v));
}
