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

extern void NuHGobjEvalAnimBlend2Root(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);

typedef void (*NuHGobjRootFn)(struct numtx_s *mtx, void *data, struct nuvec_s *t, struct nuvec_s *r,
                              struct nuvec_s *loc, f32 f);
void NuHGobjEvalAnim2Root(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim, f32 time, s32 njanims,
                          struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array, NuHGobjRootFn rootfn,
                          void *rootdata);
extern void NuAnimBuffEvaluate(s32 a, struct nuanimdata2_s *anim, s32 b, s32 c, struct nuhgobj_s *hgobj, s32 d,
                               f32 time, f32 blend);
extern void fn_8001AF8C(s32 a, struct nuhgobj_s *hgobj, s32 njanims, struct NUJOINTANIM_s *janim);
extern void NuAnimData2Fixup(s32 a, struct nuhgobj_s *hgobj, struct numtx_s *mtx_array, struct nuanimdata2_s *anim1,
                             struct nuanimdata2_s *anim2, NuHGobjRootFn rootfn, void *buf, void *rootdata);
extern void NuAnimCurve2SetApplyToJointTransLoc();
extern void NuAnimCurve2SetApplyToJoint(struct nuanimcurve2_s *curves, char *types, s32 flags,
                                        struct nuanimtime_s *atime, struct nujointdata_s *joint,
                                        struct nuvec_s *scale, struct nuvec_s *pscale, struct numtx_s *m,
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

void NuHGobjEvalAnim2(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim, f32 time, s32 njanims,
                      struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array)
{
    NuHGobjEvalAnim2Root(hgobj, anim, time, njanims, janim, mtx_array, NULL, NULL);
}

void NuHGobjEvalAnimBlend2(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    NuHGobjEvalAnimBlend2Root(a, b, c, d, e, f, 0, 0);
}

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

/* NuHGobjEvalAnim2Root draft: best 83/251 (frame 0x10 too large, register allocation); not built. */
#if 0
void NuHGobjEvalAnim2Root(struct nuhgobj_s *hgobj, struct nuanimdata2_s *anim, f32 time, s32 njanims,
                          struct NUJOINTANIM_s *janim, struct numtx_s *mtx_array, NuHGobjRootFn rootfn,
                          void *rootdata)
{
    struct numtx_s m;
    struct nuvec_s scale[256];
    struct nuanimtime_s atime;
    struct NUJOINTANIM_s *janims[256];
    struct nuvec_s t;
    struct nuvec_s loc;
    u8 buf[0x14];
    u8 i;
    u8 joint_ix;
    struct numtx_s *T;
    struct NUJOINTANIM_s *offset;
    struct nuanimcurve2_s *curves;
    char *types;
    s32 flags;
    u8 parent;

    if (*(s32 *)anim == 0x414E4934) {
        NuAnimBuffEvaluate(0, anim, 1, 0, hgobj, 0, time, 0.0f);
        if (njanims != 0) {
            fn_8001AF8C(0, hgobj, njanims, janim);
        }
        NuAnimData2Fixup(0, hgobj, mtx_array, anim, anim, rootfn, buf, rootdata);
        return;
    }
    if (hgobj->num_joints > lbl_80407C20) {
        lbl_80407C20 = hgobj->num_joints;
    }
    scale[255].z = scale[255].y = scale[255].x = 1.0f;
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
            NuAnimCurve2SetApplyToJointTransLoc(curves, types, flags, &atime, &hgobj->joints[i], &scale[i],
                                                &scale[hgobj->joints[i].parent_ix], &m, offset, &t, &loc);
            if (rootfn != NULL) {
                rootfn(&m, rootdata, &t, NULL, &loc, 0.0f);
            }
        } else {
            NuAnimCurve2SetApplyToJoint(curves, types, flags, &atime, &hgobj->joints[i], &scale[i],
                                        &scale[hgobj->joints[i].parent_ix], &m, offset);
        }
        parent = hgobj->joints[i].parent_ix;
        if (parent == 0xFF) {
            if ((flags & 0x40) != 0) {
                scale[i].x = scale[i].z = scale[i].y = 1.0f;
            }
            *T = m;
        } else {
            fn_8000EB44(T, &m, &mtx_array[parent]);
        }
    }
}
#endif

/* fn_8001AF8C draft: best 160/168; conversion constants use opposite f6/f7 registers. */
#if 0
extern const f32 lbl_801904CC[];
extern const f32 lbl_801904D8[];
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
            rot = (s32)(out[3] * lbl_801904CC[0]);
            angle[0] = rot;
            angle[1] = (s32)(out[4] * lbl_801904CC[0]);
            angle[2] = (s32)(out[5] * lbl_801904CC[0]);
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
            out[3] = (f32)angle[0] * lbl_801904D8[0];
            out[4] = (f32)angle[1] * lbl_801904D8[0];
            out[5] = (f32)angle[2] * lbl_801904D8[0];
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
#endif
