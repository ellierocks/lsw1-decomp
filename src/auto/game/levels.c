#include "game/levels.h"

extern f32 lbl_8019FEDC[];
extern f32 lbl_801A4EC0[];
extern s32 lbl_8040B170;
extern f32 lbl_8040A6B8;
extern s32 lbl_8040A6B0;
extern f32 lbl_8040A6B4;
struct fn_800CF198_s {
    u8 pad0[0x52];
    u16 h52;
    u8 pad54[4];
    f32 f58;
};
extern struct fn_800CF198_s lbl_803ED35C[];
extern f32 lbl_801A4F18[];
extern s32 gTotalStudsBalance[];
extern f32 lbl_801A4EDC[];
extern s32 lbl_801D5744[];
extern s32 lbl_801D5768[];
extern u8 lbl_801DC964[];
extern u8 lbl_802E0C78[];
extern s32 lbl_804093B8;
extern s32 lbl_804093D8;
extern s32 lbl_804093DC;
extern f32 lbl_8040B0C8;
extern s32 lbl_8040B164;

s32 DebugMenu_ToggleOption2(void)
{
    return lbl_801D5744[0];
}

s32 fn_800AFF48(void)
{
    return lbl_801D5768[0];
}

void fn_800AFF54(void)
{

}

void Menu_Unknown15_Enter(void)
{

}

void fn_800B5948(void)
{

}

s32 fn_800BA734(void)
{
    return gTotalStudsBalance[0];
}

void SceneSelect_HandleEntry(void)
{
    lbl_801DC964[0] = 0;
    lbl_804093D8 = 0;
    lbl_804093DC = -1;
}

void fn_800CDD18(void)
{
    lbl_8040B164 = 0;
    lbl_8040B0C8 = lbl_801A4EDC[0];
}

void fn_800CF02C(void)
{
    lbl_802E0C78[0] = 0;
}

void fn_800CF0B4(void)
{
    lbl_804093B8 = 115;
}

void fn_800ABF60(u8* a)
{
    f32 t0 = lbl_8019FEDC[0];
    *(u8*)(a + 16) = 0;
    *(f32*)(a + 12) = t0;
    *(f32*)(a + 4) = t0;
}

void fn_800B02E0(f32 x, f32 y, f32 z, u8* a)
{
    *(f32*)(a + 328) = x;
    *(f32*)(a + 332) = y;
    *(f32*)(a + 336) = z;
}

void fn_800CC6C4(void)
{
    f32 t0 = lbl_801A4EC0[0];
    lbl_8040A6B0 = -1;
    lbl_8040B170 = -1;
    lbl_8040A6B4 = t0;
    lbl_8040A6B8 = t0;
}

void fn_800CF198(void)
{
    lbl_803ED35C[0].f58 = lbl_801A4F18[0];
    lbl_803ED35C[0].h52 = lbl_803ED35C[0].h52 - 1;
}

extern void fn_8006DF68(f32);
extern void fn_8006DEB0(f32);

f32 fn_800B9D44(u8* settings)
{
    f32 volume = ((f32)settings[3] / 10.0f) * ((f32)settings[4] / 10.0f);
    fn_8006DF68(volume);
    return volume;
}

f32 fn_800B9DC8(u8* settings)
{
    f32 volume = ((f32)settings[2] / 10.0f) * ((f32)settings[4] / 10.0f);
    fn_8006DEB0(volume);
    return volume;
}

s32 Menu_ResetFromGame();

void Menu_Unknown13_HandleSelection(void) {
    Menu_ResetFromGame();
}

s32 fn_800DF8A8();

void Menu_Unknown14_BuildOptions(void) {
    fn_800DF8A8();
}

s32 fn_800DFC04();

void Menu_Unknown14_Close(void) {
    fn_800DFC04();
}

s32 fn_800E27CC();

void Menu_Unknown14_Enter(void) {
    fn_800E27CC();
}

s32 fn_800DF360();

void Menu_Unknown14_HandleSelection(void) {
    fn_800DF360();
}

s32 fn_800E0790();

void Menu_Unknown15_BuildOptions(void) {
    fn_800E0790();
}

s32 fn_8010C570(s32, f32);
extern f32 lbl_8040B0E0;
extern s32 lbl_8040B0EC;

void Menu_Unknown16_BuildOptions(void) {
    fn_8010C570(lbl_8040B0EC, lbl_8040B0E0);
}

s32 fn_800DD1C8();

void Menu_Unknown17_BuildOptions(void) {
    fn_800DD1C8();
}

s32 fn_800DA478();

void Menu_Unknown17_HandleSelection(void) {
    fn_800DA478();
}

s32 fn_800A20AC();

void Menu_Unknown4_Close(void) {
    fn_800A20AC();
}

s32 Menu_Unknown3_BuildOptions();

void Menu_Unknown6_BuildOptions(void) {
    Menu_Unknown3_BuildOptions();
}

s32 RotDiff(s32 arg0, s32 arg1) {
    s32 temp_r3;

    temp_r3 = arg1 - arg0;
    if (temp_r3 > 0x8000) {
        return temp_r3 + 0xFFFF0000;
    }
    if (temp_r3 < -0x8000) {
        return temp_r3 + 0x10000;
    }
    return temp_r3;
}

extern s32 gPlayer1Pointer[];
extern void *gPlayerPointers[];
extern void *lbl_80409004;
extern void *lbl_80409008;

void fn_800ABCC8(void) {
    if (((void *) (*(void **)((s8 *)(gPlayer1Pointer) + (0))) != NULL) && ((*(s32 *)((s8 *)((*(void **)((s8 *)(gPlayer1Pointer) + (0)))) + (0x20C))) & 0x01000000)) {
        lbl_80409004 = (*(void **)((s8 *)(gPlayer1Pointer) + (0)));
        if (((void *) (*(void **)((s8 *)(gPlayer1Pointer) + (4))) != NULL) && ((*(s32 *)((s8 *)((*(void **)((s8 *)(gPlayer1Pointer) + (4)))) + (0x20C))) & 0x01000000)) {
            lbl_80409008 = (*(void **)((s8 *)(gPlayer1Pointer) + (4)));
            return;
        }
        lbl_80409008 = NULL;
        return;
    }
    if (((void *) gPlayerPointers[0] != NULL) && ((*(s32 *)((s8 *)(gPlayerPointers[0]) + (0x20C))) & 0x01000000)) {
        lbl_80409004 = gPlayerPointers[0];
        lbl_80409008 = NULL;
        return;
    }
    lbl_80409004 = NULL;
}

