#include "types.h"

extern void* memset();
extern void (*fn_80009948(char* file, s32 line))(char* message, ...);

typedef struct nupartgroup_s {
    u8 unknown0[0x10];
    s16 parts[8];
    s8 flags[8];
    u8 unknown28[0x88];
} nupartgroup_s;

typedef struct nupartentry_s {
    u8 unknown0[12];
    s32 index;
    u8 unknown10[0x5C];
} nupartentry_s;

extern nupartgroup_s lbl_803D5680[64];
extern nupartentry_s lbl_803D8280[32];
extern s32 lbl_803D9000[8];
extern s32 lbl_803D9020[8];
extern s32 lbl_80408888;
extern s32 lbl_8040888C;
extern s32 lbl_804088A8;
extern s32 lbl_804088AC;
extern void* lbl_8040AEFC;
extern s32 lbl_8040AF00;

void fn_80071E9C(s32 count, u32* buffer, u32* end)
{
    void* allocation;
    s32 size;
    s32 i;

    size = count * 0x1F4;
    allocation = (void*)((*buffer + 15) & ~15);
    lbl_80408888 = count;
    *buffer = (u32)allocation;
    lbl_8040AEFC = allocation;
    *buffer = (u32)allocation + size;
    if (allocation != NULL) {
        memset(allocation, 0, size);
    }
    {
        u32 cursor = *buffer;
        u32 limit = *end;
        lbl_8040888C = 0;
        if (cursor > limit) {
            fn_80009948("../gameapi.legogc/effects/objects/ngc_obj/../part.c", 0x151)(
                "InitParts - buffer is too small by %d bytes", *buffer - *end);
        }
    }
    memset(lbl_803D5680, 0, sizeof(lbl_803D5680));
    {
        s32 offset = 0;
        s32 count = 64;
        do {
            s8* flags = (s8*)lbl_803D5680 + offset + 0x20;
            s16* parts = (s16*)((u8*)lbl_803D5680 + offset + 0x10);
            s32 left = 8;
            do {
                *parts++ = -1;
                *flags++ = -1;
            } while (--left != 0);
            offset += 0xB0;
        } while (--count > 0);
    }
    lbl_804088A8 = 0;
    memset(lbl_803D8280, 0, sizeof(lbl_803D8280));
    for (i = 31; i >= 0; i--) {
        lbl_803D8280[i].index = -1;
    }
    lbl_804088AC = 0;
    for (i = 0; i < 8; i++) {
        lbl_803D9000[i] = 0;
        lbl_803D9020[i] = 0;
    }
    lbl_8040AF00 = 0;
}
