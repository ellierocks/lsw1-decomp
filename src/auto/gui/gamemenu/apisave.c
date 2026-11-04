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

extern s32 lbl_80408BFC, lbl_80408C00, lbl_80408224;
extern f32 lbl_80408C20, lbl_80408C34;
extern f32 lbl_8019BAC4, lbl_8019BAC8;
extern void Menu_CloseCurrent(void);

void fn_80079E28(void)
{
    if (lbl_80408BFC || lbl_80408C00 || lbl_80408C20 > 0.0f)
        lbl_80408C34 = 1.5f;
    if (lbl_80408BFC && lbl_80408224) {
        lbl_80408BFC = 0;
        Menu_CloseCurrent();
    }
    if (lbl_80408C34 <= 0.0f)
        Menu_CloseCurrent();
}

extern u16 lbl_80408BAA;
extern s32 lbl_8040AE0C, lbl_80408A30, lbl_8040821C, lbl_80408220, lbl_8040822C, lbl_80408230;
extern void Menu_Open(s32, s32, s32);

void fn_800790C8(u8* menu)
{
    u16 selection = lbl_80408BAA;
    s32 active = lbl_8040AE0C;
    *(u16*)(menu + 0x12) = 1;
    *(u16*)(menu + 0x10) = selection - 1;
    lbl_80408C4C = 0;
    lbl_80408A30 = 0;
    if (active)
        Menu_Open(0x3fd, -1, -1);
    else if (lbl_8040821C != 2)
        Menu_Open(0x3ec, -1, -1);
    else if (!lbl_80408220)
        Menu_Open(0x3ee, 1, -1);
    else if (!lbl_8040822C || !lbl_80408230)
        Menu_Open(0x3ed, -1, -1);
}

extern s32 lbl_80408A54;
extern char *lbl_80408B7C;
extern void fn_8007B298(char*, s32, s32, s32, s32, s32, s32, s32, s32, f32, f32, f32, f32, f32, f32, f32);

void fn_80079434(u8* menu)
{
    fn_8007B298(lbl_80408B7C, 0, 255, 127, 0, 3, 0, 0, lbl_80408A54,
                0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.5f);
    {
        f32 time = lbl_8040AF18;
        f32 offset = lbl_80408A84;
        s32 action = lbl_80408AA0;
        *(f32*)(menu + 0x9c) = time - offset;
        fn_800769B4(menu, action, lbl_80408A57);
    }
    fn_800769B4(menu, lbl_80408AA4, lbl_80408A57);
}

extern s32 lbl_80408C0C, lbl_80408C10, gMenuDepth;
extern f32 lbl_80408C24, lbl_80408C38;
extern u8 gMenuStack[];
extern void fn_80076194(void);
extern void Menu_OpenSpecial(void*);

void fn_80079EBC(void)
{
    if (lbl_80408C0C || lbl_80408C10 || lbl_80408C24 > 0.0f)
        lbl_80408C38 = 1.5f;
    if (lbl_80408C0C && lbl_80408224) {
        lbl_80408C0C = 0;
        fn_80076194();
        Menu_OpenSpecial(gMenuStack + gMenuDepth * 0xe0);
    }
    if (lbl_80408C38 <= 0.0f)
        Menu_CloseCurrent();
}

extern char *lbl_80408B78, *lbl_80408B84, *lbl_80408B34;
extern s32 lbl_80408AB0;

void fn_8007922C(u8* menu)
{
    char* title;
    f32 half = 0.5f;
    if (lbl_8040821C == 1) title = lbl_80408B78;
    else if (lbl_8040821C == 3) title = lbl_80408B84;
    else title = lbl_80408B34;
    fn_8007B298(title, 0, 255, 127, 0, 2, 0, 0, lbl_80408A54,
                0.0f, 0.3f, 1.0f, half, half, half, 1.5f);
    {
        f32 time = lbl_8040AF18;
        f32 offset = lbl_80408A84;
        s32 action = lbl_80408AB0;
        *(f32*)(menu + 0x9c) = time - offset;
        fn_800769B4(menu, action, lbl_80408A57);
    }
    fn_800769B4(menu, lbl_80408AAC, lbl_80408A57);
}

