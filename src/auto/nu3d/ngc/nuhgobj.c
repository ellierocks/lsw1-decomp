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

struct numtl_s;

extern void NuMemFree(void *p, char *file, s32 line);
extern void fn_80015E48(void *texanims);
extern void fn_80015D60(void *texanim);
extern void fn_80024D44(struct numtl_s *mtl);
extern void fn_80024C64(struct numtl_s *mtl);
extern void NuFileWrite(s16 tid);
extern char lbl_80191AC4[];

struct nuvec_s { f32 x, y, z; };

struct numtl_s {
    u8 pad0[0xA4];
    u32 attrib;           /* 0xA4 */
    u8 padA8[0xC];
    struct numtl_s *next; /* 0xB4 */
    u8 padB8[0x3C];
    s32 tid;              /* 0xF4 */
};

struct nughgtex_s {
    u8 pad0[0xA];
    u16 nlevels;   /* 0x0A */
    s32 levels[11]; /* 0x0C: self-relative offsets */
    s32 pal;       /* 0x38: self-relative offset */
};

struct nuhgobjpair_s {
    void *a; /* 0x00 */
    void *b; /* 0x04 */
};

struct nuhgobjlist_s {
    void *a;                    /* 0x00 */
    void *b;                    /* 0x04 */
    struct nuhgobjpair_s *pairs; /* 0x08 */
    u8 hasa;                    /* 0x0C */
    u8 hasb;                    /* 0x0D */
    u8 npairs;                  /* 0x0E */
    u8 term;                    /* 0x0F: 0xFF ends the list */
};

struct nughgjoint_s { u8 pad0[0x4C]; u32 p4C; u8 pad50[0x10]; };
struct nughgthing_s { u8 pad0[0x40]; u32 p40; u8 pad44[0xC]; };

struct nughgmeshgeom_s { u8 pad0[0x1A0]; void *p1A0; };
struct nughgmesh_s {
    struct nughgmeshgeom_s *geom; /* 0x00 */
    s32 p4;                       /* 0x04 */
    u32 p8;                       /* 0x08 */
};

struct nughglayer_s {
    u32 p0;                    /* 0x00 */
    struct nughgmesh_s **a;    /* 0x04 */
    struct nughgmesh_s *b;     /* 0x08 */
    struct nughgmesh_s **c;    /* 0x0C */
    struct nughgmesh_s *d;     /* 0x10 */
};

struct nughgpoint_s { u32 p0; u32 p4; };
struct nughgpoints_s { u32 n; struct nughgpoint_s *pts; };

struct nughgtexanim_s {
    struct nughgtexanim_s *next; /* 0x00 */
    struct nughgtexanim_s *prev; /* 0x04 */
    s16 *tids;                   /* 0x08 */
    s16 ntids;                   /* 0x0C */
    void *mtl;                   /* 0x10 */
    void *anim;                  /* 0x14 */
    u32 p18;                     /* 0x18 */
    char *prog;                  /* 0x1C */
};

struct nughghdr_s {
    s32 size;        /* 0x00 */
    s32 version;     /* 0x04 */
    s32 ntid;        /* 0x08 */
    s32 tids;        /* 0x0C */
    s32 nmtl;        /* 0x10 */
    s32 mtls;        /* 0x14 */
    s32 njoint;      /* 0x18 */
    s32 joints;      /* 0x1C */
    s32 o20;         /* 0x20 */
    s32 o24;         /* 0x24 */
    s32 n28;         /* 0x28 */
    s32 o2C;         /* 0x2C */
    s32 o30;         /* 0x30 */
    s32 v34;         /* 0x34 */
    s32 n38;         /* 0x38 */
    s32 o3C;         /* 0x3C */
    s32 nthing;      /* 0x40 */
    s32 things;      /* 0x44 */
    s32 nlayer;      /* 0x48 */
    s32 layers;      /* 0x4C */
    s32 lists;       /* 0x50 */
    f32 f54;         /* 0x54 */
    f32 f58;         /* 0x58 */
    struct nuvec_s v5C; /* 0x5C */
    struct nuvec_s v68; /* 0x68 */
    f32 f74;         /* 0x74 */
    f32 f78;         /* 0x78 */
    f32 f7C;         /* 0x7C */
    s32 points;      /* 0x80 */
    f32 f84;         /* 0x84 */
    s32 ntexanim;    /* 0x88 */
    s32 texanims;    /* 0x8C */
    s32 o90;         /* 0x90 */
};

