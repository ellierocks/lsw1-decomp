#include "types.h"

u8* NuHGobjGetPOI(u8* p, u32 i)
{
    u8 j;
    if (i < p[0x80]) {
        j = (*(u8**)(p + 0x2c))[i];
        if (j != 0xff) {
            return *(u8**)(p + 0x28) + j * 0x50;
        }
    }
    return 0;
}
