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

extern s32 lbl_80408B00, lbl_80408B10, lbl_80408B14;
extern s32 lbl_80408AA0, lbl_80408AA4;
extern f32 lbl_80408A84;
extern void fn_8007AC80(s32, s32, s32, s32, s32, s32, f32, f32, f32, f32, f32, f32);

void fn_80079698(u8* menu)
{
    NuStrCpy(lbl_803D992C, lbl_80408ACC);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    fn_8007AC80(lbl_80408B00, 0, 255, 127, 0, lbl_80408A57, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f);
    {
        f32 time = lbl_8040AF18;
        f32 offset = lbl_80408A84;
        s32 action = lbl_80408AA0;
        *(f32*)(menu + 0x9c) = time - offset;
        fn_800769B4(menu, action, lbl_80408A57);
    }
    fn_800769B4(menu, lbl_80408AA4, lbl_80408A57);
}

void fn_8007975C(u8* menu)
{
    NuStrCpy(lbl_803D992C, lbl_80408AC8);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    fn_8007AC80(lbl_80408B10, 0, 255, 127, 0, lbl_80408A57, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f);
    {
        f32 offset = *(f32*)(menu + 0xa8);
        s32 action = lbl_80408AA0;
        *(f32*)(menu + 0x9c) = -0.5f - offset;
        fn_800769B4(menu, action, lbl_80408A57);
    }
    fn_800769B4(menu, lbl_80408AA4, lbl_80408A57);
}

void fn_80079824(u8* menu)
{
    NuStrCpy(lbl_803D992C, lbl_80408ACC);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    fn_8007AC80(lbl_80408B14, 0, 255, 127, 0, lbl_80408A57, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f);
    {
        f32 offset = *(f32*)(menu + 0xa8);
        s32 action = lbl_80408AA0;
        *(f32*)(menu + 0x9c) = -0.5f - offset;
        fn_800769B4(menu, action, lbl_80408A57);
    }
    fn_800769B4(menu, lbl_80408AA4, lbl_80408A57);
}
