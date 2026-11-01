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
extern void NuGCutRigidSysFixUp(s32*, s32);
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
            NuGCutRigidSysFixUp(scene, area);
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

s32 instNuGCutSceneDestroy(gcut_instance_view* instance)
{
    return (instance->flags << 12) >> 31;
}

typedef struct gcut_flag_bits {
    unsigned int unused : 2;
    unsigned int flag : 1;
    unsigned int rest : 29;
} gcut_flag_bits;

void instNuGCutSceneStart(gcut_instance_view* instance, s32 value)
{
    ((gcut_flag_bits*)&instance->flags)->flag = value;
}

extern s32 lbl_804081C8;
void NuGCutCharSysFixUp(s32* scene)
{
    if (lbl_804081C4)
        ((void (*)(s32*))lbl_804081C4)(scene);
}

void NuGCutCharSysDestroy(s32* scene)
{
    if (lbl_804081C8)
        ((void (*)(s32*))lbl_804081C8)(scene);
}

typedef struct gcut_float_pair { f32 value; f32 other; } gcut_float_pair;
typedef struct gcut_float_array { gcut_float_pair* values; } gcut_float_array;
typedef struct gcut_count8 { u8 pad[8]; u8 count; } gcut_count8;
void NuGCutLocatorCalcMtx(gcut_float_array* out, gcut_count8* count)
{
    u32 i;
    gcut_float_pair* values;
    f32* dest;
    for (i = 0; i < count->count; i++) {
        values = out->values;
        *(dest = (f32*)((u32)values + i * 8)) = 0.0f;
    }
}
void NuGCutLocatorIsVisble(gcut_float_array* out, gcut_count8* count)
{
    u32 i;
    gcut_float_pair* values;
    f32* dest;
    for (i = 0; i < count->count; i++) {
        values = out->values;
        *(dest = (f32*)((u32)values + i * 8)) = 0.0f;
    }
}

typedef struct gcut_trigger_byte { u8 value; u8 pad[3]; } gcut_trigger_byte;
typedef struct gcut_trigger_state { void* unused; gcut_trigger_byte* values; } gcut_trigger_state;
void fn_800407B0(gcut_instance_view* instance)
{
    s32 i;
    s32* system = (s32*)instance->scene;
    s32* trigger = (s32*)system[9];
    gcut_trigger_state* state = *(gcut_trigger_state**)((u8*)instance + 0xac);
    for (i = 0; i < *trigger; i++) {
        gcut_trigger_byte* values = state->values;
        u8* dest;
        *(dest = (u8*)((u32)values + i * 4)) = 0;
    }
}

void NuGCutSceneDestroy(s32* scene)
{
    if (scene[6])
        NuGCutCharSysDestroy(scene);
}
extern void NuGCutRigidCalcMtx(s32*, void*);
void instNuGCutSceneCreate(s32* scene, void* lookup)
{
    if (scene && scene[5])
        NuGCutRigidCalcMtx(scene, lookup);
}
void* StateAnimFixPtrs(void* state, s32 delta)
{
    s32* result = state ? (s32*)((u32)state + delta) : 0;
    if (result) {
        result[1] = result[1] ? result[1] + delta : 0;
        result[2] = result[2] ? result[2] + delta : 0;
    }
    return result;
}

extern void fn_8003C754(gcut_instance_view*, s32);
void NuGCutSceneSysRender(s32 arg)
{
    gcut_instance_view* instance = (gcut_instance_view*)lbl_8040A42C;
    for (; instance; instance = instance->next)
        fn_8003C754(instance, arg);
}
extern void fn_8000DC80(gcut_matrix*);
void instNuGCutSceneEnd(gcut_instance_view* instance)
{
    instance->flags |= 0x1000000;
    fn_8000DC80(&instance->matrix);
    instNuGCutSceneCalculateCentre(instance, &instance->matrix);
}

