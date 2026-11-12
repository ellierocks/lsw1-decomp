#include "types.h"
#include "numath/numath.h"

/* LSW1 camera: 0x68 bytes (NuCameraCreate allocates 0x68 and stores
 * fov 0x40, aspect 0x44, nearclip 0x48, farclip 0x4C, portal near 0x58,
 * scale 0x5C). CrashWOC's nucamera_s has no 0x50/0x54 pair. */
struct nucam_s {
    struct numtx_s mtx;
    f32 fov;
    f32 aspect;
    f32 nearclip;
    f32 farclip;
    f32 unk50;
    f32 unk54;
    f32 portalnearclip;
    struct nuvec_s scale;
};

/* Viewport (CrashWOC nuviewport_s order, confirmed by NuGlassProcessScene/NuGlassLoad). */
struct nuvp_s {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
    f32 zmin;
    f32 zmax;
    f32 centre_x;
    f32 centre_y;
    f32 clipmin_x;
    f32 clipmin_y;
    f32 clipmax_x;
    f32 clipmax_y;
    f32 clip_w;
    f32 clip_h;
    f32 scis_w;
    f32 scis_h;
};
extern struct nuvp_s lbl_8020E8F4;    /* device viewport */
extern struct nuvp_s lbl_8020E934;    /* current viewport */
extern s32 lbl_8040A338;              /* viewport invalid */
void fn_80039BE4(struct nuvp_s* vp);
void NuGlassInit(void);
void NuGlassLoad(void);

struct nuanimdata2_s {
    s32 magic;
    s16 end;
};

void* NuMemAlloc(s32 size, char* file, s32 line);
void fn_8000DC10(struct numtx_s* m);
void fn_8000CD08(s32 a, u8* b, s32 c);
void NuTexSetTexture(struct nuvec4_s* dst, struct nuvec_s* src, struct numtx_s* m);
s32 NuAnimCurveSetApplyBlendToJoint2(struct nucam_s* cam, s32 fast);

extern struct nucam_s lbl_80313268;   /* global camera */
extern u8 lbl_80313298[];       /* global camera translation (mtx row 3) */
extern struct nucam_s lbl_80314354;   /* camera saved by fn_80023B50 */
extern struct nucam_s lbl_803143BC;   /* camera saved before effect mode */
extern struct numtx_s lbl_801B1638;   /* view matrix */
extern struct numtx_s lbl_801B1738;
extern struct numtx_s lbl_801B1778;   /* view-projection-clip matrix */
extern struct numtx_s lbl_801B17F8;
extern struct numtx_s lbl_801B1878;
extern s32 lbl_80407C58;              /* camera effect mode */
extern f32 lbl_8040AD3C;              /* zy */
extern f32 lbl_8040AD40;              /* zx */

s32 NuAnimData2GetEnd(struct nuanimdata2_s* d)
{
    if (d->magic == 0x414e4934) {
        return (u16)d->end;
    }
    return d->end;
}

f32 fn_800215F0(s32* d)
{
    if (*d == 0x414e4934) {
        return *(u16*)((u8*)d + 0xa);
    }
    return *(f32*)d;
}

void fn_8002163C(s32* p, s32 off)
{
    if (p[0] == 0x414e4934) {
        if (p[10] != 0) p[10] += off;
        if (p[9] != 0) p[9] += off;
        if (p[12] != 0) p[12] += off;
        if (p[11] != 0) p[11] += off;
        if (p[13] != 0) p[13] += off;
        if (p[14] != 0) p[14] += off;
    } else if (p[0] == 0x414e4933) {
        s32* q;
        off = (s32)p - p[1];
        q = (s32*)(p[2] + off);
        p[2] = (s32)q;
        q[4] += off;
        q[3] += off * 4;
        q[5] += off;
    }
}

s32 fn_8002171C(u8* p)
{
    u8* q = *(u8**)(p + 0x1a0);
    if (q != 0) {
        return *(s32*)(q + 0x6c);
    }
    return *(s32*)(p + 0x20);
}

