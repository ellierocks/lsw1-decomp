#include "types.h"

extern s16 lbl_801BB44C[];
extern s16 lbl_801BB452[];
extern s16 lbl_801BB456[];
extern f32 lbl_801BB460[];
extern s32 lbl_80408488;
extern s32 lbl_8040848C;
extern s32 lbl_80408490;
extern s32 lbl_804085C0;
extern f32 lbl_8040870C;

void fn_80066290(void)
{

}

void fn_80066474(s32 a)
{
    lbl_80408488 = a;
}

void fn_8006647C(s32 a)
{
    lbl_8040848C = a;
}

void fn_80066484(s32 a)
{
    lbl_80408490 = a;
}

s32 fn_8006648C(void)
{
    return -1;
}

void fn_80066494(void)
{

}

void fn_800698B0(s32 a)
{
    lbl_804085C0 = a;
}

void fn_8006DEB0(f32 x)
{
    lbl_8040870C = x;
}

void fn_8006DF18(void)
{

}

s32 fn_8006DF50(void)
{
    return lbl_801BB452[0];
}

s32 fn_8006DF5C(void)
{
    return lbl_801BB456[0];
}

s32 fn_8006E2C0(void)
{
    return lbl_801BB44C[0];
}

void fn_8006E370(f32 x)
{
    lbl_801BB460[0] = x;
}

extern s32 fn_800076E4(const char*, const char*, s32);

s32 fn_80069900(const char* name)
{
    s32 i;
    if (lbl_804085C0) {
        for (i = 0; i < *(s32*)((u8*)lbl_804085C0 + 0x20); i++) {
            u8* entries = *(u8**)((u8*)lbl_804085C0 + 0x24);
            if (fn_800076E4(*(const char**)(entries + i * 0x50 + 0x44), name, 0x13) == 0)
                return i;
        }
    }
    return -1;
}

extern s32 lbl_804086EC, lbl_804086E8, lbl_80408298;
extern void fn_80044BF4(s32);

void fn_8006E00C(void)
{
    s32 minimum;
    if (!lbl_804086EC && !lbl_804086E8 && lbl_80408298 != -1 &&
        (u32)((u16)lbl_801BB44C[0] - 11) > 2) {
        minimum = 5;
        fn_80044BF4(lbl_801BB44C[2]);
        if (lbl_801BB44C[0] > 10) return;
        if (lbl_801BB44C[0] >= minimum)
            fn_80044BF4(1 - lbl_801BB44C[2]);
    }
}
