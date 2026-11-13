#include "types.h"

struct nurndr_vtxin_s;
s32 fn_800262BC(struct nurndr_vtxin_s **, void *, s32 *, s32);

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
    fn_800262BC((struct nurndr_vtxin_s **)&a, (void *)d, (s32 *)c, b);
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
struct nurndr_hgobj_s;
struct nurndr_hgmtx_s;
s32 fn_80024F68(struct nurndr_hgobj_s *hg, struct nurndr_hgmtx_s *mtx, s32 n, s16 *layers, struct nurndr_hgmtx_s *mtxs, s32 *arg5, s32 arg6);
extern s32 lbl_802158A4[];

void fn_80027404(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    NuHGobjEval(arg0, 0, 0, lbl_802158A4);
    fn_80024F68((struct nurndr_hgobj_s *)arg0, (struct nurndr_hgmtx_s *)arg1, arg2, (s16 *)arg3, (struct nurndr_hgmtx_s *)lbl_802158A4, 0, 0);
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

extern s32 lbl_80407DE8;
extern u8 lbl_8033F6D8[];
extern s32 lbl_8021F8A4[];
extern s32 lbl_8021F8E4[];
extern s32 lbl_8021F924[];
extern s32 lbl_8021F964[];
extern s32 lbl_80407F6C;
extern s32 lbl_80407DE4;
extern s32 fn_80010C30(void);

void fn_80027564(void)
{
    s32 value = fn_80010C30();
    s32 i = lbl_80407DE4;
    lbl_8021F924[i] = value;
    lbl_8021F8E4[i] = lbl_8040A340;
    lbl_8021F8A4[i] = lbl_8040A33C;
    lbl_8021F964[i] = lbl_80407DD8;
    lbl_80407DE4 = i + 1;
    lbl_80407DD8 = 0;
}

void fn_800275E0(void)
{
    s32 value = fn_80010C30();
    s32 i = lbl_80407DE4;
    lbl_8021F924[i] = value;
    lbl_8021F8E4[i] = lbl_8040A340;
    lbl_8021F8A4[i] = lbl_8040A33C;
    lbl_8021F964[i] = lbl_80407DD8;
    lbl_80407DE4 = i + 1;
    lbl_80407DD8 = 0;
}
extern u32 *lbl_8040ACF0;
extern u32 lbl_8040ACFC;
extern u32 lbl_8040AD00;
extern u32 lbl_8040AD88;
extern s32 lbl_80407D70;
extern s32 lbl_8019145C[];
extern s32 lbl_80191480[];
void OSInitSemaphore(void *, s32);
void OSSignalSemaphore(void *);
void OSWaitSemaphore(void *);
void (*fn_8000995C(s32 *, s32))(s32 *, ...);
void fn_80037188(void);
void fn_8001280C(void);
void fn_8002CEF4(void);
void fn_80036D50(void);
void fn_800356FC(void);
void fn_80033DF0(void);
void fn_80037B74(void);
void fn_800365E8(s32, s32);
void fn_80031524(void);
void fn_80030A54(void);
void fn_80012980(s32);
void fn_8002E4D8(void);
void fn_80012A1C(void);
void fn_80036858(void);
void fn_800128B0(void);
void fn_800340E0(void);
void fn_80033A88(void);

s32 NuRndrFootPrints(void) {
    s32 saved;
    s32 i;
    s32 last;

    if (lbl_80407DE8 == 0) {
        OSInitSemaphore(lbl_8033F6D8, 0);
        lbl_80407DE8 = 1;
    }
    OSSignalSemaphore(lbl_8033F6D8);
    fn_80037188();
    fn_8001280C();
    fn_8002CEF4();
    if (lbl_8021F8A4[0] == 0) {
        saved = lbl_80407F6C;
        lbl_80407F6C = lbl_8021F8A4[0];
    }
    fn_80036D50();
    if (lbl_8021F8A4[0] == 0) {
        lbl_80407F6C = saved;
    }
    fn_800356FC();
    i = 0;
    fn_80033DF0();
    last = 0;
    fn_80037B74();
    for (; i < lbl_80407DE4; i++) {
        if (lbl_8021F8A4[i] != 0 && lbl_8021F8A4[i] != last) {
            fn_800365E8(lbl_8021F8A4[i], lbl_8021F8E4[i]);
            last = lbl_8021F8A4[i];
            fn_80031524();
            fn_80030A54();
            fn_80037B74();
        }
        if (lbl_8021F924[i] != -1) {
            last = 0;
            fn_80012980(lbl_8021F924[i]);
        }
        if (lbl_8021F964[i] != 0) {
            fn_8002E4D8();
            lbl_8021F964[i] = 0;
        }
    }
    lbl_80407DE4 = 0;
    fn_80012A1C();
    fn_80036858();
    fn_800128B0();
    if (*lbl_8040ACF0 > lbl_8040ACFC) {
        fn_8000995C(lbl_8019145C, 0x9b)(lbl_80191480);
    }
    lbl_8040ACF0 = &lbl_8040AD00;
    lbl_8040ACFC = lbl_8040AD88;
    if (lbl_80407D74 == 0) {
        lbl_80407D70++;
    }
    fn_800340E0();
    fn_80033A88();
    OSWaitSemaphore(lbl_8033F6D8);
    return 1;
}

struct nurndr_mtx_s {
    f32 _00, _01, _02, _03;
    f32 _10, _11, _12, _13;
    f32 _20, _21, _22, _23;
    f32 _30, _31, _32, _33;
};
extern struct nurndr_v3_s lbl_801B18B8;
extern struct nurndr_v3_s lbl_801B18C4;
extern struct nurndr_v3_s lbl_801B18D0;
extern struct nurndr_mtx_s lbl_80313268;
extern struct nurndr_mtx_s lbl_801B0F08;
void NuVecInvMtxRotate(struct nurndr_v3_s *, struct nurndr_v3_s *, struct nurndr_mtx_s *);
void NuVecMtxRotate(struct nurndr_v3_s *, struct nurndr_v3_s *, struct nurndr_mtx_s *);
void fn_8000CE8C(struct nurndr_v3_s *, struct nurndr_v3_s *, struct nurndr_v3_s *, f32);
f32 NuVecNorm(struct nurndr_v3_s *, struct nurndr_v3_s *);
f32 NuVecDot(struct nurndr_v3_s *, struct nurndr_v3_s *);
f32 NuFsqrt(f32);
void fn_8002DD60(struct nurndr_mtx_s *, struct nurndr_v3_s *);

s32 NuTexReadBitmapMM(struct nurndr_v3_s *arg0, struct nurndr_v3_s *arg1) {
    struct nurndr_v3_s v;
    struct nurndr_mtx_s m;
    struct nurndr_v3_s neg;
    struct nurndr_v3_s d;
    struct nurndr_v3_s t;
    f32 dot;
    f32 len;
    f32 f;
    f32 g;

    if (arg0 != NULL) {
        lbl_801B18B8 = *arg0;
    }
    if (arg1 != NULL) {
        lbl_801B18C4.x = arg1->x;
        lbl_801B18C4.y = arg1->y;
        lbl_801B18C4.z = arg1->z;
    }
    NuVecInvMtxRotate(&v, &lbl_801B18B8, &lbl_80313268);
    fn_8000CE8C(&v, &v, &lbl_801B18D0, 0.5f);
    NuVecMtxRotate(&v, &v, &lbl_80313268);
    NuVecNorm(&v, &v);
    NuVecNorm(&d, &lbl_801B18B8);
    neg.x = -lbl_80313268._20;
    neg.y = -lbl_80313268._21;
    neg.z = -lbl_80313268._22;
    dot = NuVecDot(&neg, &d);
    t = v;
    m = lbl_801B0F08;
    len = NuFsqrt(t.x * t.x + t.z * t.z);
    if (len > 1.1754944e-38f) {
        m._22 = t.z;
        m._21 = t.y;
        m._11 = len;
        m._01 = 0.0f;
        m._20 = -t.x;
        m._02 = t.x / len;
        m._10 = (t.x * t.y) / len;
        m._00 = t.z / len;
        m._12 = (-t.y * t.z) / len;
    } else {
        m._21 = t.y;
        m._22 = 0.0f;
        m._11 = 0.0f;
        m._12 = -t.y;
    }
    m._30 = 0.5f;
    m._31 = 0.5f;
    if (dot < 0.0f) {
        f = dot * dot;
        f *= f;
        f *= f;
        f *= f;
        f *= f;
        g = 1.0f - f;
        if (m._00 * d.x + m._10 * d.y + m._20 * d.z >= 0.0f) {
            m._00 = m._00 * g + d.x * f;
            m._10 = m._10 * g + d.y * f;
            m._20 = m._20 * g + d.z * f;
        } else {
            m._00 = m._00 * g - d.x * f;
            m._10 = m._10 * g - d.y * f;
            m._20 = m._20 * g - d.z * f;
        }
        if (m._01 * d.x + m._11 * d.y + m._21 * d.z >= 0.0f) {
            m._01 = m._01 * g + d.x * f;
            m._11 = m._11 * g + d.y * f;
            m._21 = m._21 * g + d.z * f;
        } else {
            m._01 = m._01 * g - d.x * f;
            m._11 = m._11 * g - d.y * f;
            m._21 = m._21 * g - d.z * f;
        }
    }
    fn_8002DD60(&m, &lbl_801B18C4);
    return 1;
}

void DCFlushRange(void *, u32);
s32 fn_8002BF48(void *, s32);
s32 fn_8002C090(u32 *, struct nurndr_mtx_s *, void *, s32);
s32 fn_8002D770(void *, s32 *);
extern s32 lbl_803127F0[];
extern void *lbl_80407D50;
struct nurndr_qvtx_s {
    struct nurndr_v3_s pos;
    u32 colour;
    s16 u, v;
};
struct nurndr_mtl_s {
    u8 pad00[0x58];
    u8 *bits[4];
    s32 sel;
    u8 pad6c[8];
    u32 flags;
};
struct nurndr_pass_s {
    struct nurndr_mtl_s *mtl;
    s32 bit;
    u8 pad08[0x1c];
    s8 *dl;
};

void NuTexReadBitmapMMEx(void *arg0) {
    u32 *alloc;
    u8 *buf;
    u8 *desc;
    s32 n;
    s32 i;
    u32 pad;
    s32 size;
    s32 tex;
    struct nurndr_c58_s *e;
    struct nurndr_qvtx_s *out;
    struct nurndr_v3_s p;
    u8 c[4];
    u32 col;
    f32 r;
    struct nurndr_pass_s *pass;
    s32 bit;

    n = lbl_8040AD6C;
    if (n != 0) {
        if (arg0 == NULL) {
            arg0 = lbl_80407D50;
        }
        alloc = lbl_8040ACF0;
        buf = (u8 *)((((*alloc + 3) & ~3) + 0x1f) & ~0x1f);
        *alloc = (u32)(buf + 4);
        for (i = 0; i < n; i++) {
            e = &lbl_8033EAD8[i];
            p = e->pos;
            col = e->h << 24;
            c[0] = col;
            c[1] = col >> 8;
            c[2] = col >> 16;
            c[3] = col >> 24;
            r = e->f;
            out = (struct nurndr_qvtx_s *)*alloc;
            out[0].pos.x = p.x - r;
            out[0].pos.y = p.y;
            out[0].pos.z = p.z - r;
            out[0].colour = *(u32 *)c;
            out[0].u = 0;
            out[0].v = 0;
            out[1].pos.x = p.x + r;
            out[1].pos.y = p.y;
            out[1].pos.z = p.z - r;
            out[1].colour = *(u32 *)c;
            out[1].u = 0x3fff;
            out[1].v = 0;
            out[2].pos.x = p.x + r;
            out[2].pos.y = p.y;
            out[2].pos.z = p.z + r;
            out[2].colour = *(u32 *)c;
            out[2].u = 0x3fff;
            out[2].v = 0x3fff;
            out[3].pos.x = p.x - r;
            out[3].pos.y = p.y;
            out[3].pos.z = p.z + r;
            out[3].colour = *(u32 *)c;
            out[3].u = 0;
            out[3].v = 0x3fff;
            *alloc += 0x50;
        }
        buf[0] = 0;
        buf[1] = 0x80;
        *(s16 *)(buf + 2) = lbl_8040AD6C * 4;
        pad = ((*alloc + 0x1f) & ~0x1f) - *alloc;
        for (i = 0; (u32)i < pad; i++) {
            *(u8 *)(*alloc)++ = 0;
        }
        size = *alloc - (u32)buf;
        DCFlushRange(buf, size);
        tex = fn_8002C090(alloc, &lbl_801B0F08, arg0, 0);
        desc = (u8 *)((*alloc + 3) & ~3);
        *alloc = (u32)(desc + 0x50);
        desc[0] = 1;
        *(s32 *)(desc + 4) = 0xe80;
        *(u8 **)(desc + 0x24) = buf;
        *(s32 *)(desc + 0x28) = size;
        *(s32 *)(desc + 0xc) = 0;
        *(s32 *)(desc + 0x10) = 0;
        *(s32 *)(desc + 0x14) = 0;
        *(s32 *)(desc + 0x1c) = 0;
        *(s32 *)(desc + 0x18) = 0;
        *(s16 *)(desc + 8) = 0;
        *(s16 *)(desc + 0x2c) = 0;
        *(s32 *)(desc + 0x30) = 0;
        pass = *(struct nurndr_pass_s **)((u8 *)arg0 + 0xcc);
        pass->mtl->flags |= 0x40000000;
        bit = pass->bit;
        pass->mtl->bits[pass->mtl->sel][bit / 8] |= 1 << (bit % 8);
        fn_8002D770(pass, lbl_803127F0);
        fn_8002BF48(pass, 2);
        pass->dl[0] = 0x8c;
        pass->dl[1] = 3;
        *(s32 *)(pass->dl + 4) = tex;
        pass->dl += 8;
        pass->dl[0] = 0x82;
        pass->dl[1] = 3;
        *(u8 **)(pass->dl + 4) = desc;
        pass->dl += 8;
        lbl_8040AD6C = 0;
    }
}

struct nurndr_vtxin_s {
    f32 x, y, z;
    u8 pad0c[0xc];
    s32 colour;
    f32 u, v;
};
struct nurndr_vtxout_s {
    f32 x, y, z;
    u32 colour;
    s16 u, v;
};
s32 fn_800262BC(struct nurndr_vtxin_s **arg0, void *arg1, s32 *arg2, s32 arg3) {
    u32 *alloc;
    u8 *buf;
    u32 end;
    u8 *desc;
    s32 i;
    u32 pad;
    s32 tex;
    u8 c[4];
    struct nurndr_vtxout_s *out;
    struct nurndr_pass_s *pass;
    struct nurndr_mtl_s *mtl;
    s32 bit;

    if (arg3 != 0 && *(void **)((u8 *)arg1 + 0xcc) != NULL) {
        if (arg1 == NULL) {
            arg1 = lbl_80407D50;
        }
        alloc = lbl_8040ACF0;
        buf = (u8 *)((((*alloc + 3) & ~3) + 0x1f) & ~0x1f);
        *alloc = (u32)(buf + 4);
        for (i = 0; i < arg3; i++) {
            c[0] = (*arg0)[i].colour;
            c[1] = (*arg0)[i].colour >> 8;
            c[2] = (*arg0)[i].colour >> 16;
            c[3] = (*arg0)[i].colour >> 24;
            out = (struct nurndr_vtxout_s *)*alloc;
            out->x = (*arg0)[i].x;
            out->y = (*arg0)[i].y;
            out->z = (*arg0)[i].z;
            out->colour = *(u32 *)c;
            out->u = (*arg0)[i].u * 16383.0f;
            out->v = (*arg0)[i].v * 16383.0f;
            *alloc += 0x14;
        }
        buf[0] = 0;
        buf[1] = 0x98;
        *(s16 *)(buf + 2) = arg3;
        pad = ((*alloc + 0x1f) & ~0x1f) - *alloc;
        for (i = 0; (u32)i < pad; i++) {
            *(u8 *)(*alloc)++ = 0;
        }
        end = *alloc;
        DCFlushRange(buf, end - (u32)buf);
        if (arg2 != NULL) {
            tex = fn_8002C090(alloc, arg2, arg1, 0);
        } else {
            tex = fn_8002C090(alloc, (struct nurndr_mtx_s *)&lbl_801B0F08, arg1, 0);
        }
        desc = (u8 *)((*alloc + 3) & ~3);
        *alloc = (u32)(desc + 0x50);
        *(s32 *)(desc + 0xc) = 0;
        desc[0] = 1;
        *(s32 *)(desc + 4) = 0xe80;
        *(s32 *)(desc + 0x10) = 0;
        *(s32 *)(desc + 0x14) = 0;
        *(s32 *)(desc + 0x1c) = 0;
        *(s32 *)(desc + 0x18) = 0;
        *(s16 *)(desc + 8) = 0;
        *(s16 *)(desc + 0x2c) = 0;
        *(s32 *)(desc + 0x30) = 0;
        *(u8 **)(desc + 0x24) = buf;
        *(s32 *)(desc + 0x28) = (u8 *)end - buf;
        pass = *(struct nurndr_pass_s **)((u8 *)arg1 + 0xcc);
        pass->mtl->flags |= 0x40000000;
        bit = pass->bit;
        pass->mtl->bits[pass->mtl->sel][bit / 8] |= 1 << (bit % 8);
        fn_8002D770(pass, lbl_803127F0);
        fn_8002BF48(pass, 2);
        pass->dl[0] = 0x8c;
        pass->dl[1] = 3;
        *(s32 *)(pass->dl + 4) = tex;
        pass->dl += 8;
        pass->dl[0] = 0x82;
        pass->dl[1] = 3;
        *(u8 **)(pass->dl + 4) = desc;
        pass->dl += 8;
        return 1;
    }
}

s32 fn_80025FA0(struct nurndr_vtxin_s *arg0, void *arg1, struct nurndr_mtx_s *arg2)
{
    s32 *alloc;
    s32 buf;
    s32 p;
    s32 tex;
    s32 desc;
    struct nurndr_pass_s *pass;
    s32 bit;

    if (arg1 == NULL) {
        arg1 = lbl_80407D50;
    }
    if (arg2 == NULL) {
        arg2 = &lbl_801B0F08;
    }
    alloc = (s32 *)lbl_8040ACF0;
    buf = ((((*alloc) + 3) & ~3) + 0x1f) & ~0x1f;
    *alloc = buf;
    p = buf + 1;
    *(u8 *)buf = 0;
    *alloc = p;
    *(u8 *)(buf + 1) = 0xa8;
    *alloc = p + 1;
    *(s16 *)(p + 1) = 2;
    *alloc = buf + 4;
    *(f32 *)(p + 3) = arg0[0].x;
    *alloc = buf + 8;
    *(f32 *)(p + 7) = arg0[0].y;
    *alloc = buf + 0xc;
    *(f32 *)(p + 0xb) = arg0[0].z;
    *alloc = buf + 0x10;
    *(u8 *)(p + 0xf) = arg0[0].colour;
    *alloc = buf + 0x11;
    *(u8 *)(p + 0x10) = arg0[0].colour >> 8;
    *alloc = buf + 0x12;
    *(u8 *)(p + 0x11) = arg0[0].colour >> 16;
    *alloc = buf + 0x13;
    *(u8 *)(p + 0x12) = arg0[0].colour >> 24;
    *alloc = buf + 0x14;
    *(f32 *)(p + 0x13) = arg0[1].x;
    *alloc = buf + 0x18;
    *(f32 *)(p + 0x17) = arg0[1].y;
    *alloc = buf + 0x1c;
    *(f32 *)(p + 0x1b) = arg0[1].z;
    *alloc = buf + 0x20;
    *(u8 *)(p + 0x1f) = arg0[1].colour;
    *alloc = buf + 0x21;
    *(u8 *)(p + 0x20) = arg0[1].colour >> 8;
    *alloc = buf + 0x22;
    *(u8 *)(p + 0x21) = arg0[1].colour >> 16;
    *alloc = buf + 0x23;
    *(u8 *)(p + 0x22) = arg0[1].colour >> 24;
    *alloc = buf + 0x24;
    *(s32 *)(p + 0x23) = 0;
    *alloc = buf + 0x28;
    *(s32 *)(p + 0x27) = 0;
    *alloc = buf + 0x2c;
    *(s32 *)(p + 0x2b) = 0;
    *alloc = buf + 0x30;
    *(s32 *)(p + 0x2f) = 0;
    *alloc = buf + 0x34;
    *(s32 *)(p + 0x33) = 0;
    *alloc = buf + 0x38;
    *(s32 *)(p + 0x37) = 0;
    *alloc = buf + 0x3c;
    *(s32 *)(p + 0x3b) = 0;
    *alloc = buf + 0x40;
    DCFlushRange((void *)buf, 0x40);
    tex = fn_8002C090((u32 *)alloc, arg2, arg1, 0);
    desc = ((*alloc) + 3) & ~3;
    *alloc = desc + 0x50;
    *(u8 *)desc = 1;
    *(s32 *)(desc + 0xc) = 0;
    *(s32 *)(desc + 0x10) = 0;
    *(s32 *)(desc + 0x14) = 0;
    *(s32 *)(desc + 0x1c) = 0;
    *(s32 *)(desc + 0x18) = 0;
    *(s16 *)(desc + 8) = 0;
    *(s16 *)(desc + 0x2c) = 0;
    *(s32 *)(desc + 4) = 0xa00;
    *(s32 *)(desc + 0x30) = 0;
    *(s32 *)(desc + 0x28) = 0x40;
    *(s32 *)(desc + 0x24) = buf;
    pass = *(struct nurndr_pass_s **)((u8 *)arg1 + 0xcc);
    pass->mtl->flags |= 0x40000000;
    bit = pass->bit;
    pass->mtl->bits[pass->mtl->sel][bit / 8] |= 1 << (bit % 8);
    fn_8002D770(pass, lbl_803127F0);
    fn_8002BF48(pass, 2);
    pass->dl[0] = 0x8c;
    pass->dl[1] = 3;
    *(s32 *)(pass->dl + 4) = tex;
    pass->dl += 8;
    pass->dl[0] = 0x82;
    pass->dl[1] = 3;
    *(s32 *)(pass->dl + 4) = desc;
    pass->dl += 8;
    return 1;
}

struct nurndr_hgmtx_s {
    f32 m[16];
};
struct nurndr_hglayer_s {
    u8 pad00[4];
    void **specials04;
    void *gobj08;
    void **specials0C;
    void *gobj10;
};
struct nurndr_hgjoint_s {
    s32 unknown00;
    s32 unknown04;
    s32 unknown08;
    u8 unknown0C;
    u8 unknown0D;
    u8 unknown0E;
    u8 joint;
};
struct nurndr_hgobj_s {
    u8 pad00[0x1c];
    void *joints1C;
    u8 pad20[4];
    struct nurndr_hglayer_s *layers;
    u8 pad28[0x14];
    struct nurndr_hgjoint_s *points;
    u8 pad40[8];
    u8 bounds48[0xc];
    u8 bounds54[0x28];
    u8 numjoints;
    u8 pad7D;
    u8 numlayers;
    u8 pad7F[2];
    u8 nopoints;
    u8 pad82[0x12];
    s32 numpoints;
};
struct nurndr_hgpoint_s {
    struct nurndr_hgmtx_s mtx;
    s32 unknown40;
    s32 unknown44;
    s32 unknown48;
    struct nurndr_v3_s colour;
    s32 alpha;
    f32 unknown5C;
    f32 unknown60;
    u8 unknown64;
    u8 unknown65;
    u8 unknown66;
    u8 unknown67;
};
extern s32 lbl_80407DD4;
extern f32 lbl_8019150C[];
extern s32 lbl_80407CD8;
extern s32 lbl_80407CDC;
extern u8 lbl_80407CDF;
extern s32 lbl_80407CE0;
extern struct nurndr_hgpoint_s lbl_803378CC[];
s32 fn_80023628(void *, void *, struct nurndr_hgmtx_s *, s32, f32);
void fn_8000EB44(struct nurndr_hgmtx_s *, struct nurndr_hgmtx_s *, struct nurndr_hgmtx_s *);
void fn_8000EB64(struct nurndr_hgmtx_s *, void *, struct nurndr_hgmtx_s *, s32);
s32 NuSpecialFindMultiWC(void *, struct nurndr_hgmtx_s *);
s32 fn_80013124(void *, struct nurndr_hgmtx_s *, struct nurndr_hgmtx_s *, s32);
s32 fn_8001AF7C(void);
s32 fn_8001AF84(void);
void fn_80030A70(void);

/* NONMATCHING 222/270: point-copy anchor/register allocation; see work/fn_80024F68/status.md. */
s32 fn_80024F68(struct nurndr_hgobj_s *hg, struct nurndr_hgmtx_s *mtx, s32 n, s16 *layers, struct nurndr_hgmtx_s *mtxs, s32 *arg5, s32 arg6)
{
  s32 ret;
  s32 i;
  s32 j;
  s32 l;
  s32 a;
  struct nurndr_hgjoint_s *pt;
  struct nurndr_hgpoint_s *out;
  struct nurndr_hgmtx_s *buf;
  u8 new_var;
  u32 *alloc;
  struct nurndr_hgmtx_s tmp;
  ret = 0;
  if (lbl_80407DD4 == 0)
  {
    return 1;
  }
  if (fn_80023628(hg->bounds48, hg->bounds54, mtx, 0, lbl_8019150C[0]) == 0)
  {
    return 0;
  }
  if ((hg->points != ((void *) 0)) && (hg->nopoints == 0))
  {
    pt = hg->points;
    for (i = 0; i < hg->numpoints; i++, pt++)
    {
      if ((lbl_80407CE0 <= 0x117) && (lbl_80407CD8 != 0))
      {
        fn_8000EB44(&lbl_803378CC[lbl_80407CE0].mtx, &mtxs[pt->joint], mtx);
        lbl_803378CC[lbl_80407CE0].unknown5C = lbl_80407CD0;
        lbl_803378CC[lbl_80407CE0].unknown60 = lbl_80407CD4;
        lbl_803378CC[lbl_80407CE0].colour = lbl_8033EACC;
        lbl_803378CC[lbl_80407CE0].alpha = ((s32) lbl_8040AD68) >> 24;
        lbl_803378CC[lbl_80407CE0].unknown40 = pt->unknown00;
        lbl_803378CC[lbl_80407CE0].unknown44 = pt->unknown04;
        new_var = pt->unknown0E;
        if (new_var != 0)
        {
          lbl_803378CC[lbl_80407CE0].unknown48 = pt->unknown08;
        }
        else
        {
          lbl_803378CC[lbl_80407CE0].unknown48 = 0;
        }
        lbl_803378CC[lbl_80407CE0].unknown64 = pt->unknown0C;
        lbl_803378CC[lbl_80407CE0].unknown65 = pt->unknown0D;
        lbl_803378CC[lbl_80407CE0].unknown67 = lbl_80407CDF;
        lbl_803378CC[lbl_80407CE0].unknown66 = pt->unknown0E;
        lbl_80407CE0++;
        lbl_80407CDC++;
      }
    }

  }
  if ((fn_8001AF7C() == 0) || (fn_8001AF84() == 0))
  {
    fn_80030A70();
  }
  for (i = 0; i < n; i++)
  {
    if (layers != ((void *) 0))
    {
      l = layers[i];
      if ((layers[i] < 0) || (l >= hg->numlayers))
      {
        continue;
      }
    }
    else
    {
      n = 1;
      l = 0;
    }
    a = (arg5 != ((void *) 0)) ? (arg5[i]) : (0);
    if ((l < 0) || (l > hg->numlayers))
    {
      continue;
    }
    if (hg->layers[l].specials04 != ((void *) 0))
    {
      for (j = 0; j < hg->numjoints; j++)
      {
        if (hg->layers[l].specials04[j] != ((void *) 0))
        {
          fn_8000EB44(&tmp, &mtxs[j], mtx);
          ret |= NuSpecialFindMultiWC(hg->layers[l].specials04[j], &tmp);
        }
      }

    }
    if (hg->layers[l].specials0C != ((void *) 0))
    {
      for (j = 0; j < hg->numjoints; j++)
      {
        if (hg->layers[l].specials0C[j] != ((void *) 0))
        {
          fn_8000EB44(&tmp, &mtxs[j], mtx);
          ret |= NuSpecialFindMultiWC(hg->layers[l].specials0C[j], &tmp);
        }
      }

    }
    if ((hg->layers[l].gobj08 != ((void *) 0)) || (hg->layers[l].gobj10 != ((void *) 0)))
    {
      alloc = lbl_8040ACF0;
      buf = (struct nurndr_hgmtx_s *) (((((*alloc) + 3) & (~3)) + 0xf) & (~0xf));
      *alloc = (u32) buf;
      *alloc = (u32) (buf + hg->numjoints);
      fn_8000EB64(buf, hg->joints1C, mtxs, hg->numjoints);
      if (hg->layers[l].gobj08 != ((void *) 0))
      {
        ret |= fn_80013124(hg->layers[l].gobj08, buf, mtx, a);
      }
      if (hg->layers[l].gobj10 != ((void *) 0))
      {
        ret |= fn_80013124(hg->layers[l].gobj10, buf, mtx, a);
      }
    }
  }

  return ret;
}

extern s32 lbl_8021F9A4[];
void NuAnimCurveEval(void *);
void fn_80026710(u32 *alloc, void *arg1, s32 count, void *node, s32 arg3, s32 *total, s32 arg4, void *mtl, f32 f1, f32 f2, f32 f3, f32 f4);

struct nurndr_node_s {
    u16 count;
    u8 pad02;
    u8 type;
    struct nurndr_node_s *next;
    struct nurndr_node_s *cur;
    u32 j;
};

/* NONMATCHING 206/229: node/cur register swap and the hoisted count load; see work/fn_80026F0C/status.md. */
void fn_80026F0C(struct nurndr_node_s *node, void *arg1, void *mtl, s32 arg3, s32 arg4, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5) {
    u32 *alloc;
    u8 *buf;
    u8 *desc;
    s32 done;
    s32 i;
    u32 pad;
    s32 size;
    s32 tex;
    s32 total;
    struct nurndr_node_s *next;
    struct nurndr_node_s *cur;
    u32 j;
    struct nurndr_pass_s *pass;
    s32 bit;

    if (mtl != NULL) {
        if (*((u8 *)mtl + 0xbf) != 2 || *((u8 *)mtl + 0xc0) != 1) {
            *((u8 *)mtl + 0xbf) = 2;
            *((u8 *)mtl + 0xc0) = 1;
            fn_80024CD4(mtl);
        }
        done = 0;
        cur = node;
        NuAnimCurveEval(lbl_8021F9A4);
        alloc = lbl_8040ACF0;
        i = 0;
        buf = (u8 *)((((*alloc + 3) & ~3) + 0x1f) & ~0x1f);
        *alloc = (u32)(buf + 4);
        *(f32 *)((u8 *)arg1 + 0x1c) = f1;
        total = 0;
        do {
            next = node->next;
            switch (node->type >> 4) {
            case 2:
                if (next != NULL) {
                    fn_80026710(alloc, arg1, node->count, cur, arg3, &total, arg4, mtl, f1, f5, f2, f3);
                    cur = next;
                    node = cur;
                }
                break;
            case 6:
                fn_80026710(alloc, arg1, 0x20, cur, arg3, &total, arg4, mtl, f1, f5, f2, f3);
                done = 1;
                break;
            case 0:
                if (next != NULL) {
                    fn_80026710(alloc, arg1, 0x20, next, arg3, &total, arg4, mtl, f1, f5, f2, f3);
                    done = 1;
                }
                break;
            case 1:
                done = 1;
                break;
            case 3:
            case 4:
            case 5:
                break;
            case 7:
            default:
                done = 1;
                break;
            }
            i++;
            if (i > 0x100) {
                break;
            }
        } while (done == 0);
        if (total > 0) {
            buf[0] = 0;
            buf[1] = 0x80;
            *(s16 *)(buf + 2) = total;
            pad = ((*alloc + 0x1f) & ~0x1f) - *alloc;
            for (j = 0; j < pad; j++) {
                *(u8 *)(*alloc)++ = 0;
            }
            size = *alloc - (u32)buf;
            DCFlushRange(buf, size);
            tex = fn_8002C090(alloc, (struct nurndr_mtx_s *)&lbl_801B0F08, mtl, 0);
            desc = (u8 *)((*alloc + 3) & ~3);
            *alloc = (u32)(desc + 0x50);
            desc[0] = 1;
            *(s32 *)(desc + 4) = 0xe80;
            *(s32 *)(desc + 0x30) = 0;
            *(s32 *)(desc + 0x28) = size;
            *(s32 *)(desc + 0xc) = 0;
            *(s32 *)(desc + 0x10) = 0;
            *(s32 *)(desc + 0x14) = 0;
            *(s32 *)(desc + 0x1c) = 0;
            *(s32 *)(desc + 0x18) = 0;
            *(s16 *)(desc + 8) = 0;
            pass = *(struct nurndr_pass_s **)((u8 *)mtl + 0xcc);
            *(s16 *)(desc + 0x2c) = 0;
            *(u8 **)(desc + 0x24) = buf;
            if (*((u8 *)mtl + 0x111) == 0x97) {
                buf[1] = 0x90;
                *(s32 *)(desc + 4) |= 0x40000;
            }
            pass->mtl->flags |= 0x40000000;
            bit = pass->bit;
            pass->mtl->bits[pass->mtl->sel][bit / 8] |= 1 << (bit % 8);
            fn_8002D770(pass, lbl_803127F0);
            fn_8002BF48(pass, 2);
            pass->dl[0] = 0x8c;
            pass->dl[1] = 3;
            *(s32 *)(pass->dl + 4) = tex;
            pass->dl += 8;
            pass->dl[0] = 0x82;
            pass->dl[1] = 3;
            *(u8 **)(pass->dl + 4) = desc;
            pass->dl += 8;
        }
    }
}
