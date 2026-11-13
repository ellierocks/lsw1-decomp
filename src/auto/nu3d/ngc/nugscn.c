#include "types.h"
#include "nu3dx/nu3dx.h"
#include "nu3dx/nuanim.h"


extern s32 lbl_80407BE8;
extern s32 lbl_80407E04;
extern s32 lbl_80407E08;
extern f32 lbl_8040AD8C;
extern f32 lbl_8040AD90;
extern s32 lbl_8040AD94;
extern u32* lbl_8040A34C;
extern u32 lbl_8040A350;
extern struct nugscn_s* lbl_8040A364;
extern char lbl_8019177C[];
extern char lbl_8019178C[];
extern char lbl_801917B4[];
extern char lbl_801918E8[];
extern char lbl_80191930[];
extern char lbl_801919A8[];

void (*fn_80009948(char* file, s32 line))();
void (*fn_80009970(char* file, s32 line))();
s32 NuFileReadInt(s32 fh);
void* fn_80005D7C(s32 fh);
void fn_80005D98(s32 fh, s32 size, s32 count);
s32 fn_80027FAC(char* name, char* path, s32* arg);
void fn_800140EC(s32 font);
void fn_8002DAD0(struct nuvec_s* v);
void fn_8002DCF8();
void fn_8002DD28();
void fn_80015E48(void* texanims);
void fn_80024D44(struct numtl_s* mtl);
void fn_80024C64(struct numtl_s* mtl);
void NuFileWrite(s32 tid);
void fn_80011070(struct nudisplayscene_s* display);

static inline void* NuGScnAlloc(s32 size, s32 align)
{
    u32 p = (*lbl_8040A34C + (align - 1)) & ~(align - 1);
    if (p + size < lbl_8040A350) {
        *lbl_8040A34C = p + size;
        return (void*)p;
    }
    fn_80009948(lbl_801918E8, 0x162)(lbl_80191930);
    return NULL;
}

void fn_80028138(char* path, s32* arg)
{
    s32 local = *arg;

    if (lbl_80407E08 == 0 && lbl_80407BE8 == 0) {
        lbl_80407BE8 = 1;
        lbl_80407E04 = fn_80027FAC(lbl_8019177C, path, &local);
        if (lbl_80407E04 == 0) {
            lbl_80407E04 = fn_80027FAC(lbl_8019178C, path, &local);
            if (lbl_80407E04 == 0) {
                lbl_80407E04 = fn_80027FAC(lbl_801917B4, path, &local);
            }
        }
        fn_800140EC(lbl_80407E04);
    }
}

void fn_800281EC(void)
{
    struct nuvec_s v = { 0.15f, 0.15f, 0.15f };

    fn_8002DAD0(&v);
}

void fn_80028234(void)
{
}

void fn_80028238(void)
{
}

void fn_8002823C(s32 mode, f32 x, f32 y)
{
    lbl_8040AD8C = x;
    lbl_8040AD90 = y;
    lbl_8040AD94 = mode;
    if (mode == 0 || x == 0.0f || y == 0.0f) {
        fn_8002DCF8(0);
    } else {
        fn_8002DCF8(1);
        // Unprototyped call: GPR and FPR slots are assigned independently,
        // so this reaches fn_8002DD28(mode, x, y). Only this order matches.
        fn_8002DD28(x, mode, y);
    }
}

void fn_800282D8(void)
{
}

void fn_800282DC(void)
{
}

void fn_800282E0(void)
{
}

void fn_800282E4(void)
{
}

extern s32 lbl_8040AD98;

void fn_800282E8(s32 fh)
{
    s32 count;
    s32 i;
    struct numtl_s* mtl;

    count = NuFileReadInt(fh);
    NuFileReadInt(fh);
    lbl_8040A364->mtls = NuGScnAlloc(count * 4, 4);
    lbl_8040A364->nummtl = count;
    for (i = 0; i < count; i++) {
        lbl_8040A364->mtls[i] = fn_80005D7C(fh);
        if (lbl_8040A364->mtls[i]->tid == -1) {
            lbl_8040A364->mtls[i]->tid = 0;
        } else {
            lbl_8040A364->mtls[i]->tid = lbl_8040A364->tids[lbl_8040A364->mtls[i]->tid];
        }
        fn_80005D98(fh, 0x11C, 1);
    }
    for (i = 0; i < count; i++) {
        mtl = lbl_8040A364->mtls[i];
        if ((s32)mtl->next == -1) {
            mtl->next = NULL;
        } else {
            mtl->next = lbl_8040A364->mtls[(s32)mtl->next];
        }
    }
}

