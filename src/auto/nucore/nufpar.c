#include "types.h"

struct nufpar_a_s {
    s32 pad0;
    s32 pad4;
    s32 value;
    s32 pad0c;
    s32 pad10;
};

struct nufpar_c_s {
    u8 pad0[0x1c];
    s32 field1c;
};

struct nufpar_b_s {
    struct nufpar_c_s* data;
    s32 start;
    s32 end;
    s32 field0c;
    s32 pad10;
    s32 pad14;
    s32 field18;
    s32 pad1c;
};

extern s32 lbl_80407A4C;
extern struct nufpar_a_s lbl_803120C0[];
extern struct nufpar_b_s lbl_80312250[];

s32 fn_8000605C(s32 v)
{
    s32 old = lbl_80407A4C;
    lbl_80407A4C = v;
    return old;
}

s32 fn_80005D7C(s32 id)
{
    return lbl_803120C0[id - 0x400].value;
}

s32 fn_80005FF4(s32 id)
{
    s32 i = id - 0x800;
    struct nufpar_b_s* p = &lbl_80312250[i];
    return p->field0c;
}

s32 fn_80005E5C(s32 id)
{
    s32 i = id - 0x800;
    struct nufpar_b_s* p = &lbl_80312250[i];
    return p->end - p->start;
}

void fn_80006010(s32 id)
{
    s32 i = id - 0x800;
    s32 z = 0;
    lbl_80312250[i].data->field1c = z;
    lbl_80312250[i].field18 = z;
}
