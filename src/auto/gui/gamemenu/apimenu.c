#include "types.h"
#include "game/gamemenu.h"

extern s32 lbl_8040889C;

void fn_8007483C(void)
{

}

void fn_80074B80(s32 a)
{
    lbl_8040889C = a;
}

extern s32 lbl_803D9020[];
extern s32 lbl_804088B0;

void fn_80075CD8(s32 arg0) {
    *(s32 *)(((s8 *)lbl_803D9020) + (arg0 * 4)) = 0;
    if ((s32) lbl_804088B0 == arg0) {
        lbl_804088B0 = 0;
    }
}

s32 NuStrNCmp(char *a, char *b, s32 n);
extern char lbl_803D5680[];

s32 PARTLookupType(char *name) {
    s32 i;
    char *p;

    if (*name == 0) return -1;
    {
        for (i = 0, p = lbl_803D5680; i < 0x40; i++, p += 0xB0) {
            if (NuStrNCmp(name, p, 0x10) == 0) {
                return i;
            }
        }
    }
    return -1;
}

typedef struct { u8 pad[0xC]; s32 id; u8 rest[0x6C - 0x10]; } Entry6C;
extern Entry6C lbl_803D8280[];
extern s32 lbl_804088AC;

void fn_80074CD8(s32 i) {
    if (i != -1) {
        if (lbl_803D8280[i].id != -1) {
            lbl_803D8280[i].id = -1;
            lbl_804088AC--;
        }
    }
}

int sprintf(char *, const char *, ...);
extern s32 lbl_80408234;
extern s32 lbl_80408A54;
extern char *lbl_80408B4C;
extern char *lbl_80408B50;
extern void (*lbl_80408C70)(void);

void fn_80077038(void) {
    char buf[0x80];
    f32 one = 1.0f;
    f32 half = 0.5f;

    if (lbl_80408C70 != NULL) {
        lbl_80408C70();
    }
    sprintf(buf, lbl_80408B4C, lbl_80408234 + 1);
    fn_8007B298(buf, 0.0f, 0.45f, one, half, half, half, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
    fn_8007B298(lbl_80408B50, 0.0f, -0.5f, one, half, half, half, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
}

extern s32 lbl_80408A48;
extern f32 lbl_80408A50;

void fn_80076194(void) {
    MenuFrame *m;
    MenuDef *d;

    if (gMenuDepth != 0) {
        m = &gMenuStack[gMenuDepth];
        if (gRegisteredMenuDefinitions[m->id].close != NULL) {
            gRegisteredMenuDefinitions[m->id].close(m);
        }
        if (m->id != -1) {
            d = &gRegisteredMenuDefinitions[m->id];
            if (d->sel8 != -1) {
                d->sel8 = m->sel8;
            }
            d = &gRegisteredMenuDefinitions[m->id];
            if (d->selA != -1) {
                d->selA = m->selA;
            }
        }
        m->unkBC = 1;
        m->id = -1;
        m->unkB8 = 0;
        m->unkC0 = 0;
        gMenuDepth--;
        m = &gMenuStack[gMenuDepth];
        m->unkBC = 1;
        m->unkB8 = 1;
        m->unkB4 = 0;
        m->unkC4 = 0;
        m->t0 = 0.0f;
        m->unk50 = lbl_80408A48;
        lbl_80408A50 = 0.0f;
        lbl_80408A54 = 0;
    }
}

void Menu_CloseCurrent(void) {
    MenuFrame *m;
    MenuDef *d;

    if (gMenuDepth != 0) {
        m = &gMenuStack[gMenuDepth];
        if (gRegisteredMenuDefinitions[m->id].close != NULL) {
            gRegisteredMenuDefinitions[m->id].close(m);
        }
        if (m->id != -1) {
            d = &gRegisteredMenuDefinitions[m->id];
            if (d->sel8 != -1) {
                d->sel8 = m->sel8;
            }
            d = &gRegisteredMenuDefinitions[m->id];
            if (d->selA != -1) {
                d->selA = m->selA;
            }
        }
        m->id = -1;
        m->unkB8 = 0;
        m->unkBC = 1;
        m->unkC0 = 0;
        gMenuDepth--;
        m = &gMenuStack[gMenuDepth];
        m->unkB4 = 0;
        m->unkC4 = 0;
        m->unkBC = 1;
        m->unkB8 = 1;
        m->t0 = 0.0f;
        m->unk50 = lbl_80408A48;
        if (m->id != -1 && gRegisteredMenuDefinitions[m->id].enter != NULL) {
            gRegisteredMenuDefinitions[m->id].enter(m);
        }
        lbl_80408A54 = 0;
        lbl_80408A50 = 0.0f;
    }
}

s32 Menu_Open(s32, s32, s32);
extern s32 lbl_8040821C;
extern s32 lbl_80408A3C;
extern s32 lbl_80408A44;
extern s32 lbl_80408C4C;

void Menu_Navigate(void *arg0) {
    s32 var_r3;
    s32 var_r4;
    s16 temp_r9;
    s32 temp_r0;
    s32 var_r31;

    var_r31 = 0;
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x68))) != 0) {
        temp_r9 = (*(s16 *)((s8 *)(arg0) + (0xA)));
        (*(s32 *)((s8 *)(arg0) + (0x50))) = (s32) lbl_80408A44;
        if (temp_r9 == 0) {
            if ((s32) lbl_80408C4C != 0) {
                if ((s32) gRegisteredMenuDefinitions[gMenuStack[gMenuDepth - 1].id].id != 0x3FA) {
                    fn_80076194();
                    var_r3 = 0x3FF;
                    var_r4 = -1;
                    goto block_7;
                }
                goto block_9;
            }
            if ((s32) lbl_8040821C == 2) {
                fn_80076194();
                var_r3 = 0x3FE;
                var_r4 = 1;
block_7:
                Menu_Open(var_r3, var_r4, -1);
            } else {
                goto block_9;
            }
        } else if (temp_r9 == 1) {
block_9:
            Menu_CloseCurrent();
        } else {
            goto block_11;
        }
    } else if ((s32) (*(s32 *)((s8 *)(arg0) + (0x6C))) != 0) {
block_11:
        var_r31 = 1;
    }
    if (var_r31 != 0) {
        temp_r0 = gRegisteredMenuDefinitions[gMenuStack[gMenuDepth - 1].id].id;
        if (temp_r0 == 0x3E8) {
            fn_80076194();
            Menu_Open(0x3F8, 1, -1);
            return;
        }
        if (temp_r0 == 0x3FA) {
            lbl_80408A3C = 3;
            Menu_CloseCurrent();
            return;
        }
        fn_80076194();
        Menu_Open(0x3F9, 1, -1);
    }
}

