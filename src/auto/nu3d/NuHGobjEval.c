#include "types.h"
#include "nu3dx/nuanim.h"
#include "numath/numath.h"

extern void fn_8000EB24(struct numtx_s *m, struct nuangvec_s *ang);
extern void fn_8000EB44(struct numtx_s *dst, struct numtx_s *a, struct numtx_s *b);

extern void NuAnimData2CalcTime(struct nuanimdata2_s *data, struct nuanimtime_s *out, f32 time);
extern s32 NuAnimData2GetEnd(struct nuanimdata2_s *anim);
extern f32 *NuAnimDataChunkDestroy(s32 n);
extern void *fn_80027D54();
extern void *memset(void *p, s32 c, u32 n);

struct nujointdata_s {
    struct numtx_s orient;   /* 0x00 */
    u8 pad40[0x10];
    u8 parent_ix;            /* 0x50 */
    u8 pad51[0xF];
};

struct nuhgobj_s {
    u8 pad00[0x14];
    struct nujointdata_s *joints; /* 0x14 */
    struct numtx_s *T;            /* 0x18 */
    u8 pad1C[4];
    u8 *joint_ixs;                /* 0x20 */
    u8 pad24[0x58];
    u8 num_joints;                /* 0x7C */
    u8 num_joint_ixs;             /* 0x7D */
};

typedef void (*NuHGobjRootFn)(struct numtx_s *mtx, void *data, struct nuvec_s *t, struct nuvec_s *r,
                              struct nuvec_s *loc, f32 f);
void NuHGobjEvalAnim2Root(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim, f32 time, s32 njanims,
                          struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array, NuHGobjRootFn rootfn,
                          void *rootdata);
void NuHGobjEvalAnimBlend2Root(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim1, f32 time1,
                               struct nuanimdata2_s *anim2, f32 time2, f32 blend, s32 njanims,
                               struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array, NuHGobjRootFn rootfn,
                               void *rootdata);
extern void NuAnimBuffEvaluate(s32 buffer, struct nuanimdata2_s *anim, f32 time, s32 first, f32 blend, s32 start,
                               struct nuhgobj_s *hgobj, struct nuvec_s *root);
extern void fn_8001AF8C(s32 a, struct nuhgobj_s *hgobj, s32 njanims, struct NUJOINTANIM_s *janim);
extern void NuAnimData2Fixup(s32 a, struct nuhgobj_s *hgobj, struct numtx_s *mtx_array, struct nuanimdata2_s *anim1,
                             struct nuanimdata2_s *anim2, NuHGobjRootFn rootfn, void *buf, void *rootdata);
extern void NuAnimCurve2SetApplyToJointTransLoc();
extern void NuAnimCurve2SetApplyToJoint(struct nuanimcurve2_s *curves, char *types, s32 flags,
                                        struct nuanimtime_s *atime, struct nujointdata_s *joint,
                                        struct nuvec_s *scale, struct nuvec_s *pscale, struct numtx_s *m,
                                        struct NUJOINTANIM_s *janim);
extern void NuAnimData2Relocate(s32 a, struct nuhgobj_s *hgobj, struct numtx_s *mtx_array, struct nuanimdata2_s *anim1,
                                struct nuanimdata2_s *anim2, NuHGobjRootFn rootfn, void *buf, void *rootdata,
                                struct nuvec_s *root1, struct nuvec_s *root2, f32 blend);
extern void NuAnimCurve2SetApplyBlendToJoint2TransLoc();
extern void NuAnimCurve2SetApplyBlendToJoint2(struct nuanimcurve2_s *curves1, char *types1, s32 flags1,
                                              struct nuanimtime_s *atime1, struct nuanimcurve2_s *curves2,
                                              char *types2, s32 flags2, struct nuanimtime_s *atime2, f32 blend,
                                              struct nujointdata_s *joint, struct nuvec_s *scale,
                                              struct nuvec_s *pscale, struct numtx_s *m,
                                              struct NUJOINTANIM_s *janim);
extern s32 lbl_80407C20;
extern struct numtx_s lbl_801B0F08;


