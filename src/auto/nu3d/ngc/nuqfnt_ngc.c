#include "types.h"

struct nuqfnt_cmd_s {
    s8 op;
    u8 arg0;
    u8 pad[2];
    s32 arg1;
};

void fn_8002BE28(struct nuqfnt_cmd_s* c)
{
    s32 z = 0;
    c->arg1 = z;
    c->op = 0x87;
    c->arg0 = z;
}

void fn_8002BE40(struct nuqfnt_cmd_s* c)
{
    s32 z = 0;
    c->arg1 = z;
    c->op = 0x87;
    c->arg0 = z;
}

void fn_8002BE58(struct nuqfnt_cmd_s* c, s32 v)
{
    c->arg0 = 3;
    c->arg1 = v;
    c->op = 0x80;
}

void fn_8002BE70(struct nuqfnt_cmd_s* c)
{
    s32 z = 0;
    c->arg1 = z;
    c->op = 0x87;
    c->arg0 = z;
}

void fn_8002BE88(struct nuqfnt_cmd_s* c)
{
    s32 z = 0;
    c->arg1 = z;
    c->op = 0x87;
    c->arg0 = z;
}

void fn_8002C240(void)
{
}

void fn_8002C320(void)
{
}

s32 fn_8002C244(void)
{
    return 0;
}

struct rndr_list_s { u8 pad[0xc]; u32 count; void** items; };
struct rndr_ref_s { s32 pad; u8* flag; };
struct rndr_group_s { u32 count; s32 pad; s32* indices; };
struct rndr_obj_s {
    u8 pad[0x8];
    struct rndr_ref_s* refs;      /* 0x08 */
    u8 pad0c[0x8];
    struct rndr_group_s* groups;  /* 0x14 */
    u8 pad18[0x64];
    struct rndr_list_s* list;     /* 0x7c */
};
struct rndr_state_s {
    s32 count;
    struct rndr_obj_s* objs[0x11d]; /* 0x004 */
    u32 flags;          /* 0x478 */
    s32 pad47c;
    s32 v480;           /* 0x480 */
    u8 pad484[0x20];
    s32 v4a4;           /* 0x4a4 */
    u8 pad4a8[0x1c];
    void* p4c4;         /* 0x4c4 */
    u8 pad4c8[0x30];
    u8 buf4f8[0x10];    /* 0x4f8 */
    s32 v508;           /* 0x508 */
    u8 pad50c[0x34];
    s32 v540;           /* 0x540 */
};
extern struct rndr_state_s lbl_801B0F88;

void fn_8002C2D8(void)
{
    s32 i;
    for (i = 0; i < lbl_801B0F88.count; i++) {
        lbl_801B0F88.objs[i]->list->count = 0;
    }
}

typedef union { void* void_ptr; u32 addr; struct nuqfnt_cmd_s* cmd; u8* u8_ptr; } VARIPTR;
struct rndr_dl_s { u8 pad[0x14]; struct nuqfnt_cmd_s* last; u8* base; u8 pad1c[8]; void* start; };
extern VARIPTR* lbl_8040ACF0;

VARIPTR* fn_8002BEA0(struct rndr_dl_s* dl, s8 op, s32 target)
{
    VARIPTR* buf = lbl_8040ACF0;
    buf->addr = (buf->addr + 3) & ~3;
    dl->last->arg1 = buf->addr;
    buf->cmd->op = op;
    buf->cmd->arg0 = 3;
    if (target != 0) {
        buf->cmd->arg1 = target;
    } else {
        buf->cmd->arg1 = buf->addr + 0x10;
    }
    *(u8*)&buf->cmd[1].op = 0x8d;
    buf->cmd[1].arg0 = 1;
    buf->cmd[1].arg1 = (s32)(dl->base + 8);
    dl->last = buf->cmd + 1;
    buf->addr += 0x10;
    if (target != 0) {
        return NULL;
    }
    return buf;
}

VARIPTR* fn_8002BF48(struct rndr_dl_s* dl, s32 n)
{
    VARIPTR* buf = lbl_8040ACF0;
    buf->addr = (buf->addr + 3) & ~3;
    dl->last->arg1 = buf->addr;
    dl->start = buf->void_ptr;
    *(u8*)&(buf->cmd += n)->op = 0x8d;
    buf->cmd->arg0 = 1;
    buf->cmd->arg1 = (s32)(dl->base + 8);
    dl->last = buf->cmd;
    buf->addr += 8;
    return buf;
}

