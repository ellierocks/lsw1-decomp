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

void AddChunkControlToStack(void *arg0, void **arg1) {
    s32 temp_r0;
    void **var_r4;
    void *temp_r9;

    var_r4 = arg1;
    if ((void *) *var_r4 != NULL) {
        do {
            temp_r9 = *var_r4;
            temp_r0 = (*(s32 *)((s8 *)(temp_r9) + (0x10)));
            var_r4 = temp_r9 + 0x10;
        } while (temp_r0 != 0);
    }
    *var_r4 = arg0;
    (*(s32 *)((s8 *)(arg0) + (0x10))) = 0;
}

void FullReflect(void *arg0, void *arg1, void *arg2) {
    f32 temp_f13;
    f32 temp_f8;
    f32 temp_f9;

    temp_f8 = (*(f32 *)((s8 *)(arg1) + (0)));
    temp_f13 = (*(f32 *)((s8 *)(arg0) + (0)));
    temp_f9 = ((*(f32 *)((s8 *)(arg0) + (8))) * -(*(f32 *)((s8 *)(arg1) + (8)))) + ((temp_f13 * -temp_f8) + ((*(f32 *)((s8 *)(arg0) + (4))) * -(*(f32 *)((s8 *)(arg1) + (4)))));
    (*(f32 *)((s8 *)(arg2) + (0))) = (f32) ((2.0f * (temp_f13 * temp_f9)) + temp_f8);
    (*(f32 *)((s8 *)(arg2) + (4))) = (f32) ((2.0f * ((*(f32 *)((s8 *)(arg0) + (4))) * temp_f9)) + (*(f32 *)((s8 *)(arg1) + (4))));
    (*(f32 *)((s8 *)(arg2) + (8))) = (f32) ((2.0f * ((*(f32 *)((s8 *)(arg0) + (8))) * temp_f9)) + (*(f32 *)((s8 *)(arg1) + (8))));
}

void fn_8005A6C4(s32 arg0, s32 arg1) {
    if ((arg0 >= 0) && (arg0 < (s32) (*(s32 *)((s8 *)(lbl_8040AE4C) + (0xA59C))))) {
        (*(s32 *)((s8 *)(((arg0 * 0x6C) + (*(s32 *)((s8 *)(lbl_8040AE4C) + (0x68))))) + (0x40))) = arg1;
    }
}

void fn_8005A7F8(s32 arg0, s32 *arg1, s32 *arg2) {
    s32 temp_r11;

    if (arg0 >= 0) {
        temp_r11 = arg0 * 0x6C;
        *arg1 = (*(s32 *)((s8 *)(lbl_8040AE4C) + (0x68))) + temp_r11;
        *arg2 = (*(s32 *)((s8 *)((temp_r11 + (*(s32 *)((s8 *)(lbl_8040AE4C) + (0x68))))) + (0x40)));
    }
}

extern s32 lbl_80408404;

void fn_80063D78(s32 *arg0) {
    s32 temp_r3;

    temp_r3 = *arg0;
    if (temp_r3 != -1) {
        (*(s8 *)((s8 *)(((temp_r3 * 0x2EC) + lbl_80408404)) + (0x274))) = 0;
    }
}

s32 fn_8005EF6C(s32);
extern s32 lbl_80408404;

void fn_80064888(s32 arg0) {
    fn_8005EF6C(lbl_80408404 + (arg0 * 0x2EC));
}

extern s32 lbl_80408404;

void fn_800648D8(s32 arg0, s16 arg1) {
    if (arg0 != -1) {
        (*(s16 *)((s8 *)(((arg0 * 0x2EC) + lbl_80408404)) + (0x270))) = arg1;
    }
}

s32 fn_8005A8E4();

void fn_80064CD4(void) {
    fn_8005A8E4();
}

s32 NuScratchAlloc32(s32);
s32 NuScratchRelease();
extern s32 lbl_8040A588;
extern s32 lbl_8040A5A4;
extern s32 lbl_8040A5C4;
extern s32 lbl_8040A5E4;
extern s32 lbl_8040A5E8;
extern s32 lbl_8040AE4C;
extern s32 lbl_8040AE5C;