void NuHGobjEval(struct nuhgobj_s *hgobj, s32 njanims, struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array)
{
    struct nuvec_s scale[256];
    struct NUJOINTANIM_s *janims[256];
    u8 i;
    struct numtx_s *WT;
    struct numtx_s *T;
    struct NUJOINTANIM_s *offset;
    u8 joint_ix;

    scale[255].z = scale[255].y = scale[255].x = 1.0f;
    if (njanims != 0) {
        memset(janims, 0, hgobj->num_joints * 4);
        for (i = 0; i < njanims; i++) {
            if (janim[i].joint_id < hgobj->num_joint_ixs) {
                joint_ix = hgobj->joint_ixs[janim[i].joint_id];
                if (joint_ix != 0xFF) {
                    janims[joint_ix] = &janim[i];
                }
            }
        }
    }
    for (i = 0; i < hgobj->num_joints; i++) {
        T = &mtx_array[i];
        offset = njanims != 0 ? janims[i] : NULL;
        if (offset != NULL) {
            struct nuangvec_s rf;
            struct numtx_s LOCAL_T;
            struct numtx_s offset_mtx;

            LOCAL_T = hgobj->T[i];
            WT = &LOCAL_T;
            rf.x = offset->rx * 10430.378f;
            rf.y = offset->ry * 10430.378f;
            rf.z = offset->rz * 10430.378f;
            fn_8000EB24(&offset_mtx, &rf);
            NuMtxTranslate(&offset_mtx, (struct nuvec_s *)&offset->tx);
            fn_8000EB44(WT, &offset_mtx, WT);
        } else {
            WT = &hgobj->T[i];
        }
        if (hgobj->joints[i].parent_ix == 0xFF) {
            *T = *WT;
        } else {
            fn_8000EB44(T, WT, &mtx_array[hgobj->joints[i].parent_ix]);
        }
    }
}

/* Retail .rodata keeps the constant pools, __FILE__ paths and messages of functions
 * the linker dead-stripped from these source files; nothing in the DOL references
 * the unk_ placeholders below (docs/build_config.md, "Stripped constant pools"). */
static const u32 unk_8019037C[1] __attribute__((section(".rodata"))) = { 0x3F800000 };
static const char unk_80190380[] = "../nu2api.legogc/nu3d/ngc_obj/../NuHGobjEval.c";
static const char unk_801903B0[] = "Function 'NuHGobjEvalAnim' No Longer supported!";

void NuHGobjEvalAnim2Root(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim, f32 time, s32 njanims,
                          struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array, NuHGobjRootFn rootfn,
                          void *rootdata)
{
    struct numtx_s m;
    struct nuvec_s scales[256];
    struct nuanimtime_s atime;
    struct NUJOINTANIM_s *janims[256];
    struct nuvec_s t;
    struct nuvec_s loc;
    struct nuvec_s root_t;
    u8 i;
    u8 joint_ix;
    struct numtx_s *T;
    struct NUJOINTANIM_s *offset;
    struct nuanimcurve2_s *curves;
    char *types;
    struct numtx_s *mp;
    s32 flags;
    u8 parent;

    if (*(s32 *)anim == 0x414E4934) {
        NuAnimBuffEvaluate(0, anim, time, 1, 0.0f, 0, hgobj, NULL);
        if (njanims != 0) {
            fn_8001AF8C(0, hgobj, njanims, janim);
        }
        NuAnimData2Fixup(0, hgobj, mtx_array, anim, anim, rootfn, &root_t, rootdata);
        return;
    }
    if (hgobj->num_joints > lbl_80407C20) {
        lbl_80407C20 = hgobj->num_joints;
    }
    scales[255].z = scales[255].y = scales[255].x = 1.0f;
    NuAnimData2CalcTime(anim, &atime, time);
    if (njanims != 0) {
        memset(janims, 0, hgobj->num_joints * 4);
        for (i = 0; i < njanims; i++) {
            if (janim[i].joint_id < hgobj->num_joint_ixs) {
                joint_ix = hgobj->joint_ixs[janim[i].joint_id];
                if (joint_ix != 0xFF) {
                    janims[joint_ix] = &janim[i];
                }
            }
        }
    }
    for (i = 0; i < hgobj->num_joints; i++) {
        T = &mtx_array[i];
        if (i >= anim->nnodes) {
            *T = lbl_801B0F08;
            continue;
        }
        offset = njanims != 0 ? janims[i] : NULL;
        curves = &anim->curves[anim->ncurves * i];
        types = anim->curveflags + anim->ncurves * i;
        flags = anim->curvesetflags[i];
        if (i == 0) {
            NuAnimCurve2SetApplyToJointTransLoc(curves, types, flags, &atime, &hgobj->joints[i], &scales[i],
                                                &scales[hgobj->joints[i].parent_ix], &m, offset, &t, &loc);
        } else {
            NuAnimCurve2SetApplyToJoint(curves, types, flags, &atime, &hgobj->joints[i], &scales[i],
                                        &scales[hgobj->joints[i].parent_ix], &m, offset);
        }
        mp = &m;
        if (i == 0 && rootfn != NULL) {
            rootfn(mp, rootdata, &t, NULL, &loc, 0.0f);
        }
        parent = hgobj->joints[i].parent_ix;
        if (parent == 0xFF) {
            if ((flags & 0x40) != 0) {
                /* GC addresses scales[i] from &m (scales sits directly after m). */
                ((struct nuvec_s *)(&m + 1))[i].z = ((struct nuvec_s *)(&m + 1))[i].y =
                    ((struct nuvec_s *)(&m + 1))[i].x = 1.0f;
            }
            *T = *mp;
        } else {
            fn_8000EB44(T, mp, &mtx_array[parent]);
        }
    }
}

