#include "types.h"
#include "numath/numath.h"
#include "numath/nuvec.h"
f32 NuFdiv(f32 a, f32 b);
f32 NuMtxDet3(struct numtx_s* m);
f32 fn_8000ED40(f32 x);

void fn_8000CE8C(f32* dst, f32* a, f32* b, f32 t)
{
    dst[0] = a[0] * t + b[0] * (1.0f - t);
    dst[1] = a[1] * t + b[1] * (1.0f - t);
    dst[2] = a[2] * t + b[2] * (1.0f - t);
}

void fn_8000DC80(struct numtx_s* m, struct nuvec_s* p)
{
    m->_30 = p->x;
    m->_31 = p->y;
    m->_32 = p->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 = m->_21 = m->_23 = 0.0f;
    m->_00 = m->_11 = m->_22 = m->_33 = 1.0f;
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


f32 fn_8000CB50(struct nuvec_s* v)
{
    return NuFsqrt(NuVecMagSqr(v));
}


f32 fn_8000CB74(struct nuvec_s* dst, struct nuvec_s* src)
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
    return len;
}

void fn_8000CBF4(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* c)
{
    struct nuvec_s v1;
    struct nuvec_s v2;
    f32 len;
    f32 inv;
    NuVecSub(&v1, a, b);
    NuVecSub(&v2, a, c);
    NuVecCross(dst, &v2, &v1);
    len = NuFsqrt(NuVecMagSqr(dst));
    if (len > 0.0f) {
        inv = 1.0f / len;
    } else {
        inv = 0.0f;
    }
    dst->x *= inv;
    dst->y *= inv;
    dst->z *= inv;
}

f32 fn_8000CCA4(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* out)
{
    struct nuvec_s tmp;
    if (out != 0) {
        NuVecSub(out, a, b);
        return NuFsqrt(NuVecMagSqr(out));
    }
    NuVecSub(&tmp, a, b);
    return NuFsqrt(NuVecMagSqr(&tmp));
}

f32 fn_8000CD08(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* out)
{
    struct nuvec_s tmp;
    if (out != 0) {
        NuVecSub(out, a, b);
        return NuVecMagSqr(out);
    }
    NuVecSub(&tmp, a, b);
    return NuVecMagSqr(&tmp);
}

f32 fn_8000CD68(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* out)
{
    struct nuvec_s tmp;
    if (out != 0) {
        out->x = a->x - b->x;
        out->y = 0.0f;
        out->z = a->z - b->z;
        return NuFsqrt(NuVecMagSqr(out));
    }
    tmp.x = a->x - b->x;
    tmp.y = 0.0f;
    tmp.z = a->z - b->z;
    return NuFsqrt(NuVecMagSqr(&tmp));
}

f32 fn_8000CDFC(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* out)
{
    struct nuvec_s tmp;
    if (out != 0) {
        out->x = a->x - b->x;
        out->y = 0.0f;
        out->z = a->z - b->z;
        return NuVecMagSqr(out);
    }
    tmp.x = a->x - b->x;
    tmp.y = 0.0f;
    tmp.z = a->z - b->z;
    return NuVecMagSqr(&tmp);
}