void fn_80021738(void *arg0, void *arg1, s32 arg2) {
    void *temp_r11;

    temp_r11 = (*(void **)((s8 *)(arg1) + (0x1A0)));
    if (temp_r11 != NULL) {
        (*(void **)((s8 *)(arg0) + (0))) = arg1;
        (*(s32 *)((s8 *)(arg0) + (4))) = 0;
        (*(void **)((s8 *)(arg0) + (8))) = (void *) ((*(s32 *)((s8 *)((*(void **)((s8 *)(arg1) + (0x1A0)))) + (0x70))) + (arg2 * 0xD0));
        return;
    }
    (*(void **)((s8 *)(arg0) + (0))) = arg1;
    (*(s32 *)((s8 *)(arg0) + (4))) = (s32) ((*(s32 *)((s8 *)(arg1) + (0x24))) + (arg2 * 0x50));
    (*(void **)((s8 *)(arg0) + (8))) = temp_r11;
}

f32 NuASin(s32 a);
f32 NuAngDiff(s32 a);
void* memset();
extern struct numtx_s lbl_8033EA8C;
extern f32 lbl_8040AD58;
extern f32 lbl_8040AD5C;
extern struct nuvec_s lbl_80337860[9];
s32 fn_8000ABDC(f32, f32);

static inline void rot_xy(struct nuvec_s* v, f32 pitch, f32 yaw)
{
    f32 s, c, s2, c2, t, x, y, z;
    s32 i;
    i = (s32)pitch;
    s = NuAngDiff(i);
    c = NuASin(i);
    i = (s32)yaw;
    s2 = NuAngDiff(i);
    c2 = NuASin(i);
    z = v->z;
    y = v->y;
    x = v->x;
    t = y * s + z * c;
    y = y * c - z * s;
    z = t;
    v->z = z * c2 - x * s2;
    v->y = y;
    v->x = z * s2 + x * c2;
}

/* NONMATCHING 135/149: one rot_xy instance swaps x/y registers; see config/plan.toml. */
void fn_80022DE8(struct nuvec_s* p)
{
    struct nuvec_s ang;
    s32 a;
    s32 i;

    ang.y = fn_8000ABDC(p->x, p->z);
    ang.x = fn_8000ABDC(-p->y, NuFsqrt(p->x * p->x + p->z * p->z));
    if (ang.x < 8192.0f) {
        ang.x = 8192.0f;
    }
    p->z = NuFsqrt(p->x * p->x + p->y * p->y + p->z * p->z);
    p->y = 0.0f;
    p->x = 0.0f;
    rot_xy(p, ang.x, ang.y);
    a = 0;
    for (i = 0; i < 8; i++) {
        p = &lbl_80337860[i];
        p->x = NuASin(a);
        p->y = -NuAngDiff(a);
        a += 0x2000;
        p->z = 0.0f;
        rot_xy(p, ang.x, ang.y);
    }
}

static inline f32 cotan(f32 fov)
{
    s32 angle = (s32)(fov * 0.5f * 10430.378f);
    return NuASin(angle) / NuAngDiff(angle);
}
void fn_800234A8(struct numtx_s* mtx, f32 fov, f32 aspect, f32 nearclip, f32 farclip)
{
    struct numtx_s* m = &lbl_8033EA8C;
    f32 depth = farclip / (farclip - nearclip);
    f32 cot = cotan(fov);
    f32 x_scale = aspect * cot;
    f32 q;

    memset(mtx, 0, 0x40);
    mtx->_22 = depth;
    mtx->_23 = 1.0f;
    mtx->_11 = cot;
    mtx->_00 = x_scale;
    mtx->_32 = -depth * nearclip;

    cot = cotan(fov);
    lbl_8040AD58 = farclip;
    lbl_8040AD5C = nearclip;
    q = 1.0f / (farclip - nearclip);
    m->_00 = cot * aspect;
    m->_01 = 0.0f;
    m->_02 = 0.0f;
    m->_03 = 0.0f;
    m->_10 = 0.0f;
    m->_11 = cot;
    m->_12 = 0.0f;
    m->_13 = 0.0f;
    m->_20 = 0.0f;
    m->_21 = 0.0f;
    m->_22 = -nearclip * q;
    m->_23 = -1.0f;
    m->_30 = 0.0f;
    m->_31 = 0.0f;
    m->_32 = -(farclip * nearclip) * q;
    m->_33 = 0.0f;
}

