#include "types.h"
#include "numath/numath.h"

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

f32 fn_8000CB74(f32* dst, f32* src);
f32 fn_8000CB50(struct nuvec_s* v);
f32 fn_8000CD08(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* out);

f32 NuPlnLine3(f32* pln, f32* p, f32* dir, f32* out)
{
    f32 d;
    f32 den;
    f32 t;
    fn_8000CB74(dir, dir);
    d = -pln[3];
    den = dir[0] * pln[0] + dir[1] * pln[1] + dir[2] * pln[2];
    t = ((pln[0] * d - p[0]) * pln[0] + (pln[1] * d - p[1]) * pln[1] + (pln[2] * d - p[2]) * pln[2]) / den;
    out[0] = p[0] + t * dir[0];
    out[1] = p[1] + t * dir[1];
    out[2] = p[2] + t * dir[2];
    return t;
}

f32 fn_8000BDD0(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* c)
{
    struct nuvec_s dir;
    struct nuvec_s w;
    struct nuvec_s pt;
    struct nuvec_s tmp;
    NuVecSub(&dir, b, a);
    NuVecScale(&dir, &dir, 1.0f / fn_8000CB50(&dir));
    NuVecSub(&w, c, a);
    NuVecScale(&pt, &dir, NuVecDot(&dir, &w));
    NuVecAdd(&pt, &pt, a);
    return fn_8000CD08(&pt, c, &tmp);
}

s32 fn_8000BE70(f32* v, f32* min, f32* max)
{
    s32 code = 0;
    if (v[0] >= min[0]) {
        code = 1;
    } else if (v[0] <= max[0]) {
        code = 8;
    }
    if (v[1] >= min[1]) {
        code |= 2;
    } else if (v[1] <= max[1]) {
        code |= 0x10;
    }
    if (v[2] >= min[2]) {
        code |= 4;
    } else if (v[2] <= max[2]) {
        code |= 0x20;
    }
    return code;
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
