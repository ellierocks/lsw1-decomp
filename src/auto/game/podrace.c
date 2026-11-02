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
