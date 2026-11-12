#include "types.h"

struct ngc_video_color_s { u8 r, g, b, a; };

extern void fn_80032CB4(u8* p);

extern u8 lbl_8040A404[4];

extern s32 lbl_8040A3C4[2];

extern s32 lbl_80407F94;
extern s32 lbl_80407F5C;

extern s32 lbl_8021FCB0[];

extern s32 lbl_8040A418;
extern s32 lbl_8040A414;

extern s32 lbl_80407FAC;

extern f32 lbl_8040A3FC;
extern f32 lbl_8040A400;

extern s32 lbl_8040A3D8;

extern u16 lbl_803404DA[];

extern u16 lbl_803404D8[];

extern f32 lbl_80407F78;

extern s32 lbl_80407F6C;

extern s32 lbl_8040A3F0;

extern s32 lbl_8040A3E8;

s32 fn_80037218(void)
{
    return lbl_8040A3E8;
}

s32 fn_80037220(void)
{
    return lbl_8040A3F0;
}

void fn_800372D4(s32 a)
{
    lbl_80407F6C = a;
}

void fn_800372E8(s32 a)
{
    lbl_8040A3E8 = a;
}

void fn_8003735C(f32 a)
{
    lbl_80407F78 = a;
}

u32 fn_80037200(void)
{
    return lbl_803404D8[0];
}

u32 fn_8003720C(void)
{
    return lbl_803404DA[0];
}

void fn_800372DC(void)
{
    lbl_8040A3D8 = 0;
}

void fn_8003777C(f32 a, f32 b)
{
    lbl_8040A3FC = a;
    lbl_8040A400 = b;
}

void fn_80037B74(void)
{
    lbl_80407FAC = 1;
}

void fn_8003776C(void)
{
    s32 z = 0;
    lbl_8040A414 = z;
    lbl_8040A418 = z;
}

s32 fn_80033FF0(s32 i)
{
    return lbl_8021FCB0[i - 1];
}

void fn_80037170(void)
{
    if (lbl_80407F94 != 0) {
        lbl_80407F5C = 1;
    }
}

void fn_80033FA8(s32 i)
{
    if (i != -1) {
        lbl_8021FCB0[i] = 0;
    }
}

s32 fn_80034B98(s32 i)
{
    if (lbl_8040A3C4[i] == -1) {
        return 0;
    }
    return 1;
}

void fn_80037788(u32 v)
{
    u8* b = lbl_8040A404;
    b[0] = v;
    b[1] = v >> 8;
    b[2] = v >> 16;
    b[3] = v >> 24;
}

void fn_80033FC8(u8* p)
{
    *(s32*)(p + 0xf4) = -1;
    fn_80032CB4(p);
}

s32 GXReadDrawSync();
s32 fn_800165B4(s32, s32);
s32 fn_80016660(s32, s32, s32 *);
s32 fn_800166C4(s32, s32);
extern s32 lbl_80192660[];
extern s32 lbl_80192664[];
extern s32 lbl_8040AD9C;

void fn_80037188(void) {
    fn_800166C4(lbl_8040AD9C, 0);
    do {

    } while (GXReadDrawSync() != 0xB000);
    fn_80016660(lbl_8040AD9C, 0, lbl_80192660);
    fn_800165B4(lbl_8040AD9C, 2);
    fn_80016660(lbl_8040AD9C, 2, lbl_80192664);
}

s32 GXSetProjectionv(s32);
extern s32 lbl_80407FA8;
extern s32 lbl_80407FAC;

void fn_80037B80(s32 arg0) {
    fn_8003776C();
    if (((s32) lbl_80407FAC != 0) || (arg0 != (s32) lbl_80407FA8)) {
        GXSetProjectionv(arg0);
        lbl_80407FA8 = arg0;
        lbl_80407FAC = 0;
    }
}

s32 (*fn_80009948(s32 *, s32))();
extern s32 GXEurgb60Hz480IntDf[];
extern s32 GXMpal480IntDf[];
extern s32 GXNtsc480IntDf[];
extern s32 GXPal264Int[];
extern s32 GXPal528IntDf[];
extern s32 lbl_8019253C[];
extern s32 lbl_80192678[];
extern s32 lbl_8040A3CC;

s32 fn_80037228(void) {
    if (((s32) lbl_8040A3CC == GXNtsc480IntDf) || ((s32) lbl_8040A3CC == GXEurgb60Hz480IntDf) || ((s32) lbl_8040A3CC == GXMpal480IntDf)) {
        return 0x3C;
    }
    if (((s32) lbl_8040A3CC == GXPal528IntDf) || ((s32) lbl_8040A3CC == GXPal264Int)) {
        return 0x32;
    }
    fn_80009948(lbl_8019253C, 0x64E)(lbl_80192678);
    return 0;
}

s32 fn_800165B4(s32, s32);
s32 fn_80016660(s32, s32, s32 *);
s32 fn_800166C4(s32, s32);
extern s32 lbl_80192598[];

void fn_800372F0(s32 arg0) {
    if (arg0 == 0xB001) {
        fn_800165B4(lbl_8040AD9C, 1);
        fn_80016660(lbl_8040AD9C, 1, lbl_80192598);
        return;
    } else if (arg0 == 0xB000) {
        fn_800166C4(lbl_8040AD9C, 1);
        return;
    }
}

void GXInitTexObj(void *, void *, u16, u16, s32, s32, s32, u8);
extern s32 lbl_8040A3DC;
extern s32 lbl_8040A3E0;
extern s32 lbl_8040A3E4;

void fn_8003708C(void *arg0, s32 arg1, s32 arg2)
{
    s32 var_r0;
    s32 var_r6;

    var_r0 = lbl_8040A3E0;
    var_r6 = lbl_8040A3E4;
    if ((var_r0 == 0) || (var_r6 == 0)) {
        var_r0 = 8;
        var_r6 = 8;
    }
    GXInitTexObj(arg0, lbl_8040A3DC, (u16)var_r0, (u16)var_r6, 4, arg1, arg2, 0);
}

s32 memset();
struct ngc_pad_s {
    u16 button;
    s8 stickX, stickY, substickX, substickY;
    u8 triggerLeft, triggerRight, analogA, analogB;
    s8 err;
    u8 pad;
};
extern struct ngc_pad_s gInputState[];
extern f32 lbl_8019242C[];
extern s32 lbl_802292F0[];
extern s32 lbl_80229300[];
extern f32 lbl_80229310[];
extern f32 lbl_80229320[];
extern f32 lbl_80229330[];
extern s32 lbl_80229340[];
extern s32 lbl_80229350[];
extern s32 lbl_80229360[];
extern s32 lbl_8040A3C8;

