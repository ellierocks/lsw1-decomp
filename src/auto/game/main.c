#include "types.h"

extern s32 lbl_80408E84;
extern s32 lbl_8040A624;

void fn_8009CF2C(void)
{

}

void fn_8009CFF0()
{

}

void fn_8009CFF4()
{

}

s32 fn_8009CFF8(void)
{
    return 0;
}

s32 fn_8009D158(void)
{
    return lbl_8040A624;
}

void fn_8009D238(void)
{

}

s32 fn_8009D414(void)
{
    return lbl_80408E84;
}

void fn_8009D540(void)
{
    lbl_80408E84 = (lbl_80408E84 + -1);
}

s32 fn_800A1F94(void)
{
    return 0;
}

extern f32 lbl_8019EA00[];
extern f32 lbl_8019EA04[];
extern f32 lbl_8019EA08[];
extern f32 lbl_80408E9C;
extern f32 lbl_80408F00;
extern f32 lbl_80408F04;
extern s32 lbl_80408F08;
extern s32 lbl_80408F14;

void fn_8009DC18(void) {
    f32 temp_f0;
    f32 temp_f11;
    s32 var_r0;

    temp_f11 = lbl_80408F00;
    temp_f0 = (lbl_80408F04 * lbl_80408E9C) + temp_f11;
    lbl_80408F00 = temp_f0;
    if (temp_f0 > lbl_8019EA00[0]) {
        lbl_80408F00 = lbl_8019EA00[0];
    } else if (temp_f0 < lbl_8019EA04[0]) {
        lbl_80408F00 = lbl_8019EA04[0];
    }
    if ((temp_f11 < lbl_8019EA00[0]) && (lbl_80408F00 == lbl_8019EA00[0])) {
        var_r0 = 1;
        goto block_9;
    }
    if ((s32) lbl_80408F08 != 0) {
        var_r0 = lbl_80408F08 - 1;
block_9:
        lbl_80408F08 = var_r0;
    }
    if ((lbl_80408F00 == lbl_8019EA04[0]) || (lbl_80408F00 == lbl_8019EA00[0])) {
        lbl_80408F04 = lbl_8019EA08[0];
    }
    if ((lbl_80408F00 == lbl_8019EA04[0]) && (temp_f11 > lbl_8019EA04[0])) {
        lbl_80408F14 = 0;
    }
}

s32 fn_8009ED84(s32);
extern s32 lbl_80408FA0;

void fn_800A1BD4(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8009ED84(var_r31);
        var_r31 += 1;
    } while (var_r31 <= 1);
    lbl_80408FA0 = 0;
}

void *fn_80024C18(s32);
s32 fn_80024CD4(void *);
extern void *lbl_8040AF8C;

void fn_800A1F9C(void) {
    void *temp_r3;

    temp_r3 = fn_80024C18(1);
    lbl_8040AF8C = temp_r3;
    if (temp_r3 != NULL) {
        (*(s8 *)((s8 *)(temp_r3) + (0xBE))) = 0;
        (*(s8 *)((s8 *)(lbl_8040AF8C) + (0xB8))) = 0;
        (*(s8 *)((s8 *)(lbl_8040AF8C) + (0xC0))) = 1;
        fn_80024CD4(lbl_8040AF8C);
    }
}

s32 fn_800CA018(s32);
extern s32 lbl_80408E84;

void fn_8009D550(void)
{
  s32 new_var;
  s32 temp_r9;
  temp_r9 = lbl_80408E84 + 1;
  lbl_80408E84 = temp_r9;
  fn_800CA018(new_var);
  new_var = temp_r9;
}

void *fn_80024BF0(s32);
void *fn_80024C18(s32);
s32 fn_80024CD4(void *);
extern void *lbl_8040AF68;
extern void *lbl_8040AF6C;
extern void *lbl_8040AF70;

void fn_8009D9D8(void)
{
  void *temp_r3;
  void *temp_r3_2;
  void *temp_r3_3;
  temp_r3 = fn_80024BF0(1);
  lbl_8040AF68 = temp_r3;
  *((s8 *) (((s8 *) temp_r3) + 0xBE)) = 3;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xB9)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xBF)) = 2;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xBB)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xBC)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xC0)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF68) + 0xB8)) = 3;
  fn_80024CD4(lbl_8040AF68);
  temp_r3_2 = fn_80024BF0(1);
  lbl_8040AF6C = temp_r3_2;
  *((s8 *) (((s8 *) temp_r3_2) + 0xBE)) = 3;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xB9)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xBF)) = 2;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xBB)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xBC)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xC0)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF6C) + 0xB8)) = 1;
  fn_80024CD4(lbl_8040AF6C);
  temp_r3_3 = fn_80024C18(1);
  lbl_8040AF70 = temp_r3_3;
  *((s8 *) (((s8 *) temp_r3_3) + 0xBE)) = 3;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xB9)) = 1;
  *(((s8 *) lbl_8040AF70) + 0xBF) = 2;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xBB)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xBC)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xC0)) = 1;
  *((s8 *) (((s8 *) lbl_8040AF70) + 0xB8)) = 1;
  temp_r3_3 = lbl_8040AF70;
  *((s16 *) (((s8 *) temp_r3_3) + 0xF8)) = 0x64;
  fn_80024CD4(temp_r3_3);
}

