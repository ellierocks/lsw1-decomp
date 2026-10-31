#include "types.h"

extern void fn_8000A714(s32 id, s32 a, s32* out);
extern s32 NuFileLoadBuffer(s32 handle, s32 pos, s32 size);

s32 fn_8000584C(s32 id)
{
    s32 out = 0;
    if (id <= 0x3ff) {
        fn_8000A714(id - 1, 0, &out);
        return out;
    }
    return 0;
}

s32 fn_80005894(s32 handle, s32* pos, s32* end)
{
    s32 n = NuFileLoadBuffer(handle, *pos, *end - *pos);
    *pos = *pos + n;
    return n;
}
