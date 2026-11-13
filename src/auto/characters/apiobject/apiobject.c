#include "types.h"

extern s32 lbl_80408D2C;
extern s32 lbl_80408D30;
extern s32 lbl_80408D34;
extern s32 lbl_80408D9C;
extern s32 lbl_80408DA0;

void fn_8008F434(s32 a)
{
    lbl_80408D9C = a;
}

void fn_8008F43C(s32 a)
{
    lbl_80408DA0 = a;
}

void fn_8008F444(s32 a)
{
    lbl_80408D2C = a;
}

void fn_8008F44C(s32 a)
{
    lbl_80408D30 = a;
}

void fn_8008F454(s32 a)
{
    lbl_80408D34 = a;
}

struct aivec_s { f32 x, y, z; };
struct aigroup_s {
    void *leader;
    u8 pad4[0x4C];
    u32 flags_hi : 3;
    u32 is_in_formation : 1;
};
struct aipathroute_s {
    u8 pad0[0x18];
    u64 character_mask;
    u8 pad20[8];
};
struct aipath_s {
    u8 pad0[0x14];
    u8 route_count;
    u8 pad15[0x84 - 0x15];
    struct aipathroute_s *routes;
};
struct aipathcnx_s {
    u8 pad0[0x16];
    u16 route_mask;
};
struct aipathinfo_s {
    struct aipath_s *path;
    struct aipathcnx_s *connection;
    s32 w[4];
};
struct aipacket_s {
    u8 pad0[0x50];
    void *owner;
    u8 pad54[0xA0 - 0x54];
    s16 inside_path_node;
    u8 padA2[6];
    u64 character_type_mask;
    u8 padB0[0xF0 - 0xB0];
    struct aigroup_s *group;
    u8 padF4[0x10C - 0xF4];
    struct aipathinfo_s path_info;
    u8 pad124[0x168 - 0x124];
    u16 available_routes;
    u8 current_route;
    u8 next_route;
    u8 pad16C[0x170 - 0x16C];
    struct aivec_s fallback_destination;
    f32 fallback_stopping_distance;
    f32 movement_parameter;
    struct aipathinfo_s fallback_path_info;
    u8 pad19C[4];
    u32 movement_mode : 3;
};

extern void* memset();

void AISysCharacterSetPath(struct aipacket_s *packet, struct aipath_s *path)
{
    s32 i;
    if (packet->path_info.path == path) return;
    memset(&packet->path_info, 0, sizeof(packet->path_info));
    packet->path_info.path = path;
    packet->available_routes = 0;
    packet->inside_path_node = -1;
    packet->current_route = 0xff;
    packet->next_route = 0;
    if (path == NULL) return;
    for (i = 0; i < path->route_count; i++) {
        struct aipathroute_s *route = &path->routes[i];
        if (route->character_mask & packet->character_type_mask)
            packet->available_routes |= (u16)(1LL << i);
    }
}

/* OpenSaga AISysFindRoute: pick the next route the connection allows, round robin. */
void fn_8008F660(struct aipacket_s *packet)
{
    s32 count = packet->path_info.path->route_count;
    s32 route, first;
    if (count == 0) return;
    if ((packet->path_info.connection->route_mask & packet->available_routes) == 0) return;
    first = packet->next_route;
    if (first < 0 || first >= count) first = 0;
    route = first;
    do {
        if ((u32)((u64)(packet->path_info.connection->route_mask & packet->available_routes) >> route) & 1) {
            packet->current_route = route;
            packet->next_route = route + 1;
            return;
        }
        route++;
        if (route >= packet->path_info.path->route_count) route = 0;
    } while (route != first);
}

struct object_entry3c { char bytes[0x3c]; };
struct object_entry38 { char bytes[0x38]; };
struct object_joint {
    char* name;
    u8 unknown04[0x54];
};
struct object_hierarchy {
    u8 unknown00[0x10];
    u8 count;
    u8 unknown11[0x67];
    struct object_joint* joints;
};
struct object_names {
    u8 count;
    u8 unknown01[3];
    char** names;
    struct object_hierarchy* hierarchy;
};
struct object_lookup {
    u8 unknown00[0xc];
    struct object_names* names;
    u8 unknown10[0x10];
    s32 count3c;
    struct object_entry3c* entries3c;
    s32 count38;
    struct object_entry38* entries38;
};
extern s32 NuStrICmp(char*, char*);