s32 fn_800092F0(void *, void *, void *, s32);

void fn_8009D504(void *arg0) {
    s32 sp8;

    sp8 = (*(s32 *)((s8 *)(arg0) + (0x14)));
    fn_800092F0((s8 *)arg0 + 0x18, *(void **)((s8 *)arg0 + 0x10), &sp8, 0);
}

s32 fn_8009EC80(s32);
s32 fn_800B2ED4(void);
s32 fn_8011EAA8(s32);
extern f32 lbl_8019EAEC[];
extern s32 lbl_803D9A58[];
extern u8 lbl_803E028C[];
extern s32 lbl_80408EA8;
extern f32 lbl_80408F00;
extern s32 lbl_80408FE0;
extern s32 lbl_80409118;
extern s32 lbl_8040912C;
extern s32 lbl_8040913C;
extern s32 lbl_804093B0;
extern void *lbl_80409410;
extern s32 lbl_8040AFA8;
extern s32 lbl_8040AFAC;
extern s32 lbl_8040AFB0;
extern s32 lbl_8040AFB4;

void fn_8009F0B8(s32 arg0)
{
  s32 temp_r3;
  s32 temp_r9;
  void *new_var;
  s32 var_r0;
  void *temp_r9_2;
  void *temp_r9_3;
  if (((s32) lbl_8040912C) == 0)
  {
    temp_r9 = arg0 * 0x5C;
    new_var = (s8 *) lbl_803D9A58;
    if (((((((s32) (*((u8 *) (((s8 *) (temp_r9 + new_var)) + 0x24)))) == 0) || ((temp_r9 = (*((s32 *) (((s8 *) (*((s32 *) (new_var + temp_r9)))) + 0x6C))) & 0x20000000, (temp_r9 == 0) == 0))) || (((s32) lbl_80408FE0) != 0)) || ((temp_r3 = fn_800B2ED4(), (temp_r3 == 1) != 0))) || ((var_r0 = 1, (temp_r3 == 0x1A) != 0)))
    {
      var_r0 = 0;
    }
    new_var = (void *) 0;
    if (((((((((((((var_r0 != 0) && (((s32) lbl_8040913C) == (-1))) && (((s32) lbl_804093B0) == (-1))) && (lbl_80408F00 == lbl_8019EAEC[0])) && (((s32) lbl_80408EA8) == 0)) && (((s32) lbl_80409118) == 0)) && (((s32) lbl_8040AFA8) == 0)) && ((((s32) (*((s32 *) (((s8 *) lbl_80409410) + 0x19C)))) != 0x15) || (((s32) lbl_803E028C[0]) != 5))) && (((temp_r9_2 = *((void **) (((s8 *) lbl_80409410) + 0x1AC)), (temp_r9_2 == new_var) != 0)) || (!((*((u8 *) (((s8 *) temp_r9_2) + 0x32))) & 4)))) && (((temp_r9_3 = *((void **) (((s8 *) lbl_80409410) + 0x1A8)), (temp_r9_3 == new_var) != 0)) || (!((*((s32 *) (((s8 *) (*((void **) (((s8 *) lbl_80409410) + 0x1A8)))) + 4))) & 0x1000)))) && (fn_800B2ED4() == (-1))) && ((((s32) lbl_8040AFAC) == 0) || (fn_8011EAA8(lbl_8040AFB0) != 0))) && (((s32) lbl_8040AFB4) == 0))
    {
      fn_8009EC80(arg0);
    }
  }
}

void Menu_ResetStack(void);
s32 fn_8006DCD4(void *, void *);
void fn_8006DF18(void);
void fn_8006E184(void);
void fn_80078E90(void *);
extern s32 gMenuDepth;
extern s32 gMenuStack[];
extern s32 lbl_8019EACC[];
extern s32 lbl_8040912C;

void Menu_CleanupAndReset(void) {
    lbl_8040912C = 0;
    fn_80078E90((gMenuDepth * 0xE0) + ((s8 *)gMenuStack));
    Menu_ResetStack();
    fn_8006DF18();
    fn_8006E184();
    fn_8006DCD4(lbl_8019EACC, 0);
}

void AXFXSetHooks(s32, s32, f32);
void NuRndrFootPrints(s32);
s32 fn_800274BC(s32);
void fn_80027564(void);
void fn_8009E6FC(void);
extern f32 lbl_8019EB90[];
extern s32 lbl_80408ED0;

