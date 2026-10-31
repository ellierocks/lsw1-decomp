#include "types.h"

extern f32 lbl_8040B240;
extern f32 lbl_8040B244;
extern f32 lbl_8040B248;
extern s32 lbl_8040B0AC;
extern f32 lbl_8040B0B0;
extern f32 lbl_8040B0B4;
extern f32 lbl_8040B0B8;
extern s32 lbl_801DEC0C[];

void fn_8010BBD8(u8* a)
{
    f32 x = *(f32*)(a + 140);
    f32 y = *(f32*)(a + 144);
    f32 z = *(f32*)(a + 148);
    s32 one = 1;
    lbl_8040B0B0 = x;
    lbl_8040B0B4 = y;
    lbl_8040B0B8 = z;
    lbl_8040B240 = x;
    lbl_8040B244 = y;
    lbl_8040B248 = z;
    lbl_8040B0AC = one;
}

void fn_8010BC08(u8* a)
{
    f32 x = *(f32*)(a + 140);
    f32 y = *(f32*)(a + 144);
    f32 z = *(f32*)(a + 148);
    lbl_8040B0B0 = x;
    lbl_8040B0B4 = y;
    lbl_8040B0B8 = z;
}

void fn_8010BD90(void)
{
    s32 v = -1;
    lbl_801DEC0C[1] = v;
    lbl_801DEC0C[2] = v;
    lbl_801DEC0C[3] = v;
    lbl_801DEC0C[4] = v;
    lbl_801DEC0C[5] = v;
    lbl_801DEC0C[6] = v;
    lbl_801DEC0C[7] = v;
    lbl_801DEC0C[0] = v;
}