extern s32 lbl_804091F8;

s32 fn_800AC658(void *arg0) {
    s32 temp_r0;

    if (((s32) lbl_804091F8 == 0) && ((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0xD) && ((temp_r0 = (*(s32 *)((s8 *)(lbl_80409410) + (0x19C))), ((temp_r0 == 9) != 0)) || (temp_r0 == 0xB))) {
        return 1;
    }
    return 0;
}

s32 fn_80028238(s32);

void fn_800B9BB8(void) {
    fn_80028238(0x80808080);
}

extern s32 lbl_80409134;

u16 fn_800B9ED4(void) {
    u16 temp_r9;

    temp_r9 = (lbl_80409134 * 0x24CD) + 1;
    lbl_80409134 = (s32) temp_r9;
    return temp_r9;
}

s32 NuRandSetSeed(s32, s32);
s32 fn_80064BEC(s32);
s32 lbl_80155678(s32);
extern s32 lbl_80409134;

void fn_800B9EF0(void) {
    lbl_80155678(0);
    lbl_80409134 = 0x3039;
    NuRandSeed(0);
    fn_80064BEC(0);
    NuRandSetSeed(0, 0);
}

s32 fn_800130F4();
void *fn_8001384C(s32);

s32 fn_800BA1E8(s32 arg0) {
    void *temp_r3;

    if (fn_800130F4() != 0) {
        temp_r3 = fn_8001384C(arg0);
        if (temp_r3 != NULL) {
            (*(s32 *)((s8 *)(temp_r3) + (0x50))) = (s32) ((*(s32 *)((s8 *)(temp_r3) + (0x50))) | 0x80000000);
            return 1;
        }
    }
    return 0;
}

void fn_800BA434(void) {
    s32 var_ctr;

    var_ctr = 4;
    do {
        var_ctr -= 1;
    } while (var_ctr != 0);
}

s32 NuSpecialFind(s32, s32 *, s32 *, s32);
s32 fn_8012724C(void *, s32 *, s32);
s32 fn_801276E4(void *, s32, s32);
extern s32 lbl_801A1F2C[];
extern s32 lbl_801A1F38[];
extern s32 lbl_801A1F44[];
extern s32 lbl_801A1F50[];
extern s32 lbl_801A1F58[];
extern s32 lbl_802DA1F8[];
extern s32 lbl_802DA204[];
extern s32 lbl_803EAD24[];
extern s32 lbl_803EAD30[];
extern s32 lbl_803EB044[];
extern s32 lbl_803EB048[];
extern s32 lbl_803EB04C[];

void fn_800BD7BC(void) {
    s32 temp_r3;
    s32 temp_r3_2;
    s32 temp_r3_3;

    temp_r3 = fn_8012724C(lbl_80409410, lbl_801A1F2C, 0);
    if (temp_r3 != 0) {
        lbl_803EB044[0] = fn_801276E4(lbl_80409410, temp_r3, 0);
    }
    temp_r3_2 = fn_8012724C(lbl_80409410, lbl_801A1F38, 0);
    if (temp_r3_2 != 0) {
        lbl_803EB048[0] = fn_801276E4(lbl_80409410, temp_r3_2, 0);
    }
    temp_r3_3 = fn_8012724C(lbl_80409410, lbl_801A1F44, 0);
    if (temp_r3_3 != 0) {
        lbl_803EB04C[0] = fn_801276E4(lbl_80409410, temp_r3_3, 0);
    }
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD24, lbl_801A1F50, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_802DA1F8, lbl_801A1F50, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD30, lbl_801A1F58, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_802DA204, lbl_801A1F58, 1);
}

s32 NuSpecialFind(s32, s32 *, s32 *, s32);
s32 fn_8008FA44(s32, s32 *, s32);
s32 fn_8012724C(void *, s32 *, s32);
extern s32 lbl_801A20AC[];
extern s32 lbl_801A20B4[];
extern s32 lbl_801A20C0[];
extern s32 lbl_801A20D0[];
extern s32 lbl_801A20E0[];
extern s32 lbl_801A20F0[];
extern s32 lbl_801A2100[];
extern s32 lbl_801A2110[];
extern s32 lbl_801A2120[];
extern s32 lbl_801A2130[];
extern s32 lbl_801A2140[];
extern s32 lbl_803EAD24[];
extern s32 lbl_803EAD30[];
extern s32 lbl_803EAD3C[];
extern s32 lbl_803EAD48[];
extern s32 lbl_803EAD54[];
extern s32 lbl_803EAD60[];
extern s32 lbl_803EAEC4[];
extern s32 lbl_80408F58;
extern s32 lbl_80409240;
extern s32 lbl_80409244;
extern s32 lbl_8040B154;

void fn_800BE118(void) {
    lbl_80409240 = fn_8008FA44(lbl_80408F58, lbl_801A20AC, 0);
    lbl_80409244 = fn_8008FA44(lbl_80408F58, lbl_801A20B4, 0);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD24, lbl_801A20C0, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD30, lbl_801A20D0, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD3C, lbl_801A20E0, 1);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = fn_8012724C(lbl_80409410, lbl_801A20F0, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = fn_8012724C(lbl_80409410, lbl_801A2100, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = fn_8012724C(lbl_80409410, lbl_801A2110, 0);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD48, lbl_801A2120, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD54, lbl_801A2130, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD60, lbl_801A2140, 1);
    lbl_8040B154 = 0;
}

s32 fn_80116FEC(s32);

void fn_800CC970(void) {
    fn_80116FEC(1);
}