void fn_800A1C70(s32 arg0) {
    s32 var_r31;

    var_r31 = arg0;
    do {
        if ((s32) lbl_80408ED0 != 0) {
            fn_8009E6FC();
        } else if (fn_800274BC(-1) != 0) {
            AXFXSetHooks(0xB00, 0, lbl_8019EB90[0]);
            fn_80027564();
        }
        NuRndrFootPrints(-1);
        var_r31 -= 1;
    } while (var_r31 > 0);
    lbl_80408ED0 = 0;
}

void fn_8002A3A8(s32);

void fn_800A1F54(void *arg0) {
    if (*(s32 *)((s8 *)arg0 + 0x1C0) != 0) {
        fn_8002A3A8(*(s32 *)((s8 *)arg0 + 0x1C0));
    }
    *(s32 *)((s8 *)arg0 + 0x1C0) = 0;
}

extern s32 lbl_8019E714[];
extern s32 lbl_80408E88;
extern s32 lbl_80408F8C;

void fn_8009D57C(void) {
    volatile s32 sp8;
    s32 spC;

    lbl_80408E84++;
    spC = lbl_80408F8C - 0x100000;
    sp8 = lbl_80408F8C;
    lbl_80408E88 = fn_800092F0(lbl_8019E714, &spC, (s32 *)&sp8, 4);
}

s32 TerrainInitEx(s32, void *, s32, s32, void *, s32, s32, s32, s32, s32);
s32 fn_8005A3F8(s32, void *, s32, s32, void *, s32, s32);

void fn_800A1D44(void *arg0) {
    s32 type;
    s32 var_r3;

    *(s32 *)((s8 *)arg0 + 0x1C4) = 0;
    if (*(s32 *)((s8 *)*(void **)((s8 *)arg0 + 0x1A8) + 4) & 8) {
        *(s32 *)((s8 *)arg0 + 0x184) = (*(s32 *)((s8 *)arg0 + 0x184) + 0x3F) & ~0x3F;
        type = *(s32 *)((s8 *)arg0 + 0x198);
        if (type == 0x21) {
            var_r3 = TerrainInitEx(type, (s8 *)arg0 + 0x184, *(s32 *)((s8 *)arg0 + 0x188), 0, (s8 *)arg0 + 0x80, *(s32 *)((s8 *)arg0 + 0x1C0), 0, 0x100, 0x100, 0);
        } else {
            var_r3 = fn_8005A3F8(type, (s8 *)arg0 + 0x184, *(s32 *)((s8 *)arg0 + 0x188), 0, (s8 *)arg0 + 0x80, *(s32 *)((s8 *)arg0 + 0x1C0), 0);
        }
        *(s32 *)((s8 *)arg0 + 0x1C4) = var_r3;
    }
}

void fn_8002823C(s32, s32, s32, f32, f32);
extern f32 lbl_8019EB28[];
extern s32 lbl_804093C8;

void fn_800A039C(void) {
    s32 temp_r11;
    s32 temp_r9;
    void *temp_r10;

    if ((void *) lbl_80409410 != NULL) {
        temp_r10 = (*(void **)((s8 *)(lbl_80409410) + (0x1A8)));
        temp_r11 = (*(s32 *)((s8 *)(temp_r10) + (4)));
        if (!(temp_r11 & 0x4000) && ((s32) lbl_804093C8 != -1) && ((s32) (temp_r11 & 0x44000) == 0x40000) && (((s32) lbl_8040AFAC == 0) || !((*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_8040AFB0) + (8)))) + (0x3C))) & 0x40))) {
            temp_r9 = (*(u8 *)((s8 *)(temp_r10) + (0x59))) << 8;
            fn_8002823C(((*(u8 *)((s8 *)(temp_r10) + (0x5B))) << 0x18) | ((*(u8 *)((s8 *)(temp_r10) + (0x5A))) << 0x10) | temp_r9 | (*(u8 *)((s8 *)(temp_r10) + (0x58))), 0, 0, (*(f32 *)((s8 *)(temp_r10) + (0x50))), (*(f32 *)((s8 *)(temp_r10) + (0x54))));
            return;
        }
    }
    fn_8002823C(0, 0, 0, lbl_8019EB28[0], lbl_8019EB28[0]);
}

void NuPortalInit();
void NuPortalVisibility(void *);
void fn_8002933C(void *);
void fn_80043328();
void fn_80043D8C();
void fn_80067114(void *);
void memset();
extern s8 lbl_802D9F18[];
extern s8 *lbl_80408F94;
extern s32 lbl_8040AF9C;

typedef struct PortalEnt {
    u8 pad[0x10];
    u32 b0 : 2;
    u32 vis : 1;
    u32 rest : 29;
    u8 pad2[4];
} PortalEnt;