void fn_80079A30(u8* menu)
{
    if (*(s32*)(menu + 0x6c)) {
        fn_80076194();
        Menu_Open(0x3f8, 1, -1);
    }
    if (*(s32*)(menu + 0x68)) {
        if (*(s16*)(menu + 0x0a) == *(s16*)(menu + 0x12)) {
            fn_80076194();
            Menu_Open(0x3f8, 1, -1);
        }
        if (*(s32*)(menu + 0x68) && *(s16*)(menu + 0x0a) == 0)
            Menu_CloseCurrent();
    }
}

void fn_80079AC4(u8* menu)
{
    if (*(s32*)(menu + 0x6c)) {
        fn_80076194();
        Menu_Open(0x3f9, 1, -1);
    }
    if (*(s32*)(menu + 0x68)) {
        if (*(s16*)(menu + 0x0a) == *(s16*)(menu + 0x12)) {
            fn_80076194();
            Menu_Open(0x3f9, 1, -1);
        }
        if (*(s32*)(menu + 0x68) && *(s16*)(menu + 0x0a) == 0)
            Menu_CloseCurrent();
    }
}

void fn_80079B58(u8* menu)
{
    if (*(s32*)(menu + 0x6c)) {
        fn_80076194();
        Menu_Open(0x3f8, 1, -1);
    }
    if (*(s32*)(menu + 0x68)) {
        if (*(s16*)(menu + 0x0a) == 0) Menu_CloseCurrent();
        else if (*(s16*)(menu + 0x0a) == 1) {
            fn_80076194();
            Menu_Open(0x3f8, 1, -1);
        }
    }
}

extern s32 lbl_80408A44, lbl_80408BD0, lbl_80408BD4, lbl_80408BD8;
extern s32 lbl_80408BCC, lbl_80408BC0, lbl_80408BC4, lbl_80408BC8;

void fn_80079FA0(u8* menu)
{
    if (*(s32*)(menu + 0x68)) {
        s16 choice = *(s16*)(menu + 0x0a);
        *(s32*)(menu + 0x50) = lbl_80408A44;
        if (choice == 1) Menu_CloseCurrent();
        else if (lbl_80408C4C) {
            fn_80076194();
            Menu_Open(0x3ff, -1, -1);
        } else if (lbl_8040821C != 2) Menu_CloseCurrent();
        else {
            lbl_80408BD0 = 1;
            lbl_80408BD4 = 0;
            lbl_80408BD8 = 0;
            fn_80076194();
            Menu_Open(0x3f1, 0, -1);
        }
    } else if (*(s32*)(menu + 0x6c)) Menu_CloseCurrent();
}

void fn_8007A1E0(u8* menu)
{
    if (*(s32*)(menu + 0x68)) {
        s16 choice = *(s16*)(menu + 0x0a);
        *(s32*)(menu + 0x50) = lbl_80408A44;
        if (choice == 1) Menu_CloseCurrent();
        else if (lbl_80408C4C) {
            fn_80076194();
            Menu_Open(0x3ff, -1, -1);
        } else if (lbl_8040821C != 2) Menu_CloseCurrent();
        else {
            lbl_80408BC0 = 1;
            lbl_80408BC4 = 0;
            lbl_80408BC8 = 0;
            lbl_80408BCC = 0;
            fn_80076194();
            Menu_Open(0x3f6, 0, -1);
        }
    } else if (*(s32*)(menu + 0x6c)) Menu_CloseCurrent();
}

extern s32 lbl_80408C14;

void fn_80079D48(u8* menu)
{
    if (*(s32*)(menu + 0x68)) {
        *(s32*)(menu + 0x50) = lbl_80408A44;
        if (*(s16*)(menu + 0x0a) == 1) {
            fn_80076194();
            Menu_Open(0x3f8, 1, -1);
        } else if (lbl_80408C4C) {
            fn_80076194();
            Menu_Open(0x3ff, -1, -1);
        } else if (lbl_8040821C != 2) Menu_CloseCurrent();
        else {
            lbl_80408C0C = 1;
            lbl_80408C10 = 0;
            lbl_80408C14 = 0;
            fn_80076194();
            Menu_Open(0x3fc, 0, -1);
            *(s32*)(menu + 0x50) = lbl_80408A44;
        }
    } else if (*(s32*)(menu + 0x6c)) {
        fn_80076194();
        Menu_Open(0x3f8, 1, -1);
    }
}

