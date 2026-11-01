#include "types.h"

extern void fn_800262BC(s32* a, s32 b, s32 c, s32 d);

extern s32 fn_80037228(void);

extern s32 fn_80033F30(void);

extern s32 fn_80037218(void);

extern void AXFXSetHooks(s32 a, s32 b);

struct nurndr_ent_s {
    u8 pad[0x32];
    u8 flag;
    u8 pad33;
};

extern s32 lbl_8040AD80;
extern struct nurndr_ent_s lbl_8033F6E4[];

extern s32 lbl_80407DD8;

extern s32 lbl_8040A33C;
extern s32 lbl_8040A340;

extern s32 lbl_80407D74;

extern s32 lbl_80407D7C;

void fn_80024F0C(void)
{
}

void fn_80024F40(void)
{
}

void fn_80027690(void)
{
}

void fn_8002782C(void)
{
}

void fn_80027830(void)
{
}

void fn_80027BE0(void)
{
}

void fn_80027BF4(void)
{
}

void fn_80027C54(void)
{
}

s32 fn_80024F10(void)
{
    return 0;
}

s32 fn_80027688(void)
{
    return 0;
}

s32 fn_80027824(void)
{
    return 1;
}

s32 fn_80027BE4(void)
{
    return 0;
}

s32 fn_80027BEC(void)
{
    return 1;
}

void fn_80027BF8(s32 a)
{
    lbl_80407D7C = a;
}

void fn_80027D2C(s32 a)
{
    lbl_80407D74 = a;
}

void AXFXSetHooks(s32 a, s32 b)
{
    lbl_8040A33C = a;
    lbl_8040A340 = b;
}

void fn_80027C00(void)
{
    lbl_80407DD8 = 1;
}

void fn_80027D00(void)
{
    s32 i;
    lbl_8040AD80 = 0;
    for (i = 63; i >= 0; i--) {
        lbl_8033F6E4[i].flag = 0;
    }
}

void fn_80027668(s32 a, s32 b)
{
    AXFXSetHooks(a, b);
}

s32 fn_80027D34(void)
{
    return fn_80037218();
}

s32 fn_80024F44(void)
{
    return fn_80033F30() + 1;
}

s32 fn_80027348(void)
{
    if (fn_80037228() == 0x32) { return 0; }
    return 1;
}

void NuRndrTriStrip3dClip(s32 a, s32 b, s32 c, s32 d)
{
    fn_800262BC(&a, d, c, b);
}
