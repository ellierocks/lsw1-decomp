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
s32 FindForceObject(void *, s32 *, s32);
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

void RetakeFInit(void) {
    s32 temp_r3;
    s32 temp_r3_2;
    s32 temp_r3_3;

    temp_r3 = FindForceObject(lbl_80409410, lbl_801A1F2C, 0);
    if (temp_r3 != 0) {
        lbl_803EB044[0] = fn_801276E4(lbl_80409410, temp_r3, 0);
    }
    temp_r3_2 = FindForceObject(lbl_80409410, lbl_801A1F38, 0);
    if (temp_r3_2 != 0) {
        lbl_803EB048[0] = fn_801276E4(lbl_80409410, temp_r3_2, 0);
    }
    temp_r3_3 = FindForceObject(lbl_80409410, lbl_801A1F44, 0);
    if (temp_r3_3 != 0) {
        lbl_803EB04C[0] = fn_801276E4(lbl_80409410, temp_r3_3, 0);
    }
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD24, lbl_801A1F50, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_802DA1F8, lbl_801A1F50, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD30, lbl_801A1F58, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_802DA204, lbl_801A1F58, 1);
}

s32 NuSpecialFind(s32, s32 *, s32 *, s32);
s32 CheckAIMessage(s32, s32 *, s32);
s32 FindForceObject(void *, s32 *, s32);
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

void MaulFInit(void) {
    lbl_80409240 = CheckAIMessage(lbl_80408F58, lbl_801A20AC, 0);
    lbl_80409244 = CheckAIMessage(lbl_80408F58, lbl_801A20B4, 0);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD24, lbl_801A20C0, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD30, lbl_801A20D0, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD3C, lbl_801A20E0, 1);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = FindForceObject(lbl_80409410, lbl_801A20F0, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = FindForceObject(lbl_80409410, lbl_801A2100, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = FindForceObject(lbl_80409410, lbl_801A2110, 0);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD48, lbl_801A2120, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD54, lbl_801A2130, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD60, lbl_801A2140, 1);
    lbl_8040B154 = 0;
}

s32 ConfigureAllCharacters(s32);

void fn_800CC970(void) {
    ConfigureAllCharacters(1);
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

s32 CheckAIMessage(s32, s32 *, s32);
extern s32 lbl_801A1E5C[];
extern s32 lbl_801A1E68[];
extern s32 lbl_80408F58;
extern s32 lbl_80409224;
extern s32 lbl_80409228;

void RetakeGReset(void) {
    lbl_80409224 = CheckAIMessage(lbl_80408F58, lbl_801A1E5C, 0);
    lbl_80409228 = CheckAIMessage(lbl_80408F58, lbl_801A1E68, 0);
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
void fn_80013200(s32 *, s32);
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

s32 CheckAIMessage(s32, s32 *, s32);
extern s32 lbl_801A20B4[];
extern s32 lbl_80408F58;
extern s32 lbl_80409344;
extern s32 lbl_80409348;

void fn_800CE8C0(void) {
    lbl_80409348 = 0;
    lbl_80409344 = CheckAIMessage(lbl_80408F58, lbl_801A20B4, 0);
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

s32 CheckAIMessage(s32, s32 *, s32);
extern s32 lbl_801A2E90[];
extern s32 lbl_80408F58;
extern s32 lbl_8040A6A8;

void BonusAReset(void) {
    lbl_8040A6A8 = CheckAIMessage(lbl_80408F58, lbl_801A2E90, 0);
}

s32 Shop_DrawSubMenuAndTopShelf();
s32 fn_800E1D0C();

void fn_800CF214(void) {
    Shop_DrawSubMenuAndTopShelf();
    fn_800E1D0C();
}

s32 ResetPlayerContexts(void *);
s32 fn_800ABA78();
s32 fn_800AC020(s32, f32);
s32 fn_801044F0(void *);
extern f32 lbl_801A5258[];

void DeactivatePlayer(void *arg0, f32 farg0) {
    f32 temp_f31;
    void *temp_r9;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) != 0x1B) {
        temp_f31 = (*(f32 *)((s8 *)(temp_r9) + (0x6AC)));
        fn_800ABA78();
        ResetPlayerContexts((*(void **)((s8 *)(arg0) + (0xEEC))));
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

void CancelHint(void) {
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

s32 FindObstacle(s32, s32 *);
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

void MaulBInit(void) {
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0))) = FindObstacle(lbl_80409410, lbl_801A2080);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (4))) = FindObstacle(lbl_80409410, lbl_801A2084);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (8))) = FindObstacle(lbl_80409410, lbl_801A2088);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0xC))) = FindObstacle(lbl_80409410, lbl_801A208C);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x10))) = FindObstacle(lbl_80409410, lbl_801A2090);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x14))) = FindObstacle(lbl_80409410, lbl_801A2094);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x18))) = FindObstacle(lbl_80409410, lbl_801A2098);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x1C))) = FindObstacle(lbl_80409410, lbl_801A209C);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x20))) = FindObstacle(lbl_80409410, lbl_801A20A0);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0x24))) = FindObstacle(lbl_80409410, lbl_801A20A4);
}

s32 NewRumbleAllPlayers(f32, f32);
s32 UpdatePulses(s32 *);
void fn_800CC3FC();
s32 fn_800CCA04(s32, s32 *);
extern void *gPlayerPointers[];
extern s32 lbl_801A29B8[];
extern f32 lbl_801A29C8[];
extern f32 lbl_801A29CC[];
extern f32 lbl_801A29D0[];
extern s32 lbl_801D6648[];
extern u8 lbl_8040B060[];

void GunshipAUpdate(void) {
    UpdatePulses(lbl_801D6648);
    if (((s32) lbl_8040B060[0] == 0) && ((((void *) gPlayer1Pointer[0] != NULL) && ((s32) (*(u8 *)((s8 *)(gPlayer1Pointer[0]) + (0x601))) == 0) && ((*(f32 *)((s8 *)(gPlayer1Pointer[0]) + (0x62C))) > lbl_801A29C8[0])) || (((void *) gPlayerPointers[0] != NULL) && ((s32) (*(u8 *)((s8 *)(gPlayerPointers[0]) + (0x601))) == 0) && ((*(f32 *)((s8 *)(gPlayerPointers[0]) + (0x62C))) > lbl_801A29C8[0])))) {
        fn_800CCA04(lbl_80409410, lbl_801A29B8);
        fn_800CC3FC();
        lbl_8040B060[0] = 1;
        NewRumbleAllPlayers(lbl_801A29CC[0], lbl_801A29D0[0]);
    }
}

s32 FindObstacle(s32, s32 *);
extern s32 lbl_801A1C90[];
extern s32 lbl_801A1C98[];
extern s32 lbl_801A1CA0[];
extern s32 lbl_801A1CA8[];
extern s32 lbl_803EAE44[];

void GunganBInit(void) {
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0))) = FindObstacle(lbl_80409410, lbl_801A1C90);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (4))) = FindObstacle(lbl_80409410, lbl_801A1C98);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (8))) = FindObstacle(lbl_80409410, lbl_801A1CA0);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0xC))) = FindObstacle(lbl_80409410, lbl_801A1CA8);
}

extern s32 lbl_801A1CBC[];
extern s32 lbl_801A1CC8[];
extern s32 lbl_801A1CD4[];
extern s32 lbl_803EAEC4[];

void GunganEInit(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = FindForceObject(lbl_80409410, lbl_801A1CBC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = FindForceObject(lbl_80409410, lbl_801A1CC8, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = FindForceObject(lbl_80409410, lbl_801A1CD4, 0);
}

s32 FindObstacle(s32, s32 *);
extern s32 lbl_801A1D64[];
extern s32 lbl_801A1D70[];
extern s32 lbl_801A1D80[];
extern s32 lbl_803EAE44[];

void RescueEInit(void) {
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0))) = FindObstacle(lbl_80409410, lbl_801A1D64);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (4))) = FindObstacle(lbl_80409410, lbl_801A1D70);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (8))) = FindObstacle(lbl_80409410, lbl_801A1D80);
}

s32 FindObstacle(s32, s32 *);
extern s32 lbl_801A1E78[];
extern s32 lbl_803EAE44[];

void RetakeGInit(void) {
    lbl_803EAE44[0] = FindObstacle(lbl_80409410, lbl_801A1E78);
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

    temp_r3 = FindForceObject(lbl_80409410, lbl_801A2220, 0);
    lbl_803EAEC4[0] = temp_r3;
    if (temp_r3 != 0) {
        lbl_802DA1E8[0] = fn_8001384C(temp_r3 + 4);
    }
}

extern s32 lbl_801A2470[];
extern s32 lbl_803EAEC4[];

void KaminoEInit(void) {
    lbl_803EAEC4[0] = FindForceObject(lbl_80409410, lbl_801A2470, 0);
}

s32 fn_8014C728(s32);

void fn_800CDB98(void) {
    fn_8014C728(lbl_80409410);
}

s32 CheckAIMessage(s32, s32 *, s32);
s32 fn_8014CF8C(s32);
extern s32 lbl_801A25B8[];
extern s32 lbl_80408F58;
extern s32 lbl_8040B19C;

void FactoryBReset(void) {
    fn_8014CF8C(lbl_80409410);
    lbl_8040B19C = CheckAIMessage(lbl_80408F58, lbl_801A25B8, 0);
}

extern s32 lbl_801A2628[];
extern s32 lbl_801A2630[];
extern s32 lbl_801A2638[];
extern s32 lbl_801A2640[];
extern s32 lbl_803EAEC4[];
extern s32 lbl_804092C4;

void FactoryGInit(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = FindForceObject(lbl_80409410, lbl_801A2628, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = FindForceObject(lbl_80409410, lbl_801A2630, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = FindForceObject(lbl_80409410, lbl_801A2638, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0xC))) = FindForceObject(lbl_80409410, lbl_801A2640, 0);
    lbl_804092C4 = 0;
}

extern s32 lbl_801A2B7C[];
extern s32 lbl_801A2B84[];
extern s32 lbl_803EAEC4[];

void CruiserFInit(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = FindForceObject(lbl_80409410, lbl_801A2B7C, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = FindForceObject(lbl_80409410, lbl_801A2B84, 0);
}

extern s32 lbl_801A1BA8[];
extern s32 lbl_803EAEC4[];
extern s32 lbl_8040B154;

void fn_800CE7F4(void) {
    lbl_803EAEC4[0] = FindForceObject(lbl_80409410, lbl_801A1BA8, 0);
    lbl_8040B154 = 1;
}

extern s32 lbl_801A2BA8[];
extern s32 lbl_803EAEC4[];

void KashyyykAInit(void) {
    lbl_803EAEC4[0] = FindForceObject(lbl_80409410, lbl_801A2BA8, 0);
}

s32 CheckAIMessage(s32, s32 *, s32);
s32 fn_8011EBF4(s32, s32);
extern s32 lbl_801A2BB8[];
extern s32 lbl_801A2BC8[];
extern s32 lbl_80408F58;
extern s32 lbl_8040934C;
extern s32 lbl_80409350;
extern s32 lbl_80409354;

void fn_800CE980(void) {
    lbl_8040934C = CheckAIMessage(lbl_80408F58, lbl_801A2BB8, 0);
    lbl_80409350 = CheckAIMessage(lbl_80408F58, lbl_801A2BC8, 0);
    lbl_80409354 = fn_8011EBF4(lbl_80409410, 0x50);
}

extern s32 lbl_801A2C08[];
extern s32 lbl_803EAEC4[];

void KashyyykCInit(void) {
    lbl_803EAEC4[0] = FindForceObject(lbl_80409410, lbl_801A2C08, 0);
}

extern s32 lbl_801A2CE0[];
extern s32 lbl_801A2CEC[];
extern s32 lbl_801A2CFC[];
extern s32 lbl_801A2D0C[];
extern s32 lbl_801A2D1C[];
extern s32 lbl_803EAEC4[];

void TempleCInit(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = FindForceObject(lbl_80409410, lbl_801A2CE0, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = FindForceObject(lbl_80409410, lbl_801A2CEC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = FindForceObject(lbl_80409410, lbl_801A2CFC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0xC))) = FindForceObject(lbl_80409410, lbl_801A2D0C, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x10))) = FindForceObject(lbl_80409410, lbl_801A2D1C, 0);
}

extern s32 lbl_801A2D30[];
extern s32 lbl_803EAEC4[];

void TempleBInit(void) {
    lbl_803EAEC4[0] = FindForceObject(lbl_80409410, lbl_801A2D30, 0);
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
s32 FindForceObject(void *, s32 *, s32);
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

void BonusBInit(void)
{
  s32 temp_r3;
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0)) = FindForceObject(lbl_80409410, lbl_801A2EA4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 4)) = FindForceObject(lbl_80409410, lbl_801A2EA8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 8)) = FindForceObject(lbl_80409410, lbl_801A2EAC, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0xC)) = FindForceObject(lbl_80409410, lbl_801A2EB0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x10)) = FindForceObject(lbl_80409410, lbl_801A2EB4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x14)) = FindForceObject(lbl_80409410, lbl_801A2EB8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x18)) = FindForceObject(lbl_80409410, lbl_801A2EBC, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x1C)) = FindForceObject(lbl_80409410, lbl_801A2EC0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x20)) = FindForceObject(lbl_80409410, lbl_801A2EC4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x24)) = FindForceObject(lbl_80409410, lbl_801A2EC8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x28)) = FindForceObject(lbl_80409410, lbl_801A2ECC, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x2C)) = FindForceObject(lbl_80409410, lbl_801A2ED0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x30)) = FindForceObject(lbl_80409410, lbl_801A2ED4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x34)) = FindForceObject(lbl_80409410, lbl_801A2ED8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x38)) = FindForceObject(lbl_80409410, lbl_801A2EDC, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x3C)) = FindForceObject(lbl_80409410, lbl_801A2EE0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x40)) = FindForceObject(lbl_80409410, lbl_801A2EE4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x44)) = FindForceObject(lbl_80409410, lbl_801A2EE8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x48)) = FindForceObject(lbl_80409410, lbl_801A2EEC, 0);
  temp_r3 = FindForceObject(lbl_80409410, lbl_801A2EF0, 0);
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

s32 NegotiationsAInit(void) {
    s32 var_r3;

    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = FindForceObject(lbl_80409410, lbl_801A1AEC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = FindForceObject(lbl_80409410, lbl_801A1AF8, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = FindForceObject(lbl_80409410, lbl_801A1B08, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0xC))) = FindForceObject(lbl_80409410, lbl_801A1B18, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x10))) = FindForceObject(lbl_80409410, lbl_801A1B28, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x14))) = FindForceObject(lbl_80409410, lbl_801A1B38, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x18))) = FindForceObject(lbl_80409410, lbl_801A1B48, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x1C))) = FindForceObject(lbl_80409410, lbl_801A1B58, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x20))) = FindForceObject(lbl_80409410, lbl_801A1B68, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x24))) = FindForceObject(lbl_80409410, lbl_801A1B78, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x28))) = FindForceObject(lbl_80409410, lbl_801A1B88, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x2C))) = FindForceObject(lbl_80409410, lbl_801A1B98, 0);
    var_r3 = FindForceObject(lbl_80409410, lbl_801A1BA8, 0);
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

