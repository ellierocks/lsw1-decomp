#include "types.h"
#include "numath/numath.h"

extern void* memset();

extern u32 OSGetTick(void);

struct ngc_mtl_state_s {
    u8 pad0[8];
    s32 f08;
    u8 pad0c[8];
    s32 f14;
    u8 pad18[6];
    u16 cnt1e;
    s32 n20;
    u16 cnt24;
    u16 cnt26;
    u16 cnt28;
    u8 pad2a[2];
    s32 f2c;
    f32 f30;
    f32 f34;
    f32 f38;
    struct nuvec_s v3c;
    struct nuvec_s v48;
    struct nuvec_s v54;
    struct nuvec_s v60;
    struct nuvec_s v6c;
    struct nuvec_s v78;
    f32 f84;
    struct numtx_s m88;
    struct nuvec_s vc8;
    s32 fd4;
    f32 fd8;
    s32 fdc;
    s32 fe0;
    u8 pade4[0xc];
    s32 ff0;
    struct numtx_s projection;
    f32 f134;
    f32 f138;
    f32 f13c;
    f32 f140;
    f32 f144;
    s32 f148;
    s32 f14c;
    f32 f150;
    f32 f154;
    s32 f158;
    s32 f15c;
};

extern struct ngc_mtl_state_s lbl_803127F0;

extern s32 lbl_8021FBA8[];

extern s32 lbl_8040A378;
extern s32 lbl_8040A388;
extern s32 lbl_8040A37C;
extern s32 lbl_8040A380;
extern s32 lbl_8040A384;

extern f32 lbl_80407EF0;

extern f32 lbl_801921AC[];
extern f32 lbl_80407EF4;

extern s32 lbl_8040A398;
extern s32 lbl_8040A390;

extern s32* lbl_8040A38C;

extern f32 lbl_80192054[];

extern s32 lbl_8040A3B0;

extern s32 lbl_8040ADB4;

void fn_8002DBA4(void)
{
}

void fn_8002DBE0(void)
{
}

void fn_8002DE14(void)
{
}

void fn_8002E538(void)
{
}

void fn_80033E94(s32 a)
{
    lbl_8040ADB4 = a;
}

void fn_80033E9C(s32 a)
{
    lbl_8040A3B0 = a;
}

f32 fn_8002DE38(void)
{
    return lbl_80192054[0];
}

void fn_80030A64(s32* a)
{
    lbl_8040A38C = a + 1;
}

void fn_80030A54(void)
{
    s32 z = 0;
    lbl_8040A390 = z;
    lbl_8040A398 = z;
}

void fn_80030A0C(f32 a)
{
    lbl_80407EF4 = a * lbl_801921AC[0];
}

void fn_80030A20(void)
{
    lbl_80407EF0 = lbl_80407EF0 + lbl_80407EF4;
}

void fn_80030A34(void)
{
    s32 z = 0;
    lbl_8040A378 = -1;
    lbl_8040A37C = z;
    lbl_8040A380 = z;
    lbl_8040A384 = z;
    lbl_8040A388 = z;
}

void fn_80030A70(void)
{
    s32 i;
    for (i = 7; i >= 0; i--) {
        lbl_8021FBA8[i] = 0;
    }
}

void fn_8002DCF8(s32 a)
{
    lbl_803127F0.f14c = a;
    lbl_803127F0.f148 = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt28++;
}

void fn_8002DBA8(s32 a, s32 b, s32 c, s32 e, f32 d)
{
    lbl_803127F0.fdc = e;
    lbl_803127F0.fd4 = a;
    lbl_803127F0.fd8 = d;
    lbl_803127F0.f15c = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt1e++;
}

void fn_8002DD28(s32 a, f32 b, f32 c)
{
    lbl_803127F0.f150 = b;
    lbl_803127F0.f154 = c;
    lbl_803127F0.f158 = a;
    lbl_803127F0.f148 = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt28++;
}

u32 fn_8002DE18(void)
{
    return OSGetTick();
}