typedef struct gcut_cam_state { u8 flags; u8 value; u8 pad[2]; } gcut_cam_state;
typedef struct gcut_cam_runtime { u32 unused; gcut_cam_state* states; u8 a, b, c; } gcut_cam_runtime;
typedef struct gcut_cam_data { u32 count; u8 pad[12]; u8 value; } gcut_cam_data;
void instNuGCutCamSysStart(gcut_cam_runtime* runtime, gcut_cam_data* data)
{
    u32 i;
    runtime->a = 0;
    runtime->b = data->value;
    runtime->c = 0;
    for (i = 0; i < data->count; i++) {
        gcut_cam_state* state = &runtime->states[i];
        state->flags &= 0xfd;
        state->value = 0;
    }
}

typedef struct gcut_rigid_runtime_item { u8 pad[12]; u8 a; u8 b; u8 pad2[2]; } gcut_rigid_runtime_item;
typedef struct gcut_rigid_runtime { gcut_rigid_runtime_item* items; } gcut_rigid_runtime;
void instNuGCutRigidSysRender(gcut_rigid_runtime* runtime, gcut_rigid_system* system)
{
    u32 i;
    for (i = 0; i < system->count; i++) {
        gcut_rigid_view* rigid = &system->rigids[i];
        gcut_rigid_runtime_item* item = &runtime->items[i];
        u32 flags;
        item->a = 0;
        flags = *(u32*)((u8*)rigid + 0x58);
        if ((flags & 0x06000000) == 0x06000000)
            item->b = *((u8*)rigid + 0x58) & 1;
    }
}

typedef struct gcut_character_runtime_item { s32 value; u8 pad[20]; } gcut_character_runtime_item;
typedef struct gcut_character_runtime { gcut_character_runtime_item* items; } gcut_character_runtime;
extern s32 lbl_804081BC;
void NuSetCutSceneCharacterRenderFn(gcut_instance_view* instance)
{
    s32 i;
    gcut_character_runtime* runtime = *(gcut_character_runtime**)((u8*)instance + 0xa4);
    gcut_character_system* system = (gcut_character_system*)((s32*)instance->scene)[6];
    for (i = 0; i < system->count; i++) {
        gcut_character_view* character = &system->characters[i];
        gcut_character_runtime_item* item = &runtime->items[i];
        if (item->value && lbl_804081BC)
            ((void (*)(gcut_character_runtime_item*, gcut_character_view*))lbl_804081BC)(item, character);
    }
}

extern s32 lbl_804081D4;
void instNuGCutCharSysRender(gcut_instance_view* instance)
{
    s32 i;
    gcut_character_runtime* runtime = *(gcut_character_runtime**)((u8*)instance + 0xa4);
    gcut_character_system* system = (gcut_character_system*)((s32*)instance->scene)[6];
    if (system && system->count) {
        for (i = 0; i < system->count; i++) {
            gcut_character_view* character = &system->characters[i];
            gcut_character_runtime_item* item = &runtime->items[i];
            if ((*((u8*)character + 0x5c) & 2) && lbl_804081D4)
                ((void (*)(gcut_character_view*, gcut_character_runtime_item*))lbl_804081D4)(character, item);
        }
    }
}


extern void fn_80013200(gcut_rigid_runtime_item*, s32);
extern void fn_80013454(gcut_rigid_runtime_item*, gcut_rigid_view*);
void instNuGCutRigidSysUpdate(gcut_rigid_runtime* runtime, gcut_rigid_system* system)
{
    u32 i;
    for (i = 0; i < system->count; i++) {
        gcut_rigid_view* rigid = &system->rigids[i];
        gcut_rigid_runtime_item* item = &runtime->items[i];
        item->a = 0;
        if (*((u8*)rigid + 0x58) & 4) {
            if (*((u8*)rigid + 0x58) & 2)
                item->b = *((u8*)rigid + 0x58) & 1;
            else {
                if (*((u8*)rigid + 0x58) & 1)
                    fn_80013200(item, 1);
                else
                    fn_80013200(item, 0);
                fn_80013454(item, rigid);
            }
        }
    }
}