struct object_entry3c* AIPathFindLocator(struct object_lookup* object, char* name)
{
    s32 i;
    if (object != NULL) {
        for (i = 0; i < object->count3c; i++) {
            if (NuStrICmp(object->entries3c[i].bytes, name) == 0)
                return &object->entries3c[i];
        }
    }
    return NULL;
}

struct object_entry38* AISysFindArea(struct object_lookup* object, char* name)
{
    s32 i;
    if (object != NULL) {
        for (i = 0; i < object->count38; i++) {
            struct object_entry38* entry = &object->entries38[i];
            if (NuStrICmp(name, entry->bytes) == 0)
                return entry;
        }
    }
    return NULL;
}

char* AISysFindPath(struct object_lookup* object, char* name)
{
    s32 i;
    if (object != NULL && object->names != NULL) {
        for (i = 0; i < object->names->count; i++) {
            if (NuStrICmp(object->names->names[i], name) == 0)
                return object->names->names[i];
        }
    }
    return NULL;
}

struct object_joint* AIPathFindNode(struct object_lookup* object, struct object_hierarchy* hierarchy, char* name)
{
    s32 i;
    if (hierarchy == NULL) {
        if (object != NULL && object->names != NULL && object->names->count != 0)
            hierarchy = object->names->hierarchy;
        if (hierarchy == NULL)
            goto done;
    }
    for (i = 0; i < hierarchy->count; i++) {
        if (hierarchy->joints[i].name != NULL && NuStrICmp(hierarchy->joints[i].name, name) == 0)
            return &hierarchy->joints[i];
    }
done:
    return NULL;
}

struct object_pool_entry { u8 unknown00[0x2c]; };
struct object_pool {
    s32 count;
    struct object_pool_entry* entries;
    u8 list[8];
    u8 otherList[8];
};
extern void fn_80007610(void*, void*);
void fn_8008F968(struct object_pool* pool)
{
    s32 i;
    if (pool != NULL) {
        void* list = pool->list;
        memset(list, 0, 8);
        memset(pool->otherList, 0, 8);
        memset(pool->entries, 0, pool->count * sizeof(struct object_pool_entry));
        for (i = 0; i < pool->count; i++)
            fn_80007610(pool->list, &pool->entries[i]);
    }
}

#include "game/apiobject.h"
extern f32 fn_8000C038(void);
extern f32 NuSinApprox(s32);
extern f32 NuCosApprox(s32);
extern f32 lbl_8019DE4C[];
extern f32 lbl_8019DE50[];

void fn_8009083C(struct NUAPIOBJECT_s* a, struct NUAPIOBJECT_s* b)
{
    u16 angle = (s32)(fn_8000C038() * lbl_8019DE4C[0]);
    if (a->flags20C & 0x40000000) {
        if (!(b->flags20C & 0x40000000)) {
            f32 x = NuSinApprox(angle);
            f32 scale = lbl_8019DE50[0];
            b->velocityX = x * scale;
            b->velocityZ = NuCosApprox(angle) * scale;
            b->responseX = b->velocityX;
            b->responseZ = b->velocityZ;
            a->responseX = a->velocityX;
            a->responseZ = a->velocityZ;
            return;
        }
    } else if (b->flags20C & 0x40000000) {
        f32 x = NuSinApprox(angle);
        f32 scale = lbl_8019DE50[0];
        a->velocityX = x * scale;
        a->velocityZ = NuCosApprox(angle) * scale;
        a->responseX = a->velocityX;
        a->responseZ = a->velocityZ;
        b->responseX = b->velocityX;
        b->responseZ = b->velocityZ;
        return;
    }
    a->velocityX = NuSinApprox(angle) * 2.0f;
    a->velocityZ = NuCosApprox(angle) * 2.0f;
    a->responseX = a->velocityX;
    a->responseZ = a->velocityZ;
    b->velocityX = -a->velocityX;
    b->velocityZ = -a->velocityZ;
    b->responseX = b->velocityX;
    b->responseZ = b->velocityZ;
}

