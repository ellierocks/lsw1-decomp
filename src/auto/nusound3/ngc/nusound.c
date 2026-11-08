#include "types.h"

extern s32 lbl_8040A494;

extern s32 lbl_8040A434;
extern void fn_800424F4(void);

extern s32 lbl_8040828C;
extern void fn_800472C0(void);

extern void fn_80035050(s32 a);

extern void fn_80045C48(s32 a, s32 b, s32 c, s32 d);

extern void fn_80046090(void);

extern void fn_800460C4(void);

struct nusound_ent_s {
    u16 h;
    u8 pad[0x3e];
};

extern s32 lbl_8040AE2C;
extern s32 lbl_8040AE34;
extern s32 lbl_8040AE38;
extern s32 lbl_8040AE30;
extern struct nusound_ent_s lbl_80348080[];


void fn_80043F9C(void)
{
}

void fn_80044B20(void)
{
}

void fn_80044D0C(void)
{
}

void fn_80044D48(void)
{
}

void fn_80044D8C(void)
{
}

void fn_80044F24(void)
{
}

void fn_80045000(void)
{
}

void fn_8004504C(void)
{
}

s32 fn_80044AE0(void)
{
    return 0;
}

s32 fn_80044AE8(void)
{
    return 0;
}

s32 fn_80044D10(void)
{
    return 0;
}

s32 fn_80044D18(void)
{
    return 0;
}

s32 fn_80044D6C(void)
{
    return 0;
}

s32 fn_80044D74(void)
{
    return 0;
}

s32 fn_80044D7C(void)
{
    return 0;
}

s32 fn_80044D84(void)
{
    return 0;
}

void fn_80043F1C(void)
{
    s32 i;
    lbl_8040AE34 = 0;
    lbl_8040AE38 = 0;
    lbl_8040AE30 = 0;
    lbl_8040AE2C = 0;
    for (i = 255; i >= 0; i--) {
        lbl_80348080[i].h = 0;
    }
}

void fn_80044D4C(void)
{
    fn_800460C4();
}

void fn_80044F04(void)
{
    fn_80046090();
}

void fn_80044B24(s32 a, s32 b)
{
    fn_80045C48(a, 0, b, 0);
}

void fn_80044B50(s32 a, s32 b)
{
    fn_80045C48(a, 0, b, 0);
}

void fn_80044FD4(void)
{
    fn_80035050(0);
    fn_80035050(1);
}

s32 fn_80044AF0(s32 a)
{
    if (a == lbl_8040828C) { fn_800472C0(); }
    return 0;
}

void fn_80042F58(void)
{
    if (lbl_8040A434 == 0xb) { lbl_8040A434 = 0x13; fn_800424F4(); }
}

void fn_80042F8C(void)
{
    if (lbl_8040A434 == 0xb) { lbl_8040A434 = 9; lbl_8040A494 = -1; fn_800424F4(); }
}

s32 fn_800484F8();

void fn_80044B7C(s32 arg0) {
    if (arg0 == 0) {
        fn_800472C0();
    }
    if (arg0 == 1) {
        fn_800484F8();
    }
}

s32 fn_80047290();
s32 fn_800484C8();

void fn_80044BB8(s32 arg0) {
    if (arg0 == 0) {
        fn_80047290();
    }
    if (arg0 == 1) {
        fn_800484C8();
    }
}

s32 fn_80047260();
s32 fn_80048498();

void fn_80044BF4(s32 arg0) {
    if (arg0 == 0) {
        fn_80047260();
    }
    if (arg0 == 1) {
        fn_80048498();
    }
}

s32 fn_800472F0(s32);
s32 fn_80048528(s32);

void fn_80044C30(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        fn_800472F0(arg1);
    }
    if (arg0 == 1) {
        fn_80048528(arg1);
    }
}

void fn_80044D20(void) {
    fn_80046090();
    fn_800472C0();
    fn_800460C4();
}

extern s32 lbl_80407F34;
extern s32 lbl_80407F38;

void fn_80044F6C(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        fn_80035050(0);
        lbl_80407F34 = arg0;
    } else {
        lbl_80407F34 = 1;
    }
    if (arg1 == 0) {
        fn_80035050(1);
        lbl_80407F38 = arg1;
        return;
    }
    lbl_80407F38 = 1;
}

s32 (*fn_80009970(s32 *, s32))();
extern s32 lbl_80193F2C[];
extern s32 lbl_801940DC[];

s32 fn_80045004(void) {
    fn_80009970(lbl_80193F2C, 0x3F7)(lbl_801940DC);
    return 1;
}

void fn_80034FD4(s32, f32, f32);
extern f32 lbl_801940F0[];

void fn_80044D90(s32 arg0, s32 arg1, s32 arg2, f32 fpad0, f32 farg1, f32 farg2) {
    fn_80045C48(arg1, 1, arg2, arg0);
    if ((farg1 > lbl_801940F0[0]) || (farg2 > lbl_801940F0[0])) {
        fn_80034FD4(0, farg1, farg2);
        fn_80034FD4(1, farg1, farg2);
    }
}

void fn_80034FD4(s32, f32, f32);
extern f32 lbl_801940F4[];

void fn_80044E0C(s32 arg0, s32 arg1, s32 arg2, f32 fpad0, f32 farg1, f32 farg2) {
    fn_80045C48(arg1, 0, arg2, arg0);
    if ((farg1 > lbl_801940F4[0]) || (farg2 > lbl_801940F4[0])) {
        fn_80034FD4(0, farg1, farg2);
        fn_80034FD4(1, farg1, farg2);
    }
}

void fn_80034FD4(s32, f32, f32);
extern f32 lbl_801940F8[];

void fn_80044E88(s32 arg0, s32 arg1, s32 arg2, f32 fpad0, f32 farg1, f32 farg2) {
    fn_80045C48(arg1, 0, arg2, arg0);
    if ((farg1 > lbl_801940F8[0]) || (farg2 > lbl_801940F8[0])) {
        fn_80034FD4(0, farg1, farg2);
        fn_80034FD4(1, farg1, farg2);
    }
}
