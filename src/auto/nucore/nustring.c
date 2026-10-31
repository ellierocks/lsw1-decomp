#include "types.h"

s32 NuStrLen(char* s)
{
    s32 n = 0;
    while (*s != 0) {
        n++;
        s++;
    }
    return n;
}

s32 NuStrCpy(char* dst, char* src)
{
    s32 n = 0;
    if (src == 0) {
        *dst = 0;
    } else {
        do {
            *dst++ = *src;
            n++;
        } while (*src++ != 0);
    }
    return n;
}

void NuStrCat(char* dst, char* src)
{
    while (*dst != 0) {
        dst++;
    }
    if (src == 0) {
        return;
    }
    do {
        *dst++ = *src;
    } while (*src++ != 0);
}
