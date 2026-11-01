#include "types.h"

extern u8 lbl_802B0C20[];
extern s32 OSSendMessage(void* q, s32 m, s32 f);

extern void fn_800455A4(void);
extern void AXRegisterCallback(void (*cb)(void));

struct ngcstream_ent_s {
    s32 pad0;
    s32 state;
    u8 pad8[0x38];
};

extern struct ngcstream_ent_s lbl_802AFC20[];

extern s32 lbl_804082D8;

extern s32 lbl_804082E0;

extern s32 lbl_804082B4;

void fn_800472FC(void)
{
}

void fn_8004605C(void)
{
    lbl_804082B4 = 0;
}

void fn_800472F0(s32 a)
{
    lbl_804082E0 = a << 1;
}

void fn_80047300(void)
{
    lbl_804082D8 = 0;
}

void fn_80046090(void)
{
    s32 i;
    for (i = 0; i < 64; i++) {
        if (lbl_802AFC20[i].state == 3) {
            lbl_802AFC20[i].state = 4;
        }
    }
}

void fn_80046068(void)
{
    AXRegisterCallback(fn_800455A4);
}

void fn_80047260(void)
{
    OSSendMessage(lbl_802B0C20, 6, 1);
}

void fn_80047290(void)
{
    OSSendMessage(lbl_802B0C20, 5, 1);
}

void fn_800472C0(void)
{
    OSSendMessage(lbl_802B0C20, 7, 1);
}