void fn_8009EB80(void) {
    s32 i;
    s32 n;
    void *temp_r31;
    PortalEnt *e;
    s8 *out;

    if (*(void **)((s8 *)lbl_80409410 + 0x1C0) != NULL) {
        if (lbl_8040AF9C == 0) {
            NuPortalInit();
            NuPortalVisibility(*(void **)((s8 *)lbl_80409410 + 0x1C0));
            temp_r31 = *(void **)((s8 *)lbl_80409410 + 0x1C0);
            lbl_802D9F18[0] = 1;
            memset(lbl_80408F94, 0, 0x100);
            if (temp_r31 != NULL) {
                n = *(s32 *)((s8 *)temp_r31 + 0x90);
                if (n > 0) {
                    e = *(PortalEnt **)((s8 *)temp_r31 + 0x94);
                    out = lbl_80408F94;
                    for (i = 0; i < n; i++) {
                        if (e->vis) {
                            *out = 1;
                        } else {
                            *out = e->vis;
                        }
                        e++;
                        out++;
                    }
                }
            }
        }
        fn_8002933C(*(void **)((s8 *)lbl_80409410 + 0x1C0));
        fn_80043D8C(*(void **)((s8 *)lbl_80409410 + 0x1C0));
        fn_80043328(*(void **)((s8 *)lbl_80409410 + 0x1C0));
        fn_80067114(*(void **)((s8 *)lbl_80409410 + 0x1C0));
    }
}

void OSResumeThread(s32 *);
void OSSuspendThread(s32 *);
extern s32 lbl_802D56F8[];
extern s32 lbl_802D5A10[];
s32 fn_8009D4CC(void) {
    u8 buf[0x28];
    OSResumeThread(lbl_802D56F8);
    OSSuspendThread(lbl_802D5A10);
    return 0;
}

s32 NuFileExists(void *);
s32 fn_80065C14(char *, s32);
void fn_8006F8A8();
int sprintf(char *, const char *, ...);
extern char lbl_8019EAB4[];

void LoadObjectAnimFile(void *arg0) {
    char buf[0x100];

    if (*(s32 *)((s8 *)arg0 + 0x5E8) == -1) {
        sprintf(buf, lbl_8019EAB4, (s8 *)arg0 + 0x80);
        if (NuFileExists(buf) != 0) {
            fn_8006F8A8(1);
            *(s32 *)((s8 *)arg0 + 0x5E8) = fn_80065C14(buf, *(s32 *)((s8 *)arg0 + 0x1C0));
            fn_8006F8A8(0);
        }
    }
}

s32 NuFileExists(void *);
void fn_80068F14(char *);
void fn_800698B8();
int sprintf(char *, const char *, ...);
extern char lbl_8019EAAC[];
extern s32 lbl_803E04F0[];

void LoadObjectFile(void *arg0) {
    char buf[0x100];

    fn_800698B8();
    if (*(s32 *)((s8 *)*(void **)((s8 *)arg0 + 0x1A8) + 4) & 0x8000) {
        sprintf(buf, lbl_8019EAAC, (s8 *)arg0 + 0x80);
        if (NuFileExists(lbl_803E04F0) != 0) {
            fn_80068F14(buf);
        }
    }
}

void fn_80037170(void);
extern f32 lbl_8019EB94[];
extern f32 lbl_8040A63C;

void fn_800A1CEC(void) {
    u8 order[16] = {4, 2, 5, 3, 0, 6, 1, 7, 5, 3, 4, 2, 1, 7, 0, 6};

    fn_80037170();
    lbl_8040A63C = lbl_8019EB94[0];
}

extern s32 lbl_8040AFC8;
extern s32 lbl_8040AFCC;
extern s32 lbl_8040AFD0;
extern s32 lbl_8040AFD4;

s32 fn_8009FAD8(void) {
    s32 moved;

    moved = 0;
    {
        s32 d = lbl_8040AFC8 - lbl_8040AFD0;
        if (d != 0) {
            d /= 4;
            if (d != 0) {
                lbl_8040AFD0 += d;
            } else if (lbl_8040AFD0 < lbl_8040AFC8) {
                lbl_8040AFD0++;
            } else if (lbl_8040AFD0 > lbl_8040AFC8) {
                lbl_8040AFD0--;
            }
            moved = 1;
        }
    }
    {
        s32 d = lbl_8040AFCC - lbl_8040AFD4;
        if (d != 0) {
            d /= 4;
            if (d != 0) {
                lbl_8040AFD4 += d;
            } else if (lbl_8040AFD4 < lbl_8040AFCC) {
                lbl_8040AFD4++;
            } else if (lbl_8040AFD4 > lbl_8040AFCC) {
                lbl_8040AFD4--;
            }
            moved = 1;
        }
    }
    return moved;
}

s32 (*fn_80009970(s32 *, s32))();
s32 fn_80014EC0(s32, s32 *, s32 *, s32);
extern s32 lbl_8019E9B8[];
extern s32 lbl_801BC904[];
extern s32 lbl_80408F6C;
extern s32 lbl_80408F70;
extern s32 str_AnimatedTextureScript_LoadFailed[];

