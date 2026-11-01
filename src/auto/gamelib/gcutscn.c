#include "types.h"

extern s32 lbl_804081EC;
extern s32 lbl_8040A42C;
extern s32 lbl_8040A428;
extern s32 lbl_804081B8;
extern s32 lbl_804081C0;
extern s32 lbl_804081C4;
extern s32 lbl_804081CC;
extern s32 lbl_804081D0;
extern s32 lbl_804081DC;

s32 fn_8003A214(void)
{
    return 0;
}

s32 fn_8003A21C(void)
{
    return 0;
}

s32 fn_8003A224(void)
{
    return 0;
}

s32 fn_8003A22C(void)
{
    return 0;
}

s32 fn_8003A234(void)
{
    return 0;
}

s32 fn_8003A23C(void)
{
    return 0;
}

void fn_8003F178(s32 a)
{
    lbl_804081B8 = a;
}

void fn_8003F180(s32 a)
{
    lbl_804081C0 = a;
}

void fn_8003F188(s32 a)
{
    lbl_804081C4 = a;
}

void fn_8003F190(s32 a)
{
    lbl_804081CC = a;
}

void fn_8003F198(s32 a)
{
    lbl_804081D0 = a;
}

void fn_8003F1A0(s32 a)
{
    lbl_804081DC = a;
}

void NuGCutSceneSysInit(s32 a)
{
    lbl_804081EC = a;
    lbl_8040A428 = 0;
    lbl_8040A42C = 0;
}

typedef struct gcut_matrix {
    f32 m[16];
} gcut_matrix;
typedef struct gcut_bounds {
    f32 min[3];
    f32 max[3];
} gcut_bounds;
typedef struct gcut_scene_view {
    s32 version;
    u8 pad04[0x1c];
    gcut_bounds* bounds;
} gcut_scene_view;
typedef struct gcut_instance_view {
    struct gcut_instance_view* next;
    u8 pad04[0x14];
    gcut_matrix matrix;
    gcut_scene_view* scene;
    u8 pad5c[4];
    f32 centre[3];
    u8 pad6c[0x1c];
    s32 flags;
} gcut_instance_view;
extern void NuVecMtxTransform(void*, void*, void*);

void instNuGCutSceneCalculateCentre(gcut_instance_view* instance, gcut_matrix* matrix)
{
    gcut_scene_view* scene = instance->scene;
    if (scene->bounds) {
        instance->centre[0] = (scene->bounds->max[0] + scene->bounds->min[0]) * 0.5f;
        instance->centre[1] = (scene->bounds->max[1] + scene->bounds->min[1]) * 0.5f;
        instance->centre[2] = (scene->bounds->max[2] + scene->bounds->min[2]) * 0.5f;
    } else {
        instance->centre[0] = 0.0f;
        instance->centre[1] = 0.0f;
        instance->centre[2] = 0.0f;
    }
    if (matrix) {
        NuVecMtxTransform(instance->centre, instance->centre, matrix);
    }
}
void instNuGCutSceneSetEndCallback(gcut_instance_view* instance, gcut_matrix* matrix)
{
    instance->flags |= 0x1000000;
    instance->matrix = *matrix;
    instNuGCutSceneCalculateCentre(instance, &instance->matrix);
}
extern s32 lbl_80408190;
extern s32 lbl_8040819C;
extern s32 lbl_804081A0;
extern s32 lbl_804081A4;
extern s32 lbl_804081AC;
extern void fn_8003BA5C(gcut_instance_view*, s32, s32, f32);
void NuGCutSceneSysUpdate(s32 paused, s32 skip, f32 elapsed)
{
    gcut_instance_view* instance = (gcut_instance_view*)lbl_8040A42C;
    lbl_80408190++;
    if (lbl_80408190 > 255) {
        lbl_80408190 = 1;
    }
    lbl_804081A0 = 0;
    lbl_804081A4 = 0;
    lbl_804081AC = 0;
    lbl_8040819C = 0;
    if (paused) {
        skip = 0;
    }
    for (; instance; instance = instance->next) {
        if ((instance->flags << 19 >> 24) != lbl_80408190)
            fn_8003BA5C(instance, paused, skip, elapsed);
    }
}
extern void* memset();
typedef struct gcut_camera_instance {
    s32 unused;
    void* states;
    s32 a;
    s32 b;
} gcut_camera_instance;
gcut_camera_instance* instNuCGutCamSysCreate(s32* camera, u32* buffer)
{
    gcut_camera_instance* result = 0;
    if (camera && *camera && buffer) {
        result = (gcut_camera_instance*)((*buffer + 15) & ~15);
        *buffer = (u32)(result + 1);
        memset(result, 0, 16);
        *buffer = (*buffer + 15) & ~15;
        result->states = (void*)*buffer;
        *buffer += *camera * 4;
        memset(result->states, 0, *camera * 4);
    }
    return result;
}
typedef struct gcut_trigger_instance {
    void* scene;
    void* states;
} gcut_trigger_instance;
gcut_trigger_instance* instNuCGutTriggerSysCreate(s32* scene, void* instance, u32* buffer)
{
    s32 count = *(s32*)scene[9];
    gcut_trigger_instance* result = (gcut_trigger_instance*)((*buffer + 15) & ~15);
    *buffer = (u32)(result + 1);
    memset(result, 0, 8);
    result->scene = instance;
    result->states = (void*)*buffer;
    *buffer += count * 4;
    memset(result->states, 0, count * 4);
    return result;
}
extern void NuGCutRigidSysFixUp(s32*);
extern void NuGCutCharSysFixUp(s32*);
extern void NuGCutLocatorSysFixUp(s32*, s32);
extern void NuGCutTriggerSysFixUp(s32*, s32);
void NuGCutSceneFixUp(s32* scene, s32 area, s32 flags, s32 id)
{
    if (scene) {
        if (scene[0] > 1) {
            scene[17] = area;
            scene[18] = flags;
        }
        if (scene[5]) {
            NuGCutRigidSysFixUp(scene);
        }
        if (scene[6]) {
            NuGCutCharSysFixUp(scene);
        }
        if (scene[7]) {
            NuGCutLocatorSysFixUp(scene, id);
        }
        if (scene[9] && flags) {
            NuGCutTriggerSysFixUp(scene, flags);
        }
    }
}
extern void* StateAnimFixPtrs(void*, s32);
extern void* fn_8001678C(void*, s32);
void NuGCutCamsSysFixPtrs(s32* scene, s32 delta, s32 animDelta)
{
    if (scene[4]) {
        s32* camera = (s32*)(scene[4] + delta);
        scene[4] = (s32)camera;
        camera[1] = camera[1] ? camera[1] + delta : 0;
        if (animDelta) {
            if (camera[2]) {
                camera[2] = (s32)fn_8001678C((void*)camera[2], animDelta);
            }
            camera[3] = (s32)StateAnimFixPtrs((void*)camera[3], animDelta);
        }
    }
}

