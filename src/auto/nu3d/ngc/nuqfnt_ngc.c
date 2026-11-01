#include "types.h"

struct nuqfnt_cmd_s {
    s8 op;
    u8 arg0;
    u8 pad[2];
    s32 arg1;
};

void fn_8002BE28(struct nuqfnt_cmd_s* c)
{
    s32 z = 0;
    c->arg1 = z;
    c->op = 0x87;
    c->arg0 = z;
}

void fn_8002BE40(struct nuqfnt_cmd_s* c)
{
    s32 z = 0;
    c->arg1 = z;
    c->op = 0x87;
    c->arg0 = z;
}

void fn_8002BE58(struct nuqfnt_cmd_s* c, s32 v)
{
    c->arg0 = 3;
    c->arg1 = v;
    c->op = 0x80;
}

void fn_8002BE70(struct nuqfnt_cmd_s* c)
{
    s32 z = 0;
    c->arg1 = z;
    c->op = 0x87;
    c->arg0 = z;
}

void fn_8002BE88(struct nuqfnt_cmd_s* c)
{
    s32 z = 0;
    c->arg1 = z;
    c->op = 0x87;
    c->arg0 = z;
}

void fn_8002C240(void)
{
}

void fn_8002C320(void)
{
}

s32 fn_8002C244(void)
{
    return 0;
}