extern f32 lbl_80408C1C, lbl_80408C30;
extern s32 lbl_80408C68, lbl_80408234;

void fn_8007A044(void)
{
    if (lbl_80408BD0) lbl_80408C1C = 1.5f;
    if (lbl_80408BD0 || lbl_80408BD4 || lbl_80408C1C > 0.0f)
        lbl_80408C30 = 1.5f;
    if (lbl_80408BD0 && lbl_80408224) {
        lbl_80408BD0 = 0;
        lbl_80408BD8 = 1;
    }
    if (lbl_80408C30 <= 0.0f) {
        if (lbl_80408BD8) Menu_CloseCurrent();
        else if (lbl_80408C68 && lbl_80408234 != -1) {
            fn_80076194();
            fn_80076194();
            Menu_Open(0x3f2, 1, -1);
        } else {
            fn_80076194();
            Menu_CloseCurrent();
        }
    }
}

extern s32 lbl_8034705C[], lbl_80408218, lbl_80408C54, lbl_80408BB4, lbl_80408A4C;

void fn_800798EC(u8* menu)
{
    s32 index = *(s16*)(menu + 8) - *(s16*)(menu + 0x0c);
    if (*(s32*)(menu + 0x68)) {
        if (*(s16*)(menu + 0x0a) == *(s16*)(menu + 0x12))
            Menu_Open(0x3f8, 1, -1);
        else if (lbl_80408C4C) Menu_Open(0x3ff, -1, -1);
        else if (lbl_8040821C != 2) Menu_OpenSpecial(menu);
        else if (lbl_8034705C[index] || lbl_80408218 >= lbl_80408C54) {
            lbl_80408BB4 = index;
            Menu_Open(0x3f0, 1, -1);
            *(s32*)(menu + 0x50) = lbl_80408A44;
        } else *(s32*)(menu + 0x50) = lbl_80408A4C;
    } else if (*(s32*)(menu + 0x6c)) Menu_Open(0x3f8, 1, -1);
}

extern f32 lbl_8019BAC0[];
extern s32 lbl_80408210;

void Menu_ConditionalClose(f32 *arg0) {
    if ((*arg0 > lbl_8019BAC0[0]) && ((s32) lbl_80408210 == 1)) {
        Menu_CloseCurrent();
    }
}

extern s32 lbl_80408A44;

void fn_80079BDC(void *arg0) {
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) {
        (*(s32 *)((s8 *)(arg0) + (0x50))) = (s32) lbl_80408A44;
        if ((s16) (*(s16 *)((s8 *)(arg0) + (0xA))) == 0) {
            Menu_CloseCurrent();
        } else {
            fn_80076194();
            Menu_CloseCurrent();
        }
    }
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0) {
        fn_80076194();
        Menu_CloseCurrent();
    }
}

extern s32 lbl_80408224;

void fn_8007A138(void *arg0) {
    if ((s32) lbl_80408224 != 0) {
        Menu_CloseCurrent();
    }
    if (((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) || ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0)) {
        Menu_CloseCurrent();
    }
}

extern f32 lbl_8019BAE0[];
extern s32 lbl_80408A34;
extern f32 lbl_80408C30;

void fn_8007A18C(void *arg0) {
    if (!(lbl_80408C30 > lbl_8019BAE0[0]) && (((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) || ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0))) {
        lbl_80408A34 = 1;
        Menu_CloseCurrent();
    }
}

extern s32 lbl_80408A44;

void fn_8007A388(void *arg0) {
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) {
        if ((s16) (*(s16 *)((s8 *)(arg0) + (0xA))) == 0) {
            fn_80076194();
        }
        goto block_4;
    }
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0) {
block_4:
        Menu_CloseCurrent();
        (*(s32 *)((s8 *)(arg0) + (0x50))) = (s32) lbl_80408A44;
    }
}

