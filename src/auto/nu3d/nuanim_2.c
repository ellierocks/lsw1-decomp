#include "types.h"
#include "nu3dx/nuanim.h"

extern char lbl_80190074[];
extern char lbl_8019017C[];
extern char str_NuAnimDataLoadBuff_OldFormatExpectedNew[];
extern u8 ObjectAnim[256];
extern s32 lbl_80407C18;

extern void* NuMemAlloc(s32 size, char* file, s32 line);
extern void NuMemFree(void* p, char* file, s32 line);
extern void* memset();
extern s32 NuFileLoadBuffer(char* path, s32 addr, s32 size);
struct nuanimvariptr_s {
    s32 addr;
};

extern void* NuAnimBuffCreate(s32 max, s32* buf, struct nuanimvariptr_s end);
extern void* fn_8002163C(void* a, void* b);
extern struct nuanimdata2_s* fn_8001678C(struct nuanimdata2_s* data, s32 off);
extern void (*fn_80009948(char* file, s32 line))();
extern void (*fn_8000995C(char* file, s32 line))();

void* NuAnimInit(s32 max, s32* buf, struct nuanimvariptr_s* end)
{
    struct nuanimvariptr_s e = *end;
    s32 i;
    s32 j;

    for (i = 0; i < 256; i++) {
        ObjectAnim[i] = 0;
        for (j = 0; j < 8; j++) {
            if ((i >> j) & 1) {
                ObjectAnim[i]++;
            }
        }
    }
    lbl_80407C18 = 1;
    return NuAnimBuffCreate(max, buf, e);
}

struct nuanimdata_s* NuAnimDataCreate(s32 nchunks)
{
    struct nuanimdata_s* d;
    s32 size;

    size = nchunks * 4 + 0x10;
    d = NuMemAlloc(size, lbl_80190074, 0x321);
    memset(d, 0, size);
    d->nchunks = nchunks;
    d->chunks = (struct nuanimdatachunk_s**)(d + 1);
    return d;
}

struct nuanimcurveset_s* NuAnimCurveSetCreate(s32 ncurves)
{
    struct nuanimcurveset_s* cs;
    s32 size;

    cs = NULL;
    if (ncurves != 0) {
        cs = NuMemAlloc(0x10, lbl_80190074, 0x6A2);
        size = ncurves * 4;
        memset(cs, 0, 0x10);
        cs->ncurves = ncurves;
        cs->set = NuMemAlloc(size, lbl_80190074, 0x6A9);
        memset(cs->set, 0, size);
        cs->constants = NuMemAlloc(size, lbl_80190074, 0x6AE);
        memset(cs->constants, 0, size);
    }
    return cs;
}

struct nuanimcurve_s* NuAnimCurveCreate(s32 numkeys)
{
    struct nuanimcurve_s* c;
    s32 size;

    size = numkeys * 0x10;
    c = NuMemAlloc(0x10, lbl_80190074, 0x536);
    memset(c, 0, 0x10);
    c->numkeys = numkeys;
    c->animkeys = NuMemAlloc(size, lbl_80190074, 0x53A);
    memset(c->animkeys, 0, size);
    return c;
}

void NuAnimCurveDestroy(struct nuanimcurve_s* c)
{
    if (c->animkeys != NULL) {
        NuMemFree(c->animkeys, lbl_80190074, 0x544);
    }
    NuMemFree(c, lbl_80190074, 0x545);
}

void* NuAnimDataLoadBuff(void* path, s32* buf);

void* NuAnimDataLoad(char* path, s32* buf, s32* end, s32* out)
{
    s32 n;

    *buf = (*buf + 0xF) & ~0xF;
    n = NuFileLoadBuffer(path, *buf, *end - *buf);
    if (n == 0) {
        fn_8000995C(lbl_80190074, 0x3E5)(lbl_8019017C, path);
        return NULL;
    }
    *out = *buf;
    *buf += n;
    return NuAnimDataLoadBuff((void*)n, out);
}

