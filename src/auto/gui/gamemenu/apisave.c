#include "types.h"

extern f32 lbl_8019B9D4[];
extern s32 lbl_80408BF0;
extern f32 lbl_80408C40;
extern s32 lbl_80408C48;
extern s32 lbl_80408C4C;

void fn_800790A4(void)
{
    lbl_80408C48 = 1;
    lbl_80408BF0 = 0;
}

void fn_8007917C(void)
{
    lbl_80408C4C = 0;
}

void fn_80079188(void)
{
    lbl_80408C40 = lbl_8019B9D4[0];
}

void fn_800790B8(void)
{
    lbl_80408C48 = 0;
    lbl_80408BF0 = 0;
}
