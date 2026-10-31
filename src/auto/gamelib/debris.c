#include "types.h"

extern u8 lbl_802B1504[];
extern f32 lbl_80195760[];
extern f32 lbl_80195764[];
extern f32 lbl_80195768[];
extern f32 lbl_804083AC;
extern f32 lbl_804083C8;
extern s32 lbl_804083DC;
extern s32 lbl_8040840C;
extern s32 lbl_80408410;
extern s32 lbl_8040A5D0;
extern f32 lbl_8040A600;
extern s32 lbl_8040AE4C;

void fn_8005A42C(s32 a)
{
    lbl_8040AE4C = a;
}

s32 fn_8005A434(void)
{
    return lbl_8040AE4C;
}

s32 fn_8005A82C(void)
{
    return lbl_8040A5D0;
}

void fn_8005A8D4(f32 x)
{
    lbl_8040A600 = x;
}

void fn_8005A8DC(f32 x)
{
    lbl_804083AC = x;
}

void fn_8006497C(s32 a)
{
    lbl_8040840C = a;
}

void fn_800649FC(f32 x)
{
    lbl_804083C8 = x;
}

void fn_80064A50(s32 a)
{
    lbl_804083DC = a;
}

void fn_80064BEC(s32 a)
{
    lbl_80408410 = a;
}

void fn_8005A834(u8* a)
{
    a[0] = lbl_802B1504[96];
    a[1] = lbl_802B1504[97];
    a[2] = lbl_802B1504[98];
    a[3] = lbl_802B1504[99];
}

void GenDebMomAdjFromPosAll(s32 a, s32 b, f32* c)
{
    f32 k = lbl_80195760[0];
    c[4] = c[0] * k;
    c[5] = c[1] * k;
    c[6] = c[2] * k;
}

void GenDebMomAdjFromSplash(s32 a, s32 b, f32* c)
{
    f32 k = lbl_80195764[0];
    c[4] = c[0] * k;
    c[6] = c[2] * k;
}

void GenDebMomAdjFromAshRock(s32 a, s32 b, f32* c)
{
    f32 k = lbl_80195768[0];
    c[4] = c[0] * k;
    c[6] = c[2] * k;
}