void* NuAnimData2LoadBuff(char* path, s32* buf, s32* end)
{
    s32 out;

    return NuAnimDataLoad(path, buf, end, &out);
}

struct nuanimdatachunk_s* NuAnimDataChunkCreate(s32 numnodes)
{
    struct nuanimdatachunk_s* ch;
    s32 size;

    size = numnodes * 4;
    ch = NuMemAlloc(0x14, lbl_80190074, 0x30D);
    memset(ch, 0, 0x14);
    ch->numnodes = numnodes;
    ch->animcurvesets = NuMemAlloc(size, lbl_80190074, 0x311);
    memset(ch->animcurvesets, 0, size);
    return ch;
}

struct nuanimfilehdr_s {
    s32 version;
    s32 base;
    struct nuanimdata2_s* data;
};

void* NuAnimDataLoadBuff(void* size, s32* buf)
{
    struct nuanimfilehdr_s* hdr;
    s32 version;

    hdr = (struct nuanimfilehdr_s*)*buf;
    version = hdr->version;
    if ((u32)(version - 0x414E4933) <= 1) {
        fn_8002163C(hdr, hdr);
        return hdr;
    }
    if (version != 2) {
        fn_80009948(lbl_80190074, 0x411)(str_NuAnimDataLoadBuff_OldFormatExpectedNew);
    }
    ((struct nuanimfilehdr_s*)*buf)->version = (s32)size;
    ((struct nuanimfilehdr_s*)*buf)->data = fn_8001678C(((struct nuanimfilehdr_s*)*buf)->data,
                                                        *buf - ((struct nuanimfilehdr_s*)*buf)->base);
    ((struct nuanimfilehdr_s*)*buf)->base = *buf;
    return ((struct nuanimfilehdr_s*)*buf)->data;
}

extern void NuAnimCurve2SetApplyToJointTransLoc(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i, s32 j, s32 k);
extern void NuAnimCurve2SetApplyBlendToJoint2TransLoc(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i,
                                                      s32 j, s32 k, s32 l, s32 m, s32 n, s32 o, s32 p);

void NuAnimCurve2SetApplyToJoint(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i)
{
    NuAnimCurve2SetApplyToJointTransLoc(a, b, c, d, e, f, g, h, i, 0, 0);
}

void NuAnimCurve2SetApplyBlendToJoint2(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i, s32 j, s32 k,
                                       s32 l, s32 m)
{
    NuAnimCurve2SetApplyBlendToJoint2TransLoc(a, b, c, d, e, f, g, h, i, j, k, l, m, 0, 0, 0);
}

extern void fn_8000EB24(struct numtx_s* m, s32* ang);
extern void fn_8000EA94(struct numtx_s* m, struct nuvec_s* s);
extern void fn_8000DC10(struct numtx_s* m);

void NuAnimCurve2SetApplyToMatrix(struct nuanimcurve2_s* curves, char* flags, u32 mask, struct nuanimtime_s* time,
                                  struct numtx_s* mtx)
{
    struct nuvec_s t;
    struct nuvec_s r;
    s32 ang[3];
    struct nuvec_s s;

