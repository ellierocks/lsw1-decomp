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