void fn_800A1FF0(void) {
    s32 sp8;
    s32 *p;

    p = lbl_801BC904;
    while (*p != 0) {
        sp8 = lbl_80408F70;
        {
            s32 *a = &lbl_80408F6C;
            *a = (*a + 0x3F) & ~0x3F;
        }
        if (fn_80014EC0(*p, &lbl_80408F6C, &sp8, 0x3C) == 0) {
            fn_80009970(lbl_8019E9B8, 0xC0F)(str_AnimatedTextureScript_LoadFailed, *p);
        }
        p++;
    }
    {
        s32 *a = &lbl_80408F6C;
        *a = (*a + 0xF) & ~0xF;
    }
}

s32 OSCreateThread(void *, void *, s32, void *, s32, s32, s32);
void OSInitMessageQueue(s32 *, s32 *, s32);
void OSInitSemaphore(s32 *, s32);
void OSResumeThread(s32 *);
s32 fn_8009D41C(void);
s32 fn_8009D4CC(void);
extern s32 lbl_802D4EF7[];
extern s32 lbl_802D56F7[];
extern s32 lbl_802D5D28[];
extern s32 lbl_802D5D48[];
extern s32 lbl_802D5D54[];
extern s32 lbl_80408E78;

void fn_8009D23C(void) {
    s32 i;

    if (lbl_80408E78 == 0) {
        lbl_80408E78 = 1;
        OSCreateThread(lbl_802D56F8, fn_8009D41C, 0, lbl_802D4EF7, 0x10000, 0x14, 0);
        OSCreateThread(lbl_802D5A10, fn_8009D4CC, 0, lbl_802D56F7, 0x800, 0x14, 0);
        OSInitMessageQueue(lbl_802D5D28, lbl_802D5D54, 0x10);
        OSInitSemaphore(lbl_802D5D48, 0x10);
        OSResumeThread(lbl_802D5A10);
        for (i = 15; i >= 0; i--) {
            lbl_802D5D54[i] = i;
        }
    }
}

void OSSendMessage(void *, void *, s32);
void OSWaitSemaphore(void *);
void memcpy();
extern s32 lbl_802D5D94[];
extern s32 lbl_80408E7C;
extern s32 lbl_80408E80;

typedef struct NetMsg {
    s32 a;
    s32 b;
    u32 flags;
    s32 pad;
    u8 data[0x408];
} NetMsg;

void *fn_8009D160(s32 arg0, s32 arg1, void *src, s32 len) {
    NetMsg *m;
    void *tok;

    OSWaitSemaphore(lbl_802D5D48);
    m = &((NetMsg *)lbl_802D5D94)[lbl_80408E7C];
    tok = lbl_802D5D54[lbl_80408E7C];
    lbl_80408E80++;
    if (++lbl_80408E7C > 15) {
        lbl_80408E7C = 0;
    }
    m->a = arg0;
    m->b = arg1;
    m->flags &= 0x3FFFFFFF;
    if (src != NULL && len != 0) {
        memcpy(m->data, src, len);
    }
    OSSendMessage(lbl_802D5D28, tok, 0);
    return m;
}

static inline void *PostMsg(void *a, void *b, void *src, s32 len) {
    NetMsg *m;
    void *tok;

    OSWaitSemaphore(lbl_802D5D48);
    m = &((NetMsg *)lbl_802D5D94)[lbl_80408E7C];
    tok = lbl_802D5D54[lbl_80408E7C];
    lbl_80408E80++;
    if (++lbl_80408E7C > 15) {
        lbl_80408E7C = 0;
    }
    m->a = (s32)a;
    m->b = (s32)b;
    m->flags &= 0x3FFFFFFF;
    if (src != NULL && len != 0) {
        memcpy(m->data, src, len);
    }
    OSSendMessage(lbl_802D5D28, tok, 0);
    return m;
}

void fn_8009D000(void) {
    PostMsg(fn_8009D550, fn_8009D540, NULL, 0);
}

void fn_8009D0AC(void) {
    PostMsg(fn_8009D57C, fn_8009D540, NULL, 0);
}

void LoadWorldInfo(void *);
s32 NuHGobjEvalAnim(void *, s32);
typedef struct LevData { u8 pad[0x32]; u32 hi : 4; u32 flag : 1; u32 lo : 3; u8 pad2[0x35]; } __attribute__((packed)) LevData;
extern LevData lbl_801DC074[];
extern u8 lbl_803DB158[];
extern s32 lbl_804093BC;
extern void *lbl_80409414;
extern s32 lbl_8040AF60;
extern s32 lbl_8040AF64;