static const u32 unk_801903E8[4] = { 0, 0x3F800000, 0, 0x3F800000 };

f32 **NuHGobjEvalDwa2(s32 nlayers, s16 *layers, struct nuanimdata2_s *vtxanim, f32 vtxtime)
{
    struct nuanimtime_s atime;
    f32 **dwa;
    s32 i;
    s32 j;
    s32 node;
    s32 ncurves;
    f32 *w;
    struct nuanimcurve2_s *curves;
    char *flags;
    u32 size;

    dwa = NULL;
    if (vtxanim != NULL && nlayers != 0) {
        NuAnimData2CalcTime(vtxanim, &atime, vtxtime);
        if (layers == NULL) {
            nlayers = 1;
        }
        size = (u32)(nlayers + 3) >> 2;
        dwa = fn_80027D54(nlayers);
        memset(dwa, 0, size);
        if (dwa != NULL) {
            for (i = 0; i < nlayers; i++) {
                if (layers != NULL) {
                    node = layers[i];
                    if (node < 0) {
                        continue;
                    }
                    if (node >= NuAnimData2GetEnd(vtxanim)) {
                        dwa[i] = NuAnimDataChunkDestroy(0);
                        continue;
                    }
                } else {
                    node = 0;
                }
                ncurves = vtxanim->ncurves;
                w = NuAnimDataChunkDestroy(ncurves);
                dwa[i] = w;
                curves = vtxanim->curves + vtxanim->ncurves * node;
                flags = vtxanim->curveflags + vtxanim->ncurves * node;
                if (w != NULL) {
                    for (j = 0; j < ncurves; j++) {
                        w[j + 6] = flags[j] == 0 ? curves[j].data.constant : NuAnimCurve2CalcVal(&curves[j], &atime, flags[j]);
                    }
                }
            }
        }
    }
    return dwa;
}

f32 **NuHGobjEvalDwaBlend2(s32 nlayers, s16 *layers, struct nuanimdata2_s *vtxanim1, f32 vtxtime1,
                           struct nuanimdata2_s *vtxanim2, f32 vtxtime2, f32 blend)
{
    struct nuanimtime_s atime1;
    struct nuanimtime_s atime2;
    f32 **dwa;
    s32 i;
    s32 j;
    s32 node;
    s32 n1;
    s32 n2;
    s32 n;
    f32 *w;
    struct nuanimcurve2_s *curves1;
    struct nuanimcurve2_s *curves2;
    char *flags1;
    char *flags2;
    u32 size;
    f32 v1;
    f32 v2;

