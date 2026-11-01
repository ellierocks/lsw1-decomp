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

struct object_entry3c* fn_8008F720(struct object_lookup* object, char* name)
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

struct object_entry38* fn_8008F7A0(struct object_lookup* object, char* name)
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

char* fn_8008F820(struct object_lookup* object, char* name)
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

struct object_joint* fn_8008F8B4(struct object_lookup* object, struct object_hierarchy* hierarchy, char* name)
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
extern void* memset();
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
extern f32 NuAngDiff(s32);
extern f32 NuASin(s32);
extern f32 lbl_8019DE4C[];
extern f32 lbl_8019DE50[];

void fn_8009083C(struct NUAPIOBJECT_s* a, struct NUAPIOBJECT_s* b)
{
    u16 angle = (s32)(fn_8000C038() * lbl_8019DE4C[0]);
    if (a->flags20C & 0x40000000) {
        if (!(b->flags20C & 0x40000000)) {
            f32 x = NuAngDiff(angle);
            f32 scale = lbl_8019DE50[0];
            b->velocityX = x * scale;
            b->velocityZ = NuASin(angle) * scale;
            b->responseX = b->velocityX;
            b->responseZ = b->velocityZ;
            a->responseX = a->velocityX;
            a->responseZ = a->velocityZ;
            return;
        }
    } else if (b->flags20C & 0x40000000) {
        f32 x = NuAngDiff(angle);
        f32 scale = lbl_8019DE50[0];
        a->velocityX = x * scale;
        a->velocityZ = NuASin(angle) * scale;
        a->responseX = a->velocityX;
        a->responseZ = a->velocityZ;
        b->responseX = b->velocityX;
        b->responseZ = b->velocityZ;
        return;
    }
    a->velocityX = NuAngDiff(angle) * 2.0f;
    a->velocityZ = NuASin(angle) * 2.0f;
    a->responseX = a->velocityX;
    a->responseZ = a->velocityZ;
    b->velocityX = -a->velocityX;
    b->velocityZ = -a->velocityZ;
    b->responseX = b->velocityX;
    b->responseZ = b->velocityZ;
}
