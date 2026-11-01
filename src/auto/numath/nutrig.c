#include "numath/numath.h"

s32 NuAngAdd(s32 a, s32 b)
{
    s32 sum = (a + b) & 0xffff;
    if (sum > 0x7fff) {
        sum -= 0x10000;
    }
    return sum;
}

s32 NuAngSub(s32 a, s32 b)
{
    s32 diff = (a - b) & 0xffff;
    if (diff > 0x7fff) {
        diff -= 0x10000;
    }
    return diff;
}

s32 NuPower2(s32 n)
{
    s32 p;
    if (n > 127) {
        p = 128;
    } else {
        p = 1;
    }
    while (p < n) {
        p <<= 1;
    }
    return p;
}

s32 NuAng2AltSol(s32 a, s32 b)
{
    s32 diff = (a - b) & 0xffff;
    if (diff > 0x7fff) {
        diff -= 0x10000;
    }
    return diff;
}

f32 fn_8000B234(f32 x)
{
    s32 bits = *(s32*)&x;
    return bits < 0 ? -1.0f : 1.0f;
}

f32 fn_8000B2C4(f32 a, f32 b)
{
    return a - (s32)(a / b) * b;
}

void NuTexGetDDS(void)
{
}

void NuTexSetTextureDirect(void)
{
}

void fn_8000AFB8(void)
{
}

void fn_8000BCB0(f32* dst, f32* a, f32* b, f32 t)
{
    dst[0] = (1.0f - t) * a[0] + t * b[0];
    dst[1] = (1.0f - t) * a[1] + t * b[1];
    dst[2] = (1.0f - t) * a[2] + t * b[2];
    dst[3] = (1.0f - t) * a[3] + t * b[3];
}

f32 fn_8000B264(f32 x)
{
    f32 f = (f32)(s32)x;
    if (f > x) {
        return f - 1.0f;
    }
    return f;
}

f32 NuASin(s32 ang)
{
    f32 x = (f32)(ang & 0xffff) * 9.58738019e-05f;
    f32 sign = 1.0f;
    f32 x2;
    if (x > 3.14159274f) {
        x = 6.28318548f - x;
    }
    if (x > 1.57079637f) {
        x = 3.14159274f - x;
        sign = -1.0f;
    }
    x2 = x * x;
    return (((((-2.60500002e-07f * x2 + 2.47609005e-05f) * x2 - 0.00138883968f) * x2 + 0.0416666418f) * x2 - 0.5f) * x2 + 1.0f) * sign;
}

f32 NuAngDiff(s32 ang)
{
    f32 x = (f32)(ang & 0xffff) * 9.58738019e-05f;
    f32 sign = 1.0f;
    f32 x2;
    if (x > 3.14159274f) {
        x -= 3.14159274f;
        sign = -1.0f;
    }
    if (x > 1.57079637f) {
        x = 3.14159274f - x;
    }
    x2 = x * x;
    return ((((((-2.39000002e-08f * x2 + 2.75260004e-06f) * x2 - 0.000198409005f) * x2 + 0.00833333191f) * x2 - 0.166666672f) * x2 + 1.0f) * x) * sign;
}

s16 fn_8000AEC4(f32 x);

s16 NuACos(f32 x)
{
    return 0x4000 - fn_8000AEC4(x);
}

struct nusem_s {
    s32 pad[3];
};

extern struct nusem_s lbl_8020A140[];
extern void OSWaitSemaphore(struct nusem_s* sem);

void fn_8000A834(s32 id)
{
    if (id >= 0) {
        OSWaitSemaphore(&lbl_8020A140[id]);
    }
}

extern void OSSignalSemaphore(struct nusem_s* sem);

void fn_8000A86C(s32 id)
{
    if (id >= 0) {
        OSSignalSemaphore(&lbl_8020A140[id]);
    }
}

s32 fn_8000ABDC(f32 a, f32 b);

f32 fn_8000AFBC(f32 a, f32 b)
{
    return (f32)fn_8000ABDC(a, b) * 9.58738019e-05f;
}
void NuTexSetTexture(f32* d, struct nuvec_s* v, struct numtx_s* m)
{
    f32 a, b, c, e;
    b = v->x * m->_01 + v->y * m->_11 + v->z * m->_21 + m->_31;
    c = v->x * m->_02 + v->y * m->_12 + v->z * m->_22 + m->_32;
    e = v->x * m->_03 + v->y * m->_13 + v->z * m->_23 + m->_33;
    a = v->x * m->_00 + v->y * m->_10 + v->z * m->_20 + m->_30;
    d[0] = a;
    d[1] = b;
    d[2] = c;
    d[3] = e;
}
void NuTexLoadTextureFromDDSFile(f32* d, f32* v, struct numtx_s* m)
{
    f32 a, b, c, e;
    a = v[0] * m->_00 + v[1] * m->_10 + v[2] * m->_20 + v[3] * m->_30;
    b = v[0] * m->_01 + v[1] * m->_11 + v[2] * m->_21 + v[3] * m->_31;
    c = v[0] * m->_02 + v[1] * m->_12 + v[2] * m->_22 + v[3] * m->_32;
    e = v[0] * m->_03 + v[1] * m->_13 + v[2] * m->_23 + v[3] * m->_33;
    d[0] = a;
    d[1] = b;
    d[2] = c;
    d[3] = e;
}

