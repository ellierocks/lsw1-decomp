#include "types.h"

extern s32* lbl_80408120;

void NuScratchRelease(void)
{
    lbl_80408120 = (s32*)lbl_80408120[-1];
}
