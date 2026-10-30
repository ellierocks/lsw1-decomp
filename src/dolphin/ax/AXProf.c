#include <dolphin.h>
#include <dolphin/ax.h>

extern AXPROFILE* lbl_8040AC00;
extern u32 lbl_8040AC04;
extern u32 lbl_8040AC08;
extern u32 lbl_8040AC0C;

AXPROFILE* __AXGetCurrentProfile(void) {
    AXPROFILE* profile;

    if (lbl_8040AC0C != 0) {
        profile = &lbl_8040AC00[lbl_8040AC08];
        lbl_8040AC08 += 1;
        lbl_8040AC08 %= lbl_8040AC04;
        return profile;
    }

    return 0;
}
