#include "types.h"

struct socksys_a_s {
    u8 pad0[0x1c];
    s16* ids;
};

struct socksys_item_s {
    u8 pad0[0x20];
    f32 f20;
    f32 f24;
};

struct socksys_list_s {
    u8 pad0[8];
    struct socksys_item_s** items;
    s32* present;
};

struct socksys_b_s {
    u8 pad0[0x3d];
    u8 flag;
    u8 pad3e[2];
    s16 s40;
    s16 s42;
};

extern struct socksys_a_s* lbl_80408DF0;
extern s32 lbl_80408DE4;
extern s32 lbl_8040AF38;
extern f32 lbl_8019DFB8[];

void fn_80094F90(s32 a)
{
    lbl_80408DE4 = a;
}

s32 fn_80094F98(void)
{
    return lbl_80408DE4;
}

s32 fn_800952D8(s32 idx)
{
    if (lbl_80408DF0->ids[idx] != -1) {
        return 1;
    }
    return 0;
}

f32 fn_80095688(struct socksys_list_s* l, s32 idx)
{
    if (idx != -1 && l->present[idx] != 0) {
        return l->items[idx]->f20;
    }
    return lbl_8019DFB8[0];
}

s16 fn_800956C0(struct socksys_b_s* o)
{
    if (o->flag != 0) {
        return o->s40;
    }
    return o->s42;
}

f32* fn_80095724(struct socksys_list_s* l, s32 idx)
{
    if (l->present[idx] != 0) {
        return &l->items[idx]->f24;
    }
    return 0;
}

void fn_80095874(s32 a)
{
    lbl_8040AF38 = a;
}

extern u32 NuRandInt(void);
extern f32 lbl_8019DFC4[];
extern f32 lbl_8019DFC8[];
extern f32 lbl_8019DFCC[];
extern f32 lbl_8019DFD0[];

s32 fn_80095750(f32 rate, f32 time)
{
    s32 scaled = (s32)((rate * lbl_8019DFC4[0]) * (time * lbl_8019DFC8[0]));
    s32 count = 0;
    while (scaled > 0xffff) {
        scaled -= 0x10000;
        count++;
    }
    if ((NuRandInt() >> 16) < (u32)scaled)
        count++;
    return count;
}

s32 fn_800957E0(f32 rate, f32 time)
{
    s32 scaled = (s32)(((rate / lbl_8019DFCC[0]) * lbl_8019DFD0[0]) * (time * lbl_8019DFCC[0]));
    s32 count = 0;
    while (scaled > 0xffff) {
        scaled -= 0x10000;
        count++;
    }
    if ((NuRandInt() >> 16) < (u32)scaled)
        count++;
    return count;
}

struct socksys_anim_s {
    f32 time;
    u8 unknown04[0x42];
    s16 index;
};
extern f32 fn_8000C038(void);
extern f32 fn_800215F0(void*);
extern f32 lbl_8019DFB4[];
void fn_800955A8(struct socksys_list_s* list, struct socksys_anim_s* anim)
{
    f32 time;
    if (list != NULL && anim != NULL && list->present[anim->index] != 0) {
        time = fn_8000C038();
        time = time * (fn_800215F0((void*)list->present[anim->index]) - lbl_8019DFB4[0]) + lbl_8019DFB4[0];
        anim->time = time;
    }
}

struct relation_object { u8 pad00[0x20c]; s32 flags; u8 pad210[0x81]; u8 index; };
struct relation_pool { s32 stride; struct relation_object* objects; u64 masks[64]; };
extern u64 UI_DrawText();
extern void* memset();
void fn_80095128(struct relation_pool* pool, struct relation_object* object)
{
    u64* masks;
    struct relation_object* p;
    s32 i;
    if (pool != NULL && object != NULL) {
        if (pool->masks != NULL) {
            masks = pool->masks;
            p = pool->objects;
            for (i = 64; i != 0; i--) {
                if (p->flags < 0) {
                    u64* mask = &masks[p->index];
                    *mask &= ~UI_DrawText(1ULL, object->index);
                }
                p = (struct relation_object*)((u8*)p + pool->stride);
            }
            *(u64*)((u8*)masks + ((s32)object->index << 3)) = 0;
        }
        memset(object, 0, pool->stride);
    }
}

s32 memset(s32, s32, s32);

void fn_800951FC(void *arg0) {
    if (arg0 != NULL) {
        memset((*(s32 *)((s8 *)(arg0) + (4))), 0, (*(s32 *)((s8 *)(arg0) + (0))) << 6);
    }
}

s32 NuStrICmp(s32, s32);

s32 fn_8009587C(void *arg0, s32 arg1) {
    s32 var_r30;
    s32 var_r31;

    var_r31 = (*(s32 *)((s8 *)(arg0) + (0)));
    if (var_r31 < (s32) (*(s32 *)((s8 *)(arg0) + (4)))) {
        var_r30 = var_r31 * 0x14;
loop_2:
        if (NuStrICmp(arg1, (*(s32 *)((s8 *)(arg0) + (8))) + var_r30 + 4) == 0) {
            return var_r31;
        }
        var_r31 += 1;
        var_r30 += 0x14;
        if (var_r31 >= (s32) (*(s32 *)((s8 *)(arg0) + (4)))) {
            goto block_5;
        }
        goto loop_2;
    }
block_5:
    return -1;
}

extern f32 lbl_8019E4F4[];

f32 fn_8009CB2C(s32 *arg0, void *arg1) {
    u8 temp_r0;
    void *temp_r9;

    if (arg0 != NULL) {
        temp_r0 = (*(u8 *)((s8 *)(arg1) + (1)));
        if ((s8) temp_r0 != -1) {
            temp_r9 = (*(s32 *)((s8 *)((((s8) temp_r0 * 0x104) + *arg0)) + (0x30))) + ((*(s16 *)((s8 *)(arg1) + (2))) << 7);
            return ((*(f32 *)((s8 *)(temp_r9) + (0x78))) * (*(f32 *)((s8 *)(arg1) + (4)))) + (*(f32 *)((s8 *)(temp_r9) + (0x7C)));
        }
    }
    return lbl_8019E4F4[0];
}

void fn_8009CB88(s32 *arg0, s32 arg1) {
    void *temp_r9;

    if ((arg0 != NULL) && (arg1 >= 0) && (arg1 <= 0x1F)) {
        temp_r9 = (arg1 * 0x104) + *arg0;
        (*(u16 *)((s8 *)(temp_r9) + (0x64))) = (u16) ((*(u16 *)((s8 *)(temp_r9) + (0x64))) & 0xFDFF);
    }
}

void fn_8009CBC4(s32 *arg0, s32 arg1) {
    void *temp_r9;

    if ((arg0 != NULL) && (arg1 >= 0) && (arg1 <= 0x1F)) {
        temp_r9 = (arg1 * 0x104) + *arg0;
        (*(u16 *)((s8 *)(temp_r9) + (0x64))) = (u16) ((*(u16 *)((s8 *)(temp_r9) + (0x64))) | 0x200);
    }
}

void fn_8009CAB4(s32 arg0, s32 arg1) {
    s32 temp_r11;
    s32 temp_r8;
    s32 var_r11;

    var_r11 = arg1;
    if (arg1 < 0) {
        var_r11 = arg1 + 0x1F;
    }
    temp_r8 = arg0 + 0xD8;
    temp_r11 = (var_r11 >> 5) * 4;
    *(s32 *)(temp_r8 + temp_r11) = *(s32 *)(temp_r8 + temp_r11) | (1 << (arg1 & 0x1F));
}