typedef struct gcut_locator_record { u8 pad[0x59]; u8 event; u8 pad2[10]; } gcut_locator_record;
typedef struct gcut_locator_event { u8 pad[4]; u8 flags; u8 pad5; u16 index; u8 pad8[4]; } gcut_locator_event;
typedef struct gcut_locator_system { gcut_locator_record* records; gcut_locator_event* events; u8 count; } gcut_locator_system;
typedef struct gcut_locator_runtime_item { u8 pad[8]; } gcut_locator_runtime_item;
typedef struct gcut_locator_runtime { gcut_locator_runtime_item* items; } gcut_locator_runtime;
typedef struct gcut_event_callback { u8 pad[8]; s32 callback; u8 pad2[8]; } gcut_event_callback;
void instNuGCutLocatorSysStart(gcut_instance_view* instance)
{
    u32 i;
    gcut_locator_system* system = (gcut_locator_system*)((s32*)instance->scene)[7];
    gcut_locator_runtime* runtime = *(gcut_locator_runtime**)((u8*)instance + 0xa8);
    for (i = 0; i < system->count; i++) {
        gcut_locator_runtime_item* item = &runtime->items[i];
        gcut_locator_record* record = &system->records[i];
        gcut_locator_event* event = &system->events[record->event];
        if ((event->flags & 2) && event->index != 0xffff) {
            s32 callback = ((gcut_event_callback*)lbl_804081EC)[event->index].callback;
            if (callback)
                ((void (*)(gcut_instance_view*, gcut_locator_system*, gcut_locator_runtime_item*, gcut_locator_record*))callback)(instance, system, item, record);
        }
    }
}

extern s32 lbl_804081E4;
extern void instNuGCutDebrisLocatorUpdate(gcut_locator_system*, gcut_locator_runtime_item*, gcut_locator_record*, s32, f32);
void instNuGCutSceneUpdate(gcut_instance_view* instance, gcut_locator_system* system,
                          gcut_locator_runtime_item* item, gcut_locator_record* record,
                          f32 frame, s32 arg, s32 skip)
{
    gcut_locator_event* event = &system->events[record->event];
    if (event->flags & 1) {
        if (!skip)
            instNuGCutDebrisLocatorUpdate(system, item, record, arg, frame);
    } else if (event->flags & 2) {
        s32 callback = *(s32*)((u8*)&((gcut_event_callback*)lbl_804081EC)[event->index] + 0x10);
        if (callback && event->index != 0xffff)
            ((void (*)(gcut_instance_view*, gcut_locator_system*, gcut_locator_runtime_item*, gcut_locator_record*, s32, s32, f32))callback)(instance, system, item, record, arg, skip, frame);
    } else if ((event->flags & 4) && lbl_804081E4 && *(f32*)((u8*)instance + 0x98) != 0.0f) {
        ((void (*)(gcut_locator_system*, gcut_locator_runtime_item*, gcut_locator_record*, s32, s32, f32))lbl_804081E4)(system, item, record, arg, skip, frame);
    }
}

typedef struct gcut_area_entry { char* name; u8 pad[0x30]; } gcut_area_entry;
typedef struct gcut_area { u8 pad[8]; s32 count; gcut_area_entry* entries; } gcut_area;
extern s32 lbl_80155DE8(char*, char*);
extern char lbl_8019308C[];
extern char str_NuGCutTriggerSysFixUp_TriggerNotFound[];
extern void (*fn_80009948(char*, s32))(char*, ...);
void NuGCutTriggerSysFixUp(s32* scene, s32 area_value)
{
    gcut_trigger_system* system = (gcut_trigger_system*)scene[9];
    gcut_area* area = (gcut_area*)area_value;
    s32 i, j;
    for (i = 0; i < system->count; i++) {
        gcut_trigger_event* event = &system->events[i];
        for (j = 0; j < area->count; j++) {
            *(s16*)event = -1;
            if (lbl_80155DE8((char*)event->name, area->entries[j].name) == 0) {
                *(s16*)event = j;
                break;
            }
        }
        if (*(s16*)event == -1)
            fn_80009948(lbl_8019308C, 0x1345)(str_NuGCutTriggerSysFixUp_TriggerNotFound, event->name);
    }
}

