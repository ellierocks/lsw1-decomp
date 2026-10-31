#include "types.h"

extern s32* lbl_80408120;

void NuScratchRelease(void)
{
    lbl_80408120 = (s32*)lbl_80408120[-1];
}

void fn_80039BE0(void)
{
}

void fn_80039CA4(void)
{
}

void fn_80039CA8(void)
{
}

void fn_80039CAC(void)
{
}

s32 fn_80039D28(void)
{
    return 0;
}
