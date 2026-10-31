#include "types.h"

extern s32 lbl_80407C28;
extern s32 lbl_80407C30;

void fn_8001AF60(s32 v)
{
    lbl_80407C30 = v;
    lbl_80407C28 = 1;
}

void fn_8001AF70(void)
{
    lbl_80407C28 = 0;
}

s32 fn_8001AF7C(void)
{
    return lbl_80407C28;
}

s32 fn_8001AF84(void)
{
    return lbl_80407C30;
}
