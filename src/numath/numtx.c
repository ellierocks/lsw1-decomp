#include "numath/numath.h"

void NuMtxTranslate(struct numtx_s* m, struct nuvec_s* v)
{
    m->_30 = m->_30 + v->x;
    m->_31 = m->_31 + v->y;
    m->_32 = m->_32 + v->z;
}

void NuMtxPreTranslate(struct numtx_s* m, struct nuvec_s* v)
{
    m->_30 = v->x * m->_00 + v->y * m->_10 + v->z * m->_20 + m->_30;
    m->_31 = v->x * m->_01 + v->y * m->_11 + v->z * m->_21 + m->_31;
    m->_32 = v->x * m->_02 + v->y * m->_12 + v->z * m->_22 + m->_32;
}

void NuMtxScale(struct numtx_s* m, struct nuvec_s* v)
{
    m->_00 = m->_00 * v->x;
    m->_01 = m->_01 * v->y;
    m->_02 = m->_02 * v->z;
    m->_10 = m->_10 * v->x;
    m->_11 = m->_11 * v->y;
    m->_12 = m->_12 * v->z;
    m->_20 = m->_20 * v->x;
    m->_21 = m->_21 * v->y;
    m->_22 = m->_22 * v->z;
    m->_30 = m->_30 * v->x;
    m->_31 = m->_31 * v->y;
    m->_32 = m->_32 * v->z;
}

void NuMtxScaleU(struct numtx_s* m, f32 s)
{
    m->_00 = m->_00 * s;
    m->_01 = m->_01 * s;
    m->_02 = m->_02 * s;
    m->_10 = m->_10 * s;
    m->_11 = m->_11 * s;
    m->_12 = m->_12 * s;
    m->_20 = m->_20 * s;
    m->_21 = m->_21 * s;
    m->_22 = m->_22 * s;
    m->_30 = m->_30 * s;
    m->_31 = m->_31 * s;
    m->_32 = m->_32 * s;
}

struct nuvec_s* NuMtxGetScale(struct nuvec_s* dest, struct numtx_s* m)
{
    struct nuvec_s s;
    s.x = NuFsqrt(m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02);
    s.y = NuFsqrt(m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12);
    s.z = NuFsqrt(m->_20 * m->_20 + m->_21 * m->_21 + m->_22 * m->_22);
    *dest = s;
    return dest;
}

void NuMtxPreScale(struct numtx_s* m, struct nuvec_s* v)
{
    m->_00 = m->_00 * v->x;
    m->_01 = m->_01 * v->x;
    m->_02 = m->_02 * v->x;
    m->_10 = m->_10 * v->y;
    m->_11 = m->_11 * v->y;
    m->_12 = m->_12 * v->y;
    m->_20 = m->_20 * v->z;
    m->_21 = m->_21 * v->z;
    m->_22 = m->_22 * v->z;
}