void fn_8002DB70(void)
{
    memset(&lbl_803127F0, 0, 0x160);
}

s32 fn_80030E3C();
s32 fn_80034BB8(s32, s32);
extern s32 lbl_80407EFC;
extern s32 lbl_80407F00;
extern s32 lbl_80407F04;
extern s32 lbl_80407F08;
extern s32 lbl_80407F18;
extern s32 lbl_80407F1C;

void fn_80033DF0(void) {
    s32 temp_r9;

    lbl_80407F00 = 0;
    lbl_80407F08 = 0;
    if ((s32) lbl_80407EFC != 0) {
        if (fn_80034BB8(0, 4) != 0) {
            temp_r9 = lbl_80407F04 - 1;
            lbl_80407F04 = temp_r9;
            if (temp_r9 < 0) {
                lbl_80407F04 = 0;
            }
        }
        if (fn_80034BB8(0, 5) != 0) {
            lbl_80407F04 += 1;
        }
    }
    if ((s32) lbl_80407F1C != (s32) lbl_80407F18) {
        fn_80030E3C();
        lbl_80407F1C = lbl_80407F18;
    }
}

extern s32 lbl_80220FBC[];
extern s32 lbl_802211B8[];

void fn_80033EA4(u16 *arg0) {
    s32 *var_r9;
    void *var_r9_2;
    s32 temp_r0;
    s32 var_ctr;
    s32 var_r10;
    s32 var_r11;
    s32 var_r11_2;

    if (*arg0 & 4) {
        var_ctr = 0x80;
        var_r10 = -1;
        var_r9 = lbl_80220FBC;
        var_r11 = 0;
        do {
            temp_r0 = *var_r9;
            var_r9 += 1;
            if (temp_r0 == arg0) {
                var_r10 = var_r11;
            }
            var_r11 += 1;
            var_ctr -= 1;
        } while (var_ctr != 0);
        if (var_r10 != -1) {
            var_r11_2 = var_r10;
            if (var_r11_2 <= 0x7E) {
                var_r9_2 = &(lbl_80220FBC)[var_r11_2];
                do {
                    var_r11_2 += 1;
                    (*(s32 *)((s8 *)(var_r9_2) + (0))) = (*(s32 *)((s8 *)(var_r9_2) + (4)));
                    var_r9_2 += 4;
                } while (var_r11_2 <= 0x7E);
            }
            lbl_802211B8[0] = 0;
        }
    }
}

void fn_8003294C(void) {
    volatile s32 pad[2];
}

s32 fn_8002303C();
void fn_8002DE44();
extern s32 lbl_80407CE0;
extern s32 lbl_80407CE4;
extern s32 lbl_80407EE8;
extern s32 lbl_8040ADAC;

void fn_8002E4D8(void) {
    s32 var_r0;

    if (((s32) lbl_80407EE8 != 0) && ((s32) lbl_80407CE0 != 0)) {
        var_r0 = lbl_8040ADAC;
        if (var_r0 != 0) {
            fn_8002303C(0x80, 0);
            fn_8002DE44();
            goto block_4;
        }
    } else {
block_4:
        var_r0 = 0;
    }
    lbl_80407CE4 = var_r0;
    lbl_80407CE0 = var_r0;
}

s32 fn_8002DAD0(f32 *v)
{
    lbl_803127F0.f30 = v[0];
    lbl_803127F0.f34 = v[1];
    lbl_803127F0.f38 = v[2];
    lbl_803127F0.f2c = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt24++;
    return 1;
}

s32 fn_8002DB1C(f32 *v)
{
    lbl_803127F0.f84 = v[3];
    lbl_803127F0.f30 = v[0];
    lbl_803127F0.f34 = v[1];
    lbl_803127F0.f38 = v[2];
    lbl_803127F0.f2c = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt24++;
    return 0;
}

