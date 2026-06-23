#ifndef NUMATH_H
#define NUMATH_H

#include "types.h"

// Angles
#define ANGLE_RANGE 0x10000
#define ANGLE_0     0x0000
#define ANGLE_90    0x4000
#define ANGLE_180   0x8000
#define ANGLE_270   0xC000

#define ANG_SIN(a) NuTrigTable[(a) & (ANGLE_RANGE - 1)]
#define ANG_COS(a) NuTrigTable[((a) + ANGLE_90) & (ANGLE_RANGE - 1)]

extern f32 NuTrigTable[65536];

struct nuvec_s {
    f32 x, y, z;
};

struct nuvec4_s {
    f32 x, y, z, w;
};

struct nuivec_s {
    s32 x, y, z;
};

struct nuivec4_s {
    s32 x, y, z, w;
};

struct numtx_s {
    f32 _00, _01, _02, _03;
    f32 _10, _11, _12, _13;
    f32 _20, _21, _22, _23;
    f32 _30, _31, _32, _33;
};

struct Quat {
    f32 x, y, z, w;
};

struct nucolour3_s {
    f32 r, g, b;
};

struct nunrand_s {
    s32 idum;
};

struct nuangvec_s {
    s32 x, y, z;
};

// Math functions
void NuVecMtxTransform(struct nuvec_s* dst, struct nuvec_s* src, struct numtx_s* mtx);
void NuVecMtxTransformH(struct nuvec_s* dst, struct nuvec_s* src, struct numtx_s* mtx);
void NuVecMtxRotate(struct nuvec_s* dst, struct nuvec_s* src, struct numtx_s* mtx);
void NuVecInvMtxTransform(struct nuvec_s* dst, struct nuvec_s* src, struct numtx_s* mtx);
void NuVecInvMtxRotate(struct nuvec_s* dst, struct nuvec_s* src, struct numtx_s* mtx);
void NuVecRotateX(struct nuvec_s* dst, struct nuvec_s* src, s32 angle);
void NuVecRotateY(struct nuvec_s* dst, struct nuvec_s* src, s32 angle);
void NuVecRotateZ(struct nuvec_s* dst, struct nuvec_s* src, s32 angle);
void NuVecRotateAxis(struct nuvec_s* dst, s32 angle, f32 scale);
void NuVecAdd(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b);
void NuVecSub(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b);
void NuVecScale(struct nuvec_s* dst, struct nuvec_s* src, f32 scale);
void NuVecScaleAccum(struct nuvec_s* dst, struct nuvec_s* accum, struct nuvec_s* src, f32 scale);
void NuVecCross(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b);
f32 NuVecDot(struct nuvec_s* a, struct nuvec_s* b);
f32 NuVecMag(struct nuvec_s* v);
f32 NuVecMagSqr(struct nuvec_s* v);
f32 NuVecNorm(struct nuvec_s* v);
f32 NuVecDist(struct nuvec_s* a, struct nuvec_s* b, struct nuvec_s* dist);
f32 NuVecXZDist(struct nuvec_s* a, struct nuvec_s* b);
void NuVecMax(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b);
void NuVecMin(struct nuvec_s* dst, struct nuvec_s* a, struct nuvec_s* b);
void NuMtxSetIdentity(struct numtx_s* mtx);
void NuMtxSetTranslation(struct numtx_s* mtx, struct nuvec_s* trans);
void NuMtxSetScale(struct numtx_s* mtx, struct nuvec_s* scale);
void NuMtxSetRotationX(struct numtx_s* mtx, f32 angle);
void NuMtxSetRotationY(struct numtx_s* mtx, f32 angle);
void NuMtxSetRotationZ(struct numtx_s* mtx, f32 angle);
void NuMtxMul(struct numtx_s* dst, struct numtx_s* a, struct numtx_s* b);
void NuMtxTranspose(struct numtx_s* dst, struct numtx_s* src);
void NuMtxTranslate(struct numtx_s* m, struct nuvec_s* v);
void NuMtxPreTranslate(struct numtx_s* m, struct nuvec_s* v);
void NuMtxScale(struct numtx_s* m, struct nuvec_s* v);
void NuMtxScaleU(struct numtx_s* m, f32 s);
struct nuvec_s* NuMtxGetScale(struct nuvec_s* dest, struct numtx_s* m);
void NuMtxPreScale(struct numtx_s* m, struct nuvec_s* v);
f32 NuFsqrt(f32 x);
void NuMtxInv(struct numtx_s* dst, struct numtx_s* src);
void NuMtxToQuat(struct Quat* dst, struct numtx_s* src);
void NuQuatToMtx(struct numtx_s* dst, struct Quat* src);
void NuQuatMul(struct Quat* dst, struct Quat* a, struct Quat* b);
void NuQuatNormalise(struct Quat* q);
void NuQuatSlerp(struct Quat* dst, struct Quat* a, struct Quat* b, f32 t);
s32 NuAtan2(f32 y, f32 x);
f32 NuRand(void);
void NuRandSeed(s32 seed);

#endif
