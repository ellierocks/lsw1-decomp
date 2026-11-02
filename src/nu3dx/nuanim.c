#include "types.h"
#include "nu3dx/nuanim.h"

struct nutimebarview_s {
    u32 unknown00;
    u32 vals[16][2];
    u32 start[16];
    u8 unknownC4[0xC0];
    s32 which[16];
    s32 arg[16];
    u8 active[16];
};

struct nuanimfilebuff_s {
    u32 addr;
};

extern f32 lbl_8040A318;
extern struct nutimebarview_s* lbl_8040A314;
extern u8 ObjectAnim[256];
extern s32 lbl_80407C18;
extern f32 fn_8002DE38(void);
extern u32 fn_8002DE18(void);
extern void fn_8002DE14(s32 a, struct nuanimfilebuff_s b);
extern struct nutimebarview_s* fn_800163F4(s32 a, struct nuanimfilebuff_s b, s32 c);
extern void* fn_8002163C(void* a, void* b);
extern f32 fn_8000B264(f32);

void fn_8001662C(f32 a);

void fn_80016558(s32 a, struct nuanimfilebuff_s buf)
{
    fn_8002DE14(a, buf);
    lbl_8040A314 = fn_800163F4(a, buf, 0);
    fn_8001662C(1.0f);
}

void fn_800165B4(struct nutimebarview_s* t, u32 i)
{
    if (t == NULL) {
        t = lbl_8040A314;
    }
    i &= 0xF;
    t->which[i] ^= 1;
    t->vals[i][t->which[i] ? 0 : 1] = 0;
}

u32 fn_800165F8(struct nutimebarview_s* t, u32 i)
{
    if (t == NULL) {
        t = lbl_8040A314;
    }
    i &= 0xF;
    return t->vals[i][t->which[i]];
}

void fn_8001662C(f32 a)
{
    lbl_8040A318 = a * fn_8002DE38();
}

void fn_80016660(struct nutimebarview_s* t, u32 i, s32 arg)
{
    struct nutimebarview_s* tb;
    if (t == NULL) tb = lbl_8040A314; else tb = t;
    i &= 0xF;
    tb->start[i] = fn_8002DE18();
    tb->arg[i] = arg;
    tb->active[i] = 1;
}

u32 fn_800166C4(struct nutimebarview_s* t, u32 i)
{
    struct nutimebarview_s* tb;
    u32 d;

    if (t == NULL) tb = lbl_8040A314; else tb = t;
    if (tb->active[i] != 0) {
        tb->active[i] = 0;
        d = fn_8002DE18();
        i &= 0xF;
        d -= tb->start[(u16)i];
        if (d > 0xFFFF) {
            d += 0xFFFF;
        }
        tb->vals[i][1 - tb->which[i]] += d;
        return tb->vals[i][1 - tb->which[i]];
    }
    return 0;
}

static inline void buildBitCountTable(void)
{
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
}

#define NUANIM_RELOC(p, off) ((p) != 0 ? (void*)((s32)(p) + (off)) : NULL)

struct nuanimdata2_s* fn_8001678C(struct nuanimdata2_s* data, s32 off)
{
    struct nuanimdata2_s* d;
    s32 i;
    s32 n;

    if (lbl_80407C18 == 0) {
        buildBitCountTable();
    }
    d = NUANIM_RELOC(data, off);
    if (d != NULL) {
        if (*(s32*)d == 0x414E4934) {
            fn_8002163C(d, d);
        } else {
            d->curves = NUANIM_RELOC(d->curves, off);
            d->curveflags = NUANIM_RELOC(d->curveflags, off);
            d->curvesetflags = NUANIM_RELOC(d->curvesetflags, off);
            n = d->nnodes * d->ncurves;
            for (i = 0; i < n; i++) {
                if (d->curveflags[i] != 0) {
                    struct nuanimcurve2_s* c = &d->curves[i];
                    struct nuanimcurvedata_s* cd;
                    cd = NUANIM_RELOC(c->data.curvedata, off);
                    c->data.curvedata = cd;
                    cd->mask = NUANIM_RELOC(cd->mask, off);
                    c->data.curvedata->key_ixs = NUANIM_RELOC(c->data.curvedata->key_ixs, off);
                    c->data.curvedata->key_array = NUANIM_RELOC(c->data.curvedata->key_array, off);
                }
            }
        }
    }
    return d;
}

void fn_80016944(struct nuanimdata_s* data, f32 time, struct nuanimtime_s* out)
{
    s32 tmp;
    s32 byte;

    if (time >= data->time) {
        if (data->time == 1.0f) {
            out->time = 1.0f;
            out->chunk = 0;
            out->time_byte = 0;
            out->time_mask = 1;
            return;
        }
        out->time = data->time - 0.01f;
    } else if (time < 1.0f) {
        out->time = 1.0f;
    } else {
        out->time = time;
    }
    out->chunk = ((s32)out->time - 1) >> 5;
    if (out->chunk >= data->nchunks) {
        out->chunk = data->nchunks - 1;
    }
    out->time_offset = out->time - (f32)(out->chunk * 32);
    tmp = (s32)fn_8000B264(out->time_offset) - 1;
    out->unk14 = tmp;
    byte = tmp / 8;
    out->time_byte = (u8)byte;
    out->time_mask = (u8)((1 << (tmp - byte * 8 + 1)) - 1);
}


