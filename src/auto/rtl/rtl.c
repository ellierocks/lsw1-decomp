#include "types.h"

extern s32 lbl_804087E4;
extern s32 lbl_80408738;
extern s32 lbl_804087C8;
extern s32 lbl_804087D4;

s32 fn_8006EB90(void)
{
    return lbl_80408738;
}

void fn_8006F7E0(s32 a)
{
    lbl_804087C8 = a;
}

void fn_8006F8A8(s32 a)
{
    lbl_804087D4 = a;
}

s32 fn_80071D90(s32 a)
{
    s32 t0 = lbl_804087E4;
    lbl_804087E4 = a;
    return t0;
}
