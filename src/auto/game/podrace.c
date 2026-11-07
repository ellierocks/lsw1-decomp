#include "types.h"

extern s32 lbl_8040B2BC;
extern s32 lbl_8040B2B8;
extern s32 lbl_8040A80C;
extern s32 lbl_8040B2CC;
void fn_80146F1C(void)
{

}

void fn_801508F4(void)
{

}

void fn_8014CF8C(void)
{
    lbl_8040B2B8 = 0;
    lbl_8040A80C = 0;
    lbl_8040B2CC = 0;
    lbl_8040B2BC = 0;
}

s32 DrawTopShelf(s32);
extern s32 gShopCurrentCategory;
extern s32 (*gShopDrawAltCallback)();

void Shop_DrawSubMenuAndTopShelf(void) {
    if (((s32 (*)()) gShopDrawAltCallback) != NULL) {
        gShopDrawAltCallback();
    }
    DrawTopShelf(gShopCurrentCategory);
}

extern s32 (*gShopDrawCallback)();
extern s32 lbl_80409B98;

void fn_8014BD04(void) {
    if (((s32) lbl_80409B98 != 0) && (((s32 (*)()) gShopDrawCallback) != NULL)) {
        gShopDrawCallback();
    }
}

s32 fn_8014ABAC();

void fn_8014BE90(void) {
    fn_8014ABAC();
}

s32 PodraceUpdateStartLights();
s32 fn_8014D7C8();
s32 fn_8014DA40();
s32 fn_8014EBBC();

void fn_8014FD9C(void) {
    fn_8014EBBC();
    PodraceUpdateStartLights();
    fn_8014DA40();
    fn_8014D7C8();
}

extern s32 lbl_804093B8;
extern s32 lbl_8040B108;

void fn_8014FE74(void) {
    s32 var_r9;

    var_r9 = 0x1B;
    if ((s32) lbl_8040B108 > 2) {
        var_r9 = 0x1E;
    }
    lbl_804093B8 = var_r9;
}

s32 fn_801451A4(void *);
extern void *lbl_80409410;

void fn_80146EF4(void) {
    fn_801451A4((*(s32 *)((s8 *)(lbl_80409410) + (0x1984))));
}

s32 PodraceCreateMine();
s32 (*fn_80009948(s32 *, s32))();
void fn_8010BD90(void);
s32 fn_80142BA8(s32, s32);
s32 memset();
extern void *lbl_80409410;
extern s32 lbl_80409C94;
extern s32 str_Assert_Podrace_NotWorking[];
extern s32 str_SourceFile_Podrace[];

void fn_8014FD18(void) {
    s32 temp_r0;

    temp_r0 = (*(s32 *)((s8 *)(lbl_80409410) + (0x198C)));
    lbl_80409C94 = temp_r0;
    if (temp_r0 == 0) {
        fn_80009948(str_SourceFile_Podrace, 0x326)(str_Assert_Podrace_NotWorking);
    }
    memset(lbl_80409C94, 0, 0xB020);
    fn_80142BA8(lbl_80409C94, 0x20);
    PodraceCreateMine();
    fn_8010BD90();
}

s32 ProcessSpaceLevel(s32);
s32 fn_800D875C(s32, s32);
s32 fn_80145F18(void *, s32 *);
extern f32 lbl_801AF6FC[];
extern s32 lbl_801E184C[];
extern u8 lbl_803E0411[];
extern f32 lbl_803E8420[];
extern s32 lbl_804093D0;
extern void *lbl_80409410;
extern u8 lbl_8040B060[];

void fn_80146DF8(void) {
    fn_80145F18(lbl_80409410, lbl_801E184C);
    ProcessSpaceLevel((*(s32 *)((s8 *)(lbl_80409410) + (0x1984))));
    if (((s32) lbl_804093D0 == 0) && ((s32) lbl_8040B060[0] == 0) && ((s32) lbl_803E0411[0] == 0) && ((lbl_803E8420[0] >= lbl_801AF6FC[0]))) {
        fn_800D875C(0x26A, 0);
        lbl_8040B060[0] = 1;
    }
}

void fn_801470AC(void) {
    volatile s32 pad[4];
}

s32 fn_800130F4(void *);
s32 fn_80013200(void *, s32);
typedef struct { s32 a, b, c; } PodSnd12;
extern PodSnd12 lbl_80407154[];

void fn_8014D01C(u32 arg0) {
    s32 i;

    if (arg0 <= 2U) {
        for (i = 0; i < 3; i++) {
            if (fn_800130F4(&lbl_80407154[i]) != 0) {
                fn_80013200(&lbl_80407154[i], 0);
            }
        }
        if (fn_800130F4(&lbl_80407154[arg0]) != 0) {
            fn_80013200(&lbl_80407154[arg0], 1);
        }
    }
}

s32 PodraceUpdateStartLights();
s32 fn_8014D1F8();
s32 fn_8014EBBC();
extern PodSnd12 lbl_803EAD24[];

void fn_8014FFE8(void) {
    s32 i;

    fn_8014EBBC();
    PodraceUpdateStartLights();
    fn_8014D1F8();
    if ((s32) lbl_8040B060[0] == 1) {
        for (i = 0; i < 10; i++) {
            if (fn_800130F4(&lbl_803EAD24[i]) != 0) {
                fn_80013200(&lbl_803EAD24[i], 1);
            }
        }
        lbl_8040B060[0] = 2;
    }
}

s32 PodraceCreateMine();
s32 (*fn_80009948(s32 *, s32))();
#define fn_8010BD90 ((s32(*)(void))fn_8010BD90)
#define fn_80142BA8 ((s32(*)(s32, s32))fn_80142BA8)
s32 memset();
extern f32 lbl_801AFF48[];
extern s32 lbl_80409CFC;
extern s32 lbl_8040A820;
extern s32 lbl_8040A824;

void fn_8014FE90(void) {
    void *temp_r0;

    temp_r0 = (*(void **)((s8 *)(lbl_80409410) + (0x198C)));
    lbl_80409C94 = temp_r0;
    if (temp_r0 == NULL) {
        fn_80009948(str_SourceFile_Podrace, 0x326)(str_Assert_Podrace_NotWorking);
    }
    memset(lbl_80409C94, 0, 0xB020);
    fn_80142BA8(lbl_80409C94, 0x20);
    PodraceCreateMine();
    fn_8010BD90();
    lbl_80409CFC = 0;
    lbl_8040A820 = 0;
    lbl_8040A824 = 1;
    if ((s32) lbl_8040B108 <= 1) {
        (*(f32 *)((s8 *)(lbl_80409C94) + (0xB000))) = (f32) lbl_801AFF48[0];
    }
}
#undef fn_8010BD90
#undef fn_80142BA8

s32 NuSpecialFindMultiWC(s32 *, s32);
s32 fn_800133FC(void *);
extern s32 lbl_80407184[];
extern PodSnd12 lbl_80407190[];

void fn_8014CFA4(void) {
    s32 i;

    for (i = 0; i < lbl_8040B2BC; i++) {
        if (fn_800130F4(&lbl_80407190[i]) != 0) {
            NuSpecialFindMultiWC(lbl_80407184, fn_800133FC(&lbl_80407190[i]));
        }
    }
}
