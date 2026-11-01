#include "types.h"
#include "nu3dx/nu3dx.h"

extern char lbl_801912C4[];
extern char str_NuMtlSetCurrentRenderPlane_OutOfRange[];
extern s32 lbl_80407D24;
extern s32 lbl_80407D1C;
extern void (*fn_80009948(char* file, s32 line))();
extern void fn_80032CB4(struct numtl_s* mtl);
extern void fn_80033EA4(struct numtl_s* mtl);
extern void fn_80012B44(struct numtl_s* mtl);
extern void fn_80024F40(s32 tid);
void fn_80024D54(struct numtl_s* mtl);

extern s32 lbl_80407D20;

struct numtl_s* fn_80024A34(s32 n, s32 plane, s32 flag);
extern void* memset();
extern struct numtl_s lbl_8020E9B4[100];
extern char lbl_801912F4[];
extern void fn_80024DB8(struct numtl_s* mtl, s32 plane);
extern void fn_80012AB4(struct numtl_s* mtl);

extern void fn_800339C0(void);

extern s32 lbl_80407D5C;
s32 fn_80024DC0(void);

extern s32 lbl_80407D18;
extern struct numtl_s* lbl_80407D50;
extern struct numtl_s* lbl_80407D4C;
extern struct numtl_s* lbl_80407D54;
void NuHazeClose(void);
struct numtl_s* fn_80024C18(s32 a);
struct numtl_s* fn_80024BF0(s32 a);
struct numtl_s* fn_80024C40(s32 a, s32 b);
void fn_80024CD4(struct numtl_s* mtl);

void fn_800248CC(void)
{
    if (lbl_80407D18 != 0) {
        NuHazeClose();
    }
    lbl_80407D24 = 0;
    memset(lbl_8020E9B4, 0, sizeof(lbl_8020E9B4));
    lbl_80407D18 = 1;
    lbl_80407D50 = fn_80024C18(1);
    lbl_80407D50->unknownD0[0] = 1.0f;
    lbl_80407D50->unknownD0[1] = 1.0f;
    lbl_80407D50->unknownD0[2] = 1.0f;
    lbl_80407D50->unknownBD = 2;
    lbl_80407D50->unknownBE = 3;
    lbl_80407D50->unknownBF = 2;
    lbl_80407D50->unknownF0 = 1.0f;
    lbl_80407D50->unknownB8 = 0;
    lbl_80407D50->unknownC0 = 1;
    lbl_80407D50->unknownC2 = 1;
    fn_80024CD4(lbl_80407D50);
    lbl_80407D4C = fn_80024BF0(1);
    lbl_80407D4C->unknownD0[0] = 1.0f;
    lbl_80407D4C->unknownD0[1] = 1.0f;
    lbl_80407D4C->unknownD0[2] = 1.0f;
    lbl_80407D4C->unknownBD = 2;
    lbl_80407D4C->unknownBE = 3;
    lbl_80407D4C->unknownBF = 2;
    lbl_80407D4C->unknownF0 = 1.0f;
    lbl_80407D4C->unknownB8 = 0;
    lbl_80407D4C->unknownC0 = 1;
    lbl_80407D4C->unknownC2 = 1;
    fn_80024CD4(lbl_80407D4C);
    lbl_80407D54 = fn_80024C40(1, 100);
    lbl_80407D54->unknownD0[0] = 1.0f;
    lbl_80407D54->unknownD0[1] = 1.0f;
    lbl_80407D54->unknownD0[2] = 1.0f;
    lbl_80407D54->unknownBD = 2;
    lbl_80407D54->unknownBE = 3;
    lbl_80407D54->unknownBF = 2;
    lbl_80407D54->unknownF0 = 1.0f;
    lbl_80407D54->unknownB8 = 0;
    lbl_80407D54->unknownC0 = 1;
    lbl_80407D54->unknownC2 = 1;
    fn_80024CD4(lbl_80407D54);
}

static inline s32 NuMtlFindFree(void)
{
    s32 i;

    for (i = 0; i < 100; i++) {
        if (!lbl_8020E9B4[i].used) {
            return i;
        }
    }
    return 100;
}

struct numtl_s* fn_80024A34(s32 n, s32 plane, s32 flag)
{
    struct numtl_s* mtl = NULL;
    struct numtl_s* last = NULL;
    s32 i;

    for (i = 0; i < n; i++) {
        mtl = &lbl_8020E9B4[NuMtlFindFree()];
        memset(mtl, 0, sizeof(struct numtl_s));
        mtl->used = 1;
        mtl->unknownB9 = 1;
        mtl->unknownC2 = 1;
        mtl->unknownC7 = 1;
        mtl->unknownF0 = 1.0f;
        mtl->unknownFC[0] = 0.0f;
        mtl->unknownFC[1] = 0.0f;
        mtl->unknownFC[2] = 0.0f;
        mtl->unknownFC[3] = 0.0f;
        mtl->unknownBF = 2;
        mtl->renderPlane = plane;
        fn_80024DB8(mtl, plane);
        if (flag != 0) {
            fn_80012AB4(mtl);
        }
        mtl->next = last;
        lbl_80407D1C++;
        last = mtl;
        if (++lbl_80407D24 > 99) {
            fn_80009948(lbl_801912C4, 0x17A)(lbl_801912F4);
        }
    }
    return mtl;
}

void fn_80024C64(struct numtl_s* mtl)
{
    fn_80033EA4(mtl);
    fn_80012B44(mtl);
    if (mtl->tid != 0) {
        fn_80024F40(mtl->tid);
    }
    fn_80024D54(mtl);
    mtl->used = 0;
    lbl_80407D24--;
    lbl_80407D1C--;
}

void fn_80024CD4(struct numtl_s* mtl)
{
    fn_80032CB4(mtl);
    mtl->version++;
}

void fn_80024D54(struct numtl_s* mtl)
{
}

s32 NuMtlSetCurrentRenderPlane(u32 plane)
{
    s32 old;

    if (plane > 0x17) {
        fn_80009948(lbl_801912C4, 0x3D9)(str_NuMtlSetCurrentRenderPlane_OutOfRange);
    }
    old = lbl_80407D20;
    lbl_80407D20 = plane;
    return old;
}

void fn_80024D34(u8* p)
{
    s32 z = 0;
    *(s32*)(p + 0xb4) = z;
    p[0xa7] = z;
}

void fn_80024D44(u8* p)
{
    *(s32*)(p + 0xb4) = *(s32*)(*(u8**)(p + 0xb4) + 0xb4);
}

void NuHazeClose(void)
{
    if (lbl_80407D18 != 0) {
        lbl_80407D18 = 0;
    }
}

s32 fn_80024D0C(s32 cb)
{
    s32 old = lbl_80407D5C;
    if (cb != 0) {
        lbl_80407D5C = cb;
    } else {
        lbl_80407D5C = (s32)fn_80024DC0;
    }
    return old;
}

void fn_80024BB8(void)
{
    fn_800339C0();
}

struct numtl_s* fn_80024C40(s32 a, s32 b)
{
    return fn_80024A34(a, b, 1);
}

struct numtl_s* fn_80024BF0(s32 a)
{
    return fn_80024A34(a, lbl_80407D20, 0);
}

struct numtl_s* fn_80024C18(s32 a)
{
    return fn_80024A34(a, lbl_80407D20, 1);
}