void fn_80034008(void)
{
    s32 var_ctr;
    s32 var_r10;

    memset(gInputState, 0, 0x30);
    memset(lbl_802292F0, 0, 0x10);
    var_ctr = 4;
    var_r10 = 0;
    do {
        *(f32 *)((var_r10 + ((s8 *)lbl_80229310))) = lbl_8019242C[0];
        *(f32 *)((var_r10 + ((s8 *)lbl_80229320))) = lbl_8019242C[0];
        *(f32 *)((var_r10 + ((s8 *)lbl_80229330))) = lbl_8019242C[0];
        *(s32 *)((var_r10 + ((s8 *)lbl_80229300))) = 0;
        *(s32 *)((var_r10 + ((s8 *)lbl_80229340))) = 0xFF;
        *(s32 *)((var_r10 + ((s8 *)lbl_80229350))) = 0;
        *(s32 *)((var_r10 + ((s8 *)lbl_80229360))) = 0;
        var_r10 += 4;
        var_ctr -= 1;
    } while (var_ctr != 0);
    *lbl_8040A3C4 = -1;
    lbl_8040A3C8 = -1;
}

void GXCallDisplayList(s32, s32);
void GXClearVtxDesc();
void GXLoadPosMtxImm(s32 *, s32);
void GXLoadTexMtxImm(s32 *, s32, s32);
void GXSetCurrentMtx(s32);
void GXSetProjection(s32 *, s32);
void GXSetVtxAttrFmt(s32, s32, s32, s32, s32);
void GXSetVtxDesc(s32, s32);
void PSMTXIdentity(s32 *);
void fn_80030A34();
void fn_80030A54();
#define fn_80037B74 ((s32 (*)())fn_80037B74)
extern s32 lbl_80229370[];
extern s32 lbl_802293B0[];
extern void *lbl_8040ADB0;

void fn_80037364(void *arg0)
{
    f32 sp8[3][4];

    fn_80030A34();
    fn_80030A54();
    fn_80037B74();
    if ((s32)lbl_8040A414 == 0) {
        GXSetProjection(lbl_80229370, 1);
        GXLoadPosMtxImm(lbl_802293B0, 0);
        GXSetCurrentMtx(0);
        lbl_8040A414 = 1;
    }
    if ((s32)(*(s32 *)((s8 *)(arg0) + (0))) != (s32)lbl_8040A418) {
        GXClearVtxDesc();
        if ((*(s32 *)((s8 *)(arg0) + (0))) & 1) {
            GXSetVtxDesc(9, 1);
            GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        }
        if ((*(s32 *)((s8 *)(arg0) + (0))) & 4) {
            GXSetVtxDesc(0xB, 1);
            GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        }
        if ((*(s32 *)((s8 *)(arg0) + (0))) & 2) {
            GXSetVtxDesc(0xD, 1);
            GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
        }
        lbl_8040A418 = (*(s32 *)((s8 *)(arg0) + (0)));
    }
    if ((s32)(*(u8 *)((s8 *)(lbl_8040ADB0) + (0x30))) == 5) {
        PSMTXIdentity(sp8);
        sp8[0][3] = (*(f32 *)((s8 *)(lbl_8040ADB0) + (0x94)));
        sp8[1][3] = (*(f32 *)((s8 *)(lbl_8040ADB0) + (0x98)));
        GXLoadTexMtxImm(sp8, 0x1E, 1);
    }
    GXCallDisplayList((*(s32 *)((s8 *)(arg0) + (4))), (*(s32 *)((s8 *)(arg0) + (8))));
}
#undef fn_80037B74

void GXCopyTex(s32, s32);
void GXFlush();
void GXPixModeSync();
void GXSetDrawSync(u16);
void GXSetTexCopyDst(u16, u16, s32, u8);
void GXSetTexCopySrc(u16, u16, u16, u16);
void NuCameraClipTestExtents();
s32 fn_800165F8(s32, s32);
void fn_80035A24();
void fn_80035C10();
void fn_80038210();
extern f32 lbl_80192650[];
extern f32 lbl_80192654[];
extern f32 lbl_80192658[];
extern f32 lbl_8019265C[];
struct ngc_video_mode_s {
    u32 viTVmode;

    u16 fbWidth, efbHeight, xfbHeight, viXOrigin, viYOrigin, viWidth, viHeight;

    u32 xFBmode;
    u8 fieldRendering, aa;
    u8 samplePattern[12][2];
    u8 vfilter[7];
};
extern struct ngc_video_mode_s lbl_803404D4;
extern s32 lbl_80407F7C;
extern f32 lbl_80407F84;
extern f32 lbl_80407F88;

void fn_80036858(void)
{
    f32 temp_f13;
    f32 var_f0;
    s32 temp_r30;

    if ((s32)lbl_80407F5C != 0) {
        lbl_8040A3E0 = (s32)lbl_803404D4.fbWidth;
        lbl_8040A3E4 = (s32)lbl_803404D4.efbHeight;
        GXSetTexCopySrc(0U, 0U, lbl_803404D4.fbWidth, lbl_803404D4.efbHeight);
        GXSetTexCopyDst(lbl_803404D4.fbWidth, lbl_803404D4.efbHeight, 4, 0U);
        GXCopyTex(lbl_8040A3DC, 0);
        GXPixModeSync();
        lbl_80407F5C = 0;
    }
    fn_80035A24();
    if ((s32)lbl_80407F7C != 0) {
        fn_80035C10();
    }
    lbl_80407F84 = lbl_80407F88;
    temp_r30 = fn_800165F8(lbl_8040AD9C, 1);
    temp_f13 =
        (((f32)(f64)temp_r30 + (f32)(f64)fn_800165F8(lbl_8040AD9C, 2)) / *lbl_80192650) * *lbl_80192654;
    lbl_80407F88 = temp_f13;
    if ((s32)lbl_8040A3CC == GXPal528IntDf) {
        var_f0 = *lbl_80192658;
    } else {
        var_f0 = *lbl_8019265C;
    }
    lbl_80407F88 = temp_f13 / var_f0;
    NuCameraClipTestExtents();
    GXSetDrawSync(0xB000);
    GXFlush();
    fn_80038210();
}

extern f32 lbl_8019244C[];
extern f32 lbl_80192458[];
extern f32 lbl_8019245C[];
extern f32 lbl_80192460[];
extern f32 lbl_80192464[];

f32 fn_80034C5C(s32 arg0)
{
    f32 var_f1;
    s32 temp_r11;

    temp_r11 = lbl_8040A3C4[arg0];
    if (temp_r11 == -1) {
        return *lbl_8019244C;
    }
    var_f1 = ((f32)(f64)(s8)(u8)gInputState[temp_r11].stickX * *lbl_80192458) / *lbl_8019245C;
    if (var_f1 > *lbl_80192460) {
        var_f1 = *lbl_80192460;
    }
    if (var_f1 < *lbl_80192464) {
        var_f1 = *lbl_80192464;
    }
    return var_f1;
}

extern f32 lbl_80192468[];
extern f32 lbl_80192478[];
extern f32 lbl_8019247C[];
extern f32 lbl_80192480[];
extern f32 lbl_80192484[];