extern s32 lbl_80408A44;

void fn_8007A3E8(void *arg0) {
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) {
        if ((s16) (*(s16 *)((s8 *)(arg0) + (0xA))) == 0) {
            fn_80076194();
        }
        goto block_4;
    }
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0) {
block_4:
        Menu_CloseCurrent();
        (*(s32 *)((s8 *)(arg0) + (0x50))) = (s32) lbl_80408A44;
    }
}

s32 NuMemAlloc(s32, s32 *, s32);
extern s32 lbl_8019BB3C[];
extern s32 lbl_80408BA8;
extern s32 lbl_80408C5C;
extern s32 lbl_80408C60;
extern s32 lbl_80408C64;
extern s32 lbl_80408C68;
extern s32 lbl_80408C6C;
extern s32 lbl_80408C70;

void fn_8007ABA0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 var_r3;

    var_r3 = arg0;
    if (var_r3 > 3) {
        var_r3 = 3;
    }
    lbl_80408BA8 = var_r3;
    lbl_80408C5C = arg2;
    lbl_80408C6C = arg1;
    lbl_80408C64 = arg3;
    lbl_80408C60 = NuMemAlloc(arg3 + 4, lbl_8019BB3C, 0xDE);
    lbl_80408C68 = arg4;
    lbl_80408C70 = arg5;
}

s32 fn_80077038();
extern u8 lbl_80408A57;
extern s32 lbl_80408AA8;
extern f32 lbl_8040AF18;

void fn_800795C0(void *arg0)
{
  s8 *new_var3;
  s32 new_var4;
  s8 *new_var2;
  void *new_var;
  fn_80077038();
  new_var4 = lbl_80408AA8;
  new_var = arg0;
  new_var3 = (s8 *) arg0;
  new_var2 = new_var3 + 0x9C;
  *((f32 *) new_var2) = (f32) lbl_8040AF18;
  fn_800769B4(new_var, new_var4, lbl_80408A57);
}

extern s32 lbl_80408A38;

void fn_8007907C(void) {
    s32 var_r0;

    var_r0 = lbl_8034705C[lbl_80408BB4];
    if (var_r0 != 0) {
        var_r0 = 1;
    }
    lbl_80408A38 = var_r0;
}

s32 memset();
extern s32 gMenuDepth;

void Menu_ResetStack(void) {
    memset(gMenuStack, 0, 0x8C0);
    (*(s16 *)((s8 *)(gMenuStack) + (0x14))) = -1;
    gMenuDepth = 0;
}

extern s32 gRegisteredMenuDefinitions[];

void fn_80078E90(void *arg0) {
    s16 temp_r0;
    void *temp_r11;
    void *temp_r11_2;

    temp_r0 = (*(s16 *)((s8 *)(arg0) + (0x14)));
    if (temp_r0 != -1) {
        temp_r11 = (temp_r0 * 0x1C) + ((s8 *)gRegisteredMenuDefinitions);
        if ((s16) (*(s16 *)((s8 *)(temp_r11) + (0x14))) != -1) {
            (*(s16 *)((s8 *)(temp_r11) + (0x14))) = (s16) (*(u16 *)((s8 *)(arg0) + (8)));
        }
        temp_r11_2 = ((*(s16 *)((s8 *)(arg0) + (0x14))) * 0x1C) + ((s8 *)gRegisteredMenuDefinitions);
        if ((s16) (*(s16 *)((s8 *)(temp_r11_2) + (0x16))) != -1) {
            (*(s16 *)((s8 *)(temp_r11_2) + (0x16))) = (s16) (*(u16 *)((s8 *)(arg0) + (0xA)));
        }
    }
}

#define Menu_CloseCurrent ((s32 (*)())Menu_CloseCurrent)
extern f32 lbl_8019BAD4[];
extern f32 lbl_80408C40;

void fn_80079F68(void) {
    if ((lbl_80408C40 <= lbl_8019BAD4[0])) {
        Menu_CloseCurrent();
    }
}
#undef Menu_CloseCurrent