struct nutexanim_s {
    struct nutexanim_s* succ;
    struct nutexanim_s* prev;
    s16* tids;
    s16 ntids;
    u8 dynalloc;
    u8 pad0F;
    struct numtl_s* mtl;
    void* env;
    char* name;
    char* progname;
};

extern char lbl_80191968[];
extern s32 lbl_8040A344;
void* NuTexAnimProgFind(char* name);
void* fn_80015C88(u32* buf, struct numtl_s* mtl, s16* tids, void* prog);
void fn_80015DD0(struct nutexanim_s* texanims);

/* NONMATCHING 155/156: one scheduling slot; see work/fn_80028444/status.md. */
void fn_80028444(s32 fh)
{
    s16* tids;
    struct nutexanim_s* ta;
    s32 k;
    void* prog;
    u32 start;
    u32 buf[1];

    fn_80009970(lbl_801918E8, 0x3C0)(lbl_80191968);
    lbl_8040A364->numtexanims = NuFileReadInt(fh);
    NuFileReadInt(fh);
    lbl_8040A364->texanims = fn_80005D7C(fh);
    fn_80005D98(fh, lbl_8040A364->numtexanims << 5, 1);
    NuFileReadInt(fh);
    tids = fn_80005D7C(fh);
    for (lbl_8040A344 = 0; lbl_8040A344 < lbl_8040A364->numtexanims; lbl_8040A344++) {
        ta = &lbl_8040A364->texanims[lbl_8040A344];
        ta->tids = &tids[(s32)ta->tids];
        ta->succ = NULL;
        ta->prev = NULL;
        for (k = 0; k < ta->ntids; k++) {
            ta->tids[k] = lbl_8040A364->tids[ta->tids[k]];
        }
        ta->name = lbl_8040A364->strings + (s32)ta->name;
        ta->progname = lbl_8040A364->strings + (s32)ta->progname;
        ta->mtl = lbl_8040A364->mtls[(s32)ta->mtl];
        ta->dynalloc = 0;
        prog = NuTexAnimProgFind(ta->progname);
        start = (*lbl_8040A34C + 0xF) & ~0xF;
        buf[0] = start;
        ta->env = fn_80015C88(buf, ta->mtl, ta->tids, prog);
        NuGScnAlloc(buf[0] - start, 16);
    }
    for (lbl_8040A344 = 0; lbl_8040A344 < lbl_8040A364->numtexanims - 1; lbl_8040A344++) {
        lbl_8040A364->texanims[lbl_8040A344].succ = &lbl_8040A364->texanims[lbl_8040A344] + 1;
        (&lbl_8040A364->texanims[lbl_8040A344])[1].prev = &lbl_8040A364->texanims[lbl_8040A344];
    }
    fn_80015DD0(lbl_8040A364->texanims);
}

struct nuinstanim_s {
    struct numtx_s mtx;
    f32 tfactor;
    f32 unknown44; // GC: wrap point while repeating (fn_80029050)
    f32 unknown48; // GC: added to the anim end time (fn_80029050)
    f32 ltime;
    u32 playing : 1;
    u32 backwards : 1;
    u32 repeating : 1;
    u32 oscillate : 1;
    u32 pad : 28;
    u8 unknown54[4];
    f32 oldltime;
    u8 anim_ix;
    u8 unknown5D;
    u16 ix;
};

void fn_800286B4(s32 fh)
{
    struct nudisplayscene_s* ds;
    struct nuinstanim_s* ia;
    s32 i;

    ds = lbl_8040A364->displayScene;
    lbl_8040A364->numinstanim = NuFileReadInt(fh);
    NuFileReadInt(fh);
    ia = fn_80005D7C(fh);
    lbl_8040A364->instanims = ia;
    if (ia != NULL) {
        for (i = 0; i < ds->numSpecials; i++) {
            if ((s32)ds->specials[i].animMtx != -1) {
                ds->specials[i].animMtx = (struct numtx_s*)&ia[(s32)ds->specials[i].animMtx];
                ((struct nuinstanim_s*)ds->specials[i].animMtx)->mtx = ds->specials[i].mtx;
                ((struct nuinstanim_s*)ds->specials[i].animMtx)->ix = i;
            } else {
                ds->specials[i].animMtx = NULL;
            }
        }
    }
    if (lbl_8040A364->numinstanim != 0) {
        lbl_8040A364->instanim_mtx = NuGScnAlloc(lbl_8040A364->numinstanim << 6, 16);
    }
}

