#include "types.h"

extern void fn_8001FD98(s32 a, s32 b, s32 c, s32 d, s32 e);

extern f32 lbl_80407CBC;

extern f32 lbl_80407CC4;

void fn_800203A4(f32 a)
{
    lbl_80407CC4 = a;
}

f32 fn_800203AC(u8* p)
{
    return **(f32**)(p + 0x30) * *(f32*)(p + 0x14) * lbl_80407CBC;
}

void fn_8002043C(s32 a, s32 b, s32 c, s32 d)
{
    fn_8001FD98(0, a, b, c, d);
}
