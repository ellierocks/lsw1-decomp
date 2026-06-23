#include "numath/numath.h"

void NuVec4MtxTransformVU0(struct nuvec4_s* dest, struct nuvec4_s* a, struct numtx_s* b)
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;

    x = a->x * b->_00 + a->y * b->_10 + a->z * b->_20 + a->w * b->_30;
    y = a->x * b->_01 + a->y * b->_11 + a->z * b->_21 + a->w * b->_31;
    z = a->x * b->_02 + a->y * b->_12 + a->z * b->_22 + a->w * b->_32;
    w = a->x * b->_03 + a->y * b->_13 + a->z * b->_23 + a->w * b->_33;

    dest->x = x;
    dest->y = y;
    dest->z = z;
    dest->w = w;
}

f32 NuVecDist(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* dist)
{
    struct nuvec_s tmp;
    if (dist == 0)
    {
        NuVecSub(&tmp, a, b);
        dist = &tmp;
    }
    else
    {
        NuVecSub(dist, a, b);
    }
    return NuVecMag(dist);
}
