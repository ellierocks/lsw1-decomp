#include "game/pickups.h"

extern f32 lbl_801AEC5C[];
extern f32 lbl_801AEC60[];
extern f32 lbl_801AEC64[];
extern s32 lbl_804093C8;
extern f32 lbl_80409A08;

void fn_8013C620(struct NUPICKUPPOOL_s* pool)
{
    s32 i = 0, j;
    if (pool->state710 == 0) {
        f32* value = lbl_801AEC5C;
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
