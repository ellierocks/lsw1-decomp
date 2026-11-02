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
void *fn_800D5680(char* fmt, s32 value, s32 a, s32 r, s32 g, s32 b, s32 alpha, f32 x, f32 y, f32 z);

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
extern s32 fn_8013D550(struct NUPICKUPPOOL_s *);
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

static inline void pickup_expand(struct PickupVec *out, struct PickupVec *in, f32 r)
{
    out->x = in->x + r;
    out->y = in->y + r;
    out->z = in->z + r;
}
static inline void pickup_contract(struct PickupVec *out, struct PickupVec *in, f32 r)
{
    out->x = in->x - r;
    out->y = in->y - r;
    out->z = in->z - r;
}
struct PickupFindActor {
    char unk0[0x90];
    char collision[0x2c];
    f32 valueBC;
    char unkC0[0xc8];
    struct PickupVec min, max;
    char unk1A0[0x50];
    f32 value1F0;
};
extern s32 fn_800CFEE8(void *, f32, f32, f32, f32, void *);
extern u8 lbl_803EAF84[];
struct NUPICKUP_s *fn_8013E190(struct PickupFindActor *actor, s32 entries, s32 n, s32 rooms, f32 radius)
{
    struct PickupVec max, min;
    s32 i = 0;
    s32 cursor = entries;
    pickup_expand(&max, &actor->max, radius);
    pickup_contract(&min, &actor->min, radius);
    for (; i < n; i++, cursor += 0x40) {
        struct NUPICKUP_s *p = (void *)cursor;
        struct PickupVec *pos = (void *)p;
        if (p->active39 && p->enabled3C && (!rooms || p->kind36 == -1 || lbl_803EAF84[p->kind36])) {
            if (pos->x < max.x && pos->x > min.x && pos->z < max.z && pos->z > min.z && pos->y < max.y && pos->y > min.y && fn_800CFEE8(p, radius, radius, actor->valueBC, actor->value1F0, actor->collision)) {
                *(struct PickupVec *)((char *)p + 0xc) = *pos;
                if (p->link1C)
                    ((struct NUPICKUP_s *)p->link1C)->link18 = p->link18;
                if (p->link18)
                    ((struct NUPICKUP_s *)p->link18)->link1C = p->link1C;
                return p;
            }
        }
    }
    return 0;
}

void fn_8013D2F4(struct NUPICKUP_s *base, s32 count, s32 offsets)
{
    s32 i = 0;
    if (!offsets) {
        for (; i < count; i++) {
            struct NUPICKUP_s *p = &base[i];
            if (p->link18 < 0) p->link18 = 0;
            else p->link18 = (s32)&base[p->link18];
            if (p->link1C < 0) p->link1C = 0;
            else p->link1C = (s32)&base[p->link1C];
        }
    } else {
        if (i < count) {
            s32 zero = 0;
            struct NUPICKUP_s *p = base;
            do {
                if (p->link18 < 0) p->link18 = zero;
                else p->link18 += (s32)base;
                if (p->link1C < 0) p->link1C = zero;
                else p->link1C += (s32)base;
                p++;
            } while (++i < count);
        }
    }
    if (base) {
        if ((u8)base->kind3E <= 2) {
            for (i = 0; i < count; i++, base++) {
                base->previousY = base->y;
                if (base->kind3E > 2) base->kind3E = 2;
            }
        } else {
            for (i = 0; i < count; i++, base++) base->previousY = base->y;
        }
    }
}

