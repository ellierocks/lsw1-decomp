#include "types.h"
#include "nu3dx/nuanim.h"

void fn_80023C70(void* dst) {
    u32* s = (u32*)0x80313268;
    u32* d = (u32*)dst;
    s32 n = 0x60;
    u32 tmp;
    volatile u32 pad;
    
    do {
        tmp = s[0];
        n -= 0x18;
        d[0] = tmp;
        d[1] = s[1];
        d[2] = s[2];
        d[3] = s[3];
        d[4] = s[4];
        d[5] = s[5];
        s += 6;
        d += 6;
    } while (n != 0);
    
    d[0] = s[0];
    d[1] = s[1];
}
