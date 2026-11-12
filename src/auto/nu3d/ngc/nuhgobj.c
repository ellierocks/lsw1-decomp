#include "types.h"

extern void NuStrCatW(char *dst, char *name, char *fmt, s32 max);
extern s32 NuFileClose(char *name);
extern s32 NuFileLoadBuffer(char *name, void *buf, s32 size);
extern s32 fn_80029EC8(void);
extern void fn_80011864(void *p);

struct nuscnload_s {
    s32 size;   /* 0x00 */
    u32 start;  /* 0x04 */
    u32 *buf;   /* 0x08 */
    u32 end;    /* 0x0C */
    char *path; /* 0x10 */
};

extern char lbl_801919E0[];
extern struct nuscnload_s lbl_8021F9E4;
extern u32 lbl_8040A348;
extern u32 *lbl_8040A34C;
extern u32 lbl_8040A350;
extern u32 lbl_8040A354;
extern s32 lbl_8040A358;
extern u32 lbl_8040A35C;
extern u32 lbl_8040A360;
extern s32 lbl_80407EA8;

static inline s32 NuSceneLoadFile(char *path, u32 *buf, u32 end)
{
    s32 size;
    u32 start;

    if (NuFileClose(path) == 0) {
        return 0;
    }
    start = ((*buf + 0x1F) & ~0x1F) + 0x10;
    *buf = start;
    lbl_8040A360 = start;
    size = NuFileLoadBuffer(path, (void *)start, end - *buf);
    lbl_8021F9E4.size = size;
    lbl_8021F9E4.path = path;
    lbl_8021F9E4.start = start;
    lbl_8021F9E4.end = end;
    lbl_8021F9E4.buf = buf;
    *buf += size;
    lbl_8040A348 = *buf;
    lbl_8040A34C = buf;
    lbl_8040A354 = end;
    if (lbl_8040A358 != 0) {
        lbl_8040A350 = lbl_8040A35C;
    } else {
        lbl_8040A350 = end;
    }
    return size;
}

s32 fn_8002A4B0(u32 *buf, u32 *bufend, char *name)
{
    char path[0x200];
    u32 end = *bufend;

    NuStrCatW(path, name, lbl_801919E0, 0x200);
    if (NuSceneLoadFile(path, buf, end) != 0) {
        return fn_80029EC8();
    }
    return 0;
}

void fn_8002A5A0(u8 *p)
{
    void *q = *(void **)(p + 0x1A0);

    if (q != NULL && lbl_80407EA8 != 0) {
        fn_80011864(q);
    }
}

void fn_8002A5D8(void)
{
}

extern void NuMemFree(void *p, char *file, s32 line);
extern void fn_80015E48(void *texanims);
extern void fn_80015D60(void *texanim);
extern void fn_80024D44(struct numtl_s *mtl);
extern void fn_80024C64(struct numtl_s *mtl);
extern void NuFileWrite(s16 tid);
extern char lbl_80191AC4[];

struct numtl_s {
    u8 pad0[0xB4];
    struct numtl_s *next; /* 0xB4 */
};

struct nuhgobjpair_s {
    void *a; /* 0x00 */
    void *b; /* 0x04 */
};

struct nuhgobjlist_s {
    void *a;                    /* 0x00 */
    void *b;                    /* 0x04 */
    struct nuhgobjpair_s *pairs; /* 0x08 */
    u8 pad0C[2];
    u8 npairs;                  /* 0x0E */
    u8 term;                    /* 0x0F: 0xFF ends the list */
};

struct nuhgobj_s {
    s16 *tids;                   /* 0x00 */
    s32 numtid;                  /* 0x04 */
    struct numtl_s **mtls;       /* 0x08 */
    s32 nummtl;                  /* 0x0C */
    u32 flags;                   /* 0x10 */
    void *alloc14;               /* 0x14 */
    void *alloc18;               /* 0x18 */
    void *alloc1C;               /* 0x1C */
    void *alloc20;               /* 0x20 */
    u8 pad24[4];
    void *alloc28;               /* 0x28 */
    void *alloc2C;               /* 0x2C */
    void *alloc30;               /* 0x30 */
    u8 pad34[8];
    struct nuhgobjlist_s *lists; /* 0x3C */
    u8 pad40[0x2C];
    void *alloc6C;               /* 0x6C */
    s32 numtexanims;             /* 0x70 */
    u8 *texanims;                /* 0x74: 0x20-byte entries */
    void *alloc78;               /* 0x78 */
};