typedef struct gcut_area_flag { u8 pad[2]; u8 flags; u8 pad3; } gcut_area_flag;
typedef struct gcut_trigger_scene { u8 pad[12]; gcut_area_flag* flags; } gcut_trigger_scene;
typedef struct gcut_trigger_runtime { gcut_trigger_scene* scene; gcut_trigger_byte* states; } gcut_trigger_runtime;
extern s32 fn_800407F4(void*, gcut_trigger_byte*, u8*, f32);
void fn_8003EF38(gcut_instance_view* instance)
{
    gcut_trigger_system* system = (gcut_trigger_system*)((s32*)instance->scene)[9];
    gcut_trigger_runtime* runtime = *(gcut_trigger_runtime**)((u8*)instance + 0xac);
    s32 i;
    for (i = 0; i < system->count; i++) {
        u8 value;
        gcut_trigger_event* event = &system->events[i];
        gcut_trigger_byte* state = &runtime->states[i];
        state->value = 0;
        if (fn_800407F4(event->anim, state, &value, 1.0f)) {
            if (value)
                runtime->scene->flags[*(s16*)event].flags |= 1;
            else
                runtime->scene->flags[*(s16*)event].flags &= ~1;
        }
    }
}

void fn_800406C8(gcut_instance_view* instance, f32 frame)
{
    gcut_trigger_system* system = (gcut_trigger_system*)((s32*)instance->scene)[9];
    gcut_trigger_runtime* runtime = *(gcut_trigger_runtime**)((u8*)instance + 0xac);
    s32 i;
    for (i = 0; i < system->count; i++) {
        u8 value;
        gcut_trigger_event* event = &system->events[i];
        if (event->anim && fn_800407F4(event->anim, &runtime->states[i], &value, frame)) {
            if (value)
                runtime->scene->flags[*(s16*)event].flags |= 1;
            else
                runtime->scene->flags[*(s16*)event].flags &= ~1;
        }
    }
}

extern s32 lbl_8040810C;
void NuGCutCharAnimProcess(gcut_instance_view* instance, f32 frame)
{
    s32 i;
    gcut_scene_view* scene = instance->scene;
    gcut_character_runtime* runtime = *(gcut_character_runtime**)((u8*)instance + 0xa4);
    gcut_character_system* system = (gcut_character_system*)((s32*)scene)[6];
    for (i = 0; i < system->count; i++) {
        gcut_character_view* character = &system->characters[i];
        gcut_character_runtime_item* item = &runtime->items[i];
        if (item->value) {
            if (!(*((u8*)character + 0x5c) & 2) && lbl_804081C0)
                ((void (*)(gcut_instance_view*, gcut_scene_view*, gcut_character_runtime_item*, gcut_character_view*, f32))lbl_804081C0)(instance, scene, item, character, frame);
            if (!lbl_8040810C && lbl_804081BC)
                ((void (*)(gcut_character_runtime_item*, gcut_character_view*))lbl_804081BC)(item, character);
        }
    }
}

void instNuGCutLocatorSysUpdate(gcut_instance_view* instance, f32 frame, s32 skip)
{
    u32 i;
    gcut_locator_system* system = (gcut_locator_system*)((s32*)instance->scene)[7];
    gcut_locator_runtime* runtime = *(gcut_locator_runtime**)((u8*)instance + 0xa8);
    for (i = 0; i < system->count; i++) {
        gcut_locator_record* record = &system->records[i];
        if (*((u8*)record + 0x58) & 1) continue;
        if (*((u8*)record + 0x58) & 2) continue;
        {
            if (instance->flags & 0x1000000)
                instNuGCutSceneUpdate(instance, system, &runtime->items[i], record, frame, (s32)&instance->matrix, skip);
            else
                instNuGCutSceneUpdate(instance, system, &runtime->items[i], record, frame, 0, skip);
        }
    }
}
void NuSetCutSceneFindCharactersFn(gcut_instance_view* instance, f32 frame, s32 arg)
{
    s32 i;
    gcut_character_runtime* runtime = *(gcut_character_runtime**)((u8*)instance + 0xa4);
    gcut_scene_view* scene = instance->scene;
    gcut_character_system* system = (gcut_character_system*)((s32*)scene)[6];
    if (lbl_804081B8) {
        for (i = 0; i < system->count; i++) {
            gcut_character_view* character = &system->characters[i];
            gcut_character_runtime_item* item = &runtime->items[i];
            if (item->value)
                ((void (*)(gcut_instance_view*, gcut_scene_view*, gcut_character_runtime_item*, gcut_character_view*, f32, s32))lbl_804081B8)(instance, scene, item, character, frame, arg);
        }
    }
}