    dwa = NULL;
    curves1 = NULL;
    curves2 = NULL;
    flags1 = NULL;
    flags2 = NULL;
    if ((vtxanim1 != NULL || vtxanim2 != NULL) && nlayers != 0) {
        if (vtxanim1 != NULL) {
            NuAnimData2CalcTime(vtxanim1, &atime1, vtxtime1);
        }
        if (vtxanim2 != NULL) {
            NuAnimData2CalcTime(vtxanim2, &atime2, vtxtime2);
        }
        if (layers == NULL) {
            nlayers = 1;
        }
        size = (u32)(nlayers + 3) >> 2;
        dwa = fn_80027D54(nlayers);
        memset(dwa, 0, size);
        if (dwa != NULL) {
            for (i = 0; i < nlayers; i++) {
                if (layers != NULL) {
                    node = layers[i];
                    if (node < 0) {
                        continue;
                    }
                } else {
                    node = 0;
                }
                if (vtxanim1 != NULL && node < NuAnimData2GetEnd(vtxanim1)) {
                    n1 = vtxanim1->ncurves;
                    curves1 = vtxanim1->curves + n1 * node;
                    flags1 = vtxanim1->curveflags + n1 * node;
                } else {
                    n1 = 0;
                }
                if (vtxanim2 != NULL && node < NuAnimData2GetEnd(vtxanim2)) {
                    n2 = vtxanim2->ncurves;
                    curves2 = vtxanim2->curves + n2 * node;
                    flags2 = vtxanim2->curveflags + n2 * node;
                } else {
                    n2 = 0;
                }
                n = n2;
                if (n < n1) {
                    n = n1;
                }
                w = NuAnimDataChunkDestroy(n);
                dwa[i] = w;
                if (w != NULL) {
                    for (j = 0; j < n; j++) {
                        if (j < n1) {
                            if (flags1[j] == 0) {
                                v1 = curves1[j].data.constant;
                            } else {
                                v1 = NuAnimCurve2CalcVal(&curves1[j], &atime1, flags1[j]);
                            }
                        } else {
                            v1 = 0.0f;
                        }
                        if (j < n2) {
                            if (flags2[j] == 0) {
                                v2 = curves2[j].data.constant;
                            } else {
                                v2 = NuAnimCurve2CalcVal(&curves2[j], &atime2, flags2[j]);
                            }
                        } else {
                            v2 = 0.0f;
                        }
                        w[j + 6] = v2 * blend + v1 * (1.0f - blend);
                    }
                }
            }
        }
    }
    return dwa;
}

static const u32 unk_80190400[1] __attribute__((section(".rodata"))) = { 0x3F800000 };

/* ANI4 path; inlined (its locals sit above the main path's on the GC frame). */
static inline void NuHGobjEvalAnimBlend2Root_3(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim1, f32 time1,
                                               struct nuanimdata2_s *anim2, f32 time2, f32 blend, s32 njanims,
                                               struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array,
                                               NuHGobjRootFn rootfn, void *rootdata)
{
    struct nuvec_s root_t;
    struct nuvec_s root1;
    struct nuvec_s root2;

    NuAnimBuffEvaluate(0, anim1, time1, 1, 0.0f, 0, hgobj, &root1);
    NuAnimBuffEvaluate(0, anim2, time2, 0, blend, 0, hgobj, &root2);
    if (njanims != 0) {
        fn_8001AF8C(0, hgobj, njanims, janim);
    }
    NuAnimData2Relocate(0, hgobj, mtx_array, anim1, anim2, rootfn, &root_t, rootdata, &root1, &root2, blend);
}

