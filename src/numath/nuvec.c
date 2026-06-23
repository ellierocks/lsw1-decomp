#include "numath/nuvec.h"

void NuVecMtxTransformH(struct nuvec_s* dest, struct nuvec_s* a, struct numtx_s* b)
{
    f32 ax = a->x, ay = a->y, az = a->z;
    f32 w = 1.0f / (ax * b->_03 + ay * b->_13 + az * b->_23 + b->_33);
    f32 y = (ax * b->_01 + ay * b->_11 + az * b->_21 + b->_31) * w;
    f32 z = (ax * b->_02 + ay * b->_12 + az * b->_22 + b->_32) * w;
    f32 x = (ax * b->_00 + ay * b->_10 + az * b->_20 + b->_30) * w;
    dest->x = x;
    dest->y = y;
    dest->z = z;
}

void NuVecMtxRotate(struct nuvec_s* dest, struct nuvec_s* a, struct numtx_s* b)
{
    f32 ax = a->x, ay = a->y, az = a->z;
    f32 y = ax * b->_01 + ay * b->_11 + az * b->_21;
    f32 z = ax * b->_02 + ay * b->_12 + az * b->_22;
    f32 x = ax * b->_00 + ay * b->_10 + az * b->_20;
    dest->x = x;
    dest->y = y;
    dest->z = z;
}

void NuVecInvMtxTransform(struct nuvec_s* dest, struct nuvec_s* a, struct numtx_s* b)
{
    f32 dy = a->y - b->_31;
    f32 dx = a->x - b->_30;
    f32 dz = a->z - b->_32;
    dest->x = b->_01 * dy + b->_00 * dx + b->_02 * dz;
    dest->y = b->_11 * dy + b->_10 * dx + b->_12 * dz;
    dest->z = b->_21 * dy + b->_20 * dx + b->_22 * dz;
}

void NuVecInvMtxRotate(struct nuvec_s* dest, struct nuvec_s* a, struct numtx_s* b)
{
    f32 ax = a->x, ay = a->y, az = a->z;
    f32 y = ax * b->_10 + ay * b->_11 + az * b->_12;
    f32 z = ax * b->_20 + ay * b->_21 + az * b->_22;
    f32 x = ax * b->_00 + ay * b->_01 + az * b->_02;
    dest->x = x;
    dest->y = y;
    dest->z = z;
}

void NuVecRotateX(struct nuvec_s* dest, struct nuvec_s* v, s32 angle)
{
    f32 c = NuASin(angle);
    f32 s = NuAngDiff(angle);
    f32 y = v->y;
    dest->x = v->x;
    dest->y = y * c - v->z * s;
    dest->z = y * s + v->z * c;
}

void NuVecRotateY(struct nuvec_s* dest, struct nuvec_s* v, s32 angle)
{
    f32 c = NuASin(angle);
    f32 s = NuAngDiff(angle);
    f32 x = v->x;
    dest->x = x * c + v->z * s;
    dest->y = v->y;
    dest->z = v->z * c - x * s;
}

void NuVecRotateZ(struct nuvec_s* dest, struct nuvec_s* v, s32 angle)
{
    f32 c = NuASin(angle);
    f32 s = NuAngDiff(angle);
    f32 x = v->x;
    dest->x = x * c - v->y * s;
    dest->y = x * s + v->y * c;
    dest->z = v->z;
}

void NuVecRotateAxis(struct nuvec_s* dest, s32 angle, f32 scale)
{
    f32 c = NuASin(angle);
    f32 s = NuAngDiff(angle);
    dest->x = scale * s;
    dest->y = 0.0f;
    dest->z = scale * c;
}

void NuVecScale(struct nuvec_s* dest, struct nuvec_s* v, f32 scale)
{
    dest->x = v->x * scale;
    dest->y = v->y * scale;
    dest->z = v->z * scale;
}

void NuVecScaleAccum(struct nuvec_s* dst, struct nuvec_s* accum, struct nuvec_s* src, f32 scale)
{
    dst->x = src->x * scale + accum->x;
    dst->y = src->y * scale + accum->y;
    dst->z = src->z * scale + accum->z;
}

f32 NuVecDot(struct nuvec_s* a, struct nuvec_s* b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void NuVecMax(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b)
{
    if (a->x > b->x) dst->x = a->x; else dst->x = b->x;
    if (a->y > b->y) dst->y = a->y; else dst->y = b->y;
    if (a->z > b->z) dst->z = a->z; else dst->z = b->z;
}

void NuVecMin(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b)
{
    if (a->x < b->x) dst->x = a->x; else dst->x = b->x;
    if (a->y < b->y) dst->y = a->y; else dst->y = b->y;
    if (a->z < b->z) dst->z = a->z; else dst->z = b->z;
}
