#include "types.h"

extern s32 lbl_804083B0;
extern s32 lbl_8040A604;
extern s32 lbl_8040A608;

void HVQM4PlayerExBufvSetBusyFlag(u8* a, s32 b)
{
    *(s32*)(a + 20) = b;
}

void fn_8005A320(s32 a, u8* b, s32 c)
{
    lbl_804083B0 = a;
    lbl_8040A604 = b;
    *(s32*)b = 0;
    lbl_8040A608 = c;
}

void HVQM4PlayerEx1stFrameFunc(u8* a, s32 b, s32 c)
{
    *(s32*)(a + 988) = b;
    *(s32*)(a + 992) = c;
}

s32 HVQM4PlayerExCheckExecute(u8* a)
{
    return *(s32*)(a + 928);
}

s32 HVQM4PlayerExGetMicroSecPerFrame(u8* a)
{
    return *(s32*)(a + 40);
}

s32 HVQM4PlayerExGetAudioSamples(u8* a)
{
    return *(s32*)(a + 68);
}

s32 fn_8004ACE0(u8* a)
{
    return *(s32*)(a + 16);
}

s32 fn_8004ADAC(u8* a)
{
    return *(s32*)(a + 12);
}

s32 fn_8004ADB4(u8* a)
{
    return *(s32*)(a + 8);
}

s32 HVQM4PlayerExControlExit(u8* a)
{
    *(s32*)(a + 956) = 1;
    *(s32*)(a + 944) = 1;
    return 1;
}

s32 fn_8004BCB0(u8* a)
{
    return *(s32*)(a + 84);
}

s32 OSDisableInterrupts();
s32 OSRestoreInterrupts();

s32 HVQM4BufaChkSend(void *arg0) {
    s32 temp_r29;

    OSDisableInterrupts();
    temp_r29 = (*(s32 *)((s8 *)(arg0) + (8)));
    OSRestoreInterrupts();
    return temp_r29;
}

s32 OSDisableInterrupts();
s32 OSRestoreInterrupts();

s32 HVQM4BufaGetSendNums(void *arg0) {
    s32 var_r31;
    void *var_r9;

    var_r31 = 0;
    OSDisableInterrupts();
    var_r9 = (*(void **)((s8 *)(arg0) + (8)));
    if (var_r9 != NULL) {
        do {
            var_r9 = (*(void **)((s8 *)(var_r9) + (4)));
            var_r31 += 1;
        } while (var_r9 != NULL);
    }
    OSRestoreInterrupts();
    return var_r31;
}

s32 OSDisableInterrupts();
s32 OSRestoreInterrupts();

s32 HVQM4BufvChkDisp(void *arg0) {
    s32 temp_r29;

    OSDisableInterrupts();
    temp_r29 = (*(s32 *)((s8 *)(arg0) + (4)));
    OSRestoreInterrupts();
    return temp_r29;
}

s32 OSDisableInterrupts();
s32 OSRestoreInterrupts();

s32 HVQM4BufvGetDispNums(void *arg0) {
    s32 var_r31;
    void *var_r9;

    var_r31 = 0;
    OSDisableInterrupts();
    var_r9 = (*(void **)((s8 *)(arg0) + (4)));
    if (var_r9 != NULL) {
        do {
            var_r9 = (*(void **)((s8 *)(var_r9) + (8)));
            var_r31 += 1;
        } while (var_r9 != NULL);
    }
    OSRestoreInterrupts();
    return var_r31;
}

s32 OSDisableInterrupts();
s32 OSRestoreInterrupts();

void HVQM4BufvSetFree(s32 **arg0, s32 *arg1) {
    s32 *temp_r11;
    s32 *temp_r9;

    OSDisableInterrupts();
    temp_r11 = *arg0;
    if (temp_r11 == NULL) {
        (*(s32 **)((s8 *)(arg1) + (0))) = arg1;
        (*(s32 **)((s8 *)(arg1) + (4))) = arg1;
        *arg0 = arg1;
    } else {
        temp_r9 = (*(s32 **)((s8 *)(temp_r11) + (4)));
        (*(s32 **)((s8 *)(temp_r11) + (4))) = arg1;
        (*(s32 **)((s8 *)(arg1) + (0))) = temp_r11;
        (*(s32 **)((s8 *)(arg1) + (4))) = temp_r9;
        *temp_r9 = arg1;
    }
    OSRestoreInterrupts();
}

s32 HVQM4BufaSetFree(s32);

void HVQM4PlayerExBufaSetFree(void *arg0) {
    HVQM4BufaSetFree((*(s32 *)((s8 *)(arg0) + (0x68))));
}

s32 OSJoinThread(s32, s32 *);
s32 fn_8004A578(s32, s32 *);
extern s32 lbl_80408378;