void NuHGobjEvalAnimBlend2Root(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim1, f32 time1,
                               struct nuanimdata2_s *anim2, f32 time2, f32 blend, s32 njanims,
                               struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array, NuHGobjRootFn rootfn,
                               void *rootdata)
{
    struct numtx_s m;
    struct nuvec_s scales[256];
    struct nuanimtime_s atime1;
    struct nuanimtime_s atime2;
    struct NUJOINTANIM_s *janims[256];
    struct nuvec_s t;
    struct nuvec_s r;
    struct nuvec_s loc;
    u8 i;
    u8 joint_ix;
    struct numtx_s *T;
    struct NUJOINTANIM_s *offset;
    struct nuanimcurve2_s *curves1;
    char *types1;
    s32 flags1;
    struct nuanimcurve2_s *curves2;
    char *types2;
    s32 flags2;
    struct numtx_s *mp;
    s32 is_root;
    u8 parent;

    if (*(s32 *)anim1 == 0x414E4934) {
        NuHGobjEvalAnimBlend2Root_3(hgobj, anim1, time1, anim2, time2, blend, njanims, janim, mtx_array, rootfn,
                                    rootdata);
        return;
    }
    if (hgobj->num_joints > lbl_80407C20) {
        lbl_80407C20 = hgobj->num_joints;
    }
    scales[255].z = scales[255].y = scales[255].x = 1.0f;
    NuAnimData2CalcTime(anim1, &atime1, time1);
    NuAnimData2CalcTime(anim2, &atime2, time2);
    if (njanims != 0) {
        memset(janims, 0, hgobj->num_joints * 4);
        for (i = 0; i < njanims; i++) {
            if (janim[i].joint_id < hgobj->num_joint_ixs) {
                joint_ix = hgobj->joint_ixs[janim[i].joint_id];
                if (joint_ix != 0xFF) {
                    janims[joint_ix] = &janim[i];
                }
            }
        }
    }
    for (i = 0; i < hgobj->num_joints; i++) {
        T = &mtx_array[i];
        if (i >= anim1->nnodes || i >= anim2->nnodes) {
            *T = lbl_801B0F08;
            continue;
        }
        offset = njanims != 0 ? janims[i] : NULL;
        curves1 = &anim1->curves[anim1->ncurves * i];
        types1 = anim1->curveflags + anim1->ncurves * i;
        flags1 = anim1->curvesetflags[i];
        curves2 = &anim2->curves[anim2->ncurves * i];
        types2 = anim2->curveflags + anim2->ncurves * i;
        flags2 = anim2->curvesetflags[i];
        if (i == 0) {
            NuAnimCurve2SetApplyBlendToJoint2TransLoc(curves1, types1, flags1, &atime1, curves2, types2, flags2,
                                                      &atime2, blend, &hgobj->joints[i], &scales[i],
                                                      &scales[hgobj->joints[i].parent_ix], &m, offset, &t, &r,
                                                      &loc);
        } else {
            NuAnimCurve2SetApplyBlendToJoint2(curves1, types1, flags1, &atime1, curves2, types2, flags2, &atime2,
                                              blend, &hgobj->joints[i], &scales[i],
                                              &scales[hgobj->joints[i].parent_ix], &m, offset);
        }
        mp = &m;
        if (i == 0 && rootfn != NULL) {
            rootfn(mp, rootdata, &t, &r, &loc, blend);
        }
        parent = hgobj->joints[i].parent_ix;
        is_root = parent == 0xFF;
        if (is_root) {
            if ((flags1 & 0x40) != 0) {
                ((struct nuvec_s *)(&m + 1))[i].z = ((struct nuvec_s *)(&m + 1))[i].y =
                    ((struct nuvec_s *)(&m + 1))[i].x = 1.0f;
            }
            *T = *mp;
        } else {
            fn_8000EB44(T, mp, &mtx_array[parent]);
        }
    }
}

static const u32 unk_8019040C[6] = { 0, 0x3F800000, 0, 0, 0, 0 };

/* NuHGobjEval.c ends here; 0x80190424 opens the next source file with its VuMath.h strings. */
#include "numath/VuMath.h"

void NuHGobjEvalAnim2(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim, f32 time, s32 njanims,
                      struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array)
{
    NuHGobjEvalAnim2Root(hgobj, anim, time, njanims, janim, mtx_array, NULL, NULL);
}

void NuHGobjEvalAnimBlend2(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim1, f32 time1,
                           struct nuanimdata2_s *anim2, f32 time2, f32 blend, s32 njanims,
                           struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array)
{
    NuHGobjEvalAnimBlend2Root(hgobj, anim1, time1, anim2, time2, blend, njanims, janim, mtx_array, NULL, NULL);
}

/* Leading matrix of the global camera (nucam_s in ngc/nucamera.c). */
struct nucamhdr_s {
    struct numtx_s mtx;
};
extern s32 lbl_8040AD30;
extern struct numtx_s lbl_8020BF98;
extern struct nucamhdr_s lbl_80313268;
extern struct nuvec_s lbl_801B15D8[8];
extern struct nuvec_s lbl_80313250;
extern struct nuvec_s lbl_8031325C;

