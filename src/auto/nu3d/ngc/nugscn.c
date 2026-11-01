#include "types.h"
#include "nu3dx/nu3dx.h"


extern s32 lbl_80407BE8;
extern s32 lbl_80407E04;
extern s32 lbl_80407E08;
extern f32 lbl_8040AD8C;
extern f32 lbl_8040AD90;
extern s32 lbl_8040AD94;
extern u32* lbl_8040A34C;
extern u32 lbl_8040A350;
extern struct nugscn_s* lbl_8040A364;
extern char lbl_8019177C[];
extern char lbl_8019178C[];
extern char lbl_801917B4[];
extern char lbl_801918E8[];
extern char lbl_80191930[];
extern char lbl_801919A8[];

void (*fn_80009948(char* file, s32 line))();
void (*fn_80009970(char* file, s32 line))();
s32 NuFileReadInt(s32 fh);
void* fn_80005D7C(s32 fh);
void fn_80005D98(s32 fh, s32 size, s32 count);
s32 fn_80027FAC(char* name, char* path, s32* arg);
void fn_800140EC(s32 font);
void fn_8002DAD0(struct nuvec_s* v);
void fn_8002DCF8();
void fn_8002DD28();
void fn_80015E48(void* texanims);
void fn_80024D44(struct numtl_s* mtl);
void fn_80024C64(struct numtl_s* mtl);
void NuFileWrite(s32 tid);
void fn_80011070(struct nudisplayscene_s* display);

static inline void* NuGScnAlloc(s32 size, s32 align)
{
    u32 p = (*lbl_8040A34C + (align - 1)) & ~(align - 1);
    if (p + size < lbl_8040A350) {
        *lbl_8040A34C = p + size;
        return (void*)p;
    }
    fn_80009948(lbl_801918E8, 0x162)(lbl_80191930);
    return NULL;
}

void fn_80028138(char* path, s32* arg)
{
    s32 local = *arg;

    if (lbl_80407E08 == 0 && lbl_80407BE8 == 0) {
        lbl_80407BE8 = 1;
        lbl_80407E04 = fn_80027FAC(lbl_8019177C, path, &local);
        if (lbl_80407E04 == 0) {
            lbl_80407E04 = fn_80027FAC(lbl_8019178C, path, &local);
            if (lbl_80407E04 == 0) {
                lbl_80407E04 = fn_80027FAC(lbl_801917B4, path, &local);
            }
        }
        fn_800140EC(lbl_80407E04);
    }
}

void fn_800281EC(void)
{
    struct nuvec_s v = { 0.15f, 0.15f, 0.15f };

    fn_8002DAD0(&v);
}

void fn_80028234(void)
{
}

void fn_80028238(void)
{
}

void fn_8002823C(s32 mode, f32 x, f32 y)
{
    lbl_8040AD8C = x;
    lbl_8040AD90 = y;
    lbl_8040AD94 = mode;
    if (mode == 0 || x == 0.0f || y == 0.0f) {
        fn_8002DCF8(0);
    } else {
        fn_8002DCF8(1);
        // Unprototyped call: GPR and FPR slots are assigned independently,
        // so this reaches fn_8002DD28(mode, x, y). Only this order matches.
        fn_8002DD28(x, mode, y);
    }
}

void fn_800282D8(void)
{
}

void fn_800282DC(void)
{
}

void fn_800282E0(void)
{
}

void fn_800282E4(void)
{
}

void fn_800282E8(s32 fh)
{
    s32 count;
    s32 i;
    struct numtl_s* mtl;

    count = NuFileReadInt(fh);
    NuFileReadInt(fh);
    lbl_8040A364->mtls = NuGScnAlloc(count * 4, 4);
    lbl_8040A364->nummtl = count;
    for (i = 0; i < count; i++) {
        lbl_8040A364->mtls[i] = fn_80005D7C(fh);
        if (lbl_8040A364->mtls[i]->tid == -1) {
            lbl_8040A364->mtls[i]->tid = 0;
        } else {
            lbl_8040A364->mtls[i]->tid = lbl_8040A364->tids[lbl_8040A364->mtls[i]->tid];
        }
        fn_80005D98(fh, 0x11C, 1);
    }
    for (i = 0; i < count; i++) {
        mtl = lbl_8040A364->mtls[i];
        if ((s32)mtl->next == -1) {
            mtl->next = NULL;
        } else {
            mtl->next = lbl_8040A364->mtls[(s32)mtl->next];
        }
    }
}

void fn_8002A3A8(struct nugscn_s* scene)
{
    s32 i;

    if (scene->texanims != NULL) {
        fn_80015E48(scene->texanims);
    }
    for (i = 0; i < scene->nummtl; i++) {
        while (scene->mtls[i]->next != NULL) {
            fn_80024D44(scene->mtls[i]);
        }
        fn_80024C64(scene->mtls[i]);
    }
    fn_80009970(lbl_801918E8, 0x581)(lbl_801919A8, scene->nummtl);
    for (i = 0; i < scene->numtid; i++) {
        NuFileWrite(scene->tids[i]);
    }
    fn_80011070(scene->displayScene);
}