extern char *lbl_80408B34;
extern char *lbl_80408B40;
extern s32 lbl_80408A68;
extern s32 lbl_80408C50;
extern char *lbl_80408B78;
extern char *lbl_80408B84;
extern f32 lbl_8040AF18;
extern f32 lbl_80408A84;
extern s32 lbl_80408AB0;
extern s32 lbl_80408B18;
extern u8 lbl_80408A57;
void fn_800769B4(void *, s32, u8);

void fn_80077340(void *arg0) {
    char buf[0x80];
    char *title;
    f32 half = 0.5f;

    if (lbl_8040821C == 1) {
        title = lbl_80408B78;
    } else if (lbl_8040821C == 3) {
        title = lbl_80408B84;
    } else {
        title = lbl_80408B34;
    }
    fn_8007B298(title, 0.0f, 0.3f, 1.0f, half, half, half, 0, 255, 127, 0, 1.5f, 2, 0, 0, lbl_80408A54);
    sprintf(buf, lbl_80408B40, lbl_80408A68, lbl_80408C50);
    fn_8007B298(buf, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
    {
        f32 time = lbl_8040AF18;
        f32 offset = lbl_80408A84;
        s32 action = lbl_80408AB0;
        *(f32 *)((u8 *)arg0 + 0x9C) = time - offset;
        fn_800769B4(arg0, action, lbl_80408A57);
    }
    fn_800769B4(arg0, lbl_80408B18, lbl_80408A57);
}

extern char *lbl_80408B44;
extern char *lbl_80408B30;

void fn_80077274(void) {
    fn_8007B298(lbl_80408B44, 0.0f, 0.25f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
    fn_8007B298(lbl_80408B30, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
}

extern char *lbl_80408B54;
extern char *lbl_80408B58;
extern char *lbl_80408B5C;

void fn_80077148(void) {
    f32 one = 1.0f;
    f32 half = 0.5f;

    fn_8007B298(lbl_80408B54, 0.0f, 0.4f, one, half, half, half, 0, 255, 127, 0, 1.5f, 2, 0, 0, lbl_80408A54);
    fn_8007B298(lbl_80408B58, 0.0f, 0.0f, one, half, half, half, 0, 255, 127, 0, 1.5f, 2, 0, 0, lbl_80408A54);
    fn_8007B298(lbl_80408B5C, 0.0f, -0.4f, one, half, half, half, 0, 255, 127, 0, 1.5f, 2, 0, 0, lbl_80408A54);
}

extern char *lbl_80408B48;

void fn_80077480(void *arg0) {
    char buf[0x80];
    f32 one = 1.0f;
    f32 half = 0.5f;

    fn_8007B298(lbl_80408B48, 0.0f, 0.3f, one, half, half, half, 0, 255, 127, 0, 1.5f, 2, 0, 0, lbl_80408A54);
    sprintf(buf, lbl_80408B40, lbl_80408A68, lbl_80408C50);
    fn_8007B298(buf, 0.0f, -0.3f, one, half, half, half, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
    {
        f32 time = lbl_8040AF18;
        f32 offset = lbl_80408A84;
        s32 action = lbl_80408AB0;
        *(f32 *)((u8 *)arg0 + 0x9C) = time - offset;
        fn_800769B4(arg0, action, lbl_80408A57);
    }
    fn_800769B4(arg0, lbl_80408B18, lbl_80408A57);
}

extern char lbl_803D992C[];
extern const char *lbl_80408AC8;
extern s32 lbl_8040A618, lbl_8040A61C, lbl_8040A620;
extern s32 lbl_80408AA0, lbl_80408AAC;
extern char *lbl_80408B68, *lbl_80408B6C;
void NuStrCpy(char *, const char *);

void fn_80077C34(void *arg0) {
    NuStrCpy(lbl_803D992C, lbl_80408AC8);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    fn_8007B298(lbl_80408B68, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.2f, 2, 0, 0, lbl_80408A54);
    fn_8007B298(lbl_80408B6C, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.2f, 2, 0, 0, lbl_80408A54);
    {
        f32 time = lbl_8040AF18;
        f32 offset = lbl_80408A84;
        s32 action = lbl_80408AA0;
        *(f32 *)((u8 *)arg0 + 0x9C) = time - offset;
        fn_800769B4(arg0, action, lbl_80408A57);
    }
    fn_800769B4(arg0, lbl_80408AAC, lbl_80408A57);
}

extern s32 lbl_80408A38;
extern s32 lbl_80408AA4;
extern char *lbl_80408B04, *lbl_80408B08;

void fn_80077D54(void *arg0) {
    NuStrCpy(lbl_803D992C, lbl_80408AC8);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    if (lbl_80408A38 != 0) {
        fn_8007B298(lbl_80408B08, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.2f, 2, 0, 0, lbl_80408A54);
    } else {
        fn_8007B298(lbl_80408B04, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.2f, 2, 0, 0, lbl_80408A54);
    }
    {
        f32 time = lbl_8040AF18;
        f32 offset = lbl_80408A84;
        s32 action = lbl_80408AA0;
        *(f32 *)((u8 *)arg0 + 0x9C) = time - offset;
        fn_800769B4(arg0, action, lbl_80408A57);
    }
    fn_800769B4(arg0, lbl_80408AA4, lbl_80408A57);
}

extern char *lbl_80408B38, *lbl_80408B3C;

void fn_80077AD4(void *arg0) {
    fn_8007B298(lbl_80408B38, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.5f, 2, 0, 0, lbl_80408A54);
    fn_8007B298(lbl_80408B3C, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.2f, 2, 0, 0, lbl_80408A54);
    {
        f32 time = lbl_8040AF18;
        f32 offset = lbl_80408A84 + lbl_80408A84;
        s32 action = lbl_80408AA0;
        *(f32 *)((u8 *)arg0 + 0x9C) = time - offset;
        fn_800769B4(arg0, action, lbl_80408A57);
    }
    fn_800769B4(arg0, lbl_80408AB0, lbl_80408A57);
    if (gRegisteredMenuDefinitions[gMenuStack[gMenuDepth - 1].id].id == 0x3F4) {
        fn_800769B4(arg0, lbl_80408AAC, lbl_80408A57);
    } else {
        fn_800769B4(arg0, lbl_80408B18, lbl_80408A57);
    }
}

extern s32 lbl_80408C00, lbl_80408BFC, lbl_80408C04;
extern f32 lbl_80408C20;
extern s32 lbl_80408B8C;
extern char *lbl_80408AE8, *lbl_80408AFC, *lbl_80408AE0;

void fn_800775B4(void) {
    if (lbl_80408C00 != 0 || lbl_80408BFC != 0 || lbl_80408C20 > 0.0f) {
        f32 one = 1.0f;
        f32 half = 0.5f;

        lbl_80408B8C = 0;
        fn_8007AC80(lbl_80408AE8, 0.0f, 0.0f, one, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        fn_8007B298(lbl_80408B30, 0.0f, -0.3f, one, half, half, half, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
    } else if (lbl_80408C04 != 0) {
        if (lbl_80408B8C == 0) {
            lbl_80408A54 = 0;
            lbl_80408B8C = 1;
            lbl_80408A50 = 0.0f;
        }
        {
            f32 half = 0.5f;
            fn_8007AC80(lbl_80408AFC, 0.0f, 0.0f, 1.0f, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        }
    } else {
        if (lbl_80408B8C == 0) {
            lbl_80408A54 = 0;
            lbl_80408B8C = 1;
            lbl_80408A50 = 0.0f;
        }
        {
            f32 half = 0.5f;
            fn_8007AC80(lbl_80408AE0, 0.0f, 0.0f, 1.0f, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        }
    }
}

extern const char *lbl_80408AD0;
extern s32 lbl_80408BF4, lbl_80408BF8;
extern f32 lbl_80408C28;
extern s32 lbl_80408B90;
extern char *lbl_80408AE4, *lbl_80408ADC;

void fn_80077770(void) {
    NuStrCpy(lbl_803D992C, lbl_80408AD0);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    if (lbl_80408BF4 != 0 || lbl_80408BF8 != 0 || lbl_80408C28 > 0.0f) {
        f32 one = 1.0f;
        f32 half = 0.5f;

        lbl_80408B90 = 0;
        fn_8007AC80(lbl_80408AE4, 0.0f, 0.0f, one, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        fn_8007B298(lbl_80408B30, 0.0f, -0.3f, one, half, half, half, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
    } else {
        if (lbl_80408B90 == 0) {
            lbl_80408A54 = 0;
            lbl_80408B90 = 1;
            lbl_80408A50 = 0.0f;
        }
        {
            f32 half = 0.5f;
            fn_8007AC80(lbl_80408ADC, 0.0f, 0.0f, 1.0f, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        }
    }
}

extern s32 lbl_80408C10, lbl_80408C0C, lbl_80408C14;
extern f32 lbl_80408C24;
extern s32 lbl_80408B94;
extern char *lbl_80408B60, *lbl_80408B2C, *lbl_80408B64;

void fn_800778F0(void) {
    NuStrCpy(lbl_803D992C, lbl_80408AC8);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    if (lbl_80408C10 != 0 || lbl_80408C0C != 0 || lbl_80408C24 > 0.0f) {
        f32 one = 1.0f;
        f32 half = 0.5f;

        lbl_80408B94 = 0;
        fn_8007AC80(lbl_80408B60, 0.0f, 0.0f, one, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        fn_8007B298(lbl_80408B30, 0.0f, -0.3f, one, half, half, half, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
    } else if (lbl_80408C14 != 0) {
        if (lbl_80408B94 == 0) {
            lbl_80408A54 = 0;
            lbl_80408B94 = 1;
            lbl_80408A50 = 0.0f;
        }
        {
            f32 half = 0.5f;
            fn_8007AC80(lbl_80408B2C, 0.0f, 0.0f, 1.0f, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        }
    } else {
        if (lbl_80408B94 == 0) {
            lbl_80408A54 = 0;
            lbl_80408B94 = 1;
            lbl_80408A50 = 0.0f;
        }
        {
            f32 half = 0.5f;
            fn_8007AC80(lbl_80408B64, 0.0f, 0.0f, 1.0f, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        }
    }
}

extern const char *lbl_80408ACC;
extern s32 lbl_80408BC0, lbl_80408BC4, lbl_80408BC8, lbl_80408BCC;
extern f32 lbl_80408C18;
extern s32 lbl_80408BA0;
extern char *lbl_80408B24, *lbl_80408AF4, *lbl_80408AF0, *lbl_80408AD8;

void fn_8007818C(void) {
    NuStrCpy(lbl_803D992C, lbl_80408ACC);
    lbl_8040A618 = 0;
    lbl_8040A61C = 127;
    lbl_8040A620 = 255;
    if (lbl_80408BC0 != 0 || lbl_80408BC4 != 0 || lbl_80408C18 > 0.0f) {
        f32 one = 1.0f;
        f32 half = 0.5f;

        lbl_80408BA0 = 0;
        fn_8007AC80(lbl_80408B24, 0.0f, 0.0f, one, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        fn_8007B298(lbl_80408B30, 0.0f, -0.3f, one, half, half, half, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
    } else if (lbl_80408BC8 != 0) {
        if (lbl_80408BA0 == 0) {
            lbl_80408A54 = 0;
            lbl_80408BA0 = 1;
            lbl_80408A50 = 0.0f;
        }
        {
            f32 half = 0.5f;
            fn_8007AC80(lbl_80408AF4, 0.0f, 0.0f, 1.0f, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        }
    } else if (lbl_80408BCC != 0) {
        if (lbl_80408BA0 == 0) {
            lbl_80408A54 = 0;
            lbl_80408BA0 = 1;
            lbl_80408A50 = 0.0f;
        }
        {
            f32 half = 0.5f;
            fn_8007AC80(lbl_80408AF0, 0.0f, 0.0f, 1.0f, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        }
    } else {
        if (lbl_80408BA0 == 0) {
            lbl_80408A54 = 0;
            lbl_80408BA0 = 1;
            lbl_80408A50 = 0.0f;
        }
        {
            f32 half = 0.5f;
            fn_8007AC80(lbl_80408AD8, 0.0f, 0.0f, 1.0f, half, half, half, 0, 255, 127, 0, lbl_80408A57);
        }
    }
}

extern s32 lbl_80408A3C, lbl_80408A20;
extern f32 lbl_8040945C, lbl_8040AF1C;
extern char *lbl_80408B0C;
extern s32 lbl_80408B80;

void fn_800783B8(void *arg0) {
    char buf[0x80];
    s32 mode = lbl_80408A3C;

    if ((mode != 0 || lbl_80408A20 != 0) && mode != 3 && mode != 4) {
        *(f32 *)((u8 *)arg0 + 0x9C) = lbl_8040945C - (lbl_80408A84 + lbl_80408A84);
        switch (mode) {
        case 1:
            fn_8007B298(lbl_80408B34, 0.0f, lbl_8040AF1C, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 0, 0, 1.5f, 2, 0, 0, lbl_80408A54);
            break;
        case 2:
            fn_8007B298(lbl_80408B48, 0.0f, lbl_8040AF1C, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 0, 0, 1.5f, 2, 0, 0, lbl_80408A54);
            break;
        case 5:
            fn_8007B298(lbl_80408B78, 0.0f, lbl_8040AF1C, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 0, 0, 1.5f, 2, 0, 0, lbl_80408A54);
            break;
        case 6:
            fn_8007B298(lbl_80408B84, 0.0f, lbl_8040AF1C, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 0, 0, 1.5f, 2, 0, 0, lbl_80408A54);
            break;
        }
        sprintf(buf, lbl_80408B40, lbl_80408A68, lbl_80408C50);
        fn_8007B298(buf, 0.0f, 0.25f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
        fn_8007B298(lbl_80408B0C, 0.0f, -0.3f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 127, 0, 1.5f, 3, 0, 0, lbl_80408A54);
        fn_800769B4(arg0, lbl_80408AB0, lbl_80408A57);
        fn_800769B4(arg0, lbl_80408B18, lbl_80408A57);
        if (lbl_80408A3C == 2) {
            fn_800769B4(arg0, lbl_80408B80, lbl_80408A57);
        }
    }
}