s32 fn_8002C24C(struct rndr_obj_s* o, s32 unused, s32 idx, s32 ret)
{
    struct rndr_group_s* g = &o->groups[idx];
    u32 i;
    for (i = 0; i < g->count; i++) {
        struct rndr_ref_s* r = &o->refs[g->indices[i]] - 1;
        if (*r->flag == 0) {
            struct rndr_list_s* l;
            *r->flag = 1;
            l = o->list;
            l->items[l->count++] = r->flag;
        }
    }
    return ret;
}

struct rndr_inst_s { u8 dirty; u8 pad[3]; f32 mtx[3][4]; f32 local[3][4]; };
extern f32 lbl_80340404[3][4];
extern void PSMTXConcat(f32 a[3][4], f32 b[3][4], f32 ab[3][4]);

void fn_8002C324(struct rndr_obj_s* o)
{
    u32 i;
    for (i = 0; i < o->list->count; i++) {
        struct rndr_inst_s* inst = o->list->items[i];
        PSMTXConcat(lbl_80340404, inst->local, inst->mtx);
        inst->dirty = 0;
    }
}

struct rndr_mtx_s { s32 flags; f32 mtx[3][4]; };
extern f32 lbl_801B0F08[4][4];

struct rndr_mtx_s* fn_8002C178(VARIPTR* buf)
{
    struct rndr_mtx_s* m;
    f32 unused[3][4]; /* retail frame reserves a second Mtx */
    f32 t[3][4];
    buf->addr = (buf->addr + 3) & ~3;
    m = buf->void_ptr;
    m->flags = 0;
    buf->void_ptr = m + 1;
    t[0][0] = lbl_801B0F08[0][0];
    t[1][0] = lbl_801B0F08[0][1];
    t[2][0] = lbl_801B0F08[0][2];
    t[0][1] = lbl_801B0F08[1][0];
    t[1][1] = lbl_801B0F08[1][1];
    t[2][1] = lbl_801B0F08[1][2];
    t[0][2] = lbl_801B0F08[2][0];
    t[1][2] = lbl_801B0F08[2][1];
    t[2][2] = lbl_801B0F08[2][2];
    t[0][3] = lbl_801B0F08[3][0];
    t[1][3] = lbl_801B0F08[3][1];
    t[2][3] = lbl_801B0F08[3][2];
    PSMTXConcat(lbl_80340404, t, m->mtx);
    return m;
}

struct rndr_pair_s { u16 key; u16 val; };

u16 fn_8002C3A0(struct rndr_pair_s* tab, s32 n, u16 key)
{
    s32 mid = n / 2;
    s32 hi;
    s32 lo;
    if (key > tab[n - 1].key) {
        return 0xFFFF;
    }
    hi = n - 1;
    lo = 0;
    while (1) {
        if (tab[mid].key == key) {
            return tab[mid].val;
        }
        if (hi == lo + 1) {
            if (tab[hi].key == key) {
                return tab[hi].val;
            }
            if (tab[lo].key == key) {
                return tab[lo].val;
            }
            return 0xFFFF;
        }
        if (tab[mid].key > key) {
            hi = mid;
            mid = (lo + hi) >> 1;
        } else if (tab[mid].key < key) {
            lo = mid;
            mid = (lo + hi) >> 1;
        } else if (hi == lo) {
            return 0xFFFF;
        }
    }
}

struct rndr_mtx_s* fn_8002C090(VARIPTR* buf, f32 (*src)[4], s32 unused, u8* offset)
{
    struct rndr_mtx_s* m;
    f32 unused_mtx[3][4]; /* retail frame reserves a second Mtx */
    f32 t[3][4];
    f32 (*dst)[4];
    buf->addr = (buf->addr + 3) & ~3;
    m = buf->void_ptr;
    m->flags = 0;
    buf->void_ptr = m + 1;
    dst = m->mtx;
    t[0][0] = src[0][0];
    t[1][0] = src[0][1];
    t[2][0] = src[0][2];
    t[0][1] = src[1][0];
    t[1][1] = src[1][1];
    t[2][1] = src[1][2];
    t[0][2] = src[2][0];
    t[1][2] = src[2][1];
    t[2][2] = src[2][2];
    t[0][3] = src[3][0];
    t[1][3] = src[3][1];
    t[2][3] = src[3][2];
    if (offset != NULL) {
        t[0][3] += *(f32*)(offset + 0x44);
        t[1][3] += *(f32*)(offset + 0x48);
        t[2][3] += *(f32*)(offset + 0x4c);
    }
    PSMTXConcat(lbl_80340404, t, dst);
    return m;
}

extern void fn_80016660(s32 a, s32 b, char* name);
extern void fn_800166C4(s32 a, s32 b);
extern void fn_80030A34(void);
extern void fn_8003776C(void);
extern char lbl_80191CA4[];
extern void (*lbl_801B18DC[])(s32);
extern s32 lbl_8040AD9C;