struct aimessage_s {
    u8 links[8];
    char name[0x20];
    f32 value;
};
struct aimessagesys_s {
    u8 pad0[8];
    u8 free_list[8];
    u8 active_list[8];
};
struct aimessage_s *CheckAIMessage(struct aimessagesys_s *sys, char *name, struct aimessage_s *message);

void SetAIMessage(struct aimessagesys_s *sys, char *name, f32 value, struct aimessage_s *message)
{
    message = CheckAIMessage(sys, name, message);
    if (message != NULL) {
        message->value = value;
    }
}

extern void AIFormationFollow(struct aipacket_s *packet);
extern void fn_8008CDE8(struct aigroup_s *group, void (*move)());
extern void fn_8008C13C();
extern void fn_8008C768();

void AIMoveInstruction(struct aipacket_s *packet, struct aivec_s *destination,
                       struct aipathinfo_s *path_info, s32 mode,
                       f32 stopping_distance, f32 movement_parameter) {
    struct aigroup_s *group = packet->group;

    if (group != NULL && group->is_in_formation) {
        switch (mode) {
        case 1:
            if (group->leader == packet->owner) fn_8008CDE8(group, fn_8008C768);
            AIFormationFollow(packet);
            return;
        case 4:
            if (group->leader == packet->owner) fn_8008CDE8(group, fn_8008C13C);
            AIFormationFollow(packet);
            return;
        case 5:
            mode = 1;
            break;
        }
    }
    if (destination != NULL) packet->fallback_destination = *destination;
    if (path_info != NULL) packet->fallback_path_info = *path_info;
    packet->fallback_stopping_distance = stopping_distance;
    packet->movement_parameter = movement_parameter;
    packet->movement_mode = mode;
}

extern void *fn_800076C4(void *list);
extern void *fn_800076CC(void *list, void *link);
extern s32 fn_800076E4(char *a, char *b, s32 n);
extern void fn_80007680(void *list, void *link);
extern void NuStrNCpy(char *dst, char *src, s32 n);
struct aimessage_s *CheckAIMessage(struct aimessagesys_s *sys, char *name, struct aimessage_s *message)
{
    struct aimessage_s *result = NULL;
    if (sys != NULL) {
        if (message != NULL) {
            result = message;
        } else if (name != NULL) {
            for (message = fn_800076C4(sys->active_list); message != NULL;
                 message = fn_800076CC(sys->active_list, message)) {
                if (fn_800076E4(name, message->name, 0x20) == 0) {
                    result = message;
                    break;
                }
            }
        }
        if (result == NULL) {
            result = fn_800076C4(sys->free_list);
            if (result != NULL) {
                fn_80007680(sys->free_list, result);
                fn_80007610(sys->active_list, result);
                NuStrNCpy(result->name, name, 0x20);
            }
        }
    }
    return result;
}

