#include "types.h"

extern void fn_80032CB4(u8* p);

extern u8 lbl_8040A404[4];

extern s32 lbl_8040A3C4[2];

extern s32 lbl_80407F94;
extern s32 lbl_80407F5C;

extern s32 lbl_8021FCB0[];

extern s32 lbl_8040A418;
extern s32 lbl_8040A414;

extern s32 lbl_80407FAC;

extern f32 lbl_8040A3FC;
extern f32 lbl_8040A400;

extern s32 lbl_8040A3D8;

extern u16 lbl_803404DA[];

extern u16 lbl_803404D8[];

extern f32 lbl_80407F78;

extern s32 lbl_80407F6C;

extern s32 lbl_8040A3F0;

extern s32 lbl_8040A3E8;

s32 fn_80037218(void)
{
    return lbl_8040A3E8;
}

s32 fn_80037220(void)
{
    return lbl_8040A3F0;
}

void fn_800372D4(s32 a)
{
    lbl_80407F6C = a;
}

void fn_800372E8(s32 a)
{
    lbl_8040A3E8 = a;
}

void fn_8003735C(f32 a)
{
    lbl_80407F78 = a;
}

u16 fn_80037200(void)
{
    return lbl_803404D8[0];
}

u16 fn_8003720C(void)
{
    return lbl_803404DA[0];
}

void fn_800372DC(void)
{
    lbl_8040A3D8 = 0;
}

void fn_8003777C(f32 a, f32 b)
{
    lbl_8040A3FC = a;
    lbl_8040A400 = b;
}

void fn_80037B74(void)
{
    lbl_80407FAC = 1;
}

void fn_8003776C(void)
{
    s32 z = 0;
    lbl_8040A414 = z;
    lbl_8040A418 = z;
}

s32 fn_80033FF0(s32 i)
{
    return lbl_8021FCB0[i - 1];
}

void fn_80037170(void)
{
    if (lbl_80407F94 != 0) {
        lbl_80407F5C = 1;
    }
}

void fn_80033FA8(s32 i)
{
    if (i != -1) {
        lbl_8021FCB0[i] = 0;
    }
}

s32 fn_80034B98(s32 i)
{
    if (lbl_8040A3C4[i] == -1) {
        return 0;
    }
    return 1;
}

void fn_80037788(u32 v)
{
    u8* b = lbl_8040A404;
    b[0] = v;
    b[1] = v >> 8;
    b[2] = v >> 16;
    b[3] = v >> 24;
}

void fn_80033FC8(u8* p)
{
    *(s32*)(p + 0xf4) = -1;
    fn_80032CB4(p);
}

s32 GXReadDrawSync();
s32 fn_800165B4(s32, s32);
s32 fn_80016660(s32, s32, s32 *);
s32 fn_800166C4(s32, s32);
extern s32 lbl_80192660[];
extern s32 lbl_80192664[];
extern s32 lbl_8040AD9C;

void fn_80037188(void) {
    fn_800166C4(lbl_8040AD9C, 0);
    do {

    } while (GXReadDrawSync() != 0xB000);
    fn_80016660(lbl_8040AD9C, 0, lbl_80192660);
    fn_800165B4(lbl_8040AD9C, 2);
    fn_80016660(lbl_8040AD9C, 2, lbl_80192664);
}

s32 GXSetProjectionv(s32);
extern s32 lbl_80407FA8;
extern s32 lbl_80407FAC;

void fn_80037B80(s32 arg0) {
    fn_8003776C();
    if (((s32) lbl_80407FAC != 0) || (arg0 != (s32) lbl_80407FA8)) {
        GXSetProjectionv(arg0);
        lbl_80407FA8 = arg0;
        lbl_80407FAC = 0;
    }
}

s32 (*fn_80009948(s32 *, s32))();
extern s32 GXEurgb60Hz480IntDf[];
extern s32 GXMpal480IntDf[];
extern s32 GXNtsc480IntDf[];
extern s32 GXPal264Int[];
extern s32 GXPal528IntDf[];
extern s32 lbl_8019253C[];
extern s32 lbl_80192678[];
extern s32 lbl_8040A3CC;

s32 fn_80037228(void) {
    if (((s32) lbl_8040A3CC == GXNtsc480IntDf) || ((s32) lbl_8040A3CC == GXEurgb60Hz480IntDf) || ((s32) lbl_8040A3CC == GXMpal480IntDf)) {
        return 0x3C;
    }
    if (((s32) lbl_8040A3CC == GXPal528IntDf) || ((s32) lbl_8040A3CC == GXPal264Int)) {
        return 0x32;
    }
    fn_80009948(lbl_8019253C, 0x64E)(lbl_80192678);
    return 0;
}

s32 fn_800165B4(s32, s32);
s32 fn_80016660(s32, s32, s32 *);
s32 fn_800166C4(s32, s32);
extern s32 lbl_80192598[];

void fn_800372F0(s32 arg0) {
    if (arg0 == 0xB001) {
        fn_800165B4(lbl_8040AD9C, 1);
        fn_80016660(lbl_8040AD9C, 1, lbl_80192598);
        return;
    } else if (arg0 == 0xB000) {
        fn_800166C4(lbl_8040AD9C, 1);
        return;
    }
}
