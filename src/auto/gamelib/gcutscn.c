#include "types.h"

extern s32 lbl_804081EC;
extern s32 lbl_8040A42C;
extern s32 lbl_8040A428;
extern s32 lbl_804081B8;
extern s32 lbl_804081C0;
extern s32 lbl_804081C4;
extern s32 lbl_804081CC;
extern s32 lbl_804081D0;
extern s32 lbl_804081DC;

s32 fn_8003A214(void)
{
    return 0;
}

s32 fn_8003A21C(void)
{
    return 0;
}

s32 fn_8003A224(void)
{
    return 0;
}

s32 fn_8003A22C(void)
{
    return 0;
}

s32 fn_8003A234(void)
{
    return 0;
}

s32 fn_8003A23C(void)
{
    return 0;
}

void fn_8003F178(s32 a)
{
    lbl_804081B8 = a;
}

void fn_8003F180(s32 a)
{
    lbl_804081C0 = a;
}

void fn_8003F188(s32 a)
{
    lbl_804081C4 = a;
}

void fn_8003F190(s32 a)
{
    lbl_804081CC = a;
}

void fn_8003F198(s32 a)
{
    lbl_804081D0 = a;
}

void fn_8003F1A0(s32 a)
{
    lbl_804081DC = a;
}

void NuGCutSceneSysInit(s32 a)
{
    lbl_804081EC = a;
    lbl_8040A428 = 0;
    lbl_8040A42C = 0;
}