void fn_8000DCE0(struct numtx_s* m, struct nuvec_s* s)
{
    m->_00 = s->x;
    m->_11 = s->y;
    m->_22 = s->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 = m->_21 = m->_23 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void fn_8000DD40(struct numtx_s* m, s32 a)
{
    f32 c;
    f32 s;
    c = NuASin(a);
    m->_22 = c;
    m->_11 = c;
    s = NuAngDiff(a);
    m->_12 = s;
    m->_21 = -s;
    m->_00 = 1.0f;
    m->_01 = m->_02 = m->_03 = m->_23 = m->_10 = m->_20 = m->_13 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void fn_8000DDD0(struct numtx_s* m, s32 a)
{
    f32 c;
    f32 s;
    c = NuASin(a);
    m->_22 = c;
    m->_00 = c;
    s = NuAngDiff(a);
    m->_20 = s;
    m->_02 = -s;
    m->_11 = 1.0f;
    m->_01 = m->_10 = m->_03 = m->_23 = m->_12 = m->_21 = m->_13 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void NuMtxTranslateNeg(struct numtx_s* m, s32 a)
{
    f32 c;
    f32 s;
    c = NuASin(a);
    m->_11 = c;
    m->_00 = c;
    s = NuAngDiff(a);
    m->_01 = s;
    m->_10 = -s;
    m->_22 = 1.0f;
    m->_02 = m->_12 = m->_03 = m->_23 = m->_20 = m->_21 = m->_13 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0f;
}
void fn_8000D010(struct numtx_s* m, struct nuivec_s* r)
{
    f32 cx = NuASin(r->x);
    f32 sx = NuAngDiff(r->x);
    f32 cy = NuASin(r->y);
    f32 sy = NuAngDiff(r->y);
    f32 cz = NuASin(r->z);
    f32 sz = NuAngDiff(r->z);
    m->_00 = cy * cz;
    m->_01 = cy * sz;
    m->_02 = -sy;
    m->_03 = 0.0f;
    m->_10 = sx * sy * cz - cx * sz;
    m->_11 = sx * sy * sz + cx * cz;
    m->_12 = sx * cy;
    m->_13 = 0.0f;
    m->_20 = cx * sy * cz + sx * sz;
    m->_21 = cx * sy * sz - sx * cz;
    m->_22 = cx * cy;
    m->_23 = 0.0f;
    m->_30 = 0.0f;
    m->_31 = 0.0f;
    m->_32 = 0.0f;
    m->_33 = 1.0f;
}
void fn_8000D874(struct numtx_s* m, struct nuvec_s* v)
{
    f32 x, y, z, len;
    m->_10 = x = v->x;
    m->_11 = y = v->y;
    m->_12 = z = v->z;
    m->_02 = x * m->_21 - y * m->_20;
    m->_01 = z * m->_20 - x * m->_22;
    m->_00 = y * m->_22 - z * m->_21;
    len = NuFsqrt(m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02);
    len = NuFdiv(1.0f, len);
    m->_00 *= len;
    m->_01 *= len;
    m->_02 *= len;
    m->_20 = m->_01 * m->_12 - m->_02 * m->_11;
    m->_21 = m->_02 * m->_10 - m->_00 * m->_12;
    m->_22 = m->_00 * m->_11 - m->_01 * m->_10;
}
void fn_8000D364(struct numtx_s* d, struct numtx_s* s)
{
    struct numtx_s t;
    f32 c0 = s->_11 * s->_22 - s->_12 * s->_21;
    f32 c1 = s->_10 * s->_22 - s->_12 * s->_20;
    f32 c2 = s->_10 * s->_21 - s->_11 * s->_20;
    f32 inv = 1.0f / (s->_00 * c0 - s->_01 * c1 + s->_02 * c2);
    t._33 = 1.0f;
    t._32 = t._31 = t._30 = t._23 = t._13 = t._03 = 0.0f;
    t._00 = c0 * inv;
    t._10 = c1 * -inv;
    t._20 = c2 * inv;
    t._01 = (s->_01 * s->_22 - s->_02 * s->_21) * -inv;
    t._11 = (s->_00 * s->_22 - s->_02 * s->_20) * inv;
    t._21 = (s->_00 * s->_21 - s->_01 * s->_20) * -inv;
    t._02 = (s->_01 * s->_12 - s->_02 * s->_11) * inv;
    t._12 = (s->_00 * s->_12 - s->_02 * s->_10) * -inv;
    t._22 = (s->_00 * s->_11 - s->_01 * s->_10) * inv;
    *d = t;
}
s32 NuMtlGet3dBuffer(struct nuvec_s* p1, struct nuvec_s* d1, struct nuvec_s* p2, struct nuvec_s* d2, f32* t1, f32* t2)
{
    struct numtx_s m;
    struct nuvec_s c;
    f32 len;
    f32 inv;
    NuVecCross(&c, d1, d2);
    len = NuFsqrt(NuVecMagSqr(&c));
    if (len == 0.0f) {
        inv = 0.0f;
    } else {
        inv = 1.0f / len;
    }
    inv *= inv;
    m._00 = p2->x - p1->x;
    m._01 = p2->y - p1->y;
    m._02 = p2->z - p1->z;
    m._10 = d2->x;
    m._11 = d2->y;
    m._12 = d2->z;
    m._20 = c.x;
    m._21 = c.y;
    m._22 = c.z;
    *t1 = NuMtxDet3(&m) * inv;
    m._10 = d1->x;
    m._11 = d1->y;
    m._12 = d1->z;
    *t2 = NuMtxDet3(&m) * inv;
    return inv != 0.0f;
}
void fn_8000D134(struct numtx_s* d, struct numtx_s* a, struct numtx_s* b)
{
    struct numtx_s t;
    t._23 = t._13 = t._03 = 0.0f;
    t._00 = a->_00 * b->_00 + a->_01 * b->_10 + a->_02 * b->_20;
    t._01 = a->_00 * b->_01 + a->_01 * b->_11 + a->_02 * b->_21;
    t._02 = a->_00 * b->_02 + a->_01 * b->_12 + a->_02 * b->_22;
    t._10 = a->_10 * b->_00 + a->_11 * b->_10 + a->_12 * b->_20;
    t._11 = a->_10 * b->_01 + a->_11 * b->_11 + a->_12 * b->_21;
    t._12 = a->_10 * b->_02 + a->_11 * b->_12 + a->_12 * b->_22;
    t._20 = a->_20 * b->_00 + a->_21 * b->_10 + a->_22 * b->_20;
    t._21 = a->_20 * b->_01 + a->_21 * b->_11 + a->_22 * b->_21;
    t._22 = a->_20 * b->_02 + a->_21 * b->_12 + a->_22 * b->_22;
    t._30 = a->_30 * b->_00 + a->_31 * b->_10 + a->_32 * b->_20 + b->_30;
    t._31 = a->_30 * b->_01 + a->_31 * b->_11 + a->_32 * b->_21 + b->_31;
    t._32 = a->_30 * b->_02 + a->_31 * b->_12 + a->_32 * b->_22 + b->_32;
    t._33 = 1.0f;
    *d = t;
}
void fn_8000D95C(struct numtx_s* mt, struct nuvec_s* v)
{
    struct nuvec_s* m = (struct nuvec_s*)&mt->_00;
    struct nuvec_s* r1 = (struct nuvec_s*)&mt->_10;
    struct nuvec_s* r2 = (struct nuvec_s*)&mt->_20;
    f32 k;
    f32 s0;
    f32 s1;
    k = (mt->_20 * mt->_20 + mt->_21 * mt->_21 + mt->_22 * mt->_22) / (v->x * v->x + v->y * v->y + v->z * v->z);
    s0 = mt->_00 * mt->_00 + mt->_01 * mt->_01 + mt->_02 * mt->_02;
    s1 = mt->_10 * mt->_10 + mt->_11 * mt->_11 + mt->_12 * mt->_12;
    k = NuFsqrt(k);
    mt->_20 = k * v->x;
    mt->_21 = k * v->y;
    mt->_22 = k * v->z;
    if (fn_8000ED40(NuVecDot(r1, r2)) > 0.8660254f) {
        NuVecCross(r1, r2, m);
        k = NuFsqrt(s1 / (mt->_10 * mt->_10 + mt->_11 * mt->_11 + mt->_12 * mt->_12));
        mt->_10 *= k;
        mt->_11 *= k;
        mt->_12 *= k;
        NuVecCross(m, r1, r2);
        k = NuFsqrt(s0 / (mt->_00 * mt->_00 + mt->_01 * mt->_01 + mt->_02 * mt->_02));
        mt->_00 *= k;
        mt->_01 *= k;
        mt->_02 *= k;
    } else {
        NuVecCross(m, r1, r2);
        k = NuFsqrt(s0 / (mt->_00 * mt->_00 + mt->_01 * mt->_01 + mt->_02 * mt->_02));
        mt->_00 *= k;
        mt->_01 *= k;
        mt->_02 *= k;
        NuVecCross(r1, r2, m);
        k = NuFsqrt(s1 / (mt->_10 * mt->_10 + mt->_11 * mt->_11 + mt->_12 * mt->_12));
        mt->_10 *= k;
        mt->_11 *= k;
        mt->_12 *= k;
    }
}