typedef struct gcut_special_result { s32 a, b, c; } gcut_special_result;
extern s32 NuSpecialFind(void*, gcut_special_result*, char*, s32);
extern char str_NuGCutRigidSysFixUp_CannotFixupRigid[];
extern void (*fn_80009970(char*, s32))(char*, ...);
void NuGCutRigidCalcMtx(s32* scene, void* lookup)
{
    gcut_rigid_system* system = (gcut_rigid_system*)scene[5];
    s32 i;
    if (lookup) {
        for (i = 0; i < system->count; i++) {
            gcut_special_result special;
            gcut_rigid_view* rigid = &system->rigids[i];
            if (!(*((u8*)rigid + 0x58) & 4)) {
                if (NuSpecialFind(lookup, &special, (char*)rigid->name, 1)) {
                    *((u8*)rigid + 0x58) |= 4;
                    *(s32*)((u8*)rigid + 0x44) = special.a;
                    if (special.c) {
                        *(s32*)((u8*)rigid + 0x48) = special.c;
                        continue;
                    }
                    *(s32*)((u8*)rigid + 0x48) = special.b;
                } else {
                    fn_80009970(lbl_8019308C, 0xee7)(str_NuGCutRigidSysFixUp_CannotFixupRigid, rigid->name);
                }
            }
        }
    }
}

void NuGCutLocatorSysFixPtrs(s32* scene, s32 delta, s32 animDelta)
{
    if (scene[7]) {
        gcut_locator_system* system = (gcut_locator_system*)(scene[7] + delta);
        s32 i;
        scene[7] = (s32)system;
        system->records = system->records ? (gcut_locator_record*)((s32)system->records + delta) : 0;
        if (system->records && animDelta) {
            for (i = 0; i < system->count; i++) {
                gcut_locator_record* record = &system->records[i];
                void** anim = (void**)((u8*)record + 0x50);
                if (*anim)
                    *anim = fn_8001678C(*anim, animDelta);
            }
        }
        system->events = system->events ? (gcut_locator_event*)((s32)system->events + delta) : 0;
        if (system->events) {
            for (i = 0; i < *((u8*)system + 9); i++) {
                s32* name = (s32*)&system->events[i];
                *name = scene[3] + *name - 1;
            }
        }
    }
}

extern s8 lbl_804081B4;
extern char lbl_801932BC[], lbl_801932FC[], lbl_80193344[];
extern void (*fn_80009984(char*, s32))(s32, char*, ...);
s32 fn_80040974(gcut_instance_view* instance, s32 arg)
{
    s32* words = (s32*)instance;
    if ((words[0x8c / 4] & 0x1000000) && !arg) {
        fn_80009984(lbl_8019308C, 0x8aa)(lbl_804081B4, lbl_801932BC, ((s32*)instance->scene)[11]);
        return 0;
    }
    if (words[0xe4 / 4]) {
        fn_80009984(lbl_8019308C, 0x8b1)(lbl_804081B4, lbl_801932FC, ((s32*)instance->scene)[11]);
        return 0;
    }
    if (instance->flags & 8) {
        words[0xe4 / 4] = words[0xc0 / 4];
        instance->flags &= ~8;
    } else {
        words[0xe4 / 4] = words[0xbc / 4];
        instance->flags |= 8;
    }
    fn_80009984(lbl_8019308C, 0x8c1)(lbl_804081B4, lbl_80193344, ((s32*)instance->scene)[11]);
    return 1;
}

void NuGCutRigidSysFixUp(s32* scene, s32 lookup)
{
    gcut_rigid_system* system = (gcut_rigid_system*)scene[5];
    gcut_locator_system* locators = (gcut_locator_system*)scene[7];
    s32 i;
    if (lookup) {
        for (i = 0; i < system->count; i++) {
            gcut_special_result special;
            gcut_rigid_view* rigid = &system->rigids[i];
            u8* bytes = (u8*)rigid;
            if (NuSpecialFind((void*)lookup, &special, (char*)rigid->name, 1)) {
                bytes[0x58] |= 4;
                *(s32*)(bytes + 0x44) = special.a;
                if (special.c)
                    *(s32*)(bytes + 0x48) = special.c;
                else
                    *(s32*)(bytes + 0x48) = special.b;
            } else {
                fn_80009970(lbl_8019308C, 0xeb7)(str_NuGCutRigidSysFixUp_CannotFixupRigid, rigid->name);
            }
            if (bytes[0x59] && *(s32*)(bytes + 0x54) <= 0xfe) {
                bytes[0x5a] = bytes[0x57];
                *(s32*)(bytes + 0x54) = (s32)&locators->records[*(s32*)(bytes + 0x54)];
            } else {
                bytes[0x5a] = 0xff;
            }
        }
    }
}

