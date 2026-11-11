#include "types.h"
#include "numath/numath.h"

s32 fn_8000BE70(f32* v, f32* min, f32* max);
void fn_8000BF0C(f32* dst, f32* a, f32* b, f32* t);
void fn_8000BF70(f32* dst, f32* a, f32* b, f32* t);
void fn_8000BFD4(f32* dst, f32* a, f32* b, f32* t);

static inline void InvMtxTransform(struct nuvec_s* dest, struct nuvec_s* a, struct numtx_s* b)
{
    f32 dy = a->y - b->_31;
    f32 dx = a->x - b->_30;
    f32 dz = a->z - b->_32;
    dest->x = b->_00 * dx + b->_01 * dy + b->_02 * dz;
    dest->y = b->_10 * dx + b->_11 * dy + b->_12 * dz;
    dest->z = b->_20 * dx + b->_21 * dy + b->_22 * dz;
}

static inline void VecNorm(struct nuvec_s* dst, struct nuvec_s* src)
{
    f32 len = NuFsqrt(NuVecMagSqr(src));
    f32 inv;
    if (len > 0.0f) {
        inv = 1.0f / len;
    } else {
        inv = 0.0f;
    }
    dst->x = src->x * inv;
    dst->y = src->y * inv;
    dst->z = src->z * inv;
}

s32 fn_8000C1B8(struct nuvec_s* min, struct nuvec_s* max, struct numtx_s* m, struct nuvec_s* p0,
                struct nuvec_s* p1, struct nuvec_s* out, f32 r)
{
    struct nuvec_s a, b, dir, hit;
    struct nuvec_s lo;
    struct nuvec_s hi;
    s32 c0, c1;
    lo.x = min->x - r; lo.y = min->y - r; lo.z = min->z - r;
    hi.x = max->x + r; hi.y = max->y + r; hi.z = max->z + r;
    InvMtxTransform(&a, p0, m);
    c0 = fn_8000BE70(&a.x, &hi.x, &lo.x);
    if (c0 == 0 && out == 0)
        return 1;
    InvMtxTransform(&b, p1, m);
    c1 = fn_8000BE70(&b.x, &hi.x, &lo.x);
    if (c1 == 0 && out == 0)
        return 1;
    if (c0 == 0 && c1 == 0) {
        if (out != 0) {
            out->x = 0.0f;
            out->y = 0.0f;
            out->z = 0.0f;
        }
        return 1;
    }
    if (c0 & c1)
        return 0;
    NuVecSub(&dir, &b, &a);
    VecNorm(&dir, &dir);
    if (c0 & 1) {
        fn_8000BF0C(&hit.x, &a.x, &dir.x, &hi.x);
        if (hit.y < hi.y && hit.y > lo.y && hit.z < hi.z && hit.z > lo.z) {
            if (out != 0)
                *out = hit;
            return 1;
        }
    } else if (c0 & 8) {
        fn_8000BF0C(&hit.x, &a.x, &dir.x, &lo.x);
        if (hit.y < hi.y && hit.y > lo.y && hit.z < hi.z && hit.z > lo.z) {
            if (out != 0)
                *out = hit;
            return 1;
        }
    }
    if (c0 & 2) {
        fn_8000BF70(&hit.x, &a.x, &dir.x, &hi.y);
        if (hit.x < hi.x && hit.x > lo.x && hit.z < hi.z && hit.z > lo.z) {
            if (out != 0)
                *out = hit;
            return 1;
        }
    } else if (c0 & 16) {
        fn_8000BF70(&hit.x, &a.x, &dir.x, &lo.y);
        if (hit.x < hi.x && hit.x > lo.x && hit.z < hi.z && hit.z > lo.z) {
            if (out != 0)
                *out = hit;
            return 1;
        }
    }
    if (c0 & 4) {
        fn_8000BFD4(&hit.x, &a.x, &dir.x, &hi.z);
        if (hit.x < hi.x && hit.x > lo.x && hit.y < hi.y && hit.y > lo.y) {
            if (out != 0)
                *out = hit;
            return 1;
        }
    } else if (c0 & 32) {
        fn_8000BFD4(&hit.x, &a.x, &dir.x, &lo.z);
        if (hit.x < hi.x && hit.x > lo.x && hit.y < hi.y && hit.y > lo.y) {
            if (out != 0)
                *out = hit;
            return 1;
        }
    }
    return 0;
}
