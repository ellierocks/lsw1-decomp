#include "types.h"

extern s32 lbl_80409700;
extern s32 lbl_8040B204;
extern s32 lbl_8040B20C;
extern s32 lbl_804096FC;
extern f32 lbl_801A9A44[];
extern f32 lbl_801A9A48[];
extern f32 lbl_80409798;
extern f32 lbl_803FFE2C[];
extern s32 lbl_802E231C[];
extern s32 lbl_8040973C;
extern f32 lbl_8040A720;
extern f32 lbl_8040A724;
extern s32 lbl_8040B100;

void fn_800E32F8(s32 a)
{
    lbl_8040B100 = a;
}

void fn_800E631C(void)
{

}

void fn_801039D0(u8* a)
{
    *(s16*)(a + 78) = 1;
}

void fn_80103F64(void)
{
    lbl_8040973C = 0;
}

void fn_80103F70(f32 x, s32 a)
{
    lbl_8040973C = a;
    lbl_8040A720 = x;
    lbl_8040A724 = (x * x);
}

void fn_8010438C(void)
{
    lbl_802E231C[0] = 0;
}

void fn_800E6320(void)
{
    lbl_8040B204 = 0;
    lbl_8040B20C = 0;
    lbl_804096FC = 0;
    lbl_80409700 = 0;
}

void fn_80103EA8(u8* a)
{
    f32 k = lbl_801A9A44[0];
    f32 m = lbl_801A9A48[0];
    s32 z = 0;
    *(f32*)(a + 1856) = k;
    *(f32*)(a + 1860) = k;
    *(f32*)(a + 1928) = k;
    *(s32*)(a + 1696) = z;
    *(s32*)(a + 1700) = z;
    *(f32*)(a + 1864) = m;
}

void fn_8010403C(void)
{
    f32 k = lbl_80409798;
    lbl_803FFE2C[13] = k;
    lbl_803FFE2C[27] = k;
}

void fn_80108700(u8* a)
{
    *(s32*)(a + 16) = 0;
    *(s32*)(a + 20) = 0;
}

extern u8* lbl_80409410;
extern s32 NuStrICmp(const char*, const char*);

static inline u8* scene_find(u8* scene, const char* name)
{
    u8* entry = *(u8**)(scene + 0x238);
    s32 i;
    if (entry) {
        for (i = 0; i < *(s32*)(scene + 0x23c); i++) {
            if (NuStrICmp((char*)entry, name) == 0) return entry;
            entry += *(u16*)(entry + 0x1c);
        }
    }
    return 0;
}

void fn_80108710(const char* name, u8* entry)
{
    if (entry) *(u32*)(entry + 0x18) |= 8;
    else if (name) {
        entry = scene_find(lbl_80409410, name);
        if (entry) *(u32*)(entry + 0x18) |= 8;
    }
}

void fn_801087C4(const char* name, u8* entry)
{
    if (entry) *(u32*)(entry + 0x18) &= ~8;
    else if (name) {
        entry = scene_find(lbl_80409410, name);
        if (entry) *(u32*)(entry + 0x18) &= ~8;
    }
}

s32 NuMtxTranslate(s32 *, s32 *);
s32 fn_8000DD40(s32 *, u16);
s32 fn_8000E3C0(s32 *, u16);
s32 fn_8000E540(s32 *, u16);
extern f32 lbl_801A5D64[];
extern s32 lbl_801DD100[];
extern s32 lbl_802E18B4[];
extern s32 lbl_80408C88;
extern f32 lbl_80408CA0;
extern u16 lbl_804096B8;
extern u16 lbl_804096BA;
extern u16 lbl_804096BC;

void fn_800E3278(s32 arg0) {
    if ((s32) lbl_80408C88 != 0) {
        fn_8000DD40(lbl_802E18B4, lbl_804096B8);
        fn_8000E3C0(lbl_802E18B4, lbl_804096BA);
        fn_8000E540(lbl_802E18B4, lbl_804096BC);
        NuMtxTranslate(lbl_802E18B4, lbl_801DD100);
        if (arg0 != 0) {
            lbl_80408CA0 = lbl_801A5D64[0];
        }
    }
}

s32 fn_8006DCD4(s32 *, void *);
extern s32 lbl_801A5DE0[];
extern s32 lbl_801A5DEC[];
extern s32 lbl_801A5DF8[];
extern s32 lbl_801A5E08[];
extern s32 lbl_801A5E14[];
extern s32 lbl_801A5E20[];
extern s32 lbl_801A5E2C[];
extern s32 lbl_801A5E3C[];
extern s32 lbl_801A5E48[];

