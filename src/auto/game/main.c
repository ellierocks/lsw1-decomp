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