    if (mask & 1) {
        r.x = flags[3] == 0 ? curves[3].data.constant : NuAnimCurve2CalcVal(&curves[3], time, flags[3]);
        r.y = flags[4] == 0 ? curves[4].data.constant : NuAnimCurve2CalcVal(&curves[4], time, flags[4]);
        r.z = flags[5] == 0 ? curves[5].data.constant : NuAnimCurve2CalcVal(&curves[5], time, flags[5]);
        ang[0] = (s32)(r.x * 10430.378f);
        ang[1] = (s32)(r.y * 10430.378f);
        ang[2] = (s32)(r.z * 10430.378f);
        fn_8000EB24(mtx, ang);
    } else {
        fn_8000DC10(mtx);
    }
    if (mask & 8) {
        s.x = flags[6] == 0 ? curves[6].data.constant : NuAnimCurve2CalcVal(&curves[6], time, flags[6]);
        s.y = flags[7] == 0 ? curves[7].data.constant : NuAnimCurve2CalcVal(&curves[7], time, flags[7]);
        s.z = flags[8] == 0 ? curves[8].data.constant : NuAnimCurve2CalcVal(&curves[8], time, flags[8]);
        fn_8000EA94(mtx, &s);
    }
    t.x = flags[0] == 0 ? curves[0].data.constant : NuAnimCurve2CalcVal(&curves[0], time, flags[0]);
    t.y = flags[1] == 0 ? curves[1].data.constant : NuAnimCurve2CalcVal(&curves[1], time, flags[1]);
    t.z = flags[2] == 0 ? curves[2].data.constant : NuAnimCurve2CalcVal(&curves[2], time, flags[2]);
    NuMtxTranslate(mtx, &t);
    mtx->_02 = -mtx->_02;
    mtx->_12 = -mtx->_12;
    mtx->_20 = -mtx->_20;
    mtx->_21 = -mtx->_21;
    mtx->_23 = -mtx->_23;
    mtx->_32 = -mtx->_32;
}

extern f32 NuAnimCurveCalcVal2(struct nuanimcurve_s* curve, struct nuanimtime_s* time);

void NuAnimCurveSetApplyToMatrix(struct nuanimcurveset_s* cs, struct nuanimtime_s* time, struct numtx_s* mtx)
{
    struct nuvec_s t;
    struct nuvec_s r;
    s32 ang[3];
    struct nuvec_s s;

    if (cs->flags & 1) {
        if (cs->set[3]) {
            r.x = NuAnimCurveCalcVal2(cs->set[3], time);
        } else {
            r.x = cs->constants[3];
        }
        if (cs->set[4]) {
            r.y = NuAnimCurveCalcVal2(cs->set[4], time);
        } else {
            r.y = cs->constants[4];
        }
        if (cs->set[5]) {
            r.z = NuAnimCurveCalcVal2(cs->set[5], time);
        } else {
            r.z = cs->constants[5];
        }
        ang[0] = (s32)(r.x * 10430.378f);
        ang[1] = (s32)(r.y * 10430.378f);
        ang[2] = (s32)(r.z * 10430.378f);
        fn_8000EB24(mtx, ang);
    } else {
        fn_8000DC10(mtx);
    }
    if (cs->flags & 8) {
        if (cs->set[6]) {
            s.x = NuAnimCurveCalcVal2(cs->set[6], time);
        } else {
            s.x = cs->constants[6];
        }
        if (cs->set[7]) {
            s.y = NuAnimCurveCalcVal2(cs->set[7], time);
        } else {
            s.y = cs->constants[7];
        }
        if (cs->set[8]) {
            s.z = NuAnimCurveCalcVal2(cs->set[8], time);
        } else {
            s.z = cs->constants[8];
        }
        fn_8000EA94(mtx, &s);
    }
    if (cs->set[0]) {
        t.x = NuAnimCurveCalcVal2(cs->set[0], time);
    } else {
        t.x = cs->constants[0];
    }
    if (cs->set[1]) {
        t.y = NuAnimCurveCalcVal2(cs->set[1], time);
    } else {
        t.y = cs->constants[1];
    }
    if (cs->set[2]) {
        t.z = NuAnimCurveCalcVal2(cs->set[2], time);
    } else {
        t.z = cs->constants[2];
    }
    NuMtxTranslate(mtx, &t);
    mtx->_02 = -mtx->_02;
    mtx->_12 = -mtx->_12;
    mtx->_20 = -mtx->_20;
    mtx->_21 = -mtx->_21;
    mtx->_23 = -mtx->_23;
    mtx->_32 = -mtx->_32;
}
