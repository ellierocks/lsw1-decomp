#include "types.h"

extern void fn_8002C3A0(s32 a, s32 b, s32 c);

extern void fn_8002C644(s32 a, s32 b, s32 c);

struct nustream_obj_s {
    u8 pad[0x48];
    s32* p;
};

extern s32 lbl_80407EC8;

void fn_8002CEF0(void)
{
}

void fn_8002CB8C(s32 a)
{
    lbl_80407EC8 = a;
}

void fn_8002CB54(void* a, struct nustream_obj_s* b, s32 c)
{
    *b->p = c;
}

void fn_8002CD00(struct nustream_obj_s* a, s32 c)
{
    *a->p = c;
}

void fn_8002CDA8(s32 a, s32 b)
{
    fn_8002C644(a, b, 0);
}

void fn_8002CDCC(s32 a, s32 b)
{
    fn_8002C644(a, b, 2);
}

void fn_8002CB60(s32 a, s32 b, s32 c, s32 d)
{
    fn_8002C644(b, c, d);
}

void fn_8002CD7C(u8* a, s32 b)
{
    fn_8002C3A0(*(s32*)(a + 0x38), *(s32*)(a + 0x10), b);
}