void fn_8002A5DC(struct nuhgobj_s *hgobj)
{
    struct nuhgobjlist_s *l;
    s32 i;

    if (hgobj->texanims != NULL) {
        fn_80015E48(hgobj->texanims);
    }
    if (hgobj->lists != NULL) {
        for (l = hgobj->lists; l->term != 0xFF; l++) {
            if (l->a != NULL) {
                NuMemFree(l->a, lbl_80191AC4, 0x3E);
            }
            if (l->b != NULL) {
                NuMemFree(l->b, lbl_80191AC4, 0x41);
            }
            if (l->pairs != NULL) {
                while (l->npairs != 0) {
                    l->npairs--;
                    if (l->pairs[l->npairs].a != NULL) {
                        NuMemFree(l->pairs[l->npairs].a, lbl_80191AC4, 0x47);
                    }
                    if (l->pairs[l->npairs].b != NULL) {
                        NuMemFree(l->pairs[l->npairs].b, lbl_80191AC4, 0x49);
                    }
                }
                NuMemFree(l->pairs, lbl_80191AC4, 0x4B);
            }
        }
        NuMemFree(hgobj->lists, lbl_80191AC4, 0x4F);
    }
    if (hgobj->alloc30 != NULL) {
        NuMemFree(hgobj->alloc30, lbl_80191AC4, 0x53);
    }
    if (hgobj->alloc14 != NULL) {
        NuMemFree(hgobj->alloc14, lbl_80191AC4, 0x55);
    }
    if (hgobj->alloc18 != NULL) {
        NuMemFree(hgobj->alloc18, lbl_80191AC4, 0x57);
    }
    if (hgobj->alloc1C != NULL) {
        NuMemFree(hgobj->alloc1C, lbl_80191AC4, 0x59);
    }
    if (hgobj->mtls != NULL) {
        for (i = 0; i < hgobj->nummtl; i++) {
            while (hgobj->mtls[i]->next != NULL) {
                fn_80024D44(hgobj->mtls[i]);
            }
            fn_80024C64(hgobj->mtls[i]);
        }
        NuMemFree(hgobj->mtls, lbl_80191AC4, 0x87);
    }
    if (hgobj->tids != NULL) {
        for (i = 0; i < hgobj->numtid; i++) {
            NuFileWrite(hgobj->tids[i]);
        }
        NuMemFree(hgobj->tids, lbl_80191AC4, 0x8F);
    }
    if (hgobj->texanims != NULL) {
        fn_80015E48(hgobj->texanims);
    }
    for (i = 0; i < hgobj->numtexanims; i++) {
        fn_80015D60(hgobj->texanims + i * 0x20);
    }
    if (hgobj->texanims != NULL) {
        NuMemFree(hgobj->texanims, lbl_80191AC4, 0x98);
    }
    if (hgobj->alloc78 != NULL) {
        NuMemFree(hgobj->alloc78, lbl_80191AC4, 0x9B);
    }
    if (hgobj->alloc2C != NULL) {
        NuMemFree(hgobj->alloc2C, lbl_80191AC4, 0x9E);
    }
    if (hgobj->alloc20 != NULL) {
        NuMemFree(hgobj->alloc20, lbl_80191AC4, 0xA1);
    }
    if (hgobj->alloc28 != NULL) {
        NuMemFree(hgobj->alloc28, lbl_80191AC4, 0xA4);
    }
    if (hgobj->alloc6C != NULL) {
        NuMemFree(hgobj->alloc6C, lbl_80191AC4, 0xA7);
    }
    NuMemFree(hgobj, lbl_80191AC4, 0xA9);
}