void RetakeBInit(void) {
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

    *lbl_803EAEC4 = FindForceObject(lbl_80409410, lbl_801A1D90, 0);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB038, lbl_801A1D98, 1);
    temp_r3 = FindForceObject(lbl_80409410, lbl_801A1DA4, 0);
    if (temp_r3 != 0) {
        *lbl_803EB044 = fn_801276E4(lbl_80409410, temp_r3, 0);
    }
    temp_r3_2 = FindForceObject(lbl_80409410, lbl_801A1DB0, 0);
    if (temp_r3_2 != 0) {
        *lbl_803EB048 = fn_801276E4(lbl_80409410, temp_r3_2, 0);
    }
    temp_r3_3 = FindForceObject(lbl_80409410, lbl_801A1DBC, 0);
    if (temp_r3_3 != 0) {
        *lbl_803EB04C = fn_801276E4(lbl_80409410, temp_r3_3, 0);
    }
    temp_r3_4 = FindForceObject(lbl_80409410, lbl_801A1DC8, 0);
    if (temp_r3_4 != 0) {
        lbl_803EB050[0] = fn_801276E4(lbl_80409410, temp_r3_4, 0);
    }
    temp_r3_5 = FindForceObject(lbl_80409410, lbl_801A1DD4, 0);
    if (temp_r3_5 != 0) {
        lbl_803EB054[0] = fn_801276E4(lbl_80409410, temp_r3_5, 0);
    }
    temp_r3_6 = FindForceObject(lbl_80409410, lbl_801A1DE0, 0);
    if (temp_r3_6 != 0) {
        lbl_803EB058[0] = fn_801276E4(lbl_80409410, temp_r3_6, 0);
    }
    temp_r3_7 = FindForceObject(lbl_80409410, lbl_801A1DEC, 0);
    if (temp_r3_7 != 0) {
        lbl_803EB05C[0] = fn_801276E4(lbl_80409410, temp_r3_7, 0);
    }
    temp_r3_8 = FindForceObject(lbl_80409410, lbl_801A1DF8, 0);
    if (temp_r3_8 != 0) {
        lbl_803EB060[0] = fn_801276E4(lbl_80409410, temp_r3_8, 0);
    }
    temp_r3_9 = FindForceObject(lbl_80409410, lbl_801A1E04, 0);
    if (temp_r3_9 != 0) {
        lbl_803EB064[0] = fn_801276E4(lbl_80409410, temp_r3_9, 0);
    }
    temp_r3_10 = FindForceObject(lbl_80409410, lbl_801A1E10, 0);
    if (temp_r3_10 != 0) {
        lbl_803EB068[0] = fn_801276E4(lbl_80409410, temp_r3_10, 0);
    }
    temp_r3_11 = FindForceObject(lbl_80409410, lbl_801A1E1C, 0);
    if (temp_r3_11 != 0) {
        lbl_803EB06C[0] = fn_801276E4(lbl_80409410, temp_r3_11, 0);
    }
    temp_r3_12 = FindForceObject(lbl_80409410, lbl_801A1E28, 0);
    if (temp_r3_12 != 0) {
        lbl_803EB070[0] = fn_801276E4(lbl_80409410, temp_r3_12, 0);
    }
    temp_r3_13 = FindForceObject(lbl_80409410, lbl_801A1E34, 0);
    if (temp_r3_13 != 0) {
        lbl_803EB074[0] = fn_801276E4(lbl_80409410, temp_r3_13, 0);
    }
    temp_r3_14 = FindForceObject(lbl_80409410, lbl_801A1E40, 0);
    if (temp_r3_14 != 0) {
        lbl_803EB078[0] = fn_801276E4(lbl_80409410, temp_r3_14, 0);
    }
    temp_r3_15 = FindForceObject(lbl_80409410, lbl_801A1E4C, 0);
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

void FactoryFInit(void) {
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

void FactoryDInit(void) {
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

void CruiserAInit(void) {
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

void MapEReset(void) {
    s32 temp_r3;

    lbl_803ED3AC[0] = -1;
    temp_r3 = FindObstacle((s32) lbl_80409410, lbl_801A307C);
    if (temp_r3 != 0) {
        fn_80105500(lbl_80409410, (s8) (*(u8 *)((s8 *)(temp_r3) + (0x71))), 0, 0);
    }
}

void fn_8000CBF4(void *, void *, void *, void *);
extern f32 lbl_801A4C28[];
extern const f32 lbl_801A4C2C[];

void InitDoors(void) {
    struct lbl_80409410_240_s *a;
    s32 i;
    f32 d;
    char *pts;

    a = lbl_80409410->unk240;
    if (a != NULL) {
        for (i = 0; i < lbl_80409410->unk244; i++, a++) {
            pts = a->spl->pts;
            NuVecAdd(&a->centre, (struct nuvec_s *)pts, (struct nuvec_s *)(pts + a->spl->ptsize));
            NuVecAdd(&a->centre, &a->centre, (struct nuvec_s *)(a->spl->pts + a->spl->ptsize * 2));
            NuVecAdd(&a->centre, &a->centre, (struct nuvec_s *)(a->spl->pts + a->spl->ptsize * 3));
            NuVecScale(&a->centre, &a->centre, lbl_801A4C28[0]);
            a->radius = NuVecDist(&a->centre, a->spl->pts, 0);
            d = NuVecDist(&a->centre, a->spl->pts + a->spl->ptsize, 0);
            if (d > a->radius) {
                a->radius = d;
            }
            d = NuVecDist(&a->centre, a->spl->pts + a->spl->ptsize * 2, 0);
            if (d > a->radius) {
                a->radius = d;
            }
            d = NuVecDist(&a->centre, a->spl->pts + a->spl->ptsize * 3, 0);
            if (d > a->radius) {
                a->radius = d;
            }
            a->radius *= lbl_801A4C2C[0];
            fn_8000CBF4(a->unk74, a->spl->pts, a->spl->pts + a->spl->ptsize, a->spl->pts + a->spl->ptsize * 3);
        }
    }
}

extern const f32 lbl_8019FEE0[];
extern f32 lbl_80408E9C;

void fn_800ABF7C(void *arg0) {
    f32 temp_f13;
    f32 temp_f13_2;
    u8 temp_r9;

    temp_f13 = (*(f32 *)((s8 *)(arg0) + (0xC)));
    if (temp_f13 > lbl_8019FEE0[0]) {
        (*(f32 *)((s8 *)(arg0) + (0xC))) = (f32) (temp_f13 - lbl_80408E9C);
    }
    temp_f13_2 = (*(f32 *)((s8 *)(arg0) + (4)));
    if (temp_f13_2 > lbl_8019FEE0[0]) {
        (*(f32 *)((s8 *)(arg0) + (4))) = (f32) (temp_f13_2 - lbl_80408E9C);
    }
    temp_r9 = (*(u8 *)((s8 *)(arg0) + (0x10)));
    if ((s32) temp_r9 != 0) {
        (*(u8 *)((s8 *)(arg0) + (0x10))) = (u8) (temp_r9 - 1);
    }
}

extern f32 lbl_801A1A08[];

s32 fn_800BA244(s32 *arg0)
{
  void *temp_r3;
  if (fn_800130F4(arg0) != 0)
  {
    temp_r3 = fn_8001384C((s32) arg0);
    if (temp_r3 != ((void *) 0))
    {
      *((f32 *) (((s8 *) temp_r3) + 0x4C)) = (f32) lbl_801A1A08[0];
      temp_r3++;
      temp_r3--;
      *((s32 *) (((s8 *) temp_r3) + 0x50)) = (s32) ((*((s32 *) (((s8 *) temp_r3) + 0x50))) & 0x7FFFFFFF);
      return 1;
    }
  }
  return 0;
}

extern s32 lbl_801A2194[];
extern s32 lbl_803EB120[];
extern u8 lbl_8040A67E;
extern s32 lbl_8040B158;

void fn_800BE6E4(void)
{
  struct nuvec_s sp8;
  s32 *new_var;
  s16 temp_r0;
  s32 var_r9;
  s8 *temp_r9;
  struct nugspline_s *temp_r31;
  struct nuvec_s *var_r30;
  void *temp_r3;
  var_r30 = (void *) 0;
  temp_r31 = lbl_80409410->unk1974;
  if (temp_r31 != ((void *) 0))
  {
    if (fn_800AAC04(&sp8, (void *) 0) != 0)
    {
      var_r30 = &sp8;
    }
    if (var_r30 != ((void *) 0))
    {
      temp_r9 = temp_r31->pts;
      temp_r0 = temp_r31->ptsize;
      if (fn_800CFD4C(*((f32 *) (&sp8)), sp8.z, *((f32 *) (((s8 *) temp_r9) + 0)), *((f32 *) (((s8 *) temp_r9) + 8)), *((f32 *) (((s8 *) (&temp_r9[temp_r0])) + 0)), *((f32 *) (((s8 *) (&temp_r9[temp_r0])) + 8))) != 0)
      {
        lbl_804093B8 = 0x2E;
      }
      else
      {
        lbl_804093B8 = 0x30;
      }
    }
  }
  new_var = lbl_803EB120;
  if (((fn_800130F4(new_var) == 0) || ((temp_r3 = fn_8001384C((s32) lbl_803EB120), (temp_r3 == ((void *) 0)) != 0))) || ((var_r9 = 1, (((s32) (*((s32 *) (((s8 *) temp_r3) + 0x50)))) < 0) == 0)))
  {
    var_r9 = 0;
  }
  if ((var_r9 != 0) && (((s32) lbl_8040A67E) == 0))
  {
    lbl_8040A67E = 1;
    fn_8006DCD4(lbl_801A2194, lbl_8040B158);
  }
}

extern s32 lbl_801A21A8[];
extern s32 lbl_801A21B4[];
extern s32 lbl_801A21C0[];
extern s32 lbl_801A21CC[];
extern s32 lbl_801A21D8[];
extern s32 lbl_801A21E4[];
extern s32 lbl_801A21F0[];
extern s32 lbl_801A21FC[];
extern s32 lbl_803EAEC4[];
extern s8 lbl_8040A683;

void MaulEInit(void)
{
  int new_var2;
  s32 var_ctr;
  long new_var;
  s8 *var_r9;
  new_var2 = 1;
  var_ctr = 4;
  var_r9 = &lbl_8040A683;
  do
  {
    *var_r9 = new_var2;
    var_r9 -= new_var2;
    var_ctr -= new_var2;
  }
  while (var_ctr != 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0)) = FindForceObject(lbl_80409410, lbl_801A21A8, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 4)) = FindForceObject(lbl_80409410, lbl_801A21B4, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 8)) = FindForceObject(lbl_80409410, lbl_801A21C0, 0);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0xC)) = FindForceObject(lbl_80409410, lbl_801A21CC, 0);
  new_var = 0;
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x10)) = FindForceObject(lbl_80409410, lbl_801A21D8, new_var);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x14)) = FindForceObject(lbl_80409410, lbl_801A21E4, new_var);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x18)) = FindForceObject(lbl_80409410, lbl_801A21F0, new_var);
  *((s32 *) (((s8 *) lbl_803EAEC4) + 0x1C)) = FindForceObject(lbl_80409410, lbl_801A21FC, new_var);
}

extern s32 lbl_8038E7A0[];
extern u8 lbl_803E0199[];
extern s32 lbl_8040AFAC;

void fn_800C10CC(void)
{
  struct nuvec_s sp8;
  s16 temp_r0;
  s32 var_r29;
  s8 *temp_r9;
  s8 var_r11;
  s32 *new_var;
  struct nugspline_s *temp_r31;
  struct nuvec_s *var_r30;
  var_r29 = 0x3B;
  if (((s32) lbl_804091F8) != 0)
  {
    var_r29 = 0x3D;
  }
  var_r30 = (void *) 0;
  temp_r31 = lbl_80409410->unk1974;
  if (temp_r31 != ((void *) 0))
  {
    if (fn_800AAC04(&sp8, (void *) 0) != 0)
    {
      var_r30 = &sp8;
    }
    if (var_r30 != ((void *) 0))
    {
      temp_r9 = temp_r31->pts;
      temp_r0 = temp_r31->ptsize;
      if (fn_800CFD4C(*((f32 *) (&sp8)), sp8.z, *((f32 *) (((s8 *) temp_r9) + 0)), *((f32 *) (((s8 *) temp_r9) + 8)), *((f32 *) (((s8 *) (&temp_r9[temp_r0])) + 0)), *((f32 *) (((s8 *) (&temp_r9[temp_r0])) + 8))) != 0)
      {
        lbl_804093B8 = 0x3A;
      }
      else
      {
        lbl_804093B8 = var_r29;
      }
    }
  }
  fn_800BE9B0();
  new_var = lbl_8038E7A0;
  var_r11 = 0;
  if ((((s32) lbl_8040AFAC) != 0) || (((s32) lbl_803E0199[0]) == 0x1E))
  {
    var_r11 = 1;
  }
  *((s8 *) (((s8 *) new_var) + 1)) = var_r11;
}

extern s32 lbl_801A1EA4[];
extern s32 lbl_801A1EAC[];
extern s32 lbl_801A1EB4[];
extern s32 lbl_801A1EBC[];
extern s32 lbl_803EB084[];
extern s32 lbl_803EB090[];
extern s32 lbl_803EB09C[];
extern s32 lbl_803EB0A8[];
extern s32 lbl_8040A668;
extern s32 lbl_8040A66C;
extern s32 lbl_8040A670;

void fn_800CD2B4(void)
{
  NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB084, lbl_801A1EA4, 1);
  NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB090, lbl_801A1EAC, 1);
  NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB09C, lbl_801A1EB4, 1);
  NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB0A8, lbl_801A1EBC, 1);
  lbl_8040B154 = 0;
  lbl_8040A668 = 0;
  lbl_8040A66C = 0;
 do { lbl_8040A670 = 0; } while (0);
}

extern s32 lbl_801A1F00[];
extern s32 lbl_801A1F10[];
extern s32 lbl_801A1F20[];
extern s32 lbl_803EB0B4[];
extern s32 lbl_803EB0C0[];
extern s32 lbl_803EB0CC[];
extern s32 lbl_8040922C;
extern s32 lbl_80409230;
extern s32 lbl_80409234;

void fn_800CD364(void)
{
  NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB0B4, lbl_801A1F00, 1);
  NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB0C0, lbl_801A1F10, 1);
  NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB0CC, lbl_801A1F20, 1);
  lbl_80409234 = 0;
 do { lbl_8040922C = 0; } while (0);
  lbl_80409230 = 0;
}

s32 fn_8006DCD4(s32 *, s32);
s32 fn_8014CAE8();
extern s32 lbl_801A25C8[];
extern f32 lbl_801A4ED4[];
extern f32 lbl_801A4ED8[];
extern s32 lbl_8040B18C;