void fn_800E356C(void *arg0, s32 arg1) {
    s32 temp_r9;

    temp_r9 = (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4)));
    if (!(temp_r9 & 0x40)) {
        if (temp_r9 & 0x02000008) {
            switch (arg1) {                         /* irregular */
            case 0:
                fn_8006DCD4(lbl_801A5DE0, arg0 + 0x1AC);
                return;
            case 1:
                fn_8006DCD4(lbl_801A5DEC, arg0 + 0x1AC);
                return;
            case 3:
                if ((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) {
                    fn_8006DCD4(lbl_801A5DF8, arg0 + 0x1AC);
                    return;
                }
                fn_8006DCD4(lbl_801A5E08, arg0 + 0x1AC);
                return;
            case 4:
                if ((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) {
                    fn_8006DCD4(lbl_801A5E14, arg0 + 0x1AC);
                    return;
                }
                fn_8006DCD4(lbl_801A5E20, arg0 + 0x1AC);
                return;
            case 2:
                if ((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) {
                    fn_8006DCD4(lbl_801A5E2C, arg0 + 0x1AC);
                    return;
                }
                fn_8006DCD4(lbl_801A5E3C, arg0 + 0x1AC);
                return;
            }
        } else {
            fn_8006DCD4(lbl_801A5E48, arg0 + 0x6C);
        }
    }
}

s32 fn_8006DBB8(s32 *);
extern s32 lbl_802E1C74[];

void fn_800E6504(void) {
    fn_8006DBB8(lbl_802E1C74);
}

extern f32 lbl_801A9A0C[];
extern f32 lbl_80408E9C;

void fn_8010388C(void *arg0, void *arg1, void *arg2, f32 farg0) {
    f32 temp_f13;
    f32 temp_f13_2;
    f32 temp_f13_3;
    f32 var_f1;

    var_f1 = farg0 * lbl_80408E9C;
    if (var_f1 > lbl_801A9A0C[0]) {
        var_f1 = lbl_801A9A0C[0];
    }
    temp_f13 = (*(f32 *)((s8 *)(arg1) + (0)));
    (*(f32 *)((s8 *)(arg0) + (0))) = (f32) ((((*(f32 *)((s8 *)(arg2) + (0))) - temp_f13) * var_f1) + temp_f13);
    temp_f13_2 = (*(f32 *)((s8 *)(arg1) + (4)));
    (*(f32 *)((s8 *)(arg0) + (4))) = (f32) ((((*(f32 *)((s8 *)(arg2) + (4))) - temp_f13_2) * var_f1) + temp_f13_2);
    temp_f13_3 = (*(f32 *)((s8 *)(arg1) + (8)));
    (*(f32 *)((s8 *)(arg0) + (8))) = (f32) ((((*(f32 *)((s8 *)(arg2) + (8))) - temp_f13_3) * var_f1) + temp_f13_3);
}

s32 fn_800E6638(s32, s32, s32, f32, f32);
s32 fn_800E6BF8();
extern f32 lbl_801A9A14[];

void fn_80103988(s32 arg0) {
    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9A14[0], lbl_801A9A14[0]);
}

s32 fn_80074934(void *);

void fn_801045B0(void *arg0) {
    (*(s32 *)((s8 *)(arg0) + (0x104))) = (s32) (((*(s32 *)((s8 *)(arg0) + (0x104))) | 0x80) & 0xFFFFBFFF);
    fn_80074934(arg0);
}

s32 fn_80095750(f32, f32);
s32 fn_80095A44(s32, s32, s32, s32, void *);
s32 fn_800A5C64();
extern f32 lbl_801A9AA0[];
extern f32 lbl_80408E9C;

void fn_80104628(void *arg0) {
    s32 temp_r30;

    (*(s16 *)((s8 *)(arg0) + (0x4E))) = 1;
    fn_800A5C64();
    if ((s32) (*(u8 *)((s8 *)(arg0) + (0x290))) != 0) {
        temp_r30 = fn_80095750(lbl_801A9AA0[0], lbl_80408E9C);
        if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0x1C))) != 0) {
            fn_80095A44((*(s32 *)((s8 *)(lbl_80409410) + (0x1B8))), 5, (*(s32 *)((s8 *)(arg0) + (0xEEC))) + 0x244, temp_r30, arg0 + 0x78);
        }
        if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0x20))) != 0) {
            fn_80095A44((*(s32 *)((s8 *)(lbl_80409410) + (0x1B8))), 5, (*(s32 *)((s8 *)(arg0) + (0xEEC))) + 0x284, temp_r30, arg0 + 0x78);
        }
    }
}

s32 fn_800A5C64(void *);
s32 fn_800F8414(void *, s32, s32);
extern f32 lbl_801A9AB4[];

void fn_80104738(void *arg0) {
    void *temp_r11;
    void *temp_r9;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0x19) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 1U;
        temp_r11 = (*(void **)((s8 *)(temp_r9) + (0x61C)));
        if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A9AB4[0])) {
            fn_800F8414(arg0, 1, 0);
        }
    }
    fn_800A5C64(arg0);
}

s32 fn_800E6638(s32, s32, s32, f32, f32);
s32 fn_800E6BF8();
extern f32 lbl_801A9AE4[];

void fn_80104EB4(s32 arg0) {
    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9AE4[0], lbl_801A9AE4[0]);
}

s32 fn_800E6638(s32, s32, s32, f32, f32);
s32 fn_800E6BF8();
extern f32 lbl_801A9AEC[];

void fn_80104F50(s32 arg0) {
    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9AEC[0], lbl_801A9AEC[0]);
}

void *fn_80105DEC();

void fn_801088F4(void) {
    void *temp_r3;

    temp_r3 = fn_80105DEC();
    if (temp_r3 != NULL) {
        (*(s32 *)((s8 *)(temp_r3) + (0x18))) = (s32) (((*(s32 *)((s8 *)(temp_r3) + (0x18))) | 0x2008) & ~0x10A0);
    }
}

extern s32 lbl_801A5DB4[];
extern s32 lbl_801A5DC8[];
extern s32 lbl_804096E8;

void fn_800E6484(void) {
    s32 var_r0;

    if ((s32) lbl_804096E8 != 0) {
        fn_8006DCD4(lbl_801A5DB4, 0);
        var_r0 = 0;
    } else {
        fn_8006DCD4(lbl_801A5DC8, 0);
        var_r0 = 1;
    }
    lbl_804096E8 = var_r0;
}
