#include "types.h"

extern f32 lbl_801AEB88[];
extern f32 lbl_804099B0;
extern f32 lbl_804099E0;

f32 Condition_GlynTest(void)
{
    return lbl_804099E0;
}

s32 Condition_GlynTestInit(void)
{
    return 0;
}

s32 Condition_NumBaddiesInit(void)
{
    return 0;
}

f32 Condition_CheatProgress(void)
{
    return lbl_801AEB88[0];
}

f32 Condition_DropBackInTimer(void)
{
    return lbl_804099B0;
}
