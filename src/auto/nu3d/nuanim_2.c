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