s32 HVQM4PlayerExClose(s32 arg0, s32 *arg1) {
    s32 sp8;
    s32 var_r29;

    OSJoinThread(arg0 + 0x78, &sp8);
    var_r29 = 1;
    if (fn_8004A578(arg0, arg1) == 0) {
        var_r29 = 0;
    }
    if ((s32) lbl_80408378 != 0) {
        if (arg1 != NULL) {
            *arg1 = 0xD;
        }
        var_r29 = 0;
    }
    return var_r29;
}

s32 OSResumeThread(s32);

s32 HVQM4PlayerExStart(s32 arg0) {
    OSResumeThread(arg0 + 0x78);
    return 1;
}

s32 fn_80049A10();

s32 fn_8004B088(void) {
    fn_80049A10();
    return 1;
}

s32 fn_80049A10();

s32 fn_8004B3EC(void) {
    fn_80049A10();
    return 1;
}

s32 DVDGetCommandBlockStatus();

void fn_8004BCB8(void) {
    DVDGetCommandBlockStatus();
}

s32 DVDGetCommandBlockStatus();

s32 fn_8004BCD8(void *arg0) {
    s32 (*temp_r0)(s32, s32, s32);
    s32 temp_r3;

    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x64))) == 0) {
        temp_r3 = DVDGetCommandBlockStatus();
        if (temp_r3 != (s32) (*(s32 *)((s8 *)(arg0) + (0x4C)))) {
            temp_r0 = (*(s32 (**)(s32, s32, s32))((s8 *)(arg0) + (0x6C)));
            (*(s32 *)((s8 *)(arg0) + (0x4C))) = temp_r3;
            if (temp_r0 != NULL) {
                temp_r0((*(s32 *)((s8 *)(arg0) + (0x70))), (*(s32 *)((s8 *)(arg0) + (0x74))), temp_r3);
            }
        }
        return 1;
    }
    return 0;
}

s32 DVDGetCommandBlockStatus();

void fn_8004BDD0(void *arg0) {
    s32 (*temp_r0)(s32, s32, s32);
    s32 temp_r3;

    temp_r3 = DVDGetCommandBlockStatus();
    if (temp_r3 != (s32) (*(s32 *)((s8 *)(arg0) + (0x4C)))) {
        temp_r0 = (*(s32 (**)(s32, s32, s32))((s8 *)(arg0) + (0x6C)));
        (*(s32 *)((s8 *)(arg0) + (0x4C))) = temp_r3;
        if (temp_r0 != NULL) {
            temp_r0((*(s32 *)((s8 *)(arg0) + (0x70))), (*(s32 *)((s8 *)(arg0) + (0x74))), temp_r3);
        }
    }
}

f32 NuASin(s32);
f32 NuAngDiff(s32);
extern void *lbl_8040AE5C;

void fn_8004F590(void *arg0)
{
  f32 temp_f11;
  f32 *new_var3;
  f32 temp_f12;
  f32 temp_f13;
  f32 temp_f13_2;
  f32 temp_f1;
  f32 temp_f29;
  f32 *new_var;
  s8 *new_var2;
  f32 temp_f30;
  f32 temp_f31;
  new_var2 = (s8 *) arg0;
  temp_f29 = NuAngDiff((s32) (-(*((f32 *) (((s8 *) lbl_8040AE5C) + 0x44)))));
  temp_f30 = NuASin((s32) (-(*((f32 *) (((s8 *) lbl_8040AE5C) + 0x44)))));
  temp_f31 = NuAngDiff((s32) (-(*((f32 *) (((s8 *) lbl_8040AE5C) + 0x48)))));
  temp_f1 = NuASin((s32) (-(*((f32 *) (((s8 *) lbl_8040AE5C) + 0x48)))));
  temp_f11 = (*((f32 *) (((s8 *) arg0) + 0))) - (*((f32 *) (((s8 *) lbl_8040AE5C) + 0x1C)));
  temp_f12 = *((f32 *) (new_var2 + 4));
  temp_f13 = (*((f32 *) (new_var2 + 8))) - (*((f32 *) (((s8 *) lbl_8040AE5C) + 0x24)));
  new_var3 = (f32 *) (((s8 *) lbl_8040AE5C) + 0x20);
  new_var = (f32 *) (((s8 *) lbl_8040AE5C) + 0x50);
  if (!temp_f11)
  {
  }
  *((f32 *) (((s8 *) arg0) + 0)) = (f32) ((temp_f13 * temp_f31) + (temp_f11 * temp_f1));
  temp_f13_2 = (temp_f13 * temp_f1) - (temp_f11 * temp_f31);
  *((f32 *) (((s8 *) arg0) + 8)) = (f32) ((((temp_f12 + (*new_var)) - (*new_var3)) * temp_f29) + (temp_f13_2 * temp_f30));
  *((f32 *) (((s8 *) arg0) + 4)) = (f32) ((((temp_f12 + (*new_var)) - (*new_var3)) * temp_f30) - (temp_f13_2 * temp_f29));
}

s32 HVQM4PlayerExControlCheckNormal(void *arg0) {
    return (*(s32 *)((s8 *)(arg0) + (0x3A8))) == 0;
}