struct nuhgobj_s {
    s16 *tids;                      /* 0x00 */
    s32 numtid;                     /* 0x04 */
    struct numtl_s **mtls;          /* 0x08 */
    s32 nummtl;                     /* 0x0C */
    u32 flags;                      /* 0x10 */
    struct nughgjoint_s *joints;    /* 0x14 */
    u8 *p18;                        /* 0x18 */
    u8 *p1C;                        /* 0x1C */
    u8 *p20;                        /* 0x20 */
    struct nughglayer_s *layers;    /* 0x24 */
    struct nughgthing_s *things;    /* 0x28 */
    u8 *p2C;                        /* 0x2C */
    u8 *p30;                        /* 0x30 */
    s32 v34;                        /* 0x34 */
    f32 f38;                        /* 0x38 */
    struct nuhgobjlist_s *lists;    /* 0x3C */
    f32 f40;                        /* 0x40 */
    f32 f44;                        /* 0x44 */
    struct nuvec_s v48;             /* 0x48 */
    struct nuvec_s v54;             /* 0x54 */
    f32 f60;                        /* 0x60 */
    f32 f64;                        /* 0x64 */
    f32 f68;                        /* 0x68 */
    struct nughgpoints_s *points;   /* 0x6C */
    s32 numtexanims;                /* 0x70 */
    struct nughgtexanim_s *texanims; /* 0x74 */
    void *alloc78;                  /* 0x78 */
    u8 njoint;                      /* 0x7C */
    u8 n7D;                         /* 0x7D */
    u8 nlayer;                      /* 0x7E */
    u8 nthing;                      /* 0x7F */
    u8 n80;                         /* 0x80 */
    u8 *p84;                        /* 0x84 */
    u8 pad88[4];
    u8 *base;                       /* 0x8C */
    u32 end;                        /* 0x90 */
    s32 nlists;                     /* 0x94 */
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
    if (hgobj->p30 != NULL) {
        NuMemFree(hgobj->p30, lbl_80191AC4, 0x53);
    }
    if (hgobj->joints != NULL) {
        NuMemFree(hgobj->joints, lbl_80191AC4, 0x55);
    }
    if (hgobj->p18 != NULL) {
        NuMemFree(hgobj->p18, lbl_80191AC4, 0x57);
    }
    if (hgobj->p1C != NULL) {
        NuMemFree(hgobj->p1C, lbl_80191AC4, 0x59);
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
        fn_80015D60(&hgobj->texanims[i]);
    }
    if (hgobj->texanims != NULL) {
        NuMemFree(hgobj->texanims, lbl_80191AC4, 0x98);
    }
    if (hgobj->alloc78 != NULL) {
        NuMemFree(hgobj->alloc78, lbl_80191AC4, 0x9B);
    }
    if (hgobj->p2C != NULL) {
        NuMemFree(hgobj->p2C, lbl_80191AC4, 0x9E);
    }
    if (hgobj->p20 != NULL) {
        NuMemFree(hgobj->p20, lbl_80191AC4, 0xA1);
    }
    if (hgobj->things != NULL) {
        NuMemFree(hgobj->things, lbl_80191AC4, 0xA4);
    }
    if (hgobj->points != NULL) {
        NuMemFree(hgobj->points, lbl_80191AC4, 0xA7);
    }
    NuMemFree(hgobj, lbl_80191AC4, 0xA9);
}

extern void (*fn_80009948(char *file, s32 line))(char *fmt, ...);
extern void (*fn_80009970(char *file, s32 line))(char *fmt, ...);
extern void *memset(void *p, s32 c, u32 n);
extern s16 fn_80024F44(void *tex);
extern void fn_80024D34(void *mtl);
extern void NuTexSetTextureDirect(void);
extern void NuTexGetDDS(void);
extern void *NuTexAnimProgFind(char *name);
extern void *fn_80015C88(u8 **buf, void *mtl, s16 *tids, void *prog);
extern void fn_80015DD0(void *texanims);
extern void fn_8002963C(void *p, void *mtls, s32 a, s32 b);

extern char lbl_80191B00[];
extern char lbl_80191B04[];
extern char lbl_80191B2C[];
extern char lbl_80191B60[];
extern char lbl_80191B6C[];
extern char str_NuGHGReadEx_BufferOverflow[];
extern s32 lbl_80407EB0;

static inline void NuGHGFixMesh(struct nughgmesh_s **slot, u32 off, u8 *base, struct nuhgobj_s *obj)
{
    struct nughgmesh_s *m = (struct nughgmesh_s *)(off + (u32)base);

    *slot = m;
    *(u32 *)(off + (u32)base) += (u32)base;
    m->p4 = 0;
    m->p8 += (u32)base;
    (*(struct nughgmeshgeom_s **)(off + (u32)base))->p1A0 = obj->p84;
}

static inline u8 *NuGHGPtr(u8 *base, s32 off)
{
    return base + off;
}

struct nuhgobj_s *NuGHGReadEx(char *name, u32 *bufp, u32 *endp)
{
    char path[0x200];
    u8 *buf[1];
    u32 end = *endp;
    struct nuhgobj_s *obj = NULL;
    struct nughghdr_s *hdr;
    u8 *base;
    s32 i;
    u8 *tab;
    s32 j;

