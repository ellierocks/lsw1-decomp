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

extern char lbl_803D992C[];
extern const char *lbl_80408AC8, *lbl_80408ACC;
extern s32 lbl_8040A618, lbl_8040A61C, lbl_8040A620;
extern void (*lbl_80408A5C)(u8*, f32);
extern f32 lbl_8040AF18;
extern s32 lbl_80408AAC;
extern u8 lbl_80408A57;
extern void NuStrCpy(char*, const char*);
extern void fn_800769B4(u8*, s32, s32);

void fn_80079198(u8* menu)
{
    NuStrCpy(lbl_803D992C, lbl_80408AC8);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    if (lbl_80408A5C) lbl_80408A5C(menu, -0.5f);
    {
        s32 state = *(s32*)(menu + 0x90);
        f32 time = lbl_8040AF18;
        s32 action = lbl_80408AAC;
        *(s32*)(menu + 0x90) = state + 1;
        *(f32*)(menu + 0x9c) = time;
        fn_800769B4(menu, action, lbl_80408A57);
    }
}

void fn_80079604(u8* menu)
{
    NuStrCpy(lbl_803D992C, lbl_80408ACC);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    if (lbl_80408A5C) lbl_80408A5C(menu, -0.5f);
    {
        s32 state = *(s32*)(menu + 0x90);
        f32 time = lbl_8040AF18;
        s32 action = lbl_80408AAC;
        *(s32*)(menu + 0x90) = state + 1;
        *(f32*)(menu + 0x9c) = time;
        fn_800769B4(menu, action, lbl_80408A57);
    }
}
