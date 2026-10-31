#include "nu3dx/nu3dx.h"

extern s32 lbl_80407BA8;
extern s32 lbl_80407BAC;
extern f32 lbl_80407BB0;
extern u32 lbl_80407BB8;
extern char lbl_8018F914[];
extern char str_NuSpecialFind_NotFound[];
extern char str_NuSpecialFindMulti_SceneNull[];
extern f32 lbl_8018F9EC[];

void (*fn_80009970(char* file, s32 line))();
s32 lbl_80155DE8(char* a, char* b);
char* NuStrIStr(char* haystack, char* needle);
s32 fn_800119C8(struct nuhspecial_s* special, struct numtx_s* mtx, s32 mode, s32 a, s32 b);
s32 fn_80027688(struct nugobj_s* gobj, struct numtx_s* mtx);
f32 fn_8000CB50(struct nuvec_s* v);
s32 fn_80023628(struct nuvec_s* min, struct nuvec_s* max, struct numtx_s* mtx, f32 margin, s32 flags);

struct nuvtxstateattr_s {
    s8 unknown00;
    u8 unknown01;
    u8 unknown02[2];
    s32 unknown04;
};

struct nuvtxstate_s {
    u8 unknown00[8];
    struct nuvtxstateattr_s attr;
};

struct nuvtxstates_s {
    s32 count;
    u8* bytes;
    void* blocks;
    struct nuvtxstate_s* states;
};

struct nuspecialglobals_s {
    u8 unknown000[0x530];
    struct nuvtxstates_s vtxStates;
};

extern struct nuspecialglobals_s lbl_801B0F88;

static inline void NuVtxStateAttrInit(struct nuvtxstateattr_s* attr)
{
    attr->unknown01 = 4;
    attr->unknown04 = 0;
    attr->unknown00 = -0x7C;
}

void fn_80012C44(u8** buffer, s32 unused, s32 count)
{
    s32 i;

    lbl_801B0F88.vtxStates.count = count;
    lbl_801B0F88.vtxStates.bytes = *buffer;
    *buffer += count;
    lbl_801B0F88.vtxStates.blocks = (void*)(((u32)*buffer + 3) & ~3);
    *buffer = (u8*)((((u32)*buffer + 3) & ~3) + count * 0x24);
    lbl_801B0F88.vtxStates.states = (struct nuvtxstate_s*)(((u32)*buffer + 0xF) & ~0xF);
    *buffer = (u8*)((((u32)*buffer + 0xF) & ~0xF) + count * 0x10);
    for (i = 0; i < count; i++) {
        lbl_801B0F88.vtxStates.bytes[i] = 0;
        NuVtxStateAttrInit(&lbl_801B0F88.vtxStates.states[i].attr);
    }
}

s32 NuSpecialFind(struct nugscn_s* scene, struct nuhspecial_s* dest, char* name, s32 warn)
{
    struct nudisplayspecial_s* ds;
    struct nuspecial_s* sp;
    s32 i;

    if (scene == NULL) {
        dest->scene = NULL;
        dest->special = NULL;
        dest->display = NULL;
        return 0;
    found_display:
        dest->scene = scene;
        dest->display = ds;
        dest->special = NULL;
        return 1;
    }
    if (scene->displayScene != NULL) {
        ds = scene->displayScene->specials;
        for (i = 0; i < scene->displayScene->numSpecials; i++, ds++) {
            if (lbl_80155DE8(name, ds->name) == 0) {
                goto found_display;
            }
        }
        if (lbl_80407BA8 != 0) {
            (*fn_80009970(lbl_8018F914, 0x154))(str_NuSpecialFind_NotFound, name);
        }
        dest->display = NULL;
        dest->scene = NULL;
        dest->special = NULL;
        return 0;
    found_special:
        dest->scene = scene;
        dest->special = sp;
        dest->display = NULL;
        return 1;
    }
    sp = scene->specials;
    for (i = 0; i < scene->numSpecial; i++, sp++) {
        if (lbl_80155DE8(name, sp->name) == 0) {
            goto found_special;
        }
    }
    if (warn != 0) {
        (*fn_80009970(lbl_8018F914, 0x18A))(str_NuSpecialFind_NotFound, name);
    }
    dest->scene = NULL;
    dest->special = NULL;
    dest->display = NULL;
    return 0;
}