void fn_800CDBF8(void)
{
  s32 new_var;
  f32 var_f0;
  fn_8014CAE8();
  if (((s32) lbl_8040B18C) != 2)
  {
    fn_8006DCD4(lbl_801A25C8, 0);
  }
  if (((void *) lbl_8040B19C) != ((void *) 0))
  {
    new_var = lbl_8040B19C;
    if (((s32) lbl_8040B18C) == 2)
    {
      var_f0 = lbl_801A4ED4[0];
    }
    else
    {
      var_f0 = lbl_801A4ED8[0];
    }
    *((f32 *) (((s8 *) new_var) + 0x28)) = var_f0;
  }
}

s32 memset(s32 *, ...);
extern s32 lbl_803EAAA0[];
extern s32 lbl_804091E0;

void fn_800BA2AC(void) {
    memset(lbl_803EAAA0, 0, 0xC0);
    lbl_804091E0 = 0;
}

s32 memset(s32 *, ...);
extern s32 lbl_803E8690[];
extern s32 lbl_8040A64C;

void fn_800BA444(void) {
    memset(lbl_803E8690, 0, 0x2200);
    lbl_8040A64C = 0;
}

s32 memset(s32 *, ...);
extern s32 lbl_802E0B9C[];

void fn_800CE408(void) {
    memset(lbl_802E0B9C, 0, 0x14);
}

s32 fn_800951FC(s32);

extern void *lbl_80409430;
extern s32 lbl_8040B058;

void fn_800D4C88(void) {
    memset(lbl_80409430, 0, 0x42600);
    fn_800951FC((*(s32 *)((s8 *)(lbl_80409410) + (0x2E0))));
    lbl_8040B058 = 0;
}

s32 fn_800AB8C8();
s32 fn_80105C74(s32);
s32 fn_801205AC(s32);
s32 fn_8013DE58(s32, ...);
s32 fn_8014093C(s32);

void fn_800CCAFC(s32 arg0) {
    fn_800AB8C8();
    fn_8013DE58(arg0);
    fn_80105C74(arg0);
    fn_801205AC(arg0);
    fn_8014093C(arg0);
}

extern s32 lbl_803E8284[];

s32 fn_800ABA50(void *arg0) {
    s32 temp_r3;

    temp_r3 = lbl_803E8284[(*(u8 *)((s8 *)(*(void **)((s8 *)arg0 + 0xEEC)) + 0x7B1)) & 7];
    if (temp_r3 == 0) {
        return lbl_803E8284[0];
    }
    return temp_r3;
}

extern u8 NuTimeBar_DefaultColours[];

s32 fn_800AC304(s32 arg0) {
    u8 *var_r9;
    u8 temp_r0;

    var_r9 = NuTimeBar_DefaultColours;
    if ((s32) NuTimeBar_DefaultColours[0] != 0xFF) {
loop_1:
        if ((s32) (*(u8 *)((s8 *)(var_r9) + (0))) == arg0) {
            return 1;
        }
        temp_r0 = (*(u8 *)((s8 *)(var_r9) + (1)));
        var_r9 += 1;
        if ((s32) temp_r0 == 0xFF) {
            /* Duplicate return node #4. Try simplifying control flow for better match */
            goto done;
        }
        goto loop_1;
    }
done:
    return 0;
}

s32 fn_8002B3A0(s32);
extern void *lbl_80408DF0;

void fn_800D4C00(s32 arg0) {
    s32 var_r30;
    s32 var_r31;
    void *var_r9;

    if (arg0 == 0) {
        var_r31 = (*(s32 *)((s8 *)(lbl_80408DF0) + (0xC)));
    } else {
        var_r31 = 0;
    }
    var_r9 = lbl_80408DF0;
    if (var_r31 < (s32) (*(s32 *)((s8 *)(var_r9) + (0x10)))) {
        var_r30 = var_r31 * 0x54;
        do {
            if ((s32) (*(s32 *)((s8 *)((var_r30 + (*(s32 *)((s8 *)(var_r9) + (0x18))))) + (4))) != 0) {
                fn_8002B3A0((*(s32 *)((s8 *)((var_r30 + (*(s32 *)((s8 *)(var_r9) + (0x18))))) + (4))));
            }
            var_r9 = lbl_80408DF0;
            var_r31 += 1;
            var_r30 += 0x54;
        } while (var_r31 < (s32) (*(s32 *)((s8 *)(var_r9) + (0x10))));
    }
}

extern f32 lbl_8019FF08[];
extern f32 lbl_8019FF0C[];
extern s32 lbl_8040AF5C;

f32 fn_800AC178(struct nuvec_s *arg0, f32 farg0, f32 farg1, f32 farg2) {
    struct nuvec_s sp8;
    f32 temp_f0;
    f32 temp_f1;
    f32 temp_f28;

    temp_f28 = lbl_8019FF08[0];
    if (!(farg2 <= temp_f28)) {
        NuVecSub(&sp8, arg0, lbl_8040AF5C + 0x30);
        temp_f0 = (*(f32 *)&(sp8));
        temp_f1 = (sp8.z * sp8.z) + ((temp_f0 * temp_f0) + (sp8.y * sp8.y));
        if (temp_f1 < (farg1 * farg1)) {
            if (temp_f1 > (farg0 * farg0)) {
                return farg2 * (lbl_8019FF0C[0] - ((NuFsqrt(temp_f1) - farg0) / (farg1 - farg0)));
            }
            return farg2;
        }
    }
    return temp_f28;
}

s32 Menu_CloseCurrent();
s32 fn_800C9648(s32, s32);

void Menu_Unknown10_HandleSelection(void *arg0) {
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) {
        fn_800C9648((*(s32 *)((s8 *)(lbl_80409410) + (0x240))) + (((*(s16 *)((s8 *)(arg0) + (0xA))) - (*(s16 *)((s8 *)(arg0) + (0xE)))) * 0x94), 0);
        return;
    }
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0) {
        Menu_CloseCurrent();
    }
}

s32 Menu_CloseCurrent();
extern s32 lbl_804090E0;
extern s32 lbl_804090F4;
extern s32 lbl_804093B0;
extern s32 lbl_8040AFF0;
extern s32 lbl_8040AFF8;

void SceneSelect_HandleSelection(void *arg0) {
    s32 temp_r31;

    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) {
        temp_r31 = *(s32 *)(&lbl_804090F4 + ((*(s32 *)((s8 *)(arg0) + (0x34)))));
        if ((s32) lbl_8040AFF8 == 0) {
            lbl_8040AFF0 = 1;
            lbl_8040AFF8 = 1;
        }
        SceneSelect_HandleEntry();
        lbl_804093B0 = temp_r31;
        lbl_804090E0 = 1;
        return;
    }
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0) {
        Menu_CloseCurrent();
    }
}

s32 fn_800D05E0(void *);
extern void *lbl_80409430;
extern s32 lbl_8040B058;

void fn_800AB85C(void) {
    s32 var_r30;
    void *var_r31;

    var_r30 = 0;
    var_r31 = lbl_80409430;
    for (; var_r30 < (s32)lbl_8040B058; var_r30 += 1, var_r31 += 0x1098) {
            if (((s32) (*(s32 *)((s8 *)(var_r31) + (0x20C))) < 0) && ((*(s32 *)((s8 *)(var_r31) + (0x208))) & 0x400)) {
                fn_800D05E0(var_r31);
            }
    }
}

s32 fn_800131B8(s32 *);
void fn_8006DE5C(s32, s32);
extern s32 lbl_80408FE0;

void fn_800BBED0(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s16 temp_r0_2;
    s32 var_r29;
    s8 *temp_r9;
    s8 *temp_r9_2;
    struct nugspline_s *temp_r31;
    struct nugspline_s *temp_r31_2;
    struct nuvec_s *var_r30;
    struct nuvec_s *var_r30_2;

    if ((s32) lbl_80408FE0 == 2) {
        lbl_804093B8 = lbl_80408FE0;
    } else if ((s32) lbl_80408FE0 == 1) {
        var_r30 = NULL;
        temp_r31 = lbl_80409410->unk1974;
        if (temp_r31 != NULL) {
            if (fn_800AAC04(&sp8, NULL) != 0) {
                var_r30 = &sp8;
            }
            if (var_r30 != NULL) {
                temp_r9 = temp_r31->pts;
                temp_r0 = temp_r31->ptsize;
                if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9) + (0))), (*(f32 *)((s8 *)(temp_r9) + (8))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (0))), (*(f32 *)((s8 *)(&temp_r9[temp_r0]) + (8)))) == 0) {
                    lbl_804093B8 = 5;
                } else {
                    goto block_16;
                }
            }
        }
    } else {
        var_r29 = 5;
        if ((s32) lbl_804091F8 != 0) {
            var_r29 = 6;
        }
        var_r30_2 = NULL;
        temp_r31_2 = lbl_80409410->unk1974;
        if (temp_r31_2 != NULL) {
            if (fn_800AAC04(&sp8, NULL) != 0) {
                var_r30_2 = &sp8;
            }
            if (var_r30_2 != NULL) {
                temp_r9_2 = temp_r31_2->pts;
                temp_r0_2 = temp_r31_2->ptsize;
                if (fn_800CFD4C((*(f32 *)&(sp8)), sp8.z, (*(f32 *)((s8 *)(temp_r9_2) + (0))), (*(f32 *)((s8 *)(temp_r9_2) + (8))), (*(f32 *)((s8 *)(&temp_r9_2[temp_r0_2]) + (0))), (*(f32 *)((s8 *)(&temp_r9_2[temp_r0_2]) + (8)))) != 0) {
block_16:
                    lbl_804093B8 = 2;
                } else {
                    lbl_804093B8 = var_r29;
                }
            }
        }
    }
    if ((fn_800130F4(lbl_802DA270) != 0) && (fn_800131B8(lbl_802DA270) == 0) && ((s32) lbl_8040A660 != -1) && ((s32) lbl_8040A664 != 0)) {
        fn_8006DE5C(lbl_8040A660, lbl_8040A664);
    }
}

extern f32 lbl_8019FE90[];
extern f32 lbl_8019FE94[];
extern f32 lbl_8019FE98[];
s32 SetPlayerOnSurface(void *arg0, s32 arg1)
{
  f32 temp_f0;
  f32 temp_f0_2;
  f32 temp_f0_3;
  f32 temp_f10;
  f32 temp_f11;
  f32 temp_f12;
  s32 var_r10;
  var_r10 = 0;
  if ((!((*((s32 *) (((s8 *) (*((void **) (((s8 *) lbl_80409410) + 0x1A8)))) + 4))) & 0x20)) && ((*((f32 *) (((s8 *) arg0) + 0x220))) != lbl_8019FE90[0]))
  {
    if (((s16) (*((s16 *) (((s8 *) arg0) + 0x1074)))) == 0xF)
    {
      temp_f0 = (*((f32 *) (((s8 *) arg0) + 0x101C))) * (*((f32 *) (((s8 *) arg0) + 0xB8)));
      *((f32 *) (((s8 *) arg0) + 0x70)) = (f32) ((*((f32 *) (((s8 *) arg0) + 0x70))) - temp_f0);
      *((f32 *) (((s8 *) arg0) + 0x94)) = (f32) ((*((f32 *) (((s8 *) arg0) + 0x94))) - temp_f0);
    }
    if (arg1 == 2)
    {
      temp_f0_2 = ((*((f32 *) (((s8 *) arg0) + 0x18C))) - ((*((f32 *) (((s8 *) arg0) + 0x101C))) * (*((f32 *) (((s8 *) arg0) + 0xB8))))) + lbl_8019FE94[0];
 do { *((f32 *) (((s8 *) arg0) + 0x94)) = temp_f0_2; } while (0);
      *((f32 *) (((s8 *) arg0) + 0x70)) = temp_f0_2;
      *((f32 *) (((s8 *) arg0) + 0xAC)) = temp_f0_2;
    }
    temp_f10 = *((f32 *) (((s8 *) arg0) + 0x220));
    temp_f11 = *((f32 *) (((s8 *) arg0) + 0x101C));
    temp_f12 = *((f32 *) (((s8 *) arg0) + 0xB8));
    if ((arg1 == 0) || (!(((temp_f11 * temp_f12) + (*((f32 *) (((s8 *) arg0) + 0x70)))) > (temp_f10 + lbl_8019FE94[0]))))
    {
      var_r10 = 1;
      temp_f0_3 = (temp_f10 - (temp_f11 * temp_f12)) + lbl_8019FE94[0];
      *((f32 *) (((s8 *) arg0) + 0x94)) = temp_f0_3;
      *((f32 *) (((s8 *) arg0) + 0x70)) = temp_f0_3;
      do { *((f32 *) (((s8 *) arg0) + 0xAC)) = temp_f0_3; } while (0);
    }
    *((f32 *) (((s8 *) arg0) + 0x7C)) = (f32) lbl_8019FE98[0];
  }
  return var_r10;
}

extern const f32 lbl_801A5268[];
extern const f32 lbl_801A526C[];
extern const f32 lbl_801A5270[];

s32 GameObjectNearFloor(void *arg0, f32 *arg1) {
    f32 temp_f10;
    f32 temp_f13;
    f32 var_f11;

    temp_f10 = (*(f32 *)((s8 *)(arg0) + (0x220)));
    if (temp_f10 != lbl_801A5268[0]) {
        var_f11 = ((*(f32 *)((s8 *)(arg0) + (0xBC))) / lbl_801A526C[0]) * lbl_801A5270[0];
        if (var_f11 < lbl_801A5270[0]) {
            var_f11 = lbl_801A5270[0];
        }
        temp_f13 = (((*(f32 *)((s8 *)(arg0) + (0x101C))) * (*(f32 *)((s8 *)(arg0) + (0xB8)))) + (*(f32 *)((s8 *)(arg0) + (0x70)))) - temp_f10;
        if (arg1 != NULL) {
            *arg1 = temp_f13;
        }
        if (temp_f13 < var_f11) {
            return 1;
        }
        /* Duplicate return node #9. Try simplifying control flow for better match */
        goto done;
    }
    if (arg1 != NULL) {
        *arg1 = lbl_801A5268[0];
    }
done:
    return 0;
}

void fn_800CCBC0(void) {
    if ((fn_800130F4(lbl_802DA264) != 0) && (fn_800131B8(lbl_802DA264) == 1) && ((s32) lbl_8040A658 != -1) && ((s32) lbl_8040A65C != 0)) {
        fn_8006DE5C(lbl_8040A658, lbl_8040A65C);
    }
}

void Menu_Open(s32, s32, s32);
extern s8 gDifficulty[];

void Menu_Unknown11_HandleSelection(void *arg0) {
    s8 temp_r9;

    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) {
        temp_r9 = (*(u8 *)((s8 *)(arg0) + (0x37))) + 1;
        gDifficulty[0] = temp_r9;
        Menu_Open(7, -1, -1);
        return;
    }
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0) {
        Menu_CloseCurrent();
    }
}

void *AISysFindArea(void *, void *);
extern s32 lbl_801A1CB0[];
extern s32 lbl_803EB004[];

