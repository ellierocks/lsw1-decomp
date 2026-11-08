#include "types.h"

struct fn_80140918_s {
    s32 x;
    s32 y;
    s32 z;
};
extern struct fn_80140918_s lbl_801E1774[];

void fn_80140918(struct fn_80140918_s* a)
{
    lbl_801E1774[0] = *a;
}

s32 fn_800D875C(s32, s32);
s32 fn_800D99AC();
extern f32 lbl_801AF204[];
extern f32 lbl_801AF208[];
extern f32 lbl_80408E9C;
extern f32 lbl_80409578;
extern s32 lbl_80409AC0;
extern f32 lbl_8040A794;

void fn_80141DEC(void) {
    if ((s32) lbl_80409AC0 != -1) {
        if (lbl_8040A794 > lbl_801AF204[0]) {
            lbl_8040A794 -= lbl_80408E9C;
            return;
        }
        if ((lbl_80409578 == lbl_801AF208[0]) && (fn_800D99AC() == -1)) {
            fn_800D875C(lbl_80409AC0, 0);
        }
    }
}

s32 fn_80140E78(void *);

void fn_801410F4(void *arg0) {
    s32 var_r30;
    void *var_r31;

    var_r30 = 0;
    if (arg0 != NULL) {
        var_r31 = (*(void **)((s8 *)(arg0) + (0xC)));
        if (var_r30 < (s32) (*(u16 *)((s8 *)(arg0) + (2)))) {
            do {
                if (var_r31 != NULL) {
                    fn_80140E78(var_r31);
                    var_r31 = (*(void **)((s8 *)(var_r31) + (0x78)));
                }
                var_r30 += 1;
            } while (var_r30 < (s32) (*(u16 *)((s8 *)(arg0) + (2))));
        }
    }
}
