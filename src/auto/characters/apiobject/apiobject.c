#include "types.h"
#include "game/apiobject.h"
#include "numath/numath.h"

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

struct aiscriptentry_s {
    s32 id;
    char name[0x10];
};
struct aiscriptlist_s {
    s32 count;
    s32 max;
    struct aiscriptentry_s *entries;
};
extern void NuStrCpy(char *dst, char *src);
extern s32 fn_800633A4(char *name, s32 mode);
extern s32 lbl_804083F4;
extern char **lbl_804083FC;

struct aiscriptlist_s *fn_800911D0(s32 *buffer, s32 unused, s32 max, s32 count, char **names, s32 mode)
{
    struct aiscriptlist_s *list;
    s32 i, j;

    list = (struct aiscriptlist_s *)((*buffer + 0xf) & ~0xf);
    *buffer = (s32)(list + 1);
    if (list != NULL) {
    memset(list, 0, sizeof(*list));
    list->count = count;
    list->max = max;
    list->entries = (struct aiscriptentry_s *)((*buffer + 0xf) & ~0xf);
    *buffer = ((*buffer + 0xf) & ~0xf) + max * sizeof(struct aiscriptentry_s);
    if (list->entries != NULL) {
    memset(list->entries, -1, max * sizeof(struct aiscriptentry_s));
    for (i = 0; i < list->count; i++) {
        NuStrCpy(list->entries[i].name, names[i]);
        list->entries[i].id = -1;
        list->entries[i].id = fn_800633A4(list->entries[i].name, (s8)mode);
    }
    for (j = 1; i < list->max && j < lbl_804083F4; j++) {
        list->entries[i].id = -1;
        if (lbl_804083FC[j] != NULL) {
            NuStrCpy(list->entries[i].name, lbl_804083FC[j]);
            list->entries[i].id = fn_800633A4(list->entries[i].name, (s8)mode);
            i++;
        }
    }
    for (; i < list->max; i++) list->entries[i].id = -1;
    return list;
    }
    }
    return NULL;
}

extern f32 NuFdiv(f32 a, f32 b);
extern f32 lbl_80408DDC;
extern f32 lbl_80408DE0;

s32 fn_8009098C(struct NUAPIOBJECT_s *a, struct NUAPIOBJECT_s *b)
{
    struct nuvec_s *pa = (struct nuvec_s *)&a->positionX;
    struct nuvec_s *pb = (struct nuvec_s *)&b->positionX;
    struct nuvec_s dir;
    struct nuvec_s va;
    struct nuvec_s vb;
    struct nuvec_s avg;
    s32 touching;
    f32 f, ra, rb;
    f32 speedA, speedB, d, k, len;

    if ((a->contactMask & b->identityMask) || (b->contactMask & a->identityMask)) touching = 1;
    else touching = 0;
    a->contactMask |= b->identityMask;
    b->contactMask |= a->identityMask;
    f = a->mass + b->mass;
    if (f > 0.0f) {
        ra = 2.0f * (a->mass / f);
        rb = 2.0f - ra;
    } else {
        ra = rb = 1.0f;
    }
    if (!touching) {
        if (a->flags208 & 0x80) return 2;
        if (b->flags208 & 0x80) return 2;
        if (pa->x == pb->x && pa->z == pb->z && a->unk84 == 0.0f && a->unk8C == 0.0f
            && b->unk84 == 0.0f && b->unk8C == 0.0f) {
            fn_8009083C(a, b);
            return 2;
        }
        dir.x = pb->x - pa->x;
        dir.z = pb->z - pa->z;
        len = NuFsqrt(dir.x * dir.x + dir.z * dir.z);
        if (len == 0.0f) f = 0.0f;
        else f = 1.0f / len;
        dir.x = dir.x * f;
        dir.z = dir.z * f;
        speedA = NuFsqrt(a->unk84 * a->unk84 + a->unk8C * a->unk8C);
        if (speedA == 0.0f) f = 0.0f;
        else f = 1.0f / speedA;
        va.x = a->unk84 * f;
        va.z = a->unk8C * f;
        speedB = NuFsqrt(b->unk84 * b->unk84 + b->unk8C * b->unk8C);
        if (speedB == 0.0f) f = 0.0f;
        else f = 1.0f / speedB;
        vb.x = b->unk84 * f;
        vb.z = b->unk8C * f;
        f = (va.x * dir.x + va.z * dir.z) * speedA;
        va.x = dir.x * f;
        va.z = dir.z * f;
        f = (vb.x * dir.x + vb.z * dir.z) * speedB;
        vb.x = dir.x * f;
        vb.z = dir.z * f;
        if ((a->flags20C & 0x40000000) && !(b->flags20C & 0x40000000)) {
            b->velocityX += (va.x - vb.x) * ra * 2.0f;
            b->velocityZ += (va.z - vb.z) * ra * 2.0f;
        } else if (!(a->flags20C & 0x40000000) && (b->flags20C & 0x40000000)) {
            a->velocityX += (vb.x - va.x) * rb * 2.0f;
            a->velocityZ += (vb.z - va.z) * rb * 2.0f;
        } else {
            a->velocityX += (vb.x - va.x) * rb;
            a->velocityZ += (vb.z - va.z) * rb;
            b->velocityX += (va.x - vb.x) * ra;
            b->velocityZ += (va.z - vb.z) * ra;
        }
        a->responseX = a->velocityX;
        a->responseZ = a->velocityZ;
        b->responseX = b->velocityX;
        b->responseZ = b->velocityZ;
        return 2;
    }
    if (!(a->flags208 & 0x80) && !(b->flags208 & 0x80)) {
        if (pa->x == pb->x && pa->z == pb->z && a->unk84 == 0.0f && a->unk8C == 0.0f
            && b->unk84 == 0.0f && b->unk8C == 0.0f) {
            fn_8009083C(a, b);
        } else {
            avg.x = (a->unk84 + b->unk84) * 0.5f;
            avg.z = (a->unk8C + b->unk8C) * 0.5f;
            if (pa->x != pb->x || pa->z != pb->z) {
                dir.x = pb->x - pa->x;
                dir.z = pb->z - pa->z;
                f = NuFdiv(1.0f, NuFsqrt(dir.x * dir.x + dir.z * dir.z));
                dir.x = dir.x * f;
                dir.z = dir.z * f;
                f = NuFsqrt(avg.x * avg.x + avg.z * avg.z);
                if (f < lbl_80408DDC) f = lbl_80408DDC;
                else if (f > lbl_80408DE0) f = lbl_80408DE0;
                k = 3.0f;
                f = f * (k / 2.1f);
                dir.x = dir.x * f;
                dir.z = dir.z * f;
                if ((a->flags20C & 0x40000000) && !(b->flags20C & 0x40000000)) {
                    b->velocityX = avg.x + (dir.x + dir.x);
                    b->velocityZ = avg.z + (dir.z + dir.z);
                } else if (!(a->flags20C & 0x40000000) && (b->flags20C & 0x40000000)) {
                    a->velocityX = avg.x - (dir.x + dir.x);
                    a->velocityZ = avg.z - (dir.z + dir.z);
                } else {
                    a->velocityX = avg.x - dir.x;
                    a->velocityZ = avg.z - dir.z;
                    b->velocityX = avg.x + dir.x;
                    b->velocityZ = avg.z + dir.z;
                }
                a->responseX = a->velocityX;
                a->responseZ = a->velocityZ;
                b->responseX = b->velocityX;
                b->responseZ = b->velocityZ;
            }
        }
    }
    return 1;
}