struct PickupCollectDef { char unk0[0x1c]; s32 award; char unk20[2]; u8 flags; char unk23[0xd]; f32 radius; };
struct PickupCollectOwner { char unk0[0x61c]; s32 *handle; };
struct PickupCollectActor { char unk0[0xeec]; struct PickupCollectOwner *owner; };
extern struct PickupCollectDef lbl_801E15E8[];
extern f32 lbl_801AED54[];
extern void fn_8013DF98(struct NUPICKUP_s *, void *, s32);
extern void fn_8013E8F8(void *, struct NUPICKUP_s *);
extern void fn_8014165C(struct NUPICKUP_s *);
extern void fn_800AC020(s32, f32);
static inline s32 pickup_collect(struct NUPICKUP_s *p, struct PickupFindActor *actor)
{
    s32 collected = 1;
    switch (p->kind3E) {
    case 0: case 1: case 2:
        fn_8013DF98(p, actor, lbl_801E15E8[p->kind3E].award);
        break;
    case 4:
        fn_8013E8F8(actor, p);
        break;
    case 5:
        fn_8014165C(p);
        fn_800AC020(*((struct PickupCollectActor *)actor)->owner->handle, lbl_801AED54[0]);
        break;
    }
    return collected;
}

s32 fn_8013E34C(struct PickupFindActor *actor)
{
    struct NUPICKUPPOOL_s *pool = (void *)lbl_80409410;
    s32 i;
    struct NUPICKUP_s *p;
    if (pool->state710 == 0) {
        for (i = 0; i < 6; i++) {
            p = fn_8013E190(actor,
                (s32)((struct NUPICKUPPOOL_s *)lbl_80409410)->groups[i],
                ((struct NUPICKUPPOOL_s *)lbl_80409410)->counts[i],
                lbl_801E15E8[i].flags & 1, lbl_801E15E8[i].radius);
            if (p) {
                s32 collected = pickup_collect(p, actor);
                if (collected) p->active39 = 0;
                goto yes;
            }
        }
        p = fn_8013E190(actor,
            (s32)((struct NUPICKUPPOOL_s *)lbl_80409410)->buffers[1],
            0x40, 0, lbl_801E15E8[0].radius);
        if (p) goto final;
    }
    return 0;
final:
    p->active39 = 0;
    fn_8013DF98(p, actor, lbl_801E15E8[p->kind3E].award);
yes:
    return 1;
}

struct PickupHeartOwner {
    char unk0[0x61c];
    s32 *handle;
    char unk620[0xe8];
    f32 value708;
    char unk70c[0x24];
    f32 value730;
    char unk734[0xa3];
    u8 hearts;
};
struct PickupHeartActor {
    char unk0[0x78];
    struct PickupVec velocity;
    char unk84[0x57d];
    s8 state601;
    char unk602[0x22];
    u16 rotX, rotY;
    char unk628[0x8c4];
    struct PickupHeartOwner *owner;
    char unkef0[0x194];
    u8 total;
    s8 held;
};
extern f32 NuVecDot(struct PickupVec *, struct PickupVec *);
extern void NuVecRotateX(struct PickupVec *, struct PickupVec *, u16);
extern void NuVecRotateY(struct PickupVec *, struct PickupVec *, u16);
extern void fn_80095A44(void *, s32, struct NUPICKUP_s *, s32,
                        struct PickupVec *);
extern f32 lbl_801AED9C[], lbl_801AEDA0[], lbl_801AEDA4[];
extern struct PickupVec lbl_801D5970[];
extern s32 lbl_8040AFB8;
extern char str_Pickup_Heart[];
void fn_8013E8F8(void *arg0, struct NUPICKUP_s *p) {
    struct PickupHeartActor *actor = arg0;
    struct PickupVec velocity, projected, direction;
    if (actor->held > 0 && actor->held < actor->total)
        actor->held++;
    if (actor->owner->hearts <= 3)
        actor->owner->hearts++;
    if (lbl_8040AFB8) {
        NuVecScale(&velocity, &actor->velocity, lbl_801AED9C[0]);
        if (lbl_80409410->mode19C != 10) {
            f32 zero = lbl_801AEDA0[0];
            if (actor->owner->value730 != zero && actor->state601 != -1) {
                NuVecRotateX(&direction, lbl_801D5970, actor->rotX);
                NuVecRotateY(&direction, &direction, actor->rotY);
                projected.x = NuVecDot(&velocity, &direction);
                projected.y = zero;
                projected.z = actor->owner->value730 * actor->owner->value708;
                NuVecRotateX(&projected, &projected, actor->rotX);
                NuVecRotateY(&velocity, &projected, actor->rotY);
            }
        }
        fn_80095A44(((struct NUPICKUPPOOL_s *)lbl_80409410)->effect1B8, 14, p,
                    15, &velocity);
    } else
        fn_800958F8(((struct NUPICKUPPOOL_s *)lbl_80409410)->effect1B8, 13, p);
    fn_8006DCD4(str_Pickup_Heart, (struct PickupVec *)p);
    fn_800AC020(*actor->owner->handle, lbl_801AEDA4[0]);
}

