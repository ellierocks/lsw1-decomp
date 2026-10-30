#include "nucore/nufile.h"

extern struct nufileslot_s lbl_80200080[];
extern struct numemfile_s lbl_803120C0[];
s32 fn_8003A23C(s32 handle);
s32 fn_80005FF4(s32 handle);

s32 NuFileOpenSize(s32 handle)
{
    if (handle >= 0x1000) {
        return fn_8003A23C(handle - 0x1000);
    } else if (handle >= 0x800) {
        return fn_80005FF4(handle);
    } else if (handle >= 0x400) {
        return lbl_803120C0[handle - 0x400].end - lbl_803120C0[handle - 0x400].start;
    } else {
        handle--;
        return lbl_80200080[handle].size;
    }
}