typedef struct gcut_trigger_event {
    s32 unused;
    s32 name;
    void* anim;
} gcut_trigger_event;
typedef struct gcut_trigger_system {
    s32 count;
    gcut_trigger_event* events;
} gcut_trigger_system;
void NuGCutTriggerSysFixPtrs(s32* scene, s32 delta)
{
    if (scene[9]) {
        gcut_trigger_system* sys = (gcut_trigger_system*)(scene[9] + delta);
        s32 i;
        scene[9] = (s32)sys;
        sys->events = sys->events ? (gcut_trigger_event*)((s32)sys->events + delta) : 0;
        if (sys->events) {
            for (i = 0; i < sys->count; i++) {
                gcut_trigger_event* event = &sys->events[i];
                event->name = event->name ? event->name + delta : 0;
                event->anim = StateAnimFixPtrs(event->anim, delta);
            }
        }
    }
}
typedef struct gcut_rigid_view {
    u8 pad[0x40];
    s32 name;
    u8 pad44[8];
    void* anim;
    void* state;
    u8 pad54[8];
} gcut_rigid_view;
typedef struct gcut_rigid_system {
    gcut_rigid_view* rigids;
    u16 count;
} gcut_rigid_system;
void NuGCutRigidSysFixPtrs(s32* scene, s32 delta, s32 animDelta)
{
    if (scene[5]) {
        gcut_rigid_system* sys = (gcut_rigid_system*)(scene[5] + delta);
        s32 i;
        scene[5] = (s32)sys;
        sys->rigids = sys->rigids ? (gcut_rigid_view*)((s32)sys->rigids + delta) : 0;
        if (sys->rigids) {
            for (i = 0; i < sys->count; i++) {
                gcut_rigid_view* rigid = &sys->rigids[i];
                if (rigid->name) {
                    rigid->name = scene[3] + rigid->name - 1;
                }
                if (animDelta) {
                    if (rigid->anim) {
                        rigid->anim = fn_8001678C(rigid->anim, animDelta);
                    }
                    rigid->state = StateAnimFixPtrs(rigid->state, animDelta);
                }
            }
        }
    }
}
typedef struct gcut_character_view {
    u8 pad[0x40];
    s32 name;
    void* anim;
    void* face;
    void* extra;
    u8 pad50[0x14];
} gcut_character_view;
typedef struct gcut_character_system {
    gcut_character_view* characters;
    u16 count;
} gcut_character_system;
void NuGCutCharSysFixPtrs(s32* scene, s32 delta, s32 animDelta)
{
    if (scene[6]) {
        gcut_character_system* sys = (gcut_character_system*)(scene[6] + delta);
        s32 i;
        scene[6] = (s32)sys;
        sys->characters = sys->characters ? (gcut_character_view*)((s32)sys->characters + delta) : 0;
        if (sys->characters) {
            for (i = 0; i < sys->count; i++) {
                gcut_character_view* character = &sys->characters[i];
                if (character->name) {
                    character->name = scene[3] + character->name - 1;
                }
                if (animDelta) {
                    if (character->anim) {
                        character->anim = fn_8001678C(character->anim, animDelta);
                    }
                    if (character->face) {
                        character->face = fn_8001678C(character->face, animDelta);
                    }
                    if (character->extra) {
                        character->extra = fn_8001678C(character->extra, animDelta);
                    }
                }
            }
        }
    }
}