#include "numath/numath.h"
extern s32 lbl_80408DEC;
extern f32 lbl_80408DE8;
extern f32 NuFsqrt(f32);
extern f32 NuFdiv(f32, f32);
extern void *fn_80059C44(struct nuvec_s *pos, struct nuvec_s *delta, f32 radius, void *arg);
void *fn_8008FB18(struct nuvec_s *start, struct nuvec_s *delta, f32 radius, f32 maxdist, void *arg)
{
    f32 len;
    struct nuvec_s dir;
    struct nuvec_s moved;
    struct nuvec_s pos;
    struct nuvec_s step;
    void *hit;
    if (lbl_80408DEC == 0) goto fallback;
    len = delta->x * delta->x + delta->y * delta->y + delta->z * delta->z;
    if (len > maxdist * maxdist) return NULL;
    if (len < lbl_80408DE8 * lbl_80408DE8) goto fallback;
    len = NuFsqrt(len);
    NuVecScale(&dir, delta, NuFdiv(1.0f, len));
    moved.x = 0.0f;
    moved.y = 0.0f;
    moved.z = 0.0f;
    pos = *start;
    while (len > 0.0f) {
        if (len >= lbl_80408DE8) {
            NuVecScale(&step, &dir, lbl_80408DE8);
            len -= lbl_80408DE8;
        } else {
            NuVecScale(&step, &dir, len);
            len = 0.0f;
        }
        hit = fn_80059C44(&pos, &step, radius, arg);
        if (hit != NULL) {
            NuVecAdd(delta, &moved, &step);
            return hit;
        }
        NuVecAdd(&pos, &pos, &step);
        NuVecAdd(&moved, &moved, &step);
    }
    return NULL;
fallback:
    return fn_80059C44(start, delta, radius, arg);
}

extern void (*fn_80009948(char *, int))(char *, ...);
extern char lbl_8019DE8C[];
extern char lbl_8019DECC[];
extern void fn_80095234(void *entry);
struct apientry_s { u8 pad0[8]; void *a; void *b; void *c; u8 pad14[0x54 - 0x14]; };
struct apisys_s {
    s32 count0; s32 entry_count; s32 count8; u8 padC[8]; s32 count14;
    struct apientry_s *entries; s16 *array1c; s32 field20; void *array24;
    u8 pad28[0x34 - 0x28]; s32 field34;
};
extern struct apisys_s *lbl_80408DF0;
void fn_80091FC0(u32 *buf, s32 unused, s32 count0, s32 entry_count, s32 count8, s32 count14,
                 s32 field20, s32 field34)
{
    s32 i;
    if (lbl_80408DF0 != NULL) {
        fn_80009948(lbl_8019DE8C, 0x7F6)(lbl_8019DECC);
    }
    {
        struct apisys_s *p = (struct apisys_s *)((*buf + 0xF) & ~0xF);
        lbl_80408DF0 = p;
        *buf = (u32)p + 0x38;
        memset(p, 0, 0x38);
    }
    {
        struct apisys_s *sys = lbl_80408DF0;
        sys->count0 = count0;
        sys->count8 = count8;
        sys->count14 = count14;
        sys->entry_count = entry_count;
        if (entry_count != 0) {
            sys->entries = (struct apientry_s *)((*buf + 3) & ~3);
            *buf = (*buf + 3) & ~3;
            *buf += sys->entry_count * 0x54;
            memset(sys->entries, 0, sys->entry_count * 0x54);
            for (i = 0; i < lbl_80408DF0->entry_count; i++) {
                struct apisys_s *s2 = lbl_80408DF0;
                struct apientry_s *entry = &s2->entries[i];
                if (s2->count8 != 0) {
                    entry->a = (void *)((*buf + 3) & ~3);
                    *buf = (*buf + 3) & ~3;
                    *buf += s2->count8 * 4;
                    entry->b = (void *)*buf;
                    *buf = (*buf + 3) & ~3;
                    *buf += s2->count8 * 4;
                    entry->c = (void *)*buf;
                    *buf = (*buf + 3) & ~3;
                    *buf += s2->count8 * 4;
                }
                fn_80095234(entry);
            }
        }
    }
    {
        struct apisys_s *sys = lbl_80408DF0;
        if (sys->count0 != 0) {
            sys->array1c = (s16 *)((*buf + 3) & ~3);
            *buf = (*buf + 3) & ~3;
            *buf += sys->count0 * 2;
            memset(sys->array1c, 0, sys->count0 * 2);
        }
    }
    {
        struct apisys_s *sys = lbl_80408DF0;
        if (sys->count14 != 0) {
            sys->array24 = (void *)((*buf + 3) & ~3);
            *buf = (*buf + 3) & ~3;
            *buf += sys->count14 * 0x34;
            memset(sys->array24, 0, sys->count14 * 0x34);
        }
    }
    lbl_80408DF0->field20 = field20;
    lbl_80408DF0->field34 = field34;
}

