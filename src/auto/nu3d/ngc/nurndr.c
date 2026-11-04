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

s32 fn_800119C8(void *, void *, s32, s32, s32);
void fn_80013184(s32, f32);
extern f32 lbl_801915C8[];
extern f32 lbl_801915CC[];

s32 fn_800272A0(s32 *arg0, s32 arg1, f32 farg0) {
    s32 var_r31;

    if ((s32) *arg0 == 0) {
        return 0;
    }
    if (farg0 < lbl_801915C8[0]) {
        fn_80013184(1, farg0);
        var_r31 = fn_800119C8(arg0, arg1, 0, 0, 0);
        fn_80013184(0, lbl_801915CC[0]);
    } else {
        var_r31 = fn_800119C8(arg0, arg1, 0, 0, 0);
    }
    return var_r31;
}

void (*fn_80009948(s32 *, s32))(s32 *, ...);
extern s32 lbl_801914A0[];
extern s32 lbl_80191510[];
extern s32 lbl_8021D8A4[];
extern s32 lbl_80407D80;
extern s32 lbl_80407D84;

s32 *fn_80027D54(s32 arg0) {
    lbl_80407D84 -= arg0;
    if (lbl_80407D80 < 0) {
        fn_80009948(lbl_801914A0, 0x479)(lbl_80191510);
        return NULL;
    }
    return &lbl_8021D8A4[lbl_80407D84];
}

void *memset(void *, s32, u32);
extern u8 lbl_802198A4[];
extern s32 lbl_80407D80;
extern s32 lbl_80407D84;
extern s32 lbl_8040A33C;

s32 fn_800274BC(void) {
    lbl_8040A33C = 0;
    lbl_80407D80 = 0xF00;
    lbl_80407D84 = 0x800;
    memset(lbl_802198A4 + lbl_80407D80, 0, 0x400);
    return 1;
}

s32 NuHGobjEval(s32, s32, s32, s32 *);
void fn_80024F68(s32, s32, s32, s32, s32 *, s32, s32);
extern s32 lbl_802158A4[];

void fn_80027404(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    NuHGobjEval(arg0, 0, 0, lbl_802158A4);
    fn_80024F68(arg0, arg1, arg2, arg3, lbl_802158A4, 0, 0);
}

void *memset(void *, s32, u32);
extern u8 lbl_802198A4[];
extern s32 lbl_80407D80;
extern s32 lbl_80407D84;
extern s32 lbl_8040A33C;

s32 fn_80027510(void) {
    lbl_8040A33C = 0;
    lbl_80407D80 = 0xF00;
    lbl_80407D84 = 0x800;
    memset(lbl_802198A4 + lbl_80407D80, 0, 0x400);
    return 1;
}

struct nurndr_v3_s { f32 x, y, z; };
s32 NuCameraClipTestPoints(struct nurndr_v3_s *, s32, s32);
extern f32 lbl_80191614[];
extern s32 lbl_8040AD6C;

struct nurndr_c58_s {
    struct nurndr_v3_s pos;
    f32 f;
    s16 h;
    s16 pad;
    s32 pad2;
};
extern struct nurndr_c58_s lbl_8033EAD8[];

void fn_80027C58(struct nurndr_v3_s *arg0, s16 arg1, f32 farg0) {
    if (NuCameraClipTestPoints(arg0, 1, 0) == 0 && lbl_8040AD6C < 0x80) {
        arg0->y += lbl_80191614[0];
        lbl_8033EAD8[lbl_8040AD6C].pos = *arg0;
        lbl_8033EAD8[lbl_8040AD6C].f = farg0;
        lbl_8033EAD8[lbl_8040AD6C].h = arg1;
        lbl_8040AD6C++;
    }
}

void (*fn_80009948(s32 *, s32))(s32 *, ...);
void *memset(void *, s32, u32);
extern s32 lbl_801914A0[];
extern s32 lbl_80191510[];
extern u8 lbl_802198A4[];
extern s32 lbl_80407D80;

void *NuAnimDataChunkDestroy(s32 arg0) {
    void *p;
    s32 size = arg0 * 4 + 0x18;

    lbl_80407D80 -= size;
    if (lbl_80407D80 < 0) {
        fn_80009948(lbl_801914A0, 0x466)(lbl_80191510);
        return NULL;
    }
    p = lbl_802198A4 + lbl_80407D80;
    memset(p, 0, size);
    return p;
}

extern struct nurndr_v3_s lbl_8033EACC;
extern f32 lbl_80407CD0;
extern f32 lbl_80407CD4;
extern u32 lbl_8040AD68;

void fn_80027C0C(struct nurndr_v3_s *arg0, u32 arg1, f32 farg0, f32 farg1) {
    lbl_8033EACC = *arg0;
    lbl_8040AD68 = ((arg1 << 1) & 0xFE000000) + ((arg1 >> 1) & 0x7F7F7F);
    lbl_80407CD0 = farg0;
    lbl_80407CD4 = farg1;
}

extern f32 lbl_8019159C[];
extern struct nurndr_v3_s lbl_80313250;
extern struct nurndr_v3_s lbl_8031325C;

void fn_800265C4(struct nurndr_v3_s *arg0) {
    struct nurndr_v3_s t;
    s32 ix, iy, iz;

    t.x = (arg0->x - lbl_80313250.x) / lbl_8031325C.x;
    t.y = (arg0->y - lbl_80313250.y) / lbl_8031325C.y;
    t.z = (arg0->z - lbl_80313250.z) / lbl_8031325C.z;
    ix = (s32)(t.x + lbl_8019159C[0]) - 0x10000;
    t.x -= ix;
    iy = (s32)(t.y + lbl_8019159C[0]) - 0x10000;
    t.y -= iy;
    iz = (s32)(t.z + lbl_8019159C[0]) - 0x10000;
    t.z -= iz;
    arg0->x = t.x * lbl_8031325C.x + lbl_80313250.x;
    arg0->y = t.y * lbl_8031325C.y + lbl_80313250.y;
    arg0->z = t.z * lbl_8031325C.z + lbl_80313250.z;
}