s32 NuSpecialFindMulti(struct nugscn_s* scene, struct nuhspecial_s* dest, char* name, s32 max, s32 warn)
{
    struct nudisplayspecial_s* legacyDs = NULL;
    struct nudisplayspecial_s* ds;
    struct nuspecial_s* sp;
    s32 c1;
    s32 i1;
    s32 c2;
    s32 i2;

    if (scene != NULL) {
        if (scene->displayScene == NULL) {
            goto legacy;
        }
    {
        c1 = 0;
        ds = scene->displayScene->specials;
        for (i1 = 0; i1 < scene->displayScene->numSpecials; i1++, ds++) {
            if (NuStrIStr(ds->name, name) != NULL) {
                dest->scene = scene;
                c1++;
                dest->display = ds;
                dest->special = NULL;
                dest++;
                if (c1 >= max) {
                    goto done;
                }
            }
        }
        if (warn != 0 && lbl_80407BA8 != 0) {
            (*fn_80009970(lbl_8018F914, 0x1B0))(str_NuSpecialFind_NotFound, name);
        }
    done:
        return c1;
    }
    }
    (*fn_80009970(lbl_8018F914, 0x1CF))(str_NuSpecialFindMulti_SceneNull);
    return 0;
legacy:
    c2 = 0;
    sp = scene->specials;
    for (i2 = 0; i2 < scene->numSpecial; i2++, sp++, legacyDs++) {
        if (NuStrIStr(sp->name, name) != NULL) {
            dest->scene = scene;
            dest->special = sp;
            if (scene->displayScene != NULL) {
                dest->display = legacyDs;
            } else {
                dest->display = NULL;
            }
            c2++;
            dest++;
            if (c2 >= max) {
                return c2;
            }
        }
    }
    if (warn != 0 && lbl_80407BA8 != 0) {
        (*fn_80009970(lbl_8018F914, 0x1EC))(str_NuSpecialFind_NotFound, name);
    }
    return c2;
}

s32 fn_800130F4(struct nuhspecial_s* special)
{
    if (special != NULL && (special->special != NULL || special->display != NULL)) {
        return 1;
    }
    return 0;
}

s32 fn_800131B8(struct nuhspecial_s* special)
{
    if (special->scene != NULL) {
        if (special->special != NULL) {
            return special->special->instance->visible;
        }
        if (special->display != NULL) {
            return special->display->flags & NUDISPLAYSPECIAL_VISIBLE;
        }
    }
    return 0;
}

void fn_80013200(struct nuhspecial_s* special, s32 visible)
{
    if (special->scene == NULL) {
        return;
    }
    if (special->special != NULL) {
        struct nuinstance_s* inst = special->special->instance;
        if (inst != NULL) {
            inst->visible = visible;
        }
    } else if (special->display != NULL) {
        if (visible != 0) {
            special->display->flags |= NUDISPLAYSPECIAL_VISIBLE;
        } else {
            special->display->flags &= ~NUDISPLAYSPECIAL_VISIBLE;
        }
    }
}

void fn_80013268(struct nuhspecial_s* special, s32 enable)
{
    if (special->scene == NULL) {
        return;
    }
    if (special->special != NULL) {
        special->special->instance->noVisiTest = enable;
    } else if (special->display != NULL) {
        if (enable != 0) {
            special->display->flags |= NUDISPLAYSPECIAL_NOVISITEST;
        } else {
            special->display->flags &= ~NUDISPLAYSPECIAL_NOVISITEST;
        }
    }
}

s32 fn_800132C8(struct nuhspecial_s* special)
{
    if (special->scene != NULL) {
        if (special->special != NULL) {
            return special->special->instance->noVisiTest;
        }
        if (special->display != NULL) {
            return special->display->flags & NUDISPLAYSPECIAL_NOVISITEST;
        }
    }
    return 0;
}

s32 fn_80013310(struct nuhspecial_s* special)
{
    if (special->scene != NULL) {
        if (special->special != NULL) {
            return special->special->instance->onScreen;
        }
        if (special->display != NULL) {
            return 1;
        }
    }
    return 1;
}

struct nuvec_s* fn_80013350(struct nuhspecial_s* special)
{
    if (special->display != NULL) {
        return (struct nuvec_s*)&special->display->mtx._30;
    }
    if (special->special != NULL) {
        return (struct nuvec_s*)&special->special->mtx._30;
    }
    return NULL;
}

struct nuvec_s* fn_80013380(struct nuhspecial_s* special)
{
    if (special->display != NULL) {
        struct numtx_s* m = special->display->animMtx;
        if (m != (struct numtx_s*)-1 && m != NULL) {
            return (struct nuvec_s*)&m->_30;
        }
        return (struct nuvec_s*)&special->display->mtx._30;
    }
    if (special->special != NULL) {
        struct nuinstance_s* inst = special->special->instance;
        if (inst->animMtx != NULL) {
            return (struct nuvec_s*)&inst->animMtx->_30;
        }
        return (struct nuvec_s*)&inst->mtx._30;
    }
    return NULL;
}

