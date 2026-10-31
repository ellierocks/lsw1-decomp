#include "types.h"

extern s32 lbl_80409700;
extern s32 lbl_8040B204;
extern s32 lbl_8040B20C;
extern s32 lbl_804096FC;
extern f32 lbl_801A9A44[];
extern f32 lbl_801A9A48[];
extern f32 lbl_80409798;
extern f32 lbl_803FFE2C[];
extern s32 lbl_802E231C[];
extern s32 lbl_8040973C;
extern f32 lbl_8040A720;
extern f32 lbl_8040A724;
extern s32 lbl_8040B100;

void fn_800E32F8(s32 a)
{
    lbl_8040B100 = a;
}

void fn_800E631C(void)
{

}

void fn_801039D0(u8* a)
{
    *(s16*)(a + 78) = 1;
}

void fn_80103F64(void)
{
    lbl_8040973C = 0;
}

void fn_80103F70(f32 x, s32 a)
{
    lbl_8040973C = a;
    lbl_8040A720 = x;
    lbl_8040A724 = (x * x);
}

void fn_8010438C(void)
{
    lbl_802E231C[0] = 0;
}

void fn_800E6320(void)
{
    lbl_8040B204 = 0;
    lbl_8040B20C = 0;
    lbl_804096FC = 0;
    lbl_80409700 = 0;
}

void fn_80103EA8(u8* a)
{
    f32 k = lbl_801A9A44[0];
    f32 m = lbl_801A9A48[0];
    s32 z = 0;
    *(f32*)(a + 1856) = k;
    *(f32*)(a + 1860) = k;
    *(f32*)(a + 1928) = k;
    *(s32*)(a + 1696) = z;
    *(s32*)(a + 1700) = z;
    *(f32*)(a + 1864) = m;
}

void fn_8010403C(void)
{
    f32 k = lbl_80409798;
    lbl_803FFE2C[13] = k;
    lbl_803FFE2C[27] = k;
}

void fn_80108700(u8* a)
{
    *(s32*)(a + 16) = 0;
    *(s32*)(a + 20) = 0;
}
