#include "types.h"

extern s16 lbl_801BB44C[];
extern s16 lbl_801BB452[];
extern s16 lbl_801BB456[];
extern f32 lbl_801BB460[];
extern s32 lbl_80408488;
extern s32 lbl_8040848C;
extern s32 lbl_80408490;
extern s32 lbl_804085C0;
extern f32 lbl_8040870C;

void fn_80066290(void)
{

}

void fn_80066474(s32 a)
{
    lbl_80408488 = a;
}

void fn_8006647C(s32 a)
{
    lbl_8040848C = a;
}

void fn_80066484(s32 a)
{
    lbl_80408490 = a;
}

s32 fn_8006648C(void)
{
    return -1;
}

void fn_80066494(void)
{

}

void fn_800698B0(s32 a)
{
    lbl_804085C0 = a;
}

void fn_8006DEB0(f32 x)
{
    lbl_8040870C = x;
}

void fn_8006DF18(void)
{

}

s32 fn_8006DF50(void)
{
    return lbl_801BB452[0];
}

s32 fn_8006DF5C(void)
{
    return lbl_801BB456[0];
}

s32 fn_8006E2C0(void)
{
    return lbl_801BB44C[0];
}

void fn_8006E370(f32 x)
{
    lbl_801BB460[0] = x;
}

extern s32 fn_800076E4(const char*, const char*, s32);

s32 fn_80069900(const char* name)
{
    s32 i;
    if (lbl_804085C0) {
        for (i = 0; i < *(s32*)((u8*)lbl_804085C0 + 0x20); i++) {
            u8* entries = *(u8**)((u8*)lbl_804085C0 + 0x24);
            if (fn_800076E4(*(const char**)(entries + i * 0x50 + 0x44), name, 0x13) == 0)
                return i;
        }
    }
    return -1;
}

extern s32 lbl_804086EC, lbl_804086E8, lbl_80408298;
extern void fn_80044BF4(s32);

void fn_8006E00C(void)
{
    s32 minimum;
    if (!lbl_804086EC && !lbl_804086E8 && lbl_80408298 != -1 &&
        (u32)((u16)lbl_801BB44C[0] - 11) > 2) {
        minimum = 5;
        fn_80044BF4(lbl_801BB44C[2]);
        if (lbl_801BB44C[0] > 10) return;
        if (lbl_801BB44C[0] >= minimum)
            fn_80044BF4(1 - lbl_801BB44C[2]);
    }
}

extern s32 lbl_804086DC;
extern f32 lbl_804086FC;
extern void fn_80044C30(s32, s32);

void fn_8006DF68(f32 volume)
{
    if (!lbl_804086E8 && !lbl_804086EC) {
        lbl_804086FC = volume;
        if (lbl_801BB44C[0] == 4) {
            s32 index = lbl_801BB44C[3];
            s32* entries = (s32*)lbl_804086DC;
            fn_80044C30(lbl_801BB44C[2], (s32)((f32)entries[index * 5 + 3] * volume));
        }
    }
}

extern void fn_80044BB8(s32);

void fn_8006E094(void)
{
    s32 minimum;
    if ((!lbl_804086EC || !lbl_804086E8) && lbl_80408298 != -1 &&
        (u32)((u16)lbl_801BB44C[0] - 11) > 2) {
        minimum = 5;
        fn_80044BB8(lbl_801BB44C[2]);
        if (lbl_801BB44C[0] > 10) return;
        if (lbl_801BB44C[0] >= minimum)
            fn_80044BF4(1 - lbl_801BB44C[2]);
    }
}

struct AudioNameEntry { const char* name; u8 pad[0x10]; s16 linked; u8 rest[0x2a]; };
extern struct AudioNameEntry lbl_803B6E04[];
extern char lbl_8019854C[], lbl_8019857C[];
extern void (*fn_80009970(char*, s32))();

s32 fn_8006DDC4(const char* name, s32 length)
{
    struct AudioNameEntry* entry = lbl_803B6E04;
    s32 i;
    for (i = 0; i < 800; i++) {
        if (fn_800076E4(name, (entry++)->name, length) == 0)
            return i;
    }
    (*fn_80009970(lbl_8019854C, 0x3ef))(lbl_8019857C, name);
    return -1;
}

extern s32 fn_80044C78(s32);

s32 fn_8006CECC(void)
{
    s32 index = 1 - lbl_801BB44C[2];
    u8* pending = (u8*)lbl_801BB44C + 0x12;
    if (pending[index]) {
        pending[index]--;
        return 0;
    }
    if (lbl_804086E8 || lbl_804086EC)
        return 0;
    if (fn_80044C78(index) == 2) return 0;
    return 1;
}

extern s32 fn_8006E37C(s32);
extern s32 fn_8006E394(s32, s32);

void fn_8006DC1C(s32* list, void (*callback)(s32))
{
    s32 value = *list++;
    while (value != -1) {
        if (value >= 0) {
            s32 linked;
            callback(value);
            linked = lbl_803B6E04[value].linked;
            if (linked != -1) {
                s32 i;
                s32 count = fn_8006E37C(linked);
                for (i = 0; i < count; i++)
                    callback(fn_8006E394(linked, i));
            }
        }
        value = *list++;
    }
}