void fn_8005A560(void)
{
  int new_var;
 do { lbl_8040A5E4 = 0; new_var = 0; lbl_8040A588 = new_var; lbl_8040A5E8 = new_var; lbl_8040A5A4 = 0; lbl_8040AE5C = NuScratchAlloc32(0x948); NuScratchRelease(); } while (0);
  lbl_8040A5C4 = new_var;
  lbl_8040AE4C = 0;
}

void fn_80064C54(void *arg0, void **arg1)
{
  void *temp_r9;
  void *temp_r9_2;
  void *temp_r11;
  temp_r11 = *((void **) (((s8 *) arg0) + 0x14C));
  if (temp_r11 == ((void *) 0))
  {
    temp_r9 = *((void **) (((s8 *) arg0) + 0x148));
    *arg1 = temp_r9;
    if (temp_r9 != ((void *) 0))
    {
      *((void **) (((s8 *) temp_r9) + 0x14C)) = temp_r11;
    }
  }
  else
  {
    *((void **) (((s8 *) temp_r11) + 0x148)) = (void *) (*((void **) (((s8 *) arg0) + 0x148)));
    temp_r9_2 = *((void **) (((s8 *) arg0) + 0x148));
    if (temp_r9_2 != ((void *) 0))
    {
      *((void **) (((s8 *) temp_r9_2) + 0x14C)) = (void *) (*((void **) (((s8 *) arg0) + 0x14C)));
    }
  }
  *((void **) (((s8 *) arg0) + 0x14C)) = (void *) 0;
  *((void **) (((s8 *) arg0) + 0x148)) = (void *) 0;
}

extern f32 lbl_80195740[];
extern s32 lbl_80408404;

void fn_80064830(s32 arg0, f32 farg0, f32 farg1, f32 farg2)
{
  void *temp_r11;
  if (arg0 != (-1))
  {
 do { temp_r11 = (arg0 * 0x2EC) + lbl_80408404; *((f32 *) (((s8 *) temp_r11) + 0x280)) = (f32) lbl_80195740[0]; *((f32 *) (((s8 *) temp_r11) + 0x110)) = farg0; } while (0);
    *((f32 *) (((s8 *) temp_r11) + 0x114)) = farg1;
    *((f32 *) (((s8 *) temp_r11) + 0x118)) = farg2;
  }
}

void GenDebMomAdjFromPos(s32 pad0, s32 pad1, void *arg2) {
    (*(f32 *)((s8 *)(arg2) + (0x10))) = (f32) (2.0f * (*(f32 *)((s8 *)(arg2) + (0))));
    (*(f32 *)((s8 *)(arg2) + (0x18))) = (f32) (2.0f * (*(f32 *)((s8 *)(arg2) + (8))));
}

void GenDebMomAdjFromPosRev(s32 pad0, s32 pad1, void *arg2) {
    (*(f32 *)((s8 *)(arg2) + (0x10))) = (f32) -(*(f32 *)((s8 *)(arg2) + (0)));
    (*(f32 *)((s8 *)(arg2) + (0x18))) = (f32) -(*(f32 *)((s8 *)(arg2) + (8)));
}

s32 fn_80027510(s32, s32, s32);
s32 fn_800275E0(s32);
s32 fn_80043F9C(s32);
s32 fn_80060F58(s32, s32);
extern s32 lbl_80408428;
extern s32 lbl_8040AE94;

void fn_80064984(void) {
    s32 saved_reg_r31;
    s32 var_r9;

    if ((s32) lbl_80408428 != 0) {
        fn_80027510(-1, -2, 1);
        var_r9 = 0;
        if ((s32) lbl_8040AE94 > 0) {
            var_r9 = 1;
        }
        if (var_r9 != 0) {
            fn_80060F58(0, 2);
        }
        fn_80043F9C(saved_reg_r31);
        fn_800275E0(0);
    }
}

extern s16 lbl_8040A5D4;
#define lbl_8040AE4C (*(s32 *(*))&lbl_8040AE4C)

s16 fn_8005A43C(void) {
    void *temp_r11;

    if ((s16) lbl_8040A5D4 != -1) {
        temp_r11 = (lbl_8040A5D4 * 0x38) + *lbl_8040AE4C;
        if ((s32) (*(s32 *)((s8 *)(temp_r11) + (0x2C))) == 1) {
            return (*(s16 *)((s8 *)(temp_r11) + (0x30)));
        }
    }
    return -1;
}
#undef lbl_8040AE4C