void GunganBReset(void) {
    lbl_803EB004[0] = AISysFindArea((*(s32 *)((s8 *)(lbl_80409410) + (0x2E8))), lbl_801A1CB0);
}

s32 fn_801261FC(void *, s32);

s32 fn_800ABA78(void *arg0) {
    return fn_801261FC(arg0, 0);
}

f32 NuFsqrt(f32);
s16 fn_8000ABDC(f32, f32);
extern u16 lbl_8040B05C;
extern s16 lbl_8040B120;
extern s32 lbl_8040B124;


void fn_800CFE08(void *arg0, u16 *arg1, s16 *arg2) {
    f32 temp_f0;
    f32 temp_f1;
    f32 temp_f31;
    s16 temp_r3;

    temp_r3 = fn_8000ABDC((*(f32 *)((s8 *)(arg0) + (0))), (*(f32 *)((s8 *)(arg0) + (8))));
    lbl_8040B120 = temp_r3;
    if (arg2 != NULL) {
        *arg2 = temp_r3;
    }
    temp_f1 = (*(f32 *)((s8 *)(arg0) + (8)));
    temp_f0 = (*(f32 *)((s8 *)(arg0) + (0)));
    temp_f31 = (*(f32 *)((s8 *)(arg0) + (4)));
    lbl_8040B124 = -fn_8000ABDC(temp_f31, NuFsqrt((temp_f0 * temp_f0) + (temp_f1 * temp_f1)));
    lbl_8040B05C = ((u16 *)&lbl_8040B124)[1];
    if (arg1 != NULL) {
        *arg1 = ((u16 *)&lbl_8040B124)[1];
    }
}

f32 fn_8000ED40(f32);
extern const f32 lbl_801A0054[];
extern const f32 lbl_801A0058[];

typedef struct { char pad[0x130]; f32 f130; f32 f134; char pad2[0x176-0x138]; s8 b176; s8 b177; } AFFD8Obj;
void JudderGameCamera(AFFD8Obj *o, s8 arg1, s8 arg2, f32 farg0) {
    f32 temp_f1;
    f32 d;
    f32 v;

    temp_f1 = fn_8000ED40(farg0);
    if (temp_f1 > o->f130) {
        if (arg2 != 0) {
            d = NuVecDist((s8 *)o + 0xDC, (void *)arg2, 0);
            if (d < lbl_801A0054[0]) {
                v = temp_f1 * ((lbl_801A0054[0] - d) / lbl_801A0054[0]);
                o->f130 = o->f134 = v;
                if (farg0 < lbl_801A0058[0]) {
                    o->b176 = 1;
                } else {
                    o->b176 = 0;
                }
            }
        } else {
            o->f130 = o->f134 = temp_f1;
            if (farg0 < lbl_801A0058[0]) {
                o->b176 = 1;
            } else {
                o->b176 = arg2;
            }
        }
        o->b177 = arg1;
    }
}

s32 fn_800130F4(s32 *);
s32 fn_800B2ED4();
extern f32 lbl_801A1AD4[];
extern f32 lbl_801A1AD8[];
extern s32 lbl_803EAD24[];
extern f32 lbl_80408E9C;
extern f32 lbl_8040B14C;
extern f32 lbl_8040B150;

static inline void fn_800BB6D8_approach(f32 *v, f32 t) {
    if (*v > t) {
        *v -= 2.0f * lbl_80408E9C;
        if (*v < t) {
            *v = t;
        }
    } else if (*v < t) {
        *v += 2.0f * lbl_80408E9C;
        if (*v > t) {
            *v = t;
        }
    }
}

void fn_800BB6D8(void) {
    f32 t;
    s32 m;

    if (fn_800130F4(lbl_803EAD24) != 0) {
        m = fn_800B2ED4();
        if (m == -1 || m == 0 || m == 1 || m == 4) {
            t = lbl_801A1AD4[0];
        } else {
            t = lbl_801A1AD8[0];
        }
        fn_800BB6D8_approach(&lbl_8040B14C, t);
    }
    fn_800BB6D8_approach(&lbl_8040B150, lbl_801A1AD4[0]);
}

s32 fn_8006D1BC(s16, s32);
s32 fn_8006DCD4(s32 *, s32);
s32 fn_800B9928(f32 *, f32);
s32 fn_8010CA18(s32, s32 *, s32 *);
extern f32 NuRndrHackWaterLevel[];
extern const f32 lbl_801A0414[];
extern const f32 lbl_801A0418[];
extern s32 lbl_804086E8;
extern s32 lbl_804086EC;
extern s32 lbl_80408F7C;
extern s32 lbl_80408F80;
extern s32 lbl_80408FE0;
extern f32 lbl_804090EC;
extern s32 lbl_804091F8;
extern s32 lbl_804091FC;
extern s32 lbl_804093B0;
extern const s32 str_Confirm[];

void Menu_Main_HandleSelection(void *arg0) {
    if ((*(f32 *)((s8 *)(arg0) + (0))) >= lbl_804090EC) {
        fn_8010CA18(0, &lbl_80408F7C, &lbl_80408F80);
        if (((s32) lbl_804086E8 == 0) && ((s32) lbl_804086EC == 0)) {
            fn_8006D1BC((*(s16 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (0x3C))), 1);
        }
        (*(f32 *)((s8 *)(arg0) + (0))) = (f32) lbl_801A0414[0];
        fn_800B9928(NuRndrHackWaterLevel, lbl_801A0414[0]);
    }
    if (!(NuRndrHackWaterLevel[0] < lbl_801A0418[0])) {
        if ((s32) lbl_80408FE0 != 0) {
            if ((s32) (*(s32 *)((s8 *)(arg0) + (0x80))) != 0) {
                lbl_804091F8 = lbl_804091FC = lbl_804093B0 = 0;
                fn_8006DCD4(str_Confirm, 0);
            }
        } else if ((s32) (*(s32 *)((s8 *)(arg0) + (0x80))) != 0) {
            lbl_804093B0 = 0x7B;
            fn_8006DCD4(str_Confirm, 0);
        }
    }
}

extern s32 lbl_801A2614[];
extern s32 lbl_801D6524[];
extern s32 lbl_801D6530[];

void fn_800C12D0(void) {
    struct nuvec_s sp8;
    s16 temp_r0;
    s32 i;
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
                lbl_804093B8 = 0x44;
            } else {
                lbl_804093B8 = 0x46;
            }
        }
    }
    if ((s8) (u8) lbl_8040A68C != -1) {
        fn_8006DCD4(lbl_801A2614, (s32) lbl_801D6524);
        lbl_8040A68C = 0;
        for (i = 0; i < 8; i++) {
            if ((fn_800130F4(&lbl_803EB12C[i * 3]) != 0) && (fn_800131B8(&lbl_803EB12C[i * 3]) == 0)) {
                lbl_8040A68C++;
            }
        }
        if ((s32) (u8) lbl_8040A68C == 6) {
            lbl_8040A68C = -1;
        }
    } else {
        fn_8006DCD4(lbl_801A2614, (s32) lbl_801D6530);
    }
}

s32 FindForceObject(void *, s32 *, s32);
extern s32 lbl_801A23CC[];
extern s32 lbl_801A23E0[];
extern s32 lbl_801A23F4[];
extern s32 lbl_801A2408[];
extern s32 lbl_801A241C[];
extern s32 lbl_801A2430[];
extern s32 lbl_801A2444[];
extern s32 lbl_801A244C[];
extern s32 lbl_801A2460[];
extern s32 lbl_803EAE44[];
extern s32 lbl_803EAEC4[];
extern s32 lbl_8040B15C;
extern s32 lbl_8040B160;

void KaminoDInit(void) {
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = FindForceObject(lbl_80409410, lbl_801A23CC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = FindForceObject(lbl_80409410, lbl_801A23E0, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = FindForceObject(lbl_80409410, lbl_801A23F4, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0xC))) = FindForceObject(lbl_80409410, lbl_801A2408, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x10))) = FindForceObject(lbl_80409410, lbl_801A241C, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x14))) = FindForceObject(lbl_80409410, lbl_801A2430, 0);
    lbl_8040B15C = 0;
    lbl_8040B160 = AISysFindArea(((s32 *)lbl_80409410)[0xBA], lbl_801A2444);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x18))) = FindForceObject(lbl_80409410, lbl_801A244C, 0);
    lbl_803EAE44[0] = FindObstacle(lbl_80409410, lbl_801A2460);
}

f32 NuAngDiff(s32);
void fn_800769B4(void *, s32, s32);
extern f32 NuRndrHackWaterLevel[];
extern f32 lbl_801A0178[];
extern f32 lbl_801A017C[];
extern f32 lbl_801A0180[];
extern f32 lbl_801A0184[];
extern s32 lbl_803DED88[];
extern s32 lbl_80408A30;
extern s32 lbl_80408A54;
extern f32 lbl_804090E8;
extern f32 lbl_8040B0BC;
extern f32 str_MenuSelect[];

void Menu_Unknown1_BuildOptions(void *arg0)
{
  f32 var_f1;
  s32 temp_r30;
  s8 *tbl;
  temp_r30 = (s32) (((f32) ((f64) lbl_80408A54)) * lbl_8040B0BC);
  if (((s32) lbl_80408A30) == 0)
  {
    if (NuRndrHackWaterLevel[0] < lbl_801A0178[0])
    {
      var_f1 = lbl_801A017C[0];
    }
    else
      if (NuRndrHackWaterLevel[0] < lbl_801A0180[0])
    {
      var_f1 = NuAngDiff((s32) (2.0f * ((NuRndrHackWaterLevel[0] - lbl_801A0178[0]) * lbl_801A0184[0])));
    }
    else
    {
      var_f1 = str_MenuSelect[0];
    }
    tbl = (s8 *) lbl_803DED88;
    *((f32 *) (((s8 *) arg0) + 0x9C)) = (f32) (lbl_804090E8 - (*((f32 *) (((s8 *) arg0) + 0xA8))));
    temp_r30 = (s32) (((f32) ((f64) temp_r30)) * var_f1);
    fn_800769B4(arg0, *((s32 *) (tbl + 0xAF4)), (u8) temp_r30);
    fn_800769B4(arg0, *((s32 *) (tbl + 0xBD4)), (u8) temp_r30);
    fn_800769B4(arg0, *((s32 *) (tbl + 0xB20)), (u8) temp_r30);
  }
}

s32 NuSpecialFind(s32, s32 *, s32 *, s32);
s32 fn_800130F4(s32 *);
void fn_80013200(s32 *, s32);
s32 fn_80108A34(s32, s32);
s32 FindForceObject(void *, s32 *, s32);
extern s32 lbl_801A29D4[];
extern s32 lbl_801A29DC[];
extern s32 lbl_801A29E4[];
extern s32 lbl_801A29EC[];
extern s32 lbl_801A29F8[];
extern s32 lbl_801A2A04[];
extern s32 lbl_801A2A10[];
extern s32 lbl_801A2A1C[];
extern s32 lbl_801A2A28[];
extern s32 lbl_801A2A34[];
extern s32 lbl_801A2A40[];
extern s32 lbl_801A2A4C[];
extern s32 lbl_801A2A58[];
extern s32 lbl_801A2A64[];
extern s32 lbl_801A2A70[];
extern s32 lbl_801A2A78[];
extern s32 lbl_801A2A80[];
extern s32 lbl_801A2A88[];
extern s32 lbl_801A2A90[];
extern s32 lbl_801A2A98[];
extern s32 lbl_801A2AA0[];
extern s32 lbl_801A2AA8[];
extern s32 lbl_801D6774[];
extern s32 lbl_803EAD24[];
extern s32 lbl_803EAE14[];
extern s32 lbl_803EAE20[];
extern s32 lbl_803EAE44[];
extern s32 lbl_803EAEC4[];

typedef struct {
    s32 *name;
    char pad[0xE];
    s16 flag;
} LevelSpecialEntry;

void GunshipBInit(void) {
    s32 i;
    LevelSpecialEntry *tbl;

    lbl_803EAE44[0] = FindObstacle(lbl_80409410, lbl_801A29D4);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAE14, lbl_801A29DC, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAE20, lbl_801A29E4, 1);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0))) = FindForceObject(lbl_80409410, lbl_801A29EC, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (4))) = FindForceObject(lbl_80409410, lbl_801A29F8, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (8))) = FindForceObject(lbl_80409410, lbl_801A2A04, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0xC))) = FindForceObject(lbl_80409410, lbl_801A2A10, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x10))) = FindForceObject(lbl_80409410, lbl_801A2A1C, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x14))) = FindForceObject(lbl_80409410, lbl_801A2A04, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x18))) = FindForceObject(lbl_80409410, lbl_801A2A28, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x1C))) = FindForceObject(lbl_80409410, lbl_801A2A34, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x20))) = FindForceObject(lbl_80409410, lbl_801A2A40, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x24))) = FindForceObject(lbl_80409410, lbl_801A2A4C, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x28))) = FindForceObject(lbl_80409410, lbl_801A2A58, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x2C))) = FindForceObject(lbl_80409410, lbl_801A2A64, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x50))) = FindForceObject(lbl_80409410, lbl_801A2A70, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x54))) = FindForceObject(lbl_80409410, lbl_801A2A78, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x58))) = FindForceObject(lbl_80409410, lbl_801A2A80, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x5C))) = FindForceObject(lbl_80409410, lbl_801A2A88, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x60))) = FindForceObject(lbl_80409410, lbl_801A2A90, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x64))) = FindForceObject(lbl_80409410, lbl_801A2A98, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x68))) = FindForceObject(lbl_80409410, lbl_801A2AA0, 0);
    (*(s32 *)((s8 *)(lbl_803EAEC4) + (0x6C))) = FindForceObject(lbl_80409410, lbl_801A2AA8, 0);
    tbl = (LevelSpecialEntry *)lbl_801D6774;
    if (tbl[0].name != 0) {
        i = 0;
        do {
            NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), &lbl_803EAD24[i * 3], tbl[i].name, 1);
            if (fn_800130F4(&lbl_803EAD24[i * 3]) != 0) {
                fn_80013200(&lbl_803EAD24[i * 3], 0);
                tbl[i].flag = 0;
            }
            i++;
        } while (tbl[i].name != 0);
    }
    fn_80108A34(lbl_803EAE44[0], 0);
}

void fn_8014E9B8(void);
extern s32 gPlayer1Pointer[];
extern s32 gPlayerTotalStuds;
extern s32 gTrueJediTotal;
extern f32 lbl_801A5354[];
extern f32 lbl_801A5358[];
extern f32 lbl_801A535C[];
extern f32 lbl_801A5360[];
extern f32 lbl_801A5364[];
extern u8 lbl_803E028C[];
extern f32 lbl_80408E9C;
extern f32 lbl_80408F00;
extern s32 lbl_80409118;
extern s32 lbl_8040912C;
extern s32 lbl_804091F8;
extern f32 lbl_8040A6BC;
extern f32 lbl_8040A6C0;
extern f32 lbl_8040A6C4;
extern s32 lbl_8040AFAC;
extern f32 lbl_8040B0D0;
extern f32 lbl_8040B1BC;
extern f32 lbl_8040B1C0;
extern f32 lbl_8040B1C4;
extern f32 lbl_8040B1D8;
extern f32 lbl_8040B1DC;
extern f32 lbl_8040B1E0;

