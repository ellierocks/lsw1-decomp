#include "types.h"

extern void NuHGobjEvalAnimBlend2Root(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);

extern void NuHGobjEvalAnim2Root(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g);

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

void NuHGobjEvalAnim2(s32 a, s32 b, s32 c, s32 d, s32 e)
{
    NuHGobjEvalAnim2Root(a, b, c, d, e, 0, 0);
}

void NuHGobjEvalAnimBlend2(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    NuHGobjEvalAnimBlend2Root(a, b, c, d, e, f, 0, 0);
}
