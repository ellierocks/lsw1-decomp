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
extern f32 lbl_8019DFB8[];

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