static inline void fn_800D64B0_approach(f32 *v, f32 t) {
    if (*v > t) {
        *v -= lbl_80408E9C;
        if (*v < t) {
            *v = t;
        }
    } else if (*v < t) {
        *v += lbl_80408E9C;
        if (*v > t) {
            *v = t;
        }
    }
}

#define FN_800D64B0_APPROACHK(v, k)                     \
    if ((v) > lbl_801A5358[0]) {                        \
        (v) -= lbl_80408E9C * (k);                      \
        if ((v) < lbl_801A5358[0]) {                    \
            (v) = lbl_801A5358[0];                      \
        }                                               \
    } else if ((v) < lbl_801A5358[0]) {                 \
        (v) += lbl_80408E9C * (k);                      \
        if ((v) > lbl_801A5358[0]) {                    \
            (v) = lbl_801A5358[0];                      \
        }                                               \
    }

void fn_800D64B0(void) {
    f32 t0;
    f32 t1;
    f32 t2;
    f32 t3;
    s32 n;
    void *p;

    if ((*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (4))) & 0x4002) {
        if ((lbl_80408F00 == lbl_801A5354[0]) && (lbl_8040AFAC == 0) && (lbl_80409118 == 0) && (((*(s32 *)((s8 *)(lbl_80409410) + (0x19C))) != 0x15) || (lbl_803E028C[0] != 5))) {
            t0 = lbl_801A5358[0];
        } else {
            t0 = lbl_801A535C[0];
        }
        fn_800D64B0_approach(&lbl_8040A6BC, t0);
        if (lbl_8040B1E0 > lbl_801A535C[0]) {
            lbl_8040B1E0 -= lbl_80408E9C;
        }
        if ((lbl_80408F00 == lbl_801A5354[0]) && (lbl_8040AFAC == 0) && ((lbl_8040912C != 0) || (lbl_8040B1E0 > lbl_801A535C[0]))) {
            t1 = lbl_801A5358[0];
        } else {
            t1 = lbl_801A535C[0];
        }
        fn_800D64B0_approach(&lbl_8040A6C0, t1);
        FN_800D64B0_APPROACHK(lbl_8040B1DC, lbl_801A5360[0])
        if (lbl_8040B1C4 > lbl_801A535C[0]) {
            lbl_8040B1C4 -= lbl_80408E9C;
        }
        if ((lbl_80408F00 == lbl_801A5354[0]) && (lbl_8040AFAC == 0) && ((lbl_8040912C != 0) || (lbl_8040B1C4 > lbl_801A535C[0]) || (gTrueJediTotal != 0))) {
            t2 = lbl_801A5358[0];
        } else {
            t2 = lbl_801A535C[0];
        }
        fn_800D64B0_approach(&lbl_8040B1BC, t2);
        p = (*(void **)((s8 *)(lbl_80409410) + (0x1AC)));
        if ((p != NULL) && ((*(u8 *)((s8 *)((*(void **)((s8 *)(p) + (0x40)))) + (2))) == 0)) {
            if (lbl_804091F8 != 0) {
                n = (*(s32 *)((s8 *)(p) + (0x1C)));
            } else {
                n = (*(s32 *)((s8 *)(p) + (0x18)));
            }
            if (n > 0) {
                if (gTrueJediTotal == 0) {
                    gPlayerTotalStuds = 0;
                    if ((*(void **)((s8 *)(gPlayer1Pointer) + (0))) != NULL) {
                        gPlayerTotalStuds = (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(gPlayer1Pointer) + (0)))) + (0xEEC)))) + (0x140)));
                    }
                    if ((*(void **)((s8 *)(gPlayer1Pointer) + (4))) != NULL) {
                        gPlayerTotalStuds += (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(gPlayer1Pointer) + (4)))) + (0xEEC)))) + (0x140)));
                    }
                    if (gPlayerTotalStuds >= n) {
                        gTrueJediTotal = 1;
                    }
                }
                FN_800D64B0_APPROACHK(lbl_8040B1C0, lbl_801A5364[0])
            }
        }
        if (lbl_8040B0D0 > lbl_801A535C[0]) {
            lbl_8040B0D0 -= lbl_80408E9C;
        }
        if ((lbl_80408F00 == lbl_801A5354[0]) && (lbl_8040AFAC == 0) && ((lbl_8040912C != 0) || (lbl_8040B0D0 > lbl_801A535C[0]))) {
            t3 = lbl_801A5358[0];
        } else {
            t3 = lbl_801A535C[0];
        }
        fn_800D64B0_approach(&lbl_8040A6C4, t3);
        FN_800D64B0_APPROACHK(lbl_8040B1D8, lbl_801A5364[0])
        if ((*(s32 *)((s8 *)(lbl_80409410) + (0x19C))) == 3) {
            if (((*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (4))) & 0x5000) == 0) {
                fn_8014E9B8();
            }
        }
    }
}

s32 fn_800BB3FC(s32, s32, f32);
s32 fn_8011E9C4(s32);
extern f32 lbl_801A2BA4[];
extern f32 lbl_80408E9C;
extern s32 lbl_804091F8;
extern u8 lbl_8040B060[];
extern f32 lbl_8040B0C8;

void fn_800C4EA0(void) {
    f32 temp_f0;
    f32 temp_f31;
    s32 var_r0;
    void *var_r3;

    if ((void *) lbl_80409348 == NULL) {
        lbl_80409348 = fn_800D515C(0x4A, 1, 1);
    }
    if ((s32) lbl_804091F8 != 0) {
        fn_800BB3FC(0x4A, 0, lbl_801A2BA4[0]);
        return;
    }
    temp_f31 = lbl_801A2BA4[0];
    switch ((s32) lbl_8040B060[0]) {
    case 0:
        var_r3 = fn_800D515C(0x4A, 1, 1);
        if ((var_r3 == NULL) || !((*(s32 *)((s8 *)(var_r3) + (0xF24))) & 2) || (((s32) (*(u8 *)((s8 *)(var_r3) + (0x28F))) == 0) && ((s32) (*(u8 *)((s8 *)(var_r3) + (0x1085))) != 0))) {
            var_r3 = NULL;
        }
        if (var_r3 == NULL) {
            goto block_17;
        }
        if (!(temp_f31 <= lbl_801A2BA4[0])) {
            lbl_8040B060[0] = 1;
            goto block_17;
        }
        goto block_16;
    case 1:
        temp_f0 = lbl_8040B0C8 + lbl_80408E9C;
        lbl_8040B0C8 = temp_f0;
        if (temp_f0 >= temp_f31) {
block_16:
            lbl_8040B060[0] = 2;
            var_r0 = 1;
            break;
        }
    default:
block_17:
        var_r0 = 0;
        break;
    }
    if (var_r0 != 0) {
        fn_8011E9C4(0x4D);
    }
}

extern s32 lbl_801A1F60[];
extern s32 lbl_801A1F74[];
extern s32 lbl_801A1F88[];
extern s32 lbl_801A1F98[];
extern s32 lbl_801A1FA8[];
extern s32 lbl_801A1FB4[];
extern s32 lbl_801A1FC0[];
extern s32 lbl_801A1FD0[];
extern s32 lbl_801A1FE0[];
extern s32 lbl_803EAED0[];
extern s32 lbl_803EB0D8[];
extern s32 lbl_803EB0E4[];
extern s32 lbl_803EB0F0[];
extern s32 lbl_803EB0FC[];
extern s32 lbl_803EB108[];
extern s32 lbl_803EB114[];
extern s32 lbl_80409238;
extern s32 lbl_8040923C;
extern s8 lbl_8040A674;
extern s8 lbl_8040A675;
extern s8 lbl_8040A676;
extern s8 lbl_8040A677;
extern s8 lbl_8040A678;
extern s8 lbl_8040A679;
extern s8 lbl_8040A67A;
extern s8 lbl_8040A67B;
extern s8 lbl_8040A67C;
extern s8 lbl_8040A67D;

void fn_800BD8E8(void) {
    lbl_8040A674 = 0;
    lbl_8040A675 = 0;
    lbl_8040A676 = 0;
    lbl_8040A677 = 0;
    lbl_8040A678 = 0;
    lbl_8040A679 = 0;
    lbl_8040A67A = 0;
    lbl_8040A67B = 0;
    lbl_8040A67C = 0;
    lbl_8040A67D = 0;
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB0D8, lbl_801A1F60, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB0E4, lbl_801A1F74, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB0F0, lbl_801A1F88, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB0FC, lbl_801A1F98, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB108, lbl_801A1FA8, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB114, lbl_801A1FB4, 1);
    lbl_803EAED0[0] = FindForceObject(lbl_80409410, lbl_801A1FC0, 0);
    lbl_80409238 = CheckAIMessage(lbl_80408F58, lbl_801A1FD0, 0);
    lbl_8040923C = CheckAIMessage(lbl_80408F58, lbl_801A1FE0, 0);
}

s32 fn_8000E250(s32, s32);
s32 fn_8000E3C0(s32, s32);
s32 fn_8000E540(s32, s32);

void fn_800D0374(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    switch (arg1) {
    case 0:
        if (arg2 != 0) fn_8000E250(arg0, arg2);
        if (arg3 != 0) fn_8000E3C0(arg0, arg3);
        if (arg4 != 0) fn_8000E540(arg0, arg4);
        break;
    case 1:
        if (arg3 != 0) fn_8000E3C0(arg0, arg3);
        if (arg2 != 0) fn_8000E250(arg0, arg2);
        if (arg4 != 0) fn_8000E540(arg0, arg4);
        break;
    case 2:
        if (arg3 != 0) fn_8000E3C0(arg0, arg3);
        if (arg4 != 0) fn_8000E540(arg0, arg4);
        if (arg2 != 0) fn_8000E250(arg0, arg2);
        break;
    case 3:
        if (arg4 != 0) fn_8000E540(arg0, arg4);
        if (arg2 != 0) fn_8000E250(arg0, arg2);
        if (arg3 != 0) fn_8000E3C0(arg0, arg3);
        break;
    }
}

s32 NewRumble(s32, f32);
s32 fn_800AC020(s32, f32);
s32 fn_801261FC(void *, s32);
extern s32 gPlayer1Pointer[];
extern f32 lbl_801A19A0[];
extern f32 lbl_801A19A4[];
extern void *lbl_80409430;
extern s32 lbl_8040B058;

void PartKill(void *arg0, s32 arg1) {
    s32 i;
    s8 *o;
    u8 f;
    s32 *pl;

    o = lbl_80409430;
    for (i = 0; i < lbl_8040B058; i++, o += 0x1098) {
        if (*(s32 *)(o + 0x20C) < 0 && *(void **)(*(s8 **)(o + 0xEEC) + 0x64C) == arg0) {
            fn_801261FC(o, 0);
        }
    }
    if (arg1 == 1) {
        f = *(u8 *)((s8 *)arg0 + 0x1EA);
        if ((s8)f != -1) {
            if ((f & 1) && (void *)gPlayer1Pointer[0] != NULL) {
                fn_800AC020(**(s32 **)(*(s8 **)(gPlayer1Pointer[0] + 0xEEC) + 0x61C), lbl_801A19A0[0]);
                NewRumble(**(s32 **)(*(s8 **)(gPlayer1Pointer[0] + 0xEEC) + 0x61C), lbl_801A19A4[0]);
            }
            if ((*(u8 *)((s8 *)arg0 + 0x1EA) & 2) && (pl = gPlayer1Pointer, (void *)pl[1] != NULL)) {
                fn_800AC020(**(s32 **)(*(s8 **)(pl[1] + 0xEEC) + 0x61C), lbl_801A19A0[0]);
                NewRumble(**(s32 **)(*(s8 **)(pl[1] + 0xEEC) + 0x61C), lbl_801A19A4[0]);
            }
        }
    }
}

f32 NuASin(s32);
f32 fn_8000CD08(void *, s32, struct nuvec_s *);
extern s32 PartKill_ForceThrow[];
extern s32 fn_800C5640[];
extern const f32 lbl_801A19D4[];
extern const f32 lbl_801A19D8[];
extern s32 lbl_80408888;
extern void *lbl_8040AEFC;

void *fn_800B9BE0(s32 arg0, struct nuvec_s *arg1, f32 farg0) {
    struct nuvec_s sp8;
    s32 ang;
    f32 d;
    f32 dot;
    f32 best;
    s32 i;
    void *found;
    s8 *o;

    found = NULL;
    o = lbl_8040AEFC;
    best = farg0;
    for (i = 0; i < lbl_80408888; i++, o += 0x1F4) {
        if (*(s32 *)(o + 0x128) < 0 && (*(s32 *)(o + 0x198) == (s32)fn_800C5640 || *(s32 *)(o + 0x1A8) == (s32)PartKill_ForceThrow)) {
            d = fn_8000CD08(o + 0x30, arg0, &sp8);
            if (d < farg0) {
                NuVecNorm(&sp8, &sp8);
                dot = NuVecDot(&sp8, arg1);
                if (d < lbl_801A19D4[0]) {
                    ang = 0x38E3;
                } else if (d < lbl_801A19D8[0]) {
                    ang = 0x238E;
                } else {
                    ang = 0xE38;
                }
                if (dot > NuASin(ang) && d < best) {
                    best = d;
                    found = o;
                }
            }
        }
    }
    return found;
}

s32 OpenDoorObstacle(s32, s32, s32, s32, f32);
s32 fn_8013F0AC(s32);
extern f32 lbl_801A20A8[];
extern u8 lbl_803EAF8E[];

void fn_800BE054(void) {
    s32 i;
    s32 all;

    all = 1;
    if (lbl_803EAF8E[0] == 0) {
        for (i = 0; i < 10; i++) {
            if (lbl_803EAE44[i] != 0 && !(*(s32 *)(lbl_803EAE44[i] + 0x18) & 0x1000)) {
                all = 0;
            }
        }
        if (all) {
            for (i = 0; i < 10; i++) {
                if (lbl_803EAE44[i] != 0) {
                    OpenDoorObstacle(0, lbl_803EAE44[i], 1, 1, lbl_801A20A8[0]);
                }
            }
            lbl_803EAF8E[0] = 1;
            fn_8013F0AC(0xA);
        }
    }
}

s32 NuSplineFind(s32, s32);
s32 NuStrCat(s32 *, s32 *);
s32 NuStrCpy(s32 *, s32);
s32 Shop_Init();
s32 InitMapAI();
s32 fn_800E1564();
s32 fn_800E1F88();
extern s32 lbl_801A3000[];
extern s32 lbl_801A3008[];
extern s32 lbl_801A300C[];
extern s32 lbl_801A3010[];
extern s32 lbl_801A3014[];
extern s32 lbl_801A3018[];
extern s32 lbl_801A3024[];
extern s32 lbl_801A3030[];
extern s32 lbl_801A303C[];
extern s32 lbl_801A3048[];
extern s32 lbl_801A3058[];
extern s32 lbl_801A3068[];
extern f32 lbl_801A3078[];
extern s32 lbl_801DC074[];
extern s32 lbl_802DA210[];

