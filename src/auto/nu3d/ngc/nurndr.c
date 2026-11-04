#include "types.h"

extern void fn_800262BC(s32* a, s32 b, s32 c, s32 d);

extern s32 fn_80037228(void);

extern s32 fn_80033F30(void);

extern s32 fn_80037218(void);

extern void AXFXSetHooks(s32 a, s32 b);

struct nurndr_ent_s {
    u8 pad[0x32];
    u8 flag;
    u8 pad33;
};

extern s32 lbl_8040AD80;
extern struct nurndr_ent_s lbl_8033F6E4[];

extern s32 lbl_80407DD8;

extern s32 lbl_8040A33C;
extern s32 lbl_8040A340;

extern s32 lbl_80407D74;

extern s32 lbl_80407D7C;

void fn_80024F0C(void)
{
}

void fn_80024F40(void)
{
}

void fn_80027690(void)
{
}

void fn_8002782C(void)
{
}

void fn_80027830(void)
{
}

void fn_80027BE0(void)
{
}

void fn_80027BF4(void)
{
}

void fn_80027C54(void)
{
}

s32 fn_80024F10(void)
{
    return 0;
}

s32 fn_80027688(void)
{
    return 0;
}

s32 fn_80027824(void)
{
    return 1;
}

s32 fn_80027BE4(void)
{
    return 0;
}

s32 fn_80027BEC(void)
{
    return 1;
}

void fn_80027BF8(s32 a)
{
    lbl_80407D7C = a;
}

void fn_80027D2C(s32 a)
{
    lbl_80407D74 = a;
}

void AXFXSetHooks(s32 a, s32 b)
{
    lbl_8040A33C = a;
    lbl_8040A340 = b;
}

void fn_80027C00(void)
{
    lbl_80407DD8 = 1;
}

void fn_80027D00(void)
{
    s32 i;
    lbl_8040AD80 = 0;
    for (i = 63; i >= 0; i--) {
        lbl_8033F6E4[i].flag = 0;
    }
}

void fn_80027668(s32 a, s32 b)
{
    AXFXSetHooks(a, b);
}

s32 fn_80027D34(void)
{
    return fn_80037218();
}

s32 fn_80024F44(void)
{
    return fn_80033F30() + 1;
}

s32 fn_80027348(void)
{
    if (fn_80037228() == 0x32) { return 0; }
    return 1;
}

void NuRndrTriStrip3dClip(s32 a, s32 b, s32 c, s32 d)
{
    fn_800262BC(&a, d, c, b);
}

s32 fn_80028138(s32, s32 *);
s32 fn_8002CFF8(s32, s32);
s32 fn_8002E538();

void fn_8002746C(s32 arg0, s32 arg1, s32 *arg2)
{
  s32 sp8;
  sp8 = *arg2;
  fn_80028138(arg1, &sp8);
 do { fn_8002CFF8(arg0, arg1); } while (0);
  fn_8002E538();
}

s32 fn_80033FA8();

void NuFileWrite(s32 arg0) {
    fn_80033FA8(arg0 - 1);
}

void fn_80024CD4(void *);
s32 fn_800374C8(s32, s32, void *);
void fn_80037788(u32);
s32 fn_800377AC(f32, f32, f32);
s32 fn_80037864();
extern f32 lbl_801915D8[];
extern f32 lbl_801915DC[];
extern void *lbl_80407D4C;