void fn_800CC994(void) {
    s32 (*temp_r0)();

    temp_r0 = (*(s32 (**)())((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (0x20)));
    if (temp_r0 != NULL) {
        temp_r0();
    }
}


void fn_800CC9CC(void) {
    s32 (*temp_r0)();

    temp_r0 = (*(s32 (**)())((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (0x28)));
    if (temp_r0 != NULL) {
        temp_r0();
    }
}

s32 fn_8012676C(s32);

void fn_800CCDC0(void) {
    fn_8012676C(0x1B);
}

s32 fn_8008FA44(s32, s32 *, s32);
extern s32 lbl_801A1E5C[];
extern s32 lbl_801A1E68[];
extern s32 lbl_80408F58;
extern s32 lbl_80409224;
extern s32 lbl_80409228;

void fn_800CD234(void) {
    lbl_80409224 = fn_8008FA44(lbl_80408F58, lbl_801A1E5C, 0);
    lbl_80409228 = fn_8008FA44(lbl_80408F58, lbl_801A1E68, 0);
}

s32 fn_80127518(s32);
extern s32 lbl_803EB044[];

void fn_800CD484(void) {
    fn_80127518((*(s32 *)((s8 *)(lbl_803EB044) + (0))));
    fn_80127518((*(s32 *)((s8 *)(lbl_803EB044) + (4))));
    fn_80127518((*(s32 *)((s8 *)(lbl_803EB044) + (8))));
}

s32 fn_800BE9B0();

void fn_800CD960(void) {
    fn_800BE9B0();
}

s32 fn_800D515C(s32, s32, s32);
s32 fn_800D95B8();
extern f32 lbl_801A4ED0[];
extern void *lbl_802DA6E4[];

s32 fn_800CD980(void) {
    s32 var_r3;

    var_r3 = fn_800D515C(7, 1, 1);
    if ((var_r3 != 0) && ((void *) lbl_802DA6E4[0] != NULL) && ((*(f32 *)((s8 *)(lbl_802DA6E4[0]) + (0x28))) == lbl_801A4ED0[0])) {
        var_r3 = fn_800D95B8();
    }
    return var_r3;
}

s32 fn_8014CFA4();

void fn_800CDC64(void) {
    fn_8014CFA4();
}

extern s32 lbl_804091F8;
extern s32 lbl_804093B8;

void fn_800CE064(void) {
    s32 var_r9;

    var_r9 = 0x4A;
    if ((s32) lbl_804091F8 != 0) {
        var_r9 = 0x4B;
    }
    lbl_804093B8 = var_r9;
}

s32 fn_800C3A24(s32 *);
extern s32 lbl_801D6648[];

void fn_800CE11C(void) {
    fn_800C3A24(lbl_801D6648);
}

s32 fn_800D7ED8(s32, s32, s32);
extern f32 lbl_801A4EE4[];
extern u8 lbl_8040B060[];
extern f32 lbl_8040B168;

void fn_800CE18C(void) {
    if (((s32) lbl_8040B060[0] == 1) && (lbl_8040B168 > lbl_801A4EE4[0])) {
        fn_800D7ED8((s32) lbl_8040B168 + 1, 0, 0);
    }
}

s32 fn_800D515C(s32, s32, s32);
s32 fn_800D95B8();
extern f32 lbl_801A4EEC[];
extern void *lbl_802E0B8C[];

s32 fn_800CE280(void) {
    s32 var_r3;

    var_r3 = fn_800D515C(0xE, 1, 1);
    if ((var_r3 != 0) && ((void *) lbl_802E0B8C[0] != NULL) && ((*(f32 *)((s8 *)(lbl_802E0B8C[0]) + (0x28))) == lbl_801A4EEC[0])) {
        var_r3 = fn_800D95B8();
    }
    return var_r3;
}

extern s32 lbl_804091F8;
extern s32 lbl_804093B8;

void fn_800CE2DC(void) {
    s32 var_r9;

    var_r9 = 0x52;
    if ((s32) lbl_804091F8 != 0) {
        var_r9 = 0x53;
    }
    lbl_804093B8 = var_r9;
}

s32 fn_800130F4(s32 *);
s32 fn_80013200(s32 *, s32);
extern s32 lbl_803EAD24[];
extern s32 lbl_803EAD30[];
extern u8 lbl_8040B060[];

void fn_800CE38C(void) {
    if ((s32) lbl_8040B060[0] == 1) {
        lbl_8040B060[0] = 2;
        if (fn_800130F4(lbl_803EAD24) != 0) {
            fn_80013200(lbl_803EAD24, 1);
        }
        if (fn_800130F4(lbl_803EAD30) != 0) {
            fn_80013200(lbl_803EAD30, 1);
        }
    }
}

s32 fn_8008FA44(s32, s32 *, s32);
extern s32 lbl_801A20B4[];
extern s32 lbl_80408F58;
extern s32 lbl_80409344;
extern s32 lbl_80409348;

void fn_800CE8C0(void) {
    lbl_80409348 = 0;
    lbl_80409344 = fn_8008FA44(lbl_80408F58, lbl_801A20B4, 0);
}

extern f32 lbl_801A4F0C[];
extern s32 lbl_80408F2C;
extern void *lbl_80409368;
extern s32 lbl_804093B0;
extern s32 lbl_8040AF64;
extern s32 lbl_8040AF94;

void fn_800CEFE8(void) {
    if (((void *) lbl_80409368 != NULL) && ((*(f32 *)((s8 *)(lbl_80409368) + (0x28))) == lbl_801A4F0C[0])) {
        lbl_80408F2C = 1;
        lbl_804093B0 = 0x72;
        if ((s32) lbl_8040AF64 != -1) {
            lbl_8040AF94 = 1;
        }
    }
}

s32 fn_8008FA44(s32, s32 *, s32);
extern s32 lbl_801A2E90[];
extern s32 lbl_80408F58;
extern s32 lbl_8040A6A8;

void fn_800CF0C0(void) {
    lbl_8040A6A8 = fn_8008FA44(lbl_80408F58, lbl_801A2E90, 0);
}

s32 Shop_DrawSubMenuAndTopShelf();
s32 fn_800E1D0C();

void fn_800CF214(void) {
    Shop_DrawSubMenuAndTopShelf();
    fn_800E1D0C();
}

s32 fn_800A2F94(void *);
s32 fn_800ABA78();
s32 fn_800AC020(s32, f32);
s32 fn_801044F0(void *);
extern f32 lbl_801A5258[];

void fn_800D4FDC(void *arg0, f32 farg0) {
    f32 temp_f31;
    void *temp_r9;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) != 0x1B) {
        temp_f31 = (*(f32 *)((s8 *)(temp_r9) + (0x6AC)));
        fn_800ABA78();
        fn_800A2F94((*(void **)((s8 *)(arg0) + (0xEEC))));
        (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6AC))) = temp_f31;
        (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) = 0x1BU;
        (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = farg0;
        (*(s16 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7A6))) = 0x47;
        if ((u32) ((*(u16 *)((s8 *)(arg0) + (0x1074))) - 0x1D) <= 1U) {
            fn_801044F0(arg0);
        }
        fn_800AC020(*(*(s32 **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C))), lbl_801A5258[0]);
    }
}

