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

s32 fn_80013890(void *arg0, void *arg1) {
    s32 temp_r9;
    s32 temp_r9_2;

    temp_r9 = (*(s32 *)((s8 *)(arg0) + (4)));
    if ((temp_r9 != 0) && (temp_r9 == (s32) (*(s32 *)((s8 *)(arg1) + (4))))) {
        return 1;
    }
    temp_r9_2 = (*(s32 *)((s8 *)(arg0) + (8)));
    if (temp_r9_2 != 0 && temp_r9_2 == (s32) (*(s32 *)((s8 *)(arg1) + (8)))) {
        return 1;
    }
    return 0;
}

void *NuMemAlloc(s32, s32 *, s32);
extern s32 lbl_8018FC1C[];

void fn_80015C88(s32 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_r0;
    s32 var_r3;

    if (arg0 != NULL) {
        var_r3 = (*arg0 + 3) & 0xFFFFFFFC;
        *arg0 = var_r3 + 0xEC;
    } else {
        var_r3 = NuMemAlloc(0xEC, lbl_8018FC1C, 0x1BD);
    }
    if (var_r3 != 0) {
        (*(s32 *)((s8 *)(var_r3) + (0))) = arg3;
        (*(s32 *)((s8 *)(var_r3) + (4))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0x88))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xCC))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xD0))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xD4))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xD8))) = 0;
        (*(s32 *)((s8 *)(var_r3) + (0xDC))) = arg1;
        (*(s32 *)((s8 *)(var_r3) + (0xE0))) = arg2;
        (*(s32 *)((s8 *)(var_r3) + (0xE4))) = 0;
        if (arg0 != NULL) {
            var_r0 = (*(s32 *)((s8 *)(var_r3) + (0xE8))) & 0x7FFFFFFF;
        } else {
            var_r0 = (*(s32 *)((s8 *)(var_r3) + (0xE8))) | 0x80000000;
        }
        (*(s32 *)((s8 *)(var_r3) + (0xE8))) = var_r0;
    }
}