struct variptr_s { u32 addr; };
struct charanim_s {
    char *name;
    s16 id;
    u8 pad6[2];
    u16 flags;
    u8 padA[0x40 - 0xA];
};
struct chardata_s {
    u8 pad0[4];
    s32 model_flags;
    char *dir;
    char *file;
    struct charanim_s *animations;
    u8 pad14[0x48 - 0x14];
};
struct charmodel_s {
    s16 model_id;
    u8 flag2;
    u8 flag3;
    void *hierarchy;
    struct charanim_s **anim;
    void **data_b;
    void **data_c;
    void *poi[16];
};
struct animlist_s {
    char path[0x30];
    void *anim;
};
struct apicharsys_s {
    s32 character_count;
    s32 model_capacity;
    s32 anim_id_capacity;
    s32 permanent_model_count;
    s32 loaded_model_count;
    s32 animation_capacity;
    struct charmodel_s *models;
    s16 *playermodelids;
    struct chardata_s *char_data;
    struct animlist_s *animations;
    s32 area_animation_count;
    s32 loaded_animation_count;
    s32 animation_load_attempts;
};
struct charmodellist_s { u8 model_id; u8 count; };
#define apicharsys ((struct apicharsys_s *)lbl_80408DF0)
extern s32 lbl_80408DF4;
extern void (*fn_80009970(char *, int))(char *, ...);
extern char lbl_8019DEFC[];
extern char lbl_8019DF04[];
extern char lbl_8019DF08[];
extern char lbl_8019DF20[];
extern char lbl_8019DF30[];
extern char lbl_8019DF38[];
extern char lbl_8019DF40[];
extern char lbl_8019DF48[];
extern char lbl_8019DF50[];
extern char lbl_8019DF58[];
extern char lbl_8019DF60[];
extern s32 NuStrCpy(char *, char *);
extern void NuStrCat(char *, char *);
extern s32 NuFileExists(char *);
extern void *NuGHGReadEx(char *, struct variptr_s *, struct variptr_s);
extern void *NuHGobjGetPOI(void *, u8);
extern void *fn_800092F0(char *, struct variptr_s *, struct variptr_s, s32);
extern s32 fn_80009324(void *, char *);
extern s32 fn_80009408(void *, s32, s32 **, s32 *);
extern void fn_800094C4(void *, s32, s32);
extern s32 NuLightStoreCurrentLights(void *);
extern void *NuAnimDataLoadBuff(s32, s32 **);
extern void *NuAnimData2LoadBuff(char *, struct variptr_s *, struct variptr_s *);

static inline void APIResetCharacterRemap(void)
{
    s32 i;
    for (i = 0; i < apicharsys->character_count; i++) {
        if ((apicharsys->char_data[i].model_flags & 2) == 0) {
            apicharsys->playermodelids[i] = -1;
        }
    }
}

static inline void APICharacterModelReset(struct charmodel_s *model)
{
    model->model_id = 0;
    model->flag2 = 0;
    model->flag3 = 0;
    model->hierarchy = NULL;
    if (apicharsys->anim_id_capacity != 0) {
        memset(model->anim, 0, apicharsys->anim_id_capacity * 4);
        memset(model->data_b, 0, apicharsys->anim_id_capacity * 4);
        memset(model->data_c, 0, apicharsys->anim_id_capacity * 4);
    }
    memset(model->poi, 0, sizeof(model->poi));
}

static inline void *LoadAnim(char *path, s32 area_animation, struct variptr_s *buf, struct variptr_s buf_end)
{
    struct animlist_s *entry = apicharsys->animations;
    s32 i;
    for (i = 0; i < apicharsys->loaded_animation_count; i++, entry++) {
        if (NuStrICmp(path, entry->path) == 0) {
            return entry->anim;
        }
    }
    if (i < apicharsys->animation_capacity) {
        NuStrCpy(entry->path, path);
        entry->anim = NuAnimData2LoadBuff(entry->path, buf, &buf_end);
        if (entry->anim != NULL) {
            if (area_animation != 0) {
                apicharsys->area_animation_count++;
            }
            {
                struct apicharsys_s *sys = apicharsys;
                sys->loaded_animation_count++;
                sys->animation_load_attempts++;
            }
        }
        return entry->anim;
    }
    apicharsys->animation_load_attempts++;
    return NULL;
}