void NuAnimCurveSetEval(struct nuvec_s *size)
{
    struct nuvec_s s;
    struct nuvec_s c;
    struct nuvec_s min;
    struct nuvec_s max;
    struct nuvec_s pts[8];
    s32 i;

    if (lbl_8040AD30 == 0) {
        lbl_8020BF98 = lbl_80313268.mtx;
    }
    s = *size;
    pts[0].x = max.x = min.x = lbl_8020BF98._00 * s.x * lbl_801B15D8[0].x + lbl_8020BF98._30 + lbl_8020BF98._10 * s.y * lbl_801B15D8[0].y + lbl_8020BF98._20 * s.z * lbl_801B15D8[0].z;
    pts[0].y = max.y = min.y = lbl_8020BF98._01 * s.x * lbl_801B15D8[0].x + lbl_8020BF98._31 + lbl_8020BF98._11 * s.y * lbl_801B15D8[0].y + lbl_8020BF98._21 * s.z * lbl_801B15D8[0].z;
    pts[0].z = max.z = min.z = lbl_8020BF98._02 * s.x * lbl_801B15D8[0].x + lbl_8020BF98._32 + lbl_8020BF98._12 * s.y * lbl_801B15D8[0].y + lbl_8020BF98._22 * s.z * lbl_801B15D8[0].z;
    for (i = 1; i < 8; i++) {
        pts[i].x = lbl_8020BF98._00 * s.x * lbl_801B15D8[i].x + lbl_8020BF98._30 + lbl_8020BF98._10 * s.y * lbl_801B15D8[i].y + lbl_8020BF98._20 * s.z * lbl_801B15D8[i].z;
        pts[i].y = lbl_8020BF98._01 * s.x * lbl_801B15D8[i].x + lbl_8020BF98._31 + lbl_8020BF98._11 * s.y * lbl_801B15D8[i].y + lbl_8020BF98._21 * s.z * lbl_801B15D8[i].z;
        pts[i].z = lbl_8020BF98._02 * s.x * lbl_801B15D8[i].x + lbl_8020BF98._32 + lbl_8020BF98._12 * s.y * lbl_801B15D8[i].y + lbl_8020BF98._22 * s.z * lbl_801B15D8[i].z;
        if (pts[i].x < min.x) min.x = pts[i].x;
        if (pts[i].y < min.y) min.y = pts[i].y;
        if (pts[i].z < min.z) min.z = pts[i].z;
        if (pts[i].x > max.x) max.x = pts[i].x;
        if (pts[i].y > max.y) max.y = pts[i].y;
        if (pts[i].z > max.z) max.z = pts[i].z;
    }
    c.x = (max.x + min.x) * 0.5f - s.x * 0.5f;
    c.y = (max.y + min.y) * 0.5f - s.y * 0.5f;
    c.z = (max.z + min.z) * 0.5f - s.z * 0.5f;
    lbl_80313250 = c;
    lbl_8031325C = s;
}

struct nurndr_vtxin_s {
    f32 x, y, z;
    u8 pad0c[0xc];
    s32 colour;
    f32 u, v;
};
extern s32 fn_80025FA0(struct nurndr_vtxin_s *verts, void *mtl, struct numtx_s *mtx);

void NuAnimCurveSetLoad(struct nuvec_s *a, struct nuvec_s *b, struct numtx_s *mtx, s32 colour)
{
    struct nurndr_vtxin_s c[8];
    struct nurndr_vtxin_s line[2];

    c[0].x = a->x;
    c[0].y = a->y;
    c[0].z = a->z;
    c[1].x = b->x;
    c[1].y = a->y;
    c[1].z = a->z;
    c[2].x = b->x;
    c[2].y = a->y;
    c[2].z = b->z;
    c[3].x = a->x;
    c[3].y = a->y;
    c[3].z = b->z;
    c[4].x = a->x;
    c[4].y = b->y;
    c[4].z = a->z;
    c[5].x = b->x;
    c[5].y = b->y;
    c[5].z = a->z;
    c[6].x = b->x;
    c[6].y = b->y;
    c[6].z = b->z;
    c[7].x = a->x;
    c[7].y = b->y;
    c[7].z = b->z;
    c[0].colour = colour;
    c[1].colour = colour;
    c[2].colour = colour;
    c[3].colour = colour;
    c[4].colour = colour;
    c[5].colour = colour;
    c[6].colour = colour;
    c[7].colour = colour;
#define LINE(i, j)                          \
    line[0] = c[i];                         \
    line[1] = c[j];                         \
    line[0].v = 0.0f;                       \
    line[1].v = 1.0f;                       \
    line[0].u = 0.0f;                       \
    line[1].u = 1.0f;                       \
    fn_80025FA0(line, NULL, mtx)
    LINE(0, 1);
    LINE(1, 2);
    LINE(2, 3);
    LINE(3, 0);
    LINE(4, 5);
    LINE(5, 6);
    LINE(6, 7);
    LINE(7, 4);
    LINE(0, 4);
    LINE(1, 5);
    LINE(2, 6);
    LINE(3, 7);
#undef LINE
}