void fn_8000D134(struct numtx_s* dst, struct numtx_s* a, struct numtx_s* b);
extern struct numtx_s lbl_801B0F08;

s32 fn_80023628(struct nuvec_s* min, struct nuvec_s* max, struct numtx_s* wm, f32 farclip)
{
    struct nuvec_s src[8];
    struct nuvec_s dst[8];
    struct numtx_s m;
    u8 oc[8];
    s32 new_var;
    s32 i;
    s32 c;
    s32 r;

    if (farclip == 0.0f) {
        farclip = lbl_80313268.farclip;
    }
    if (wm == 0 || wm == &lbl_801B0F08) {
        m = lbl_801B1638;
    } else {
        fn_8000D134(&m, wm, &lbl_801B1638);
    }
    src[0].x = min->x; src[0].y = min->y; src[0].z = min->z;
    src[1].x = max->x; src[1].y = max->y; src[1].z = max->z;
    src[2].x = min->x; src[2].y = min->y; src[2].z = max->z;
    src[3].x = max->x; src[3].y = max->y; src[3].z = min->z;
    src[4].x = min->x; src[4].y = max->y; src[4].z = min->z;
    src[5].x = min->x; src[5].y = max->y; src[5].z = max->z;
    src[6].x = max->x; src[6].y = min->y; src[6].z = min->z;
    src[7].x = max->x; src[7].y = min->y; src[7].z = max->z;
    NuVecMtxTransform(&dst[0], &src[0], &m);
    NuVecMtxTransform(&dst[1], &src[1], &m);
    NuVecMtxTransform(&dst[2], &src[2], &m);
    NuVecMtxTransform(&dst[3], &src[3], &m);
    NuVecMtxTransform(&dst[4], &src[4], &m);
    NuVecMtxTransform(&dst[5], &src[5], &m);
    NuVecMtxTransform(&dst[6], &src[6], &m);
    NuVecMtxTransform(&dst[7], &src[7], &m);
    new_var = 2;
    for (i = 0; i < 8; i++) {
        c = 0;
        if (dst[i].z < lbl_80313268.nearclip) {
            c = 0x20;
        }
        if (dst[i].z > farclip) {
            c |= 0x10;
        }
        if (dst[i].x < -dst[i].z * lbl_8040AD40) {
            c |= 1;
        }
        if (dst[i].x > dst[i].z * lbl_8040AD40) {
            c |= 2;
        }
        if (dst[i].y < -dst[i].z * lbl_8040AD3C) {
            c |= 8;
        }
        if (dst[i].y > dst[i].z * lbl_8040AD3C) {
            c |= 4;
        }
        oc[i] = c;
    }
    if ((oc[0] & oc[1] & oc[new_var] & oc[3] & oc[4] & oc[5] & oc[6] & oc[7]) == 0) {
        r = 1;
        if (oc[0] | oc[1] | oc[2] | oc[3] | oc[4] | oc[5] | oc[6] | oc[7]) {
            r = 2;
        }
        return r;
    }
    return 0;
}

