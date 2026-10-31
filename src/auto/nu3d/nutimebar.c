#include "types.h"

struct nutimebar_s {
    u8 pad[0x1b4];
    s16 count;
    u8 pad1b6[4];
    s16 ids[1];
};

extern struct nutimebar_s* lbl_8040A2F8;

void fn_800162A0(void)
{
    struct nutimebar_s* t = lbl_8040A2F8;
    t->ids[t->count++] = 12;
}

void fn_80016360(void)
{
    struct nutimebar_s* t = lbl_8040A2F8;
    t->ids[t->count++] = 14;
}

void fn_80016388(void)
{
    struct nutimebar_s* t = lbl_8040A2F8;
    t->ids[t->count++] = 16;
}
