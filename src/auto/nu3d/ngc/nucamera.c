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

/* Viewport (CrashWOC nuviewport_s order, confirmed by NuVpInit/NuVpUpdate). */
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
void NuVpSetScalingMtx(void);
void NuVpUpdate(void);

struct nuanimdata2_s {
    s32 magic;
    s16 end;
};

void* NuMemAlloc(s32 size, char* file, s32 line);
void NuMtxSetIdentity(struct numtx_s* m);
void fn_8000CD08(s32 a, u8* b, s32 c);
void NuVec4MtxTransform(struct nuvec4_s* dst, struct nuvec_s* src, struct numtx_s* m);
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

f32 NuCosApprox(s32 a);
f32 NuSinApprox(s32 a);
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
    s = NuSinApprox(i);
    c = NuCosApprox(i);
    i = (s32)yaw;
    s2 = NuSinApprox(i);
    c2 = NuCosApprox(i);
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
        p->x = NuCosApprox(a);
        p->y = -NuSinApprox(a);
        a += 0x2000;
        p->z = 0.0f;
        rot_xy(p, ang.x, ang.y);
    }
}

static inline f32 cotan(f32 fov)
{
    s32 angle = (s32)(fov * 0.5f * 10430.378f);
    return NuCosApprox(angle) / NuSinApprox(angle);
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
    NuMtxSetIdentity(&cam->mtx);
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
    NuVec4MtxTransform(&d, &v, &lbl_801B1878);
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

void NuVpInit(void)
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
    NuVpUpdate();
}