struct PickupCoinDef {
    char unk0[0x20];
    s16 effect;
    char unk22[0x12];
};
struct PickupCoinPool {
    char unk0[0x774];
    s32 handles[1][4];
};
struct PickupCoinOwner {
    char unk0[0x140];
    s32 award;
    char unk144[0x4d8];
    s32 *handle;
};
struct PickupCoinActor {
    char unk0[0x284];
    s8 player;
    char unk285[0xc67];
    struct PickupCoinOwner *owner;
};
struct PickupCoinEffect {
    char unk0[0x93];
    u8 mode;
};
extern void *gPlayerPointers[];
extern f32 lbl_801AED48[], lbl_801AED4C[], lbl_801AED50[];
extern s32 lbl_8040948C, lbl_80409490, lbl_80409494;
extern f32 lbl_804094AC, lbl_804094B0, lbl_804094B8, lbl_804094D0, lbl_80409504;
extern f32 lbl_80409518[], lbl_80409A0C, lbl_8040B0D0, lbl_8040B1C4;
extern char str_Pickup_Coin[], str_Pickup_CoinB[];
static inline void pickup_coin_sound(s32 *handle, f32 value) {
    fn_800AC020(*handle, value);
}
void fn_8013DF98(struct NUPICKUP_s *p, void *arg1, s32 award) {
    struct PickupCoinActor *actor = arg1;
    s8 kind;
    struct PickupVec position;
    f32 value;
    s16 effect;
    struct PickupCoinEffect *result;
    if (actor->player == -1) {
        actor->owner->award += award;
        return;
    }
    if (p->kind3E == 2) {
        pickup_coin_sound(actor->owner->handle, lbl_801AED48[0]);
    }
    kind = p->kind3E;
    if (kind == 2)
        fn_8006DCD4(str_Pickup_CoinB, (void *)p);
    else
        fn_8006DCD4(str_Pickup_Coin, (void *)p);
    if (lbl_80409410->mode19C == 21) {
        if (actor->player <= 1)
            position.x = lbl_80409518[actor->player];
        else
            position.x = lbl_801AED4C[0];
        position.y = lbl_804094D0;
        value = lbl_80409504;
        lbl_8040B0D0 = lbl_80409A0C + lbl_801AED50[0];
    } else {
        if (actor == gPlayerPointers[0])
            position.x = lbl_804094AC;
        else
            position.x = -lbl_804094AC;
        position.y = lbl_804094D0 + lbl_804094B0;
        value = lbl_804094B8;
        lbl_8040B1C4 = lbl_80409A0C + lbl_801AED50[0];
    }
    position.z = lbl_801AED50[0];
    effect = ((struct PickupCoinDef *)lbl_801E15E8)[p->kind3E].effect;
    if (effect != -1 &&
        ((struct PickupCoinPool *)lbl_80409410)->handles[effect][0]) {
        lbl_8040948C = effect;
        lbl_80409490 = actor->player;
        lbl_80409494 = award;
        result = fn_800D5680(0, (s32)p, (s32)&position, 0, 0, 0, 0x12d,
                             position.z, value, lbl_80409A0C);
        if (result && lbl_80409410->mode19C == 21)
            result->mode = 5;
    }
}

