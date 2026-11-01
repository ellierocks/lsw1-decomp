#include "types.h"

extern void* memset();

extern u32 OSGetTick(void);

struct ngc_mtl_state_s {
    u8 pad0[0x1e];
    u16 cnt1e;
    s32 n20;
    u8 pad24[4];
    u16 cnt28;
    u8 pad2a[0xaa];
    s32 fd4;
    f32 fd8;
    s32 fdc;
    u8 pade0[0x68];
    s32 f148;
    s32 f14c;
    f32 f150;
    f32 f154;
    s32 f158;
    s32 f15c;
};

extern struct ngc_mtl_state_s lbl_803127F0;

extern s32 lbl_8021FBA8[];

extern s32 lbl_8040A378;
extern s32 lbl_8040A388;
extern s32 lbl_8040A37C;
extern s32 lbl_8040A380;
extern s32 lbl_8040A384;

extern f32 lbl_80407EF0;

extern f32 lbl_801921AC[];
extern f32 lbl_80407EF4;

extern s32 lbl_8040A398;
extern s32 lbl_8040A390;

extern s32* lbl_8040A38C;

extern f32 lbl_80192054[];

extern s32 lbl_8040A3B0;

extern s32 lbl_8040ADB4;

void fn_8002DBA4(void)
{
}

void fn_8002DBE0(void)
{
}

void fn_8002DE14(void)
{
}

void fn_8002E538(void)
{
}

void fn_80033E94(s32 a)
{
    lbl_8040ADB4 = a;
}

void fn_80033E9C(s32 a)
{
    lbl_8040A3B0 = a;
}

f32 fn_8002DE38(void)
{
    return lbl_80192054[0];
}

void fn_80030A64(s32* a)
{
    lbl_8040A38C = a + 1;
}

void fn_80030A54(void)
{
    s32 z = 0;
    lbl_8040A390 = z;
    lbl_8040A398 = z;
}

void fn_80030A0C(f32 a)
{
    lbl_80407EF4 = a * lbl_801921AC[0];
}

void fn_80030A20(void)
{
    lbl_80407EF0 = lbl_80407EF0 + lbl_80407EF4;
}

void fn_80030A34(void)
{
    s32 z = 0;
    lbl_8040A378 = -1;
    lbl_8040A37C = z;
    lbl_8040A380 = z;
    lbl_8040A384 = z;
    lbl_8040A388 = z;
}

void fn_80030A70(void)
{
    s32 i;
    for (i = 7; i >= 0; i--) {
        lbl_8021FBA8[i] = 0;
    }
}

void fn_8002DCF8(s32 a)
{
    lbl_803127F0.f14c = a;
    lbl_803127F0.f148 = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt28++;
}

void fn_8002DBA8(s32 a, s32 b, s32 c, s32 e, f32 d)
{
    lbl_803127F0.fdc = e;
    lbl_803127F0.fd4 = a;
    lbl_803127F0.fd8 = d;
    lbl_803127F0.f15c = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt1e++;
}

void fn_8002DD28(s32 a, f32 b, f32 c)
{
    lbl_803127F0.f150 = b;
    lbl_803127F0.f154 = c;
    lbl_803127F0.f158 = a;
    lbl_803127F0.f148 = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt28++;
}

u32 fn_8002DE18(void)
{
    return OSGetTick();
}

void fn_8002DB70(void)
{
    memset(&lbl_803127F0, 0, 0x160);
}