f32 fn_80034D04(s32 arg0)
{
    f32 var_f1;
    s32 temp_r11;

    temp_r11 = lbl_8040A3C4[arg0];
    if (temp_r11 == -1) {
        return *lbl_80192468;
    }
    var_f1 = ((f32)(f64)(s8)(u8)gInputState[temp_r11].stickY * *lbl_80192478) / *lbl_8019247C;
    if (var_f1 > *lbl_80192480) {
        var_f1 = *lbl_80192480;
    }
    if (var_f1 < *lbl_80192484) {
        var_f1 = *lbl_80192484;
    }
    return var_f1;
}

extern f32 lbl_80192488[];
extern f32 lbl_80192498[];
extern f32 lbl_8019249C[];
extern f32 lbl_801924A0[];
extern f32 lbl_801924A4[];

f32 fn_80034DAC(s32 arg0)
{
    f32 var_f1;
    s32 temp_r11;

    temp_r11 = lbl_8040A3C4[arg0];
    if (temp_r11 == -1) {
        return *lbl_80192488;
    }
    var_f1 = ((f32)(f64)(s8)(u8)gInputState[temp_r11].substickX * *lbl_80192498) / *lbl_8019249C;
    if (var_f1 > *lbl_801924A0) {
        var_f1 = *lbl_801924A0;
    }
    if (var_f1 < *lbl_801924A4) {
        var_f1 = *lbl_801924A4;
    }
    return var_f1;
}

extern f32 lbl_801924A8[];
extern f32 lbl_801924B8[];
extern f32 lbl_801924BC[];
extern f32 lbl_801924C0[];
extern f32 lbl_801924C4[];

f32 fn_80034E54(s32 arg0)
{
    f32 var_f1;
    s32 temp_r11;

    temp_r11 = lbl_8040A3C4[arg0];
    if (temp_r11 == -1) {
        return *lbl_801924A8;
    }
    var_f1 = ((f32)(f64)(s8)(u8)gInputState[temp_r11].substickY * *lbl_801924B8) / *lbl_801924BC;
    if (var_f1 > *lbl_801924C0) {
        var_f1 = *lbl_801924C0;
    }
    if (var_f1 < *lbl_801924C4) {
        var_f1 = *lbl_801924C4;
    }
    return var_f1;
}

extern f32 lbl_801924C8[];
extern f32 lbl_801924D8[];

f32 fn_80034EFC(s32 arg0)
{
    s32 temp_r11;

    temp_r11 = lbl_8040A3C4[arg0];
    if (temp_r11 == -1) {
        return *lbl_801924C8;
    }
    return (f32)(f64)gInputState[temp_r11].triggerLeft / *lbl_801924D8;
}

extern f32 lbl_801924DC[];
extern f32 lbl_801924E8[];

f32 fn_80034F68(s32 arg0)
{
    s32 temp_r11;

    temp_r11 = lbl_8040A3C4[arg0];
    if (temp_r11 == -1) {
        return *lbl_801924DC;
    }
    return (f32)(f64)gInputState[temp_r11].triggerRight / *lbl_801924E8;
}

extern s32 lbl_801B1CF8[];

s32 fn_80034BB8(s32 arg0, s32 arg1)
{
    s32 temp_r0;
    s32 var_r30;

    if (arg0 == 2) {
        var_r30 = 0;
        if ((fn_80034BB8(0, arg1) != 0) || (fn_80034BB8(1, arg1) != 0)) {
            var_r30 = 1;
        }
        return var_r30;
    }
    temp_r0 = lbl_8040A3C4[arg0];
    if (temp_r0 == -1) {
        return 0;
    }
    return gInputState[temp_r0].button & *(s32 *)(((s8 *)lbl_801B1CF8) + (arg1 * 4));
}

extern const f32 lbl_80192804[];
extern s32 *lbl_8040ACF0;
s32 fn_80037BD8(f32 farg0, f32 farg1, f32 farg2, f32 fpad3, f32 farg4)
{
    s32 temp_r9;

    temp_r9 = (*lbl_8040ACF0 + 3) & 0xFFFFFFFC;
    *lbl_8040ACF0 = temp_r9 + 0x1C;
    (*(f32 *)((s8 *)(temp_r9) + (4))) = farg0;
    (*(f32 *)((s8 *)(temp_r9) + (0xC))) = farg1;
    (*(f32 *)((s8 *)(temp_r9) + (0))) = (f32)lbl_80192804[0];
    (*(f32 *)((s8 *)(temp_r9) + (8))) = (f32)lbl_80192804[0];
    (*(f32 *)((s8 *)(temp_r9) + (0x10))) = (f32)lbl_80192804[0];
    (*(f32 *)((s8 *)(temp_r9) + (0x14))) = farg2;
    (*(f32 *)((s8 *)(temp_r9) + (0x18))) = farg4;
    return temp_r9;
}

extern f32 lbl_801924EC[];

void fn_80035050(s32 arg0)
{
    s32 temp_r0;
    s32 temp_r0_2;

    temp_r0 = *(s32 *)((s8 *)lbl_8040A3C4 + arg0 * 4);
    if (temp_r0 != -1) {
        temp_r0_2 = temp_r0 * 4;
        *(s32 *)((((s8 *)lbl_80229300) + temp_r0_2)) = 4;
        *(f32 *)((((s8 *)lbl_80229310) + temp_r0_2)) = lbl_801924EC[0];
        *(f32 *)((((s8 *)lbl_80229330) + temp_r0_2)) = lbl_801924EC[0];
        *(f32 *)((((s8 *)lbl_80229320) + temp_r0_2)) = lbl_801924EC[0];
        *(s32 *)((((s8 *)lbl_80229340) + temp_r0_2)) = 0xFF;
    }
}

extern s32 lbl_80407F30;
extern s32 lbl_80407F34;

void fn_80034FD4(s32 arg0, f32 farg0, f32 farg1)
{
    s32 temp_r0;
    s32 temp_r11;
    s32 temp_r3;

    temp_r3 = arg0 * 4;
    temp_r11 = *(s32 *)((s8 *)lbl_8040A3C4 + temp_r3);
    if ((temp_r11 != -1) && ((s32)lbl_80407F30 != 0) &&
        ((s32) * (s32 *)((s8 *)&lbl_80407F34 + temp_r3) != 0)) {
        temp_r0 = temp_r11 * 4;
        *(s32 *)((((s8 *)lbl_80229300) + temp_r0)) = 1;
        *(f32 *)((((s8 *)lbl_80229310) + temp_r0)) = farg0;
        *(f32 *)((((s8 *)lbl_80229330) + temp_r0)) = farg1;
        *(f32 *)((((s8 *)lbl_80229320) + temp_r0)) = farg1;
        *(s32 *)((((s8 *)lbl_80229340) + temp_r0)) = 0xFF;
    }
}

void C_MTXOrtho(s32 *, f32, f32, f32, f32, f32, f32);
void PSMTXIdentity(s32 *);
extern f32 lbl_80192748[];
extern f32 lbl_8019274C[];
extern f32 lbl_80407FA0;
extern f32 lbl_80407FA4;

