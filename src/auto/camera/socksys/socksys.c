#include "types.h"

/* 0x10 bytes and up of a character animation record (OpenSaga CHARACTERANIM_s). */
struct socksys_anim_rec_s {
    u8 pad0[0x10];
    f32 blend_out_time;
    f32 playback_rate;
};

/* 0x54-byte character model (OpenSaga CHARACTERMODEL_s): per-animation tables. */
struct socksys_model_s {
    u8 pad0[0x8];
    struct socksys_anim_rec_s** anims;
    void** anim_data;
    u8 pad10[0x54 - 0x10];
};

/* Character system behind lbl_80408DF0 (OpenSaga apicharsys). */
struct socksys_a_s {
    s32 character_count;
    u8 pad4[4];
    s32 anim_capacity;
    u8 padC[0x18 - 0xC];
    struct socksys_model_s* models;
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

/* Sock (camera rail) node state; fn_80095558 initialises it and fn_80095630 picks the
 * sub-record whose id matches. */
struct socksys_b_s {
    f32 f0;
    f32 f4;
    u8 pad8[0x10 - 0x8];
    f32 f10;
    f32 f14;
    u8 pad18[0x3c - 0x18];
    u8 b3c;
    u8 flag;
    s16 s3e;
    s16 s40;
    s16 s42;
    s16 s44;
    s16 s46;
    u8 pad48[0x4c - 0x48];
    u8 b4c;
    u8 b4d;
    u8 b4e;
    u8 pad4f[0x52 - 0x4f];
    s16 s52;
};

/* 0x14-byte effect records (lbl_80409410->unk1B8): fn_800958F8 spawns record idx. */
struct socksys_fx_item_s {
    s32 id;
    u8 pad4[0x14 - 0x4];
};

struct socksys_fx_s {
    u8 pad0[4];
    s32 count;
    struct socksys_fx_item_s *items;
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

s32 ParticlesPerFrame(f32 rate, f32 time)
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
extern u64 __ashldi3();
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
                    *mask &= ~__ashldi3(1ULL, object->index);
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

s32 FindGameDebris(void *arg0, s32 arg1) {
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

s32 fn_80093D84(void *, void *, void *, void *, void *, f32, s32);

void fn_80095B58(void *arg0, void *arg1, void *arg2, void *arg3, void *arg4, f32 farg0) {
    fn_80093D84(arg0, arg1, arg2, arg3, arg4, farg0, 0);
}

s32 fn_80093D84(void *, void *, void *, void *, void *, f32, s32);

void fn_80095B7C(void *arg0, void *arg1, void *arg2, void *arg3, void *arg4, f32 farg0) {
    fn_80093D84(arg0, arg1, arg2, arg3, arg4, farg0, 1);
}

struct socksys_b_s *fn_80095630(struct socksys_b_s *o, s32 id, s32 a, s32 b) {
    if (o->flag != 0) {
        if (a != 0 && o->s40 == id) {
            return (struct socksys_b_s *)&o->f14;
        }
        if (b != 0 && o->s3e == id) {
            return (struct socksys_b_s *)&o->f10;
        }
    } else if (o->s42 == id) {
        return o;
    }
    return NULL;
}

extern void fn_80063E14(s32 *, s32, void *, s32);

s32 fn_800958F8(struct socksys_fx_s *l, s32 idx, void *pos) {
    s32 sp8;

    if (idx >= 0 && idx < l->count && l->items[idx].id != -1) {
        sp8 = -1;
        fn_80063E14(&sp8, l->items[idx].id, pos, 1);
        return 1;
    }
    return 0;
}

extern f32 lbl_8019DFB0[];

void fn_80095558(struct socksys_b_s *o, s32 id) {
    f32 z;
    if (o != NULL) {
        z = lbl_8019DFB0[0];
        o->s46 = id;
        o->s44 = id;
        o->s42 = id;
        o->f4 = z;
        o->f14 = z;
        o->f0 = z;
        o->b3c = 4;
        o->s52 = -1;
        o->flag = 0;
        o->b4e = 0;
        o->b4c = 0;
        o->b4d = 0;
    }
}

extern f32 lbl_8019DF68[];
extern f32 lbl_8019DF6C[];
extern f32 lbl_8019DF70[];
extern f32 lbl_8019DF74[];
extern f32 lbl_8040AF3C;

f32 AnimDuration(s32 character, s32 anim, f32 start, f32 end, s32 subtract_blend) {
    f32 duration;
    struct socksys_model_s *model;

    if (character < 0 || character >= lbl_80408DF0->character_count)
        return lbl_8019DF68[0];
    if (anim < 0 || anim >= lbl_80408DF0->anim_capacity)
        return lbl_8019DF68[0];
    if (lbl_80408DF0->ids[character] == -1)
        return lbl_8019DF68[0];

    model = &lbl_80408DF0->models[lbl_80408DF0->ids[character]];
    if (model->anim_data[anim] == NULL)
        return lbl_8019DF68[0];

    duration = fn_800215F0(model->anim_data[anim]);
    if (start >= lbl_8019DF6C[0] && start < duration) {
        if (end >= lbl_8019DF6C[0] && end < duration && end > start) {
            duration = end - start;
        } else {
            duration -= start;
        }
    } else if (end >= lbl_8019DF6C[0] && end < duration && end > start) {
        duration = end - lbl_8019DF6C[0];
    } else {
        duration -= lbl_8019DF6C[0];
    }

    duration *= (lbl_8019DF6C[0] / (model->anims[anim]->playback_rate / lbl_8019DF70[0])) * lbl_8019DF74[0];
    lbl_8040AF3C = model->anims[anim]->blend_out_time;
    if (subtract_blend != 0) {
        duration -= lbl_8040AF3C;
    }
    return duration;
}