void fn_8009CF30(void) {
    if (lbl_80409414 == lbl_80409410) {
        s32 i = lbl_804093C8;
        s32 ok = 1;
        if (i != -1) {
            LevData *p = &lbl_801DC074[i];
            ok = !p->flag;
        }
        if (ok) {
            u8 *w = lbl_803DB158;
            if (lbl_80409414 == w) {
                w += 0x1E18;
            }
            lbl_80409414 = w;
        }
    }
    if (NuHGobjEvalAnim(lbl_80409414, lbl_8040AF60) != 0) {
        lbl_8040AF64 = lbl_8040AF60;
        LoadWorldInfo(lbl_80409414);
    }
    {
        s32 *cur = lbl_80409414;
        lbl_8040AF64 = -1;
        if (cur[0x64] != 0) {
            lbl_804093BC = 1;
        }
    }
}

/* The 0xF00 calls also pass a third int argument in r5. */
#define AXFXSetHooks4 ((void (*)(s32, s32, s32, f32))AXFXSetHooks)
void NuCameraCalcFrustrumPlanes(void *);
void fn_80027668(s32, s32, s32, f32);
void fn_800B4A88(f32);
typedef struct Mtx44Blk { s32 m[16]; } Mtx44Blk;
extern f32 lbl_8019EB24[];
extern Mtx44Blk lbl_801B0F08;
extern s32 lbl_8040A628;
extern s32 lbl_8040A62C;
extern Mtx44Blk *lbl_8040AF5C;
extern f32 lbl_8040AF74;

void fn_800A02A0(void) {
    *lbl_8040AF5C = lbl_801B0F08;
    NuCameraCalcFrustrumPlanes(lbl_8040AF5C);
    if (fn_800274BC(-1) != 0) {
        if (lbl_8040A628 == lbl_8040A62C) {
            AXFXSetHooks4(0xF00, lbl_8040A62C, lbl_8040A62C, lbl_8019EB24[0]);
        } else {
            fn_80027668(0xF00, lbl_8040A628, lbl_8040A62C, lbl_8019EB24[0]);
        }
        if (lbl_8040AF74 != lbl_8019EB24[0]) {
            fn_800B4A88(lbl_8019EB24[0] - lbl_8040AF74);
        }
        fn_80027564();
    }
}

void NuTexReadBitmapMMEx(s32);
void fn_800140EC(s32);
void fn_80014180(s32, s32);
s32 fn_800274BC(s32);
void fn_80027564();
void fn_800275E0(s32);
void fn_80027830();
void fn_80027C00(s32, s32 *);
void fn_80027C54(s32, s32 *, s32 *);
void fn_800282E0();
void fn_8005D6E4();
void fn_8005D8BC();
void fn_80060F58(s32, s32);
void fn_80064984();
void fn_8006EB40();
void fn_80074374(s32);
void fn_8009EB80();
void fn_800A039C();
void fn_800A2498();
void fn_800A2A9C();
void fn_800A8FF4();
void fn_800ABDAC();
void fn_800AFF54();
void fn_800B4974();
void fn_800B4A88(f32);
void fn_800B7F70();
void fn_800B9BB8();
void fn_800BA434();
void fn_800CBD6C();
void fn_800CC9CC();
void fn_800EBD40();
void fn_800EBF2C(void *);
void fn_800F9374();
void fn_8010ACC0();
void fn_8010BC24(s32);
void fn_8011E1EC();
void fn_80121724(void *);
void fn_801258B4();
void fn_801259EC(void *);
void fn_80126C6C();
void fn_8013EA90(void *, s32, s32);
void fn_8013FA74();
void fn_8013FB08();
void fn_801410F4(s32);
extern s32 MenuHeader[];
extern f32 lbl_8019EB2C[];
extern f32 lbl_8019EB30[];
extern s32 lbl_801BC964[];
extern s32 lbl_801BC9E4[];
extern s32 lbl_8038E6FC[];
extern s32 lbl_803E04C4[];
extern s32 lbl_80408EB4;
extern f32 lbl_80408FE4;
extern f32 lbl_80408FE8;
extern f32 lbl_80408FEC;
extern s32 lbl_80409698;
extern f32 lbl_80409874;
extern s32 lbl_804098D8;
extern f32 lbl_8040A640;
extern s32 lbl_8040AF78;
extern s32 lbl_8040AF7C;
extern s32 lbl_8040AFE0;
extern s32 *lbl_8040AFE4;
extern f32 lbl_8040AFE8;
extern s32 lbl_8040AFEC;