void fn_800376AC(void)
{
    f32 temp_f31;

    temp_f31 = (f32)fn_8003720C() / lbl_80407FA4;
    C_MTXOrtho(lbl_80229370, 0.0f, temp_f31, 0.0f, (f32)fn_80037200() / lbl_80407FA0, 0.0f, 16777215.0f);
    PSMTXIdentity(lbl_802293B0);
    lbl_8040A414 = 0;
    lbl_8040A418 = 0;
}

void C_MTXOrtho(s32 *, f32, f32, f32, f32, f32, f32);
extern f32 lbl_80192758[];
extern f32 lbl_8019275C[];

void fn_800378F4(f32 farg0)
{
    f32 temp_f31;

    lbl_80407FA0 = farg0;
    temp_f31 = (f32)fn_8003720C() / lbl_80407FA4;
    C_MTXOrtho(lbl_80229370, 0.0f, temp_f31, 0.0f, (f32)fn_80037200() / lbl_80407FA0, 0.0f, 16777215.0f);
}

void C_MTXOrtho(s32 *, f32, f32, f32, f32, f32, f32);
extern f32 lbl_80192768[];
extern f32 lbl_8019276C[];

void fn_800379A0(f32 farg0)
{
    f32 temp_f31;

    lbl_80407FA4 = farg0;
    temp_f31 = (f32)fn_8003720C() / lbl_80407FA4;
    C_MTXOrtho(lbl_80229370, 0.0f, temp_f31, 0.0f, (f32)fn_80037200() / lbl_80407FA0, 0.0f, 16777215.0f);
}

extern s32 *lbl_8040A408;
extern u16 lbl_8040A40C;
union ngc_video_cursor_u {
    u8 *byte;
    u16 *half;
    u32 *word;
    f32 *real;
};
extern union ngc_video_cursor_u *lbl_8040A410;

void fn_800377AC(f32 x, f32 y, f32 z)
{
    *lbl_8040A410->real++ = x;
    *lbl_8040A410->real++ = y;
    *lbl_8040A410->real++ = -z;
    if (*lbl_8040A408 & 4) {
        *lbl_8040A410->word++ = *(u32 *)lbl_8040A404;
    }
    if (*lbl_8040A408 & 2) {
        *lbl_8040A410->real++ = lbl_8040A3FC;
        *lbl_8040A410->real++ = lbl_8040A400;
    }
    lbl_8040A40C++;
}

void GXInitTexObj(void *, void *, u16, u16, s32, s32, s32, u8);
extern s32 lbl_8040A3D4;
void fn_80037028(void *arg0, s32 arg1, s32 arg2)
{
    u16 new_var;
    u32 var_r5;
    u32 var_r6;
    {
        s8 *lbl_803404D4_p = (s8 *)&lbl_803404D4;
        new_var = *((u16 *)(lbl_803404D4_p + 4));
        var_r6 = ((u16)(*((u16 *)(lbl_803404D4_p + 6)))) >> 1U;
        var_r5 = ((u16)new_var) >> 1U;
    }
    if ((var_r5 == 0) || (((s32)var_r6) == 0)) {
        var_r5 = 8;
        var_r6 = 8;
    }
    GXInitTexObj(arg0, lbl_8040A3D4, var_r5, var_r6, 4, arg1, arg2, 0);
}

void DCFlushRange(void *, u32);

void fn_80037864(void)
{
    s32 temp_r0_2;
    s8 *temp_r11;
    s8 *temp_r9;
    u32 temp_r0;
    u32 var_r10;
    void *temp_r7;

    var_r10 = 0U;
    temp_r7 = (*(void **)((s8 *)(lbl_8040A408) + (4)));
    (*(u16 *)((s8 *)(temp_r7) + (2))) = (u16)lbl_8040A40C;
    temp_r11 = lbl_8040A410->byte;
    temp_r0 = ((s32)(temp_r11 + 0x1F) & 0xFFFFFFE0) - (u32)temp_r11;
    if (var_r10 < temp_r0) {
        do {
            var_r10 += 1;
            *lbl_8040A410->byte++ = 0;
        } while (var_r10 < temp_r0);
    }
    temp_r0_2 = lbl_8040A410->byte - (u8 *)temp_r7;
    {
        s8 *lbl_8040A408_p = (s8 *)lbl_8040A408;
        (*(s32 *)(lbl_8040A408_p + (8))) = temp_r0_2;
        DCFlushRange((*(void **)(lbl_8040A408_p + (4))), temp_r0_2);
    }
}

extern f32 lbl_80192440[];
extern f32 lbl_80192444[];
extern f32 lbl_80192448[];

void fn_80034A9C(s32 arg0, f32 farg0)
{
    s32 temp_r0;
    s32 temp_r0_2;
    s32 temp_r6;
    s32 temp_r6_2;

    temp_r0 = *(s32 *)((s8 *)lbl_8040A3C4 + arg0 * 4);
    if (temp_r0 != -1) {
        if (farg0 < lbl_80192440[0]) {
            temp_r6 = temp_r0 * 4;
            temp_r0_2 = lbl_80229300[temp_r0];
            if ((temp_r0_2 != 0) && (temp_r0_2 != 4)) {
                lbl_80229300[temp_r0] = 4;
                lbl_80229310[temp_r0] = lbl_80192444[0];
                lbl_80229330[temp_r0] = lbl_80192444[0];
                lbl_80229320[temp_r0] = lbl_80192444[0];
                lbl_80229340[temp_r0] = 0xFF;
            }
        } else {
            temp_r6_2 = temp_r0 * 4;
            if ((s32)lbl_80229300[temp_r0] == 2) {
                lbl_80229330[temp_r0] = farg0;
                return;
            }
            lbl_80229300[temp_r0] = 1;
            lbl_80229310[temp_r0] = lbl_80192448[0];
            lbl_80229330[temp_r0] = farg0;
            lbl_80229320[temp_r0] = farg0;
            lbl_80229340[temp_r0] = 0;
        }
    }
}

void GXCopyDisp(s32, s32);
void GXSetColorUpdate(s32);
void GXSetDrawSyncCallback(s32 *);
void GXSetZMode(s32, s32, s32);
void VIFlush();
void VISetBlack(s32);
void VISetNextFrameBuffer(s32);
void VIWaitForRetrace();
#define fn_800165B4 ((void (*)(s32, s32))fn_800165B4)
#define fn_80016660 ((void (*)(s32, s32, s32 *))fn_80016660)
#define fn_800166C4 ((void (*)(s32, s32))fn_800166C4)
void fn_800369C0(s32, s32);
void fn_80038210();
void fn_80047384();
void fn_8004856C();
void fn_80048998();
extern s32 lbl_8019266C[];
extern f32 lbl_80192670[];
extern f32 lbl_80192674[];
extern s32 lbl_80407B00;
extern s32 lbl_80407F60;
extern s32 lbl_80407F64;
extern s32 lbl_80407F68;
extern s32 lbl_80407F74;
extern s32 lbl_80407F90;
extern s32 lbl_80407F98;
extern s32 lbl_80407F9C;
extern f32 lbl_804082B0;
extern s32 lbl_80408350;
extern s32 lbl_8040A3EC;
extern s32 lbl_8040ADEC;

