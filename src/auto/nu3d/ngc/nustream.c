#include "types.h"

extern void fn_8002C3A0(s32 a, s32 b, s32 c);

extern void fn_8002C644(s32 a, s32 b, s32 c);

struct nustream_mtx_s {
    f32 m[16];
};

/* +0x48 target of fn_8002CAC8/fn_8002CD0C (flag word at +0 via fn_8002CD00). */
struct nustream_xf_s {
    s32 flag;
    f32 pos[3];
    struct nustream_mtx_s mtx;
};

struct nustream_obj_s {
    u8 pad0[0x18];
    f32 scale; /* 0x18: fn_8002CAC8 */
    u8 pad1c[0x10];
    f32* x; /* 0x2c: fn_8002C9E4 */
    f32* y; /* 0x30: fn_8002C9E4 */
    u8 pad34[0x14];
    s32* p;
};

#define XF(o) ((struct nustream_xf_s*)(o)->p)

extern s32 lbl_80407CC0;
extern s32 lbl_8040AD20;
extern s32 lbl_8040AD24;
extern s32 lbl_8040AD28;
extern s32 lbl_8040AD2C;
extern f32 lbl_80407CA8;
extern f32 lbl_80407CAC;
extern f32 lbl_80407CB0;
extern f32 lbl_80407CB4;

void fn_8002C9E4(void* a, struct nustream_obj_s* b, f32 x, f32 y)
{
    if (lbl_80407CC0 == 4) {
        *b->x = x;
        *b->y = y;
        return;
    }
    *b->x = x * (f32)lbl_8040AD28 / (f32)lbl_8040AD20;
    *b->y = y * 0.5f * (f32)lbl_8040AD2C / (f32)lbl_8040AD24;
}

void fn_8002CAC8(void* a, struct nustream_obj_s* b, f32 x, f32 y, f32 z)
{
    if (lbl_80407CC0 != 4) {
        x = x * 640.0f * lbl_80407CA8 + (320.0f - lbl_80407CB0 * 320.0f);
        y = y * 448.0f * lbl_80407CAC + (224.0f - lbl_80407CB4 * 224.0f) - b->scale * *b->y;
    }
    XF(b)->pos[0] = x;
    XF(b)->pos[1] = y;
    XF(b)->pos[2] = z;
}

extern f32 lbl_80191E30[];
void fn_8002CB94(struct nustream_obj_s* b, f32 x, f32 y)
{
    if (lbl_80407CC0 == 4) { *b->x = x; *b->y = y; return; }
    *b->x = x * (f32)lbl_8040AD28 / (f32)lbl_8040AD20;
    *b->y = y * lbl_80191E30[0] * (f32)lbl_8040AD2C / (f32)lbl_8040AD24;
}

void fn_8002CC74(struct nustream_obj_s* b, f32 x, f32 y, f32 z)
{
    if (lbl_80407CC0 != 4) {
        x = x * 640.0f * lbl_80407CA8 + (320.0f - lbl_80407CB0 * 320.0f);
        y = y * 448.0f * lbl_80407CAC + (224.0f - lbl_80407CB4 * 224.0f) - b->scale * *b->y;
    }
    XF(b)->pos[0] = x;
    XF(b)->pos[1] = y;
    XF(b)->pos[2] = z;
}

void fn_8002CD0C(struct nustream_obj_s* b, struct nustream_mtx_s* m)
{
    XF(b)->mtx = *m;
}

extern s32 lbl_80407EC8;

void fn_8002CEF0(void)
{
}

void fn_8002CB8C(s32 a)
{
    lbl_80407EC8 = a;
}

void fn_8002CB54(void* a, struct nustream_obj_s* b, s32 c)
{
    *b->p = c;
}

void fn_8002CD00(struct nustream_obj_s* a, s32 c)
{
    *a->p = c;
}

void fn_8002CDA8(s32 a, s32 b)
{
    fn_8002C644(a, b, 0);
}

void fn_8002CDCC(s32 a, s32 b)
{
    fn_8002C644(a, b, 2);
}

void fn_8002CB60(s32 a, s32 b, s32 c, s32 d)
{
    fn_8002C644(b, c, d);
}

void fn_8002CD7C(u8* a, s32 b)
{
    fn_8002C3A0(*(s32*)(a + 0x38), *(s32*)(a + 0x10), b);
}

/* Material fields copied by fn_8002CDF0 (struct numtl_s offsets, see nu3dx.h). */
struct nustream_mtlattr_s {
    u32 w[5];
};

struct nustream_mtl_view_s {
    u8 pad0[0xB8];
    struct nustream_mtlattr_s attr; /* 0xB8 */
    u8 padCC[0x28];
    s32 tid; /* 0xF4 */
    u16 unkF8;
};

