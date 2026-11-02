#include "game/pickups.h"

extern f32 lbl_801AEC5C[];
extern f32 lbl_801AEC60[];
extern f32 lbl_801AEC64[];
extern s32 lbl_804093C8;
extern f32 lbl_80409A08;

void fn_8013C620(struct NUPICKUPPOOL_s* pool)
{
    s32 i = 0, j;
    f32* value = lbl_801AEC5C;
    if (pool->state710 == 0) {
        for (; i < 6; i++) {
            f32 v = *value;
            for (j = 0; j < pool->counts[i]; j++) {
                pool->groups[i][j].active39 = 1;
                pool->groups[i][j].flags38 |= 0x1c;
                pool->groups[i][j].value2C = v;
            }
        }
        if (lbl_804093C8 == 3)
            lbl_80409A08 = lbl_801AEC60[0];
        else
            lbl_80409A08 = lbl_801AEC64[0];
    }
}

extern f32 lbl_801AEDE8[];
extern f32 lbl_801AEDEC[];

void fn_8013F1B0(struct NUPICKUPVISUAL_s* visual)
{
    f32 value = lbl_801AEDE8[0];
    visual->value100 = value;
    if (visual->kind122 != 0xe8)
        visual->value100 = value * lbl_801AEDEC[0];
}

extern u32 lbl_803E04B4[];
extern f32 lbl_801AEDF0[];
extern f32 lbl_801AEDF4[];
extern f32 lbl_801AEDF8[];

s32 fn_8013F1DC(struct NUPICKUPVISUAL_s* visual)
{
    f32 value = visual->value100;
    if (value > lbl_801AEDF0[0] && lbl_803E04B4[0] % 6 > 2) {
        f32 limit = lbl_801AEDF4[0];
        if (visual->kind122 != 0xe8)
            limit *= lbl_801AEDF8[0];
        if (value < limit)
            return 0;
    }
    return 1;
}
extern f32 lbl_801AEDD0[];
extern f32 lbl_801AEDD4[];
extern f32 lbl_80408E9C;

struct PickupFadeView { char unk0[0x144]; f32 fade144; };
struct PickupFadeOwner { char unk0[0xeec]; struct PickupFadeView* view; };

void fn_8013EFD8(struct PickupFadeOwner* owner)
{
    struct PickupFadeView* view = owner->view;
    f32 target = lbl_801AEDD0[0];
    f32 v = view->fade144;
    if (v > target) {
        view->fade144 = v - lbl_80408E9C * lbl_801AEDD4[0];
        if (owner->view->fade144 < target)
            owner->view->fade144 = target;
    } else if (v < target) {
        view->fade144 = lbl_80408E9C * lbl_801AEDD4[0] + v;
        if (owner->view->fade144 > target)
            owner->view->fade144 = target;
    }
}

extern f32 lbl_801AEDD8[];
extern f32 lbl_801AEDDC[];
extern s32 lbl_80409488;
extern char str_Format_Hash[];
void fn_800D5680(char* fmt, s32 value, s32 a, s32 r, s32 g, s32 b, s32 alpha, f32 x, f32 y, f32 z);

void fn_8013F050(s32 value)
{
    lbl_80409488 = 4;
    fn_800D5680(str_Format_Hash, value, 0, 0xff, 0xff, 0xff, 0x83, lbl_801AEDD8[0], lbl_801AEDDC[0], lbl_801AEDDC[0]);
}

struct PickupMtx { f32 m[16]; };
struct PickupVec { f32 x, y, z; };
struct PickupGlobalView { char unk0[0x19C]; s32 mode19C; };
extern f32 lbl_801AEC68[];
extern f32 lbl_801AEC6C[];
extern struct PickupGlobalView* lbl_80409410;
extern f32 lbl_80409A10;
extern f32 lbl_80409A1C;
extern f32 lbl_80409A20;
extern f32 lbl_80409A24;
void NuMtxPreScale(struct PickupMtx* mtx, struct PickupVec* scale);

s32 fn_8013C6EC(struct NUPICKUPVISUAL_s* visual)
{
    struct PickupMtx mtx;
    struct PickupVec scale;

    if (lbl_80409410->mode19C == 3)
        scale.x = lbl_80409A1C;
    else if (lbl_80409410->mode19C == 10)
        scale.x = lbl_80409A20;
    else
        scale.x = lbl_80409A24;
    scale.x *= lbl_801AEC68[0] - visual->value100 / lbl_80409A10;
    if (scale.x > lbl_801AEC6C[0]) {
        scale.z = scale.x;
        scale.y = scale.x;
        mtx = *(struct PickupMtx*)visual;
        NuMtxPreScale(&mtx, &scale);
    }
    return 0;
}

extern f32 lbl_801AEDFC[];
extern f32 lbl_801AEE00[];
extern f32 lbl_801AEE04[];
extern f32 lbl_801AEE08[];
extern f32 lbl_80409A28;
extern char str_Pickup_LegoSingle[];
f32 fn_8000CB50(void* pos);
void NuASin(s32 value);
void fn_8006DD1C(char* name, void* pos);

void fn_8013F24C(struct NUPICKUPVISUAL_s* visual)
{
    f32 minDist;
    f32 dist;

    if (lbl_80409410->mode19C != 10) {
        minDist = lbl_801AEDFC[0];
        if (lbl_80409A28 <= minDist) {
            dist = fn_8000CB50(visual->unk0 + 0x80);
            if (dist > minDist) {
                if (dist > lbl_801AEE00[0])
                    dist = lbl_801AEE00[0];
                NuASin((s32)((lbl_801AEE00[0] - dist) * lbl_801AEE04[0]));
                fn_8006DD1C(str_Pickup_LegoSingle, visual->unk0 + 0x30);
                lbl_80409A28 = lbl_801AEE08[0];
            }
        }
    }
}

extern void* memset();

void fn_8013D438(struct NUPICKUPPOOL_s* pool)
{
    s32 i = 0;
    s32 j;
    struct NUPICKUP_s* p;

    memset(pool->counts, 0, sizeof(pool->counts));
    *(s32*)((char*)pool + 0x74c) = i;
    for (; i < 0x100; i++) {
        s8 kind = *(s8*)((char*)(*(struct NUPICKUP_s**)pool->unk714 + i) + 0x3e);
        if (kind != -1) {
            pool->counts[kind]++;
            (*(s32*)((char*)pool + 0x74c))++;
        }
    }
    *(s32*)((char*)pool + 0x728) = 0;
    pool->groups[0] = *(struct NUPICKUP_s**)pool->unk714;
    for (i = 0; i <= 5; i++) {
        p = *(struct NUPICKUP_s**)pool->unk714;
        j = 0;
        while (*(s8*)((char*)p + 0x3e) != i && j < 0x100) {
            p++;
            j++;
        }
        if (j == 0x100 || p == NULL)
            pool->groups[i] = NULL;
        else
            pool->groups[i] = p;
    }
}