void fn_80036D50(void)
{
    s32 var_r4;
    s32 var_r4_2;
    s32 temp_r0;
    s32 temp_r31;

    if ((s32)lbl_80407F60 == 0) {
        VISetBlack(0);
        lbl_80407F60 = -1;
    } else if ((s32)lbl_80407F60 > 0) {
        VISetBlack(1);
        lbl_80407F60 -= 1;
    }
    fn_800166C4(lbl_8040AD9C, 0);
    fn_800166C4(lbl_8040AD9C, 2);
    fn_800165B4(lbl_8040AD9C, 6);
    fn_80016660(lbl_8040AD9C, 6, lbl_8019266C);
    fn_80048998();
    fn_800166C4(lbl_8040AD9C, 6);
    VISetNextFrameBuffer(lbl_8040ADEC);
    VIFlush();
    if ((s32)lbl_80407F9C != 0) {
        if ((s32)lbl_8040A3E8 == (s32)lbl_8040A3EC) {
            VIWaitForRetrace();
        }
    } else {
        VIWaitForRetrace();
    }
    GXSetZMode(1, 7, 1);
    GXSetColorUpdate(1);
    if (((u32)(lbl_80407B00 - 4) <= 2U) || ((s32)lbl_80407B00 == 0xB)) {
        var_r4 = 0;
        if ((s32)lbl_80407F74 == 0) {
            var_r4 = 1;
            lbl_80407F74 = 1;
        }
        fn_800369C0(lbl_80407B00, var_r4);
        lbl_804082B0 = lbl_80192670[0];
        fn_80047384();
        fn_8004856C();
        temp_r31 = lbl_80407B00;
        if ((s32)lbl_80407F98 == 0) {
            while ((((u32)(lbl_80407B00 - 4) <= 2U) || (lbl_80407B00 == 0xB)) && lbl_80407B00 == temp_r31) {
                fn_80038210();
                VIWaitForRetrace();
            }
        }
    } else if (((u32)(lbl_80408350 - 4) <= 2U) || ((s32)lbl_80408350 == 0xB)) {
        var_r4_2 = 0;
        if ((s32)lbl_80407F74 == 0) {
            var_r4_2 = 1;
            lbl_80407F74 = 1;
        }
        fn_800369C0(lbl_80408350, var_r4_2);
        lbl_804082B0 = lbl_80192670[0];
        fn_80047384();
        fn_8004856C();
    } else if ((s32)lbl_80407F74 == 0) {
        if ((s32)lbl_80407F6C != 0) {
            GXCopyDisp(lbl_8040ADEC, 1);
            lbl_80407F68 = 1;
        } else {
            GXCopyDisp(lbl_8040ADEC, 0);
        }
        lbl_804082B0 = lbl_80192674[0];
    } else {
        lbl_80407F74 -= 1;
    }
    if ((s32)lbl_80407F90 != 0) {
        fn_80016660(lbl_8040AD9C, 6, lbl_8019266C);
        fn_80048998();
        fn_800166C4(lbl_8040AD9C, 0);
        if ((s32)(lbl_8040A3E8 - lbl_8040A3EC) == 1) {
            VIWaitForRetrace();
        }
    }
    fn_800165B4(lbl_8040AD9C, 0);
    fn_800165B4(lbl_8040AD9C, 3);
    fn_800165B4(lbl_8040AD9C, 4);
    fn_800165B4(lbl_8040AD9C, 5);
    fn_80016660(lbl_8040AD9C, 0, lbl_80192660);
    temp_r0 = lbl_8040A3E8 - lbl_8040A3EC;
    lbl_8040A3EC = lbl_8040A3E8;
    lbl_8040A3F0 = temp_r0;
    if ((s32)lbl_80407F64 == 0) {
        GXSetDrawSyncCallback(fn_800372F0);
        lbl_80407F64 = 1;
    }
}
#undef fn_800165B4
#undef fn_80016660
#undef fn_800166C4

void fn_800370E8(void)
{
    u16 new_var;
    u32 temp_r29;
    u32 temp_r30;
    if (((s32)lbl_8040A3D8) == 0) {

        new_var = (u16)lbl_803404D4.fbWidth;
        temp_r29 = ((u16)lbl_803404D4.efbHeight) >> 1U;
        temp_r30 = new_var >> 1U;
        GXSetTexCopySrc(0, 0, lbl_803404D4.fbWidth, lbl_803404D4.efbHeight);

        GXSetTexCopyDst(temp_r30, temp_r29, 4, 1);
        GXCopyTex(lbl_8040A3D4, 0);
        GXPixModeSync();
        lbl_8040A3D8 = 1;
    }
}

void *NuCameraGetProjectionMtx();
s32 PSMTXInverse(s32 *, s32 *);
s32 PSMTXTranspose(s32 *, s32 *);
extern const f32 lbl_80192800[];
extern s32 lbl_80340404[];
extern s32 lbl_80340434[];
extern s32 lbl_80340528[];
extern s32 lbl_80340558[];

void fn_80037A4C(void)
{
    f32 matrix[3][4];
    void *temp_r3;

    temp_r3 = NuCameraGetProjectionMtx();
    (*(f32 *)((s8 *)(lbl_80340404) + (0))) = (f32)(*(f32 *)((s8 *)(temp_r3) + (0)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0x10))) = (f32)(*(f32 *)((s8 *)(temp_r3) + (4)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0x20))) = (f32) - (*(f32 *)((s8 *)(temp_r3) + (8)));
    (*(f32 *)((s8 *)(lbl_80340404) + (4))) = (f32)(*(f32 *)((s8 *)(temp_r3) + (0x10)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0x14))) = (f32)(*(f32 *)((s8 *)(temp_r3) + (0x14)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0x24))) = (f32) - (*(f32 *)((s8 *)(temp_r3) + (0x18)));
    (*(f32 *)((s8 *)(lbl_80340404) + (8))) = (f32)(*(f32 *)((s8 *)(temp_r3) + (0x20)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0x18))) = (f32)(*(f32 *)((s8 *)(temp_r3) + (0x24)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0x28))) = (f32) - (*(f32 *)((s8 *)(temp_r3) + (0x28)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0xC))) = (f32)(*(f32 *)((s8 *)(temp_r3) + (0x30)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0x1C))) = (f32)(*(f32 *)((s8 *)(temp_r3) + (0x34)));
    (*(f32 *)((s8 *)(lbl_80340404) + (0x2C))) = (f32) - (*(f32 *)((s8 *)(temp_r3) + (0x38)));
    PSMTXInverse(lbl_80340404, lbl_80340528);
    PSMTXTranspose(lbl_80340528, lbl_80340558);
    (*(f32 *)((s8 *)(lbl_80340434) + (0))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (0)));
    (*(f32 *)((s8 *)(lbl_80340434) + (0x10))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (0x10)));
    (*(f32 *)((s8 *)(lbl_80340434) + (0x20))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (0x20)));
    (*(f32 *)((s8 *)(lbl_80340434) + (4))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (4)));
    (*(f32 *)((s8 *)(lbl_80340434) + (0x14))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (0x14)));
    (*(f32 *)((s8 *)(lbl_80340434) + (0x24))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (0x24)));
    (*(f32 *)((s8 *)(lbl_80340434) + (8))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (8)));
    (*(f32 *)((s8 *)(lbl_80340434) + (0x18))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (0x18)));
    (*(f32 *)((s8 *)(lbl_80340434) + (0x28))) = (f32)(*(f32 *)((s8 *)(lbl_80340404) + (0x28)));
    (*(f32 *)((s8 *)(lbl_80340434) + (0xC))) = (f32)lbl_80192800[0];
    (*(f32 *)((s8 *)(lbl_80340434) + (0x1C))) = (f32)lbl_80192800[0];
    (*(f32 *)((s8 *)(lbl_80340434) + (0x2C))) = (f32)lbl_80192800[0];
}

