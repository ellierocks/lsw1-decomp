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