typedef struct gcut_anim { s32 magic; u8 pad[8]; f32* curves; s8* types; } gcut_anim;
extern f32* fn_8001BFC4(gcut_anim*, s32);
extern f32 NuAnimCurve2CalcVal(f32*, void*, s32);
s32 instNuGCutSceneClipTest(gcut_locator_record* locator, void* time, f32* scale)
{
    gcut_anim* anim = *(gcut_anim**)((u8*)locator + 0x50);
    s32 visible;
    if (anim) {
        if (anim->magic == 0x414e4934) {
            f32* values = fn_8001BFC4(anim, 0);
            s32 result = (s32)values[7];
            if (result) {
                if (scale)
                    *scale = values[6];
            }
            return result;
        } else {
            s8* types = anim->types;
            f32* curves = anim->curves;
            if (types[7] == 0)
                visible = (s32)curves[7];
            else
                visible = (s32)NuAnimCurve2CalcVal(&curves[7], time, types[7]);
            if (visible && scale) {
                f32 value;
                if (types[6] == 0)
                    value = curves[6];
                else
                    value = NuAnimCurve2CalcVal(&curves[6], time, types[6]);
                *scale = value;
            }
        }
    } else {
        visible = *((u8*)locator + 0x58) & 8;
        if (visible && scale)
            *scale = *(f32*)((u8*)locator + 0x4c);
    }
    return visible;
}

extern void instNuGCutRigidSysStart(gcut_instance_view*, f32);
void fn_8003B310(gcut_instance_view* instance)
{
    s32* words = (s32*)instance;
    s32 flags = words[0x88/4];
    s32* scene = (s32*)instance->scene;
    words[0x88/4] = flags & ~0x40000000;
    words[0x88/4] &= ~0x80000;
    *(f32*)((u8*)instance + 0x90) = 1.0f;
    words[0x8c/4] &= ~0x2000000;
    *(f32*)((u8*)instance + 0x94) = 1.0f;
    if (flags & 0x2000) {
        if (scene[5])
            instNuGCutRigidSysStart(instance, *(f32*)((u8*)instance->scene + 8) - *(f32*)((u8*)instance + 0x90));
        if (words[0xa4/4])
            NuGCutCharAnimProcess(instance, *(f32*)((u8*)instance->scene + 8) - *(f32*)((u8*)instance + 0x90));
        if (words[0xa8/4])
            ((void (*)(gcut_float_array*, gcut_count8*, f32))NuGCutLocatorCalcMtx)((gcut_float_array*)words[0xa8/4], (gcut_count8*)scene[7], *(f32*)((u8*)instance->scene + 8) - *(f32*)((u8*)instance + 0x90));
    } else {
        if (scene[5])
            instNuGCutRigidSysStart(instance, *(f32*)((u8*)instance + 0x90));
        if (words[0xa4/4])
            NuGCutCharAnimProcess(instance, *(f32*)((u8*)instance + 0x90));
        if (words[0xa8/4])
            ((void (*)(gcut_float_array*, gcut_count8*, f32))NuGCutLocatorCalcMtx)((gcut_float_array*)words[0xa8/4], (gcut_count8*)scene[7], *(f32*)((u8*)instance + 0x90));
    }
}