void GXBegin(s32, s32, u16);
void GXClearVtxDesc();
void GXLoadPosMtxImm(s32 *, s32);
void GXSetAlphaCompare(s32, s32, s32, s32, s32);
void GXSetBlendMode(s32, s32, s32, s32);
void GXSetChanCtrl(s32, s32, s32, s32, s32, s32, s32);
void GXSetChanMatColor(s32, struct ngc_video_color_s);
void GXSetNumTevStages(u8);
void GXSetNumTexGens(u8);
void GXSetProjection(s32 *, s32);
void GXSetTevOp(s32, s32);
void GXSetTevOrder(s32, s32, s32, s32);
void GXSetVtxAttrFmt(s32, s32, s32, s32, s32);
void GXSetVtxDesc(s32, s32);
void GXSetZCompLoc(s32);
void GXSetZMode(s32, s32, s32);
extern f32 lbl_801925BC[];
extern f32 lbl_801925C0[];
extern s32 lbl_80340464[];
extern s32 lbl_80340494[];

void fn_80035A24(void)
{
    f32 matrix[4][4];
    struct ngc_video_color_s sp48;
    f32 temp_f0;
    f32 temp_f13;
    f32 temp_f31;

    temp_f31 = lbl_801925BC[0];
    if ((lbl_80407F78 <= temp_f31) == 0) {
        GXSetProjection(lbl_80340494, 1);
        GXLoadPosMtxImm(lbl_80340464, 0);
        GXSetZMode(1, 7, 0);
        GXSetBlendMode(1, 4, 5, 0xF);
        GXSetAlphaCompare(7, 0, 0, 7, 0);
        GXSetZCompLoc(1);
        GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        GXSetTevOp(0, 4);
        GXSetNumTexGens(0);
        GXSetNumTevStages(1);
        sp48.r = sp48.g = sp48.b = 0;
        sp48.a = (s32)(lbl_80407F78 * lbl_801925C0[0]);
        GXSetChanMatColor(4, sp48);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXBegin(0x80, 0, 4);
        {
            volatile f32 *fifo = (volatile f32 *)0xCC010000;
            f32 width = (f32)(u32)lbl_803404D4.fbWidth;
            f32 height = (f32)((s32)lbl_803404D4.efbHeight + 1);
            fifo[-8192] = temp_f31;
            fifo[-8192] = temp_f31;
            temp_f13 = width;
            fifo[-8192] = temp_f31;
            fifo[-8192] = temp_f13;
            fifo[-8192] = temp_f31;
            fifo[-8192] = temp_f31;
            fifo[-8192] = temp_f13;
            temp_f0 = height;
            fifo[-8192] = temp_f0;
            fifo[-8192] = temp_f31;
            fifo[-8192] = temp_f31;
            fifo[-8192] = temp_f0;
            fifo[-8192] = temp_f31;
        }
    }
}

void GXBegin(s32, s32, u16);
void GXClearVtxDesc(void);
void GXLoadPosMtxImm(s32 *, s32);
void GXSetAlphaCompare(s32, s32, s32, s32, s32);
void GXSetAlphaUpdate(s32);
void GXSetBlendMode(s32, s32, s32, s32);
void GXSetChanCtrl(s32, s32, s32, s32, s32, s32, s32);
void GXSetChanMatColor(s32, struct ngc_video_color_s);
void GXSetColorUpdate(s32);
void GXSetNumTevStages(u8);
void GXSetNumTexGens(u8);
void GXSetProjection(s32 *, s32);
void GXSetTevOp(s32, s32);
void GXSetTevOrder(s32, s32, s32, s32);
void GXSetVtxAttrFmt(s32, s32, s32, s32, s32);
void GXSetVtxDesc(s32, s32);
void GXSetZCompLoc(s32);
void GXSetZMode(s32, s32, s32);
extern f32 lbl_8019262C[];
extern f32 lbl_80192630[];
extern f64 lbl_80192640[];

void fn_800365E8(s32 arg0, s32 arg1)
{

    f32 matrix[4][4];
    struct ngc_video_color_s sp48;
    f32 temp_f0;
    f32 temp_f13;
    f32 temp_f31;

    temp_f31 = lbl_8019262C[0];
    if ((arg1 & 0xFFFFFF) || !(arg0 & 1) || ((s32)lbl_80407F68 == 0)) {
        lbl_80407F68 = 0;
        GXSetProjection(lbl_80340494, 1);
        GXLoadPosMtxImm(lbl_80340464, 0);
        if (arg0 & 0x200) {
            GXSetZMode(1, 7, 1);
        } else {
            GXSetZMode(0, 7, 0);
        }
        if (arg0 & 0x100) {
            GXSetColorUpdate(1);
            GXSetAlphaUpdate(1);
        } else {
            GXSetColorUpdate(0);
            GXSetAlphaUpdate(0);
        }
        GXSetBlendMode(0, 1, 0, 0xF);
        GXSetAlphaCompare(7, 0, 0, 7, 0);
        GXSetZCompLoc(1);
        GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        GXSetTevOp(0, 4);
        GXSetNumTexGens(0);
        GXSetNumTevStages(1);
        sp48.a = 0x3F;
        sp48.b = (arg1 >> 16) * 2;
        sp48.g = (arg1 >> 8) * 2;
        sp48.r = arg1 * 2;
        GXSetChanMatColor(4, sp48);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXBegin(0x80, 0, 4);
        {
            volatile f32 *fifo = (volatile f32 *)0xCC010000;
            f32 width = (f32)(u32)lbl_803404D4.fbWidth;
            f32 height = (f32)((s32)lbl_803404D4.efbHeight + 1);
            fifo[-8192] = lbl_80192630[0];
            fifo[-8192] = lbl_80192630[0];
            temp_f13 = width;
            fifo[-8192] = temp_f31;
            fifo[-8192] = temp_f13;
            fifo[-8192] = lbl_80192630[0];
            fifo[-8192] = temp_f31;
            fifo[-8192] = temp_f13;
            temp_f0 = height;
            fifo[-8192] = temp_f0;
            fifo[-8192] = temp_f31;
            fifo[-8192] = lbl_80192630[0];
            fifo[-8192] = temp_f0;
            fifo[-8192] = temp_f31;
            GXSetColorUpdate(1);
            GXSetAlphaUpdate(0);
        }
    }
}

