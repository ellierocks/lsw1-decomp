#include "nucore/nufile.h"

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

s8 NuFileReadChar(s32 handle)
{
    s8 data = 0;
    NuFileRead(handle, &data, 1);
    return data;
}

u16 NuFileReadWChar(s32 handle)
{
    u16 data = 0;
    NuFileRead(handle, &data, 2);
    return data;
}
