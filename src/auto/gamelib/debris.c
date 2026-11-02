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
