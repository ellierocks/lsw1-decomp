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
        idx = ObjectAnim[mask_bytes[3]];
        idx += ObjectAnim[mask_bytes[2]];
        idx += ObjectAnim[mask_bytes[1] & time->time_mask];
        break;
    case 3:
        idx = ObjectAnim[mask_bytes[3]];
        idx += ObjectAnim[mask_bytes[2]];
        idx += ObjectAnim[mask_bytes[1]];
        idx += ObjectAnim[mask_bytes[0] & time->time_mask];
        break;
    }

    {
        struct nuanimkey_s* curr = &curve->animkeys[idx];
        struct nuanimkey_s* prev = curr - 1;
        u32 flags = curve->flags;

        if (flags & 1) {
            do {
                if ((flags & 2) && idx - 1 < curve->numkeys) {
                    f32 d1 = time->time - curr[-1].time;
                    f32 d2 = prev[1].time - time->time;
                    if (d1 > d2) return prev[1].d;
                }
            } while (0);
            return prev->d;
        }

        {
            f32 value = prev->d;
            f32 span = curr->time - curr[-1].time;
            f32 delta = value - curr->d;
            f32 t = (time->time - curr[-1].time) * prev->dtime;
            f32 tangent0 = prev->c * span;
            f32 tangent1 = curr->c * span;
            f32 result = delta + delta;
            result += tangent0;
            result += tangent1;
            result = t * result + delta * -3.0f;
            result -= tangent0 + tangent0;
            result -= tangent1;
            tangent0 = t * result + tangent0;
            return t * tangent0 + value;
        }
    }
}

extern char lbl_80190074[];
extern void (*fn_80009948(char* file, s32 line))();

extern char str_NuAnimCurve2CalcVal_NoneAlreadyEvaluated[];
extern char str_NuAnimCurve2CalcVal_BooleanAlreadyEvaluated[];

struct nuanimkey2float_s {
    f32 time, reciprocal, value, tangent;
};
struct nuanimkey2packed_s {
    s16 value;
    s8 tangent;
    u8 time;
};
struct nuanimkey2wide_s {
    s16 value;
    s8 tangent;
    u8 unknown03;
    u16 time;
};

