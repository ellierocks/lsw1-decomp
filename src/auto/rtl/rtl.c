#include "types.h"

extern s32 lbl_804087E4;
extern s32 lbl_80408738;
extern s32 lbl_804087C8;
extern s32 lbl_804087D4;

s32 fn_8006EB90(void)
{
    return lbl_80408738;
}

void fn_8006F7E0(s32 a)
{
    lbl_804087C8 = a;
}

void fn_8006F8A8(s32 a)
{
    lbl_804087D4 = a;
}

s32 fn_80071D90(s32 a)
{
    s32 t0 = lbl_804087E4;
    lbl_804087E4 = a;
    return t0;
}

extern void* lbl_804087D8;
extern s32 lbl_804087DC;
extern u8* fn_80040DE8(void*, s32);

struct RtlVec { u32 x, y, z; };

s32 fn_80071AEC(s32 index, struct RtlVec* value)
{
    struct RtlVec* p;
    if (lbl_804087D8 && index >= 0 && index < lbl_804087DC &&
        (p = (struct RtlVec*)fn_80040DE8(lbl_804087D8, index)) && value) {
        *p = *value;
        return 1;
    }
    return 0;
}

s32 fn_80071B78(s32 index, f32 a, f32 b)
{
    u8* p;
    if (lbl_804087D8 && index >= 0 && index < lbl_804087DC &&
        (p = fn_80040DE8(lbl_804087D8, index))) {
        *(f32*)(p + 0x3c) = a;
        if (a > b) *(f32*)(p + 0x40) = a;
        else *(f32*)(p + 0x40) = b;
        return 1;
    }
    return 0;
}

s32 fn_80071C04(s32 index, struct RtlVec* a, struct RtlVec* b)
{
    u8* p;
    if (lbl_804087D8 && index >= 0 && index < lbl_804087DC &&
        (p = fn_80040DE8(lbl_804087D8, index)) && (a || b)) {
        if (a) *(struct RtlVec*)(p + 0x24) = *a;
        if (b) *(struct RtlVec*)(p + 0x30) = *b;
        return 1;
    }
    return 0;
}

extern u8 lbl_803D4680[];
extern s32 lbl_8040AEF8;
extern s32 lbl_804087CC;
extern void* memcpy(void*, const void*, ...);
extern s32 NuFileRead(s32, void*, s32);

s16 fn_8006F55C(void)
{
    s16 value;
    u8* dest = (u8*)&value;
    s32 remaining = 2;
    u8* bytes = dest;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    if (lbl_804087D4) {
        u8 t = *(u8*)&value;
        *(u8*)&value = bytes[1];
        bytes[1] = t;
    }
    return value;
}

u16 fn_8006F63C(void)
{
    u16 value;
    u8* dest = (u8*)&value;
    s32 remaining = 2;
    u8* bytes = dest;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    if (lbl_804087D4) {
        u8 t = *(u8*)&value;
        *(u8*)&value = bytes[1];
        bytes[1] = t;
    }
    return value;
}

s8 fn_8006F71C(void)
{
    s8 value;
    u8* dest = (u8*)&value;
    s32 remaining = 1;
    do {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    } while (remaining > 0);
    return value;
}

void fn_8006F7E8(u8* dest, s32 remaining)
{
    while (remaining > 0) {
        s32 n = 0x1000 - lbl_8040AEF8;
        if (n > remaining) n = remaining;
        memcpy(dest, lbl_803D4680 + lbl_8040AEF8, n);
        dest += n;
        remaining -= n;
        lbl_8040AEF8 += n;
        if (lbl_8040AEF8 == 0x1000 && lbl_804087CC != -1) {
            switch (lbl_804087C8) {
            case 0: break;
            case 1: NuFileRead(lbl_804087CC, lbl_803D4680, 0x1000);
            case 2: lbl_8040AEF8 = 0; break;
            }
        }
    }
}
