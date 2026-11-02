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
        s8 kind = *(s8*)((char*)((struct NUPICKUP_s*)pool->buffers[0] + i) + 0x3e);
        if (kind != -1) {
            pool->counts[kind]++;
            (*(s32*)((char*)pool + 0x74c))++;
        }
    }
    *(s32*)((char*)pool + 0x728) = 0;
    pool->groups[0] = (struct NUPICKUP_s*)pool->buffers[0];
    for (i = 0; i <= 5; i++) {
        p = (struct NUPICKUP_s*)pool->buffers[0];
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

extern void fn_8013D7E0(struct NUPICKUP_s *, s32);
struct PickupHeight { f32 height; char unk4[0x30]; };
extern struct PickupHeight lbl_801E1614[];
void fn_8013D9FC(struct NUPICKUPPOOL_s *pool, s32 update)
{
    s32 i = 0;
    s32 j;
    if (pool->state710 == 0) {
        for (; i < 6; i++) {
            if (update)
                fn_8013D7E0(pool->groups[i], pool->counts[i]);
            for (j = 0; j < pool->counts[i]; j++) {
                struct NUPICKUP_s *p = (struct NUPICKUP_s *)(j * sizeof(struct NUPICKUP_s) + (s32)pool->groups[i]);
                if (!(p->flags38 & 1)) {
                    f32 height = p->baseY + lbl_801E1614[p->kind3E].height;
                    if (p->y <= height || p->y > height)
                        p->y = height;
                }
                pool->groups[i][j].previousY = pool->groups[i][j].y;
            }
        }
    }
}

extern void (*fn_80009948(char *, s32))(char *, ...);
extern char str_SourceFile_Pickups[], str_Assert_Pickups_BufferOverflow[];
extern void fn_8013D550(struct NUPICKUPPOOL_s *);
void fn_8013EF38(struct NUPICKUPPOOL_s *pool)
{
    s32 base, next, end, overflow;
    if (pool->state710 == 0) {
        pool->state750 = 0;
        base = (pool->buffer184 + 3) & ~3;
        next = base + 0x4000;
        end = base + 0x5000;
        overflow = end - pool->bufferEnd188;
        pool->buffer184 = end;
        pool->groups[0] = (void *)base;
        pool->groups[4] = (void *)next;
        pool->buffers[1] = next;
        pool->buffers[0] = base;
        if (overflow > 0)
            fn_80009948(str_SourceFile_Pickups, 0x7cd)(str_Assert_Pickups_BufferOverflow, overflow);
        fn_8013D550(pool);
    }
}

extern void NuVecAdd(struct PickupVec *, struct PickupVec *, void *);
extern void NuVecScale(struct PickupVec *, struct PickupVec *, f32);
extern void fn_8006DCD4(char *, struct PickupVec *);
extern void fn_800958F8(void *, s32, void *);
extern f32 lbl_801AEDE0[], lbl_801AEDE4[];
extern char str_Pickup_MKAppear[];
void fn_8013F0AC(s32 id)
{
    struct PickupVec sum;
    f32 count;
    f32 unit;
    f32 zero;
    s32 i;
    struct NUPICKUP_s *p;
    struct NUPICKUPPOOL_s *pool = (void *)lbl_80409410;
    i = 0;
    zero = lbl_801AEDE0[0];
    sum.z = zero;
    sum.y = zero;
    sum.x = zero;
    count = zero;
    p = pool->groups[5];
    if (i < pool->counts[5]) {
        unit = lbl_801AEDE4[0];
        do {
            if (p->active39 && p->kind36 == id) {
                count += unit;
                fn_800958F8(pool->effect1B8, 0x12, p);
                NuVecAdd(&sum, &sum, p);
            }
            pool = (void *)lbl_80409410;
            i++;
            p++;
        } while (i < pool->counts[5]);
    }
    if (count > lbl_801AEDE0[0]) {
        if (count > lbl_801AEDE4[0]) NuVecScale(&sum, &sum, lbl_801AEDE4[0] / count);
        fn_8006DCD4(str_Pickup_MKAppear, &sum);
    }
}