s16 fn_8000AEC4(f32 x)
{
    s32 pos;
    s32 r;
    pos = x >= 0.0f;
    if (!pos) {
        x = -x;
    }
    if (x >= 1.0f) {
        if (pos) {
            return 0x4000;
        }
        return -0x4000;
    }
    r = (1.5707964f - NuFsqrt(1.0f - x) * (x * -0.212114394f + 1.57072878f + x * 0.074261002f * x + x * -0.0187292993f * x * x)) * 10430.378f;
    if (!pos) {
        r = -r;
    }
    return r;
}

extern u16 lbl_801B0AC8[];
u16 fn_8000A9F0(f32 a, f32 b)
{
    f32 t;
    f32 f;
    f32 r;
    s32 i;
    if (a > b) {
        t = b * 512.0f / a;
        i = t;
        f = t - i;
        r = (s32)(0x4000 - lbl_801B0AC8[i]) * (1.0f - f) + (s32)(0x4000 - lbl_801B0AC8[i + 1]) * f;
        return (s32)r;
    }
    if (a == b) {
        return lbl_801B0AC8[(s32)(a * 512.0f / b)];
    }
    t = a * 512.0f / b;
    i = t;
    f = t - i;
    r = (u32)lbl_801B0AC8[i] * (1.0f - f) + (u32)lbl_801B0AC8[i + 1] * f;
    return (s32)r;
}

static inline u16 atanq(f32 a, f32 b)
{
    if (a > b) {
        return 0x4000 - lbl_801B0AC8[(s32)(b * 512.0f / a)];
    }
    return lbl_801B0AC8[(s32)(a * 512.0f / b)];
}
s32 fn_8000ABDC(f32 a, f32 b)
{
    if (a == 0.0f) {
        return b < 0.0f ? 0x8000 : 0;
    }
    if (b == 0.0f) {
        return a < 0.0f ? 0xC000 : 0x4000;
    }
    if (a < 0.0f) {
        if (b < 0.0f) {
            return atanq(-a, -b) + 0x8000;
        }
        return -atanq(-a, b);
    }
    if (b < 0.0f) {
        return 0x8000 - atanq(a, -b);
    }
    return atanq(a, b);
}

s32 fn_8000AE18(f32 a, f32 b)
{
    s32 r;
    if (a == 0.0f) {
        if (b < 0.0f) {
            r = 0x8000;
        } else {
            r = 0;
        }
    } else if (b == 0.0f) {
        if (a < 0.0f) {
            r = 0xC000;
        } else {
            r = 0x4000;
        }
    } else if (a < 0.0f) {
        if (b < 0.0f) {
            r = fn_8000A9F0(-a, -b) + 0x8000;
        } else {
            r = -fn_8000A9F0(-a, b);
        }
    } else if (!(b < 0.0f)) {
        r = fn_8000A9F0(a, b);
    } else {
        r = 0x8000 - fn_8000A9F0(a, -b);
    }
    return r;
}
void fn_8000BAFC(f32* out, f32* a, f32* b, f32 t)
{
    f32 tmp[4];
    f32 cosom;
    f32 omega;
    f32 sinom;
    f32 s0;
    f32 s1;
    cosom = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
    if (cosom < 0.0f) {
        tmp[0] = -b[0];
        tmp[1] = -b[1];
        tmp[2] = -b[2];
        tmp[3] = -b[3];
        cosom = -cosom;
    } else {
        tmp[0] = b[0];
        tmp[1] = b[1];
        tmp[2] = b[2];
        tmp[3] = b[3];
    }
    if (1.0f - cosom > 0.0f) {
        omega = NuACos(cosom);
        sinom = NuAngDiff(omega);
        s0 = NuAngDiff((1.0f - t) * omega) / sinom;
        s1 = NuAngDiff(t * omega) / sinom;
    } else {
        s0 = 1.0f - t;
        s1 = t;
    }
    out[0] = s0 * a[0] + s1 * tmp[0];
    out[1] = s0 * a[1] + s1 * tmp[1];
    out[2] = s0 * a[2] + s1 * tmp[2];
    out[3] = s0 * a[3] + s1 * tmp[3];
}
void NuMtxMulR(struct numtx_s* mt, f32* q)
{
    f32* m = (f32*)mt;
    s32 nxt[3] = { 1, 2, 0 };
    f32 qq[4];
    f32 tr;
    f32 s;
    s32 i, j, k;
    tr = m[0] + m[5] + m[10];
    if (tr > 0.0f) {
        s = NuFsqrt(tr + 1.0f);
        q[3] = s * 0.5f;
        s = 0.5f / s;
        q[0] = (m[6] - m[9]) * s;
        q[1] = (m[8] - m[2]) * s;
        q[2] = (m[1] - m[4]) * s;
    } else {
        i = 0;
        if (m[5] > m[0]) {
            i = 1;
        }
        if (m[10] > m[i * 4 + i]) {
            i = 2;
        }
        j = nxt[i];
        k = nxt[j];
        s = NuFsqrt(m[i * 4 + i] - (m[j * 4 + j] + m[k * 4 + k]) + 1.0f);
        qq[i] = s * 0.5f;
        if (s != 0.0f) {
            s = 0.5f / s;
        }
        qq[3] = (m[j * 4 + k] - m[k * 4 + j]) * s;
        qq[j] = (m[i * 4 + j] + m[j * 4 + i]) * s;
        qq[k] = (m[i * 4 + k] + m[k * 4 + i]) * s;
        q[0] = qq[0];
        q[1] = qq[1];
        q[2] = qq[2];
        q[3] = qq[3];
    }
}

