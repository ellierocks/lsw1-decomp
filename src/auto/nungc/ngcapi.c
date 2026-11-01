#include "types.h"

extern void* memset();

extern void* memset(void* p, s32 c, u32 n);

extern s32 lbl_804080F8;

extern u8 lbl_80340588[];

void fn_80037C1C(void)
{
}

void fn_80037DA8(u8* p)
{
    *(void**)(p + 0x1cc) = lbl_80340588;
}

void fn_800385A4(void)
{
    if (lbl_804080F8 == 0) {
        lbl_804080F8 = 4;
    }
}

s32 fn_800385BC(void)
{
    if (lbl_804080F8 == 0) {
        return 0;
    }
    return 1;
}

void fn_80037DB8(void)
{
    memset(lbl_80340588, 0xff, 0x271);
}

void fn_80037DE8(void)
{
    memset(lbl_80340588, 0, 0x271);
}