struct nuani4hdr_s {
    s32 magic;
    u8 unknown04[6];
    u16 nframes;
};

void NuAnimData2CalcTime(struct nuanimdata2_s* data, struct nuanimtime_s* out, f32 time)
{
    s32 tmp;
    s32 byte;

    if (*(s32*)data == 0x414E4934) {
        if (time < 1.0f) {
            out->time = 1.0f;
        } else {
            u32 n = ((struct nuani4hdr_s*)data)->nframes;
            if (time >= (f32)(s32)n) {
                out->time = (f32)n - 0.01f;
            } else {
                out->time = time;
            }
        }
        return;
    }
    if (time >= data->endframe) {
        if (data->endframe == 1.0f) {
            out->time = 1.0f;
            out->chunk = 0;
            out->time_byte = 0;
            out->time_mask = 1;
            return;
        }
        out->time = data->endframe - 0.01f;
    } else if (time < 1.0f) {
        out->time = 1.0f;
    } else {
        out->time = time;
    }
    out->chunk = ((s32)out->time - 1) >> 5;
    if (out->chunk >= data->nchunks) {
        out->chunk = data->nchunks - 1;
    }
    out->time_offset = out->time - (f32)(out->chunk * 32);
    tmp = (s32)fn_8000B264(out->time_offset) - 1;
    out->unk14 = tmp;
    byte = tmp / 8;
    out->time_byte = (u8)byte;
    out->time_mask = (u8)((1 << (tmp - byte * 8 + 1)) - 1);
}

f32 NuAnimCurveCalcVal2(struct nuanimcurve_s* curve, struct nuanimtime_s* time)
{
    s32 idx = 0;
    u8* mask_bytes = (u8*)&curve->mask;

    switch (time->time_byte) {
    case 0:
        idx = ObjectAnim[mask_bytes[3] & time->time_mask];
        break;
    case 1:
        idx = ObjectAnim[mask_bytes[3]];
        idx += ObjectAnim[mask_bytes[2] & time->time_mask];
        break;
    case 2:
        idx = ObjectAnim[mask_bytes[3]] + ObjectAnim[mask_bytes[2]];
        idx += ObjectAnim[mask_bytes[1] & time->time_mask];
        break;
    case 3:
        idx = ObjectAnim[mask_bytes[3]] + ObjectAnim[mask_bytes[2]] + ObjectAnim[mask_bytes[1]];
        idx += ObjectAnim[mask_bytes[0] & time->time_mask];
        break;
    }

    {
        struct nuanimkey_s* prev = curve->animkeys + (idx - 1);
        struct nuanimkey_s* curr = curve->animkeys + idx;
        u32 flags = curve->flags;

        if (flags & 1) {
            if ((flags & 2) && idx - 1 < curve->numkeys) {
                f32 d1 = time->time - prev->time;
                f32 d2 = curr->time - time->time;
                if (d1 > d2) return curr->d;
            }
            return prev->d;
        }

        {
            f32 t1 = curr->time - prev->time;
            f32 t2 = prev->d - curr->d;
            f32 t4 = prev->c;
            f32 t5 = curr->c;
            f32 t6 = prev->dtime;
            f32 t3 = time->time - prev->time;

            f32 t7 = t4 * t1;
            f32 t8 = t2 * 2.0f;
            f32 t9 = t5 * t1;
            f32 t10 = t8 + t7;
            f32 t11 = t3 * t6;
            f32 t12 = t2 * -3.0f;
            f32 t13 = t10 + t9;
            f32 t14 = t7 * 2.0f;
            f32 t15 = t11 * t13 + t12;
            f32 t16 = t15 - t14;
            f32 t17 = t16 - t9;
            f32 t18 = t11 * t17 + t7;
            return t11 * t18 + prev->d;
        }
    }
}

f32 NuAnimCurve2CalcVal(struct nuanimcurve2_s* curves, struct nuanimtime_s* time, s32 type)
{
    return 0.0f;
}

extern char lbl_80190074[];
extern char lbl_8019017C[];
extern char str_NuAnimDataLoadBuff_OldFormatExpectedNew[];

extern void* NuMemAlloc(s32 size, char* file, s32 line);
extern void NuMemFree(void* p, char* file, s32 line);
extern void* memset();
extern s32 NuFileLoadBuffer(char* path, s32 addr, s32 size);
struct nuanimvariptr_s {
    s32 addr;
};

extern void* NuAnimBuffCreate(s32 max, s32* buf, struct nuanimvariptr_s end);
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