void fn_800D5090(void *arg0) {
    f32 temp_f11;
    f32 temp_f12;

    temp_f12 = (*(f32 *)((s8 *)(arg0) + (0x1014)));
    temp_f11 = (*(f32 *)((s8 *)(arg0) + (0x1018)));
    (*(f32 *)((s8 *)(arg0) + (0x101C))) = temp_f12;
    (*(f32 *)((s8 *)(arg0) + (0x1020))) = temp_f11;
    (*(f32 *)((s8 *)(arg0) + (0x1030))) = (f32) ((temp_f11 - temp_f12) / (2.0f * (*(f32 *)((s8 *)(arg0) + (0x1028)))));
}

extern f32 lbl_801A553C[];
extern f32 lbl_801A5540[];
extern void *lbl_80409570;
extern f32 lbl_8040B1E4;

void fn_800D9A60(void) {
    f32 temp_f0;
    f32 temp_f12;

    if ((void *) lbl_80409570 != NULL) {
        temp_f12 = (*(f32 *)((s8 *)(lbl_80409570) + (4)));
        if (temp_f12 != lbl_801A553C[0]) {
            temp_f0 = temp_f12 - lbl_801A5540[0];
            if (lbl_8040B1E4 < temp_f0) {
                lbl_8040B1E4 = temp_f0;
            }
        }
        (*(f32 *)((s8 *)(lbl_80409570) + (0xC))) = (f32) lbl_801A553C[0];
    }
}

s32 fn_80108E6C(s32, s32 *);
extern s32 lbl_801A2080[];
extern s32 lbl_801A2084[];
extern s32 lbl_801A2088[];
extern s32 lbl_801A208C[];
extern s32 lbl_801A2090[];
extern s32 lbl_801A2094[];
extern s32 lbl_801A2098[];
extern s32 lbl_801A209C[];
extern s32 lbl_801A20A0[];
extern s32 lbl_801A20A4[];
extern s32 lbl_803EAE44[];

void fn_800BDF60(void) {
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0))) = fn_80108E6C(lbl_80409410, lbl_801A2080);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (4))) = fn_80108E6C(lbl_80409410, lbl_801A2084);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (8))) = fn_80108E6C(lbl_80409410, lbl_801A2088);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0xC))) = fn_80108E6C(lbl_80409410, lbl_801A208C);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x10))) = fn_80108E6C(lbl_80409410, lbl_801A2090);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x14))) = fn_80108E6C(lbl_80409410, lbl_801A2094);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x18))) = fn_80108E6C(lbl_80409410, lbl_801A2098);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x1C))) = fn_80108E6C(lbl_80409410, lbl_801A209C);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x20))) = fn_80108E6C(lbl_80409410, lbl_801A20A0);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x24))) = fn_80108E6C(lbl_80409410, lbl_801A20A4);
}

s32 fn_800AC0C0(f32, f32);
s32 fn_800C3B54(s32 *);
s32 fn_800CC3FC();
s32 fn_800CCA04(s32, s32 *);
extern void *gPlayerPointers[];
extern s32 lbl_801A29B8[];
extern f32 lbl_801A29C8[];
extern f32 lbl_801A29CC[];
extern f32 lbl_801A29D0[];
extern s32 lbl_801D6648[];
extern u8 lbl_8040B060[];

void fn_800C3EEC(void) {
    fn_800C3B54(lbl_801D6648);
    if (((s32) lbl_8040B060[0] == 0) && ((((void *) gPlayer1Pointer[0] != NULL) && ((s32) (*(u8 *)((s8 *)(gPlayer1Pointer[0]) + (0x601))) == 0) && ((*(f32 *)((s8 *)(gPlayer1Pointer[0]) + (0x62C))) > lbl_801A29C8[0])) || (((void *) gPlayerPointers[0] != NULL) && ((s32) (*(u8 *)((s8 *)(gPlayerPointers[0]) + (0x601))) == 0) && ((*(f32 *)((s8 *)(gPlayerPointers[0]) + (0x62C))) > lbl_801A29C8[0])))) {
        fn_800CCA04(lbl_80409410, lbl_801A29B8);
        fn_800CC3FC();
        lbl_8040B060[0] = 1;
        fn_800AC0C0(lbl_801A29CC[0], lbl_801A29D0[0]);
    }
}

s32 fn_80108E6C(s32, s32 *);
extern s32 lbl_801A1C90[];
extern s32 lbl_801A1C98[];
extern s32 lbl_801A1CA0[];
extern s32 lbl_801A1CA8[];
extern s32 lbl_803EAE44[];

void fn_800CCC98(void) {
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0))) = fn_80108E6C(lbl_80409410, lbl_801A1C90);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (4))) = fn_80108E6C(lbl_80409410, lbl_801A1C98);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (8))) = fn_80108E6C(lbl_80409410, lbl_801A1CA0);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0xC))) = fn_80108E6C(lbl_80409410, lbl_801A1CA8);
}

extern s32 lbl_801A1CBC[];
extern s32 lbl_801A1CC8[];
extern s32 lbl_801A1CD4[];
extern s32 lbl_803EAEC4[];

void fn_800CCD4C(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = fn_8012724C(lbl_80409410, lbl_801A1CBC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = fn_8012724C(lbl_80409410, lbl_801A1CC8, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = fn_8012724C(lbl_80409410, lbl_801A1CD4, 0);
}

s32 fn_80108E6C(s32, s32 *);
extern s32 lbl_801A1D64[];
extern s32 lbl_801A1D70[];
extern s32 lbl_801A1D80[];
extern s32 lbl_803EAE44[];

void fn_800CD0DC(void) {
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0))) = fn_80108E6C(lbl_80409410, lbl_801A1D64);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (4))) = fn_80108E6C(lbl_80409410, lbl_801A1D70);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (8))) = fn_80108E6C(lbl_80409410, lbl_801A1D80);
}

s32 fn_80108E6C(s32, s32 *);
extern s32 lbl_801A1E78[];
extern s32 lbl_803EAE44[];

void fn_800CD280(void) {
    lbl_803EAE44[0] = fn_80108E6C(lbl_80409410, lbl_801A1E78);
}

s32 fn_800D515C(s32, s32, s32);
s32 fn_800D95B8();
extern f32 lbl_801A4ECC[];