f32 NuAnimCurve2CalcVal(struct nuanimcurve2_s* curves, struct nuanimtime_s* time, s32 type)
{
    struct nuanimcurvedata_s* data;
    u32* masks = curves->data.curvedata->mask;
    s32 chunk = time->chunk;
    u8* mask = (u8*)&masks[chunk];
    s32 key = 0;
    s32 index;
    f32 result;

    if (type == 4) {
        s32 frame = (s32)fn_8000B264(time->time_offset);
        return (f32)((masks[chunk] >> (frame - 1)) & 1);
    }
    switch (time->time_byte) {
    case 0:
        key = ObjectAnim[mask[3] & time->time_mask];
        break;
    case 1:
        key = ObjectAnim[mask[3]];
        key += ObjectAnim[mask[2] & time->time_mask];
        break;
    case 2:
        key = ObjectAnim[mask[3]];
        key += ObjectAnim[mask[2]];
        key += ObjectAnim[mask[1] & time->time_mask];
        break;
    case 3:
        key = ObjectAnim[mask[3]];
        key += ObjectAnim[mask[2]];
        key += ObjectAnim[mask[1]];
        key += ObjectAnim[mask[0] & time->time_mask];
        break;
    }
    data = curves->data.curvedata;
    index = data->key_ixs[time->chunk];
    switch ((u32)type) {
    case 0:
        fn_80009948(lbl_80190074, 0x5C5)(str_NuAnimCurve2CalcVal_NoneAlreadyEvaluated);
        result = 0.0f;
        break;
    case 1: {
        struct nuanimkey2float_s* first = &((struct nuanimkey2float_s*)data->key_array)[index + key - 1];
        struct nuanimkey2float_s* next = first + 1;
        f32 value = first->value;
        f32 span = next->time - first->time;
        f32 delta = value - next->value;
        f32 t = (time->time - first->time) * first->reciprocal;
        f32 tangent0 = first->tangent * span;
        f32 tangent1 = next->tangent * span;
        result = delta + delta;
        result += tangent0;
        result += tangent1;
        result = t * result - delta * 3.0f;
        result -= tangent0 + tangent0;
        result -= tangent1;
        result = t * result + tangent0;
        result = t * result + value;
        break;
    }
    case 5: {
        f32* header = data->key_array;
        struct nuanimkey2wide_s* first = &((struct nuanimkey2wide_s*)data->key_array)[index + key + 2];
        struct nuanimkey2wide_s* next = first + 1;
        f32 firstTime = (f32)(u32)first->time;
        f32 span = (f32)(u32)next->time - firstTime;
        f32 reciprocal = 1.0f / span;
        f32 tangent0 = (f32)first->tangent * ((f32*)data->key_array)[0] * span;
        f32 tangent1 = (f32)next->tangent * ((f32*)data->key_array)[0] * span;
        f32 value = (f32)first->value * ((f32*)data->key_array)[1] + ((f32*)data->key_array)[2];
        f32 delta = value - ((f32)next->value * ((f32*)data->key_array)[1] + ((f32*)data->key_array)[2]);
        f32 t = (time->time - 1.0f - firstTime) * reciprocal;
        result = delta + delta;
        result += tangent0;
        result += tangent1;
        result = t * result - delta * 3.0f;
        result -= tangent0 + tangent0;
        result -= tangent1;
        result = t * result + tangent0;
        result = t * result + value;
        break;
    }
    case 6: {
        f32* header = data->key_array;
        struct nuanimkey2packed_s* first = &((struct nuanimkey2packed_s*)data->key_array)[index + key + 3];
        struct nuanimkey2packed_s* next = first + 1;
        f32 firstTime = (f32)(u32)first->time * ((f32*)data->key_array)[3];
        f32 nextTime = (f32)(u32)next->time * ((f32*)data->key_array)[3];
        f32 span, reciprocal, tangent0, tangent1, value, delta, t;
        if (nextTime == firstTime) nextTime = firstTime + 1.0f;
        span = nextTime - firstTime;
        reciprocal = 1.0f / span;
        tangent0 = (f32)first->tangent * ((f32*)data->key_array)[0] * span;
        tangent1 = (f32)next->tangent * ((f32*)data->key_array)[0] * span;
        value = (f32)first->value * ((f32*)data->key_array)[1] + ((f32*)data->key_array)[2];
        delta = value - ((f32)next->value * ((f32*)data->key_array)[1] + ((f32*)data->key_array)[2]);
        t = (time->time - 1.0f - firstTime) * reciprocal;
        result = delta + delta;
        result += tangent0;
        result += tangent1;
        result = t * result - delta * 3.0f;
        result -= tangent0 + tangent0;
        result -= tangent1;
        result = t * result + tangent0;
        result = t * result + value;
        break;
    }
    case 2: {
        f32* header = data->key_array;
        struct nuanimkey2packed_s* first = &((struct nuanimkey2packed_s*)data->key_array)[index + key + 1];
        struct nuanimkey2packed_s* next = first + 1;
        f32 firstTime = (f32)(u32)first->time;
        f32 span = (f32)(u32)next->time - firstTime;
        f32 reciprocal = span == 0.0f ? 0.0f : 1.0f / span;
        f32 tangent0 = (f32)first->tangent * ((f32*)data->key_array)[0] * span;
        f32 tangent1 = (f32)next->tangent * ((f32*)data->key_array)[0] * span;
        f32 value = (f32)first->value * ((f32*)data->key_array)[1];
        f32 delta = value - (f32)next->value * ((f32*)data->key_array)[1];
        f32 t = (time->time - 1.0f - firstTime) * reciprocal;
        result = delta + delta;
        result += tangent0;
        result += tangent1;
        result = t * result - delta * 3.0f;
        result -= tangent0 + tangent0;
        result -= tangent1;
        result = t * result + tangent0;
        result = t * result + value;
        break;
    }
    case 3:
        result = *(f32*)((u8*)data->key_array + (index + key - 1) * 8);
        break;
    case 4:
        fn_80009948(lbl_80190074, 0x663)(str_NuAnimCurve2CalcVal_BooleanAlreadyEvaluated);
        result = 0.0f;
        break;
    default:
        result = 0.0f;
        break;
    }
    return result;
}

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
extern void NuAnimCurve2SetApplyBlendToJoint2TransLoc(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, f32 weight, s32 i,
                                                      s32 j, s32 k, s32 l, s32 m, s32 n, s32 o, s32 p);