static inline void nuquat_fix_sign(f32* a, f32* b)
{
    if (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3] < 0.0f) {
        b[0] = -b[0];
        b[1] = -b[1];
        b[2] = -b[2];
        b[3] = -b[3];
    }
}
static inline f32 nuherm4(f32 t, f32 p0, f32 p1, f32 p2, f32 p3)
{
    f32 m0 = (p2 - p0) * 0.5f;
    f32 m1 = (p3 - p1) * 0.5f;
    f32 t2 = t * t;
    f32 t3 = t * t2;
    return (2.0f * t3 - 3.0f * t2 + 1.0f) * p1 + (t3 - 2.0f * t2 + t) * m0 + (-2.0f * t3 + 3.0f * t2) * p2 + (t3 - t2) * m1;
}
void fn_8000B8CC(f32* out, f32* a, f32* b, f32* c, f32* d, f32 t)
{
    nuquat_fix_sign(a, b);
    nuquat_fix_sign(b, c);
    nuquat_fix_sign(c, d);
    out[0] = nuherm4(t, a[0], b[0], c[0], d[0]);
    out[1] = nuherm4(t, a[1], b[1], c[1], d[1]);
    out[2] = nuherm4(t, a[2], b[2], c[2], d[2]);
    out[3] = nuherm4(t, a[3], b[3], c[3], d[3]);
}

static inline f32 nucubic4(f32 t, f32 p0, f32 p1, f32 p2, f32 p3)
{
    f32 k3 = -p0 / 6.0f + p1 * 0.5f + -p2 * 0.5f + p3 / 6.0f;
    f32 k2 = p0 * 0.5f - p1 + p2 * 0.5f;
    f32 k1 = -p0 / 3.0f + -p1 * 0.5f + p2 + -p3 / 6.0f;
    f32 t2 = t * t;
    f32 t3 = t * t2;
    return k3 * t3 + k2 * t2 + k1 * t + p1;
}
void fn_8000B5F4(f32* out, f32* a, f32* b, f32* c, f32* d, f32 t)
{
    nuquat_fix_sign(a, b);
    nuquat_fix_sign(b, c);
    nuquat_fix_sign(c, d);
    out[0] = nucubic4(t, a[0], b[0], c[0], d[0]);
    out[1] = nucubic4(t, a[1], b[1], c[1], d[1]);
    out[2] = nucubic4(t, a[2], b[2], c[2], d[2]);
    out[3] = nucubic4(t, a[3], b[3], c[3], d[3]);
}

static inline f32 nuquat_mul_comp(f32* q, s32 i, s32 j)
{
    return q[i] * q[j];
}
void fn_8000B518(f32* q, struct numtx_s* m)
{
    f32 xx, yy, zz, ww;
    f32 xy, zw, xz, yw, yz, xw;
    ww = nuquat_mul_comp(q, 3, 3);
    xx = nuquat_mul_comp(q, 0, 0);
    yy = nuquat_mul_comp(q, 1, 1);
    zz = nuquat_mul_comp(q, 2, 2);
    xy = nuquat_mul_comp(q, 0, 1);
    xy += xy;
    xz = nuquat_mul_comp(q, 0, 2);
    xz += xz;
    xw = nuquat_mul_comp(q, 0, 3);
    xw += xw;
    yz = nuquat_mul_comp(q, 1, 2);
    yz += yz;
    yw = nuquat_mul_comp(q, 1, 3);
    yw += yw;
    zw = nuquat_mul_comp(q, 2, 3);
    zw += zw;
    m->_23 = m->_13 = m->_03 = m->_32 = m->_31 = m->_30 = 0.0f;
    m->_33 = 1.0f;
    m->_00 = ww + xx - yy - zz;
    m->_10 = xy - zw;
    m->_20 = xz + yw;
    m->_01 = xy + zw;
    m->_11 = ww - xx + yy - zz;
    m->_21 = yz - xw;
    m->_02 = xz - yw;
    m->_12 = yz + xw;
    m->_22 = ww - xx - yy + zz;
}
