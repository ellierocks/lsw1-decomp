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