void NuAnimCurve2SetApplyToJoint(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i)
{
    NuAnimCurve2SetApplyToJointTransLoc(a, b, c, d, e, f, g, h, i, 0, 0);
}

void NuAnimCurve2SetApplyBlendToJoint2(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h, f32 weight, s32 i, s32 j, s32 k,
                                       s32 l, s32 m)
{
    NuAnimCurve2SetApplyBlendToJoint2TransLoc(a, b, c, d, e, f, g, h, weight, i, j, k, l, m, 0, 0, 0);
}

extern void fn_8000EB24(struct numtx_s* m, s32* ang);
extern void fn_8000EA94(struct numtx_s* m, struct nuvec_s* s);
extern void NuMtxSetIdentity(struct numtx_s* m);

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
        NuMtxSetIdentity(mtx);
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
        NuMtxSetIdentity(mtx);
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

extern struct nuvec_s lbl_801B0ECC[];
extern void fn_8000EBC8(struct numtx_s*, struct numtx_s*, void*);
extern void NuVec4MtxTransformVU0x4(struct numtx_s*, struct nuvec_s*);

void NuAnimCurve2SetApplyToJointTransLoc(s32 a, s32 b, s32 flags, s32 d, s32 e, s32 f, s32 g, s32 h, s32 i, s32 j, s32 k)
{
    struct nuvec_s translation;
    struct nuvec_s rotation;
    struct nuvec_s negativePivot;
    s32 angles[3];
    struct nuvec_s inverse;
    struct nuanimcurve2_s* curves = (void*)a;
    s8* types = (void*)b;
    struct nuanimtime_s* time = (void*)d;
    u8* joint = (void*)e;
    struct nuvec_s* scale = (void*)f;
    struct nuvec_s* parentScale = (void*)g;
    struct numtx_s* matrix = (void*)h;
    struct NUJOINTANIM_s* override = (void*)i;
    struct nuvec_s* root = (void*)j;
    struct nuvec_s* locator = (void*)k;
    s8 overrideFlags;

    if (root != NULL) *root = lbl_801B0ECC[0];
    if (locator != NULL) *locator = lbl_801B0ECC[0];
    if (override != NULL) overrideFlags = override->flags;
    else overrideFlags = 0;
    if ((flags & 1) || (overrideFlags & 1)) {
        if (flags & 1) {
            rotation.x = types[3] == 0 ? curves[3].data.constant : NuAnimCurve2CalcVal(&curves[3], time, types[3]);
            rotation.y = types[4] == 0 ? curves[4].data.constant : NuAnimCurve2CalcVal(&curves[4], time, types[4]);
            rotation.z = types[5] == 0 ? curves[5].data.constant : NuAnimCurve2CalcVal(&curves[5], time, types[5]);
        } else {
            rotation.x = rotation.z = rotation.y = 0.0f;
        }
        if (overrideFlags & 1) {
            f32 rx = rotation.x + override->rx;
            f32 ry = rotation.y + override->ry;
            f32 rz = rotation.z + override->rz;
            s32 ax;
            rotation.x = rx;
            ax = (s32)(rx * 10430.378f);
            rotation.y = ry;
            angles[1] = (s32)(ry * 10430.378f);
            rotation.z = rz;
            angles[2] = (s32)(rz * 10430.378f);
            angles[0] = ax;
            if (overrideFlags & 8) {
                angles[0] &= 0xffff;
                if (angles[0] > 32767) angles[0] -= 65536;
                if (angles[0] > override->max_rx) angles[0] = override->max_rx;
                else if (angles[0] < override->min_rx) angles[0] = override->min_rx;
            }
            if (overrideFlags & 16) {
                angles[1] &= 0xffff;
                if (angles[1] > 32767) angles[1] -= 65536;
                if (angles[1] > override->max_ry) angles[1] = override->max_ry;
                else if (angles[1] < override->min_ry) angles[1] = override->min_ry;
            }
            if (overrideFlags & 32) {
                angles[2] &= 0xffff;
                if (angles[2] > 32767) angles[2] -= 65536;
                if (angles[2] > override->max_rz) angles[2] = override->max_rz;
                else if (angles[2] < override->min_rz) angles[2] = override->min_rz;
            }
        } else {
            angles[0] = (s32)(rotation.x * 10430.378f);
            angles[1] = (s32)(rotation.y * 10430.378f);
            angles[2] = (s32)(rotation.z * 10430.378f);
        }
        fn_8000EB24(matrix, angles);
    } else NuMtxSetIdentity(matrix);
    if (flags & 0x20) fn_8000EBC8(matrix, matrix, joint);
    if ((flags & 8) || (overrideFlags & 4)) {
        if (flags & 8) {
            scale->x = types[6] == 0 ? curves[6].data.constant : NuAnimCurve2CalcVal(&curves[6], time, types[6]);
            scale->y = types[7] == 0 ? curves[7].data.constant : NuAnimCurve2CalcVal(&curves[7], time, types[7]);
            scale->z = types[8] == 0 ? curves[8].data.constant : NuAnimCurve2CalcVal(&curves[8], time, types[8]);
        } else scale->x = scale->z = scale->y = 0.0f;
        if (overrideFlags & 4) {
            scale->x += override->sx;
            scale->y += override->sy;
            scale->z += override->sz;
        }
        fn_8000EA94(matrix, scale);
        scale->x *= parentScale->x;
        scale->y *= parentScale->y;
        scale->z *= parentScale->z;
    } else *scale = *parentScale;
    if ((flags & 0x10) && parentScale != NULL) {
        if (parentScale->x == 0.0f || parentScale->y == 0.0f || parentScale->z == 0.0f) {
            inverse.z = inverse.x = inverse.y = 0.0f;
        } else {
            inverse.z = 1.0f / parentScale->z;
            inverse.x = 1.0f / parentScale->x;
            inverse.y = 1.0f / parentScale->y;
        }
        NuVec4MtxTransformVU0x4(matrix, &inverse);
        scale->x *= inverse.x;
        scale->y *= inverse.y;
        scale->z *= inverse.z;
    }
    translation.x = types[0] == 0 ? curves[0].data.constant : NuAnimCurve2CalcVal(&curves[0], time, types[0]);
    translation.y = types[1] == 0 ? curves[1].data.constant : NuAnimCurve2CalcVal(&curves[1], time, types[1]);
    translation.z = types[2] == 0 ? curves[2].data.constant : NuAnimCurve2CalcVal(&curves[2], time, types[2]);
    if (root != NULL) {
        root->z = -translation.z;
        root->x = translation.x;
        root->y = translation.y;
    }
    if (overrideFlags & 2) {
        translation.x += override->tx;
        translation.y += override->ty;
        translation.z += override->tz;
    }
    NuMtxTranslate(matrix, &translation);
    if (joint[0x51] & 8) {
        struct nuvec_s* pivot = (void*)(joint + 0x40);
        NuMtxPreTranslate(matrix, pivot);
        negativePivot.z = -pivot->z;
        negativePivot.x = -pivot->x;
        negativePivot.y = -pivot->y;
        NuMtxTranslate(matrix, &negativePivot);
        if (locator != NULL) {
            NuVecMtxRotate(locator, pivot, matrix);
            NuVecSub(locator, locator, pivot);
            locator->z = -locator->z;
        }
    }
    matrix->_02 = -matrix->_02;
    matrix->_12 = -matrix->_12;
    matrix->_32 = -matrix->_32;
    matrix->_20 = -matrix->_20;
    matrix->_21 = -matrix->_21;
    matrix->_23 = -matrix->_23;
}