struct nustream_gobj_s {
    u8 pad0[6];
    u16 flags; /* 0x06 */
    s32 size; /* 0x08 */
    u8 padC[0x28];
    s32 p34;
    s32 p38;
    u8 pad3C[4];
    struct nustream_mtl_view_s* mtl; /* 0x40 */
};

extern void fn_8000759C(void* dst, void* src, s32 size);
extern struct nustream_mtl_view_s* fn_80024C18(s32 a);
extern void fn_80024CD4(struct nustream_mtl_view_s* mtl);

struct nustream_gobj_s* fn_8002CDF0(struct nustream_gobj_s* g, s32 flags, s32 c, u32* cur, u32* end)
{
    struct nustream_gobj_s* n;

    if (!(flags & 8)) {
        return g;
    }
    n = (struct nustream_gobj_s*)((*cur + 0xF) & ~0xF);
    *cur = (u32)n;
    *cur = (u32)n + g->size;
    if (*cur < *end) {
        fn_8000759C(n, g, g->size);
        n->flags |= 1;
        n->p34 = n->p34 - (s32)g + (s32)n;
        n->p38 = n->p38 - (s32)g + (s32)n;
    }
    n->mtl = fn_80024C18(1);
    n->mtl->attr = g->mtl->attr;
    n->mtl->tid = g->mtl->tid;
    n->mtl->unkF8 = g->mtl->unkF8;
    fn_80024CD4(n->mtl);
    return n;
}

/* Double-buffered render stream ("Render stream buffer overflow."). */
extern u32 lbl_8040A368; /* buffer 0 */
extern u32 lbl_8040A36C; /* buffer 1 */
extern u32 lbl_8040A370; /* bytes used this frame */
extern u32 lbl_8040A374; /* peak bytes used */
extern s32 lbl_80407ED0; /* current buffer index */
extern u32 lbl_80407ED4; /* buffer size */
extern u32 lbl_80407ED8; /* bytes used last frame */
extern u32 lbl_8040AD00; /* write cursor */
extern u32 lbl_8040AD88; /* buffer end */
extern char lbl_80191EE8[];
extern char lbl_80191F18[];
extern void (*fn_80009948(char* file, s32 line))();
extern u32 NuMemAlloc(u32 size, char* file, s32 line);
extern void* memset();

void fn_8002CEF4(void)
{
    switch (lbl_80407ED0) {
    case 0:
        lbl_8040A370 = lbl_8040AD00 - lbl_8040A368;
        break;
    case 1:
        lbl_8040A370 = lbl_8040AD00 - lbl_8040A36C;
        break;
    }
    lbl_80407ED8 = lbl_8040A370;
    if (lbl_8040A370 > lbl_8040A374) {
        lbl_8040A374 = lbl_8040A370;
    }
    if (lbl_8040A370 > lbl_80407ED4) {
        fn_80009948(lbl_80191EE8, 0x74)(lbl_80191F18);
        for (;;) {
        }
    }
    lbl_80407ED0 = (lbl_80407ED0 + 1) % 2;
    switch (lbl_80407ED0) {
    case 0:
        lbl_8040AD00 = lbl_8040A368;
        lbl_8040AD88 = lbl_8040AD00 + lbl_80407ED4;
        break;
    case 1:
        lbl_8040AD00 = lbl_8040A36C;
        lbl_8040AD88 = lbl_8040AD00 + lbl_80407ED4;
        break;
    }
    if (lbl_8040AD00 & 0xF) {
        lbl_8040AD00 = (lbl_8040AD00 + 0xF) & ~0xF;
    }
}

void fn_8002CFF8(u32 size, u32* mem)
{
    if (mem == NULL) {
        lbl_8040A368 = NuMemAlloc(size, lbl_80191EE8, 0x88);
        lbl_8040A36C = NuMemAlloc(size, lbl_80191EE8, 0x89);
    } else {
        u32 p0 = (*mem + 0x1F) & ~0x1F;
        u32 p1;
        *mem = p0;
        lbl_8040A368 = p0;
        p1 = (p0 + size + 0x1F) & ~0x1F;
        *mem = p1;
        lbl_8040A36C = p1;
        *mem = p1 + size;
    }
    lbl_80407ED0 = 0;
    lbl_80407ED4 = size;
    memset(lbl_8040A368, 0, size);
    memset(lbl_8040A36C, 0, size);
    switch (lbl_80407ED0) {
    case 0:
        lbl_8040AD00 = lbl_8040A368;
        lbl_8040AD88 = lbl_8040AD00 + lbl_80407ED4;
        break;
    case 1:
        lbl_8040AD00 = lbl_8040A36C;
        lbl_8040AD88 = lbl_8040AD00 + lbl_80407ED4;
        break;
    }
    if (lbl_8040AD00 & 0xF) {
        lbl_8040AD00 = (lbl_8040AD00 + 0xF) & ~0xF;
    }
}