void fn_80027694(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, void *arg5, s32 arg6) {
    f32 temp_f28;
    f32 temp_f29;
    f32 temp_f30;
    f32 temp_f31;
    void *var_r31;

    temp_f30 = (f32) (f64) arg0 * lbl_801915D8[0];
    var_r31 = arg5;
    temp_f31 = (f32) (f64) arg1 * lbl_801915D8[0];
    temp_f29 = (f32) (f64) arg2 * lbl_801915D8[0];
    temp_f28 = (f32) (f64) arg3 * lbl_801915D8[0];
    if (var_r31 == NULL) {
        var_r31 = lbl_80407D4C;
    }
    if ((s32) (*(u8 *)((s8 *)(var_r31) + (0xC0))) != 1) {
        (*(u8 *)((s8 *)(var_r31) + (0xC0))) = 1U;
        fn_80024CD4(var_r31);
    }
    fn_800374C8(3, 5, var_r31);
    fn_80037788(arg4);
    fn_800377AC(temp_f30, temp_f31, lbl_801915DC[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f29, temp_f31, lbl_801915DC[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f29, temp_f28, lbl_801915DC[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f30, temp_f28, lbl_801915DC[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f30, temp_f31, lbl_801915DC[0]);
    fn_80037864();
}

void fn_80024CD4(void *);
s32 fn_800374C8(s32, s32, void *);
void fn_80037788(u32);
s32 fn_800377AC(f32, f32, f32);
s32 fn_80037864();
extern f32 lbl_80191600[];
extern f32 lbl_80191604[];
extern void *lbl_80407D4C;

void fn_80027A48(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, void *arg5, s32 arg6) {
    f32 temp_f28;
    f32 temp_f29;
    f32 temp_f30;
    f32 temp_f30_2;
    f32 temp_f31;
    f32 temp_f31_2;
    void *var_r31;

    temp_f28 = (f32) (f64) arg0 * lbl_80191600[0];
    var_r31 = arg5;
    temp_f29 = (f32) (f64) arg1 * lbl_80191600[0];
    temp_f31 = (f32) (f64) arg2 * lbl_80191600[0];
    temp_f30 = (f32) (f64) arg3 * lbl_80191600[0];
    if (var_r31 == NULL) {
        var_r31 = lbl_80407D4C;
    }
    if ((s32) (*(u8 *)((s8 *)(var_r31) + (0xC0))) != 1) {
        (*(u8 *)((s8 *)(var_r31) + (0xC0))) = 1U;
        fn_80024CD4(var_r31);
    }
    fn_800374C8(3, 5, var_r31);
    temp_f31_2 = temp_f28 + temp_f31;
    temp_f30_2 = temp_f29 + temp_f30;
    fn_80037788(arg4);
    fn_800377AC(temp_f28, temp_f29, lbl_80191604[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f31_2, temp_f29, lbl_80191604[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f31_2, temp_f30_2, lbl_80191604[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f28, temp_f30_2, lbl_80191604[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f28, temp_f29, lbl_80191604[0]);
    fn_80037864();
}

s32 fn_8003777C(f32, f32);
extern f32 lbl_80191558[];
extern f32 lbl_8019155C[];
extern f32 lbl_80191560[];

void fn_80025C38(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u32 arg4, void *arg5, s32 arg6) {
    f32 temp_f28;
    f32 temp_f29;
    f32 temp_f30;
    f32 temp_f31;
    void *var_r29;

    temp_f28 = (f32) (f64) arg0 * lbl_80191558[0];
    var_r29 = arg5;
    temp_f29 = (f32) (f64) arg1 * lbl_80191558[0];
    temp_f31 = (f32) (f64) arg2 * lbl_80191558[0];
    temp_f30 = (f32) (f64) arg3 * lbl_80191558[0];
    if (var_r29 == NULL) {
        var_r29 = lbl_80407D4C;
    }
    if ((s32) (*(u8 *)((s8 *)(var_r29) + (0xC0))) != 1) {
        (*(u8 *)((s8 *)(var_r29) + (0xC0))) = 1U;
        fn_80024CD4(var_r29);
    }
    fn_800374C8(4, 7, var_r29);
    fn_8003777C(lbl_8019155C[0], lbl_8019155C[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f28, temp_f29, lbl_8019155C[0]);
    fn_8003777C(lbl_80191560[0], lbl_8019155C[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f28 + temp_f31, temp_f29, lbl_8019155C[0]);
    fn_8003777C(lbl_80191560[0], lbl_80191560[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f28 + temp_f31, temp_f29 + temp_f30, lbl_8019155C[0]);
    fn_8003777C(lbl_8019155C[0], lbl_80191560[0]);
    fn_80037788(arg4);
    fn_800377AC(temp_f28, temp_f29 + temp_f30, lbl_8019155C[0]);
    fn_80037864();
}

s32 fn_8003777C(f32, f32);
extern f32 lbl_80191570[];
extern f32 lbl_80191574[];
extern f32 lbl_80191578[];

void fn_80025DEC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u32 *arg4, void *arg5, s32 arg6) {
    f32 temp_f28;
    f32 temp_f29;
    f32 temp_f30;
    f32 temp_f31;
    void *var_r29;

    temp_f28 = (f32) (f64) arg0 * lbl_80191570[0];
    var_r29 = arg5;
    temp_f29 = (f32) (f64) arg1 * lbl_80191570[0];
    temp_f31 = (f32) (f64) arg2 * lbl_80191570[0];
    temp_f30 = (f32) (f64) arg3 * lbl_80191570[0];
    if (var_r29 == NULL) {
        var_r29 = lbl_80407D4C;
    }
    if ((s32) (*(u8 *)((s8 *)(var_r29) + (0xC0))) != 1) {
        (*(u8 *)((s8 *)(var_r29) + (0xC0))) = 1U;
        fn_80024CD4(var_r29);
    }
    fn_800374C8(4, 7, var_r29);
    fn_8003777C(lbl_80191574[0], lbl_80191574[0]);
    fn_80037788(arg4[0]);
    fn_800377AC(temp_f28, temp_f29, lbl_80191574[0]);
    fn_8003777C(lbl_80191578[0], lbl_80191574[0]);
    fn_80037788(arg4[1]);
    fn_800377AC(temp_f28 + temp_f31, temp_f29, lbl_80191574[0]);
    fn_8003777C(lbl_80191578[0], lbl_80191578[0]);
    fn_80037788(arg4[3]);
    fn_800377AC(temp_f28 + temp_f31, temp_f29 + temp_f30, lbl_80191574[0]);
    fn_8003777C(lbl_80191574[0], lbl_80191578[0]);
    fn_80037788(arg4[2]);
    fn_800377AC(temp_f28, temp_f29 + temp_f30, lbl_80191574[0]);
    fn_80037864();
}

s32 fn_8003777C(f32, f32);
extern f32 lbl_801915F0[];
extern f32 lbl_801915F4[];

s32 fn_8003777C(f32, f32);
extern f32 lbl_801915F0[];
extern f32 lbl_801915F4[];

void fn_80027868(s32 arg0, s32 arg1, s32 arg2, s32 arg3, f32 farg0, f32 farg1, f32 farg2, f32 farg3, u32 arg4, void *arg5, s32 arg6) {
    f32 temp_f28;
    f32 temp_f29;
    f32 temp_f30;
    f32 temp_f31;
    void *var_r31;

    temp_f28 = (f32) (f64) arg0 * lbl_801915F0[0];
    temp_f29 = (f32) (f64) arg1 * lbl_801915F0[0];
    var_r31 = arg5;
    temp_f31 = (f32) (f64) arg2 * lbl_801915F0[0];
    temp_f30 = (f32) (f64) arg3 * lbl_801915F0[0];
    if (var_r31 == NULL) {
        var_r31 = lbl_80407D4C;
    }
    if ((s32) (*(u8 *)((s8 *)(var_r31) + (0xC0))) != 1) {
        (*(u8 *)((s8 *)(var_r31) + (0xC0))) = 1U;
        fn_80024CD4(var_r31);
    }
    fn_800374C8(4, 7, var_r31);
    fn_8003777C(farg0, farg1);
    fn_80037788(arg4);
    fn_800377AC(temp_f28, temp_f29, lbl_801915F4[0]);
    fn_8003777C(farg2, farg1);
    fn_80037788(arg4);
    fn_800377AC(temp_f28 + temp_f31, temp_f29, lbl_801915F4[0]);
    fn_8003777C(farg2, farg3);
    fn_80037788(arg4);
    fn_800377AC(temp_f28 + temp_f31, temp_f29 + temp_f30, lbl_801915F4[0]);
    fn_8003777C(farg0, farg3);
    fn_80037788(arg4);
    fn_800377AC(temp_f28, temp_f29 + temp_f30, lbl_801915F4[0]);
    fn_80037864();
}