s32 fn_8005FF74(s32, void *, s32, s16, s16, void *);

void fn_80063E40(s32 arg0, void *arg1, s32 arg2, s16 arg3, s16 arg4) {
    fn_8005FF74(arg0, arg1, arg2, arg3, arg4, 0);
}

s32 fn_800631DC(void *, s8, s32);

void fn_80064A04(void *arg0, s8 arg1) {
    fn_800631DC(arg0, arg1, 0);
}

s32 fn_800631DC(void *, s8, s32);

void fn_80064A28(void *arg0) {
    fn_800631DC(arg0, 0, 0);
}

s32 fn_8005F114(void *, s32, void *, void *, void *, s32);

void fn_80063E14(void *arg0, s32 arg1, void *arg2, s32 arg3) {
    fn_8005F114(arg0, arg1, arg2, 0, 0, arg3);
}

s32 TerrainInitEx(s32, void *, void *, s32, void *, void *, s32, s32, s32, s32);

void fn_8005A3F8(s32 arg0, void *arg1, void *arg2, s32 arg3, void *arg4, void *arg5, s32 arg6) {
    s32 sp8;
    s32 spC;

    TerrainInitEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6, 0x1000, 0xC00, 0x100);
}

void fn_80064860(s32 arg0, s16 arg1, s16 arg2, f32 farg0, f32 farg1) {
    void *temp_r9;

    if (arg0 != -1) {
        temp_r9 = (arg0 * 0x2EC) + lbl_80408404;
        (*(s16 *)((s8 *)(temp_r9) + (0x25C))) = arg1;
        (*(s16 *)((s8 *)(temp_r9) + (0x25E))) = arg2;
        (*(f32 *)((s8 *)(temp_r9) + (0x260))) = farg0;
        (*(f32 *)((s8 *)(temp_r9) + (0x264))) = farg1;
    }
}

void fn_800648B4(s32 arg0, s32 arg1, s32 arg2, f32 farg0) {
    void *temp_r9;

    if (arg0 != -1) {
        temp_r9 = (arg0 * 0x2EC) + lbl_80408404;
        (*(s32 *)((s8 *)(temp_r9) + (0x250))) = arg1;
        (*(s32 *)((s8 *)(temp_r9) + (0x254))) = arg2;
        (*(f32 *)((s8 *)(temp_r9) + (0x258))) = farg0;
    }
}

struct DebBits_8005A6F4 { u32 flag : 1; u32 rest : 31; };

void fn_8005A6F4(s32 arg0, s32 arg1) {
    if ((arg0 >= 0) && (arg0 < (*(s32 *)((s8 *)(lbl_8040AE4C) + (0xA59C))))) {
        ((struct DebBits_8005A6F4 *)((s8 *)((arg0 * 0x6C) + (*(s32 *)((s8 *)(lbl_8040AE4C) + (0x68)))) + 0x4C))->flag = arg1;
    }
}

extern s32 lbl_804083F0;
s32 fn_8005EB80(s32 *);
void fn_80063D9C(void) {
    s32 sp8;
    s32 i;

    for (i = 0; i < lbl_804083F0; i++) {
        s8 *p = (s8 *)(i * 0x2EC + lbl_80408404);
        if (*(s16 *)(p + 0x102) != 0 && *(u8 *)(p + 0x279) != 0) {
            sp8 = i;
            fn_8005EB80(&sp8);
        }
    }
}

extern const f32 lbl_80195420[];

void fn_8005A72C(s32 arg0, f32 farg0, f32 farg1, f32 farg2) {
    if (arg0 >= 0 && arg0 < *(s32 *)((s8 *)lbl_8040AE4C + 0xA59C)) {
        s8 *g = (s8 *)lbl_8040AE4C;
        if (*(f32 *)(arg0 * 0x6C + *(s8 **)(g + 0x68) + 0x54) == lbl_80195420[0]) {
            *(f32 *)(arg0 * 0x6C + *(s8 **)(g + 0x68) + 0x58) = lbl_80195420[0];
            *(f32 *)(arg0 * 0x6C + *(s8 **)(g + 0x68) + 0x5C) = lbl_80195420[0];
        }
        g = (s8 *)lbl_8040AE4C;
        *(f32 *)(arg0 * 0x6C + *(s8 **)(g + 0x68) + 0x54) = farg0;
        *(f32 *)(arg0 * 0x6C + *(s8 **)(g + 0x68) + 0x60) = farg1;
        *(f32 *)(arg0 * 0x6C + *(s8 **)(g + 0x68) + 0x64) = farg2;
    }
}

