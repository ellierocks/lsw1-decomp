#include "types.h"

extern f32 lbl_8040B240;
extern f32 lbl_8040B244;
extern f32 lbl_8040B248;
extern s32 lbl_8040B0AC;
extern f32 lbl_8040B0B0;
extern f32 lbl_8040B0B4;
extern f32 lbl_8040B0B8;
extern s32 lbl_801DEC0C[];

void fn_8010BBD8(u8* a)
{
    f32 x = *(f32*)(a + 140);
    f32 y = *(f32*)(a + 144);
    f32 z = *(f32*)(a + 148);
    s32 one = 1;
    lbl_8040B0B0 = x;
    lbl_8040B0B4 = y;
    lbl_8040B0B8 = z;
    lbl_8040B240 = x;
    lbl_8040B244 = y;
    lbl_8040B248 = z;
    lbl_8040B0AC = one;
}

void fn_8010BC08(u8* a)
{
    f32 x = *(f32*)(a + 140);
    f32 y = *(f32*)(a + 144);
    f32 z = *(f32*)(a + 148);
    lbl_8040B0B0 = x;
    lbl_8040B0B4 = y;
    lbl_8040B0B8 = z;
}

void fn_8010BD90(void)
{
    s32 v = -1;
    lbl_801DEC0C[1] = v;
    lbl_801DEC0C[2] = v;
    lbl_801DEC0C[3] = v;
    lbl_801DEC0C[4] = v;
    lbl_801DEC0C[5] = v;
    lbl_801DEC0C[6] = v;
    lbl_801DEC0C[7] = v;
    lbl_801DEC0C[0] = v;
}

extern s32 NuStrICmp(const char*, const char*);

u8* fn_80109294(u8* scene, const char* name)
{
    u8* entry;
    s32 i;
    if (scene && (entry = *(u8**)(scene + 0x230))) {
        for (i = 0; i < *(s32*)(scene + 0x234); i++, entry += 0x30) {
            if (NuStrICmp(name, (char*)entry) == 0)
                return entry;
        }
    }
    return 0;
}

void fn_80109314(u8* scene)
{
    u8* entry;
    s32 i;
    u8* selection;
    if (scene && (entry = *(u8**)(scene + 0x230))) {
        for (i = 0; i < *(s32*)(scene + 0x234); i++, entry += 0x30) {
            u8* object = *(u8**)(entry + 0x28);
            selection = scene + 0x250;
            if ((!object || (*(u16*)(object + 0x20) & 0x101)) &&
                (*(s8*)(entry + 0x2d) == -1 ||
                 selection[*(s8*)(entry + 0x2d)] == entry[0x2e]))
                entry[0x2c] = 1;
            else
                entry[0x2c] = 0;
        }
    }
}

u8* fn_80108E6C(u8* scene, const char* name)
{
    u8* entry = *(u8**)(scene + 0x238);
    s32 i;
    if (entry) {
        for (i = 0; i < *(s32*)(scene + 0x23c); i++) {
            if (NuStrICmp((char*)entry, name) == 0)
                return entry;
            entry += *(u16*)(entry + 0x1c);
        }
    }
    return 0;
}

u8* fn_80109128(u8* scene, const char* name)
{
    u8* entry;
    s32 i;
    if (scene) {
        entry = *(u8**)(scene + 0x248);
        for (i = 0; i < *(s32*)(scene + 0x24c); i++) {
            const char* entry_name = *(const char**)entry;
            if (entry_name && NuStrICmp(name, entry_name) == 0)
                return entry;
            entry += *(u16*)(entry + 4);
        }
    }
    return 0;
}

s32 fn_8010B6FC(u8* table, const char* name)
{
    s32 i;
    if (table) {
        for (i = *(s32*)table; i < *(s32*)(table + 4); i++) {
            u8* entries = *(u8**)(table + 8);
            if (NuStrICmp(name, (char*)(entries + i * 0x14 + 4)) == 0)
                return *(s32*)(*(u8**)(table + 8) + i * 0x14);
        }
    }
    return -1;
}
