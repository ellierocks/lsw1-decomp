#include "game/creature.h"

extern s32 lbl_8040B058;
extern struct NUGAMEOBJECT_s* lbl_80409430;
extern s32 lbl_8040912C;
extern s32 fn_800A8DAC(void*, void*, void*, void*, void*, void*, struct NUGAMEOBJECT_s*, u32);
extern void fn_800AA4A8(struct NUGAMEOBJECT_s*);

void fn_800AA92C(void)
{
    s32 i = 0;
    struct NUGAMEOBJECT_s* object = lbl_80409430;

    for (; i < lbl_8040B058; i++, object++) {
        if (!(object->motionFlags & 0x800)) {
            object->drawResult = fn_800A8DAC(object->characterModel,
                (char*)object + 8, (char*)object + 0xC8,
                (object->flagsF28 & 0x200) ? (char*)object + 0x108 : NULL,
                object->drawMatrixEnabled ? (char*)object + 0x148 : NULL,
                (char*)object->playerPacket + 0x194, object, object->drawFlags);
            if (object->drawResult) {
                fn_800AA4A8(object);
            }
            object->drawUpdated = 1;
            if (object->drawResult) {
                object->flagsF28 |= 0x100;
            }
            if (lbl_8040912C == 0 && object->characterData->drawFn != NULL) {
                object->characterData->drawFn(object);
            }
        }
    }
}


void fn_800B9D44(void *);
void fn_800B9DC8(void *);
extern char lbl_803E0298[];
extern s32 lbl_8040AFC8;
extern s32 lbl_8040AFCC;

void fn_800A20AC(void)
{
    lbl_8040AFC8 = (s32) (s8) lbl_803E0298[0xA];
    lbl_8040AFCC = (s32) (s8) lbl_803E0298[0xB];
    fn_800B9DC8(lbl_803E0298 + 4);
    fn_800B9D44(lbl_803E0298 + 4);
}

extern u8 NuTimeBar_DefaultColours[];
extern u8 lbl_801D53C8[];
extern s32 lbl_8040B044;

static inline s32 creatureHasCharacter(s32 id)
{
    u8 *p = NuTimeBar_DefaultColours;
    while (*p != 255) {
        if (*p == id) {
            return 1;
        }
        p++;
    }
    return 0;
}

extern s32 lbl_80409074[];
extern void *lbl_80409410;
extern s32 lbl_8040AFB8;
void fn_800AB428(s32 force, s32 a, s32 b)
{
    s32 pair[2];
    s32 i;
    if (lbl_8040AFB8 == 0 && (force || ((*(u32 *)(*(char **)((char *)lbl_80409410 + 0x1A8) + 4) & 0x5002) == 2))) {
        pair[0] = a;
        pair[1] = b;
        for (i = 0; i < 2; i++) {
            if (pair[i] != -1 && pair[i] != i[lbl_80409074] && creatureHasCharacter(pair[i])) {
                if (pair[i] != lbl_80409074[(i + 1) & 1]) {
                    i[lbl_80409074] = pair[i];
                }
            }
        }
        if (a != b && a == lbl_80409074[1] && b == lbl_80409074[0]) {
            lbl_80409074[0] = a;
            lbl_80409074[1] = b;
        }
    }
}

/* Local views preserve base-plus-member addressing; array extents are unknown. */
struct NUCHARACTERBITS_s {
    char pad0[0x1D8];
    u32 bits[0];
};
static inline s32 creatureCharacterIndex(s32 id)
{
    u8 *p = lbl_801D53C8;
    s32 i = 0;
    while (*p != 255) {
        if (*p == id) {
            return i;
        }
        p += 12;
        i++;
    }
    return -1;
}

s32 fn_800AAE44(s32 id)
{
    if (creatureHasCharacter(id)) {
        return 0;
    }
    if (creatureCharacterIndex(id) == -1 || lbl_8040B044 == 63) {
        return 0;
    }
    {
        NuTimeBar_DefaultColours[lbl_8040B044] = id;
        NuTimeBar_DefaultColours[lbl_8040B044 + 1] = 255;
        lbl_8040B044++;
        ((struct NUCHARACTERBITS_s *)lbl_803E0298)->bits[id / 32] |= 1 << (id & 31);
        return 1;
    }
    return 0;
}

struct NULEVELAVAILABILITY_s {
    char pad0[0x111];
    struct {
        u8 available;
        char pad1[7];
    } levels[0];
};
s32 fn_800AC340(void);
s32 fn_800CC764(s32);
struct NUCHARACTERAVAILABILITY_s {
    char pad0[0x1EC];
    u32 bits[0];
};
void *fn_800AAD10(s32 id)
{
    s32 index;
    s32 allowed;
    char *entry;
    index = fn_800AC340();
    allowed = 0;
    if (index != -1) {
        if (!((((struct NUCHARACTERAVAILABILITY_s *)lbl_803E0298)->bits[id / 32] >> (id & 31)) & 1)) {
            entry = (char *)lbl_801D53C8 + index * 12;
            switch ((u8)entry[1]) {
            case 1:
                break;
            case 2:
                if (*(s16 *)(entry + 2) == -1 || (u8)((struct NULEVELAVAILABILITY_s *)lbl_803E0298)->levels[*(s16 *)(entry + 2)].available == 0) {
                    break;
                }
            case 0:
allow:
                allowed = 1;
                break;
            case 3:
                if (fn_800CC764(0) && fn_800CC764(1) && fn_800CC764(2)) {
                    allowed = 1;
                } else {
                    allowed = 0;
                }
                break;
            }
        } else {
            goto allow;
        }
    }
    return allowed ? (char *)lbl_801D53C8 + index * 12 : NULL;
}

s32 APILoadCharacterModels(s32, s32, s32 *, s32 *, s32);
s32 (*fn_80009970(s32 *, s32))();
extern s32 lbl_8019FA30[];
extern s32 lbl_8019FA48[];
extern s32 lbl_8019FA64[];
extern s32 lbl_8019FA70[];
extern s32 lbl_80408F6C;
extern s32 lbl_80408F70;
extern s32 lbl_80408F7C;
extern s32 lbl_80408F80;

void GameLoadCharacterModels(s32 arg0, s32 arg1)
{
    s32 sp8;
    s32 (*temp_r0)();
    s32 *var_r4;

    temp_r0 = fn_80009970(lbl_8019FA30, 0x1A6B);
    if (arg1 != 0) {
        var_r4 = lbl_8019FA64;
    } else {
        var_r4 = lbl_8019FA70;
    }
    temp_r0(lbl_8019FA48, var_r4);
    if (arg1 != 0) {
        sp8 = lbl_80408F70;
        APILoadCharacterModels(arg0, arg1, &lbl_80408F6C, &sp8, 1);
        return;
    }
    sp8 = lbl_80408F80;
    APILoadCharacterModels(arg0, 0, &lbl_80408F7C, &sp8, 1);
}