void fn_80039BE4(struct nuvp_s* vp);
void NuVpSetScalingMtx(void);
void NuVpUpdate(void);
void NuVpUpdate(void)
{
    if (lbl_8040A338 != 0) {
        lbl_8040A338 = 0;
        NuVpSetScalingMtx();
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
void NuVpSetScalingMtx(void)
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
/* GC record layout read by fn_8002303C (0x68 stride; field names provisional).
 * fn_80021784 and fn_800225C4 take the matrix and direction by value: the
 * caller's copies are the ABI's argument temporaries at sp+0x18/0x58. */
/* Bit 0 of the first word (p0.x read as an integer) selects the cylinder path. */
struct camera_child40 {
    struct nuvec_s p0;
    struct nuvec_s a;
    struct nuvec_s b;
    f32 unk24;
    struct nuvec_s c;
    u8 unk34[0xC];
};
struct camera_child30 { struct nuvec_s corners[4]; };
struct camera_pair { struct nuvec4_s *faces; struct nuvec4_s *verts; };
struct camera_record {
    struct numtx_s mtx;
    struct camera_child30 *children30;
    struct camera_child40 *children40;
    struct camera_pair *pairs;
    f32 vector_x;
    f32 vector_y;
    f32 vector_z;
    u32 mask;
    f32 outer_radius;
    f32 inner_radius;
    u8 count30;
    u8 count40;
    u8 count_pair;
    u8 group;
};

extern struct camera_record lbl_803378CC[];
/* lbl_80337924 is lbl_803378CC[0].mask; the target relocation names it. */
extern u32 lbl_80337924[];
extern s32 lbl_80407CE0;
extern s32 lbl_8040AD54;
s32 fn_80021784(struct camera_child40 *, struct numtx_s, struct nuvec_s);
void fn_800225C4(struct camera_child30 *, struct camera_record *, struct nuvec_s, f32, f32);
void fn_80022A88(struct camera_record *, struct nuvec4_s *, struct nuvec4_s *, struct nuvec_s *);

extern struct nuvec_s lbl_803145E0[];
extern s32 lbl_80407CE4;
void fn_8000EE70(struct nuvec4_s *dst, struct nuvec4_s *src, struct numtx_s *m);
void NuVec4MtxTransformVU0(struct nuvec4_s *dst, struct nuvec4_s *src, struct numtx_s *m);

/* fn_80021784: v[0] is the transformed origin, v[1] the first edge and v[2]
 * the unit normal (edge x dir). l is reused for |v[2]|^2, its inverse length,
 * the second dot and the radius; one pseudo keeps all four in f2. */
void NuVecCross(struct nuvec_s *dst, struct nuvec_s *a, struct nuvec_s *b);
f32 NuFdiv(f32 a, f32 b);

s32 fn_80021784(struct camera_child40 *box, struct numtx_s mtx, struct nuvec_s dir)
{
    struct nuvec_s e[8];
    struct nuvec4_s v[35];
    struct nuvec4_s *t;
    f32 s1, c1, s3, c3;
    f32 l, k, r;
    f32 d1;
    s32 i;

    s1 = NuSinApprox(0x2000);
    c1 = NuCosApprox(0x2000);
    s3 = NuSinApprox(0x6000);
    c3 = NuCosApprox(0x6000);
    if (*(u32 *)&box->p0.x & 1) {
        t = &v[3];
        v[7].x = box->p0.x;
        v[7].y = box->p0.y;
        v[7].z = box->p0.z;
        v[7].w = 1.0f;
        NuVec4MtxTransformVU0(&v[0], &v[7], &mtx);
        v[3].x = box->a.x + box->p0.x;
        v[3].y = box->a.y + box->p0.y;
        v[3].z = box->a.z + box->p0.z;
        v[3].w = 1.0f;
        NuVec4MtxTransformVU0(t, t, &mtx);
        e[0].x = v[3].x - v[0].x;
        e[0].y = v[3].y - v[0].y;
        e[0].z = v[3].z - v[0].z;
        v[4].x = box->b.x + box->p0.x;
        v[4].y = box->b.y + box->p0.y;
        v[4].z = box->b.z + box->p0.z;
        v[4].w = 1.0f;
        NuVec4MtxTransformVU0(&v[4], &v[4], &mtx);
        e[1].x = v[4].x - v[0].x;
        e[1].y = v[4].y - v[0].y;
        e[1].z = v[4].z - v[0].z;
        v[5].x = box->c.x + box->p0.x;
        v[5].y = box->c.y + box->p0.y;
        v[5].z = box->c.z + box->p0.z;
        v[5].w = 1.0f;
        NuVec4MtxTransformVU0(&v[5], &v[5], &mtx);
        e[2].x = v[5].x - v[0].x;
        e[2].y = v[5].y - v[0].y;
        e[2].z = v[5].z - v[0].z;
        v[1].x = e[0].x;
        v[1].y = e[0].y;
        v[1].z = e[0].z;
        NuVecCross((struct nuvec_s *)&v[2], &e[0], &dir);
        l = v[2].x * v[2].x + v[2].y * v[2].y + v[2].z * v[2].z;
        if (l > 0.0f) {
            l = NuFdiv(1.0f, NuFsqrt(l));
        }
        v[2].x *= l;
        v[2].y *= l;
        v[2].z *= l;
        k = v[1].x * v[1].x + v[1].y * v[1].y + v[1].z * v[1].z;
        if (k > 0.0f) {
            k = NuFdiv(1.0f, NuFsqrt(k));
        } else {
            k = 0.0f;
        }
        l = (v[2].x * e[1].x + v[2].y * e[1].y + v[2].z * e[1].z);
        d1 = (v[2].x * e[2].x + v[2].y * e[2].y + v[2].z * e[2].z);
        l = NuFsqrt(l * l + d1 * d1);
        for (i = 0; i < 10; i++) {
            lbl_8040AD54 += 2;
            switch (i) {
            case 0:
                v[3].x = v[1].x + v[2].x * l;
                v[3].y = v[1].y + v[2].y * l;
                v[3].z = v[1].z + v[2].z * l;
                v[4].x = -v[1].x + v[2].x * l;
                v[4].y = -v[1].y + v[2].y * l;
                v[4].z = -v[1].z + v[2].z * l;
                break;
            case 1:
                v[3].x = -v[1].x + v[2].x * l;
                v[3].y = -v[1].y + v[2].y * l;
                v[3].z = -v[1].z + v[2].z * l;
                v[4].x = -v[1].x + v[2].x * l * c1 - v[1].x * k * l * s1;
                v[4].y = -v[1].y + v[2].y * l * c1 - v[1].y * k * l * s1;
                v[4].z = -v[1].z + v[2].z * l * c1 - v[1].z * k * l * s1;
                break;
            case 2:
                v[3].x = -v[1].x + v[2].x * l * c1 - v[1].x * k * l * s1;
                v[3].y = -v[1].y + v[2].y * l * c1 - v[1].y * k * l * s1;
                v[3].z = -v[1].z + v[2].z * l * c1 - v[1].z * k * l * s1;
                v[4].x = -v[1].x - v[1].x * l;
                v[4].y = -v[1].y - v[1].y * l;
                v[4].z = -v[1].z - v[1].z * l;
                break;
            case 3:
                v[3].x = -v[1].x - v[1].x * l;
                v[3].y = -v[1].y - v[1].y * l;
                v[3].z = -v[1].z - v[1].z * l;
                v[4].x = -v[1].x + v[2].x * l * c3 - v[1].x * k * l * s3;
                v[4].y = -v[1].y + v[2].y * l * c3 - v[1].y * k * l * s3;
                v[4].z = -v[1].z + v[2].z * l * c3 - v[1].z * k * l * s3;
                break;
            case 4:
                v[3].x = -v[1].x + v[2].x * l * c3 - v[1].x * k * l * s3;
                v[3].y = -v[1].y + v[2].y * l * c3 - v[1].y * k * l * s3;
                v[3].z = -v[1].z + v[2].z * l * c3 - v[1].z * k * l * s3;
                v[4].x = -v[1].x - v[2].x * l;
                v[4].y = -v[1].y - v[2].y * l;
                v[4].z = -v[1].z - v[2].z * l;
                break;
            case 5:
                v[3].x = -v[1].x - v[2].x * l;
                v[3].y = -v[1].y - v[2].y * l;
                v[3].z = -v[1].z - v[2].z * l;
                v[4].x = v[1].x - v[2].x * l;
                v[4].y = v[1].y - v[2].y * l;
                v[4].z = v[1].z - v[2].z * l;
                break;
            case 6:
                v[3].x = v[1].x - v[2].x * l;
                v[3].y = v[1].y - v[2].y * l;
                v[3].z = v[1].z - v[2].z * l;
                v[4].x = v[1].x - v[2].x * l * c1 + v[1].x * k * l * s1;
                v[4].y = v[1].y - v[2].y * l * c1 + v[1].y * k * l * s1;
                v[4].z = v[1].z - v[2].z * l * c1 + v[1].z * k * l * s1;
                break;
            case 7:
                v[3].x = v[1].x - v[2].x * l * c1 + v[1].x * k * l * s1;
                v[3].y = v[1].y - v[2].y * l * c1 + v[1].y * k * l * s1;
                v[3].z = v[1].z - v[2].z * l * c1 + v[1].z * k * l * s1;
                v[4].x = v[1].x + v[1].x * l;
                v[4].y = v[1].y + v[1].y * l;
                v[4].z = v[1].z + v[1].z * l;
                break;
            case 8:
                v[3].x = v[1].x + v[1].x * l;
                v[3].y = v[1].y + v[1].y * l;
                v[3].z = v[1].z + v[1].z * l;
                v[4].x = v[1].x - v[2].x * l * c3 + v[1].x * k * l * s3;
                v[4].y = v[1].y - v[2].y * l * c3 + v[1].y * k * l * s3;
                v[4].z = v[1].z - v[2].z * l * c3 + v[1].z * k * l * s3;
                break;
            case 9:
                v[3].x = v[1].x - v[2].x * l * c3 + v[1].x * k * l * s3;
                v[3].y = v[1].y - v[2].y * l * c3 + v[1].y * k * l * s3;
                v[3].z = v[1].z - v[2].z * l * c3 + v[1].z * k * l * s3;
                v[4].x = v[1].x + v[2].x * l;
                v[4].y = v[1].y + v[2].y * l;
                v[4].z = v[1].z + v[2].z * l;
                break;
            }
            v[3].x += v[0].x;
            v[3].y += v[0].y;
            v[3].z += v[0].z;
            v[4].x += v[0].x;
            v[4].y += v[0].y;
            v[4].z += v[0].z;
            v[5].x = v[3].x + dir.x * 5.0f;
            v[5].y = v[3].y + dir.y * 5.0f;
            v[5].z = v[3].z + dir.z * 5.0f;
            v[6].x = v[4].x + dir.x * 5.0f;
            v[6].y = v[4].y + dir.y * 5.0f;
            v[6].z = v[4].z + dir.z * 5.0f;
            lbl_803145E0[lbl_80407CE4].x = v[3].x;
            lbl_803145E0[lbl_80407CE4].y = v[3].y;
            lbl_803145E0[lbl_80407CE4].z = v[3].z;
            lbl_803145E0[lbl_80407CE4 + 1].x = v[5].x;
            lbl_803145E0[lbl_80407CE4 + 1].y = v[5].y;
            lbl_803145E0[lbl_80407CE4 + 1].z = v[5].z;
            lbl_803145E0[lbl_80407CE4 + 2].x = v[6].x;
            lbl_803145E0[lbl_80407CE4 + 2].y = v[6].y;
            lbl_803145E0[lbl_80407CE4 + 2].z = v[6].z;
            lbl_803145E0[lbl_80407CE4 + 3].x = v[4].x;
            lbl_803145E0[lbl_80407CE4 + 3].y = v[4].y;
            lbl_803145E0[lbl_80407CE4 + 3].z = v[4].z;
            lbl_80407CE4 += 4;
        }
    } else {
        t = &v[3];
        v[7].x = box->p0.x;
        v[7].y = box->p0.y;
        v[7].z = box->p0.z;
        v[7].w = 1.0f;
        NuVec4MtxTransformVU0(&v[0], &v[7], &mtx);
        v[3].x = box->a.x + box->p0.x;
        v[3].y = box->a.y + box->p0.y;
        v[3].z = box->a.z + box->p0.z;
        v[3].w = 1.0f;
        NuVec4MtxTransformVU0(t, t, &mtx);
        e[0].x = v[3].x - v[0].x;
        e[0].y = v[3].y - v[0].y;
        e[0].z = v[3].z - v[0].z;
        v[4].x = box->b.x + box->p0.x;
        v[4].y = box->b.y + box->p0.y;
        v[4].z = box->b.z + box->p0.z;
        v[4].w = 1.0f;
        NuVec4MtxTransformVU0(&v[4], &v[4], &mtx);
        e[1].x = v[4].x - v[0].x;
        e[1].y = v[4].y - v[0].y;
        e[1].z = v[4].z - v[0].z;
        v[5].x = box->c.x + box->p0.x;
        v[5].y = box->c.y + box->p0.y;
        v[5].z = box->c.z + box->p0.z;
        v[5].w = 1.0f;
        NuVec4MtxTransformVU0(&v[5], &v[5], &mtx);
        e[2].x = v[5].x - v[0].x;
        e[2].y = v[5].y - v[0].y;
        e[2].z = v[5].z - v[0].z;
        v[1].x = e[0].x;
        v[1].y = e[0].y;
        v[1].z = e[0].z;
        NuVecCross((struct nuvec_s *)&v[2], &e[0], &dir);
        l = v[2].x * v[2].x + v[2].y * v[2].y + v[2].z * v[2].z;
        if (l > 0.0f) {
            l = NuFdiv(1.0f, NuFsqrt(l));
        }
        v[2].x *= l;
        v[2].y *= l;
        v[2].z *= l;
        k = v[1].x * v[1].x + v[1].y * v[1].y + v[1].z * v[1].z;
        if (k > 0.0f) {
            k = NuFdiv(1.0f, NuFsqrt(k));
        }
        l = (v[2].x * e[1].x + v[2].y * e[1].y + v[2].z * e[1].z);
        d1 = (v[2].x * e[2].x + v[2].y * e[2].y + v[2].z * e[2].z);
        l = NuFsqrt(l * l + d1 * d1);
        v[2].x *= l;
        v[2].y *= l;
        v[2].z *= l;
        for (i = 0; i < 4; i++) {
            switch (i) {
            case 0:
                v[3].x = v[1].x + v[2].x;
                v[3].y = v[1].y + v[2].y;
                v[3].z = v[1].z + v[2].z;
                v[4].x = -v[1].x + v[2].x;
                v[4].y = -v[1].y + v[2].y;
                v[4].z = -v[1].z + v[2].z;
                break;
            case 1:
                v[3].x = -v[1].x + v[2].x;
                v[3].y = -v[1].y + v[2].y;
                v[3].z = -v[1].z + v[2].z;
                v[4].x = -v[1].x - v[2].x;
                v[4].y = -v[1].y - v[2].y;
                v[4].z = -v[1].z - v[2].z;
                break;
            case 2:
                v[3].x = -v[1].x - v[2].x;
                v[3].y = -v[1].y - v[2].y;
                v[3].z = -v[1].z - v[2].z;
                v[4].x = v[1].x - v[2].x;
                v[4].y = v[1].y - v[2].y;
                v[4].z = v[1].z - v[2].z;
                break;
            case 3:
                v[3].x = v[1].x - v[2].x;
                v[3].y = v[1].y - v[2].y;
                v[3].z = v[1].z - v[2].z;
                v[4].x = v[1].x + v[2].x;
                v[4].y = v[1].y + v[2].y;
                v[4].z = v[1].z + v[2].z;
                break;
            }
            v[3].x += v[0].x;
            v[3].y += v[0].y;
            v[3].z += v[0].z;
            v[4].x += v[0].x;
            v[4].y += v[0].y;
            v[4].z += v[0].z;
            v[5].x = v[3].x + dir.x * 5.0f;
            v[5].y = v[3].y + dir.y * 5.0f;
            v[5].z = v[3].z + dir.z * 5.0f;
            v[6].x = v[4].x + dir.x * 5.0f;
            v[6].y = v[4].y + dir.y * 5.0f;
            v[6].z = v[4].z + dir.z * 5.0f;
            lbl_803145E0[lbl_80407CE4].x = v[3].x;
            lbl_803145E0[lbl_80407CE4].y = v[3].y;
            lbl_803145E0[lbl_80407CE4].z = v[3].z;
            lbl_803145E0[lbl_80407CE4 + 1].x = v[5].x;
            lbl_803145E0[lbl_80407CE4 + 1].y = v[5].y;
            lbl_803145E0[lbl_80407CE4 + 1].z = v[5].z;
            lbl_803145E0[lbl_80407CE4 + 2].x = v[6].x;
            lbl_803145E0[lbl_80407CE4 + 2].y = v[6].y;
            lbl_803145E0[lbl_80407CE4 + 2].z = v[6].z;
            lbl_803145E0[lbl_80407CE4 + 3].x = v[4].x;
            lbl_803145E0[lbl_80407CE4 + 3].y = v[4].y;
            lbl_803145E0[lbl_80407CE4 + 3].z = v[4].z;
            lbl_80407CE4 += 4;
        }
    }
}

/* fn_800225C4: e[0..2] are the transformed box edges, e[3] the ring point's
 * edge coordinates and e[4] the inverse edge lengths. The ring is
 * lbl_80337860 from fn_80022DE8. a and b are unused here. */
void fn_800225C4(struct camera_child30 *box, struct camera_record *rec, struct nuvec_s dir, f32 a, f32 b)
{
    struct nuvec_s e[8];
    struct nuvec4_s v[33];
    struct nuvec4_s *t;
    s32 i;

    t = &v[1];
    v[5].x = box->corners[0].x;
    v[5].y = box->corners[0].y;
    v[5].z = box->corners[0].z;
    v[5].w = 1.0f;
    NuVec4MtxTransformVU0(&v[0], &v[5], &rec->mtx);
    v[1].x = box->corners[2].x + box->corners[0].x;
    v[1].y = box->corners[2].y + box->corners[0].y;
    v[1].z = box->corners[2].z + box->corners[0].z;
    v[1].w = 1.0f;
    NuVec4MtxTransformVU0(t, t, &rec->mtx);
    e[0].x = v[1].x - v[0].x;
    e[0].y = v[1].y - v[0].y;
    e[0].z = v[1].z - v[0].z;
    v[2].x = box->corners[1].x + box->corners[0].x;
    v[2].y = box->corners[1].y + box->corners[0].y;
    v[2].z = box->corners[1].z + box->corners[0].z;
    v[2].w = 1.0f;
    NuVec4MtxTransformVU0(&v[2], &v[2], &rec->mtx);
    e[1].x = v[2].x - v[0].x;
    e[1].y = v[2].y - v[0].y;
    e[1].z = v[2].z - v[0].z;
    v[3].x = box->corners[3].x + box->corners[0].x;
    v[3].y = box->corners[3].y + box->corners[0].y;
    v[3].z = box->corners[3].z + box->corners[0].z;
    v[3].w = 1.0f;
    NuVec4MtxTransformVU0(&v[3], &v[3], &rec->mtx);
    e[2].x = v[3].x - v[0].x;
    e[2].y = v[3].y - v[0].y;
    e[2].z = v[3].z - v[0].z;
    e[4].x = 1.0f / NuFsqrt(e[0].x * e[0].x + e[0].y * e[0].y + e[0].z * e[0].z);
    e[4].y = 1.0f / NuFsqrt(e[1].x * e[1].x + e[1].y * e[1].y + e[1].z * e[1].z);
    e[4].z = 1.0f / NuFsqrt(e[2].x * e[2].x + e[2].y * e[2].y + e[2].z * e[2].z);
    for (i = 0; i < 8; i++) {
        lbl_8040AD54 += 2;
        e[3].x = (lbl_80337860[i].x * e[0].x + lbl_80337860[i].y * e[0].y + lbl_80337860[i].z * e[0].z) * e[4].x;
        e[3].y = (lbl_80337860[i].x * e[1].x + lbl_80337860[i].y * e[1].y + lbl_80337860[i].z * e[1].z) * e[4].y;
        e[3].z = (lbl_80337860[i].x * e[2].x + lbl_80337860[i].y * e[2].y + lbl_80337860[i].z * e[2].z) * e[4].z;
        v[1].x = e[3].x * e[0].x + e[3].y * e[1].x + e[3].z * e[2].x + v[0].x;
        v[1].y = e[3].x * e[0].y + e[3].y * e[1].y + e[3].z * e[2].y + v[0].y;
        v[1].z = e[3].x * e[0].z + e[3].y * e[1].z + e[3].z * e[2].z + v[0].z;
        e[3].x = (lbl_80337860[(i + 1) & 7].x * e[0].x + lbl_80337860[(i + 1) & 7].y * e[0].y + lbl_80337860[(i + 1) & 7].z * e[0].z) * e[4].x;
        e[3].y = (lbl_80337860[(i + 1) & 7].x * e[1].x + lbl_80337860[(i + 1) & 7].y * e[1].y + lbl_80337860[(i + 1) & 7].z * e[1].z) * e[4].y;
        e[3].z = (lbl_80337860[(i + 1) & 7].x * e[2].x + lbl_80337860[(i + 1) & 7].y * e[2].y + lbl_80337860[(i + 1) & 7].z * e[2].z) * e[4].z;
        v[2].x = e[3].x * e[0].x + e[3].y * e[1].x + e[3].z * e[2].x + v[0].x;
        v[2].y = e[3].x * e[0].y + e[3].y * e[1].y + e[3].z * e[2].y + v[0].y;
        v[2].z = e[3].x * e[0].z + e[3].y * e[1].z + e[3].z * e[2].z + v[0].z;
        v[3].x = v[1].x + dir.x;
        v[3].y = v[1].y + dir.y;
        v[3].z = v[1].z + dir.z;
        v[4].x = v[2].x + dir.x;
        v[4].y = v[2].y + dir.y;
        v[4].z = v[2].z + dir.z;
        lbl_803145E0[lbl_80407CE4].x = v[1].x;
        lbl_803145E0[lbl_80407CE4].y = v[1].y;
        lbl_803145E0[lbl_80407CE4].z = v[1].z;
        lbl_80407CE4++;
        lbl_803145E0[lbl_80407CE4].x = v[3].x;
        lbl_803145E0[lbl_80407CE4].y = v[3].y;
        lbl_803145E0[lbl_80407CE4].z = v[3].z;
        lbl_80407CE4++;
        lbl_803145E0[lbl_80407CE4].x = v[4].x;
        lbl_803145E0[lbl_80407CE4].y = v[4].y;
        lbl_803145E0[lbl_80407CE4].z = v[4].z;
        lbl_80407CE4++;
        lbl_803145E0[lbl_80407CE4].x = v[2].x;
        lbl_803145E0[lbl_80407CE4].y = v[2].y;
        lbl_803145E0[lbl_80407CE4].z = v[2].z;
        lbl_80407CE4++;
    }
}

/* fn_80022A88: faces holds one plane point per polygon (w = vertex count);
 * verts->w is the polygon count. The pointer is assigned twice so GCC loses
 * its frame base: the target reloads the scratch vectors after every
 * lbl_803145E0 store, which only happens when the base is unknown. */


void fn_80022A88(struct camera_record *rec, struct nuvec4_s *faces, struct nuvec4_s *verts, struct nuvec_s *dir)
{
    u8 buffer[0x400];
    struct numtx_s *m;
    struct nuvec4_s *v;
    struct nuvec4_s *p;
    s32 n, k, last;

    v = (struct nuvec4_s *)buffer;
    v = (struct nuvec4_s *)(((u32)v + 15) & ~15);
    m = (struct numtx_s *)v;
    NuCameraGetClippingMtx();
    v += 4;
    *m = rec->mtx;
    for (n = *(s32 *)&verts->w; n > 0; n--) {
        v[0] = *faces;
        v[0].w = 1.0f;
        fn_8000EE70(&v[0], &v[0], m);
        if (!(v[0].x * dir->x + v[0].y * dir->y + v[0].z * dir->z > 0.0f)) {
            p = verts;
            last = 8;
            for (k = *(s32 *)&faces->w; k > 0; k--, p++) {
                lbl_8040AD54 += 6;
                v[0] = *p;
                v[0].w = 1.0f;
                NuVec4MtxTransformVU0(&v[0], &v[0], m);
                if (k == 1) {
                    v[1] = *verts;
                } else {
                    v[1] = p[1];
                }
                v[1].w = 1.0f;
                NuVec4MtxTransformVU0(&v[1], &v[1], m);
                v[2].x = v[0].x + dir->x;
                v[2].y = v[0].y + dir->y;
                v[2].z = v[0].z + dir->z;
                v[3].x = v[1].x + dir->x;
                v[3].y = v[1].y + dir->y;
                v[3].z = v[1].z + dir->z;
                v[2].w = 1.0f;
                v[3].w = 1.0f;
                lbl_803145E0[lbl_80407CE4].x = v[0].x;
                lbl_803145E0[lbl_80407CE4].y = v[0].y;
                lbl_803145E0[lbl_80407CE4].z = v[0].z;
                lbl_80407CE4++;
                lbl_803145E0[lbl_80407CE4].x = v[2].x;
                lbl_803145E0[lbl_80407CE4].y = v[2].y;
                lbl_803145E0[lbl_80407CE4].z = v[2].z;
                lbl_80407CE4++;
                lbl_803145E0[lbl_80407CE4].x = v[3].x;
                lbl_803145E0[lbl_80407CE4].y = v[3].y;
                lbl_803145E0[lbl_80407CE4].z = v[3].z;
                lbl_80407CE4++;
                lbl_803145E0[lbl_80407CE4].x = v[1].x;
                lbl_803145E0[lbl_80407CE4].y = v[1].y;
                lbl_803145E0[lbl_80407CE4].z = v[1].z;
                lbl_80407CE4++;
            }
            v[last] = v[1];
        }
        verts += *(s32 *)&faces->w;
        faces++;
    }
}

void fn_8002303C(s32 mask, s32 value)
{
    struct nuvec_s direction;
    s32 previous;
    s32 i, j;
    s32 state;
    s32 active;
    f32 dx, dy, dz, distance, radius, near_radius;
    f32 a, b;

    active = 0;
    previous = -1;
    lbl_8040AD54 = 0;
    for (i = 0; i < lbl_80407CE0; i++) {
        if (lbl_803378CC[i].group != previous) previous = lbl_803378CC[i].group;
    }
    previous = -1;
    state = 1;
    for (i = 0; i < lbl_80407CE0; i++) {
        if (lbl_803378CC[i].group != previous) {
            previous = lbl_803378CC[i].group;
            if ((lbl_80337924[i * 0x1a] & mask) == value) {
                dx = lbl_803378CC[i].mtx._30 - lbl_80313268.mtx._30;
                dy = lbl_803378CC[i].mtx._31 - lbl_80313268.mtx._31;
                dz = lbl_803378CC[i].mtx._32 - lbl_80313268.mtx._32;
                distance = dx * dx + dy * dy + dz * dz;
                radius = lbl_803378CC[i].inner_radius * lbl_803378CC[i].inner_radius;
                a = 10000000.0f;
                b = -10000000.0f;
                near_radius = lbl_803378CC[i].outer_radius;
                active = 0;
                if (!(distance >= radius)) {
                    active = 1;
                    if (lbl_803378CC[i].vector_x >= 65536.0f) {
                        direction.x = 0.0f;
                        direction.y = -1.0f;
                        direction.z = direction.x;
                        fn_80022DE8(&direction);
                        direction.y = -4.0f;
                        if (state != 5 && lbl_803378CC[i].vector_z == 0.0f) state = 5;
                        a = lbl_803378CC[i].vector_y;
                        b = lbl_803378CC[i].vector_z;
                    } else {
                        direction.x = lbl_803378CC[i].vector_x;
                        direction.y = lbl_803378CC[i].vector_y;
                        direction.z = lbl_803378CC[i].vector_z;
                        if (state != 1) state = 1;
                        fn_80022DE8(&direction);
                        direction.x *= 4.0f;
                        direction.y *= 4.0f;
                        direction.z *= 4.0f;
                    }
                    if (active) {
                        if (distance > near_radius * near_radius) NuFsqrt(distance);
                    }
                }
            } else active = 0;
        }
        if (active) {
            if (lbl_803378CC[i].children40) {
                for (j = 0; j < lbl_803378CC[i].count40; j++) {
                    fn_80021784(&lbl_803378CC[i].children40[j], lbl_803378CC[i].mtx, direction);
                }
            }
            if (lbl_803378CC[i].children30) {
                for (j = 0; j < lbl_803378CC[i].count30; j++) {
                    fn_800225C4(&lbl_803378CC[i].children30[j], &lbl_803378CC[i], direction, a, b);
                }
            }
            if (lbl_803378CC[i].pairs) {
                for (j = 0; j < lbl_803378CC[i].count_pair; j++) {
                    fn_80022A88(&lbl_803378CC[i], lbl_803378CC[i].pairs->faces, lbl_803378CC[i].pairs->verts, &direction);
                }
            }
        }
    }
}
