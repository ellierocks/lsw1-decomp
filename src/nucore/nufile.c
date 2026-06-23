#include "types.h"

s32 NuFileRead(s32 handle, void* data, s32 size);

s32 NuFileReadInt(s32 handle)
{
    s32 data;
    NuFileRead(handle, &data, 4);
    return data;
}

s16 NuFileReadShort(s32 handle)
{
    s16 data;
    NuFileRead(handle, &data, 2);
    return data;
}