void MapEInit(void) {
    s32 buf[16];
    s32 i;
    s8 *o;

    o = (s8 *)lbl_801DC074;
    for (i = 0x16; i != 0; i--, o += 0x68) {
        if (*(s32 **)(o + 0x20) != NULL) {
            *(s32 *)(o + 0x44) = FindObstacle((s32) lbl_80409410, *(s32 **)(o + 0x20));
            *(s32 *)(o + 0x48) = FindObstacle((s32) lbl_80409410, *(s32 **)(o + 0x24));
        }
        if (*(s32 *)(o + 0x34) != 0) {
            memset((s32 *)(o + 0x4C), 0, 0xC);
            NuStrCpy(buf, *(s32 *)(o + 0x34));
            NuStrCat(buf, lbl_801A3000);
            NuSpecialFind(lbl_80409410->unk1C0, (s32 *)(o + 0x4C), buf, 1);
            memset((s32 *)(o + 0x58), 0, 0xC);
            NuStrCpy(buf, *(s32 *)(o + 0x34));
            NuStrCat(buf, lbl_801A3008);
            NuSpecialFind(lbl_80409410->unk1C0, (s32 *)(o + 0x58), buf, 1);
        }
        *(s32 *)(o + 0x64) = NuSplineFind(lbl_80409410->unk1C0, *(s32 *)(o + 0x28));
    }
    (*(s32 *)((s8 *)(lbl_803EAE44) + (0))) = FindObstacle((s32) lbl_80409410, lbl_801A300C);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (4))) = FindObstacle((s32) lbl_80409410, lbl_801A3010);
    (*(s32 *)((s8 *)(lbl_803EAE44) + (8))) = FindObstacle((s32) lbl_80409410, lbl_801A3014);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EAD24, lbl_801A3018, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EAD30, lbl_801A3024, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EAD3C, lbl_801A3030, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EAD48, lbl_801A303C, 1);
    fn_800CC6C4();
    Shop_Init();
    fn_800E1F88();
    fn_800E1564();
    lbl_8040B0C8 = lbl_801A3078[0];
    InitMapAI();
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA1F8, lbl_801A3048, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA204, lbl_801A3058, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_802DA210, lbl_801A3068, 1);
}

s32 NuSpecialFind(s32, s32 *, s32 *, s32);
s32 fn_8002A4B0(s32 *, s32 *, s32 *);
extern s32 lbl_801A161C[];
extern s32 lbl_801A1630[];
extern s32 lbl_801A1638[];
extern s32 lbl_801A1640[];
extern s32 lbl_801A1648[];
extern s32 lbl_803E8660[];
extern s32 lbl_80408F64;
extern s32 lbl_80408F6C;
extern s32 lbl_80408F70;

void InitStarField(void) {
    s32 sp8;
    s32 *p;

    sp8 = lbl_80408F70;
    lbl_80408F64 = fn_8002A4B0(&lbl_80408F6C, &sp8, lbl_801A161C);
    p = lbl_803E8660;
    memset(p, 0, 0x30);
    NuSpecialFind(lbl_80408F64, p, lbl_801A1630, 1);
    NuSpecialFind(lbl_80408F64, p + 3, lbl_801A1638, 1);
    NuSpecialFind(lbl_80408F64, p + 6, lbl_801A1640, 1);
    NuSpecialFind(lbl_80408F64, p + 9, lbl_801A1648, 1);
}

s32 fn_800D95B8(void *);
extern f32 lbl_801A4F00[];
extern s32 lbl_80409348;

void fn_800CE8FC(void) {
    if (((s32) lbl_80409348 != 0) && ((void *) lbl_80409344 != NULL) && ((*(f32 *)((s8 *)(lbl_80409344) + (0x28))) == lbl_801A4F00[0])) {
        fn_800D95B8((void *) lbl_80409348);
    }
}

void fn_800D6968(s32, s32, s32, f32, f32, f32, f32, f32, f32);
s32 fn_800D7B64(void *, s32, f32, f32, f32);
extern f32 lbl_801A54FC[];
extern f32 lbl_801A5500[];
extern f32 lbl_801A5504[];
extern f32 lbl_80408F00;
extern f32 lbl_80409534;
extern f32 lbl_80409538;
extern f32 lbl_8040953C;
extern f32 lbl_80409540;

s32 fn_800D95B8(void *arg0) {
    if (lbl_80408F00 == lbl_801A54FC[0]) {
        fn_800D6968(*(s16 *)((s8 *)arg0 + 0x1074), 0xCA, 1, lbl_801A5500[0], lbl_8040953C, lbl_801A5500[0], lbl_80409540, lbl_801A5504[0], lbl_801A5504[0]);
        fn_800D7B64(arg0, 0, lbl_801A5500[0], lbl_80409534, lbl_80409538);
    }
}

extern s32 lbl_801A2184[];
extern s32 lbl_801A218C[];

void fn_800CD794(void) {
    struct nuvec_s sp8;

    NuSpecialFind(lbl_80409410->unk1C0, &sp8, lbl_801A2184, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB120, lbl_801A218C, 1);
    lbl_8040B158 = fn_800133FC(&sp8) + 0x30;
    lbl_8040A67E = 0;
}

s32 Menu_CloseCurrent();
s32 PauseMenu_HandleEntry1(s32 *);
s32 fn_800B9D2C(s32);
s32 fn_800C8E38(s32);
extern s32 lbl_80409158;
extern s32 lbl_8040915C;
extern s32 lbl_8040AFF4;
extern const s32 str_Cancel[];
extern const s32 str_Confirm[];

void Menu_Unknown3_HandleSelection(void *arg0) {
    if (*(s32 *)((s8 *)arg0 + 0x68) != 0) {
        if (*(s32 *)((s8 *)arg0 + 0x34) == 0) {
            SceneSelect_HandleEntry();
            fn_800C8E38(lbl_80409410);
            fn_800B9D2C(lbl_8040AFF4);
            lbl_80409158 = 1;
            lbl_8040915C |= 1;
            *(s32 *)((s8 *)arg0 + 0x50) = PauseMenu_HandleEntry1((s32 *)str_Confirm);
            return;
        }
    } else if (*(s32 *)((s8 *)arg0 + 0x6C) == 0) {
        return;
    }
    Menu_CloseCurrent();
    *(s32 *)((s8 *)arg0 + 0x50) = PauseMenu_HandleEntry1((s32 *)str_Cancel);
}

extern s32 lbl_801A1CE0[];
extern s32 lbl_801A1CEC[];
extern s32 lbl_801A1CF8[];
extern s32 lbl_803EB014[];
extern s32 lbl_803EB020[];
extern s32 lbl_803EB02C[];
extern s32 lbl_80409214;
extern s32 lbl_80409218;
extern s32 lbl_8040921C;
extern s32 lbl_80409220;

void fn_800CD04C(void) {
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB014, lbl_801A1CE0, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB020, lbl_801A1CEC, 1);
    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EB02C, lbl_801A1CF8, 1);
    lbl_80409214 = 0;
    lbl_80409218 = 0;
    lbl_8040921C = 0;
    lbl_80409220 = 0;
}

extern s32 lbl_801A2B98[];
extern s32 lbl_803EACC4[];

void fn_800CE834(void) {
    u8 *p;

    NuSpecialFind(lbl_80409410->unk1C0, lbl_803EAD24, lbl_801A2B98, 1);
    if (fn_800130F4(lbl_803EAD24) != 0) {
        if (lbl_804091F8 != 0 || (p = (u8 *)lbl_803EACC4, p[0x4B] != 0 && p[0x4C] != 0)) {
            fn_80013200(lbl_803EAD24, 0);
        }
    }
}

int sprintf(void *, void *, ...);
extern s32 lbl_801A2C14[];
extern s32 lbl_801A2C1C[];
extern s32 lbl_801A2C24[];
extern s32 lbl_801A2C2C[];
extern s32 lbl_801A2C38[];
extern s32 lbl_801A2C44[];
extern s32 lbl_801A2C50[];
extern s32 lbl_801A2C5C[];
extern f32 lbl_801A2C68[];
extern s32 lbl_802DA1E8[];
extern s32 lbl_803E04F0[];
extern s32 lbl_803EAD24[];
extern s32 lbl_803EAD9C[];
extern s32 lbl_803EADD8[];
extern s32 lbl_803EADE4[];
extern s32 lbl_803EADF0[];
extern s32 lbl_803EADFC[];
extern s32 lbl_803EAE44[];
extern s32 lbl_803EAEC4[];
extern s32 lbl_803ED320[];
extern s32 lbl_8040935C;
extern f32 lbl_8040A6A4;

void KashyyykDInit(void) {
    s32 *p;
    s32 i;

    lbl_803EAEC4[0] = FindForceObject(lbl_80409410, lbl_801A2C14, 0);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EAD9C, lbl_801A2C1C, 1);
    for (i = 0; i < 3; i++) {
        lbl_803ED320[i] = 0;
        sprintf(lbl_803E04F0, lbl_801A2C24, i + 1);
        p = &lbl_803EAD24[i * 3];
        NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), p, lbl_803E04F0, 1);
        if (fn_800130F4(p) != 0) {
            fn_80013200(p, 1);
            lbl_802DA1E8[i] = fn_8001384C(p);
        }
    }
    lbl_8040A6A4 = lbl_801A2C68[0];
    lbl_8040935C = 0;
    lbl_803EAE44[0] = FindObstacle(lbl_80409410, lbl_801A2C2C);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EADD8, lbl_801A2C38, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EADE4, lbl_801A2C44, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EADF0, lbl_801A2C50, 1);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), lbl_803EADFC, lbl_801A2C5C, 1);
}

s32 SetAIMessage(s32, s32 *, s32, f32);
s32 CheckAIMessage(s32, s32 *, s32);
extern s32 lbl_801A20B4[];
extern s32 lbl_801A2AE4[];
extern s32 lbl_801A2AF0[];
extern f32 lbl_801A4EE8[];
extern s32 lbl_802E0B88[];
extern s32 lbl_80408F58;

void DookuCReset(void) {
    s32 *p;

    p = (s32 *)lbl_802E0B88;
    memset(p, 0, 0x14);
    p[0] = SetAIMessage(lbl_80408F58, lbl_801A2AE4, 0, lbl_801A4EE8[0]);
    p[1] = CheckAIMessage(lbl_80408F58, lbl_801A20B4, 0);
    NuSpecialFind((*(s32 *)((s8 *)(lbl_80409410) + (0x1C0))), p + 2, lbl_801A2AF0, 1);
}

s32 CloseDoorObstacle(s32, void *);
extern s32 lbl_802DA6D8[];

typedef struct {
    char pad[0x28];
    s8 *a[4];
    s32 n;
    s8 *b[4];
} C0438Tbl;

void fn_800C0438(void) {
    s32 i;
    C0438Tbl *t;

    t = (C0438Tbl *)lbl_802DA6D8;
    t->n = 0;
    for (i = 0; i < 4; i++) {
        *(u16 *)(t->a[i] + 0x6E) &= 0xFF3F;
        *(u16 *)(t->a[i] + 0x6E) |= 1;
        *(s8 *)(t->a[i] + 0x73) = 0xA;
        CloseDoorObstacle(0, t->b[i]);
        *(s32 *)(t->b[i] + 0x18) = 0x18;
    }
}

extern f32 lbl_801A5530[];
extern f32 lbl_801A5534[];
extern f32 lbl_80409A70;
extern f32 lbl_8040A6BC;
extern f32 lbl_8040A6C4;
extern f32 lbl_8040B0D0;
extern f32 lbl_8040B1BC;
extern f32 lbl_8040B1C0;
extern f32 lbl_8040B1C4;
extern f32 lbl_8040B1D8;
extern f32 lbl_8040B1DC;
extern f32 lbl_8040B1E0;
void fn_800D97BC(void) {
    lbl_8040A6BC = lbl_801A5530[0];
    lbl_80409A70 = lbl_801A5530[0];
    lbl_8040B1E0 = lbl_801A5530[0];
    lbl_8040B1DC = lbl_801A5534[0];
    lbl_8040B1C4 = lbl_801A5530[0];
    lbl_8040B1BC = lbl_801A5530[0];
    lbl_8040B1C0 = lbl_801A5534[0];
    lbl_8040B0D0 = lbl_801A5530[0];
    lbl_8040A6C4 = lbl_801A5530[0];
    lbl_8040B1D8 = lbl_801A5534[0];
}

extern s32 lbl_801A2DE8[];
extern f32 lbl_801A4F08[];
void VaderBReset(void) {
    lbl_80409368 = SetAIMessage(lbl_80408F58, lbl_801A2DE8, 0, lbl_801A4F08[0]);
}

extern f32 lbl_801A5248[];
void fn_800D4EC8(void *arg0, f32 arg1) {
    NewRumble(**(s32 **)(*(s8 **)((s8 *)arg0 + 0xEEC) + 0x61C), arg1);
    fn_800AC020(**(s32 **)(*(s8 **)((s8 *)arg0 + 0xEEC) + 0x61C), lbl_801A5248[0]);
}

extern f32 lbl_801A524C[];
extern f32 lbl_801A5250[];
void KillRumble(void *arg0) {
    NewRumble(**(s32 **)(*(s8 **)((s8 *)arg0 + 0xEEC) + 0x61C), lbl_801A524C[0]);
    fn_800AC020(**(s32 **)(*(s8 **)((s8 *)arg0 + 0xEEC) + 0x61C), lbl_801A5250[0]);
}

extern f32 lbl_801A5240[];
extern f32 lbl_801A5244[];
void fn_800D4E70(void *arg0) {
    NewRumble(**(s32 **)(*(s8 **)((s8 *)arg0 + 0xEEC) + 0x61C), lbl_801A5240[0]);
    fn_800AC020(**(s32 **)(*(s8 **)((s8 *)arg0 + 0xEEC) + 0x61C), lbl_801A5244[0]);
}

s32 VaderAStartCollapseStage();
s32 fn_800D7ED8(s32, s32, s32);
extern s32 lbl_802E0BBC[];
void fn_800CEEF0(void) {
    s32 *p;
    fn_800D7ED8(0, 0, 1);
    p = lbl_802E0BBC;
    if (*(s16 *)((s8 *)p + 8) != 0) {
        VaderAStartCollapseStage();
    }
    p[9] = 1;
}

s32 fn_80127518(s32);
extern s32 lbl_803EB044[];
void fn_800CD1F0(void) {
    s32 i;
    for (i = 0; i <= 14; i++) {
        fn_80127518(lbl_803EB044[i]);
    }
}

extern u8 lbl_801D53C8[];
s32 fn_800AC340(s32 arg0) {
    s32 i;
    u8 *p;
    i = 0;
    p = lbl_801D53C8;
    while (*p != 0xFF) {
        if (*p == arg0) {
            return i;
        }
        p += 0xC;
        i++;
    }
    return -1;
}