gcut_character_runtime* instNuCGutCharSysCreate(s32* scene, u32* memory)
{
    gcut_character_system* system = (gcut_character_system*)scene[6];
    gcut_character_runtime* result = 0;
    s32 i;
    if (system && system->count && memory) {
        gcut_character_runtime* aligned = (gcut_character_runtime*)((*memory + 15) & ~15);
        result = aligned;
        *memory = (u32)(result + 1);
        result->items = 0;
        *memory = (*memory + 15) & ~15;
        result->items = (gcut_character_runtime_item*)*memory;
        *memory += system->count * sizeof(gcut_character_runtime_item);
        memset(result->items, 0, system->count * sizeof(gcut_character_runtime_item));
        for (i = 0; i < system->count; i++) {
            gcut_character_view* character = &system->characters[i];
            gcut_character_runtime_item* item = &aligned->items[i];
            ((u8*)item)[0x16] = 0xff;
            ((u8*)item)[0x15] = 0xff;
            if (((u8*)character)[0x5c] & 2) {
                if (lbl_804081D0)
                    ((void (*)(gcut_character_view*, gcut_character_runtime_item*, u32*))lbl_804081D0)(character, item, memory);
            } else {
                item->value = *(s32*)((u8*)character + 0x50);
            }
        }
    }
    return result;
}

extern s32 fn_800131B8(gcut_rigid_runtime_item*);
extern void instNuGCutRigidSysEnd(gcut_rigid_view*, f32, gcut_matrix*);
extern void fn_8000D134(gcut_matrix*, gcut_matrix*, gcut_matrix*);
void instNuGCutRigidSysStart(gcut_instance_view* instance, f32 frame)
{
    gcut_rigid_system* system = (gcut_rigid_system*)((s32*)instance->scene)[5];
    gcut_rigid_runtime* runtime = *(gcut_rigid_runtime**)((u8*)instance + 0xa0);
    u8 visible;
    gcut_matrix matrix;
    s32 i;
    for (i = 0; i < system->count; i++) {
        gcut_rigid_view* rigid = &system->rigids[i];
        gcut_rigid_runtime_item* item = &runtime->items[i];
        if (*((u8*)rigid + 0x58) & 4) {
          if (!(*((u8*)rigid + 0x58) & 2)) {
            if (rigid->state && fn_800407F4(rigid->state, (gcut_trigger_byte*)&item->a, &visible, frame)) {
                if (visible)
                    fn_80013200(item, 1);
                else
                    fn_80013200(item, 0);
            }
            if (fn_800131B8(item)) {
                instNuGCutRigidSysEnd(rigid, frame, &matrix);
                if (instance->flags & 0x1000000)
                    fn_8000D134(&matrix, &matrix, &instance->matrix);
                fn_80013454(item, (gcut_rigid_view*)&matrix);
            }
          }
        }
    }
}

extern void NuAnimDataFixPtrs(gcut_instance_view*);
extern s16 lbl_80346FFC[];
extern s32 fn_80044AE8(s32);
extern void fn_80044AF0(s32);
extern void fn_80044B20(s32);
void fn_8003AA94(gcut_instance_view* instance)
{
    gcut_instance_view* previous;
    if (instance->flags & 0x40000000)
        NuAnimDataFixPtrs(instance);
    if (*(s32*)((u8*)instance + 0xa4))
        instNuGCutCharSysRender(instance);
    if (instance->next)
        *(gcut_instance_view**)((u8*)instance->next + 4) = *(gcut_instance_view**)((u8*)instance + 4);
    previous = *(gcut_instance_view**)((u8*)instance + 4);
    if (previous)
        previous->next = instance->next;
    else
        lbl_8040A42C = (s32)instance->next;
    if (*(s16*)((u8*)instance + 0xd8)) {
        if (lbl_80346FFC[*(s8*)((u8*)instance + 0xde)] == *(s16*)((u8*)instance + 0xd8) && fn_80044AE8(*(s8*)((u8*)instance + 0xde)) == *(s16*)((u8*)instance + 0xd8))
            fn_80044AF0(*(s8*)((u8*)instance + 0xde));
        if (*(s16*)((u8*)instance + 0xda) && lbl_80346FFC[*(s8*)((u8*)instance + 0xde) + 1] == *(s16*)((u8*)instance + 0xda) && fn_80044AE8(*(s8*)((u8*)instance + 0xde) + 1) == *(s16*)((u8*)instance + 0xda))
            fn_80044AF0(*(s8*)((u8*)instance + 0xde) + 1);
        fn_80044B20(*(s16*)((u8*)instance + 0xd8));
        if (*(s16*)((u8*)instance + 0xda))
            fn_80044B20(*(s16*)((u8*)instance + 0xda));
    }
}

