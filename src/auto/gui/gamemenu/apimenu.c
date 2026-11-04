#include "types.h"

extern s32 lbl_8040889C;

void fn_8007483C(void)
{

}

void fn_80074B80(s32 a)
{
    lbl_8040889C = a;
}

extern s32 lbl_803D9020[];
extern s32 lbl_804088B0;

void fn_80075CD8(s32 arg0) {
    *(s32 *)(((s8 *)lbl_803D9020) + (arg0 * 4)) = 0;
    if ((s32) lbl_804088B0 == arg0) {
        lbl_804088B0 = 0;
    }
}