void APILoadCharacterModels(struct charmodellist_s *list, s32 area_animation, struct variptr_s *buf,
                            struct variptr_s buf_end, s32 area_models)
{
    char directory[0x40];
    char path[0x100];
    char pak_path[0x100];
    struct charmodel_s *model;
    struct charanim_s *anim;
    struct chardata_s *character;
    s32 i;
    s32 j;
    u8 model_id;
    s32 loaded;
    void *pak;

    if (lbl_80408DF4 == 0) {
        for (i = 0; i < apicharsys->character_count; i++) {
            if ((apicharsys->char_data[i].model_flags & 2) == 0) {
                apicharsys->playermodelids[i] = -1;
            }
        }
        model = apicharsys->models;
        for (i = 0; i < apicharsys->permanent_model_count; i++, model++) {
            for (j = 0; j < apicharsys->anim_id_capacity; j++) {
                if ((model->data_b[j] != NULL || model->data_c[j] != NULL) && (model->anim[j]->flags & 1) == 0) {
                    model->anim[j] = NULL;
                    model->data_b[j] = NULL;
                    model->data_c[j] = NULL;
                }
            }
        }
        for (i = apicharsys->permanent_model_count; i < apicharsys->model_capacity; i++, model++) {
            model->hierarchy = NULL;
        }
        apicharsys->loaded_model_count = apicharsys->permanent_model_count;
    }
    lbl_80408DF4 = 0;
    if (list == NULL) {
        return;
    }
    while ((model_id = list->model_id) != 0xFF && apicharsys->loaded_model_count < apicharsys->model_capacity) {
        character = &apicharsys->char_data[model_id];
        loaded = 0;
        NuStrCpy(directory, lbl_8019DEFC);
        NuStrCat(directory, character->dir);
        NuStrCat(directory, lbl_8019DF04);
        if (apicharsys->playermodelids[model_id] != -1) {
            fn_80009970(lbl_8019DE8C, 0x8A7)(lbl_8019DF08);
            model = &apicharsys->models[apicharsys->playermodelids[model_id]];
        } else {
            fn_80009970(lbl_8019DE8C, 0x8AD)(lbl_8019DF20);
            model = &apicharsys->models[apicharsys->loaded_model_count];
            APICharacterModelReset(model);
            NuStrCpy(path, directory);
            NuStrCat(path, character->file);
            NuStrCpy(pak_path, path);
            NuStrCat(pak_path, lbl_8019DF30);
            NuStrCat(path, lbl_8019DF38);
            model->hierarchy = NuGHGReadEx(path, buf, buf_end);
            if (model->hierarchy == NULL) {
                list++;
                continue;
            }
            for (i = 0; i < 16; i++) {
                model->poi[i] = NuHGobjGetPOI(model->hierarchy, i);
            }
            loaded = 1;
        }
        if (area_models != 0 && list->count != 0) {
            anim = apicharsys->char_data[model_id].animations;
        } else {
            anim = NULL;
        }
        pak = fn_800092F0(pak_path, buf, buf_end, 0x10);
        if (pak != NULL) {
            for (; anim != NULL && anim->name != NULL; anim++) {
                if ((anim->flags & 0x8000) == 0) {
                    s32 item;
                    NuStrCpy(path, lbl_8019DF04);
                    NuStrCat(path, anim->name);
                    NuStrCat(path, lbl_8019DF40);
                    fn_80009324(pak, path);
                    NuStrCpy(path, lbl_8019DF04);
                    NuStrCat(path, anim->name);
                    NuStrCat(path, lbl_8019DF48);
                    item = fn_80009324(pak, path);
                    if (item != 0) {
                        fn_800094C4(pak, item, 1);
                    }
                }
            }
            buf->addr -= NuLightStoreCurrentLights(pak);
            if (area_models != 0 && list->count != 0) {
                anim = apicharsys->char_data[model_id].animations;
            } else {
                anim = NULL;
            }
            for (; anim != NULL && anim->name != NULL; anim++) {
                if ((anim->flags & 0x8000) == 0) {
                    s32 item;
                    s32 *data;
                    s32 size;
                    NuStrCpy(path, lbl_8019DF04);
                    NuStrCat(path, anim->name);
                    NuStrCat(path, lbl_8019DF40);
                    fn_80009324(pak, path);
                    NuStrCpy(path, lbl_8019DF04);
                    NuStrCat(path, anim->name);
                    NuStrCat(path, lbl_8019DF48);
                    item = fn_80009324(pak, path);
                    if (item != 0 && fn_80009408(pak, item, &data, &size) != 0) {
                        if (*data == 2) {
                            if ((model->data_b[anim->id] = NuAnimDataLoadBuff(size, &data)) != NULL) {
                                model->anim[anim->id] = anim;
                            }
                        }
                        if (*data >= 0) {
                            void *p = NuAnimDataLoadBuff(size, &data);
                            *data |= 0x80000000;
                            if ((model->data_b[anim->id] = p) != NULL) {
                                model->anim[anim->id] = anim;
                            }
                        }
                    }
                }
            }
            if (area_models != 0 && list->count != 0) {
                anim = apicharsys->char_data[model_id].animations;
            } else {
                anim = NULL;
            }
            for (; anim != NULL && anim->name != NULL; anim++) {
                if ((anim->flags & 0x8000) == 0) {
                    s32 item;
                    s32 *data;
                    s32 size;
                    NuStrCpy(path, lbl_8019DF04);
                    NuStrCat(path, anim->name);
                    NuStrCat(path, lbl_8019DF40);
                    item = fn_80009324(pak, path);
                    if (item != 0 && fn_80009408(pak, item, &data, &size) != 0) {
                        *data &= 0x7FFFFFFF;
                    }
                }
            }
        } else {
            for (; anim != NULL && anim->name != NULL; anim++) {
                if ((anim->flags & 0x8000) == 0
                    && ((area_animation != 0 && (anim->flags & 1) != 0)
                        || (area_animation == 0 && (anim->flags & 1) == 0))) {
                    if ((anim->flags & 4) != 0 && model->data_b[anim->id] == NULL) {
                        NuStrCpy(path, directory);
                        NuStrCat(path, anim->name);
                        NuStrCat(path, lbl_8019DF50);
                        if (NuFileExists(path) != 0) {
                            model->data_b[anim->id] = LoadAnim(path, area_animation, buf, buf_end);
                        }
                        if (model->data_b[anim->id] == NULL) {
                            NuStrCpy(path, directory);
                            NuStrCat(path, anim->name);
                            NuStrCat(path, lbl_8019DF58);
                            model->data_b[anim->id] = LoadAnim(path, area_animation, buf, buf_end);
                        }
                        if (model->data_b[anim->id] != NULL) {
                            model->anim[anim->id] = anim;
                        }
                    }
                    if ((anim->flags & 8) != 0 && model->data_c[anim->id] == NULL) {
                        NuStrCpy(path, directory);
                        NuStrCat(path, anim->name);
                        NuStrCat(path, lbl_8019DF60);
                        model->data_c[anim->id] = LoadAnim(path, area_animation, buf, buf_end);
                        if (model->data_c[anim->id] != NULL) {
                            model->anim[anim->id] = anim;
                        }
                    }
                }
            }
        }
        if (loaded) {
            model->model_id = model_id;
            apicharsys->playermodelids[model_id] = apicharsys->loaded_model_count;
            if (area_animation != 0) {
                apicharsys->permanent_model_count++;
                character->model_flags |= 2;
            }
            apicharsys->loaded_model_count++;
        }
        list++;
    }
}