s32 fn_80037220();
extern f32 lbl_80192388[];
extern f32 lbl_8019238C[];
extern f32 lbl_80192390[];
extern f32 lbl_80192394[];
extern f32 lbl_80192398[];
extern f32 lbl_8019239C[];
extern f32 lbl_80407F24;
extern f32 lbl_80407F28;
extern f32 lbl_80407F2C;

void fn_80033A88(void) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f12;
    f32 var_f13;
    u32 temp_r3;

    temp_r3 = fn_80037220();
    switch (temp_r3) {
    case 2:
        var_f13 = lbl_8019238C[0];
        break;
    case 1:
        var_f13 = lbl_80192390[0];
        break;
    case 3:
        var_f13 = lbl_80192388[0];
        break;
    default:
        var_f13 = lbl_80192388[0];
        break;
    }
    if (lbl_80407EF4 != lbl_80192394[0]) {
        temp_f12 = var_f13 * lbl_80192398[0] * lbl_80407F24;
        temp_f0 = lbl_80407F28 + temp_f12;
        lbl_80407F28 = temp_f0;
        if (temp_f0 > lbl_8019239C[0]) {
            lbl_80407F28 = temp_f0 - lbl_8019239C[0];
        }
        if (lbl_80407F28 < lbl_80192394[0]) {
            lbl_80407F28 += lbl_8019239C[0];
        }
        temp_f0_2 = lbl_80407F2C + temp_f12;
        lbl_80407F2C = temp_f0_2;
        if (temp_f0_2 > lbl_8019239C[0]) {
            lbl_80407F2C = temp_f0_2 - lbl_8019239C[0];
        }
        if (lbl_80407F2C < lbl_80192394[0]) {
            lbl_80407F2C += lbl_8019239C[0];
        }
    }
}

void fn_8002DD60(struct numtx_s *m, struct nuvec_s *v)
{
    lbl_803127F0.m88 = *m;
    lbl_803127F0.vc8 = *v;
    lbl_803127F0.f2c = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt24++;
}

s32 fn_8002D9E8(struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *c,
    struct nuvec_s *d, struct nuvec_s *e, struct nuvec_s *f)
{
    lbl_803127F0.v3c = *a;
    lbl_803127F0.v48 = *c;
    lbl_803127F0.v54 = *e;
    lbl_803127F0.v60 = *b;
    lbl_803127F0.v6c = *d;
    lbl_803127F0.v78 = *f;
    lbl_803127F0.f2c = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt24++;
    return 1;
}

void fn_8002DC2C(void) {
    s32 zero = 0;
    s32 invalid = -1;
    lbl_803127F0.fd4 = zero;
    lbl_803127F0.fe0 = zero;
    lbl_803127F0.f148 = zero;
    lbl_803127F0.f2c = zero;
    lbl_803127F0.ff0 = zero;
    lbl_803127F0.f15c = zero;
    lbl_803127F0.f08 = zero;
    lbl_803127F0.f14 = invalid;
    lbl_803127F0.n20 = invalid;
    lbl_803127F0.cnt24 = invalid;
    lbl_803127F0.cnt26 = invalid;
    lbl_803127F0.cnt28 = invalid;
    lbl_803127F0.cnt1e = invalid;
}

void OSPanic(void *, int, void *, ...);
extern s32 lbl_80192290[];
extern s32 lbl_801923B4[];
extern s32 lbl_8021FCB0[];
s32 fn_80033F30(s32 value)
{
    s32 i;
    for (i = 0; i < 0x400; i++) {
        if (lbl_8021FCB0[i] == 0) {
            lbl_8021FCB0[i] = value;
            return i;
        }
    }
    OSPanic(lbl_80192290, 0xB84, lbl_801923B4);
    return -1;
}

void fn_8002DC74(struct ngc_mtl_state_s *state)
{
    *state = lbl_803127F0;
    state->f148 = 0;
    state->ff0 = 0;
    state->f2c = 0;
    state->f15c = 0;
}