extern s32 lbl_80407E90;

void fn_80028860(s32 fh)
{
    s32 count;
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    u32 n;
    s32 nzero;
    struct nuanimdata_s* ad;
    struct nuanimdatachunk_s* chunk;
    struct nuanimcurveset_s* cs;
    struct nuanimcurve_s* curve;
    struct nuanimkey_s* key;

    count = NuFileReadInt(fh);
    NuFileReadInt(fh);
    lbl_8040A364->instanim_data = fn_80005D7C(fh);
    for (i = 0; i < count; i++) {
        lbl_8040A364->instanim_data[i] = lbl_8040A364->instanim_data[i] != NULL
            ? (struct nuanimdata_s*)((s32)lbl_8040A364->instanim_data[i] + (s32)&lbl_8040A364->instanim_data[i]) : NULL;
        ad = lbl_8040A364->instanim_data[i];
        if (ad == NULL) {
            continue;
        }
        ad->chunks = ad->chunks != NULL ? (struct nuanimdatachunk_s**)((s32)ad->chunks + (s32)&ad->chunks) : NULL;
        for (j = 0; j < ad->nchunks; j++) {
            ad->chunks[j] = ad->chunks[j] != NULL ? (struct nuanimdatachunk_s*)((s32)ad->chunks[j] + (s32)&ad->chunks[j]) : NULL;
            chunk = ad->chunks[j];
            chunk->animcurvesets = chunk->animcurvesets != NULL
                ? (struct nuanimcurveset_s**)((s32)chunk->animcurvesets + (s32)&chunk->animcurvesets) : NULL;
            for (k = 0; k < chunk->numnodes; k++) {
                chunk->animcurvesets[k] = chunk->animcurvesets[k] != NULL
                    ? (struct nuanimcurveset_s*)((s32)chunk->animcurvesets[k] + (s32)&chunk->animcurvesets[k]) : NULL;
                cs = chunk->animcurvesets[k];
                cs->constants = cs->constants != NULL ? (f32*)((s32)cs->constants + (s32)&cs->constants) : NULL;
                cs->set = cs->set != NULL ? (struct nuanimcurve_s**)((s32)cs->set + (s32)&cs->set) : NULL;
                nzero = 0;
                for (m = 0; m < cs->ncurves; m++) {
                    if (cs->set[m] != NULL) {
                        cs->set[m] = (struct nuanimcurve_s*)((s32)cs->set[m] + (s32)&cs->set[m]);
                        curve = cs->set[m];
                        curve->animkeys = curve->animkeys != NULL
                            ? (struct nuanimkey_s*)((s32)curve->animkeys + (s32)&curve->animkeys) : NULL;
                        if (lbl_80407E90 != 0 && cs->ncurves == 9 && (u32)(m - 6) <= 2) {
                            for (n = 2; n < curve->numkeys; n++) {
                                key = &curve->animkeys[n];
                                if ((key[-2].d == 1.0f && key[-1].d == 0.0f && key[0].d == 0.0f)
                                    || (key[-2].d == 0.0f && key[-1].d == 1.0f && key[-1].d == 0.0f)) {
                                    nzero++;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (nzero == 3) {
                    cs->set[6]->flags |= 3;
                    cs->set[7]->flags |= 3;
                    cs->set[8]->flags |= 3;
                }
            }
        }
    }
}

extern char lbl_80191990[];
s32 NuFileReadShort(s32 fh);

void fn_80028B88(s32 fh)
{
    struct nugscn_s* scene;
    struct nuportalinfo_s* p;
    s32 i;
    struct nuroom_s* room;

    scene = lbl_8040A364;
    p = &scene->portals;
    scene->portalinfo = p;
    fn_80009970(lbl_801918E8, 0x49C)(lbl_80191990);
    NuFileReadInt(fh);
    p->numportals = NuFileReadShort(fh);
    p->numrooms = NuFileReadShort(fh);
    p->numindices = NuFileReadShort(fh);
    p->numplanes = NuFileReadShort(fh);
    p->numverts = NuFileReadShort(fh);
    p->numportalix = NuFileReadShort(fh);
    p->depth = 0x100;
    p->indices = fn_80005D7C(fh);
    fn_80005D98(fh, p->numindices * 2, 1);
    p->planes = fn_80005D7C(fh);
    fn_80005D98(fh, p->numplanes * 16, 1);
    p->portalix = fn_80005D7C(fh);
    fn_80005D98(fh, p->numportalix * 2, 1);
    p->verts = fn_80005D7C(fh);
    fn_80005D98(fh, p->numverts * 12, 1);
    p->rooms = fn_80005D7C(fh);
    fn_80005D98(fh, p->numrooms * 0x18, 1);
    p->portals = fn_80005D7C(fh);
    fn_80005D98(fh, p->numportals * 0x20, 1);
    for (i = 0; i < p->numportals; i++) {
        p->portals[i].vertices = &p->verts[(s32)p->portals[i].vertices];
    }
    for (i = 0; i < p->numrooms; i++) {
        room = &p->rooms[i];
        room->unknown00 = &p->indices[(s32)room->unknown00];
        room->planes = &p->planes[(s32)room->planes];
        room->portalIndices = &p->portalix[(s32)room->portalIndices];
    }
}

struct nuvtxset_s {
    s16 count;
    s16 stride;
    char* name;
    void* data;
};

struct nuvtxstride_s {
    char* name;
    s32 stride;
};

extern struct nuvtxstride_s* lbl_80407E94;
extern u32 lbl_8040A354;
extern s32 lbl_8040A358;
extern u32 lbl_8040A35C;
char* strstr(const char* s, const char* sub);

static inline s32 NuVtxSetStride(char* name)
{
    struct nuvtxstride_s* t = lbl_80407E94;

    if (t != NULL && t->name != NULL) {
        do {
            if (strstr(name, t->name) != NULL) {
                return t->stride;
            }
            t++;
        } while (t->name != NULL);
    }
    return 0xC;
}

/* NONMATCHING 166/169: register/schedule residue; see work/fn_80028DAC/status.md. */
void fn_80028DAC(s32 fh)
{
    s32 size;
    s32* p;
    s32 i;
    s32 k;
    char* name;
    u8* dst;

    lbl_8040A364->numvtxsets = NuFileReadInt(fh);
    size = NuFileReadInt(fh);
    lbl_8040A364->vtxsets = NuGScnAlloc(lbl_8040A364->numvtxsets * 0xC, 4);
    p = fn_80005D7C(fh);
    fn_80005D98(fh, size, 1);
    for (i = 0; i < lbl_8040A364->numvtxsets; i++) {
        lbl_8040A364->vtxsets[i].count = *(u16*)p;
        p++;
        lbl_8040A364->vtxsets[i].name = *p + lbl_8040A364->strings;
        p++;
        name = lbl_8040A364->vtxsets[i].name;
        lbl_8040A364->vtxsets[i].stride = NuVtxSetStride(name);
        if (lbl_8040A364->vtxsets[i].stride != 0xC) {
            lbl_8040A364->vtxsets[i].data = NuGScnAlloc(lbl_8040A364->vtxsets[i].stride * lbl_8040A364->vtxsets[i].count, 4);
            dst = lbl_8040A364->vtxsets[i].data;
            for (k = 0; k < lbl_8040A364->vtxsets[i].count; k++) {
                *(struct nuvec_s*)dst = *(struct nuvec_s*)p;
                p += 3;
                dst += lbl_8040A364->vtxsets[i].stride;
            }
        } else {
            lbl_8040A364->vtxsets[i].data = p;
            for (k = 0; k < lbl_8040A364->vtxsets[i].count; k++) {
                p += 3;
            }
        }
    }
    if (lbl_8040A358 != 0) {
        lbl_8040A350 = lbl_8040A35C;
    } else {
        lbl_8040A350 = lbl_8040A354;
    }
}

void NuAnimCurveSetApplyToMatrix(struct nuanimcurveset_s* cs, struct nuanimtime_s* time, struct numtx_s* mtx);
void fn_80016944(struct nuanimdata_s* data, f32 time, struct nuanimtime_s* out);
f32 fn_8000B264(f32 x);
void fn_8002A5D8(struct nugscn_s* scene);

void fn_80029050(struct nugscn_s* scene, f32 dt)
{
    struct nuanimtime_s atime;
    s32 i;
    struct nuinstanim_s* ia;
    struct numtx_s* m;
    struct nudisplayspecial_s* sp;
    struct nuanimdata_s* ad;
    f32 t;

    t = 0.0f;
    m = scene->instanim_mtx;
    ia = scene->instanims;
    if (scene->instanim_data != NULL) {
        for (i = 0; i < scene->numinstanim; i++, ia++, m++) {
            sp = &scene->displayScene->specials[ia->ix];
            if (!(sp->flags & 2)) {
                continue;
            }
            ad = scene->instanim_data[ia->anim_ix];
            if (ad == NULL) {
                continue;
            }
            if (ia->playing) {
                ia->ltime += dt * ia->tfactor;
                if (ia->repeating) {
                    if (ia->ltime >= ia->unknown44) {
                        ia->repeating = 0;
                        ia->ltime = ia->ltime - (ia->unknown44 - 1.0f);
                    }
                }
                if (!ia->repeating) {
                    if (ia->ltime >= ad->time + ia->unknown48) {
                        if (ia->oscillate) {
                            ia->ltime -= fn_8000B264((ia->ltime - 1.0f) / (ad->time + ia->unknown48 - 1.0f)) * (ad->time + ia->unknown48 - 1.0f);
                            t = ia->ltime;
                        } else {
                            ia->playing = 0;
                            ia->ltime = ad->time;
                            t = ia->ltime;
                        }
                    } else if (ia->ltime > ad->time) {
                        t = ad->time;
                    } else if (ia->ltime < 1.0f) {
                        ia->ltime = 1.0f;
                        t = 1.0f;
                        ia->playing = 0;
                    } else {
                        t = ia->ltime;
                    }
                    if (ia->backwards) {
                        t = ad->time + 1.0f - t;
                    }
                }
            } else {
                t = ia->ltime;
            }
            if (t != ia->oldltime) {
                fn_80016944(ad, t, &atime);
                NuAnimCurveSetApplyToMatrix(ad->chunks[atime.chunk]->animcurvesets[0], &atime, m);
                ia->oldltime = t;
            }
            ia->mtx = *m;
            ia->mtx._30 += sp->drawMtx._30;
            ia->mtx._31 += sp->drawMtx._31;
            ia->mtx._32 += sp->drawMtx._32;
        }
    }
    fn_8002A5D8(scene);
}

extern struct nucamera_s lbl_80313268;
extern f32 lbl_801919D8[];
extern f32 lbl_801919DC[];
void NuCameraCalcFrustrumPlanes(struct nucamera_s* cam);
s32 fn_80027688(struct nugobj_s* gobj, struct numtx_s* mtx);
void fn_8002A5A0(struct nugscn_s* scene);

void fn_8002933C(struct nugscn_s* scene)
{
    struct numtx_s mtx;
    s32 n;
    s32 lod;
    f32 scale;
    struct nuinstance_s* inst;
    struct numtx_s* animMtx;

    n = scene->numInstance;
    lod = 0;
    scale = lbl_80313268.scale.x;
    inst = scene->instances + n;
    while (n != 0) {
        inst--;
        n--;
        if ((inst->unknownFlag2 || inst->noVisiTest) && inst->visible) {
            animMtx = inst->animMtx;
            if (lod != inst->unknown42) {
                if (inst->unknown42 == 0) {
                    lbl_80313268.scale.x = scale;
                } else {
                    lbl_80313268.scale.x = inst->unknown42 * 0.0625f;
                }
                NuCameraCalcFrustrumPlanes(&lbl_80313268);
                lod = inst->unknown42;
            }
            if (animMtx != NULL) {
                inst->onScreen = fn_80027688(scene->gobjs[inst->objid], animMtx);
            } else if (inst->unknown4C == 0.0f) {
                inst->onScreen = fn_80027688(scene->gobjs[inst->objid], &inst->mtx);
            } else {
                inst->onScreen = fn_80027688(scene->gobjs[inst->objid], &mtx);
            }
        }
    }
    if (lod != 0) {
        lbl_80313268.scale.x = scale;
        NuCameraCalcFrustrumPlanes(&lbl_80313268);
    }
    fn_8002A5A0(scene);
}
struct nughgtex_s {
    u8 pad0[0xA];
    u16 nlevels;
    s32 levels[11];
    s32 pal;
};

void* memset(void* p, s32 c, u32 n);
s32 fn_80024F44(struct nughgtex_s* tex);

/* NONMATCHING 80/93: register allocation in the texture loop; see work/NuMtlRender/status.md. */
void NuMtlRender(s32 fh)
{
  s32 count;
  s32 i;
  s32 k;
  struct nughgtex_s *tex;
  count = NuFileReadInt(fh);
  lbl_8040A364->numtex = count;
  lbl_8040A364->texs = fn_80005D7C(fh);
  lbl_8040A364->tids = NuGScnAlloc(count * 4, 4);
  memset(lbl_8040A364->tids, 0, count * 4);
  lbl_8040A364->numtid = count;
  for (i = 0; i < lbl_8040A364->numtex; i++)
  {
    tex = &lbl_8040A364->texs[i];
    for (k = 0; k < tex->nlevels; k++)
    {
      tex->levels[k] = (tex->levels[k] != 0) ? (tex->levels[k] + ((s32) (&tex->levels[k]))) : (0);
    }

    do
    {
      if (tex->pal != 0)
      {
        k = tex->pal + ((s32) (&tex->pal));
      }
      else
      {
        k = 0;
      }
    }
    while (0);
    tex->pal = k;
    lbl_8040A364->tids[i] = fn_80024F44(tex);
  }

}

void fn_80024CD4(void* mtl);
void fn_80010EF8(struct nudisplayscene_s* ds);

struct nudispgeom_s {
    u8 fixed;
    u8 unknown01[0xB];
    s32 num0C;
    void** unknown10;
    void* unknown14;
    void* unknown18;
    void* unknown1C;
    u8 unknown20[4];
    void* unknown24;
    u8 unknown28[4];
    u16 num2C;
    u8 unknown2E[2];
    struct nudispentry0C_s* unknown30;
    void* unknown34;
    u8 unknown38[4];
    void* unknown3C;
};

struct nudispobj8F_s {
    s32 fixed;
    u8 unknown04[8];
    void* unknown0C;
};

#define DS_FIX(T, x) ((x) = (x) != NULL ? (T)((s32)(x) + (s32)&(x)) : NULL)

void fn_8002963C(struct nudisplayscene_s* ds, struct numtl_s** mtls, struct nugscnbounds_s* bounds, struct nugscn_s* scene)
{
    struct nudispcmd_s* cmd;
    s32 i;

    DS_FIX(void*, ds->unknown00);
    DS_FIX(struct nudispcmd_s*, ds->displist);
    DS_FIX(void*, ds->unknown18);
    DS_FIX(void*, ds->unknown1C);
    DS_FIX(void*, ds->unknown20);
    DS_FIX(void*, ds->unknown24);
    DS_FIX(void*, ds->unknown58);
    DS_FIX(void*, ds->unknown5C);
    DS_FIX(void*, ds->unknown30);
    DS_FIX(void*, ds->unknown34);
    DS_FIX(void*, ds->unknown28);
    DS_FIX(void*, ds->unknown2C);
    DS_FIX(void*, ds->unknown80);
    DS_FIX(struct nudispsub7C_s*, ds->unknown7C);
    DS_FIX(void*, ds->unknown7C->unknown04);
    DS_FIX(void*, ds->unknown7C->unknown10);
    DS_FIX(void*, ds->unknown48);
    if (lbl_8040AD98 > 1) {
        DS_FIX(void*, ds->unknown88);
    }
    DS_FIX(struct nudispentry0C_s*, ds->entries0C);
    for (i = 0; i < ds->num0C; i++) {
        DS_FIX(void*, ds->entries0C[i].unknown08);
        DS_FIX(void*, ds->entries0C[i].unknown04);
    }
    DS_FIX(struct nudisplayspecial_s*, ds->specials);
    for (i = 0; i < ds->numSpecials; i++) {
        DS_FIX(char*, ds->specials[i].name);
        DS_FIX(void*, ds->specials[i].unknownB0);
        DS_FIX(void*, ds->specials[i].unknownBC);
    }
    DS_FIX(struct nudispentry24_s*, ds->entries24);
    for (i = 0; i < ds->num24; i++) {
        DS_FIX(void*, ds->entries24[i].unknown04);
        DS_FIX(void*, ds->entries24[i].unknown10);
        DS_FIX(void*, ds->entries24[i].unknown1C);
    }
    ds->mtls = mtls;
    DS_FIX(struct nudispmtl_s**, ds->mtlentries);
    for (i = 0; i < ds->nummtl; i++) {
        struct nudispmtl_s* e;

        DS_FIX(struct nudispmtl_s*, ds->mtlentries[i]);
        e = ds->mtlentries[i];
        DS_FIX(void*, e->unknown00);
        DS_FIX(void*, e->unknown08);
        DS_FIX(void*, e->unknown0C);
        DS_FIX(void*, e->unknown18);
        e->unknown14 = DS_FIX(void*, e->unknown10);
    }
    for (i = 0; i < ds->nummtl; i++) {
        ds->mtls[i]->unknownCC = ds->mtlentries[i];
    }
    if (bounds != NULL) {
        ds->bounds = *bounds;
    }
    cmd = ds->displist;
    while (cmd->type != 0x8E) {
        void* d;

        d = cmd->data;
        if (d != NULL) {
            d = (void*)((s32)d + (s32)cmd);
        }
        cmd->data = d;
        switch (cmd->type) {
        case 0x80:
            fn_80024CD4(d);
            break;
        case 0x82: {
            struct nudispgeom_s* g = d;

            if (g->fixed == 0) {
                DS_FIX(void**, g->unknown10);
                for (i = 0; i < g->num0C; i++) {
                    DS_FIX(void*, g->unknown10[i]);
                }
                DS_FIX(void*, g->unknown14);
                DS_FIX(void*, g->unknown18);
                DS_FIX(void*, g->unknown1C);
                DS_FIX(void*, g->unknown24);
                DS_FIX(struct nudispentry0C_s*, g->unknown30);
                DS_FIX(void*, g->unknown34);
                DS_FIX(void*, g->unknown3C);
                if (g->unknown30 != NULL) {
                    for (i = 0; i < g->num2C; i++) {
                        if (*(u8*)&g->unknown30[i] == 0) {
                            DS_FIX(void*, g->unknown30[i].unknown08);
                            *(u8*)&g->unknown30[i] = 1;
                        }
                    }
                }
                g->fixed = 1;
            }
            break;
        }
        case 0x83:
            break;
        case 0x8F: {
            struct nudispobj8F_s* o = d;

            if (o->fixed == 0) {
                DS_FIX(void*, o->unknown0C);
                o->fixed = 1;
            }
            break;
        }
        }
        cmd++;
    }
    fn_80010EF8(ds);
}

struct nuscnload_s {
    s32 size;
    u32 start;
    u32* buf;
    u32 end;
    char* path;
};

extern struct nuscnload_s lbl_8021F9E4;
extern s32 lbl_80407EA4;
extern s32 lbl_80407E78;
extern char lbl_801919E4[];
extern char lbl_80191A00[];
extern char lbl_80191A18[];
extern char lbl_80191A40[];
void (*fn_8000995C(char* file, s32 line))();
void* memset(void* p, s32 c, u32 n);
s32 fn_80005CC4(char* buf, s32 size, u32 mode);
void fn_80005D34(s32 fh);
s32 NuFileBeginBlkRead(s32 fh, s32 tag);
void NuFileEndBlockRead(s32 fh);
void fn_800282E8(s32 fh);
void fn_8002963C(struct nudisplayscene_s* ds, struct numtl_s** mtls, struct nugscnbounds_s* bounds, struct nugscn_s* scene);

struct nugscn_s* fn_80029EC8(void)
{
    s32 size;
    char* path;
    char* buf;
    u32* bufp;
    s32 fh;
    s32 id;
    s32 i;
    s32 n;
    struct nugscn_s* scene;
    struct nugscnbounds_s* bounds;

    fh = lbl_8021F9E4.size;
    buf = (char*)lbl_8021F9E4.start;
    bufp = lbl_8021F9E4.buf;
    path = lbl_8021F9E4.path;
    lbl_8040A364 = NULL;
    lbl_80407EA4 = 0;
    if (fh > 0) {
        lbl_8040A364 = NuGScnAlloc(sizeof(struct nugscn_s), 4);
        if (lbl_8040A364 != NULL) {
            memset(lbl_8040A364, 0, sizeof(struct nugscn_s));
            fh = fn_80005CC4(buf, fh, 0);
            if (fh != 0) {
                NuFileBeginBlkRead(fh, 0x3032554E);
                lbl_8040AD98 = NuFileReadInt(fh);
                NuFileReadInt(fh);
                if (lbl_8040AD98 < lbl_80407E78) {
                    fn_8000995C(lbl_801918E8, 0x856)(lbl_80191A18, path);
                }
                if (lbl_8040AD98 > lbl_80407E78) {
                    fn_8000995C(lbl_801918E8, 0x85A)(lbl_80191A40);
                }
                while ((id = NuFileBeginBlkRead(fh, 0)) != 0) {
                    switch (id) {
                    case 0x44494E49:
                        break;
                    case 0x30545354:
                        NuMtlRender(fh);
                        break;
                    case 0x3030534D:
                        fn_800282E8(fh);
                        break;
                    case 0x50534944:
                        fn_80009970(lbl_801918E8, 0x7EC)(lbl_801919E4);
                        fn_8002963C(lbl_8040A364->displayScene = fn_80005D7C(fh), lbl_8040A364->mtls,
                                    &lbl_8040A364->bounds, lbl_8040A364);
                        break;
                    case 0x53444E42:
                        scene = lbl_8040A364;
                        bounds = &scene->bounds;
                        fn_80009970(lbl_801918E8, 0x800)(lbl_80191A00);
                        NuFileReadInt(fh);
                        n = NuFileReadInt(fh);
                        scene->bounds.numa = n;
                        bounds->numb = n;
                        NuFileReadInt(fh);
                        NuFileReadInt(fh);
                        bounds->a = fn_80005D7C(fh);
                        fn_80005D98(fh, scene->bounds.numa << 4, 1);
                        bounds->b = fn_80005D7C(fh);
                        fn_80005D98(fh, bounds->numb << 5, 1);
                        break;
                    case 0x484E4942:
                        i = 0;
                        lbl_8040A364->numnamed = NuFileReadInt(fh);
                        lbl_8040A364->names = fn_80005D7C(fh);
                        for (; i < lbl_8040A364->numnamed; i++) {
                            lbl_8040A364->names[i] = lbl_8040A364->strings + NuFileReadInt(fh);
                        }
                        lbl_8040A364->named = fn_80005D7C(fh);
                        fn_80005D98(fh, lbl_8040A364->numnamed << 2, 1);
                        lbl_8040A364->numnamed = 0;
                        break;
                    case 0x304E4942:
                        lbl_8040A364->named[lbl_8040A364->numnamed++] = fn_80005D7C(fh);
                        break;
                    case 0x30545353:
                        fn_80028DAC(fh);
                        break;
                    case 0x4C42544E:
                        size = NuFileReadInt(fh);
                        lbl_8040A364->strings = fn_80005D7C(fh);
                        fn_80005D98(fh, size, 1);
                        break;
                    case 0x4C424149:
                        fn_800286B4(fh);
                        break;
                    case 0x42494C41:
                        fn_80028860(fh);
                        break;
                    case 0x54524F50:
                        fn_80028B88(fh);
                        break;
                    case 0x30534154:
                        fn_80028444(fh);
                        break;
                    }
                    NuFileEndBlockRead(fh);
                }
                NuFileEndBlockRead(fh);
                fn_80005D34(fh);
            }
            *bufp = (*bufp + 0xF) & ~0xF;
        }
    }
    lbl_8040A364->displayScene->scene = lbl_8040A364;
    return lbl_8040A364;
}

void fn_8002A3A8(struct nugscn_s* scene)
{
    s32 i;

    if (scene->texanims != NULL) {
        fn_80015E48(scene->texanims);
    }
    for (i = 0; i < scene->nummtl; i++) {
        while (scene->mtls[i]->next != NULL) {
            fn_80024D44(scene->mtls[i]);
        }
        fn_80024C64(scene->mtls[i]);
    }
    fn_80009970(lbl_801918E8, 0x581)(lbl_801919A8, scene->nummtl);
    for (i = 0; i < scene->numtid; i++) {
        NuFileWrite(scene->tids[i]);
    }
    fn_80011070(scene->displayScene);
}
