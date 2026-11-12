#include "types.h"

#define _STDARG_H
#include "sn/include/va-ppc.h"
typedef __gnuc_va_list va_list;

typedef union {
    void* void_ptr;
    u32 addr;
    u8* u8_ptr;
} VARIPTR;

typedef struct numtx_s {
    f32 m[4][4];
} NUMTX;

typedef struct nucamera_s {
    NUMTX mtx;
    u8 pad[0x28];
} NUCAMERA;

extern NUMTX* NuCameraGetProjectionMtx(void);
extern void NuCameraGet(NUCAMERA* cam);
extern NUMTX lbl_8021F9A4;
extern f32 lbl_80191628[];
extern f32 lbl_8019162C[];
extern f32 lbl_8040AD70;
extern f32 lbl_8040AD74;
extern f32 lbl_8040AD78;
extern f32 lbl_8040AD7C;

void fn_80027DC8(void)
{
    NUCAMERA cam;
    NUMTX* p = NuCameraGetProjectionMtx();
    NUMTX* d = &lbl_8021F9A4;
    s32 row;

    row = 0;
    d->m[row][0] = -p->m[0][0];
    d->m[1][0] = p->m[0][1];
    d->m[2][0] = p->m[0][2];
    d->m[0][1] = -p->m[1][0];
    d->m[1][1] = p->m[1][1];
    d->m[2][1] = p->m[1][2];
    d->m[0][2] = -p->m[2][0];
    d->m[1][2] = p->m[2][1];
    d->m[2][2] = p->m[2][2];
    d->m[2][3] = 0.0f;
    d->m[1][3] = 0.0f;
    d->m[0][3] = 0.0f;
    d->m[3][3] = 1.0f;

    NuCameraGet(&cam);
    lbl_8040AD74 = cam.mtx.m[2][0];
    lbl_8040AD70 = cam.mtx.m[2][1];
    lbl_8040AD78 = cam.mtx.m[2][2];
    lbl_8040AD7C = -(cam.mtx.m[3][0] * cam.mtx.m[2][0] + cam.mtx.m[3][1] * cam.mtx.m[2][1] + cam.mtx.m[3][2] * cam.mtx.m[2][2]);
}

extern s32 fn_8001394C();
extern void fn_80014214(s32 x, s32 y);
extern void fn_80013CDC();

void fn_80027EA4(s32 x, s32 y, s32 align, char* fmt, ...)
{
    va_list args;
    s32 w;
    va_start(args, fmt);
    w = fn_8001394C(fmt, args);
    switch (align) {
    case 0:
        fn_80014214(x, y);
        break;
    case 1:
        fn_80014214(x - w, y);
        break;
    case 2:
        fn_80014214(x - (w >> 1), y);
        break;
    }
    fn_80013CDC(fmt, args);
    va_end(args);
}

struct nufont_mtl_s {
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

typedef struct nufont_s {
    u8 pad[0x18];
    struct nufont_mtl_s* mtl; /* 0x18 */
    u8 pad1c[4];
    u8 map[0x100]; /* 0x20 */
} NUFONT_NGC;

extern char lbl_801917D0[];
extern char lbl_801917D8[];
extern char lbl_8019167C[];
extern f32 lbl_801917F4[];
extern int sprintf(char* s, const char* fmt, ...);
extern int toupper();
extern s32 NuFileLoadBuffer(char* name, void* buf, s32 size);
extern s32 fn_80024DC8(char* name, VARIPTR* buf, VARIPTR* end);
extern struct nufont_mtl_s* fn_80024BF0(s32 n);
extern void fn_80024CD4(struct nufont_mtl_s* mtl);
extern void NuFileWrite(s32 tid);
extern void (*fn_80009970(char* file, s32 line))(char* fmt, ...);

NUFONT_NGC* fn_80027FAC(char* name, VARIPTR* buf, VARIPTR* end)
{
    char path[0x100];
    VARIPTR texend;
    VARIPTR start = *buf;
    s32 size;
    buf->addr = (buf->addr + 0xf) & ~0xf;
    sprintf(path, lbl_801917D0, name);
    size = NuFileLoadBuffer(path, buf->void_ptr, end->addr - buf->addr);
    if (size > 0) {
        NUFONT_NGC* font = buf->void_ptr;
        s32 tid;
        buf->addr += size;
        texend = *end;
        tid = fn_80024DC8(name, buf, &texend);
        if (tid != 0) {
            font->mtl = fn_80024BF0(1);
            if (font->mtl != NULL) {
                f32 one = lbl_801917F4[0];
                s32 c;
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
                for (c = 'a'; c <= 'z'; c++) {
                    if (font->map[c] == 0xff) {
                        font->map[c] = font->map[toupper(c)];
                    }
                }
                return font;
            }
            NuFileWrite(tid);
        }
    }
    fn_80009970(lbl_8019167C, 0x214)(lbl_801917D8, name);
    *buf = start;
    return NULL;
}