s32 fn_8011FB44(void *);
void fn_800CEE00(void) {
    void *p = (void *)lbl_803EAEC4[0];
    if (p != NULL && (*(u16 *)((s8 *)p + 0x20) & 0x101) == 0) {
        fn_8011FB44(p);
    }
}

extern s32 lbl_8038E7A0[];
extern u8 lbl_803E0199[];
extern s32 lbl_8040AFAC;
void fn_800CD8E8(void) {
    s8 *p;
    s8 v;
    p = (s8 *)lbl_8038E7A0;
    v = 0;
    if (lbl_8040AFAC == 0 && (s8)lbl_803E0199[0] != 5 && (s8)lbl_803E0199[0] != 6) {
        v = 1;
    }
    p[1] = v;
}

extern f32 lbl_801A5538[];
extern s16 lbl_801DCB5C[];
void fn_800D98B4(void) {
    s16 *p;
    p = lbl_801DCB5C;
    while (*p != -1) {
        *(f32 *)(p + 6) = lbl_801A5538[0];
        p += 8;
    }
    lbl_80409570 = 0;
}

s16 *fn_800D9878(s32 arg0) {
    s16 *p;
    p = lbl_801DCB5C;
    while (*p != -1) {
        if (*p == arg0) {
            return p;
        }
        p += 8;
    }
    return NULL;
}

s32 KillParts(void *, s32, s32, s32);
extern s32 lbl_80409444;
void fn_800D532C(void *arg0, s32 arg1) {
    s32 v;
    v = -1;
    if (*(s16 *)((s8 *)arg0 + 0x1074) == 0x71 && *(s8 *)((s8 *)arg0 + 0x1085) <= 1) {
        v = lbl_80409444;
    }
    KillParts(arg0, -1, v, arg1);
}

s32 fn_80044F28(s32, s32, f32, f32, f32);
extern f32 lbl_8019FEEC[];
s32 fn_800AC020(s32 arg0, f32 farg0) {
    if (arg0 != 0) {
        arg0 = fn_80044F28(arg0, 0, farg0, lbl_8019FEEC[0], lbl_8019FEEC[0]);
    }
    return arg0;
}

s32 fn_800E1F88();
extern s32 lbl_803E0298[];
extern s32 lbl_804093E0;
typedef struct DMH1Ent { u8 a; u8 b; u8 pad[6]; } DMH1Ent;
typedef struct DMH1Tbl { u8 pad[0x110]; DMH1Ent e[0x16]; } DMH1Tbl;
void DebugMenu_HandleEntry1(void) {
    s32 i;
    DMH1Tbl *t;
    t = (DMH1Tbl *)lbl_803E0298;
    for (i = 0; i < 0x16; i++) {
        t->e[i].a = 1;
        t->e[i].b = 1;
    }
    if (*(s32 *)((s8 *)lbl_80409410 + 0x198) == 0x7B) {
        fn_800E1F88();
    }
    lbl_804093E0 = 1;
}

typedef struct D4Tbl { u8 pad[0x14]; s32 a[8]; } D4Tbl;
extern f32 lbl_801A523C[];
void fn_800D4CD0(void *arg0) {
    f32 f;
    s8 idx;
    void *t;
    void *q;
    if (*(u8 *)((s8 *)arg0 + 0x107D) != 0) {
        t = *(void **)((s8 *)arg0 + 0xEEC);
        if (t != NULL && *(f32 *)((s8 *)t + 0x6AC) == lbl_801A523C[0]) {
            q = *(void **)((s8 *)*(void **)((s8 *)arg0 + 0x64) + 0x20);
            idx = *(s8 *)((s8 *)q + 0xDC);
            if (idx != -1 && (*(D4Tbl **)((s8 *)arg0 + 0x60))->a[idx] != 0
                && *(u8 *)((s8 *)arg0 + 0x290) != 0) {
                f = *(f32 *)((s8 *)arg0 + 0x1024) * *(f32 *)((s8 *)q + 0x6C);
                *(f32 *)((s8 *)arg0 + 0x1F0) = f;
                *(f32 *)((s8 *)arg0 + 0x1EC) = f;
                return;
            }
        }
    }
    *(f32 *)((s8 *)arg0 + 0x1EC) = *(f32 *)((s8 *)arg0 + 0xBC);
    *(f32 *)((s8 *)arg0 + 0x1F0) = *(f32 *)((s8 *)arg0 + 0xC0);
}

s16 NuPortalWhichRoom(void *, void *);
typedef struct AC728Flags { u32 pad : 25; u32 b40 : 1; u32 b20 : 1; u32 lo : 5; } AC728Flags;
void fn_800AC728(void *arg0, s32 arg1, s32 arg2, s16 arg3) {
    s16 r;
    AC728Flags *fl = (AC728Flags *)((s8 *)arg0 + 0xF24);
    if (arg1 != 0) {
        fl->b40 = 1;
        fl->b20 = arg2;
        if (arg3 != -1) {
            *(s16 *)((s8 *)arg0 + 0x1078) = arg3;
            return;
        }
        if (*(void **)((s8 *)lbl_80409410 + 0x1C0) != NULL) {
            r = NuPortalWhichRoom(*(void **)((s8 *)lbl_80409410 + 0x1C0), (s8 *)arg0 + 0x90);
        } else {
            r = -1;
        }
        *(s16 *)((s8 *)arg0 + 0x1078) = r;
        return;
    }
    fl->b40 = 0;
}

s32 fn_800130F4(s32 *);
extern s32 lbl_802DA1F8[];
void fn_800CC500(void) {
    s32 i;
    for (i = 0; i <= 7; i++) {
        if (fn_800130F4(lbl_802DA1F8 + i * 3) != 0) {
            fn_80013200(lbl_802DA1F8 + i * 3, 0);
        }
    }
}

extern s32 lbl_802E0BBC[];
void fn_800CEF44(void) {
    s8 *p = (s8 *)lbl_802E0BBC;
    if (*(s16 *)(p + 8) != 0 && *(s16 *)(p + 8) <= 2) {
        fn_800D7ED8((s32)*(f32 *)(p + 4) + 1, *(s16 *)(p + 0xA), 0);
        *(s16 *)(p + 0xA) = 0;
    }
}

extern f32 lbl_801A4EE0[];
extern s32 lbl_801D6774[];
void fn_800CE144(void) {
    u8 *p = (u8 *)lbl_8040B060;
    p[0] = 0;
    p[1] = 0;
    lbl_8040B168 = lbl_801A4EE0[0];
    fn_800C3A24(lbl_801D6774);
}

extern f32 lbl_801A4F10[];
extern void *lbl_802E0BE4[];
void fn_800CF03C(void) {
    s32 v;
    if (lbl_802E0BE4[0] != NULL && *(f32 *)((s8 *)lbl_802E0BE4[0] + 0x28) == lbl_801A4F10[0] && lbl_80409008 == NULL) {
        if ((s32)lbl_80409004 == gPlayer1Pointer[0]) {
            v = gPlayer1Pointer[1];
        } else {
            v = gPlayer1Pointer[0];
        }
        if (v != 0) {
            fn_800D95B8(v);
        }
    }
}

extern f32 lbl_801A4EF0[];
extern s32 lbl_802E0B9C[];
void fn_800CE43C(void) {
    s32 *p = lbl_802E0B9C;
    if (p[1] != 0 && (void *)p[0] != NULL && *(f32 *)((s8 *)p[0] + 0x28) == lbl_801A4EF0[0]) {
        fn_800D95B8((void *)p[1]);
    }
}

s32 fn_800C8CE4();
extern s32 lbl_8040B110;
extern s32 lbl_8040B190;
void fn_800CC3FC(s32 arg0) {
    s32 t = arg0;
    if (t != 0 && t != lbl_8040B110) {
        NuStrCpy((s32 *)lbl_801DC964, t);
        fn_800C8CE4();
        lbl_8040B110 = t;
        lbl_8040B190 = t;
    }
}

extern f32 lbl_801A4F9C[];
extern f32 lbl_80408E9C;
u16 SeekRot(s32 arg0, s32 arg1, f32 farg0) {
    f32 f;
    s32 d;
    f = farg0 * lbl_80408E9C;
    if (f > lbl_801A4F9C[0]) {
        f = lbl_801A4F9C[0];
    }
    d = arg1 - arg0;
    if (d > 0x8000) {
        d -= 0x10000;
    } else if (d < -0x8000) {
        d += 0x10000;
    }
    return (u16)(s32)((f32)arg0 + (f32)d * f);
}

extern f32 lbl_801A511C[];
extern f32 lbl_801A5120[];
extern void *lbl_80408DF0;
extern void *lbl_804093A8;
extern s32 lbl_8040AFC4;
void fn_800D3ACC(void *arg0) {
    s8 *a = arg0;
    f32 x, hi, lo;
    s32 v;
    s8 *t;
    if (lbl_8040AFC4 != 0 && (x = *(f32 *)(a + 0x1048)) < (hi = *(f32 *)((s8 *)lbl_804093A8 + 0x84))) {
        lo = *(f32 *)((s8 *)lbl_804093A8 + 0x80);
        if (x > lo) {
            *(f32 *)(a + 0x1050) = lbl_801A5120[0] - (x - lo) / (hi - lo);
        } else {
            *(f32 *)(a + 0x1050) = lbl_801A5120[0];
        }
    } else {
        *(f32 *)(a + 0x1050) = lbl_801A511C[0];
    }
    if (lbl_8040AFC4 > 0 && *(s16 *)(a + 0x1074) != 0x75) {
        if (*(u8 *)(a + 0x28F) != 0
            && (v = *(s32 *)(*(s8 **)(*(s32 *)((s8 *)lbl_80408DF0 + 0x20) + **(s16 **)(a + 0x60) * 0x48 + 0x20) + 0xAC)) != 0) {
            *(s32 *)(a + 0x1060) = v;
        } else {
            x = *(f32 *)(a + 0x1048);
            t = *(s8 **)(*(s8 **)(a + 0x64) + 0x20);
            if (x > *(f32 *)(t + 0xB4)) {
                *(s32 *)(a + 0x1060) = *(s32 *)(t + 0xA8);
            } else if (x > *(f32 *)(t + 0xB0)) {
                *(s32 *)(a + 0x1060) = *(s32 *)(t + 0xA4);
            } else {
                *(s32 *)(a + 0x1060) = *(s32 *)(t + 0xA0);
            }
        }
        switch (*(s16 *)(a + 0x1074)) {
        case 0x71:
            if (*(s8 *)(a + 0x1085) <= 1) {
                *(s32 *)(a + 0x1060) &= ~0x10;
            }
            break;
        case 0x43:
            if (*(s32 *)(a + 0xF28) & 0x40000) {
                *(s32 *)(a + 0x1060) |= 0x20;
            } else {
                *(s32 *)(a + 0x1060) |= 0x40;
            }
            break;
        }
    }
}

extern f32 lbl_8019FEF8[];
extern f32 lbl_8019FEFC[];

void fn_800AC058(s32 arg0, s32 arg1) {
    if (arg0 != 0) {
        fn_80044F28(arg0, 0, (f32) (f64) arg1 / lbl_8019FEF8[0], lbl_8019FEFC[0], lbl_8019FEFC[0]);
    }
}

extern s32 lbl_8040913C;
extern s32 lbl_804093C0;

s32 fn_800B9D2C(s32 arg0) {
    lbl_8040913C = arg0;
    lbl_804093C0 = (-1 - arg0) == 0;
}

extern u8 lbl_802DA6BD[];

s32 fn_800CC610(void) {
    return (lbl_802DA6BD[0] ^ 2) == 0;
}

extern s16 lbl_803ED3AC[];

s32 fn_800CC6E4(void) {
    return (lbl_803ED3AC[0] ^ 4) == 0;
}

s32 fn_800D5408(void *arg0) {
    return ((*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) ^ 0x24) == 0;
}

#define Menu_Open ((s32 (*)(s32, s32, s32))Menu_Open)
void Menu_ResetStack(void);
s32 Shop_UpdateMenu();
void fn_80078E90(void *);
s32 fn_8007AC44();
int memcmp(void *, void *, u32);
extern s32 gMenuDepth;
extern s32 gMenuStack[];
extern f32 lbl_801A03B0[];
extern f32 lbl_801A03B4[];
extern f32 lbl_801A03B8[];
extern s32 lbl_803E8430[];
extern s32 lbl_80409124;
extern s32 lbl_80409B98;
extern f32 lbl_8040AFA0;
extern f32 lbl_8040B0C8;
extern s32 lbl_8040B0CC;
extern f32 lbl_8040B0D0;
extern f32 lbl_8040B0D4;
extern s32 str_menuBack2[];

void Menu_Unknown12_HandleSelection(void) {
    f32 temp_f31;
    s32 temp_r31;

    temp_r31 = lbl_80409124;
    if (temp_r31 != 0) {
        lbl_8040B0C8 = lbl_801A03B0[0];
        lbl_8040B0CC = 1;
        lbl_8040B0D0 = lbl_801A03B4[0];
        fn_80078E90((gMenuDepth * 0xE0) + ((s8 *)gMenuStack));
        Menu_ResetStack();
        lbl_80409124 = 0;
        lbl_8040AFA0 = lbl_801A03B8[0];
        return;
    }
    if (Shop_UpdateMenu() != 0) {
        fn_8006DCD4(str_menuBack2, 0);
        temp_f31 = lbl_801A03B8[0];
        lbl_80409B98 = temp_r31;
        lbl_8040B0D4 = temp_f31;
        if ((memcmp(lbl_803E8430, lbl_803E0298, 0x210) != 0) && (fn_8007AC44() == 0)) {
            lbl_80409124 = 1;
            Menu_Open(0x3E8, -1, -1);
            return;
        }
        lbl_8040B0C8 = lbl_801A03B0[0];
        lbl_8040B0CC = 1;
        lbl_8040B0D0 = lbl_801A03B4[0];
        fn_80078E90((gMenuDepth * 0xE0) + ((s8 *)gMenuStack));
        Menu_ResetStack();
        lbl_8040AFA0 = temp_f31;
    }
}
#undef Menu_Open

#define fn_800B9ED4 ((s32 (*)(void))fn_800B9ED4)
extern const f32 lbl_8019FEC0[];
extern const f32 lbl_8019FED0[];
extern const f32 lbl_8019FED4[];
extern const f32 lbl_8019FED8[];

void fn_800ABEB0(void *arg0, s32 arg1) {
    f32 temp_f31;

    temp_f31 = lbl_8019FEC0[0];
    (*(s16 *)((s8 *)(arg0) + (0x50))) = 1;
    (*(f32 *)((s8 *)(arg0) + (0x24))) = temp_f31;
    (*(f32 *)((s8 *)(arg0) + (0x28))) = (f32) (((f32) (f64) fn_800B9ED4() * lbl_8019FED0[0] * lbl_8019FED4[0]) + lbl_8019FED8[0]);
    if (arg1 > 0) {
        (*(f32 *)((s8 *)(arg0) + (0x20))) = temp_f31;
        if (arg1 > 1) {
            (*(s16 *)((s8 *)(arg0) + (0x52))) = -1;
        }
    }
}
#undef fn_800B9ED4