void NuAnimCurve2SetApplyBlendToJoint2TransLoc(s32 a, s32 b, s32 flagsA, s32 d, s32 e, s32 f, s32 flagsB, s32 h,
                                              f32 weight, s32 i, s32 j, s32 k, s32 l, s32 m, s32 n, s32 o, s32 p)
{
    struct nuvec_s negativePivot;
    s32 angles[3];
    struct nuvec_s rotation;
    struct nuvec_s rotA;
    struct nuvec_s rotB;
    struct nuvec_s scaleA;
    struct nuvec_s scaleB;
    struct nuvec_s translation;
    struct nuvec_s inverse;
    struct nuanimcurve2_s* curvesA = (void*)a;
    s8* typesA = (void*)b;
    struct nuanimtime_s* timeA = (void*)d;
    struct nuanimcurve2_s* curvesB = (void*)e;
    s8* typesB = (void*)f;
    struct nuanimtime_s* timeB = (void*)h;
    u8* joint = (void*)i;
    struct nuvec_s* scale = (void*)j;
    struct nuvec_s* parentScale = (void*)k;
    struct numtx_s* matrix = (void*)l;
    struct NUJOINTANIM_s* override = (void*)m;
    struct nuvec_s* rootA = (void*)n;
    struct nuvec_s* rootB = (void*)o;
    struct nuvec_s* locator = (void*)p;
    f32 inverseWeight = 1.0f - weight;
    s8 overrideFlags;
    s32 component;
    if (rootA != NULL) *rootA = lbl_801B0ECC[0];
    if (rootB != NULL) *rootB = lbl_801B0ECC[0];
    if (locator != NULL) *locator = lbl_801B0ECC[0];
    if (override != NULL) overrideFlags = override->flags;
    else overrideFlags = 0;
    if ((flagsA & 1) || (flagsB & 1) || (overrideFlags & 1)) {
        {
            f32* outA = (f32*)&rotA;
            s32 outputIndex = 0;
            struct nuanimcurve2_s* curveA = curvesA + 3;
            struct nuanimcurve2_s* curveB = curvesB + 3;
            f32* constantA = (f32*)(curvesA + 3);
            f32* constantB = (f32*)(curvesB + 3);
            for (component = 3; component <= 5; component++) {
                *outA = typesA[component] == 0 ? *constantA : NuAnimCurve2CalcVal(curveA, timeA, typesA[component]);
                *(f32*)((u8*)&rotB + outputIndex) = typesB[component] == 0 ? *constantB : NuAnimCurve2CalcVal(curveB, timeB, typesB[component]);
                outputIndex += 4;
                outA++;
                curveA++;
                curveB++;
                constantA++;
                constantB++;
            }
        }
        rotA.x -= rotA.x - rotB.x > 3.1415927f ? 6.2831855f : 0.0f;
        rotB.x -= rotB.x - rotA.x > 3.1415927f ? 6.2831855f : 0.0f;
        rotA.y -= rotA.y - rotB.y > 3.1415927f ? 6.2831855f : 0.0f;
        rotB.y -= rotB.y - rotA.y > 3.1415927f ? 6.2831855f : 0.0f;
        rotA.z -= rotA.z - rotB.z > 3.1415927f ? 6.2831855f : 0.0f;
        rotB.z -= rotB.z - rotA.z > 3.1415927f ? 6.2831855f : 0.0f;
        rotation.x = rotA.x * inverseWeight + rotB.x * weight;
        rotation.y = rotA.y * inverseWeight + rotB.y * weight;
        rotation.z = rotA.z * inverseWeight + rotB.z * weight;
        if (overrideFlags & 1) {
            f32 rx = rotation.x + override->rx;
            f32 ry = rotation.y + override->ry;
            f32 rz = rotation.z + override->rz;
            s32 ax;
            rotation.x = rx;
            ax = (s32)(rx * 10430.378f);
            rotation.y = ry;
            angles[1] = (s32)(ry * 10430.378f);
            rotation.z = rz;
            angles[2] = (s32)(rz * 10430.378f);
            angles[0] = ax;
            if (overrideFlags & 8) {
                angles[0] &= 0xffff;
                if (angles[0] > 32767) angles[0] -= 65536;
                if (angles[0] > override->max_rx) angles[0] = override->max_rx;
                else if (angles[0] < override->min_rx) angles[0] = override->min_rx;
            }
            if (overrideFlags & 16) {
                angles[1] &= 0xffff;
                if (angles[1] > 32767) angles[1] -= 65536;
                if (angles[1] > override->max_ry) angles[1] = override->max_ry;
                else if (angles[1] < override->min_ry) angles[1] = override->min_ry;
            }
            if (overrideFlags & 32) {
                angles[2] &= 0xffff;
                if (angles[2] > 32767) angles[2] -= 65536;
                if (angles[2] > override->max_rz) angles[2] = override->max_rz;
                else if (angles[2] < override->min_rz) angles[2] = override->min_rz;
            }
        } else {
            angles[0] = (s32)(rotation.x * 10430.378f);
            angles[1] = (s32)(rotation.y * 10430.378f);
            angles[2] = (s32)(rotation.z * 10430.378f);
        }
        fn_8000EB24(matrix, angles);
    } else NuMtxSetIdentity(matrix);
    if ((flagsA & 0x20) || (flagsB & 0x20)) fn_8000EBC8(matrix, matrix, joint);
    if ((flagsA & 8) || (flagsB & 8) || (overrideFlags & 4)) {
        {
            f32* outA = (f32*)&scaleA;
            s32 outputIndex = 0;
            struct nuanimcurve2_s* curveA = curvesA + 6;
            struct nuanimcurve2_s* curveB = curvesB + 6;
            f32* constantA = (f32*)(curvesA + 6);
            f32* constantB = (f32*)(curvesB + 6);
            for (component = 6; component <= 8; component++) {
                *outA = typesA[component] == 0 ? *constantA : NuAnimCurve2CalcVal(curveA, timeA, typesA[component]);
                *(f32*)((u8*)&scaleB + outputIndex) = typesB[component] == 0 ? *constantB : NuAnimCurve2CalcVal(curveB, timeB, typesB[component]);
                outputIndex += 4;
                outA++;
                curveA++;
                curveB++;
                constantA++;
                constantB++;
            }
        }
        scale->x = scaleA.x * inverseWeight + scaleB.x * weight;
        scale->y = scaleA.y * inverseWeight + scaleB.y * weight;
        scale->z = scaleA.z * inverseWeight + scaleB.z * weight;
        if (overrideFlags & 4) {
            scale->x += override->sx;
            scale->y += override->sy;
            scale->z += override->sz;
        }
        fn_8000EA94(matrix, scale);
        scale->x *= parentScale->x;
        scale->y *= parentScale->y;
        scale->z *= parentScale->z;
    } else *scale = *parentScale;
    if ((flagsA | flagsB) & 0x10) {
        if (parentScale->x == 0.0f || parentScale->y == 0.0f || parentScale->z == 0.0f) {
            inverse.z = inverse.x = inverse.y = 0.0f;
        } else {
            inverse.z = 1.0f / parentScale->z;
            inverse.x = 1.0f / parentScale->x;
            inverse.y = 1.0f / parentScale->y;
        }
        NuVec4MtxTransformVU0x4(matrix, &inverse);
        scale->x *= inverse.x;
        scale->y *= inverse.y;
        scale->z *= inverse.z;
    }
    {
        f32* outA = (f32*)rootA;
        f32* outB = (f32*)rootB;
        f32* outT = (f32*)&translation;
        struct nuanimcurve2_s* curveA = curvesA;
        struct nuanimcurve2_s* curveB = curvesB;
        f32* constantA = (f32*)curvesA;
        f32* constantB = (f32*)curvesB;
        for (component = 0; component < 3; component++) {
            f32 valueA = typesA[component] == 0 ? *constantA : NuAnimCurve2CalcVal(curveA, timeA, typesA[component]);
            f32 valueB = typesB[component] == 0 ? *constantB : NuAnimCurve2CalcVal(curveB, timeB, typesB[component]);
            if (rootA != NULL) *outA = component == 2 ? -valueA : valueA;
            if (rootB != NULL) *outB = component == 2 ? -valueB : valueB;
            *outT = inverseWeight * valueA + weight * valueB;
            outA++;
            outB++;
            outT++;
            curveA++;
            curveB++;
            constantA++;
            constantB++;
        }
    }
    if (overrideFlags & 2) {
        translation.x += override->tx;
        translation.y += override->ty;
        translation.z += override->tz;
    }
    NuMtxTranslate(matrix, &translation);
    if (joint[0x51] & 8) {
        struct nuvec_s* pivot = (void*)(joint + 0x40);
        NuMtxPreTranslate(matrix, pivot);
        negativePivot.z = -pivot->z;
        negativePivot.x = -pivot->x;
        negativePivot.y = -pivot->y;
        NuMtxTranslate(matrix, &negativePivot);
        if (locator != NULL) {
            NuVecMtxRotate(locator, pivot, matrix);
            NuVecSub(locator, locator, pivot);
            locator->z = -locator->z;
        }
    }
    matrix->_02 = -matrix->_02;
    matrix->_12 = -matrix->_12;
    matrix->_32 = -matrix->_32;
    matrix->_20 = -matrix->_20;
    matrix->_21 = -matrix->_21;
    matrix->_23 = -matrix->_23;
}