s32 fn_800CD6A8(void) {
    s32 var_r3;

    var_r3 = fn_800D515C(8, 1, 1);
    if ((var_r3 != 0) && ((void *) lbl_80409244 != NULL) && ((*(f32 *)((s8 *)(lbl_80409244) + (0x28))) == lbl_801A4ECC[0])) {
        var_r3 = fn_800D95B8();
    }
    return var_r3;
}

extern s32 lbl_801A2220[];
extern s32 lbl_802DA1E8[];
extern s32 lbl_803EAEC4[];

void fn_800CD898(void) {
    s32 temp_r3;

    temp_r3 = fn_8012724C(lbl_80409410, lbl_801A2220, 0);
    lbl_803EAEC4[0] = temp_r3;
    if (temp_r3 != 0) {
        lbl_802DA1E8[0] = fn_8001384C(temp_r3 + 4);
    }
}

extern s32 lbl_801A2470[];
extern s32 lbl_803EAEC4[];

void fn_800CD928(void) {
    lbl_803EAEC4[0] = fn_8012724C(lbl_80409410, lbl_801A2470, 0);
}

s32 fn_8014C728(s32);

void fn_800CDB98(void) {
    fn_8014C728(lbl_80409410);
}

s32 fn_8008FA44(s32, s32 *, s32);
s32 fn_8014CF8C(s32);
extern s32 lbl_801A25B8[];
extern s32 lbl_80408F58;
extern s32 lbl_8040B19C;

void fn_800CDBBC(void) {
    fn_8014CF8C(lbl_80409410);
    lbl_8040B19C = fn_8008FA44(lbl_80408F58, lbl_801A25B8, 0);
}

extern s32 lbl_801A2628[];
extern s32 lbl_801A2630[];
extern s32 lbl_801A2638[];
extern s32 lbl_801A2640[];
extern s32 lbl_803EAEC4[];
extern s32 lbl_804092C4;

void fn_800CDC84(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = fn_8012724C(lbl_80409410, lbl_801A2628, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = fn_8012724C(lbl_80409410, lbl_801A2630, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = fn_8012724C(lbl_80409410, lbl_801A2638, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0xC))) = fn_8012724C(lbl_80409410, lbl_801A2640, 0);
    lbl_804092C4 = 0;
}

extern s32 lbl_801A2B7C[];
extern s32 lbl_801A2B84[];
extern s32 lbl_803EAEC4[];

void fn_800CE490(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = fn_8012724C(lbl_80409410, lbl_801A2B7C, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = fn_8012724C(lbl_80409410, lbl_801A2B84, 0);
}

extern s32 lbl_801A1BA8[];
extern s32 lbl_803EAEC4[];
extern s32 lbl_8040B154;

void fn_800CE7F4(void) {
    lbl_803EAEC4[0] = fn_8012724C(lbl_80409410, lbl_801A1BA8, 0);
    lbl_8040B154 = 1;
}

extern s32 lbl_801A2BA8[];
extern s32 lbl_803EAEC4[];

void fn_800CE948(void) {
    lbl_803EAEC4[0] = fn_8012724C(lbl_80409410, lbl_801A2BA8, 0);
}

s32 fn_8008FA44(s32, s32 *, s32);
s32 fn_8011EBF4(s32, s32);
extern s32 lbl_801A2BB8[];
extern s32 lbl_801A2BC8[];
extern s32 lbl_80408F58;
extern s32 lbl_8040934C;
extern s32 lbl_80409350;
extern s32 lbl_80409354;

void fn_800CE980(void) {
    lbl_8040934C = fn_8008FA44(lbl_80408F58, lbl_801A2BB8, 0);
    lbl_80409350 = fn_8008FA44(lbl_80408F58, lbl_801A2BC8, 0);
    lbl_80409354 = fn_8011EBF4(lbl_80409410, 0x50);
}

extern s32 lbl_801A2C08[];
extern s32 lbl_803EAEC4[];

void fn_800CEA9C(void) {
    lbl_803EAEC4[0] = fn_8012724C(lbl_80409410, lbl_801A2C08, 0);
}

extern s32 lbl_801A2CE0[];
extern s32 lbl_801A2CEC[];
extern s32 lbl_801A2CFC[];
extern s32 lbl_801A2D0C[];
extern s32 lbl_801A2D1C[];
extern s32 lbl_803EAEC4[];

void fn_800CEC90(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = fn_8012724C(lbl_80409410, lbl_801A2CE0, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = fn_8012724C(lbl_80409410, lbl_801A2CEC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = fn_8012724C(lbl_80409410, lbl_801A2CFC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0xC))) = fn_8012724C(lbl_80409410, lbl_801A2D0C, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x10))) = fn_8012724C(lbl_80409410, lbl_801A2D1C, 0);
}

extern s32 lbl_801A2D30[];
extern s32 lbl_803EAEC4[];

void fn_800CEDC8(void) {
    lbl_803EAEC4[0] = fn_8012724C(lbl_80409410, lbl_801A2D30, 0);
}

extern f32 lbl_801A4F14[];
extern s32 lbl_80408F2C;
extern s32 lbl_804093B0;
extern s32 lbl_8040AF64;
extern s32 lbl_8040AF94;

void fn_800CF0F4(void) {
    if (((void *) lbl_8040A6A8 != NULL) && ((*(f32 *)((s8 *)(lbl_8040A6A8) + (0x28))) == lbl_801A4F14[0])) {
        lbl_804093B0 = 0x7A;
        lbl_80408F2C = 1;
        if ((s32) lbl_8040AF64 != -1) {
            lbl_8040AF94 = 1;
        }
    }
}

extern s32 lbl_803FD4E0[];
extern s32 lbl_80409484;

void fn_800D97F8(void)
{
  s8 *new_var;
  s32 new_var2;
  s32 var_ctr;
  var_ctr = 0x40;
  new_var = lbl_803FD4E0;
  do
  {
    new_var = (s8 *) new_var;
    *((s8 *) (((s8 *) new_var) + 0x8B)) = 0;
    new_var += 0x9C;
    var_ctr -= 1;
  }
  while (var_ctr != 0);
  var_ctr = 1;
  new_var2 = var_ctr;
  lbl_80409484 = -new_var2;
}