struct numtx_s* fn_800133E4(struct nuhspecial_s* special)
{
    if (special->display != NULL) {
        return &special->display->mtx;
    }
    return &special->special->mtx;
}

struct numtx_s* fn_800133FC(struct nuhspecial_s* special)
{
    if (special->special != NULL) {
        struct nuinstance_s* inst = special->special->instance;
        if (inst->animMtx != NULL) {
            return inst->animMtx;
        }
        return &inst->mtx;
    }
    if (special->display != NULL) {
        struct numtx_s* m = special->display->animMtx;
        if (m != (struct numtx_s*)-1 && m != NULL) {
            return m;
        }
        return &special->display->drawMtx;
    }
    return NULL;
}

void fn_80013454(struct nuhspecial_s* special, struct numtx_s* mtx)
{
    if (special == NULL || special->scene == NULL) {
        return;
    }
    if (special->special != NULL) {
        special->special->instance->mtx = *mtx;
    } else {
        special->display->drawMtx = *mtx;
    }
}

struct numtx_s* fn_80013550(struct nuhspecial_s* special)
{
    if (special->special != NULL) {
        return &special->special->instance->mtx;
    }
    if (special->display != NULL) {
        return &special->display->drawMtx;
    }
    return NULL;
}

char* NuSpecialGetInstAnim(struct nuhspecial_s* special)
{
    if (special->display != NULL) {
        return special->display->name;
    }
    if (special->special != NULL) {
        return special->special->name;
    }
    return NULL;
}

s32 fn_800135B0(struct nuhspecial_s* special)
{
    struct nuspecial_s* sp = special->special;

    if (sp != NULL) {
        struct nugscn_s* scene = special->scene;
        s32 i;
        for (i = 0; i < scene->numInstance; i++) {
            if (&scene->instances[i] == sp->instance) {
                return i;
            }
        }
    } else if (special->display != NULL) {
        return special->display->instanceIx;
    }
    return -1;
}

void fn_80013618(struct nuhspecial_s* special, struct nuvec_s* min, struct nuvec_s* max)
{
    if (special->special != NULL) {
        struct nugobj_s* gobj = special->scene->gobjs[special->special->instance->objid];
        while (gobj->nextGobj != NULL) {
            gobj = gobj->nextGobj;
        }
        *min = gobj->boundingBoxMin;
        *max = gobj->boundingBoxMax;
    } else if (special->display != NULL) {
        *min = special->display->boundsMin;
        *max = special->display->boundsMax;
    }
}

void fn_800136CC(struct nuhspecial_s* special, struct nuvec_s* pos, f32* radius)
{
    if (special->special != NULL) {
        *radius = special->scene->gobjs[special->special->instance->objid]->boundingRadiusFromCenter;
        *pos = special->scene->gobjs[special->special->instance->objid]->boundingBoxCenter;
    } else {
        *pos = special->display->center;
        *radius = special->display->radius;
    }
}

f32 fn_80013764(struct nuhspecial_s* special)
{
    f32 r;

    if (special->special != NULL) {
        r = special->scene->gobjs[special->special->instance->objid]->boundingRadiusFromOrigin;
    } else {
        r = fn_8000CB50(&special->display->center) + special->display->radius;
    }
    return r;
}

s32 fn_800137D0(struct nuhspecial_s* special, struct numtx_s* mtx)
{
    if (special->special != NULL) {
        struct nugobj_s* gobj = special->scene->gobjs[special->special->instance->objid];
        return fn_80023628(&gobj->boundingBoxMin, &gobj->boundingBoxMax, mtx, lbl_8018F9EC[0], 0);
    }
    return fn_80023628(&special->display->boundsMin, &special->display->boundsMax, mtx, lbl_8018F9EC[0], 0);
}

s32 NuSpecialFindMultiWC(struct nuhspecial_s* special, struct numtx_s* mtx)
{
    if (special == NULL || special->scene == NULL) {
        return 0;
    }
    if (special->special != NULL) {
        return fn_80027688(special->scene->gobjs[special->special->instance->objid], mtx);
    }
    if (special->display != NULL) {
        return fn_800119C8(special, mtx, 0, 0, 0);
    }
    return 0;
}

s32 fn_80013124(struct nuhspecial_s* special, s32 a, struct numtx_s* mtx, s32 b)
{
    if (special->scene == NULL) {
        return 0;
    }
    if (special->display == NULL) {
        return 0;
    }
    return fn_800119C8(special, mtx, 2, a, b);
}

void fn_80013184(s32 enable, f32 alpha)
{
    if (enable != 0) {
        lbl_80407BAC = enable;
        lbl_80407BB0 = alpha;
        lbl_80407BB8 |= 1;
    } else {
        lbl_80407BAC = enable;
        lbl_80407BB8 &= ~1;
    }
}