void C_MTXOrtho(s32 *, f32, f32, f32, f32, f32, f32);
void GXFlush(void);
void GXInvalidateTexAll(void);
void GXInvalidateVtxCache(void);
void GXSetDispCopyDst(u16, u16);
void GXSetDispCopySrc(u16, u16, u16, u16);
u32 GXSetDispCopyYScale(f32);
void GXSetDrawSync(u16);
void GXSetNumChans(u8);
void GXSetPixelFmt(s32, s32);
void GXSetScissor(u32, u32, u32, u32);
void GXSetViewport(f32, f32, f32, f32, f32, f32);
void GXSetViewportJitter(f32, f32, f32, f32, f32, f32, u32);
u32 VIGetNextField(void);
void fn_800350B8(s32 *, s32 *, s32 *);
extern f32 lbl_8019259C[];
extern f32 lbl_801925A0[];
extern f32 lbl_801925B0[];
extern f32 lbl_801925B4[];
extern f32 lbl_801925B8[];

extern s32 lbl_80407F8C;
extern u16 lbl_80407F8E;
extern s32 lbl_80408E68;
extern f32 lbl_8040A3D0;
extern s32 lbl_8040A3F4;
extern s32 lbl_8040A3F8;
extern s32 lbl_8040ADAC;
extern s32 lbl_8040ADF4;

void fn_800356FC(void)
{

    f32 temp_f30;
    f32 temp_f31;

    GXSetDrawSync(0xB001);
    GXFlush();
    if ((s32)lbl_8040ADF4 != 0) {
        if ((s32)lbl_8040A3F8 == 0) {
            lbl_8040A3CC = GXEurgb60Hz480IntDf;
            fn_800350B8(GXEurgb60Hz480IntDf, 0, 0);
        }
    } else if ((s32)lbl_8040A3F8 != 0) {
        lbl_8040A3CC = GXPal528IntDf;
        fn_800350B8(GXPal528IntDf, 0, 0);
    }
    lbl_8040A3F8 = lbl_8040ADF4;
    if ((s32)lbl_80408E68 != 0) {
        if ((s32)lbl_8040ADAC == 0) {
            GXSetPixelFmt(1, 0);
        }
    } else if ((s32)lbl_8040ADAC != 0) {
        GXSetPixelFmt(0, 0);
    }
    lbl_8040ADAC = lbl_80408E68;
    if ((lbl_80407F88 > 1.7999999523162842f) && ((lbl_80407F84 <= 1.7999999523162842f))) {
        lbl_80407F8C = 0x180;
    }
    if ((lbl_80407F88 < 1.5f) && ((lbl_80407F84 >= 1.5f))) {
        lbl_80407F8C = 0x1C0;
    }
    if ((s32)lbl_80407F8C != (s32)lbl_8040A3F4) {
        f32 height;
        f32 numerator = (f32)(u32)lbl_803404D4.xfbHeight;
        lbl_8040A3F4 = lbl_80407F8C;
        lbl_803404D4.efbHeight = lbl_80407F8E;
        height = (f32)(u32)lbl_803404D4.efbHeight;
        lbl_8040A3D0 = numerator / height;
        C_MTXOrtho(lbl_80340494, 0.0f, height, 0.0f, (f32)(u32)lbl_803404D4.fbWidth, 0.0f, 16777215.0f);
    }
    GXSetDispCopyYScale(lbl_8040A3D0);
    {
        s8 *lbl_803404D4_p = (s8 *)&lbl_803404D4;
        GXSetDispCopySrc(0, 0, lbl_803404D4.fbWidth, lbl_803404D4.efbHeight);
        GXSetDispCopyDst(lbl_803404D4.fbWidth, lbl_803404D4.xfbHeight);
        GXSetScissor(0, 0, lbl_803404D4.fbWidth, lbl_803404D4.efbHeight);
    }
    if ((s32)lbl_803404D4.fieldRendering != 0) {
        {
            s8 *lbl_803404D4_p = (s8 *)&lbl_803404D4;
            temp_f30 = (f32)(f64)lbl_803404D4.fbWidth;
            temp_f31 = (f32)(f64)lbl_803404D4.efbHeight;
        }
        GXSetViewportJitter(0.0f, 0.0f, temp_f30, temp_f31, 0.0f, 1.0f, VIGetNextField());
    } else {
        GXSetViewport(0.0f, 0.0f, (f32)lbl_803404D4.fbWidth, (f32)lbl_803404D4.efbHeight, 0.0f, 1.0f);
    }
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
    GXSetNumChans(1);
    lbl_8040A3D8 = 0;
}

void GXFlush();
u32 GXGetTexBufferSize(u16, u16, s32, u8, u8);
void GXSetCopyClear(struct ngc_video_color_s, u32);
void GXSetDispCopyGamma(s32);

s32 NuMemAlloc(s32, s32 *, s32);
s32 OSGetCurrentThread();
void VIFlush();
u32 VIGetTvFormat(void);
void VISetBlack(s32);
void VISetPostRetraceCallback(s32 *);
void VIWaitForRetrace();
s32 (*fn_80009948(s32 *, s32))();
void fn_800350B8(s32 *, s32 *, s32 *);
s32 memset(s32, s32, s32);
extern s32 lbl_80192584[];

extern s32 lbl_80407F70;

void fn_80035470(s32 *arg0, s32 *arg1)
{
    struct ngc_video_color_s sp8;
    s32 temp_r11;
    s32 temp_r11_2;
    s32 temp_r30;
    s32 temp_r30_2;

    s32 var_r31;
    s32 var_r31_2;
    lbl_80407F70 = OSGetCurrentThread();
    lbl_8040ADF4 = 0;
    lbl_8040A3F8 = 0;
    switch (VIGetTvFormat()) {
    case 0:
        lbl_8040A3CC = GXNtsc480IntDf;
        break;
    case 1:
        lbl_8040A3CC = GXPal528IntDf;
        break;
    case 5:
        lbl_8040ADF4 = 1;
        lbl_8040A3CC = GXEurgb60Hz480IntDf;
        lbl_8040A3F8 = 1;
        break;
    case 2:
        lbl_8040A3CC = GXMpal480IntDf;
        break;
    default:
        fn_80009948(lbl_8019253C, 0x124)(lbl_80192584);
        break;
    }
    lbl_8040ADEC = 0;
    fn_800350B8(lbl_8040A3CC, arg0, arg1);
    temp_r30 =
        GXGetTexBufferSize((u16)lbl_803404D4.fbWidth >> 1U, (u16)lbl_803404D4.efbHeight >> 1U, 4, 0, 0);
    if (arg0 != NULL) {
        var_r31 = *arg0;
        temp_r11 = temp_r30 + 0x20;
        *arg0 = var_r31 + temp_r11;
        var_r31 = (var_r31 + 0x1F) & 0xFFFFFFE0;
        *arg1 -= temp_r11;
    } else {
        var_r31 = (NuMemAlloc(temp_r30 + 0x20, lbl_8019253C, 0x69) + 0x1F) & 0xFFFFFFE0;
    }
    memset(var_r31, 0, temp_r30);
    lbl_8040A3D4 = var_r31;
    if ((s32)lbl_80407F94 != 0) {
        temp_r30_2 = GXGetTexBufferSize((u32)lbl_803404D4.fbWidth, (u32)lbl_803404D4.efbHeight, 4, 0, 0);
        if (arg0 != NULL) {
            var_r31_2 = *arg0;
            temp_r11_2 = temp_r30_2 + 0x20;
            *arg0 = var_r31_2 + temp_r11_2;
            var_r31_2 = (var_r31_2 + 0x1F) & 0xFFFFFFE0;
            *arg1 -= temp_r11_2;
        } else {
            var_r31_2 = (NuMemAlloc(temp_r30_2 + 0x20, lbl_8019253C, 0x69) + 0x1F) & 0xFFFFFFE0;
        }
        memset(var_r31_2, 0, temp_r30_2);
        lbl_8040A3DC = var_r31_2;
    }
    GXSetDispCopyGamma(0);
    sp8.r = sp8.g = sp8.b = sp8.a = 0;
    GXSetCopyClear(sp8, 0xFFFFFF);
    lbl_8040A3E8 = 0;
    VISetPostRetraceCallback(fn_800372E8);
    VISetBlack(1);
    VIFlush();
    VIWaitForRetrace();
    VIFlush();
    VIWaitForRetrace();
    if (lbl_803404D4.viTVmode & 1) {
        VIWaitForRetrace();
    }
    GXSetDrawSync(0xB000);
    GXFlush();
}

