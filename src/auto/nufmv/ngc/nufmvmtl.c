#include "types.h"

extern u8 lbl_802B10C0[];
extern s32 OSSendMessage(void* q, s32 m, s32 f);

struct nufmv_ent_s {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
};

extern struct nufmv_ent_s lbl_8034C080[];

extern s32 lbl_8040831C;

extern s32 lbl_80408314;

void fn_80049A10(void)
{
}

void fn_80048528(s32 a)
{
    lbl_80408314 = a << 1;
}

void fn_80048534(void)
{
    lbl_8040831C = 0;
}

void fn_80048540(s32* p)
{
    lbl_8034C080[p[6]].a = 0;
    lbl_8034C080[p[6]].c = 0;
}

void fn_80048498(void)
{
    OSSendMessage(lbl_802B10C0, 6, 1);
}

void fn_800484C8(void)
{
    OSSendMessage(lbl_802B10C0, 5, 1);
}

void fn_800484F8(void)
{
    OSSendMessage(lbl_802B10C0, 7, 1);
}