void fn_800A0474(void) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f13;
    s32 temp_r11;
    s32 temp_r3;
    s32 var_r31;

    fn_800A039C();
    fn_8013FA74();
    fn_800B9BB8();
    lbl_8040AFE4 = MenuHeader;
    if ((s32) lbl_8040AFE0 == 0) {
        fn_800A8FF4(1);
    }
    temp_r3 = fn_800274BC(-1);
    if (temp_r3 != 0) {
        if (lbl_8040A628 == lbl_8040A62C) {
            AXFXSetHooks4(0xF00, lbl_8040A62C, lbl_8040A62C, lbl_8019EB2C[0]);
        } else {
            fn_80027668(0xF00, lbl_8040A628, lbl_8040A62C, lbl_8019EB2C[0]);
        }
        fn_801259EC(lbl_80409410);
    }
    if ((s32) lbl_8040AFE0 == 0) {
        fn_80074374(lbl_8040912C);
        if ((s32) lbl_8040AFE0 == 0) {
            fn_800ABDAC();
        }
    }
    fn_8013FA74();
    if (((s32) lbl_80408EA8 == 0) && ((s32) lbl_8040912C == 0) && ((s32) lbl_8040AFAC == 0)) {
        if ((s32) lbl_8040AFE0 == 0) {
            fn_800A2498();
            fn_800F9374();
            goto block_15;
        }
    } else {
block_15:
        if ((s32) lbl_8040AFE0 == 0) {
            fn_800A2A9C();
        }
    }
    if (((s32) (*(s32 *)((s8 *)(lbl_80409410) + (0x19C))) == 3) && ((s32) lbl_8040AFE0 == 0)) {
        temp_f13 = lbl_8040AFE8 / lbl_80409874;
        if (lbl_8040A640 > temp_f13) {
            lbl_8040A640 = lbl_8040A640 - (2.0f * lbl_80408E9C);
            if (lbl_8040A640 < temp_f13) {
                lbl_8040A640 = temp_f13;
            }
        } else if (lbl_8040A640 < temp_f13) {
            lbl_8040A640 = lbl_8040A640 + (2.0f * lbl_80408E9C);
            if (lbl_8040A640 > temp_f13) {
                lbl_8040A640 = temp_f13;
            }
        }
        fn_8009CFF0(lbl_80408FE8 * lbl_8040A640, lbl_80408FEC * lbl_8040A640);
        if (lbl_80408FE4 > lbl_8019EB30[0]) {
            fn_8009CFF4(0, lbl_80408FE4);
        }
    }
    if ((s32) lbl_80408EB4 != 0) {
        var_r31 = lbl_8040AF78;
    } else {
        var_r31 = lbl_8040912C;
    }
    if (((s32) (*(s32 *)((s8 *)(lbl_80409410) + (0x774))) != 0) && (((s32) lbl_8040AFE0 == 0) || ((s32) lbl_8040AFEC != 0))) {
        fn_800B4974();
    }
    if (temp_r3 != 0) {
        if ((s32) lbl_8040AFE0 == 0) {
            fn_8009EB80();
            fn_801258B4();
            fn_800CC9CC();
            fn_800CBD6C();
            fn_80126C6C();
            fn_8010ACC0();
            fn_800B7F70();
            fn_800BA434();
            fn_800EBF2C(lbl_80409410);
            fn_800EBD40();
            fn_8013EA90(lbl_80409410, (*(s32 *)((s8 *)(lbl_80409410) + (0x71C))), (*(s32 *)((s8 *)(lbl_80409410) + (0x734))));
            fn_8013EA90(lbl_80409410, (*(s32 *)((s8 *)(lbl_80409410) + (0x720))), (*(s32 *)((s8 *)(lbl_80409410) + (0x738))));
            fn_8013EA90(lbl_80409410, (*(s32 *)((s8 *)(lbl_80409410) + (0x724))), (*(s32 *)((s8 *)(lbl_80409410) + (0x73C))));
            fn_8013EA90(lbl_80409410, (*(s32 *)((s8 *)(lbl_80409410) + (0x72C))), (*(s32 *)((s8 *)(lbl_80409410) + (0x744))));
            fn_8013FB08();
            fn_8013EA90(lbl_80409410, (*(s32 *)((s8 *)(lbl_80409410) + (0x730))), (*(s32 *)((s8 *)(lbl_80409410) + (0x748))));
            fn_8013FA74();
            fn_8013EA90(lbl_80409410, (*(s32 *)((s8 *)(lbl_80409410) + (0x718))), 0x40);
            fn_80121724(lbl_80409410);
            fn_80027C54(lbl_8038E6FC[0], lbl_801BC964, lbl_801BC9E4);
        }
        fn_8011E1EC();
        NuTexReadBitmapMMEx(lbl_8040AF7C);
        fn_800282E0(var_r31);
        temp_r11 = (*(s32 *)((s8 *)(lbl_80409410) + (0x198)));
        if ((temp_r11 == 0x7C) || ((*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (4))) & 0x4000) || (temp_r11 == 0x7D)) {
            fn_800B4A88(lbl_8019EB2C[0]);
        }
        if (lbl_8040AF74 != lbl_8019EB2C[0]) {
            fn_800B4A88(lbl_8019EB2C[0] - lbl_8040AF74);
        }
        if ((s32) lbl_80408EA8 != 0) {
            fn_800140EC(lbl_80409698);
            fn_80014180(0xC, 0xC);
            fn_8006EB40();
        }
        fn_80027830();
        fn_80060F58(var_r31 ^ lbl_80408EA8, 1);
        fn_801410F4(lbl_804098D8);
        fn_8010BC24(var_r31);
        fn_80027564();
        fn_80027C00(var_r31, lbl_803E04C4);
        fn_800274BC(-1);
        fn_8005D8BC();
        fn_8005D6E4();
        fn_80060F58(var_r31 ^ lbl_80408EA8, 0);
        fn_800275E0(0);
        fn_80064984();
        if ((s32) lbl_80408EB4 == 0) {
            fn_800AFF54();
        }
    }
}

