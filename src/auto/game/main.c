#include "types.h"

extern s32 lbl_80408E84;
extern s32 lbl_8040A624;

void fn_8009CF2C(void)
{

}

void fn_8009CFF0(void)
{

}

void fn_8009CFF4(void)
{

}

s32 fn_8009CFF8(void)
{
    return 0;
}

s32 fn_8009D158(void)
{
    return lbl_8040A624;
}

void fn_8009D238(void)
{

}

s32 fn_8009D414(void)
{
    return lbl_80408E84;
}

void fn_8009D540(void)
{
    lbl_80408E84 = (lbl_80408E84 + -1);
}

s32 fn_800A1F94(void)
{
    return 0;
}

extern f32 lbl_8019EA00[];
extern f32 lbl_8019EA04[];
extern f32 lbl_8019EA08[];
extern f32 lbl_80408E9C;
extern f32 lbl_80408F00;
extern f32 lbl_80408F04;
extern s32 lbl_80408F08;
extern s32 lbl_80408F14;

void fn_8009DC18(void) {
    f32 temp_f0;
    f32 temp_f11;
    s32 var_r0;

    temp_f11 = lbl_80408F00;
    temp_f0 = (lbl_80408F04 * lbl_80408E9C) + temp_f11;
    lbl_80408F00 = temp_f0;
    if (temp_f0 > lbl_8019EA00[0]) {
        lbl_80408F00 = lbl_8019EA00[0];
    } else if (temp_f0 < lbl_8019EA04[0]) {
        lbl_80408F00 = lbl_8019EA04[0];
    }
    if ((temp_f11 < lbl_8019EA00[0]) && (lbl_80408F00 == lbl_8019EA00[0])) {
        var_r0 = 1;
        goto block_9;
    }
    if ((s32) lbl_80408F08 != 0) {
        var_r0 = lbl_80408F08 - 1;
block_9:
        lbl_80408F08 = var_r0;
    }
    if ((lbl_80408F00 == lbl_8019EA04[0]) || (lbl_80408F00 == lbl_8019EA00[0])) {
        lbl_80408F04 = lbl_8019EA08[0];
    }
    if ((lbl_80408F00 == lbl_8019EA04[0]) && (temp_f11 > lbl_8019EA04[0])) {
        lbl_80408F14 = 0;
    }
}

s32 fn_8009ED84(s32);
extern s32 lbl_80408FA0;

void fn_800A1BD4(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8009ED84(var_r31);
        var_r31 += 1;
    } while (var_r31 <= 1);
    lbl_80408FA0 = 0;
}

void *fn_80024C18(s32);
s32 fn_80024CD4(void *);
extern void *lbl_8040AF8C;

void fn_800A1F9C(void) {
    void *temp_r3;

    temp_r3 = fn_80024C18(1);
    lbl_8040AF8C = temp_r3;
    if (temp_r3 != NULL) {
        (*(s8 *)((s8 *)(temp_r3) + (0xBE))) = 0;
        (*(s8 *)((s8 *)(lbl_8040AF8C) + (0xB8))) = 0;
        (*(s8 *)((s8 *)(lbl_8040AF8C) + (0xC0))) = 1;
        fn_80024CD4(lbl_8040AF8C);
    }
}

s32 fn_800CA018(s32);
extern s32 lbl_80408E84;

void fn_8009D550(void)
{
  s32 new_var;
  s32 temp_r9;
  temp_r9 = lbl_80408E84 + 1;
  lbl_80408E84 = temp_r9;
  fn_800CA018(new_var);
  new_var = temp_r9;
}

void *fn_80024BF0(s32);
void *fn_80024C18(s32);
s32 fn_80024CD4(void *);
extern void *lbl_8040AF68;
extern void *lbl_8040AF6C;
extern void *lbl_8040AF70;