void fn_800D4D64(void *);

void fn_800ABC50(void *arg0) {
    f32 temp_f0;
    f32 temp_f13;
    void *temp_r10;

    temp_r10 = (*(void **)((s8 *)(arg0) + (0x64)));
    (*(u8 *)((s8 *)(arg0) + (0x1087))) = (u8) (*(u8 *)((s8 *)((*(void **)((s8 *)(temp_r10) + (0x20)))) + (0xE0)));
    (*(f32 *)((s8 *)(arg0) + (0xB8))) = (f32) (*(f32 *)((s8 *)(temp_r10) + (0x38)));
    (*(f32 *)((s8 *)(arg0) + (0x1028))) = (f32) (*(f32 *)((s8 *)(temp_r10) + (0x2C)));
    temp_f0 = (*(f32 *)((s8 *)(temp_r10) + (0x30)));
    (*(f32 *)((s8 *)(arg0) + (0x1014))) = temp_f0;
    temp_f13 = (*(f32 *)((s8 *)(temp_r10) + (0x34)));
    (*(f32 *)((s8 *)(arg0) + (0x101C))) = temp_f0;
    (*(f32 *)((s8 *)(arg0) + (0x1018))) = temp_f13;
    (*(f32 *)((s8 *)(arg0) + (0x1020))) = temp_f13;
    fn_800D4D64(arg0);
}

s32 fn_800D9824(s32 arg0, s32 arg1) {
    u8 *var_r9;
    s32 var_ctr;
    s32 var_r3;

    var_ctr = 0x40;
    var_r9 = (u8 *)lbl_803FD4E0;
    var_r3 = 0;
    do {
        if (((s32) (*(u8 *)((s8 *)(var_r9) + (0x8B))) != 0) && ((s8) (*(u8 *)((s8 *)(var_r9) + (0x93))) == arg0)) {
            var_r3 = 1;
            if (arg1 != 0) {
                (*(u8 *)((s8 *)(var_r9) + (0x8B))) = 0U;
            }
        }
        var_r9 += 0x9C;
        var_ctr -= 1;
    } while (var_ctr != 0);
    return var_r3;
}

extern s32 MenuHeader[];
extern f32 lbl_801A0060[];
extern f32 lbl_801A0064[];
extern s32 lbl_80409088;
extern f32 lbl_8040993C;
extern s32 lbl_8040B06C;
extern f32 lbl_8040B070;
extern f32 lbl_8040B074;
extern f32 lbl_8040B078;
extern f32 lbl_8040B07C;
extern f32 lbl_8040B088;
extern s32 lbl_8040B08C;

void fn_800B0120(s32 arg0, s32 arg1, f32 farg0, f32 farg1, f32 farg2, f32 farg3) {
    if (arg0 != 0) {
        lbl_8040B06C = arg0;
        lbl_8040B074 = farg1;
        lbl_8040B078 = farg3;
        lbl_80409088 = 0;
        lbl_8040B07C = farg0;
        lbl_8040B070 = lbl_801A0060[0];
        lbl_8040B088 = farg2;
        lbl_8040B08C = arg1;
        if ((arg1 != 0) && (farg0 == lbl_801A0060[0]) && ((farg2 <= lbl_801A0060[0]))) {
            lbl_8040993C = lbl_801A0064[0];
        }
        if (((lbl_8040B07C <= lbl_801A0060[0])) && ((lbl_8040B088 <= lbl_801A0060[0]))) {
            s8 *p = (s8 *)MenuHeader;
            *(s8 *)(p + 0x174) = -1;
            *(f32 *)(p + 0x138) = *(f32 *)(p + 0x13C);
        }
    }
}

extern s32 lbl_801A1D14[];
extern s32 lbl_801A1D28[];
extern f32 lbl_801A1D3C[];
extern f32 lbl_801A1D40[];
extern f32 lbl_801A1D44[];
extern f32 lbl_801A1D48[];
extern f32 lbl_801A1D4C[];
extern f32 lbl_801A1D50[];
extern f32 lbl_801A1D54[];
extern f32 lbl_801A1D58[];
extern f32 lbl_801A1D5C[];
extern f32 lbl_801A1D60[];

void RescueCAlwaysUpdate(void)
{
  struct nuvec_s sp8;
  f32 temp_f31;
  f32 temp_f31_3;
  s16 temp_r0;
  s8 *temp_r9;
  struct nugspline_s *temp_r31;
  struct nuvec_s *var_r30;
  void *temp_r3;
  void *temp_r3_2;
  void *temp_r3_3;
  var_r30 = (void *) 0;
  temp_r31 = lbl_80409410->unk1974;
  if (temp_r31 != ((void *) 0))
  {
    if (fn_800AAC04(&sp8, (void *) 0) != 0)
    {
      var_r30 = &sp8;
    }
    if (var_r30 != ((void *) 0))
    {
      temp_r9 = temp_r31->pts;
      temp_r0 = temp_r31->ptsize;
      if (fn_800CFD4C(*((f32 *) (&sp8)), sp8.z, *((f32 *) (((s8 *) temp_r9) + 0)), *((f32 *) (((s8 *) temp_r9) + 8)), *((f32 *) (((s8 *) (&temp_r9[temp_r0])) + 0)), *((f32 *) (((s8 *) (&temp_r9[temp_r0])) + 8))) != 0)
      {
        lbl_804093B8 = 0x12;
      }
      else
      {
        lbl_804093B8 = 0x14;
      }
    }
  }
  if (fn_800130F4(lbl_803EB014) != 0)
  {
    temp_r3 = fn_8001384C((s32) lbl_803EB014);
    if ((temp_r3 != ((void *) 0)) && ((((temp_f31 = *((f32 *) (((s8 *) temp_r3) + 0x4C)), temp_f31 >= lbl_801A1D3C[0])) && (temp_f31 <= lbl_801A1D40[0])) || ((temp_f31 >= lbl_801A1D44[0]) && (temp_f31 <= lbl_801A1D48[0]))))
    {
      fn_8006DCD4(lbl_801A1D04, fn_800133FC(lbl_803EB014) + 0x30);
    }
  }
  if (fn_800130F4(lbl_803EB020) != 0)
  {
    temp_r3_2 = fn_8001384C((s32) lbl_803EB020);
    if (temp_r3_2 != ((void *) 0))
    {
      temp_f31 = *((f32 *) (((s8 *) temp_r3_2) + 0x4C));
      if (((temp_f31 >= lbl_801A1D4C[0]) && (temp_f31 <= lbl_801A1D50[0])) || ((temp_f31 >= lbl_801A1D54[0]) && (temp_f31 <= lbl_801A1D58[0])))
      {
        lbl_80409214 = 0;
      }
      if ((temp_f31 >= lbl_801A1D5C[0]) && (temp_f31 <= lbl_801A1D60[0]))
      {
        lbl_80409218 = 0;
      }
      if (((((s32) lbl_80409214) == 0) && (temp_f31 >= lbl_801A1D3C[0])) && (temp_f31 <= lbl_801A1D40[0]))
      {
        fn_8006DCD4(lbl_801A1D14, fn_800133FC(lbl_803EB020) + 0x30);
        lbl_80409214 = 1;
      }
      if (((((s32) lbl_80409218) == 0) && (temp_f31 >= lbl_801A1D44[0])) && (temp_f31 <= lbl_801A1D48[0]))
      {
        fn_8006DCD4(lbl_801A1D28, fn_800133FC(lbl_803EB020) + 0x30);
        lbl_80409218 = 1;
      }
    }
  }
  if (fn_800130F4(lbl_803EB02C) != 0)
  {
    temp_r3_3 = fn_8001384C((s32) lbl_803EB02C);
    if (temp_r3_3 != ((void *) 0))
    {
      temp_f31_3 = *((f32 *) (((s8 *) temp_r3_3) + 0x4C));
      if (((temp_f31_3 >= lbl_801A1D4C[0]) && (temp_f31_3 <= lbl_801A1D50[0])) || ((temp_f31_3 >= lbl_801A1D54[0]) && (temp_f31_3 <= lbl_801A1D58[0])))
      {
        lbl_8040921C = 0;
      }
      if ((temp_f31_3 >= lbl_801A1D5C[0]) && (temp_f31_3 <= lbl_801A1D60[0]))
      {
        lbl_80409220 = 0;
      }
      if (((((s32) lbl_8040921C) == 0) && (temp_f31_3 >= lbl_801A1D3C[0])) && (temp_f31_3 <= lbl_801A1D40[0]))
      {
        fn_8006DCD4(lbl_801A1D14, fn_800133FC(lbl_803EB02C) + 0x30);
        lbl_8040921C = 1;
      }
      if (((((s32) lbl_80409220) == 0) && (temp_f31_3 >= lbl_801A1D44[0])) && (temp_f31_3 <= lbl_801A1D48[0]))
      {
        fn_8006DCD4(lbl_801A1D28, fn_800133FC(lbl_803EB02C) + 0x30);
        lbl_80409220 = 1;
      }
    }
  }
}

void fn_800BA740(void) { volatile s32 pad[3]; }

void fn_800ABCAC(void *arg0, s32 arg1) {
    u8 max = *(u8 *)((s8 *)arg0 + 0x1084);
    *(s8 *)((s8 *)arg0 + 0x1085) = arg1;
    if ((s8)arg1 > max) {
        *(s8 *)((s8 *)arg0 + 0x1085) = max;
    }
}

extern f32 lbl_801A5274[];
void fn_800D5530(void *arg0, f32 farg0) {
    f32 lim = lbl_801A5274[0];
    f32 v = *(f32 *)((s8 *)arg0 + 0x1EC) - farg0;
    *(f32 *)((s8 *)arg0 + 0x1EC) = v;
    if (v <= lim) {
        *(s32 *)((s8 *)arg0 + 0x198) = 0;
        *(s32 *)((s8 *)arg0 + 0x1B8) = 0;
    }
}

s16 fn_800D99AC(void) { return lbl_80409570 ? *(s16 *)lbl_80409570 : -1; }

extern f32 lbl_801A5278[];
s32 fn_800D5560(void *arg0) { f32 c = lbl_801A5278[0]; s32 r = 0; if (!(*(f32 *)((s8 *)arg0 + 0x1EC) > c)) r = 1; return r; }

extern f32 lbl_801A5524[];
extern f32 lbl_801A5528[];
extern f32 lbl_801A552C[];
extern f32 lbl_80409450;
extern f32 lbl_80409454;
extern f32 lbl_8040B094;
extern f32 lbl_8040B098;
void fn_800D9784(void) {
    f32 a = lbl_801A5524[0];
    f32 b = lbl_801A552C[0];
    lbl_8040B094 = b / (lbl_80409454 * lbl_801A5528[0]);
    lbl_8040B098 = a / lbl_80409450;
}

extern f32 lbl_801A4F98[];
s32 fn_800CFD4C(f32 farg0, f32 farg1, f32 farg2, f32 farg3, f32 farg4, f32 farg5) {
    f32 t = ((farg0 - farg2) * (farg5 - farg3)) + ((farg1 - farg3) * (farg2 - farg4));
    return t >= lbl_801A4F98[0];
}

s32 fn_800CC5D8(void) {
    s32 r = 0;
    if (*(s32 *)((s8 *)lbl_80409410 + 0x19C) == 7 && *(s32 *)((s8 *)lbl_80409410 + 0x198) == 0x37) {
        r = (lbl_803E0199[0] ^ 0x15) == 0;
    }
    return r;
}

typedef struct { s32 v[3]; } DbgVec;
extern void *lbl_80409004;
void DebugMenu_ToggleOption1(s32 arg0, s32 arg1) {
    if (lbl_80409004 != NULL) {
        lbl_801D5744[0] = arg0;
        if (arg0 != 0) {
            lbl_801D5744[9] = arg1;
            *(DbgVec *)&lbl_801D5744[1] = *(DbgVec *)((s8 *)lbl_80409004 + 0x90);
            return;
        }
        lbl_801D5744[9] = arg0;
    }
}

s32 fn_800CFE9C(void *arg0, void *arg1, f32 farg0, f32 farg1) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f12;
    f32 temp_f13;
    f32 temp_f2;
    f32 temp_f2_2;

    temp_f2_2 = farg0 + farg1;
    temp_f2 = temp_f2_2 * temp_f2_2;
    temp_f12 = (*(f32 *)((s8 *)(arg1) + (4))) - (*(f32 *)((s8 *)(arg0) + (4)));
    temp_f13 = (*(f32 *)((s8 *)(arg1) + (0))) - (*(f32 *)((s8 *)(arg0) + (0)));
    temp_f0_2 = (*(f32 *)((s8 *)(arg1) + (8))) - (*(f32 *)((s8 *)(arg0) + (8)));
    temp_f0 = (temp_f0_2 * temp_f0_2) + ((temp_f13 * temp_f13) + (temp_f12 * temp_f12));
    return temp_f0 <= temp_f2;
}

typedef struct { u8 pad[0x14]; s16 id; u8 rest[0xE0 - 0x16]; } MenuFrame;
extern s32 gRegisteredMenuDefinitions[];
s32 fn_800B2ED4(void) {
    s16 id = ((MenuFrame *)gMenuStack)[gMenuDepth].id;
    if (id == -1) {
        return -1;
    }
    return *(s32 *)((s8 *)gRegisteredMenuDefinitions + id * 0x1C);
}

extern f32 lbl_8019FEA8[];
extern f32 lbl_80408E9C;
void fn_800ABD50(void *arg0) {
    f32 zero = lbl_8019FEA8[0];
    s8 *p = (s8 *)*(s32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x61C) + 0x40;
    if (*(f32 *)(p + 0xC) > zero) {
        *(f32 *)(p + 0xC) -= lbl_80408E9C;
    }
    if (*(f32 *)(p + 4) > zero) {
        *(f32 *)(p + 4) -= lbl_80408E9C;
    }
    if (*(u8 *)(p + 0x10) != 0) {
        *(u8 *)(p + 0x10) -= 1;
    }
}

extern s32 lbl_801D5AA0[];

void fn_800BA628(s32 arg0) {
    s32 i_var_r31;
    void *var_r31;
    void *var_r30;
    s32 *var_r3;
    s32 temp_r3;

    i_var_r31 = 0;
    do {
        var_r31 = (void *) ((s8 *) (lbl_801D5AA0) + i_var_r31 * 8);
        var_r30 = (void *) ((s8 *) (arg0 + 0x76C) + i_var_r31 * 16);
        var_r3 = (*(s32 **)((s8 *)(var_r31) + (0)));
        if (var_r3 == NULL) {
            var_r3 = arg0 + 0x1C0;
        } else if (var_r3 == (s32 *)-1) {
            var_r3 = arg0 + 0x1C8;
        }
        temp_r3 = *var_r3;
        if (temp_r3 != 0) {
            NuSpecialFind(temp_r3, var_r30, (*(s32 **)((s8 *)(var_r31) + (4))), 1);
        }
        i_var_r31 += 1;
    } while (i_var_r31 <= 286);
}
