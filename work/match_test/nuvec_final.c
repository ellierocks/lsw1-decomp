#include "numath.h"

extern f32 NuASin(s32 angle);
extern f32 NuAngDiff(s32 angle);

void NuVecMtxRotate(struct nuvec_s* dest, struct nuvec_s* a, struct numtx_s* b)
{
    f32 ax = a->x, ay = a->y, az = a->z;
    dest->x = ax * b->_00 + ay * b->_10 + az * b->_20;
    dest->y = ax * b->_01 + ay * b->_11 + az * b->_21;
    dest->z = ax * b->_02 + ay * b->_12 + az * b->_22;
}

void NuVecInvMtxTransform(struct nuvec_s* dest, struct nuvec_s* a, struct numtx_s* b)
{
    f32 dy = a->y - b->_31;
    f32 dx = a->x - b->_30;
    f32 dz = a->z - b->_32;
    dest->x = b->_02 * dz + b->_00 * dx + b->_01 * dy;
    dest->y = b->_12 * dz + b->_10 * dx + b->_11 * dy;
    dest->z = b->_22 * dz + b->_20 * dx + b->_21 * dy;
}

void NuVecInvMtxRotate(struct nuvec_s* dest, struct nuvec_s* a, struct numtx_s* b)
{
    f32 ax = a->x, ay = a->y, az = a->z;
    dest->x = ax * b->_00 + ay * b->_01 + az * b->_02;
    dest->y = ax * b->_10 + ay * b->_11 + az * b->_12;
    dest->z = ax * b->_20 + ay * b->_21 + az * b->_22;
}

void NuVecRotateX(struct nuvec_s* dest, struct nuvec_s* v, s32 angle)
{
    f32 c = NuASin(angle);
    f32 s = NuAngDiff(angle);
    dest->x = v->x;
    dest->y = v->y * c - v->z * s;
    dest->z = v->y * s + v->z * c;
}

void NuVecRotateY(struct nuvec_s* dest, struct nuvec_s* v, s32 angle)
{
    f32 c = NuASin(angle);
    f32 s = NuAngDiff(angle);
    dest->x = v->x * c + v->z * s;
    dest->y = v->y;
    dest->z = v->z * c - v->x * s;
}

void NuVecRotateZ(struct nuvec_s* dest, struct nuvec_s* v, s32 angle)
{
    f32 c = NuASin(angle);
    f32 s = NuAngDiff(angle);
    dest->x = v->x * c - v->y * s;
    dest->y = v->x * s + v->y * c;
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
    dst->x = (a->x > b->x) ? a->x : b->x;
    dst->y = (a->y > b->y) ? a->y : b->y;
    dst->z = (a->z > b->z) ? a->z : b->z;
}

void NuVecMin(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b)
{
    dst->x = (a->x < b->x) ? a->x : b->x;
    dst->y = (a->y < b->y) ? a->y : b->y;
    dst->z = (a->z < b->z) ? a->z : b->z;
}