s32 NuSpecialFind(s32, s32 *, s32 *, s32);
s32 fn_8012724C(void *, s32 *, s32);
extern s32 lbl_801A2EA4[];
extern s32 lbl_801A2EA8[];
extern s32 lbl_801A2EAC[];
extern s32 lbl_801A2EB0[];
extern s32 lbl_801A2EB4[];
extern s32 lbl_801A2EB8[];
extern s32 lbl_801A2EBC[];
extern s32 lbl_801A2EC0[];
extern s32 lbl_801A2EC4[];
extern s32 lbl_801A2EC8[];
extern s32 lbl_801A2ECC[];
extern s32 lbl_801A2ED0[];
extern s32 lbl_801A2ED4[];
extern s32 lbl_801A2ED8[];
extern s32 lbl_801A2EDC[];
extern s32 lbl_801A2EE0[];
extern s32 lbl_801A2EE4[];
extern s32 lbl_801A2EE8[];
extern s32 lbl_801A2EEC[];
extern s32 lbl_801A2EF0[];
extern s32 lbl_801A2EF4[];
extern s32 lbl_803EAEC4[];
extern s32 lbl_803ED350[];
extern s8 lbl_8040936C;
extern s8 lbl_8040A6AC;

void fn_800C6AEC(void)
{
  s32 temp_r3;
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0)) = fn_8012724C(lbl_80409410, lbl_801A2EA4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 4)) = fn_8012724C(lbl_80409410, lbl_801A2EA8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 8)) = fn_8012724C(lbl_80409410, lbl_801A2EAC, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0xC)) = fn_8012724C(lbl_80409410, lbl_801A2EB0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x10)) = fn_8012724C(lbl_80409410, lbl_801A2EB4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x14)) = fn_8012724C(lbl_80409410, lbl_801A2EB8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x18)) = fn_8012724C(lbl_80409410, lbl_801A2EBC, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x1C)) = fn_8012724C(lbl_80409410, lbl_801A2EC0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x20)) = fn_8012724C(lbl_80409410, lbl_801A2EC4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x24)) = fn_8012724C(lbl_80409410, lbl_801A2EC8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x28)) = fn_8012724C(lbl_80409410, lbl_801A2ECC, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x2C)) = fn_8012724C(lbl_80409410, lbl_801A2ED0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x30)) = fn_8012724C(lbl_80409410, lbl_801A2ED4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x34)) = fn_8012724C(lbl_80409410, lbl_801A2ED8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x38)) = fn_8012724C(lbl_80409410, lbl_801A2EDC, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x3C)) = fn_8012724C(lbl_80409410, lbl_801A2EE0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x40)) = fn_8012724C(lbl_80409410, lbl_801A2EE4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x44)) = fn_8012724C(lbl_80409410, lbl_801A2EE8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x48)) = fn_8012724C(lbl_80409410, lbl_801A2EEC, 0);
  temp_r3 = fn_8012724C(lbl_80409410, lbl_801A2EF0, 0);
  lbl_8040936C = (lbl_8040A6AC = 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x4C)) = temp_r3;
  NuSpecialFind(*((s32 *) (((s8 *) lbl_80409410) + 0x1C0)), lbl_803ED350, lbl_801A2EF4, 1);
}

s32 NuAnimDataFixPtrs();
extern s32 lbl_801A1AEC[];
extern s32 lbl_801A1AF8[];
extern s32 lbl_801A1B08[];
extern s32 lbl_801A1B18[];
extern s32 lbl_801A1B28[];
extern s32 lbl_801A1B38[];
extern s32 lbl_801A1B48[];
extern s32 lbl_801A1B58[];
extern s32 lbl_801A1B68[];
extern s32 lbl_801A1B78[];
extern s32 lbl_801A1B88[];
extern s32 lbl_801A1B98[];
extern u8 lbl_803EACC9[];

s32 fn_800BB950(void) {
    s32 var_r3;

    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = fn_8012724C(lbl_80409410, lbl_801A1AEC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = fn_8012724C(lbl_80409410, lbl_801A1AF8, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = fn_8012724C(lbl_80409410, lbl_801A1B08, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0xC))) = fn_8012724C(lbl_80409410, lbl_801A1B18, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x10))) = fn_8012724C(lbl_80409410, lbl_801A1B28, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x14))) = fn_8012724C(lbl_80409410, lbl_801A1B38, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x18))) = fn_8012724C(lbl_80409410, lbl_801A1B48, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x1C))) = fn_8012724C(lbl_80409410, lbl_801A1B58, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x20))) = fn_8012724C(lbl_80409410, lbl_801A1B68, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x24))) = fn_8012724C(lbl_80409410, lbl_801A1B78, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x28))) = fn_8012724C(lbl_80409410, lbl_801A1B88, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x2C))) = fn_8012724C(lbl_80409410, lbl_801A1B98, 0);
    var_r3 = fn_8012724C(lbl_80409410, lbl_801A1BA8, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x30))) = var_r3;
    lbl_8040B154 = 1;
    if ((s32) lbl_803EACC9[0] != 0) {
        var_r3 = fn_8011EBF4((s32) lbl_80409410, 5);
        if (var_r3 != 0) {
            var_r3 = NuAnimDataFixPtrs();
        }
    }
    return var_r3;
}

extern s32 lbl_801A1D90[];
extern s32 lbl_801A1D98[];
extern s32 lbl_801A1DA4[];
extern s32 lbl_801A1DB0[];
extern s32 lbl_801A1DBC[];
extern s32 lbl_801A1DC8[];
extern s32 lbl_801A1DD4[];
extern s32 lbl_801A1DE0[];
extern s32 lbl_801A1DEC[];
extern s32 lbl_801A1DF8[];
extern s32 lbl_801A1E04[];
extern s32 lbl_801A1E10[];
extern s32 lbl_801A1E1C[];
extern s32 lbl_801A1E28[];
extern s32 lbl_801A1E34[];
extern s32 lbl_801A1E40[];
extern s32 lbl_801A1E4C[];
extern s32 lbl_803EB038[];
extern s32 lbl_803EB050[];
extern s32 lbl_803EB054[];
extern s32 lbl_803EB058[];
extern s32 lbl_803EB05C[];
extern s32 lbl_803EB060[];
extern s32 lbl_803EB064[];
extern s32 lbl_803EB068[];
extern s32 lbl_803EB06C[];
extern s32 lbl_803EB070[];
extern s32 lbl_803EB074[];
extern s32 lbl_803EB078[];
extern s32 lbl_803EB07C[];

