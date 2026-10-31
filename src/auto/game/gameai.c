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