s32 NuCameraClipTestPoints(struct nuvec_s* pnts, s32 cnt, struct numtx_s* wm)
{
    struct numtx_s m;
    struct nuvec_s v;
    s32 c;
    s32 out;
    s32 i;

    if (wm != 0) {
        NuMtxMul(&m, wm, &lbl_801B1638);
    } else {
        m = lbl_801B1638;
    }
    out = -1;
    for (i = 0; i < cnt; i++) {
        NuVecMtxTransform(&v, pnts, &m);
        c = 0;
        if (v.z < lbl_80313268.nearclip) {
            c = 0x20;
        }
        if (v.z > lbl_80313268.farclip) {
            c |= 0x10;
        }
        if (v.x < -v.z * lbl_8040AD40) {
            c |= 1;
        }
        if (v.x > v.z * lbl_8040AD40) {
            c |= 2;
        }
        if (v.y < -v.z * lbl_8040AD3C) {
            c |= 8;
        }
        if (v.y > v.z * lbl_8040AD3C) {
            c |= 4;
        }
        out &= c;
        pnts++;
    }
    return out;
}

void fn_80023B50(s32 mode)
{
    if (mode != lbl_80407C58 && lbl_80407C58 == 0) {
        lbl_803143BC = lbl_80313268;
    }
    if (mode != 0) {
        lbl_80314354 = lbl_80313268;
        NuAnimCurveSetApplyBlendToJoint2(&lbl_803143BC, 0);
    }
    lbl_80407C58 = mode;
}

void NuCameraGet(struct nucam_s* cam)
{
    *cam = lbl_80313268;
}

struct nucam_s* NuCameraCreate(void)
{
    struct nucam_s* cam = NuMemAlloc(0x68, "../nu2api.legogc/nu3d/ngc/ngc_obj/../nucamera.c", 0x49);
    fn_8000DC10(&cam->mtx);
    cam->fov = 0.75f;
    cam->nearclip = 0.15f;
    cam->portalnearclip = 0.0f;
    cam->farclip = 10000.0f;
    cam->aspect = 0.75f;
    cam->scale.z = 1.0f;
    cam->scale.y = 1.0f;
    cam->scale.x = 1.0f;
    return cam;
}

void NuCameraCalcFrustrumPlanes(struct nucam_s* cam)
{
    NuAnimCurveSetApplyBlendToJoint2(cam, 0);
}

struct numtx_s* NuCameraGetMtx(void)
{
    return &lbl_80313268.mtx;
}

struct numtx_s* NuCameraGetViewMtx(void)
{
    return &lbl_80313268.mtx;
}

struct numtx_s* NuCameraGetProjectionMtx(void)
{
    return &lbl_801B1638;
}

struct numtx_s* NuCameraGetClippingMtx(void)
{
    return &lbl_801B17F8;
}

void NuCameraGetClipMtx(struct numtx_s* a, struct numtx_s* b)
{
    if (a != 0) {
        *a = lbl_801B1778;
    }
    if (b != 0) {
        *b = lbl_801B1738;
    }
}

void NuCameraTransformClip(struct nuvec_s* dest, struct nuvec_s* src, s32 n, struct numtx_s* w)
{
    struct nuvec_s* end;
    struct numtx_s m;

    end = &src[n];
    if (w != 0) {
        NuMtxMul(&m, w, &lbl_801B1778);
    } else {
        m = lbl_801B1778;
    }
    for (; src < end; src++, dest++) {
        NuVecMtxTransformH(dest, src, &m);
    }
}

s32 NuCameraClipTestBoundingSphere(struct nuvec_s* c, struct numtx_s* wm, f32 r)
{
    struct nuvec4_s d;
    struct nuvec_s v;
    struct nucam_s* cam = &lbl_80313268;
    struct numtx_s* vm = &lbl_801B1638;

    NuVecMtxTransform(&v, c, wm);
    NuVecMtxTransform(&v, &v, vm);
    if (v.z - cam->nearclip + r < 0.0f || cam->farclip - v.z + r < 0.0f) {
        return 1;
    }
    NuTexSetTexture(&d, &v, &lbl_801B1878);
    if (d.x < -r) return 1;
    if (d.y < -r) return 1;
    if (d.z < -r) return 1;
    return d.w < -r;
}