void fn_8002BFC0(struct nuqfnt_cmd_s* cmd)
{
    s32 run;
    fn_80016660(lbl_8040AD9C, 4, lbl_80191CA4);
    run = 1;
    fn_80030A34();
    fn_8003776C();
    do {
        switch (cmd->arg0) {
        case 3:
            if (lbl_801B18DC[(u8)cmd->op - 0x80] != NULL) {
                lbl_801B18DC[(u8)cmd->op - 0x80](cmd->arg1);
            }
            cmd++;
            break;
        case 4:
            run = 0;
            break;
        case 0:
            cmd++;
            break;
        case 1:
            cmd = (struct nuqfnt_cmd_s*)cmd->arg1;
            break;
        }
    } while (run);
    fn_800166C4(lbl_8040AD9C, 4);
}

struct nuqfnt_mtl_s {
    u8 pad[0xb8];
    u8 b8;
    u8 padb9[4];
    u8 bd;
    u8 be;
    u8 padbf;
    u8 c0;
    u8 padc1[0xf];
    f32 r;     /* 0xd0 */
    f32 g;     /* 0xd4 */
    f32 b;     /* 0xd8 */
    u8 paddc[0x14];
    f32 alpha; /* 0xf0 */
    s32 tid;   /* 0xf4 */
};
typedef struct vufnt_android_s { u32 colour; f32 x, y, z; f32 mtx[4][4]; f32 x_scale; f32 y_scale; } VUFNT_ANDROID;
typedef struct nuqfnt_s {
    u8 pad[0x2c];
    f32* x_scale;      /* 0x2c */
    f32* y_scale;      /* 0x30 */
    u8* glyphs;        /* 0x34 */
    u8* map;           /* 0x38 */
    void* hdr;         /* 0x3c */
    struct nuqfnt_mtl_s* mtl; /* 0x40 */
    u32* colour;       /* 0x44 */
    VUFNT_ANDROID* platform; /* 0x48 */
} NUQFNT_NGC;
extern char lbl_80191D74[];
extern char lbl_80191D7C[];
extern char lbl_80191D80[];
extern char lbl_80191DB4[];
extern f32 lbl_80191DD0[];
extern int sprintf(char* s, const char* fmt, ...);
extern s32 NuFileLoadBuffer(char* name, void* buf, s32 size);
extern s32 fn_80024DC8(char* name, VARIPTR* buf, VARIPTR* end);
extern struct nuqfnt_mtl_s* fn_80024BF0(s32 n);
extern void fn_80024CD4(struct nuqfnt_mtl_s* mtl);
extern void NuFileWrite(s32 tid);
extern void (*fn_80009970(char* file, s32 line))(char* fmt, ...);

NUQFNT_NGC* fn_8002C460(char* name, char* texname, s32 a2, s32 a3, VARIPTR* buf, VARIPTR* end)
{
    char path[0x400];
    VARIPTR texend;
    VARIPTR start = *buf;
    s32 size;
    buf->addr = (buf->addr + 0xf) & ~0xf;
    sprintf(path, lbl_80191D74, name);
    size = NuFileLoadBuffer(path, buf->void_ptr, end->addr - buf->addr);
    if (size != 0) {
        NUQFNT_NGC* font = buf->void_ptr;
        s32 tid;
        font->glyphs += (u32)font;
        font->map += (u32)font;
        buf->addr = (buf->addr + size + 0xf) & ~0xf;
        font->platform = buf->void_ptr;
        buf->addr += sizeof(VUFNT_ANDROID);
        font->x_scale = &font->platform->x_scale;
        font->y_scale = &font->platform->y_scale;
        font->colour = &font->platform->colour;
        if (texname == NULL) {
            sprintf(path, lbl_80191D7C, name);
        } else {
            sprintf(path, lbl_80191D7C, texname);
        }
        texend = *end;
        tid = fn_80024DC8(path, buf, &texend);
        if (tid != 0) {
            font->mtl = fn_80024BF0(1);
            if (font->mtl != NULL) {
                f32 one = lbl_80191DD0[0];
                font->mtl->r = one;
                font->mtl->g = one;
                font->mtl->b = one;
                font->mtl->bd = 2;
                font->mtl->be = 3;
                font->mtl->alpha = one;
                font->mtl->b8 = 1;
                font->mtl->c0 = 1;
                font->mtl->tid = tid;
                fn_80024CD4(font->mtl);
                return font;
            }
            NuFileWrite(tid);
        }
    }
    fn_80009970(lbl_80191D80, 988)(lbl_80191DB4, name);
    *buf = start;
    return NULL;
}