s32 NuAnimCurveSetApplyToJointBasic(s32, s32, f32, f32, s32);
extern s32 lbl_8040AEA0;
extern s32 lbl_8040AEA4;

s32 fn_80064A94(s32 arg0, f32 farg0, f32 farg1) {
    s32 var_r30;
    s32 var_r31;

    var_r30 = 0;
    if (var_r30 < (s32) lbl_8040AEA4) {
        var_r31 = 0;
loop_2:
        if (((*(s32 *)((s8 *)((var_r31 + lbl_8040AEA0)) + (4))) & 3) && (NuAnimCurveSetApplyToJointBasic(*(s32 *)(var_r31 + lbl_8040AEA0), arg0, farg0, farg1, 3) != 0)) {
            return *(s32 *)(var_r31 + lbl_8040AEA0);
        }
        var_r30 += 1;
        var_r31 += 8;
        if (var_r30 >= (s32) lbl_8040AEA4) {
            goto block_6;
        }
        goto loop_2;
    }
block_6:
    return -1;
}

s32 fn_80063834(s32, void *, f32, f32, u8);
extern s32 lbl_8040AE9C;
extern s32 lbl_8040AEA8;

s32 fn_80064B40(s32 arg0, f32 farg0, f32 farg1) {
    s32 var_r30;
    s32 var_r31;

    var_r30 = 0;
    if (var_r30 < (s32) lbl_8040AEA8) {
        var_r31 = 0;
loop_2:
        if (((*(s32 *)((s8 *)((var_r31 + lbl_8040AE9C)) + (4))) & 3) && (fn_80063834(*(s32 *)(var_r31 + lbl_8040AE9C), arg0, farg0, farg1, 3) != 0)) {
            return *(s32 *)(var_r31 + lbl_8040AE9C);
        }
        var_r30 += 1;
        var_r31 += 8;
        if (var_r30 >= (s32) lbl_8040AEA8) {
            goto block_6;
        }
        goto loop_2;
    }
block_6:
    return -1;
}

s32 fn_80027348();
s32 fn_8005F254(s32, s32, s32, s32, s32, s32, s32, s32, f32);
extern f32 lbl_8019570C[];
extern f32 lbl_80195710[];
extern s32 lbl_804083FC;

void fn_80063F80(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 var_r31;
    u8 temp_r0;
    void *temp_r9;

    if ((arg0 >= 0) && (temp_r9 = *(s32 *)((arg0 * 4) + lbl_804083FC), ((temp_r9 == NULL) == 0))) {
        temp_r0 = (*(u8 *)((s8 *)(temp_r9) + (0x2E)));
        switch ((s8) temp_r0) {                     /* irregular */
        case 2:
            var_r31 = 0xC350;
            break;
        case 3:
            var_r31 = 0x9C40;
            break;
        case 7:
            var_r31 = 0x4E20;
            break;
        case 0:
        default:
            var_r31 = 0x7530;
            break;
        }
        if (fn_80027348() == 0) {
            fn_8005F254(arg0, arg1, arg2, arg3 * 0x32, arg4, arg5, var_r31, 0, lbl_8019570C[0]);
            return;
        }
        fn_8005F254(arg0, arg1, arg2, arg3 * 0x3C, arg4, arg5, var_r31, 0, lbl_80195710[0]);
    }
}

extern s32 lbl_8040A5AC;

s32 fn_8005A5C8(void) {
    if (lbl_8040A5AC != NULL) {
        return *(u8 *)((s8 *)lbl_8040A5AC + 0x60);
    }
    return -1;
}

extern s32 lbl_8040A5A8;

s32 fn_8005A5E0(void) {
    if (lbl_8040A5A8 != NULL) {
        return *(u8 *)((s8 *)lbl_8040A5A8 + 0x61);
    }
    return -1;
}

s32 fn_8005A5B0(void) {
    if (lbl_8040A5A4 != NULL) {
        return *(u8 *)((s8 *)lbl_8040A5A4 + 0x60);
    }
    return -1;
}
