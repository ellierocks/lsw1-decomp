typedef float f32;
typedef signed int s32;

struct nuvec_s { f32 x, y, z; };
struct Mtx {
    f32 m11, m12, m13, m14;
    f32 m21, m22, m23, m24;
    f32 m31, m32, m33, m34;
    f32 m41, m42, m43, m44;
};

void NuVecMtxRotate(struct nuvec_s* dest, struct nuvec_s* a, struct Mtx* b)
{
    dest->x = a->x * b->m11 + a->y * b->m21 + a->z * b->m31;
    dest->y = a->x * b->m12 + a->y * b->m22 + a->z * b->m32;
    dest->z = a->x * b->m13 + a->y * b->m23 + a->z * b->m33;
}

void NuVecInvMtxTransform(struct nuvec_s* dest, struct nuvec_s* a, struct Mtx* b)
{
    f32 dy = a->y - b->m42;
    f32 dx = a->x - b->m41;
    f32 dz = a->z - b->m43;
    dest->x = b->m13 * dz + b->m11 * dx + b->m12 * dy;
    dest->y = b->m23 * dz + b->m21 * dx + b->m22 * dy;
    dest->z = b->m33 * dz + b->m31 * dx + b->m32 * dy;
}

void NuVecInvMtxRotate(struct nuvec_s* dest, struct nuvec_s* a, struct Mtx* b)
{
    dest->x = a->x * b->m11 + a->y * b->m12 + a->z * b->m13;
    dest->y = a->x * b->m21 + a->y * b->m22 + a->z * b->m23;
    dest->z = a->x * b->m31 + a->y * b->m32 + a->z * b->m33;
}

void NuVecScale(f32 scale, struct nuvec_s* dest, struct nuvec_s* v)
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
