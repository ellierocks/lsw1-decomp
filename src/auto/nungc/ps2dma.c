#include "types.h"

extern s32* lbl_80408120;

void NuScratchRelease(void)
{
    lbl_80408120 = (s32*)lbl_80408120[-1];
}

void fn_80039BE0(void)
{
}

void fn_80039CA4(void)
{
}

void fn_80039CA8(void)
{
}

void fn_80039CAC(void)
{
}

s32 fn_80039D28(void)
{
    return 0;
}

void* NuScratchAlloc32(s32 size)
{
    u32 previous;
    u32 rounded;
    u32 allocation;

    previous = (u32)lbl_80408120;
    rounded = (size + 3) & ~3;
    allocation = (previous + 3) & ~3;
    lbl_80408120 = (s32*)(allocation + rounded);
    *(u32*)(allocation + rounded) = previous;
    lbl_80408120++;
    return (void*)allocation;
}

typedef struct nudma_s {
    u32 words[4];
} nudma_s;

extern nudma_s* lbl_8040AE08[1];

typedef struct nudmacursor_s {
    u32 address;
} nudmacursor_s;
extern nudmacursor_s fn_80039D74();
extern nudmacursor_s fn_80039DB4();

void fn_8003A0E0(s32 list, s32 count)
{
    s32 index;
    nudma_s* packet;
    s32 base;

    index = count - 2;
    packet = *(nudma_s**)(list + count * 4 - 4);
    packet->words[0] = 0x60;
    packet->words[1] = 0;
    if (index >= 0) {
        base = list;
        list = index * 4 + base;
        do {
            packet = *(nudma_s**)list;
            packet->words[0] = 0x20;
            packet->words[1] = *(u32*)(list + 4);
            list -= 4;
        } while (list >= base);
    }
}

void fn_80034A9C(s32, f32);

void fn_80039CB0(s32* handle, s32 enabled, s32 amount)
{
    f32 value;
    value = 0.0f;
    if (amount != 0) {
        value = (f32)amount / 255.0f;
    } else if (enabled != 0) {
        value = 255.0f;
    }
    fn_80034A9C(*handle, value);
}

extern void* memset();
extern s32 lbl_80408110;
extern s32 lbl_8040AD4C;
extern s32 lbl_8040AD50;
extern s32 lbl_8040AD60;
extern s32 lbl_8040AD64;

typedef struct nudmaview_s {
    s32 x;
    s32 y;
    s32 width;
    s32 height;
    f32 depth;
    f32 zero;
    u8 unk18[0x28];
} nudmaview_s;

void fn_80039BE4(nudmaview_s* view)
{
    memset(view, 0, 0x40);
    view->x = (lbl_8040AD60 - lbl_8040AD4C / 2) * 16;
    view->y = (lbl_8040AD64 - lbl_8040AD50 / 2) * 16;
    view->width = lbl_8040AD4C * 16;
    view->height = lbl_8040AD50 * 16;
    view->zero = 0.0f;
    view->depth = (f32)lbl_80408110;
}

extern void (*fn_80009948(char*, s32))();
extern void (*fn_80009970(char*, s32))();
extern char lbl_80192E0C[];
extern char lbl_80192E38[];
extern char lbl_80192E68[];

u32 fn_8003A134(u32 address, u32* sizeOut)
{
    nudmacursor_s cursor;
    nudmacursor_s out1;
    nudmacursor_s out2;
    nudmacursor_s out3;
    nudmacursor_s out4;
    u32 aligned;
    u32 next;
    u32 end;
    s32 count;

    aligned = (address + 15) & ~15;
    cursor.address = aligned;
    out1 = fn_80039D74(cursor, 0, 0);
    next = aligned + 0x30;
    cursor.address = next;
    out2 = fn_80039DB4(cursor);
    cursor.address = next;
    out3 = fn_80039D74(cursor, 0, 0);
    *(u16*)(aligned + 0x40) = 0x38;
    *(f32*)(aligned + 0x14) = 0.0f;
    *(u16*)(aligned + 0x44) = 0xC8;
    *(u16*)(aligned + 0x42) = 0x48;
    *(u16*)(aligned + 0x46) = 0x48;
    end = aligned + 0x48;
    count = 0x40;
    do {
        end += 0x28;
    } while (--count != 0);
    cursor.address = end;
    out4 = fn_80039DB4(cursor);
    next = out4.address;
    *sizeOut = next - aligned;
    return aligned;
}

nudmacursor_s fn_80039DB4(nudmacursor_s in)
{
    s32 size;
    u32 end;
    nudmacursor_s out;

    end = in.address;
    if (lbl_8040AE08[0] == NULL) {
        fn_80009948(lbl_80192E0C, 0xDB)(lbl_80192E38);
    }
    size = ((end - (u32)lbl_8040AE08[0]) >> 4) - 1;
    if (size > 0xFFFF) {
        fn_80009970(lbl_80192E0C, 0xDF)(lbl_80192E68);
    }
    *(s16*)lbl_8040AE08[0] = size;
    lbl_8040AE08[0] = NULL;
    out.address = end;
    return out;
}