/* NEAR MISS 120/137: GCC folds g+0x50/+4 into g+0x54; r6/r7 swap in the 0x44 copy. */
struct rndr_vec3_s { s32 a, b, c; };
typedef struct rndr_geom_s {
    u8 type;          /* 0x00 */
    u32 flags;        /* 0x04 */
    u16 v08;          /* 0x08 */
    s32 count;        /* 0x0c */
    s32* p10;         /* 0x10 */
    s32 v14;
    s32 v18;
    s32 v1c;
    s32 v20;
    s32 v24;
    s32 v28;
    u16 v2c;
    s32 v30;
    s32 v34;
    u16 v38;
    s32 v3c;
    u16 v40;
    struct rndr_vec3_s v44; /* 0x44 */
} RNDR_GEOM;
extern void fn_80030210(VARIPTR* buf, RNDR_GEOM* src, s32 a, RNDR_GEOM* dst);
extern void fn_800303D8(VARIPTR* buf, RNDR_GEOM* src, s32 a, RNDR_GEOM* dst);
extern void fn_8002F220(VARIPTR* buf, RNDR_GEOM* src, s32 a, RNDR_GEOM* dst);
extern void fn_8002F744(VARIPTR* buf, RNDR_GEOM* src, s32 a, RNDR_GEOM* dst);
extern void fn_8002FDEC(VARIPTR* buf, RNDR_GEOM* src, s32 a, RNDR_GEOM* dst);

s32 fn_8002BB60(VARIPTR* buf, s32 a1, s32 a2, RNDR_GEOM* src, RNDR_GEOM** out)
{
    RNDR_GEOM* g;
    s32* p;
    if (a2 != 0 && src->count > 1) {
        g = (RNDR_GEOM*)((buf->addr + 3) & ~3);
        p = (s32*)(g + 1);
        buf->addr = (u32)(p + 1);
        g->type = 1;
        g->flags = src->flags;
        g->count = 1;
        g->p10 = p;
        g->v14 = src->v14;
        g->v18 = src->v18;
        g->v1c = src->v1c;
        g->v20 = 0;
        g->v24 = src->v24;
        g->v28 = src->v28;
        g->v08 = src->v08;
        g->v2c = src->v2c;
        g->v30 = src->v30;
        g->v34 = src->v34;
        g->v38 = src->v38;
        g->v3c = src->v3c;
        g->v40 = src->v40;
        g->v44 = src->v44;
        if (src->flags & 0x1000) {
            fn_80030210(buf, src, a2, g);
        } else {
            fn_800303D8(buf, src, a2, g);
        }
        src = g;
    }
    g = (RNDR_GEOM*)((buf->addr + 3) & ~3);
    p = (s32*)(g + 1);
    buf->addr = (u32)(p + 1);
    g->type = 1;
    g->flags = src->flags;
    g->count = 1;
    g->p10 = p;
    g->v18 = src->v18;
    g->v1c = src->v1c;
    g->v20 = 0;
    g->v24 = src->v24;
    g->v28 = src->v28;
    g->v08 = src->v08;
    g->v30 = 0;
    g->v2c = 0;
    g->v34 = src->v34;
    g->v44 = src->v44;
    if (src->flags & 0x1000) {
        if (src->flags & 0x20000) {
            fn_8002F744(buf, src, a1, g);
        } else {
            g->v14 = src->v14;
            fn_8002F220(buf, src, a1, g);
        }
    } else {
        g->v14 = src->v14;
        fn_8002FDEC(buf, src, a1, g);
    }
    *out = g;
    return 0;
}

extern void fn_8002DB70(void);
extern void fn_8000FD18(s32 a, VARIPTR b);
extern void fn_80012C44(s32 a, VARIPTR b, s32 c);
extern void fn_8001280C(void);
extern void fn_800128B0(void);
extern s32 fn_8000A788(void);
extern VARIPTR lbl_8040AD00;
extern u32 lbl_8040ACFC;
extern u32 lbl_8040AD88;

void fn_8002BD84(s32 a, VARIPTR* b)
{
    VARIPTR v = *b;
    fn_8002DB70();
    lbl_801B0F88.count = 0;
    lbl_801B0F88.p4c4 = lbl_801B0F88.buf4f8;
    lbl_801B0F88.v4a4 = 0;
    lbl_801B0F88.v508 = 0;
    fn_8000FD18(a, v);
    lbl_801B0F88.flags |= 0x08000000;
    lbl_801B0F88.v480 = 0;
    fn_80012C44(a, v, 0x40);
    lbl_8040ACF0 = &lbl_8040AD00;
    lbl_8040ACFC = lbl_8040AD88;
    fn_8001280C();
    fn_800128B0();
    lbl_801B0F88.v540 = fn_8000A788();
}
