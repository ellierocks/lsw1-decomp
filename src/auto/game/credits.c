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

u8* FindObstacle(u8* scene, const char* name)
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

s32 NuMtxTranslate(s32, s32);
s32 fn_8000DD40(s32, u16);
s32 fn_8000E3C0(s32, u16);
s32 fn_800CFE08(s32, s32, s32);
extern u16 lbl_8040B05C;
extern u16 lbl_8040B120;

void fn_8010B940(s32 arg0, s32 arg1, s32 arg2) {
    fn_800CFE08(arg1, 0, 0);
    fn_8000DD40(arg0, lbl_8040B05C);
    fn_8000E3C0(arg0, lbl_8040B120);
    NuMtxTranslate(arg0, arg2);
}

typedef struct { u8 pad[0x250]; u8 arr[1]; } D250;
u8 fn_80108DA8(D250 *arg0, s32 arg1) { return arg0->arr[arg1]; }

s32 NuFileExists(void *);
s32 fn_8006A6C8(s32 *, s32, s32);
s32 fn_800911D0(void *, void *, s32, s32, s32 *, s32);
int sprintf(void *, void *, ...);
extern s32 lbl_801A9CFC[];
extern s32 lbl_801DE70C[];

void LoadParticleFile(void *arg0) {
    char sp8[0x100];

    (*(s32 *)((s8 *)(arg0) + (0x5E0))) = -1;
    sprintf(sp8, lbl_801A9CFC, arg0 + 0x80);
    if (NuFileExists(sp8) != 0) {
        (*(s32 *)((s8 *)(arg0) + (0x5E0))) = fn_8006A6C8(sp8, 1, (*(s32 *)((s8 *)(arg0) + (0x1C0))));
    }
    (*(s32 *)((s8 *)(arg0) + (0x1B8))) = fn_800911D0(arg0 + 0x184, arg0 + 0x188, 0x100, 0x42, lbl_801DE70C, (*(s32 *)((s8 *)(arg0) + (0x5E0))));
}

void fn_80007610(void *, void *);
void fn_80007680(void *, void *);
s32 memset();

void fn_80108CCC(s32 arg0, s32 arg1) {
    if ((arg0 != 0) && (arg1 != 0)) {
        fn_80007680(arg0 + 0x10, arg1);
        memset(arg1, 0, 0x160);
        fn_80007610(arg0 + 8, arg1);
    }
}

s32 TriggerObstacle(void *, void *);
extern void *lbl_80409410;

void fn_80108DB4(void *arg0, void *arg1) {
    s32 var_r30;
    u8 temp_r0;
    void *var_r31;

    var_r31 = (*(void **)((s8 *)(arg0) + (0x238)));
    if (var_r31 != NULL) {
        var_r30 = 0;
        if (var_r30 < (s32) (*(s32 *)((s8 *)(lbl_80409410) + (0x23C)))) {
            do {
                if (((s32) (*(u8 *)((s8 *)(var_r31) + (0x22))) == 0) && !((*(s32 *)((s8 *)(var_r31) + (0x18))) & 0x20000) && ((s32) (*(u8 *)((s8 *)(var_r31) + (0x1F))) == 0)) {
                    temp_r0 = (*(u8 *)((s8 *)(var_r31) + (0x21)));
                    if (((s8) temp_r0 != -1) && ((s8) (*(u8 *)((s8 *)((*(void **)((s8 *)(arg1) + (0xEEC)))) + (0x7E7))) == (s8) temp_r0)) {
                        TriggerObstacle(var_r31, arg1);
                    }
                }
                var_r30 += 1;
                var_r31 += (*(u16 *)((s8 *)(var_r31) + (0x1C)));
            } while (var_r30 < (s32) (*(s32 *)((s8 *)(lbl_80409410) + (0x23C))));
        }
    }
}