void fn_8009D9D8(void)
{
  void *temp_r3;
  void *temp_r3_2;
  void *temp_r3_3;
  temp_r3 = fn_80024BF0(1);
  lbl_8040AF68 = temp_r3;
  *((s8 *) (((s8 *) temp_r3) + 0xBE)) = 3;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xB9)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xBF)) = 2;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xBB)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xBC)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xC0)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xB8)) = 3;
  fn_80024CD4(lbl_8040AF68);
  temp_r3_2 = fn_80024BF0(1);
  lbl_8040AF6C = temp_r3_2;
  *((s8 *) (((s8 *) temp_r3_2) + 0xBE)) = 3;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xB9)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xBF)) = 2;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xBB)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xBC)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xC0)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xB8)) = 1;
  fn_80024CD4(lbl_8040AF6C);
  temp_r3_3 = fn_80024C18(1);
  lbl_8040AF70 = temp_r3_3;
  *((s8 *) (((s8 *) temp_r3_3) + 0xBE)) = 3;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xB9)) = 1;
  *(((s8 *) lbl_8040AF70) + 0xBF) = 2;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xBB)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xBC)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xC0)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xB8)) = 1;
  temp_r3_3 = lbl_8040AF70;
  *((s16 *) (((s8 *) temp_r3_3) + 0xF8)) = 0x64;
  fn_80024CD4(temp_r3_3);
}

void fn_800092F0(s32, s32, void *, s32);

void fn_8009D504(void *arg0) {
    s32 sp8;

    sp8 = (*(s32 *)((s8 *)(arg0) + (0x14)));
    fn_800092F0(arg0 + 0x18, (*(s32 *)((s8 *)(arg0) + (0x10))), &sp8, 0);
}

s32 fn_8009EC80(s32);
s32 fn_800B2ED4(void);
s32 fn_8011EAA8(s32);
extern f32 lbl_8019EAEC[];
extern s32 lbl_803D9A58[];
extern u8 lbl_803E028C[];
extern s32 lbl_80408EA8;
extern f32 lbl_80408F00;
extern s32 lbl_80408FE0;
extern s32 lbl_80409118;
extern s32 lbl_8040912C;
extern s32 lbl_8040913C;
extern s32 lbl_804093B0;
extern void *lbl_80409410;
extern s32 lbl_8040AFA8;
extern s32 lbl_8040AFAC;
extern s32 lbl_8040AFB0;
extern s32 lbl_8040AFB4;

void fn_8009F0B8(s32 arg0)
{
  s32 temp_r3;
  s32 temp_r9;
  void *new_var;
  s32 var_r0;
  void *temp_r9_2;
  void *temp_r9_3;
  if (((s32) lbl_8040912C) == 0)
  {
    temp_r9 = arg0 * 0x5C;
    new_var = (s8 *) lbl_803D9A58;
    if (((((((s32) (*((u8 *) (((s8 *) (temp_r9 + new_var)) + 0x24)))) == 0) || ((temp_r9 = (*((s32 *) (((s8 *) (*((s32 *) (new_var + temp_r9)))) + 0x6C))) & 0x20000000, (temp_r9 == 0) == 0))) || (((s32) lbl_80408FE0) != 0)) || ((temp_r3 = fn_800B2ED4(), (temp_r3 == 1) != 0))) || ((var_r0 = 1, (temp_r3 == 0x1A) != 0)))
    {
      var_r0 = 0;
    }
    new_var = (void *) 0;
    if (((((((((((((var_r0 != 0) && (((s32) lbl_8040913C) == (-1))) && (((s32) lbl_804093B0) == (-1))) && (lbl_80408F00 == lbl_8019EAEC[0])) && (((s32) lbl_80408EA8) == 0)) && (((s32) lbl_80409118) == 0)) && (((s32) lbl_8040AFA8) == 0)) && ((((s32) (*((s32 *) (((s8 *) lbl_80409410) + 0x19C)))) != 0x15) || (((s32) lbl_803E028C[0]) != 5))) && (((temp_r9_2 = *((void **) (((s8 *) lbl_80409410) + 0x1AC)), (temp_r9_2 == new_var) != 0)) || (!((*((u8 *) (((s8 *) temp_r9_2) + 0x32))) & 4)))) && (((temp_r9_3 = *((void **) (((s8 *) lbl_80409410) + 0x1A8)), (temp_r9_3 == new_var) != 0)) || (!((*((s32 *) (((s8 *) (*((void **) (((s8 *) lbl_80409410) + 0x1A8)))) + 4))) & 0x1000)))) && (fn_800B2ED4() == (-1))) && ((((s32) lbl_8040AFAC) == 0) || (fn_8011EAA8(lbl_8040AFB0) != 0))) && (((s32) lbl_8040AFB4) == 0))
    {
      fn_8009EC80(arg0);
    }
  }
}
