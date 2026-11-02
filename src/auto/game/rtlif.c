#include "types.h"


s32 fn_8013F540(void)
{
    return 16384;
}

s32 fn_80063E40(s32, void *, s32, s32, s32);
s32 fn_800957E0(f32, f32);
extern f32 lbl_80408E9C;
extern void *lbl_80409410;
extern f32 lbl_80409A30;

void fn_8013F4CC(void *arg0) {
    s32 temp_r31;
    s32 temp_r3;

    if ((*(s32 *)((s8 *)(arg0) + (0x128))) & 0x400000) {
        temp_r31 = (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1B8)))) + (8)))) + (0x474)));
        if (temp_r31 != -1) {
            temp_r3 = fn_800957E0(lbl_80409A30, lbl_80408E9C);
            if (temp_r3 > 0) {
                fn_80063E40(temp_r31, arg0 + 0x30, temp_r3, 0, 0);
            }
        }
    }
}