void NuCameraDistSqr(s32 a)
{
    fn_8000CD08(a, lbl_80313298, 0);
}

void NuCameraEnableClipping(struct nuvec_s* v, f32 x, f32 y)
{
    v->x = x * lbl_8040AD40;
    v->y = y * lbl_8040AD3C;
    v->z = 1.0f;
    NuVecMtxRotate(v, v, &lbl_80313268.mtx);
}

void NuCameraTransformScreenClip(void)
{
    if (lbl_80407C58 == 1) {
        NuAnimCurveSetApplyBlendToJoint2(&lbl_80314354, 0);
    }
}

void NuPs2CreateSkin(void)
{
    if (lbl_80407C58 == 1) {
        NuAnimCurveSetApplyBlendToJoint2(&lbl_803143BC, 0);
    }
}

void NuGlassProcessScene(void)
{
    fn_80039BE4(&lbl_8020E8F4);
    lbl_8020E8F4.centre_x = 0.5f;
    lbl_8020E8F4.centre_y = 0.5f;
    lbl_8020E8F4.clipmin_x = 0.0f;
    lbl_8020E8F4.clipmin_y = 0.0f;
    lbl_8020E8F4.clipmax_x = 1.0f;
    lbl_8020E8F4.clipmax_y = 1.0f;
    lbl_8020E934 = lbl_8020E8F4;
    lbl_8040A338 = 1;
    NuGlassLoad();
}

void fn_80039BE4(struct nuvp_s* vp);
void NuGlassInit(void);
void NuGlassLoad(void);
void NuGlassLoad(void)
{
    if (lbl_8040A338 != 0) {
        lbl_8040A338 = 0;
        NuGlassInit();
        if (lbl_8020E934.clipmin_x == 0.0f && lbl_8020E934.clipmax_x == 1.0f) {
            lbl_8020E934.clip_w = 1.0f;
            lbl_8020E934.scis_w = (f32)lbl_8020E934.width * (1.0f / 65536.0f);
        } else {
            lbl_8020E934.clip_w = lbl_8020E934.scis_w = 1.0f / (lbl_8020E934.clipmax_x - lbl_8020E934.clipmin_x);
        }
        if (lbl_8020E934.clipmin_y == 0.0f && lbl_8020E934.clipmax_y == 1.0f) {
            lbl_8020E934.clip_h = 1.0f;
            lbl_8020E934.scis_h = (f32)lbl_8020E934.height * (1.0f / 65536.0f);
        } else {
            lbl_8020E934.clip_h = lbl_8020E934.scis_h = 1.0f / (lbl_8020E934.clipmax_y - lbl_8020E934.clipmin_y);
        }
    }
}

extern struct numtx_s lbl_8020E974;
void NuGlassInit(void)
{
    f32 x = (f32)lbl_8020E934.x;
    f32 y = (f32)lbl_8020E934.y;
    f32 w = (f32)lbl_8020E934.width;
    f32 h = (f32)lbl_8020E934.height;

    lbl_8020E974._00 = w * 0.5f;
    lbl_8020E974._01 = 0.0f;
    lbl_8020E974._02 = 0.0f;
    lbl_8020E974._03 = 0.0f;
    lbl_8020E974._10 = 0.0f;
    lbl_8020E974._11 = -h * 0.5f;
    lbl_8020E974._12 = 0.0f;
    lbl_8020E974._13 = 0.0f;
    lbl_8020E974._20 = 0.0f;
    lbl_8020E974._21 = 0.0f;
    lbl_8020E974._22 = 16777214.0f;
    lbl_8020E974._23 = 0.0f;
    lbl_8020E974._30 = w * 0.5f + x;
    lbl_8020E974._31 = h * 0.5f + y;
    lbl_8020E974._32 = 0.0f;
    lbl_8020E974._33 = 1.0f;
}