void fn_800BCA74(void) {
    s32 temp_r3;
    s32 temp_r3_10;
    s32 temp_r3_11;
    s32 temp_r3_12;
    s32 temp_r3_13;
    s32 temp_r3_14;
    s32 temp_r3_15;
    s32 temp_r3_2;
    s32 temp_r3_3;
    s32 temp_r3_4;
    s32 temp_r3_5;
    s32 temp_r3_6;
    s32 temp_r3_7;
    s32 temp_r3_8;
    s32 temp_r3_9;

    *lbl_803EAEC4 = fn_8012724C(lbl_80409410, lbl_801A1D90, 0);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB038, lbl_801A1D98, 1);
    temp_r3 = fn_8012724C(lbl_80409410, lbl_801A1DA4, 0);
    if (temp_r3 != 0) {
        *lbl_803EB044 = fn_801276E4(lbl_80409410, temp_r3, 0);
    }
    temp_r3_2 = fn_8012724C(lbl_80409410, lbl_801A1DB0, 0);
    if (temp_r3_2 != 0) {
        *lbl_803EB048 = fn_801276E4(lbl_80409410, temp_r3_2, 0);
    }
    temp_r3_3 = fn_8012724C(lbl_80409410, lbl_801A1DBC, 0);
    if (temp_r3_3 != 0) {
        *lbl_803EB04C = fn_801276E4(lbl_80409410, temp_r3_3, 0);
    }
    temp_r3_4 = fn_8012724C(lbl_80409410, lbl_801A1DC8, 0);
    if (temp_r3_4 != 0) {
        lbl_803EB050[0] = fn_801276E4(lbl_80409410, temp_r3_4, 0);
    }
    temp_r3_5 = fn_8012724C(lbl_80409410, lbl_801A1DD4, 0);
    if (temp_r3_5 != 0) {
        lbl_803EB054[0] = fn_801276E4(lbl_80409410, temp_r3_5, 0);
    }
    temp_r3_6 = fn_8012724C(lbl_80409410, lbl_801A1DE0, 0);
    if (temp_r3_6 != 0) {
        lbl_803EB058[0] = fn_801276E4(lbl_80409410, temp_r3_6, 0);
    }
    temp_r3_7 = fn_8012724C(lbl_80409410, lbl_801A1DEC, 0);
    if (temp_r3_7 != 0) {
        lbl_803EB05C[0] = fn_801276E4(lbl_80409410, temp_r3_7, 0);
    }
    temp_r3_8 = fn_8012724C(lbl_80409410, lbl_801A1DF8, 0);
    if (temp_r3_8 != 0) {
        lbl_803EB060[0] = fn_801276E4(lbl_80409410, temp_r3_8, 0);
    }
    temp_r3_9 = fn_8012724C(lbl_80409410, lbl_801A1E04, 0);
    if (temp_r3_9 != 0) {
        lbl_803EB064[0] = fn_801276E4(lbl_80409410, temp_r3_9, 0);
    }
    temp_r3_10 = fn_8012724C(lbl_80409410, lbl_801A1E10, 0);
    if (temp_r3_10 != 0) {
        lbl_803EB068[0] = fn_801276E4(lbl_80409410, temp_r3_10, 0);
    }
    temp_r3_11 = fn_8012724C(lbl_80409410, lbl_801A1E1C, 0);
    if (temp_r3_11 != 0) {
        lbl_803EB06C[0] = fn_801276E4(lbl_80409410, temp_r3_11, 0);
    }
    temp_r3_12 = fn_8012724C(lbl_80409410, lbl_801A1E28, 0);
    if (temp_r3_12 != 0) {
        lbl_803EB070[0] = fn_801276E4(lbl_80409410, temp_r3_12, 0);
    }
    temp_r3_13 = fn_8012724C(lbl_80409410, lbl_801A1E34, 0);
    if (temp_r3_13 != 0) {
        lbl_803EB074[0] = fn_801276E4(lbl_80409410, temp_r3_13, 0);
    }
    temp_r3_14 = fn_8012724C(lbl_80409410, lbl_801A1E40, 0);
    if (temp_r3_14 != 0) {
        lbl_803EB078[0] = fn_801276E4(lbl_80409410, temp_r3_14, 0);
    }
    temp_r3_15 = fn_8012724C(lbl_80409410, lbl_801A1E4C, 0);
    if (temp_r3_15 != 0) {
        lbl_803EB07C[0] = fn_801276E4(lbl_80409410, temp_r3_15, 0);
    }
}

s32 fn_800133FC(s32 *);
s32 fn_8006DCD4(s32 *, s32);
s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);
extern s32 lbl_801A1D04[];
extern s32 lbl_803EB038[];

void fn_800BCED8(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x24;
            } else {
                lbl_804093B8 = 0x26;
            }
        }
    }
    if ((fn_800130F4(lbl_803EB038) != 0) && ((s32) (*(s32 *)((s8 *)(fn_8001384C((s32) lbl_803EB038)) + (0x50))) < 0)) {
        fn_8006DCD4(lbl_801A1D04, fn_800133FC(lbl_803EB038) + 0x30);
    }
}

extern s32 lbl_801A25D4[];
extern s32 lbl_801A25DC[];
extern s32 lbl_801A25E4[];
extern s32 lbl_801A25EC[];
extern s32 lbl_801A25F4[];
extern s32 lbl_801A25FC[];
extern s32 lbl_801A2604[];
extern s32 lbl_801A260C[];
extern s32 lbl_803EB12C[];
extern s32 lbl_803EB138[];
extern s32 lbl_803EB144[];
extern s32 lbl_803EB150[];
extern s32 lbl_803EB15C[];
extern s32 lbl_803EB168[];
extern s32 lbl_803EB174[];
extern s32 lbl_803EB180[];
extern s8 lbl_8040A68C;

void fn_800C11AC(void) {
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB12C, lbl_801A25D4, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB138, lbl_801A25DC, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB144, lbl_801A25E4, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB150, lbl_801A25EC, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB15C, lbl_801A25F4, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB168, lbl_801A25FC, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB174, lbl_801A2604, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB180, lbl_801A260C, 1);
    lbl_8040A68C = 0;
}

s32 PauseMenu_HandleEntry1(s32 *);
s32 fn_800133FC(s32 *);
extern s32 lbl_801A1BD0[];
extern s32 lbl_801A1BE4[];
extern s32 lbl_802DA264[];
extern s32 lbl_8040A658;
extern s32 lbl_8040A65C;

