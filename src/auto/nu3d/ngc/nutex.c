#include "types.h"

void fn_80024DB8(u8* p, u8 v)
{
    p[0xa7] = v;
}

s32 fn_80024DC0(void)
{
    return 0;
}

extern void NuStrCatW(char *dst, char *name, char *fmt, s32 max);
extern s32 NuFileClose(char *name);
extern s32 NuFileOpen(char *path, s32 mode);
extern s32 NuFileRead(s32 handle, void *data, s32 size);
extern void fn_80005764(s32 handle);
extern s32 fn_80033F30(void *data);
extern void (*fn_8000995C(char *file, s32 line))(char *fmt, ...);

struct texhdr_s {
    u8 pad0[0xA];
    u16 count;          /* 0x0A */
    void *ptrs[11];     /* 0x0C: self-relative offsets on disk */
    void *extra;        /* 0x38 */
};

/* Loads a texture file into the 32-byte-aligned buffer cursor *buf, relocates its
 * offset table (count at 0x0A, offsets from 0x0C, extra block at 0x38) and registers it. */
s32 fn_80024DC8(char *name, u8 **buf)
{
    char path[0x400];
    s32 size;
    s32 fh;
    u8 *p;
    u32 i;
    struct texhdr_s *h;

    NuStrCatW(path, name, "ctx", 0x400);
    size = NuFileClose(path);
    if (size <= 0) {
        fn_8000995C("../nu2api.legogc/nu3d/ngc/ngc_obj/../nutex.c", 0x33)("NuTexRead cant find file %s.", path);
        return 0;
    }
    fh = NuFileOpen(path, 0);
    if (fh == 0) {
        fn_8000995C("../nu2api.legogc/nu3d/ngc/ngc_obj/../nutex.c", 0x3D)("NuTexRead cant find file %s.", path);
        return 0;
    }
    p = (u8 *)(((u32)*buf + 0x1F) & ~0x1F);
    *buf = p + size;
    NuFileRead(fh, p, size);
    h = (struct texhdr_s *)p;
    for (i = 0; i < h->count; i++) {
        h->ptrs[i] = (h->ptrs[i] != NULL) ? (void *)((s32)h->ptrs[i] + (s32)&h->ptrs[i]) : NULL;
    }
    h->extra = (h->extra != NULL) ? (void *)((s32)h->extra + (s32)&h->extra) : NULL;
    fn_80005764(fh);
    return fn_80033F30(p) + 1;
}

/* Unreferenced word between this file's strings and the VuMath.h stub strings. */
static const u32 unk_8019140C[1] __attribute__((section(".rodata"))) = { 0 };

#include "numath/VuMath.h"