void C_MTXOrtho(s32 *, f32, f32, f32, f32, f32, f32);
void GXAdjustForOverscan(void *, void *, u16, u16);
void GXCopyDisp(s32, s32);
void GXSetCopyFilter(u8, void *, s32, void *);
void GXSetDispCopyDst(u16, u16);

u32 GXSetDispCopyYScale(f32);
void GXSetDither(s32);
void GXSetFieldMode(u8, s32);
void GXSetPixelFmt(s32, s32);

void GXSetViewport(f32, f32, f32, f32, f32, f32);
void GXSetViewportJitter(f32, f32, f32, f32, f32, f32, u32);
s32 NuMemAlloc(s32, s32 *, s32);
void PSMTXIdentity(s32 *);
void VIConfigure(s32 *);
void VIFlush();
u32 VIGetNextField(void);
void VISetNextFrameBuffer(s32);
void VIWaitForRetrace();
s32 memset(s32, s32, s32);
extern f32 lbl_80192578[];
extern f32 lbl_8019257C[];
extern f32 lbl_80192580[];

extern s32 lbl_8040ADF0;

void fn_800350B8(s32 *arg0, s32 *arg1, s32 *arg2)
{
    s32 *var_r9;
    f32 temp_f13;
    f32 temp_f30;
    f32 temp_f31;

    s32 temp_r11;
    s32 temp_r31;

    s32 *var_r30;
    void *var_r3;
    var_r3 = arg0;
    if (((s32)(*((u16 *)(((s8 *)var_r3) + 6)))) == 0x210) {
        lbl_803404D4 = *((struct ngc_video_mode_s *)arg0);
    } else {
        GXAdjustForOverscan(arg0, &lbl_803404D4, 0, 0x10);
    }

    lbl_80407F8C = 0x1C0;
    lbl_803404D4.efbHeight = *((u16 *)(((u8 *)(&lbl_80407F8C)) + 2));
    lbl_8040A3F4 = 0x1C0;
    temp_r31 = ((lbl_803404D4.fbWidth + 0xF) & 0xFFF0) * 0x420;

    if (((s32)lbl_8040ADEC) == 0) {
        if (arg1 != ((void *)0)) {
            var_r30 = (s32 *)(*arg1);
            temp_r11 = temp_r31 + 0x20;
            *arg1 = ((s32)var_r30) + temp_r11;
            var_r30 = (s32 *)((((u32)var_r30) + 0x1F) & 0xFFFFFFE0);
            *arg2 -= temp_r11;
        } else {
            var_r30 = (s32 *)((NuMemAlloc(temp_r31 | 0x20, lbl_8019253C, 0x69) + 0x1F) & 0xFFFFFFE0);
        }
        memset(var_r30, 0, temp_r31);
        lbl_8040ADEC = (s32)var_r30;
        lbl_8040ADF0 = temp_r31;
    }
    temp_f13 = ((f32)((f64)lbl_803404D4.xfbHeight)) / ((f32)((f64)lbl_803404D4.efbHeight));
    lbl_8040A3D0 = temp_f13;
    GXSetDispCopyYScale(temp_f13);
    GXSetPixelFmt(1, 0);
    lbl_80408E68 = 1;
    lbl_8040ADAC = 1;
    GXSetDither(0);
    {
        s8 *lbl_803404D4_p = (s8 *)(&lbl_803404D4);
        GXSetCopyFilter(lbl_803404D4.aa, ((s8 *)(&lbl_803404D4)) + 0x1A, lbl_803404D4.fieldRendering == 0,
                        ((s8 *)(&lbl_803404D4)) + 0x32);
        GXSetFieldMode(lbl_803404D4.fieldRendering,
                       ((u16)lbl_803404D4.xfbHeight) < ((u16)lbl_803404D4.viHeight));
    }
    VIConfigure((s32 *)(&lbl_803404D4));
    VIFlush();
    VIWaitForRetrace();
    VIWaitForRetrace();
    {
        s8 *lbl_803404D4_p = (s8 *)(&lbl_803404D4);
        GXSetDispCopySrc(0, 0, lbl_803404D4.fbWidth, lbl_803404D4.efbHeight);
        GXSetDispCopyDst(lbl_803404D4.fbWidth, lbl_803404D4.xfbHeight);
    }
    if (((s32)lbl_803404D4.fieldRendering) != 0) {
        {
            s8 *lbl_803404D4_p = (s8 *)(&lbl_803404D4);
            temp_f30 = (f32)((f64)lbl_803404D4.fbWidth);
            temp_f31 = (f32)((f64)lbl_803404D4.efbHeight);
        }
        GXSetViewportJitter(0.0f, 0.0f, temp_f30, temp_f31, 0.0f, 1.0f, VIGetNextField());
    } else {
        GXSetViewport(0.0f, 0.0f, (f32)((f64)lbl_803404D4.fbWidth), (f32)((f64)lbl_803404D4.efbHeight), 0.0f,
                      1.0f);
    }
    GXSetScissor(0, 0, lbl_803404D4.fbWidth, lbl_803404D4.efbHeight);
    VISetNextFrameBuffer(lbl_8040ADEC);
    GXCopyDisp(lbl_8040ADEC, 1);
    C_MTXOrtho(lbl_80340494, 0.0f, (f32)lbl_803404D4.efbHeight, 0.0f, (f32)lbl_803404D4.fbWidth, 0.0f,
               16777215.0f);
    PSMTXIdentity(lbl_80340464);
    lbl_8040A3F0 = 0;
}