void fn_800CCB4C(void) {
    s32 var_r3;

    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA264, lbl_801A1BD0, 1);
    lbl_8040A658 = PauseMenu_HandleEntry1(lbl_801A1BE4);
    var_r3 = fn_800130F4(lbl_802DA264);
    if (var_r3 != 0) {
        var_r3 = fn_800133FC(lbl_802DA264) + 0x30;
    }
    lbl_8040A65C = var_r3;
}

s32 PauseMenu_HandleEntry1(s32 *);
s32 fn_800133FC(s32 *);
extern s32 lbl_801A1BE4[];
extern s32 lbl_801A1BEC[];
extern s32 lbl_802DA270[];
extern s32 lbl_8040A660;
extern s32 lbl_8040A664;

void fn_800CCC24(void) {
    s32 var_r3;

    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA270, lbl_801A1BEC, 1);
    lbl_8040A660 = PauseMenu_HandleEntry1(lbl_801A1BE4);
    var_r3 = fn_800130F4(lbl_802DA270);
    if (var_r3 != 0) {
        var_r3 = fn_800133FC(lbl_802DA270) + 0x30;
    }
    lbl_8040A664 = var_r3;
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CCDE4(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 8;
            } else {
                lbl_804093B8 = 0xA;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CCE78(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 9;
            } else {
                lbl_804093B8 = 0xB;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CCF0C(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s32 var_r29;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r29 = 0xC;
    if ((s32) lbl_804091F8 != 0) {
        var_r29 = 0xD;
    }
    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0xA;
                return;
            }
            lbl_804093B8 = var_r29;
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CCFB8(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x11;
            } else {
                lbl_804093B8 = 0x13;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CD144(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s32 var_r29;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r29 = 0x15;
    if ((s32) lbl_804091F8 != 0) {
        var_r29 = 0x16;
    }
    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x13;
                return;
            }
            lbl_804093B8 = var_r29;
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CD3F0(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x26;
            } else {
                lbl_804093B8 = 0x28;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CD4C8(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x27;
            } else {
                lbl_804093B8 = 0x29;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CD55C(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s32 var_r29;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r29 = 0x2A;
    if ((s32) lbl_804091F8 != 0) {
        var_r29 = 0x2B;
    }
    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x28;
                return;
            }
            lbl_804093B8 = var_r29;
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CD700(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x2D;
            } else {
                lbl_804093B8 = 0x2F;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CD804(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x2F;
            } else {
                lbl_804093B8 = 0x31;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CD9DC(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x36;
            } else {
                lbl_804093B8 = 0x37;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CDA70(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x38;
            } else {
                lbl_804093B8 = 0x3A;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CDB04(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x37;
            } else {
                lbl_804093B8 = 0x39;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CDD30(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x41;
            } else {
                lbl_804093B8 = 0x43;
            }
        }
    }
}

extern s32 lbl_801A269C[];
extern s32 lbl_801A26A4[];
extern s32 lbl_801A26AC[];
extern s32 lbl_801A26B4[];
extern s32 lbl_801A26BC[];
extern s32 lbl_801A26C4[];
extern s32 lbl_802DA750[];
extern s32 lbl_802DA75C[];
extern s32 lbl_802DA768[];
extern s32 lbl_802DA774[];
extern s32 lbl_802DA780[];
extern s32 lbl_802DA78C[];

void fn_800CDDC4(void) {
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA750, lbl_801A269C, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA75C, lbl_801A26A4, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA768, lbl_801A26AC, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA774, lbl_801A26B4, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA780, lbl_801A26BC, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA78C, lbl_801A26C4, 1);
    lbl_8040B154 = 0;
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CDEA8(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x42;
            } else {
                lbl_804093B8 = 0x44;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CDF3C(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x43;
            } else {
                lbl_804093B8 = 0x45;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CDFD0(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x45;
            } else {
                lbl_804093B8 = 0x47;
            }
        }
    }
}

extern s32 lbl_801A2B00[];
extern s32 lbl_801A2B10[];
extern u8 lbl_803EAD09[];

void fn_800CE2F8(void) {
    *lbl_8040B060 = 0;
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EAD24, lbl_801A2B00, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EAD30, lbl_801A2B10, 1);
    if (((s32) lbl_804091F8 != 0) || ((s32) lbl_803EAD09[0] != 0)) {
        *lbl_8040B060 = 2;
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CE5C4(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x5C;
            } else {
                lbl_804093B8 = 0x5A;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CE6B4(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s32 var_r29;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r29 = 0x5F;
    if ((s32) lbl_804091F8 != 0) {
        var_r29 = 0x61;
    }
    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x5C;
                return;
            }
            lbl_804093B8 = var_r29;
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CE760(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x58;
            } else {
                lbl_804093B8 = 0x59;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CEAD4(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x64;
            } else {
                lbl_804093B8 = 0x66;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CEB68(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x65;
            } else {
                lbl_804093B8 = 0x67;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CEBFC(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x66;
            } else {
                lbl_804093B8 = 0x68;
            }
        }
    }
}

s32 fn_800CFD4C(f32, f32, f32, f32, f32, f32);

void fn_800CED34(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s8 *temp_r9;
    struct nugspline_s *temp_r31;
    struct nuvec_s *var_r30;

    var_r30 = NULL;
    temp_r31 = lbl_80409410->unk1974;
    if (temp_r31 != NULL) {
        if (fn_800AAC04(&sp8, NULL) != 0) {
            var_r30 = &sp8;
        }
        if (var_r30 != NULL) {
            temp_r9 = temp_r31->pts;
            temp_r0 = temp_r31->ptsize;
            if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) != 0) {
                lbl_804093B8 = 0x6D;
            } else {
                lbl_804093B8 = 0x6B;
            }
        }
    }
}

s32 fn_80105500(struct lbl_80409410_s *, s8, s32, s32);
extern s32 lbl_801A307C[];
extern s16 lbl_803ED3AC[];

void fn_800CF1BC(void) {
    s32 temp_r3;

    lbl_803ED3AC[0] = -1;
    temp_r3 = fn_80108E6C((s32) lbl_80409410, lbl_801A307C);
    if (temp_r3 != 0) {
        fn_80105500(lbl_80409410, (s8) (*(u8 *)((s8 *)(temp_r3) + (0x71))), 0, 0);
    }
}