void fn_80027690(s32, s32, s32, s32, f32, f32, f32, f32, u32 *, s32);
void fn_80027868(s32, s32, s32, s32, f32, f32, f32, f32, u32, s32);
extern f32 lbl_8019EA8C[];
extern f32 lbl_8019EA90[];
extern f32 lbl_8019EA94[];
extern s32 lbl_80408F40;
extern s32 lbl_80408F44;
extern s32 lbl_80408F48;
extern s32 lbl_80408F4C;
extern s32 lbl_8040A630;
extern s32 lbl_8040AD20;
extern s32 lbl_8040AD24;

void fn_8009E6FC(void) {
    u32 col[4];

    if (fn_800274BC(-1) != 0) {
        if (lbl_8040AF74 < lbl_8019EA8C[0]) {
            u32 c = ((s32)(lbl_8040AF74 * lbl_8019EA90[0]) << 24) | 0x808080;
            col[0] = c;
            col[1] = c;
            col[2] = c;
            col[3] = c;
            fn_80027690(lbl_80408F40, lbl_80408F44, lbl_80408F48, lbl_80408F4C, lbl_8019EA94[0], lbl_8019EA94[0], lbl_8019EA8C[0], lbl_8019EA8C[0], col, lbl_8040A630);
        } else {
            fn_80027868(0, 0, lbl_8040AD20 * 16, lbl_8040AD24 * 16, lbl_8019EA94[0], lbl_8019EA94[0], lbl_8019EA8C[0], lbl_8019EA8C[0], 0x80808080, lbl_8040A630);
        }
        fn_80027564();
    }
}

typedef struct AreaEnt { s32 pad; s32 flags; u8 pad2[0x3C]; s8 lev; u8 pad3[0x63]; } AreaEnt;
extern AreaEnt lbl_801D68E0[];
extern s32 lbl_804093AC;
extern s32 lbl_80408F30;
extern s32 lbl_80408F18;
extern s32 lbl_80408ED4;

void fn_8009E620(void) {
    s32 lev;
    LevData *p;

    lbl_80408F30 = 0;
    lev = lbl_801D68E0[lbl_804093B0].lev;
    if ((lbl_801D68E0[lbl_804093AC].flags & 0x4000) && lbl_804093B0 != 0x7B
        && (lev == -1 || (p = &lbl_801DC074[lev], !(((u8 *)p)[0x32] & 4)))) {
        lbl_80408ED4 = 1;
        lbl_80408F18 = 1;
        return;
    }
    if (lbl_8040913C == -1 && lbl_804093B0 != -1) {
        if (lbl_801D68E0[lbl_804093AC].flags & 0x4000) {
            if (lbl_804093B0 != 0x7B) {
                lbl_80408F30 = 1;
            }
        } else if (lev != -1 && lev == lbl_804093C8) {
            lbl_80408F30 = 1;
        }
    }
}

s32 OSReceiveMessage(void *, void *, s32);
s32 OSSignalSemaphore(void *);
typedef struct NetJob {
    void (*volatile run)(struct NetJob *);
    void (*volatile done)(struct NetJob *);
    u8 state;
    u8 pad1[3];
    s32 pad;
    u8 data[0x408];
} NetJob;

s32 fn_8009D41C(void) {
    s32 idx;

    for (;;) {
        OSReceiveMessage(lbl_802D5D28, &idx, 1);
        lbl_8040A624 = (s32)&((NetJob *)lbl_802D5D94)[idx];
        if ((NetJob *)lbl_8040A624 != NULL) {
            ((NetJob *)lbl_8040A624)->state |= 0x80;
            if (((NetJob *)lbl_802D5D94)[idx].run != NULL) {
                ((NetJob *)lbl_802D5D94)[idx].run((NetJob *)lbl_8040A624);
            }
            if (((NetJob *)lbl_8040A624)->done != NULL) {
                ((NetJob *)lbl_8040A624)->done((NetJob *)lbl_8040A624);
            }
            lbl_8040A624 = 0;
        }
        OSSignalSemaphore(lbl_802D5D48);
        lbl_80408E80--;
    }
}