    NuStrCatW(path, name, lbl_80191B00, 0x200);
    buf[0] = (u8 *)((*bufp + 0x1F) & ~0x1F);
    base = buf[0];
    if (NuFileLoadBuffer(path, base, 0x7FFFFFFF) > 0) {
        hdr = (struct nughghdr_s *)buf[0];
        if (hdr->version < lbl_80407EB0) {
            fn_80009948(lbl_80191AC4, 0x3BB)(lbl_80191B04, path);
            for (;;) {
            }
        }
        if (hdr->version > lbl_80407EB0) {
            fn_80009948(lbl_80191AC4, 0x3C0)(lbl_80191B2C);
            for (;;) {
            }
        }
        fn_80009970(lbl_80191AC4, 0x3C4)(lbl_80191B60, path);
        obj = (struct nuhgobj_s *)((u32)(buf[0] + *(s32 *)buf[0] + 0xF) & ~0xF);
        buf[0] = (u8 *)(obj + 1);
        memset(obj, 0, sizeof(struct nuhgobj_s));
        obj->flags = 1;
        obj->p84 = (u8 *)(hdr->o90 + (u32)base);
        if (hdr->tids != 0) {
            s32 *offs;

            obj->tids = (s16 *)buf[0];
            buf[0] += hdr->ntid * 4;
            obj->numtid = hdr->ntid;
            tab = NuGHGPtr(base, hdr->tids);
            offs = (s32 *)tab;
            for (i = 0; i < obj->numtid; i++) {
                struct nughgtex_s *tex;
                s32 k;

                tex = (struct nughgtex_s *)NuGHGPtr(base, offs[i]);

                for (k = 0; k < tex->nlevels; k++) {
                    tex->levels[k] = tex->levels[k] != 0 ? tex->levels[k] + (s32)&tex->levels[k] : 0;
                }
                tex->pal = tex->pal != 0 ? tex->pal + (s32)&tex->pal : 0;
                obj->tids[i] = fn_80024F44(tex);
            }
        }
        if (hdr->mtls != 0) {
            s32 *offs;

            obj->mtls = (struct numtl_s **)buf[0];
            buf[0] += hdr->nmtl * 4;
            obj->nummtl = hdr->nmtl;
            offs = (s32 *)(base + hdr->mtls);
            for (i = 0; i < obj->nummtl; i++) {
                struct numtl_s *next;

                obj->mtls[i] = (struct numtl_s *)(base + offs[i]);
                if (obj->mtls[i]->tid != 0) {
                    obj->mtls[i]->tid = obj->tids[obj->mtls[i]->tid - 1];
                }
                NuTexSetTextureDirect();
                next = obj->mtls[i]->next;
                obj->mtls[i]->attrib |= 0x400;
                fn_80024D34(obj->mtls[i]);
                obj->mtls[i]->next = next;
                NuTexGetDDS();
            }
            for (i = 0; i < obj->nummtl; i++) {
                s32 n = (s32)obj->mtls[i]->next;

                if (n == -1) {
                    obj->mtls[i]->next = NULL;
                } else if ((u32)(n - 1) < 0xFFFF) {
                    obj->mtls[i]->next = obj->mtls[n];
                }
            }
            NuTexSetTextureDirect();
            for (i = 0; i < obj->nummtl; i++) {
                obj->mtls[i]->attrib &= ~0x400;
            }
            NuTexGetDDS();
        }
        buf[0] = (u8 *)(((u32)buf[0] + 0xF) & ~0xF);
        obj->f38 = hdr->f84;
        obj->p30 = base + hdr->o30;
        obj->v34 = hdr->v34;
        obj->njoint = hdr->njoint;
        obj->joints = (struct nughgjoint_s *)(base + hdr->joints);
        obj->p18 = base + hdr->o20;
        obj->p1C = base + hdr->o24;
        obj->n7D = hdr->n28;
        obj->p20 = base + hdr->o2C;
        for (i = 0; i < obj->njoint; i++) {
            if (obj->joints[i].p4C != 0) {
                obj->joints[i].p4C += (u32)base;
            }
        }
        if (hdr->n38 != 0) {
            obj->n80 = hdr->n38;
            obj->p2C = base + hdr->o3C;
        }
        if (hdr->nthing != 0) {
            obj->nthing = hdr->nthing;
            obj->things = (struct nughgthing_s *)(base + hdr->things);
            for (i = 0; i < obj->nthing; i++) {
                if (obj->things[i].p40 != 0) {
                    obj->things[i].p40 += (u32)base;
                }
            }
        }
        {
            struct nughglayer_s *l;

            obj->layers = l = (struct nughglayer_s *)(base + hdr->layers);
            obj->nlayer = hdr->nlayer;
            for (i = 0; i < obj->nlayer; i++, l++) {
                if (l->p0 != 0) {
                    l->p0 += (u32)base;
                }
                if (l->a != NULL) {
                    struct nughgmesh_s **arr = (struct nughgmesh_s **)((u32)l->a + (u32)base);

                    l->a = arr;
                    for (j = 0; j < obj->njoint; j++) {
                        if (arr[j] != NULL) {
                            NuGHGFixMesh(&arr[j], (u32)arr[j], base, obj);
                        }
                    }
                }
                if (l->b != NULL) {
                    NuGHGFixMesh(&l->b, (u32)l->b, base, obj);
                }
                if (l->c != NULL) {
                    struct nughgmesh_s **arr = (struct nughgmesh_s **)((u32)l->c + (u32)base);

                    l->c = arr;
                    for (j = 0; j < obj->njoint; j++) {
                        if (arr[j] != NULL) {
                            NuGHGFixMesh(&arr[j], (u32)arr[j], base, obj);
                        }
                    }
                }
                if (l->d != NULL) {
                    NuGHGFixMesh(&l->d, (u32)l->d, base, obj);
                }
            }
        }
        obj->f40 = hdr->f54;
        obj->f44 = hdr->f58;
        obj->v48 = hdr->v5C;
        obj->v54 = hdr->v68;
        obj->f60 = hdr->f74;
        obj->f64 = hdr->f78;
        obj->f68 = hdr->f7C;
        obj->points = NULL;
        if (hdr->points != 0) {
            struct nughgpoints_s *p = (struct nughgpoints_s *)(hdr->points + (u32)base);

            obj->points = p;
            p->pts = (struct nughgpoint_s *)((u32)p->pts + (u32)base);
            for (i = 0; i < p->n; i++) {
                {
                    struct nughgpoint_s *pt = &p->pts[i];

                    pt->p4 += (u32)base;
                }
            }
        }
        obj->numtexanims = hdr->ntexanim;
        if (obj->numtexanims != 0) {
            obj->texanims = (struct nughgtexanim_s *)(base + hdr->texanims);
            for (i = 0; i < obj->numtexanims; i++) {
                struct nughgtexanim_s *ta = &obj->texanims[i];
                void *prog;

                ta->prev = NULL;
                ta->next = NULL;
                ta->tids = (s16 *)(base + (u32)ta->tids);
                for (j = 0; j < ta->ntids; j++) {
                    ta->tids[j] = obj->tids[ta->tids[j]];
                }
                ta->mtl = obj->mtls[(s32)ta->mtl];
                ta->prog += (u32)base;
                ta->p18 += (u32)base;
                prog = NuTexAnimProgFind(ta->prog);
                buf[0] = (u8 *)(((u32)buf[0] + 0xF) & ~0xF);
                ta->anim = fn_80015C88(buf, ta->mtl, ta->tids, prog);
            }
            for (j = 0; j < obj->numtexanims - 1; j++) {
                obj->texanims[j].next = &obj->texanims[j] + 1;
                obj->texanims[j + 1].prev = &obj->texanims[j];
            }
            fn_80015DD0(obj->texanims);
        }
        obj->lists = NULL;
        obj->nlists = 0;
        if (hdr->lists != 0) {
            struct nuhgobjlist_s *l;

            j = 0;
            l = obj->lists = (struct nuhgobjlist_s *)(hdr->lists + (u32)base);
            for (; l->term != 0xFF; l++) {
                j++;
                if (l->hasa) {
                    l->a = (u8 *)l->a + (u32)base;
                }
                if (l->hasb) {
                    l->b = (u8 *)l->b + (u32)base;
                }
                if (l->npairs) {
                    struct nuhgobjpair_s *p;

                    p = l->pairs = (struct nuhgobjpair_s *)((u32)l->pairs + (u32)base);
                    for (i = 0; i < l->npairs; i++, p++) {
                        p->b = (u8 *)p->b + (u32)base;
                        p->a = (u8 *)p->a + (u32)base;
                    }
                }
            }
            obj->nlists = j;
        }
        buf[0] = (u8 *)(((u32)buf[0] + 0xF) & ~0xF);
        fn_80009970(lbl_80191AC4, 0x4DD)(lbl_80191B6C, path, (u32)buf[0] - *bufp);
        *bufp = (u32)buf[0];
        obj->base = base;
        obj->end = (u32)buf[0];
        if (hdr->o90 != 0) {
            obj->p84 = (u8 *)(hdr->o90 + (u32)base);
            fn_8002963C(obj->p84, obj->mtls, 0, 0);
        }
    }
    if (*bufp > end) {
        fn_80009948(lbl_80191AC4, 0x4F0)(str_NuGHGReadEx_BufferOverflow, *bufp - end);
    }
    return obj;
}