struct PickupLoadRecord {
    struct PickupVec pos, oldPos;
    s32 link18, link1C;
    f32 baseY;
    char unk24[0xe];
    u16 room32, room34;
    s16 room36;
    u8 flags, active, value3a;
    char unk3b[3];
    s8 kind;
    char unk3f;
};
struct PickupLoadPool {
    char unk0[0x714];
    struct PickupLoadRecord *data;
    s32 buffer718;
    struct NUPICKUP_s *groups[6];
    s32 counts[6];
    s32 count;
};
extern void *memset(void *, s32, u32);
extern void NuStrCat(char *, char *);
extern s32 fn_8006F0BC(char *, s32);
extern void fn_8006F1D8(void);
extern f32 fn_8006F28C(void);
extern s32 fn_8006F37C(void);
extern s16 fn_8006F55C(void);
extern s8 fn_8006F71C(void);
extern void fn_8006F7E0(s32);
extern void fn_8006F8A8();
extern char lbl_801AECCC[];
#define PICKUP_LOAD_RECORD                                                     \
    ((struct PickupLoadRecord *)(offset + (s32)pool->data))
s32 fn_8013D550(struct NUPICKUPPOOL_s *arg0) {
    s32 minimumVersion;
    struct PickupLoadPool *pool = (void *)arg0;
    char filename[0x8100];
    s32 version = -1, i, offset;
    s32 maximum = 0;
    memset(pool->data, -1, 0x4000);
    pool->count = maximum;
    for (i = 0; i < 0x100; i++)
        pool->groups[0][i].kind3E = -1;
    filename[0] = 0;
    NuStrCat(filename, (char *)pool + 0x80);
    maximum = 0x100;
    minimumVersion = 1;
    NuStrCat(filename, lbl_801AECCC);
    fn_8006F8A8(1);
    fn_8006F7E0(1);
    if (fn_8006F0BC(filename, 0)) {
        version = fn_8006F37C();
        if (version > 8) {
            if (version < minimumVersion)
                goto invalid;
            if (version <= 9)
                goto read_records;
        invalid:
            {
                fn_8006F1D8();
                fn_8006F8A8(0);
                return -1;
            }
        read_records:
            pool->count = fn_8006F37C();
            if (pool->count > 0x100)
                pool->count = maximum;
            maximum = 0;
            if (maximum < pool->count) {
                offset = 0;
                do {
                    PICKUP_LOAD_RECORD->active = fn_8006F71C();
                    PICKUP_LOAD_RECORD->kind = fn_8006F71C();
                    PICKUP_LOAD_RECORD->value3a = fn_8006F71C();
                    PICKUP_LOAD_RECORD->pos.x = fn_8006F28C();
                    PICKUP_LOAD_RECORD->pos.y = fn_8006F28C();
                    PICKUP_LOAD_RECORD->pos.z = fn_8006F28C();
                    PICKUP_LOAD_RECORD->baseY = fn_8006F28C();
                    PICKUP_LOAD_RECORD->room32 = fn_8006F55C();
                    PICKUP_LOAD_RECORD->room34 = fn_8006F55C();
                    PICKUP_LOAD_RECORD->link18 = fn_8006F37C();
                    PICKUP_LOAD_RECORD->link1C = fn_8006F37C();
                    PICKUP_LOAD_RECORD->room36 = fn_8006F55C();
                    PICKUP_LOAD_RECORD->flags = fn_8006F71C();
                    maximum++;
                    offset += 0x40;
                } while (maximum < pool->count);
            }
            fn_8013D438((void *)pool);
        }
        fn_8006F1D8();
    }
    offset = 0x10;
    fn_8013D2F4(pool->groups[0], pool->counts[0], 0);
    i = 2;
    fn_8013D2F4(pool->groups[1], pool->counts[1], 0);
    fn_8013D2F4(pool->groups[2], pool->counts[2], 0);
    {
        struct NUPICKUP_s **groups = pool->groups;
        s32 *counts = pool->counts;
        s32 offsets = version <= 7;

        do {
            fn_8013D2F4(*(struct NUPICKUP_s **)(offset + (s32)groups),
                        *(s32 *)(offset + (s32)counts), offsets);
            offset += 4;
        } while (--i);
    }
    fn_8006F8A8(0);
    return 1;
}
#undef PICKUP_LOAD_RECORD
