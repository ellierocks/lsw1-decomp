#include "numath/numath.h"
#include "numath/nuvec_ps.h"

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

s32 fn_8000D010();

void fn_8000EB24(void) {
    fn_8000D010();
}

s32 fn_8000EFE0();

void fn_8000EBC8(void) {
    fn_8000EFE0();
}

s32 fn_8000F65C(s32, s32, s32);
extern s32 lbl_80407B84;

void fn_8000F840(void *arg0) {
    s32 *temp_r4;
    s32 var_r0;

    temp_r4 = (*(s32 **)((s8 *)(arg0) + (4)));
    if ((temp_r4 != NULL) && ((s32) lbl_80407B84 != 0)) {
        fn_8000F65C((*(s32 *)((s8 *)(arg0) + (0x20))), *temp_r4, (*(s32 *)((s8 *)(arg0) + (0))));
        var_r0 = (*(s32 *)((s8 *)(arg0) + (0x28))) | 0x20000000;
    } else {
        var_r0 = (*(s32 *)((s8 *)(arg0) + (0x28))) & 0xDFFFFFFF;
    }
    (*(s32 *)((s8 *)(arg0) + (0x28))) = var_r0;
}

void NuVec4MtxTransformVU0x4(struct numtx_s *arg0, struct nuvec_s *arg1) {
    NuMtxScale(arg0, arg1);
}

void fn_8000EA94(struct numtx_s *arg0, struct nuvec_s *arg1) {
    NuMtxPreScale(arg0, arg1);
}

void fn_8000EB44(struct numtx_s *arg0, struct numtx_s *arg1, struct numtx_s *arg2) {
    NuMtxMul(arg0, arg1, arg2);
}

void NuVecAdd(struct nuvec_s *arg0, struct nuvec_s *arg1, struct nuvec_s *arg2) {
    NuVecAddPS(arg0, arg1, arg2);
}

void NuVecSub(struct nuvec_s *arg0, struct nuvec_s *arg1, struct nuvec_s *arg2) {
    NuVecSubPS(arg0, arg1, arg2);
}

void NuVecMtxTransform(struct nuvec_s *dst, struct nuvec_s *src, struct numtx_s *mtx) {
    NuVecMtxTransformPS(dst, src, mtx);
}

extern f64 lbl_80154144();
extern f32 lbl_8018F138[];
extern f32 lbl_8018F13C[];
extern f32 lbl_8018F140[];

f32 NuFsqrt(f32 x) {
    f32 y;
    f32 h;

    if (x > lbl_8018F138[0]) {
        asm("frsqrte %0, %1" : "=f"(y) : "f"(x));
        h = x * lbl_8018F13C[0];
        y = y * (lbl_8018F140[0] - h * y * y);
        y = y * (lbl_8018F140[0] - h * y * y);
        y = y * (lbl_8018F140[0] - h * y * y);
        y = y * (lbl_8018F140[0] - h * y * y);
        return y * x;
    }
    if (x == lbl_8018F138[0]) {
        return lbl_8018F138[0];
    }
    return lbl_80154144();
}