struct numtx_s *NuCameraGetProjectionMtx(void);
void fn_80037A4C(void);
void fn_80027DC8(void);
extern struct numtx_s lbl_8033EA8C;
s32 fn_8002D0FC(void)
{
    fn_80037A4C();
    lbl_803127F0.f134 = lbl_8033EA8C._00;
    lbl_803127F0.f138 = lbl_8033EA8C._11;
    lbl_803127F0.f13c = lbl_8033EA8C._22;
    lbl_803127F0.f140 = lbl_8033EA8C._23;
    lbl_803127F0.f144 = lbl_8033EA8C._32;
    lbl_803127F0.projection = *NuCameraGetProjectionMtx();
    lbl_803127F0.ff0 = 0;
    lbl_803127F0.n20++;
    lbl_803127F0.cnt26++;
    fn_80027DC8();
    return 1;
}

extern const f32 lbl_80192380[];
extern const f32 lbl_80192384[];

struct ngc_mtl_uv_s {
    u16 flags;
    u8 pad02[0x92];
    f32 u;
    f32 v;
    u8 pad9c[0x5e];
    u8 uMode;
    u8 vMode;
    u8 padfc[8];
    f32 uSpeed;
    f32 vSpeed;
};
void fn_800339C0(f32 dt)
{
    s32 count = 128;
    s32 *entry = lbl_80220FBC;
    do {
        struct ngc_mtl_uv_s *mtl = (struct ngc_mtl_uv_s *)*entry++;
        if (mtl != NULL && (mtl->flags & 4)) {
            if (mtl->uMode == 2) {
                mtl->u = mtl->uSpeed * dt + mtl->u;
                if (mtl->u > lbl_80192380[0]) {
                    mtl->u -= lbl_80192380[0];
                }
                if (mtl->u < lbl_80192384[0]) {
                    mtl->u += lbl_80192380[0];
                }
            }
            if (mtl->vMode == 2) {
                mtl->v = mtl->vSpeed * dt + mtl->v;
                if (mtl->v > lbl_80192380[0]) {
                    mtl->v -= lbl_80192380[0];
                }
                if (mtl->v < lbl_80192384[0]) {
                    mtl->v += lbl_80192380[0];
                }
            }
        }
        count--;
    } while (count != 0);
}

typedef struct { u32 words[16]; } NgcLightObj;
typedef struct { u8 r, g, b, a; } NgcColor;
void GXInitLightAttn(NgcLightObj *, f32, f32, f32, f32, f32, f32);
void GXInitLightPos(NgcLightObj *, f32, f32, f32);
void GXInitLightColor(NgcLightObj *, NgcColor);
void GXLoadLightObjImm(NgcLightObj *, s32);
void GXSetChanCtrl(s32, u8, s32, s32, u32, s32, s32);
void GXSetChanAmbColor(s32, NgcColor);
extern void *lbl_8040A3A8;
extern void *lbl_8040ADB0;

