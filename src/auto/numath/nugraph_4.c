#include "types.h"
#include "numath/numath.h"
#include "numath/nuvec.h"


void fn_8000E250(struct numtx_s* m, s32 a)
{
    f32 s = NuASin(a);
    f32 c = NuAngDiff(a);
    f32 m01 = m->_01, m02 = m->_02;
    f32 m11 = m->_11, m12 = m->_12;
    f32 m21 = m->_21, m22 = m->_22;
    f32 m31 = m->_31, m32 = m->_32;
    m->_01 = m01 * s - m02 * c;
    m->_02 = m01 * c + m02 * s;
    m->_11 = m11 * s - m12 * c;
    m->_12 = m11 * c + m12 * s;
    m->_21 = m21 * s - m22 * c;
    m->_22 = m21 * c + m22 * s;
    m->_31 = m31 * s - m32 * c;
    m->_32 = m31 * c + m32 * s;
}

void fn_8000E318(struct numtx_s* m, s32 a)
{
    f32 s = NuASin(a);
    f32 c = NuAngDiff(a);
    f32 m10 = m->_10, m11 = m->_11, m12 = m->_12;
    f32 m20 = m->_20, m21 = m->_21, m22 = m->_22;
    m->_10 = s * m10 + c * m20;
    m->_11 = s * m11 + c * m21;
    m->_12 = s * m12 + c * m22;
    m->_20 = s * m20 - c * m10;
    m->_21 = s * m21 - c * m11;
    m->_22 = s * m22 - c * m12;
}

void fn_8000E3C0(struct numtx_s* m, s32 a)
{
    f32 s = NuASin(a);
    f32 c = NuAngDiff(a);
    f32 m00 = m->_00, m10 = m->_10, m20 = m->_20, m30 = m->_30;
    f32 m02 = m->_02, m12 = m->_12, m22 = m->_22, m32 = m->_32;
    m->_00 = m00 * s + m02 * c;
    m->_02 = m02 * s - m00 * c;
    m->_10 = m10 * s + m12 * c;
    m->_12 = m12 * s - m10 * c;
    m->_20 = m20 * s + m22 * c;
    m->_22 = m22 * s - m20 * c;
    m->_30 = m30 * s + m32 * c;
    m->_32 = m32 * s - m30 * c;
}

void fn_8000E498(struct numtx_s* m, s32 a)
{
    f32 s = NuASin(a);
    f32 c = NuAngDiff(a);
    f32 m00 = m->_00, m01 = m->_01, m02 = m->_02;
    f32 m20 = m->_20, m21 = m->_21, m22 = m->_22;
    m->_00 = s * m00 - c * m20;
    m->_01 = s * m01 - c * m21;
    m->_02 = s * m02 - c * m22;
    m->_20 = c * m00 + s * m20;
    m->_21 = c * m01 + s * m21;
    m->_22 = c * m02 + s * m22;
}

void fn_8000E540(struct numtx_s* m, s32 a)
{
    f32 s = NuASin(a);
    f32 c = NuAngDiff(a);
    f32 m00 = m->_00, m01 = m->_01;
    f32 m10 = m->_10, m11 = m->_11;
    f32 m20 = m->_20, m21 = m->_21;
    f32 m30 = m->_30, m31 = m->_31;
    m->_00 = m00 * s - m01 * c;
    m->_01 = m00 * c + m01 * s;
    m->_10 = m10 * s - m11 * c;
    m->_11 = m10 * c + m11 * s;
    m->_20 = m20 * s - m21 * c;
    m->_21 = m20 * c + m21 * s;
    m->_30 = m30 * s - m31 * c;
    m->_31 = m30 * c + m31 * s;
}

void fn_8000E608(struct numtx_s* m, s32 a)
{
    f32 s = NuASin(a);
    f32 c = NuAngDiff(a);
    f32 m00 = m->_00, m01 = m->_01, m02 = m->_02;
    f32 m10 = m->_10, m11 = m->_11, m12 = m->_12;
    m->_00 = s * m00 + c * m10;
    m->_01 = s * m01 + c * m11;
    m->_02 = s * m02 + c * m12;
    m->_10 = s * m10 - c * m00;
    m->_11 = s * m11 - c * m01;
    m->_12 = s * m12 - c * m02;
}

void fn_8000E6B0(struct numtx_s* d, struct numtx_s* s)
{
    f32 t;
    f32 tx = -s->_30;
    f32 ty = -s->_31;
    f32 tz = -s->_32;
    t = s->_01;
    d->_01 = s->_10;
    d->_10 = t;
    t = s->_02;
    d->_02 = s->_20;
    d->_20 = t;
    t = s->_12;
    d->_12 = s->_21;
    d->_21 = t;
    d->_00 = s->_00;
    d->_11 = s->_11;
    d->_22 = s->_22;
    d->_30 = tx * d->_00 + ty * d->_10 + tz * d->_20;
    d->_31 = tx * d->_01 + ty * d->_11 + tz * d->_21;
    d->_32 = tx * d->_02 + ty * d->_12 + tz * d->_22;
    d->_23 = 0.0f;
    d->_13 = 0.0f;
    d->_03 = 0.0f;
    d->_33 = 1.0f;
}

void fn_8000E76C(struct numtx_s* d, struct numtx_s* s)
{
    f32 t;
    t = s->_01;
    d->_01 = s->_10;
    d->_10 = t;
    t = s->_02;
    d->_02 = s->_20;
    d->_20 = t;
    t = s->_12;
    d->_12 = s->_21;
    d->_21 = t;
    d->_00 = s->_00;
    d->_11 = s->_11;
    d->_22 = s->_22;
    d->_23 = 0.0f;
    d->_13 = 0.0f;
    d->_03 = 0.0f;
    d->_32 = 0.0f;
    d->_31 = 0.0f;
    d->_30 = 0.0f;
    d->_33 = 1.0f;
}
