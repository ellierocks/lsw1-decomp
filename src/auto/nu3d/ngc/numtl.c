#include "types.h"

extern s32 lbl_80407D20;

extern void fn_80024A34(s32 a, s32 b, s32 c);

extern void fn_800339C0(void);

extern s32 lbl_80407D5C;
s32 fn_80024DC0(void);

extern s32 lbl_80407D18;

void fn_80024D54(void)
{
}

void fn_80024D34(u8* p)
{
    s32 z = 0;
    *(s32*)(p + 0xb4) = z;
    p[0xa7] = z;
}

void fn_80024D44(u8* p)
{
    *(s32*)(p + 0xb4) = *(s32*)(*(u8**)(p + 0xb4) + 0xb4);
}

void NuHazeClose(void)
{
    if (lbl_80407D18 != 0) {
        lbl_80407D18 = 0;
    }
}

s32 fn_80024D0C(s32 cb)
{
    s32 old = lbl_80407D5C;
    if (cb != 0) {
        lbl_80407D5C = cb;
    } else {
        lbl_80407D5C = (s32)fn_80024DC0;
    }
    return old;
}

void fn_80024BB8(void)
{
    fn_800339C0();
}

void fn_80024C40(s32 a, s32 b)
{
    fn_80024A34(a, b, 1);
}

void fn_80024BF0(s32 a)
{
    fn_80024A34(a, lbl_80407D20, 0);
}

void fn_80024C18(s32 a)
{
    fn_80024A34(a, lbl_80407D20, 1);
}
