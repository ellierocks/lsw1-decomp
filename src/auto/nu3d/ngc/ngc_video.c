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
