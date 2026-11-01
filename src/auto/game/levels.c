#include "types.h"

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