typedef struct gcut_events { s32 count; f32* times; u8* values; } gcut_events;
extern char lbl_801935D0[];
s32 fn_800407F4(void* events_ptr, gcut_trigger_byte* state, u8* output, f32 frame)
{
    gcut_events* events = events_ptr;
    s32 changed = 0;
    if (state->value < events->count) do {
        if (!(frame >= events->times[state->value]))
            break;
        *output = events->values[state->value];
        changed = 1;
        state->value++;
    } while (state->value < events->count);
    if (!changed) {
        if (state->value) do {
            if (!(frame < events->times[state->value - 1]))
                break;
            state->value--;
            if (state->value)
                *output = events->values[state->value - 1];
            else {
                fn_80009970(lbl_8019308C, 0x13d3)(lbl_801935D0);
                *output = events->values[0];
            }
            changed = 1;
        } while (state->value);
    }
    return changed;
}

extern void fn_80044D0C(void);
void fn_8003ABC4(gcut_instance_view* instance)
{
    s32 flags = instance->flags;
    instance->flags &= ~0x60000000;
    instance->flags &= ~0x80000;
    if (flags & 0x2000) {
        if (*(s32*)((u8*)instance + 0xa4))
            ((void (*)(gcut_instance_view*, f32))NuSetCutSceneCharacterRenderFn)(instance, *(f32*)((u8*)instance->scene + 8) - *(f32*)((u8*)instance + 0x94));
    } else {
        if (*(s32*)((u8*)instance + 0xa4))
            ((void (*)(gcut_instance_view*, f32))NuSetCutSceneCharacterRenderFn)(instance, *(f32*)((u8*)instance + 0x94));
    }
    if (*(s16*)((u8*)instance + 0xd8)) {
        if (lbl_80346FFC[*(s8*)((u8*)instance + 0xde)] == *(s16*)((u8*)instance + 0xd8) && fn_80044AE8(*(s8*)((u8*)instance + 0xde)) == *(s16*)((u8*)instance + 0xd8))
            fn_80044AF0(*(s8*)((u8*)instance + 0xde));
        if (*(s16*)((u8*)instance + 0xda) && lbl_80346FFC[*(s8*)((u8*)instance + 0xde) + 1] == *(s16*)((u8*)instance + 0xda) && fn_80044AE8(*(s8*)((u8*)instance + 0xde) + 1) == *(s16*)((u8*)instance + 0xda))
            fn_80044AF0(*(s8*)((u8*)instance + 0xde) + 1);
        if (!*(u8*)((u8*)instance + 0xde) && (*(s32*)((u8*)instance + 0x8c) & 0x100000)) {
            fn_80044D0C();
            *(s32*)((u8*)instance + 0x8c) &= ~0x100000;
        }
    }
}

gcut_locator_runtime* NuCGutLocatorSysCreateInst(gcut_locator_system* system, u32* memory)
{
    gcut_locator_runtime* result = 0;
    s32 i;
    if (system && system->count && memory) {
        gcut_locator_runtime* aligned = (gcut_locator_runtime*)((*memory + 15) & ~15);
        result = aligned;
        *memory = (u32)(result + 1);
        result->items = 0;
        *memory = (*memory + 15) & ~15;
        result->items = (gcut_locator_runtime_item*)*memory;
        *memory += system->count * sizeof(gcut_locator_runtime_item);
        memset(result->items, 0, system->count * sizeof(gcut_locator_runtime_item));
        for (i = 0; i < system->count; i++) {
            gcut_locator_record* record = &system->records[i];
            gcut_locator_event* event = &system->events[record->event];
            gcut_locator_runtime_item* item = &aligned->items[i];
            if (!(event->flags & 1)) {
                if (!(event->flags & 2)) {
                    if (event->flags & 4) {
                        *memory = (*memory + 15) & ~15;
                        *(void**)((u8*)item + 4) = (void*)*memory;
                        *memory += 16;
                        memset(*(void**)((u8*)item + 4), 0, 16);
                    }
                }
            }
        }
    }
    return result;
}
