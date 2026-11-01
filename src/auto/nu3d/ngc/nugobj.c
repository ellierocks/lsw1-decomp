#include "types.h"
#include "nu3dx/nu3dx.h"

// 0x40-byte glass state blocks; only the size is confirmed.
struct nuglassview_s {
    u32 words[16];
};

extern struct nuglassview_s lbl_8020E934;
extern struct nuglassview_s lbl_8020E974;
extern char lbl_80191200[];
extern s32 lbl_80407D08;
extern struct nugobj_s* lbl_80407D0C;

extern void NuMemFree(void* p, char* file, s32 line);
extern void fn_80039CA4(struct nugobj_s* gobj);
void NuGobjClose(void);
void fn_80024704(struct nugeom_s* geom);
void fn_80024854(struct nuprim_s* prim);

void NuGlassClose(struct nuglassview_s* p)
{
    if (p != NULL) {
        *p = lbl_8020E974;
    }
}

struct nuglassview_s* NuGlassRenderStatic(void)
{
    return &lbl_8020E934;
}

void NuGobjInit(struct nugobj_s* gobj)
{
    struct nugeom_s* geom = gobj->geom;
    struct nufaceongeom_s* fgeom = gobj->faceonGeom;
    struct nugeom_s* next;
    struct nufaceongeom_s* fnext;

    fn_80039CA4(gobj);
    while (geom != NULL) {
        next = geom->next;
        if (geom->blendgeom != NULL) {
            if (geom->blendgeom->blend_offsets != NULL) {
                NuMemFree(geom->blendgeom->blend_offsets, lbl_80191200, 0x84);
            }
            if (geom->blendgeom->offsets != NULL) {
                NuMemFree(geom->blendgeom->offsets, lbl_80191200, 0x87);
            }
            if (geom->blendgeom->ix != NULL) {
                NuMemFree(geom->blendgeom->ix, lbl_80191200, 0x8A);
            }
            NuMemFree(geom->blendgeom, lbl_80191200, 0x8C);
        }
        fn_80024704(geom);
        geom = next;
    }
    if (fgeom != NULL) {
        if (fgeom->unknown1C != NULL) {
            NuMemFree(fgeom->unknown1C, lbl_80191200, 0x95);
        }
        while (fgeom != NULL) {
            fnext = fgeom->next;
            fn_800247FC(fgeom);
            fgeom = fnext;
        }
    }
    if (gobj->sysnext != NULL) {
        gobj->sysnext->syslast = gobj->syslast;
    }
    if (gobj->syslast != NULL) {
        gobj->syslast->sysnext = gobj->sysnext;
    } else {
        lbl_80407D0C = gobj->sysnext;
    }
    NuMemFree(gobj, lbl_80191200, 0xA5);
}

void fn_80024670(void)
{
    if (lbl_80407D08 != 0) {
        NuGobjClose();
    }
    lbl_80407D0C = NULL;
    lbl_80407D08 = 1;
}

void NuGobjClose(void)
{
    struct nugobj_s* next;

    if (lbl_80407D08 != 0) {
        while (lbl_80407D0C != NULL) {
            next = lbl_80407D0C->sysnext;
            NuGobjInit(lbl_80407D0C);
            lbl_80407D0C = next;
        }
    }
}

void fn_80024704(struct nugeom_s* geom)
{
    struct nuprim_s* prim = geom->prim;
    struct nuprim_s* pnext;
    struct nuskin_s* snext;

    if (geom->unknown28 != NULL) {
        NuMemFree(geom->unknown28, lbl_80191200, 0x18C);
    }
    while (prim != NULL) {
        pnext = prim->next;
        fn_80024854(prim);
        prim = pnext;
    }
    while (geom->skin != NULL) {
        snext = geom->skin->next;
        if (geom->skin->mtxid != NULL) {
            NuMemFree(geom->skin->mtxid, lbl_80191200, 0x198);
        }
        if (geom->skin->weights != NULL) {
            NuMemFree(geom->skin->weights, lbl_80191200, 0x19A);
        }
        NuMemFree(geom->skin, lbl_80191200, 0x19B);
        geom->skin = snext;
    }
    if (geom->vtxskininfo != NULL) {
        NuMemFree(geom->vtxskininfo, lbl_80191200, 0x1A0);
    }
    NuMemFree(geom, lbl_80191200, 0x1A3);
}

void fn_800247FC(struct nufaceongeom_s* fgeom)
{
    if (fgeom->unknown10 != NULL) {
        NuMemFree(fgeom->unknown10, lbl_80191200, 0x1B7);
    }
    NuMemFree(fgeom, lbl_80191200, 0x1BA);
}

void fn_80024854(struct nuprim_s* prim)
{
    if (prim != NULL) {
        if (prim->unknown10 != NULL) {
            NuMemFree(prim->unknown10, lbl_80191200, 0x216);
        }
        if (prim->unknown14 != NULL) {
            NuMemFree(prim->unknown14, lbl_80191200, 0x217);
        }
        NuMemFree(prim, lbl_80191200, 0x218);
    }
}
