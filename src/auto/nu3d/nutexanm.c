#include "types.h"

extern u8* lbl_8040A2F8;
extern s32 NuFParGetInt(void);

extern s32 lbl_80407BE0;
extern s32 lbl_80407BDC;
extern s32 lbl_8040AD2C;
extern s32 lbl_8040AD28;
extern s32 lbl_8040AD24;
extern s32 lbl_8040AD20;
extern s32 lbl_8040AD18;

extern u32 lbl_801B14D8[];

extern u32 lbl_8040A304;
extern u32 lbl_8040A300;
extern u32 lbl_8040A2FC;

void NuTexAnimSetSignals(u32 s)
{
    u32 old = lbl_8040A304;
    u32 x;
    lbl_8040A304 = s;
    x = s ^ old;
    lbl_8040A2FC = x & s;
    lbl_8040A300 = x & ~s;
}

void fn_80014248(u32 v)
{
    lbl_801B14D8[0] = v & 0xff;
    lbl_801B14D8[1] = (v >> 8) & 0xff;
    lbl_801B14D8[2] = (v >> 16) & 0xff;
    lbl_801B14D8[3] = v >> 24;
}

void fn_80014214(s32 a, s32 b)
{
    lbl_80407BE0 = b * lbl_8040AD2C / lbl_8040AD24 - lbl_8040AD18;
    lbl_80407BDC = a * lbl_8040AD28 / lbl_8040AD20;
}

void fn_80015F7C(void)
{
    s32 v = NuFParGetInt();
    *(s16*)((u8*)lbl_8040A2F8 + 0x1b8) = v;
}
