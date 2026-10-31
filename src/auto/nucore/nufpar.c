#include "types.h"

struct nufpar_a_s {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fc;
    s32 f10;
};

struct nufpar_c_s {
    u8 pad0[0x1c];
    s32 field1c;
    s32 handle;
    u8 pad24[0xc];
    s32 pos;
};

struct nufpar_s {
    u8 pad0[0x108];
    union {
        u16 w[0x101];
        u8 b[0x202];
    } line;
    union {
        u16 w[0x100];
        u8 b[0x200];
    } word;
    u8 pad50c[6];
    s32 linePos;
    u8 pad514[0x34];
    u8 isUtf16;
};

struct nufpar_b_s {
    struct nufpar_c_s* data;
    s32 base;
    s32 pos;
    s32 size;
    s32 pad10;
    s32 pad14;
    s32 field18;
    s32 pad1c;
};

extern s32 lbl_80407A4C;
extern s32 lbl_8040A2C4;
extern struct nufpar_a_s lbl_803120C0[];
extern struct nufpar_b_s lbl_80312250[];
s32 fn_80006830(struct nufpar_s* p);

s32 NuDatOpenEx(s32 a, s32 b, s32 c, s32 d);
void fn_80006010(s32 id);
s32 NuFileSeek(s32 handle, s32 pos, s32 whence);
s32 NuDatFileFindTree(s32 id, s32 off, s32 whence);

s32 fn_8000605C(s32 v)
{
    s32 old = lbl_80407A4C;
    lbl_80407A4C = v;
    return old;
}

s32 fn_80005D7C(s32 id)
{
    return lbl_803120C0[id - 0x400].f8;
}

s32 fn_80005FF4(s32 id)
{
    s32 i = id - 0x800;
    struct nufpar_b_s* p = &lbl_80312250[i];
    return p->size;
}

s32 fn_80005E5C(s32 id)
{
    s32 i = id - 0x800;
    struct nufpar_b_s* p = &lbl_80312250[i];
    return p->pos - p->base;
}

void fn_80006010(s32 id)
{
    s32 i = id - 0x800;
    s32 z = 0;
    lbl_80312250[i].data->field1c = z;
    lbl_80312250[i].field18 = z;
}

s32 fn_80006038(s32 a, s32 b, s32 c)
{
    return NuDatOpenEx(a, b, c, 0);
}

void fn_80005D34(s32 id)
{
    if (id > 0x7ff) {
        fn_80006010(id);
    } else {
        id -= 0x400;
        lbl_803120C0[id].f10 = 0;
    }
}

s32 fn_80005CC4(s32 a, s32 b, u32 c)
{
    s32 i;
    if (b > 0 && c <= 1) {
        for (i = 0; i < 20; i++) {
            if (lbl_803120C0[i].f10 == 0) {
                lbl_803120C0[i].f4 = a + b - 1;
                lbl_803120C0[i].f0 = a;
                lbl_803120C0[i].f8 = a;
                lbl_803120C0[i].fc = c;
                lbl_803120C0[i].f10 = 1;
                return i + 0x400;
            }
        }
    }
    return 0;
}

s32 fn_80006830(struct nufpar_s* p)
{
    u16* line = p->line.w;
    u16* word = p->word.w;
    s32 len = 0;
    s32 quoted = 0;
    lbl_8040A2C4 = p->linePos;
    while (line[p->linePos & 0xff] != 0) {
        u16 c = line[p->linePos];
        switch (c & 0xff) {
            case ' ':
            case ',':
            case '\t':
                if (!quoted) {
                    if (len != 0) {
                        word[len & 0xff] = 0;
                        return len;
                    }
                    break;
                }
            default:
                if (c == '"') {
                    quoted = 1 - quoted;
                } else {
                    word[len & 0xff] = c;
                    len++;
                }
                break;
        }
        p->linePos++;
    }
    word[len & 0xff] = 0;
    return len;
}

s32 NuDatFileFindTree(s32 id, s32 off, s32 whence)
{
    s32 i = id - 0x800;
    struct nufpar_b_s* p = &lbl_80312250[i];
    s32 target;
    switch (whence) {
        case 0:
        default:
            target = p->base + off;
            break;
        case 1:
            target = p->pos + off;
            break;
        case 2:
            target = p->base + p->size - off;
            break;
    }
    p->data->pos = NuFileSeek(p->data->handle, target, 0);
    p->pos = p->data->pos;
    return p->pos;
}

s32 fn_80005D98(s32 id, s32 off, s32 whence)
{
    if (id > 0x7ff) {
        return NuDatFileFindTree(id, off, whence);
    }
    id -= 0x400;
    switch (whence) {
        case 0:
        default:
            lbl_803120C0[id].f8 = lbl_803120C0[id].f0 + off;
            break;
        case 1:
            lbl_803120C0[id].f8 = lbl_803120C0[id].f8 + off;
            break;
        case 2:
            lbl_803120C0[id].f8 = lbl_803120C0[id].f4 - off;
            break;
    }
    return lbl_803120C0[id].f8 - lbl_803120C0[id].f0;
}

s32 NuFParGetWord(struct nufpar_s* p)
{
    s32 len;
    s32 quoted;
    s32 found = 0;
    if (p->isUtf16) {
        return fn_80006830(p);
    }
    len = 0;
    quoted = 0;
    lbl_8040A2C4 = p->linePos;
    while (p->line.b[p->linePos & 0x1ff] != 0) {
        s8 c = p->line.b[p->linePos];
        switch (c) {
            case ' ':
            case ',':
            case '\t':
                if (!quoted) {
                    if (len != 0) {
                        p->word.b[len & 0x1ff] = 0;
                        return len;
                    }
                    break;
                }
            default:
                if (c == '"') {
                    quoted = 1 - quoted;
                    found = 1;
                } else {
                    p->word.b[len & 0x1ff] = c;
                    len++;
                }
                break;
        }
        p->linePos++;
        if (found && quoted == 0 && len == 0) {
            break;
        }
    }
    p->word.b[len & 0x1ff] = 0;
    return len;
}