/* Nuanimbuffer.c starts at 0x8019047C; VuMath.h's stub is already defined above, so its
 * strings are carried verbatim. */
static const u32 unk_8019047C[1] __attribute__((section(".rodata"))) = { 0 };
static const char unk_80190480[] = "../nu2api.legogc/numath/ngc/VuMath.h";
static const char unk_801904A8[] = "VuMtxInitRot NOT fully implemented!";

extern s32 lbl_80407C28;
extern s32 lbl_80407C30;

void fn_8001AF60(s32 v)
{
    lbl_80407C30 = v;
    lbl_80407C28 = 1;
}

void fn_8001AF70(void)
{
    lbl_80407C28 = 0;
}

s32 fn_8001AF7C(void)
{
    return lbl_80407C28;
}

s32 fn_8001AF84(void)
{
    return lbl_80407C30;
}

extern void *lbl_80407C38;

void fn_8001AF8C(s32 buffer, struct nuhgobj_s *hgobj, s32 count, struct NUJOINTANIM_s *joint)
{
    s32 angle[3];
    u8 id, flags, ix;
    s32 i, rot;
    f32 *out;

    if (buffer == 0) {
        buffer = (s32)lbl_80407C38;
    }
    for (i = 0; i < count; i++, joint++) {
        id = joint->joint_id;
        if (id >= hgobj->num_joint_ixs) continue;
        ix = hgobj->joint_ixs[id];
        if (ix == 0xff) continue;
        out = (f32 *)((u8 *)*(void **)((u8 *)buffer + 8) + ix * 0x24);
        flags = joint->flags;
        if (flags & 1) {
            out[3] += joint->rx;
            out[4] += joint->ry;
            out[5] += joint->rz;
            rot = (s32)(out[3] * 10430.378f);
            angle[0] = rot;
            angle[1] = (s32)(out[4] * 10430.378f);
            angle[2] = (s32)(out[5] * 10430.378f);
            if (flags & 8) {
                angle[0] = (u16)rot;
                if (angle[0] > 0x7fff) angle[0] -= 0x10000;
                if (angle[0] > joint->max_rx) angle[0] = joint->max_rx;
                else if (angle[0] < joint->min_rx) angle[0] = joint->min_rx;
            }
            if (flags & 0x10) {
                angle[1] = (u16)angle[1];
                if (angle[1] > 0x7fff) angle[1] -= 0x10000;
                if (angle[1] > joint->max_ry) angle[1] = joint->max_ry;
                else if (angle[1] < joint->min_ry) angle[1] = joint->min_ry;
            }
            if (flags & 0x20) {
                angle[2] = (u16)angle[2];
                if (angle[2] > 0x7fff) angle[2] -= 0x10000;
                if (angle[2] > joint->max_rz) angle[2] = joint->max_rz;
                else if (angle[2] < joint->min_rz) angle[2] = joint->min_rz;
            }
            out[3] = (f32)angle[0] * 9.58738e-5f;
            out[4] = (f32)angle[1] * 9.58738e-5f;
            out[5] = (f32)angle[2] * 9.58738e-5f;
        }
        if (flags & 4) {
            out[6] += joint->sx;
            out[7] += joint->sy;
            out[8] += joint->sz;
        }
        if (flags & 2) {
            out[0] += joint->tx;
            out[1] += joint->ty;
            out[2] += joint->tz;
        }
    }
}


static const char unk_801904DC[] = "../nu2api.legogc/nu3d/ngc_obj/../Nuanimbuffer.c";
static const char unk_8019050C[] = "Function 'NuHGobjEvalAnimBlend2Root_buff' No Longer supported!";
static const char unk_8019054C[] = "Function 'NuHGobjEvalAnim2Rootx' No Longer supported!";
static const u32 unk_80190584[10] = { 0, 0x3F800000, 0x3F800000, 0, 0x40490FDB, 0x40C90FDB, 0x3F800000, 0, 0x40490FDB, 0x40C90FDB };