struct ngc_lighting_s {
    NgcColor ambient;
    s32 flags;
    struct {
        NgcColor color;
        struct nuvec_s pos;
    } lights[3];
};
struct ngc_channel_s {
    u8 enabled;
    u8 pad01[3];
    s32 source;
};
void fn_80032768(void) {
    NgcLightObj sp8;
    s32 temp_r0;
    struct ngc_channel_s *temp_r11;

    if ((void *) lbl_8040ADB4 != (void *) lbl_8040A3A8) {
        temp_r0 = ((struct ngc_lighting_s *)lbl_8040ADB4)->flags;
        if (temp_r0 & 7) {
            if (temp_r0 & 1) {
                GXInitLightAttn(&sp8, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                GXInitLightPos(&sp8,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[0].pos.x,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[0].pos.y,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[0].pos.z);
                GXInitLightColor(&sp8, ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[0].color);
                GXLoadLightObjImm(&sp8, 1);
            }
            if (((struct ngc_lighting_s *)lbl_8040ADB4)->flags & 2) {
                GXInitLightAttn(&sp8, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                GXInitLightPos(&sp8,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[1].pos.x,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[1].pos.y,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[1].pos.z);
                GXInitLightColor(&sp8, ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[1].color);
                GXLoadLightObjImm(&sp8, 2);
            }
            if (((struct ngc_lighting_s *)lbl_8040ADB4)->flags & 4) {
                GXInitLightAttn(&sp8, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                GXInitLightPos(&sp8,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[2].pos.x,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[2].pos.y,
                    ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[2].pos.z);
                GXInitLightColor(&sp8, ((struct ngc_lighting_s *)lbl_8040ADB4)->lights[2].color);
                GXLoadLightObjImm(&sp8, 4);
            }
        }
        temp_r11 = *(struct ngc_channel_s **)((s8 *)lbl_8040ADB0 + 0x50);
        if (temp_r11->enabled != 0) {
            GXSetChanCtrl(0, 1, 0, temp_r11->source,
                ((struct ngc_lighting_s *)lbl_8040ADB4)->flags, 2, 2);
            GXSetChanAmbColor(4, ((struct ngc_lighting_s *)lbl_8040ADB4)->ambient);
        }
        lbl_8040A3A8 = (void *)lbl_8040ADB4;
    }
}

struct ngc_tev_material_s {
    u16 flags;
    u8 pad02[0x34];
    u8 f36;
    u8 f37;
    u8 pad38[0x20];
    s32 f58, f5c, f60;
    s32 *f64, *f68;
    s32 f6c, f70, f74;
    s32 *f78, *f7c;
};
void GXSetTevKColor(s32, NgcColor);
void fn_800316D4(void *);
extern s32 lbl_80220CB0[];
extern s32 lbl_80220D1C[];
extern s32 lbl_80220D88[];
extern s32 lbl_80220DF4[];
extern s32 lbl_80220E84[];
extern s32 lbl_80220EA8[];
extern u8 lbl_8040ADE1[1];

void fn_80032958(void *arg0) {
    NgcColor color;
    struct ngc_tev_material_s *mtl;
    s32 *var_r9;
    u16 temp_r8;
    u16 temp_r9;

    /* Retail initializes only alpha; the TEV stage does not consume RGB. */
    color.a = 255;
    if (*(u8 *)arg0 != 0) {
        color.a = (s32)*(f32 *)((s8 *)arg0 + 4);
    }
    if (color.a != lbl_8040ADE1[0]) {
        GXSetTevKColor(0, color);
        lbl_8040ADE1[0] = color.a;
    }
    if ((((struct ngc_tev_material_s *)lbl_8040ADB0)->flags & 0x200) && color.a != 0xFF) {
        ((struct ngc_tev_material_s *)lbl_8040ADB0)->f36 = 1;
        ((struct ngc_tev_material_s *)lbl_8040ADB0)->f37 = 2;
        mtl = lbl_8040ADB0;
        temp_r9 = mtl->flags;
        mtl->f60 = 4;
        mtl->f58 = 0xFF;
        mtl->f5c = 0xFF;
        mtl->f68 = lbl_80220CB0;
        if (!(temp_r9 & 0x1000)) {
            if (temp_r9 & 0x400) {
                var_r9 = lbl_80220DF4;
            } else {
                var_r9 = lbl_80220D88;
            }
            mtl->f64 = var_r9;
        }
        mtl = lbl_8040ADB0;
        temp_r8 = mtl->flags;
        mtl->f74 = 0xFF;
        mtl->f6c = 0;
        mtl->f70 = 0;
        mtl->f7c = lbl_80220D1C;
        if (!(temp_r8 & 0x1000)) {
            if (temp_r8 & 0x400) {
                mtl->f64 = lbl_80220EA8;
            } else {
                mtl->f78 = lbl_80220E84;
            }
        }
        ((struct ngc_tev_material_s *)lbl_8040ADB0)->flags &= 0xFDFF;
        fn_800316D4(lbl_8040ADB0);
    }
}
