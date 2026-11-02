#include "types.h"

struct fn_8012CD28_s {
    s32 x;
    s32 y;
    s32 z;
};
extern struct fn_8012CD28_s lbl_804017EC[];
extern s32 lbl_8040B010;

void fn_8012CD28(s32 a, struct fn_8012CD28_s* b)
{
    lbl_8040B010 = a;
    lbl_804017EC[0] = *b;
}

extern void *lbl_80408DF0;

s32 fn_8012CEF4(s32 arg0) {
    if ((arg0 != 0xFF) && (arg0 < (s32) (*(s32 *)((s8 *)(lbl_80408DF0) + (0))))) {
        return (*(s32 *)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0xC)));
    }
    return 0;
}
