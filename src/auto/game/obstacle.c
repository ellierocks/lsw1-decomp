#include "types.h"

extern s32 lbl_80409700;
extern s32 lbl_8040B204;
extern s32 lbl_8040B20C;
extern s32 lbl_804096FC;
extern f32 lbl_801A9A44[];
extern f32 lbl_801A9A48[];
extern f32 lbl_80409798;
struct Lbl803FFE2CEntry {
    void *obj;     /* 0x00 */
    f32 pos[3][3]; /* 0x04 */
    f32 t[3];      /* 0x28 */
    f32 timer;     /* 0x34 */
};
extern struct Lbl803FFE2CEntry lbl_803FFE2C[];
extern s32 lbl_802E231C[];
extern s32 lbl_8040973C;
extern f32 lbl_8040A720;
extern f32 lbl_8040A724;
extern s32 lbl_8040B100;

void fn_800E32F8(s32 a)
{
    lbl_8040B100 = a;
}

void fn_800E631C(void)
{

}

void fn_801039D0(u8* a)
{
    *(s16*)(a + 78) = 1;
}

void fn_80103F64(void)
{
    lbl_8040973C = 0;
}

void fn_80103F70(f32 x, s32 a)
{
    lbl_8040973C = a;
    lbl_8040A720 = x;
    lbl_8040A724 = (x * x);
}

void fn_8010438C(void)
{
    lbl_802E231C[0] = 0;
}

void fn_800E6320(void)
{
    lbl_8040B204 = 0;
    lbl_8040B20C = 0;
    lbl_804096FC = 0;
    lbl_80409700 = 0;
}

void fn_80103EA8(u8* a)
{
    f32 k = lbl_801A9A44[0];
    f32 m = lbl_801A9A48[0];
    s32 z = 0;
    *(f32*)(a + 1856) = k;
    *(f32*)(a + 1860) = k;
    *(f32*)(a + 1928) = k;
    *(s32*)(a + 1696) = z;
    *(s32*)(a + 1700) = z;
    *(f32*)(a + 1864) = m;
}

void fn_8010403C(void)
{
    f32 k = lbl_80409798;
    lbl_803FFE2C[0].timer = k;
    lbl_803FFE2C[1].timer = k;
}

void fn_80108700(u8* a)
{
    *(s32*)(a + 16) = 0;
    *(s32*)(a + 20) = 0;
}

extern u8* lbl_80409410;
extern s32 NuStrICmp(const char*, const char*);

static inline u8* scene_find(u8* scene, const char* name)
{
    u8* entry = *(u8**)(scene + 0x238);
    s32 i;
    if (entry) {
        for (i = 0; i < *(s32*)(scene + 0x23c); i++) {
            if (NuStrICmp((char*)entry, name) == 0) return entry;
            entry += *(u16*)(entry + 0x1c);
        }
    }
    return 0;
}

void fn_80108710(const char* name, u8* entry)
{
    if (entry) *(u32*)(entry + 0x18) |= 8;
    else if (name) {
        entry = scene_find(lbl_80409410, name);
        if (entry) *(u32*)(entry + 0x18) |= 8;
    }
}

void fn_801087C4(const char* name, u8* entry)
{
    if (entry) *(u32*)(entry + 0x18) &= ~8;
    else if (name) {
        entry = scene_find(lbl_80409410, name);
        if (entry) *(u32*)(entry + 0x18) &= ~8;
    }
}

s32 NuMtxTranslate(s32 *, s32 *);
s32 fn_8000DD40(s32 *, u16);
s32 fn_8000E3C0(s32 *, u16);
s32 fn_8000E540(s32 *, u16);
extern f32 lbl_801A5D64[];
extern s32 lbl_801DD100[];
extern s32 lbl_802E18B4[];
extern s32 lbl_80408C88;
extern f32 lbl_80408CA0;
extern u16 lbl_804096B8;
extern u16 lbl_804096BA;
extern u16 lbl_804096BC;

void fn_800E3278(s32 arg0) {
    if ((s32) lbl_80408C88 != 0) {
        fn_8000DD40(lbl_802E18B4, lbl_804096B8);
        fn_8000E3C0(lbl_802E18B4, lbl_804096BA);
        fn_8000E540(lbl_802E18B4, lbl_804096BC);
        NuMtxTranslate(lbl_802E18B4, lbl_801DD100);
        if (arg0 != 0) {
            lbl_80408CA0 = lbl_801A5D64[0];
        }
    }
}

s32 fn_8006DCD4(s32 *, void *);
extern s32 lbl_801A5DE0[];
extern s32 lbl_801A5DEC[];
extern s32 lbl_801A5DF8[];
extern s32 lbl_801A5E08[];
extern s32 lbl_801A5E14[];
extern s32 lbl_801A5E20[];
extern s32 lbl_801A5E2C[];
extern s32 lbl_801A5E3C[];
extern s32 lbl_801A5E48[];

void PlayJumpSfx(void *arg0, s32 arg1) {
    s32 temp_r9;

    temp_r9 = (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4)));
    if (!(temp_r9 & 0x40)) {
        if (temp_r9 & 0x02000008) {
            switch (arg1) {                         /* irregular */
            case 0:
                fn_8006DCD4(lbl_801A5DE0, arg0 + 0x1AC);
                return;
            case 1:
                fn_8006DCD4(lbl_801A5DEC, arg0 + 0x1AC);
                return;
            case 3:
                if ((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) {
                    fn_8006DCD4(lbl_801A5DF8, arg0 + 0x1AC);
                    return;
                }
                fn_8006DCD4(lbl_801A5E08, arg0 + 0x1AC);
                return;
            case 4:
                if ((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) {
                    fn_8006DCD4(lbl_801A5E14, arg0 + 0x1AC);
                    return;
                }
                fn_8006DCD4(lbl_801A5E20, arg0 + 0x1AC);
                return;
            case 2:
                if ((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) {
                    fn_8006DCD4(lbl_801A5E2C, arg0 + 0x1AC);
                    return;
                }
                fn_8006DCD4(lbl_801A5E3C, arg0 + 0x1AC);
                return;
            }
        } else {
            fn_8006DCD4(lbl_801A5E48, arg0 + 0x6C);
        }
    }
}

s32 fn_8006DBB8(s32 *);
extern s32 lbl_802E1C74[];

void fn_800E6504(void) {
    fn_8006DBB8(lbl_802E1C74);
}

extern f32 lbl_801A9A0C[];
extern f32 lbl_80408E9C;

void fn_8010388C(void *arg0, void *arg1, void *arg2, f32 farg0) {
    f32 temp_f13;
    f32 temp_f13_2;
    f32 temp_f13_3;
    f32 var_f1;

    var_f1 = farg0 * lbl_80408E9C;
    if (var_f1 > lbl_801A9A0C[0]) {
        var_f1 = lbl_801A9A0C[0];
    }
    temp_f13 = (*(f32 *)((s8 *)(arg1) + (0)));
    (*(f32 *)((s8 *)(arg0) + (0))) = (f32) ((((*(f32 *)((s8 *)(arg2) + (0))) - temp_f13) * var_f1) + temp_f13);
    temp_f13_2 = (*(f32 *)((s8 *)(arg1) + (4)));
    (*(f32 *)((s8 *)(arg0) + (4))) = (f32) ((((*(f32 *)((s8 *)(arg2) + (4))) - temp_f13_2) * var_f1) + temp_f13_2);
    temp_f13_3 = (*(f32 *)((s8 *)(arg1) + (8)));
    (*(f32 *)((s8 *)(arg0) + (8))) = (f32) ((((*(f32 *)((s8 *)(arg2) + (8))) - temp_f13_3) * var_f1) + temp_f13_3);
}

s32 ApplyGravity(s32, s32, s32, f32, f32);
s32 ResetCode();
extern f32 lbl_801A9A14[];

void fn_80103988(s32 arg0) {
    ResetCode();
    ApplyGravity(arg0, 0, 0, lbl_801A9A14[0], lbl_801A9A14[0]);
}

s32 fn_80074934(void *);

void fn_801045B0(void *arg0) {
    (*(s32 *)((s8 *)(arg0) + (0x104))) = (s32) (((*(s32 *)((s8 *)(arg0) + (0x104))) | 0x80) & 0xFFFFBFFF);
    fn_80074934(arg0);
}

s32 ParticlesPerFrame(f32, f32);
s32 fn_80095A44(s32, s32, s32, s32, void *);
s32 fn_800A5C64();
extern f32 lbl_801A9AA0[];
extern f32 lbl_80408E9C;

void fn_80104628(void *arg0) {
    s32 temp_r30;

    (*(s16 *)((s8 *)(arg0) + (0x4E))) = 1;
    fn_800A5C64();
    if ((s32) (*(u8 *)((s8 *)(arg0) + (0x290))) != 0) {
        temp_r30 = ParticlesPerFrame(lbl_801A9AA0[0], lbl_80408E9C);
        if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0x1C))) != 0) {
            fn_80095A44((*(s32 *)((s8 *)(lbl_80409410) + (0x1B8))), 5, (*(s32 *)((s8 *)(arg0) + (0xEEC))) + 0x244, temp_r30, arg0 + 0x78);
        }
        if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0x20))) != 0) {
            fn_80095A44((*(s32 *)((s8 *)(lbl_80409410) + (0x1B8))), 5, (*(s32 *)((s8 *)(arg0) + (0xEEC))) + 0x284, temp_r30, arg0 + 0x78);
        }
    }
}

s32 fn_800A5C64(void *);
s32 fn_800F8414(void *, s32, s32);
extern f32 lbl_801A9AB4[];

void fn_80104738(void *arg0) {
    void *temp_r11;
    void *temp_r9;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0x19) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 1U;
        temp_r11 = (*(void **)((s8 *)(temp_r9) + (0x61C)));
        if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A9AB4[0])) {
            fn_800F8414(arg0, 1, 0);
        }
    }
    fn_800A5C64(arg0);
}

s32 ApplyGravity(s32, s32, s32, f32, f32);
s32 ResetCode();
extern f32 lbl_801A9AE4[];

void fn_80104EB4(s32 arg0) {
    ResetCode();
    ApplyGravity(arg0, 0, 0, lbl_801A9AE4[0], lbl_801A9AE4[0]);
}

s32 ApplyGravity(s32, s32, s32, f32, f32);
s32 ResetCode();
extern f32 lbl_801A9AEC[];

void fn_80104F50(s32 arg0) {
    ResetCode();
    ApplyGravity(arg0, 0, 0, lbl_801A9AEC[0], lbl_801A9AEC[0]);
}

void *fn_80105DEC();

void fn_801088F4(void) {
    void *temp_r3;

    temp_r3 = fn_80105DEC();
    if (temp_r3 != NULL) {
        (*(s32 *)((s8 *)(temp_r3) + (0x18))) = (s32) (((*(s32 *)((s8 *)(temp_r3) + (0x18))) | 0x2008) & ~0x10A0);
    }
}

extern s32 lbl_801A5DB4[];
extern s32 lbl_801A5DC8[];
extern s32 lbl_804096E8;

void fn_800E6484(void) {
    s32 var_r0;

    if ((s32) lbl_804096E8 != 0) {
        fn_8006DCD4(lbl_801A5DB4, 0);
        var_r0 = 0;
    } else {
        fn_8006DCD4(lbl_801A5DC8, 0);
        var_r0 = 1;
    }
    lbl_804096E8 = var_r0;
}

extern f32 lbl_801A9640[];
extern f32 lbl_801A9644[];
extern f32 lbl_801A9648[];
extern f32 lbl_8040987C;

void fn_800FC4F4(void *arg0)
{
  f32 temp_f13;
  s16 temp_r0_2;
  u8 temp_r0;
  void *temp_r10;
  temp_r10 = *((void **) (((s8 *) arg0) + 0xEEC));
  *((s16 *) (((s8 *) arg0) + 0x4E)) = 6;
  if (((((s32) (*((u8 *) (((s8 *) temp_r10) + 0x7D5)))) == 0) && (((((temp_r0, (((s32) (*((u8 *) (((s8 *) arg0) + 0x285)))) == 0) == 0)) || ((*((f32 *) (((s8 *) temp_r10) + 0x6E8))) > lbl_801A9640[0])) || (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x18)))) == 0)) || (((((*((f32 *) (((s8 *) temp_r10) + 0x778))) < lbl_8040987C) && ((temp_f13 = *((f32 *) (((s8 *) temp_r10) + 0x760)), (temp_f13 == lbl_801A9644[0]) == 0))) && (temp_f13 < lbl_801A9648[0])) && ((*((f32 *) (((s8 *) arg0) + 0x7C))) < lbl_801A9640[0])))) && (((((temp_r0_2 = *((s16 *) (((s8 *) arg0) + 0x1074)), (temp_r0_2 == 7) == 0)) && (temp_r0_2 != 0x43)) || (((s32) (*((u8 *) (((s8 *) arg0) + 0x285)))) != 0)) || (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x18)))) == 0)))
  {
    *((s16 *) (((s8 *) arg0) + 0x4E)) = 1;
  }
}

s32 fn_800F8618();
extern f32 lbl_801A9AC4[];

void fn_80104A58(void *arg0)
{
  unsigned int temp_r0;
  void *temp_r11;
  void *new_var;
  void *temp_r9;
  temp_r9 = *((void **) (((s8 *) arg0) + 0xEEC));
  if (((s32) (*((u8 *) (((s8 *) temp_r9) + 0x7DF)))) == 0x19)
  {
    *((u16 *) (((s8 *) arg0) + 0x4E)) = (u16) (*((u16 *) (((s8 *) temp_r9) + 0x7A6)));
    return;
  }
  *((u16 *) (((s8 *) arg0) + 0x4E)) = 1U;
  new_var = *((void **) (((s8 *) temp_r9) + 0x61C));
  temp_r11 = new_var;
  temp_r0 = (*((s32 *) (((s8 *) temp_r11) + 0x58))) & 0x4000;
  if ((temp_r0 == 0) && ((*((f32 *) (((s8 *) temp_r11) + 0x28))) > lbl_801A9AC4[0]))
  {
    temp_r9 = arg0;
    *((u16 *) (((s8 *) temp_r9) + 0x4E)) = temp_r0;
  }
  fn_800F8618();
}

s32 fn_800F8618();
extern f32 lbl_801A9AD4[];

void fn_80104CCC(void *arg0)
{
  int temp_r0;
  void *temp_r11;
  void *temp_r9;
  temp_r9 = *((void **) (((s8 *) arg0) + 0xEEC));
  if (((s32) (*((u8 *) (((s8 *) temp_r9) + 0x7DF)))) == 0x19)
  {
    *((u16 *) (((s8 *) arg0) + 0x4E)) = (u16) (*((u16 *) (((s8 *) temp_r9) + 0x7A6)));
    return;
  }
  *((u16 *) (((s8 *) arg0) + 0x4E)) = 1U;
  temp_r11 = *((void **) (((s8 *) temp_r9) + 0x61C));
  temp_r0 = (*((s32 *) (((s8 *) temp_r11) + 0x58))) & 0x4000;
  if ((temp_r0 == 0) && ((*((f32 *) (((s8 *) temp_r11) + 0x28))) > lbl_801A9AD4[0]))
  {
    *((u16 *) (((s8 *) arg0) + 0x4E)) = temp_r0;
  }
  fn_800F8618();
}

s32 fn_800F8618();
extern f32 lbl_801A9AD8[];

void fn_80104D38(void *arg0)
{
  u16 temp_r0;
  void *temp_r11;
  void *temp_r9;
  int new_var;
  temp_r9 = *((void **) (((s8 *) arg0) + 0xEEC));
  if (((s32) (*((u8 *) (((s8 *) temp_r9) + 0x7DF)))) == 0x19)
  {
    *((u16 *) (((s8 *) arg0) + 0x4E)) = (u16) (*((u16 *) (((s8 *) temp_r9) + 0x7A6)));
    return;
  }
  *((u16 *) (((s8 *) arg0) + 0x4E)) = 1U;
  temp_r11 = *((void **) (((s8 *) temp_r9) + 0x61C));
  new_var = (*((s32 *) (((s8 *) temp_r11) + 0x58))) & 0x4000;
  temp_r0 = new_var;
  if ((temp_r0 == 0) && ((*((f32 *) (((s8 *) temp_r11) + 0x28))) > lbl_801A9AD8[0]))
  {
    *((u16 *) (((s8 *) arg0) + 0x4E)) = temp_r0;
  }
  fn_800F8618();
}

s32 fn_800F8618();
extern f32 lbl_801A9ADC[];

void fn_80104DA4(void *arg0)
{
  unsigned long var_r0;
  s8 *new_var;
  unsigned long long pad;
  void *temp_r11;
  s8 *new_var2;
  void *temp_r11_2;
  temp_r11 = *((void **) (((s8 *) arg0) + 0xEEC));
  if (((s32) (*((u8 *) (((s8 *) temp_r11) + 0x7DF)))) == 0x19)
  {
    *((u16 *) (((s8 *) arg0) + 0x4E)) = (u16) (*((u16 *) (((s8 *) temp_r11) + 0x7A6)));
    return;
  }
  *((u16 *) (((s8 *) arg0) + 0x4E)) = 1U;
  new_var2 = (s8 *) temp_r11;
  if (((s32) (*((u8 *) (new_var2 + 0x7DF)))) == 0xB)
  {
    var_r0 = *((u16 *) (temp_r11 + 0x79C));
    goto block_6;
  }
  temp_r11_2 = *((void **) (((s8 *) temp_r11) + 0x61C));
  new_var = ((s8 *) temp_r11_2) + 0x58;
  var_r0 = (*((s32 *) new_var)) & 0x4000;
  if ((var_r0 == 0) && ((*((f32 *) (((s8 *) temp_r11_2) + 0x28))) > lbl_801A9ADC[0]))
  {
    block_6:
    *((u16 *) (((s8 *) arg0) + 0x4E)) = var_r0;

  }
  fn_800F8618();
}

s32 fn_8006DCD4(s32 *, void *);
s32 fn_800A5C64(void *);
extern s32 lbl_801A9998[];
extern f32 lbl_801A9AF0[];

void fn_80104F98(void *arg0)
{
  unsigned long temp_r0;
  void *temp_r11;
  *((s16 *) (((s8 *) arg0) + 0x4E)) = 1;
  temp_r11 = *((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0xEEC)))) + 0x61C));
  temp_r0 = (*((s32 *) (((s8 *) temp_r11) + 0x58))) & 0x4000;
  if ((temp_r0 == 0) && ((*((f32 *) (((s8 *) temp_r11) + 0x28))) > lbl_801A9AF0[0]))
  {
    *((s16 *) (((s8 *) arg0) + 0x4E)) = temp_r0;
    fn_8006DCD4(lbl_801A9998, arg0 + 0x90);
  }
  fn_800A5C64(arg0);
}

s32 fn_800F8618();
extern f32 lbl_801A9ACC[];

void fn_80104BB4(void *arg0)
{
  unsigned int new_var;
  u16 temp_r0;
  void *temp_r11;
  void *temp_r9;
  temp_r9 = *((void **) (((s8 *) arg0) + 0xEEC));
  if (((s32) (*((u8 *) (((s8 *) temp_r9) + 0x7DF)))) == 0x19)
  {
    *((u16 *) (((s8 *) arg0) + 0x4E)) = (u16) (*((u16 *) (((s8 *) temp_r9) + 0x7A6)));
    return;
  }
  *((u16 *) (((s8 *) arg0) + 0x4E)) = 1U;
  temp_r11 = *((void **) (((s8 *) temp_r9) + 0x61C));
  new_var = *((s32 *) (((s8 *) temp_r11) + 0x58));
  temp_r0 = new_var & 0x4000;
  if ((temp_r0 == 0) && ((*((f32 *) (((s8 *) temp_r11) + 0x28))) > lbl_801A9ACC[0]))
  {
    *((u16 *) (((s8 *) arg0) + 0x4E)) = temp_r0;
  }
  fn_800F8618();
}

void fn_80104858(void *arg0) {
    (*(s16 *)((s8 *)(arg0) + (0x4E))) = 1;
    fn_800A5C64(arg0);
}

void fn_80104880(void *arg0) {
    (*(s16 *)((s8 *)(arg0) + (0x4E))) = 1;
    fn_800A5C64(arg0);
}

void fn_801048A8(void *arg0) {
    s16 var_r0;
    u8 temp_r0;

    (*(s16 *)((s8 *)(arg0) + (0x4E))) = 1;
    temp_r0 = (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF)));
    if ((s8) temp_r0 == 0x29) {
        var_r0 = 0x30;
        goto block_4;
    }
    if ((s8) temp_r0 == 0x2A) {
        var_r0 = 0x31;
block_4:
        (*(s16 *)((s8 *)(arg0) + (0x4E))) = var_r0;
    }
    fn_800A5C64(arg0);
}

s32 fn_8006DCD4(s32 *, void *);
s32 fn_800957E0(f32, f32);
s32 fn_800959D4(s32, s32, void *, s32, s32, u16, void *);
extern s32 lbl_801A965C[];
extern f32 lbl_80408E9C;
extern f32 lbl_80409884;

void fn_800FCC00(void *arg0)
{
  s32 temp_r30;
  u8 temp_r0;
  void *temp_r9;
  if ((((((s32) (*((u8 *) (((s8 *) arg0) + 0x28F)))) == 0) && (((((temp_r9 = *((void **) (((s8 *) arg0) + 0xEEC)), temp_r0 = *((u8 *) (((s8 *) temp_r9) + 0x7DF)), (((s8) temp_r0) == (-1)) != 0)) || ((((s8) temp_r0) == 0x19) && (((s16) (*((s16 *) (((s8 *) temp_r9) + 0x7A6)))) == 0x44))) || ((((s8) temp_r0) == 0x18) && (((s16) (*((s16 *) (((s8 *) temp_r9) + 0x79C)))) == 0x5C))) || ((((s8) temp_r0) == 0xB) && (((s16) (*((s16 *) (((s8 *) temp_r9) + 0x79C)))) == 0x42)))) && (((s32) (*((u8 *) (((s8 *) (*((void **) (((s8 *) arg0) + 0xEEC)))) + 0x7CB)))) == 1)) && (((s32) (*((u8 *) (((s8 *) arg0) + 0x290)))) != 0))
  {
    temp_r30 = fn_800957E0(lbl_80409884, lbl_80408E9C);
    if (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0x30)))) != 0)
    {
      fn_800959D4(*((s32 *) (((s8 *) lbl_80409410) + 0x1B8)), 0x19, (*((void **) (((s8 *) arg0) + 0xEEC))) + 0x384, temp_r30, 0, *((u16 *) (((s8 *) arg0) + 0x1066)), lbl_80409410);
    }
    if (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0x34)))) != 0)
    {
      fn_800959D4(*((s32 *) (((s8 *) lbl_80409410) + 0x1B8)), 0x19, (*((void **) (((s8 *) arg0) + 0xEEC))) + 0x3C4, temp_r30, 0, *((u16 *) (((s8 *) arg0) + 0x1066)), lbl_80409410);
    }
    fn_8006DCD4(lbl_801A965C, arg0 + 0x90);
  }
}

void fn_800E64DC(void *arg0) {
    s16 *var_r9;
    s32 var_ctr;

    var_ctr = 0x40;
    var_r9 = (s16 *)((s8 *)arg0 + 0x1D94);
    do {
        *var_r9 = -1;
        var_r9 -= 8;
        var_ctr -= 1;
    } while (var_ctr != 0);
    (*(s32 *)((s8 *)(arg0) + (0x1D98))) = 0;
}

s32 fn_800A5C64(void *);
s32 fn_800F8414(void *, s32, s32);
s32 fn_800F8618(void *);
s32 fn_800FB024(void *);
s32 fn_800FB26C(void *);
extern f32 lbl_801A96F4[];
extern f32 lbl_80408E9C;
extern u8 *const lbl_80409740;

void fn_800FE40C(void *arg0) {
    s16 temp_r11_2;
    s32 var_r10;
    u8 temp_r0;
    s8 temp_r0_2;
    void *temp_r10;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(u8 *)((s8 *)(temp_r9) + (0x7DF)));
    if ((s8) temp_r0 == 0x19) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if ((s8) temp_r0 == 0x20) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else {
        fn_800FC4F4(arg0);
        temp_r0_2 = (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF)));
        if (temp_r0_2 == 0) {
            fn_800FB024(arg0);
        } else {
            var_r10 = 0;
            if ((lbl_80409740[temp_r0_2] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
                var_r10 = 1;
            }
            if (var_r10 != 0) {
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
            } else if ((fn_800FB26C(arg0) == 0) && ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) != 6)) {
                temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)));
                if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A96F4[0])) {
                    fn_800F8414(arg0, 1, 0);
                }
            }
        }
        fn_800F8618(arg0);
    }
    fn_800A5C64(arg0);
    temp_r11_2 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_2 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_2 == 0x2C) || (temp_r11_2 == 0x5A) || (temp_r11_2 == 0x5B))) || ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10) + (0x7B4))) == 0) && (temp_r11_2 == 0x54))) {
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_2) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_2) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10) + (0x778))) = (f32) lbl_801A96F4[0];
}

s32 fn_8006DCD4(s32 *, void *);
s32 fn_8006DD1C(s32 *, void *, f32);
s32 ParticlesPerFrame(f32, f32);
s32 fn_800959D4(s32, s32, void *, s32, s32, u16, void *);
extern s32 lbl_801A9720[];
extern s32 lbl_801A972C[];
extern f32 lbl_801A9734[];
extern f32 lbl_801A9738[];
extern f32 lbl_801A973C[];
extern f32 lbl_801A9740[];
extern f32 lbl_80408E9C;

void PostAnimate_ASTROMECH(void *arg0) {
    f32 var_f1;
    s32 temp_r3;
    void *temp_r9;
    void *temp_r9_2;

    if ((s32) (*(u8 *)((s8 *)(arg0) + (0x28F))) == 0) {
        temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if (((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0) && ((s32) (*(u8 *)((s8 *)(arg0) + (0x28C))) != 0) && ((s32) (*(u8 *)((s8 *)(arg0) + (0x290))) != 0)) {
            if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7CB))) == 3) {
                if ((*(s32 *)((s8 *)((*(void **)((s8 *)(temp_r9) + (0x61C)))) + (4))) & 0x20) {
                    var_f1 = lbl_801A9734[0];
                    goto block_8;
                }
                goto block_17;
            }
            var_f1 = lbl_801A9738[0];
block_8:
            temp_r3 = ParticlesPerFrame(var_f1, lbl_80408E9C);
            if (temp_r3 > 0) {
                if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0x1C))) != 0) {
                    fn_800959D4((*(s32 *)((s8 *)(lbl_80409410) + (0x1B8))), 0xA, (*(void **)((s8 *)(arg0) + (0xEEC))) + 0x244, temp_r3, 0x71C7, (*(u16 *)((s8 *)(arg0) + (0x1066))), lbl_80409410);
                }
                if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0x20))) != 0) {
                    fn_800959D4((*(s32 *)((s8 *)(lbl_80409410) + (0x1B8))), 0xA, (*(void **)((s8 *)(arg0) + (0xEEC))) + 0x284, temp_r3, 0x8E38, (*(u16 *)((s8 *)(arg0) + (0x1066))), lbl_80409410);
                }
            }
            fn_8006DD1C(lbl_801A9720, arg0 + 0x90, ((s32) (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7CB))) == 3) ? lbl_801A973C[0] : lbl_801A9738[0]);
            return;
        }
    }
block_17:
    if ((s32) (*(u8 *)((s8 *)(arg0) + (0x285))) != 0) {
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if (((s8) (*(u8 *)((s8 *)(temp_r9_2) + (0x7DF))) == -1) && ((*(f32 *)((s8 *)((*(void **)((s8 *)(temp_r9_2) + (0x61C)))) + (0x28))) > lbl_801A9740[0])) {
            fn_8006DCD4(lbl_801A972C, arg0 + 0x90);
        }
    }
}

s32 fn_800A5C64(void *);
s32 fn_800F8414(void *, s32, s32);
s32 fn_800F8618(void *);
extern f32 lbl_801A974C[];
extern f32 lbl_80408E9C;
extern u8 *const lbl_80409740;

void fn_800FEE14(void *arg0) {
    s16 temp_r11_2;
    s32 var_r10;
    u8 temp_r0;
    s8 temp_r0_2;
    u8 temp_r0_3;
    void *temp_r10;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;
    void *temp_r9_4;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(u8 *)((s8 *)(temp_r9) + (0x7DF)));
    if (((s8) temp_r0 == 0x19) || ((s8) temp_r0 == 0x1B)) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if ((s8) temp_r0 == 0x1E) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x1BU;
    } else if (((s8) temp_r0 == 0x20) || (((s32) (*(s32 *)((s8 *)(arg0) + (0xEF8))) != 0) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xBC))) != 0))) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else {
        fn_800FC4F4(arg0);
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        temp_r0_2 = (*(s8 *)((s8 *)(temp_r9_2) + (0x7DF)));
        if (temp_r0_2 == 0xB) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9_2) + (0x79C)));
        } else {
            var_r10 = 0;
            if ((lbl_80409740[temp_r0_2] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
                var_r10 = 1;
            }
            if (var_r10 != 0) {
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
            } else {
                temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                temp_r0_3 = (*(u8 *)((s8 *)(temp_r9_3) + (0x7DF)));
                if ((s8) temp_r0_3 == 8) {
                    (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x13U;
                } else if ((s8) temp_r0_3 == 7) {
                    (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x12U;
                } else if ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) != 6) {
                    temp_r11 = (*(void **)((s8 *)(temp_r9_3) + (0x61C)));
                    if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A974C[0])) {
                        fn_800F8414(arg0, 1, 0);
                    }
                }
            }
        }
        fn_800F8618(arg0);
    }
    fn_800A5C64(arg0);
    temp_r11_2 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_2 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_2 == 0x2C) || (temp_r11_2 == 0x5A) || (temp_r11_2 == 0x5B))) || ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10) + (0x7B4))) == 0) && (temp_r11_2 == 0x54))) {
        temp_r9_4 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_4) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_4) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10) + (0x778))) = (f32) lbl_801A974C[0];
}

s32 fn_800A5C64(void *);
s32 fn_800F8414(void *, s32, s32);
s32 fn_800F8618(void *);
s32 fn_800FB024(void *);
s32 fn_800FB26C(void *);
extern f32 lbl_801A97B4[];
extern f32 lbl_80408E9C;
extern u8 *const lbl_80409740;

void fn_800FFB84(void *arg0) {
    s16 temp_r11_2;
    s32 var_r10;
    u8 temp_r0;
    s8 temp_r0_2;
    void *temp_r10;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(u8 *)((s8 *)(temp_r9) + (0x7DF)));
    if ((s8) temp_r0 == 0x19) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if ((s8) temp_r0 == 0x20) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else {
        fn_800FC4F4(arg0);
        temp_r0_2 = (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF)));
        if (temp_r0_2 == 0) {
            fn_800FB024(arg0);
        } else {
            var_r10 = 0;
            if ((lbl_80409740[temp_r0_2] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
                var_r10 = 1;
            }
            if (var_r10 != 0) {
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
            } else if (fn_800FB26C(arg0) == 0) {
                temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                if ((s32) (*(u8 *)((s8 *)(temp_r9_2) + (0x7DF))) == 0x12) {
                    if ((s32) (*(u8 *)((s8 *)(temp_r9_2) + (0x7CA))) == 1) {
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x30U;
                    } else {
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x31U;
                    }
                } else if ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) != 6) {
                    temp_r11 = (*(void **)((s8 *)(temp_r9_2) + (0x61C)));
                    if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A97B4[0])) {
                        if ((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x35) {
                            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 4U;
                        } else {
                            fn_800F8414(arg0, 1, 0);
                        }
                    }
                }
            }
        }
        fn_800F8618(arg0);
    }
    fn_800A5C64(arg0);
    temp_r11_2 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_2 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_2 == 0x2C) || (temp_r11_2 == 0x5A) || (temp_r11_2 == 0x5B))) || ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10) + (0x7B4))) == 0) && (temp_r11_2 == 0x54))) {
        temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_3) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_3) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10) + (0x778))) = (f32) lbl_801A97B4[0];
}

s32 fn_800A5C64(void *);
s32 fn_800F8414(void *, s32, s32);
s32 fn_800F8618(void *);
s32 fn_800FB024(void *);
s32 fn_800FB26C(void *);
extern f32 lbl_801A9994[];
extern f32 lbl_80408E9C;
extern u8 *const lbl_80409740;

void fn_80102A3C(void *arg0) {
    s16 temp_r11_2;
    s32 var_r10;
    s8 temp_r0;
    void *temp_r10;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0x19) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else {
        fn_800FC4F4(arg0);
        temp_r0 = (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF)));
        if (temp_r0 == 0) {
            fn_800FB024(arg0);
        } else {
            var_r10 = 0;
            if ((lbl_80409740[temp_r0] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
                var_r10 = 1;
            }
            if (var_r10 != 0) {
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
            } else if (fn_800FB26C(arg0) == 0) {
                temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                if ((s32) (*(u8 *)((s8 *)(temp_r9_2) + (0x7DF))) == 9) {
                    (*(u16 *)((s8 *)(arg0) + (0x4E))) = 1U;
                } else if ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) != 6) {
                    temp_r11 = (*(void **)((s8 *)(temp_r9_2) + (0x61C)));
                    if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A9994[0])) {
                        fn_800F8414(arg0, 1, 1);
                    }
                }
            }
        }
        fn_800F8618(arg0);
    }
    fn_800A5C64(arg0);
    temp_r11_2 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_2 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_2 == 0x2C) || (temp_r11_2 == 0x5A) || (temp_r11_2 == 0x5B))) || ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10) + (0x7B4))) == 0) && (temp_r11_2 == 0x54))) {
        temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_3) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_3) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10) + (0x778))) = (f32) lbl_801A9994[0];
}

void fn_800B00B8(void *, f32, f32);
extern char MenuHeader[];
extern f32 lbl_801A99EC[];
extern f32 lbl_804097C0;

void fn_80103590(void *arg0) {
    s32 var_r7;
    void **temp_r8;
    void **var_r10;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;

    var_r7 = 0;
    if (arg0 != NULL) {
        temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if (temp_r9 != NULL) {
            temp_r9_2 = (*(void **)((s8 *)(temp_r9) + (0x620)));
            if (temp_r9_2 != NULL) {
                temp_r11 = (*(void **)((s8 *)(temp_r9_2) + (0x4C)));
                if ((s32) (*(s32 *)((s8 *)(temp_r11) + (4))) == 8) {
                    temp_r8 = (*(s32 *)((s8 *)(lbl_80409410) + (0x598))) + ((*(s32 *)((s8 *)(temp_r11) + (8))) * 0x210);
                    if (var_r7 < (s32) (*(s32 *)((s8 *)(temp_r8) + (0x200)))) {
                        var_r10 = temp_r8;
                        do {
                            temp_r9_3 = *var_r10;
                            var_r7 += 1;
                            var_r10 += 1;
                            (*(u16 *)((s8 *)(temp_r9_3) + (0x20))) = (u16) ((*(u16 *)((s8 *)(temp_r9_3) + (0x20))) & 0xEFBC);
                        } while (var_r7 < (s32) (*(s32 *)((s8 *)(temp_r8) + (0x200))));
                    }
                } else {
                    (*(u16 *)((s8 *)(temp_r9_2) + (0x20))) = (u16) ((*(u16 *)((s8 *)(temp_r9_2) + (0x20))) & 0xEFBC);
                }
            }
            (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6A0))) = 0;
            (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DA))) = 0;
            (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6A4))) = 0;
            (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) = -1;
        }
    }
    fn_800B00B8(MenuHeader, lbl_804097C0, lbl_801A99EC[0]);
}

extern void *gPlayer1Pointer[];
extern f32 lbl_801A9A30[];
extern f32 lbl_801A9A40[];
extern f32 lbl_8040AFE8;

void fn_80103D18(void) {
    s32 i;
    s32 count;
    void *p;

    count = 0;
    lbl_8040AFE8 = lbl_801A9A30[0];
    for (i = 0; i < 2; i++) {
        p = gPlayer1Pointer[i];
        if ((p != NULL) && ((*(s32 *)((s8 *)(p) + (0x20C))) & 0x01000000)) {
            count += 1;
            lbl_8040AFE8 += (*(f32 *)((s8 *)((*(void **)((s8 *)(p) + (0xEEC)))) + (0x708)));
        }
    }
    if (count > 1) {
        lbl_8040AFE8 *= lbl_801A9A40[0] / (f32) count;
    }
}

f32 AnimDuration(s16, s32, f32, f32, s32);
void fn_80095558(void *, s32);
extern f32 lbl_801A9A58[];

void fn_80104054(void *arg0) {
    void *temp_r9;

    if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xC0))) != 0) {
        temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(s8 *)((s8 *)(temp_r9) + (0x7DF))) = 0x29;
        (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = AnimDuration((*(s16 *)((s8 *)(arg0) + (0x1074))), 0x30, lbl_801A9A58[0], lbl_801A9A58[0], 1);
        fn_80095558(arg0 + 8, -1);
    }
}

f32 AnimDuration(s16, s32, f32, f32, s32);
void fn_80095558(void *, s32);
extern f32 lbl_801A9A5C[];

void fn_801040CC(void *arg0) {
    void *temp_r9;

    if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xC4))) != 0) {
        temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(s8 *)((s8 *)(temp_r9) + (0x7DF))) = 0x2A;
        (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = AnimDuration((*(s16 *)((s8 *)(arg0) + (0x1074))), 0x31, lbl_801A9A5C[0], lbl_801A9A5C[0], 1);
        fn_80095558(arg0 + 8, -1);
    }
}

s32 ObjHitObj(s32, void *, s32);
s32 fn_800D4EC8(void *, f32);
extern f32 lbl_801A9A60[];
extern f32 lbl_801A9A64[];
extern const f32 lbl_80409890;

void fn_80104144(void *arg0, s32 arg1, s32 arg2) {
    void *temp_r11;
    void *temp_r3;

    if (arg1 != 0) {
        ObjHitObj(0, arg0, 1);
        goto block_3;
    }
    if (arg2 != 0) {
block_3:
        fn_800D4EC8(arg0, lbl_801A9A60[0]);
    }
    temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    (*(f32 *)((s8 *)(temp_r11) + (0x708))) = (f32) ((*(f32 *)((s8 *)(temp_r11) + (0x708))) * lbl_801A9A64[0]);
    temp_r3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((*(f32 *)((s8 *)(temp_r3) + (0x708))) < lbl_80409890) {
        (*(f32 *)((s8 *)(temp_r3) + (0x708))) = (f32) lbl_80409890;
    }
}

s32 fn_800958F8(s32, s32, s32);
void NewRumbleAllPlayers(f32, f32);
void JudderGameCamera(void *, s32, s32, f32);
s32 PartKill(s32, s32);
extern char MenuHeader[];
extern f32 lbl_801A9A88[];
extern f32 lbl_801A9A8C[];
extern f32 lbl_801A9A90[];

void JangoRocket_Kill(s32 arg0, s32 arg1) {
    fn_800958F8((*(s32 *)((s8 *)(lbl_80409410) + (0x1B8))), 0x1A, arg0 + 0x30);
    JudderGameCamera(MenuHeader, 0, 0, lbl_801A9A88[0]);
    PartKill(arg0, arg1);
    NewRumbleAllPlayers(lbl_801A9A8C[0], lbl_801A9A90[0]);
}

s32 fn_800A5C64(void *);
s32 fn_800F8414(void *, s32, s32);
extern f32 lbl_801A9AC0[];
extern u8 *const lbl_80409740;

void fn_801048FC(void *arg0) {
    s32 var_r10;
    void *temp_r11;
    void *temp_r9;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0x19) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 1U;
        var_r10 = 0;
        if ((lbl_80409740[(s8) (*(u8 *)((s8 *)(temp_r9) + (0x7DF)))] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
            var_r10 = 1;
        }
        if (var_r10 != 0) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
        } else {
            temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)));
            if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A9AC0[0])) {
                fn_800F8414(arg0, 0, 0);
            }
        }
    }
    fn_800A5C64(arg0);
}

s32 ResetCode();
s32 ForceCode(void *, s32, s32, s32, s32);
extern f32 lbl_801A9AD0[];
extern f32 lbl_80408E9C;

void fn_80104C20(void *arg0) {
    void *temp_r9;
    void *temp_r9_2;

    ResetCode();
    ApplyGravity(arg0, 0, 0, lbl_801A9AD0[0], lbl_801A9AD0[0]);
    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0x19) {
        (*(f32 *)((s8 *)(temp_r9) + (0x714))) = (f32) ((*(f32 *)((s8 *)(temp_r9) + (0x714))) - lbl_80408E9C);
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if ((*(f32 *)((s8 *)(temp_r9_2) + (0x714))) <= lbl_801A9AD0[0]) {
            (*(s8 *)((s8 *)(temp_r9_2) + (0x7DF))) = -1;
        }
    }
    if ((*(s32 *)((s8 *)(arg0) + (0x20C))) & 0x01000000) {
        ForceCode(arg0, 0, 0, -1, 0);
    }
}

extern f32 lbl_801A9AE8[];

void fn_80104EFC(void *arg0) {
    unsigned long temp_r0;
    void *temp_r11;

    (*(s16 *)((s8 *)(arg0) + (0x4E))) = 1;
    temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)));
    temp_r0 = (*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000;
    if ((temp_r0 == 0) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A9AE8[0])) {
        (*(s16 *)((s8 *)(arg0) + (0x4E))) = temp_r0;
    }
    fn_800A5C64(arg0);
}

s32 ResetCode();
s32 ForceCode(void *, s32, s32, s32, s32);
s32 fn_800F99D4(void *, s32, s32, s32, s32, s32);
s32 fn_800FB6B0(void *);
s32 fn_800FBB08(void *);
extern f32 lbl_801A9970[];
extern f32 lbl_80408E9C;

void fn_801025A8(void *arg0) {
    void *temp_r11;
    void *temp_r11_2;
    void *temp_r9;

    ResetCode();
    ApplyGravity(arg0, 0, 0, lbl_801A9970[0], lbl_801A9970[0]);
    (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7B4))) = 1;
    (*(s8 *)((s8 *)(arg0) + (0x1083))) = 0;
    (*(s32 *)((s8 *)(arg0) + (0xF24))) = (s32) ((*(s32 *)((s8 *)(arg0) + (0xF24))) | 0x10000000);
    if ((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x100000) {
        fn_800FBB08(arg0);
    } else {
        fn_800FB6B0(arg0);
    }
    temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r11) + (0x7DF))) == 0x19) {
        (*(f32 *)((s8 *)(temp_r11) + (0x714))) = (f32) ((*(f32 *)((s8 *)(temp_r11) + (0x714))) - lbl_80408E9C);
        temp_r11_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if ((*(f32 *)((s8 *)(temp_r11_2) + (0x714))) <= lbl_801A9970[0]) {
            (*(s8 *)((s8 *)(temp_r11_2) + (0x7DF))) = -1;
        }
    }
    temp_r9 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)));
    fn_800F99D4(arg0, (*(s32 *)((s8 *)(temp_r9) + (8))) & 0x40, 0, 0, 0, 0);
    if ((*(s32 *)((s8 *)(arg0) + (0x20C))) & 0x01000000) {
        ForceCode(arg0, 0, 0, -1, 0);
    }
}

s32 Menu_ResetStack();
extern s32 lbl_801A5890[];
extern f32 lbl_801A5A28[];
struct MenuResetView { char unk0[8]; s32 flags8; char unkC[0x50]; };
extern struct MenuResetView lbl_803D9A58[];
extern s32 lbl_8040B0A8;
extern u8 lbl_8040B0DC[2];
extern f32 lbl_8040B178;
extern f32 lbl_8040B17C;

void Menu_ResetFromGame(void) {
    s32 i;
    s32 reset;
    u8 *flag;

    reset = 1;
    if ((s32) lbl_8040B0A8 != -1) {
        reset = 0;
        i = 0;
        flag = lbl_8040B0DC;
        for (; (i < 2) && (reset == 0); i++) {
            if ((*flag++ != 0) && (lbl_803D9A58[i].flags8 & 0x10)) {
                reset = 1;
            }
        }
    }
    if (reset != 0) {
        fn_8006DCD4(lbl_801A5890, NULL);
        Menu_ResetStack();
        lbl_8040B0A8 = -1;
        lbl_8040B17C = lbl_801A5A28[0];
        lbl_8040B178 = lbl_801A5A28[0];
    }
}

void fn_8006DE5C(s32, s32);
extern s32 lbl_801A5F1C[];
extern s32 lbl_801A5F28[];
extern s32 lbl_801A5F38[];
extern s32 lbl_801A5F44[];
extern s32 lbl_801A5F50[];
extern s32 lbl_801A5F5C[];
extern s32 lbl_801A5F68[];

void fn_800E3824(void *arg0) {
    s32 flags;
    s8 kind;
    s16 text;
    void *temp_r11;
    void *temp_r9;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0x64)));
    temp_r11 = (*(void **)((s8 *)(temp_r9) + (0x20)));
    text = (*(s16 *)((s8 *)(temp_r11) + (0xBC)));
    if (text != -1) {
        fn_8006DE5C(text, (s32) (arg0 + 0x90));
        return;
    }
    flags = (*(s32 *)((s8 *)(temp_r9) + (4)));
    if (flags & 0x20000) {
        fn_8006DCD4(lbl_801A5F1C, arg0 + 0x90);
        return;
    }
    kind = (*(s8 *)((s8 *)(temp_r11) + (0xE5)));
    if (kind == 8) {
        fn_8006DCD4(lbl_801A5F28, arg0 + 0x90);
    } else if (kind == 1) {
        fn_8006DCD4(lbl_801A5F38, arg0 + 0x90);
    } else if (kind == 0) {
        fn_8006DCD4(lbl_801A5F44, arg0 + 0x90);
    } else if (flags & 0x20) {
        fn_8006DCD4(lbl_801A5F50, arg0 + 0x90);
    } else if (flags & 0x4010) {
        fn_8006DCD4(lbl_801A5F5C, arg0 + 0x90);
    } else {
        fn_8006DCD4(lbl_801A5F68, arg0 + 0x90);
    }
}

extern s32 lbl_801A5E54[];
extern s32 lbl_801A5E60[];
extern s32 lbl_801A5E6C[];
extern s32 lbl_801A5E80[];
extern s32 lbl_801A5E8C[];
extern s32 lbl_801A5E98[];
extern s32 lbl_801A5EA4[];

void PlayLandSfx(void *arg0, s32 arg1) {
    s32 *var_r3;
    s32 *var_r3_2;
    s32 var_r0;

    if (arg1 == 1) {
        fn_8006DCD4(lbl_801A5E54, arg0 + 0x90);
    } else if (arg1 == 2) {
        fn_8006DCD4(lbl_801A5E60, arg0 + 0x90);
    } else if (arg1 == 3) {
        fn_8006DCD4(lbl_801A5E6C, arg0 + 0x90);
    } else if ((*(u16 *)((s8 *)(arg0) + (0x28A))) == 0) {
        if ((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x10) {
            if ((*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (4))) & 0x20000) {
                var_r3 = lbl_801A5E80;
            } else {
                var_r3 = lbl_801A5E8C;
            }
            fn_8006DCD4(var_r3, arg0 + 0x1AC);
        } else if (arg1 == 0) {
            if ((*(s32 *)((s8 *)(lbl_80409410) + (0x19C))) == 7) {
                var_r0 = 1;
            } else {
                var_r0 = (*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (4))) & 0x20000;
            }
            if (var_r0 != 0) {
                var_r3_2 = lbl_801A5E98;
            } else {
                var_r3_2 = lbl_801A5EA4;
            }
            fn_8006DCD4(var_r3_2, arg0 + 0x1AC);
        }
    }
}

s32 ObjHitObj(s32, void *, s32);
extern f32 lbl_801A97D8[];
extern f32 lbl_801A97DC[];
extern f32 lbl_801A97E0[];
extern f32 lbl_801A97E4[];
extern f32 lbl_803E8420[];
extern const f32 lbl_80409890;
extern f32 lbl_80409894;
extern f32 lbl_8040A71C;

void PodCollisionCode(void *arg0) {
    f32 temp_f13;
    f32 temp_f13_2;
    void *temp_r11;
    void *temp_r3;
    void *temp_r9;

    if (!(lbl_803E8420[0] < lbl_801A97D8[0]) && ((s8) (*(u8 *)((s8 *)(arg0) + (0x284))) != -1) && ((*(s32 *)((s8 *)(arg0) + (0x20C))) & 0x01000000) && ((s32) (*(u8 *)((s8 *)(arg0) + (0x107C))) != 0)) {
        temp_f13 = (*(f32 *)((s8 *)(arg0) + (0x648)));
        if ((temp_f13 <= lbl_801A97DC[0]) && (temp_f13 >= lbl_801A97E0[0])) {
            temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            temp_f13_2 = (lbl_801A97D8[0] / (*(f32 *)((s8 *)(temp_r9) + (0x824)))) * (*(f32 *)((s8 *)(temp_r9) + (0x828)));
            lbl_8040A71C = temp_f13_2;
            if (temp_f13_2 < lbl_801A97D8[0]) {
                if (temp_f13_2 < lbl_80409894) {
                    ObjHitObj(0, arg0, 1);
                }
                temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                (*(f32 *)((s8 *)(temp_r11) + (0x708))) = (f32) ((*(f32 *)((s8 *)(temp_r11) + (0x708))) * lbl_801A97E4[0]);
                temp_r3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                if ((*(f32 *)((s8 *)(temp_r3) + (0x708))) < lbl_80409890) {
                    (*(f32 *)((s8 *)(temp_r3) + (0x708))) = (f32) lbl_80409890;
                }
            }
        }
    }
}

s32 fn_8006DCD4(s32 *, void *);
s32 fn_800A5C64(void *);
extern s32 lbl_801A97CC[];

void fn_801046DC(void *arg0) {
    u16 var_r0;
    void *temp_r9;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0xB) {
        var_r0 = (*(u16 *)((s8 *)(temp_r9) + (0x79C)));
    } else {
        var_r0 = 1;
    }
    (*(u16 *)((s8 *)(arg0) + (0x4E))) = var_r0;
    fn_800A5C64(arg0);
    fn_8006DCD4(lbl_801A97CC, arg0 + 0x90);
}

extern f32 lbl_801A9AB8[];
extern f32 lbl_801A9ABC[];

void fn_801047C0(void *arg0) {
    f32 temp_f11;
    unsigned long var_r0;
    void *temp_r11;
    void *temp_r11_2;
    void *temp_r11_3;

    temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    (*(u16 *)((s8 *)(arg0) + (0x4E))) = 1U;
    if ((s32) (*(u8 *)((s8 *)(temp_r11) + (0x7DF))) == 0x19) {
        var_r0 = (*(u16 *)((s8 *)(temp_r11) + (0x7A6)));
        goto block_6;
    }
    temp_r11_2 = (*(void **)((s8 *)(temp_r11) + (0x61C)));
    var_r0 = (*(s32 *)((s8 *)(temp_r11_2) + (0x58))) & 0x4000;
    if (var_r0 == 0) {
        temp_f11 = (*(f32 *)((s8 *)(temp_r11_2) + (0x28)));
        if (temp_f11 > lbl_801A9AB8[0]) {
            temp_r11_3 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (0x20)));
            if (!(temp_f11 <= (((*(f32 *)((s8 *)(temp_r11_3) + (0x14))) + (*(f32 *)((s8 *)(temp_r11_3) + (0x18)))) * lbl_801A9ABC[0]))) {
                var_r0 = 4;
            }
block_6:
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = var_r0;
        }
    }
    fn_800A5C64(arg0);
}

void fn_801049D0(void *arg0) {
    u8 temp_r0;
    void *temp_r9;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(u8 *)((s8 *)(temp_r9) + (0x7DF)));
    if ((s8) temp_r0 == 0x1B) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if ((s8) temp_r0 == 0x20) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else if (((s8) temp_r0 == 0x23) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
    } else if (((s32) (*(s32 *)((s8 *)(arg0) + (0xEF8))) == 0) || ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xBC))) == 0)) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 1U;
    } else {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    }
}

s32 ResetCode();
s32 ForceCode(void *, s32, s32, s32, s32);
s32 fn_800F99D4(void *, s32, s32, s32, s32, s32);
extern f32 lbl_801A9AC8[];
extern f32 lbl_80408E9C;

void fn_80104AC4(void *arg0) {
    s32 temp_r11;
    s32 temp_r29;
    s32 temp_r30;
    void *temp_r10;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;

    temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r9 = (*(void **)((s8 *)(temp_r10) + (0x61C)));
    temp_r11 = (*(s32 *)((s8 *)(temp_r9) + (8)));
    (*(s8 *)((s8 *)(temp_r10) + (0x7B4))) = 1;
    temp_r30 = temp_r11 & 0x40;
    temp_r29 = temp_r11 & 0x80;
    (*(s32 *)((s8 *)(arg0) + (0xF24))) = (s32) ((*(s32 *)((s8 *)(arg0) + (0xF24))) | 0x10000000);
    ResetCode();
    ApplyGravity(arg0, 0, 0, lbl_801A9AC8[0], lbl_801A9AC8[0]);
    temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9_2) + (0x7DF))) == 0x19) {
        (*(f32 *)((s8 *)(temp_r9_2) + (0x714))) = (f32) ((*(f32 *)((s8 *)(temp_r9_2) + (0x714))) - lbl_80408E9C);
        temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if ((*(f32 *)((s8 *)(temp_r9_3) + (0x714))) <= lbl_801A9AC8[0]) {
            (*(s8 *)((s8 *)(temp_r9_3) + (0x7DF))) = -1;
        }
    }
    fn_800F99D4(arg0, temp_r30, temp_r29, 0, 0, 0);
    if ((*(s32 *)((s8 *)(arg0) + (0x20C))) & 0x01000000) {
        ForceCode(arg0, 0, 0, -1, 0);
    }
}

struct Fn80108930Entry { void *ptr; char unk4[0x20]; u8 flags24; char unk25[0xF]; };
struct Fn80108930Obj { char unk0[0x1E]; u8 count1E; char unk1F[0x95]; struct Fn80108930Entry entries[1]; };

void fn_80108930(struct Fn80108930Obj *arg0, s32 arg1, s32 arg2) {
    s32 i;
    s32 j;
    s8 *e;
    s8 *found;

    if (arg0 == NULL) {
        e = (*(s8 **)((s8 *)(lbl_80409410) + (0x238)));
        if (e != NULL) {
            for (i = 0; i < (*(s32 *)((s8 *)(lbl_80409410) + (0x23C))); i++) {
                if (((*(s8 *)(e + 0x71)) == arg1) && ((*(s8 *)(e + 0x72)) == arg2)) {
                    found = e;
                    goto done;
                }
                e += (*(u16 *)(e + 0x1C));
            }
        }
        found = NULL;
    done:
        arg0 = (struct Fn80108930Obj *)found;
        if (arg0 == NULL) {
            return;
        }
    }
    for (j = 0; j < arg0->count1E; j++) {
        if ((arg0->entries[j].flags24 & 0x18) == 0) {
            if (arg0->entries[j].ptr != NULL) {
                (*(s32 *)((s8 *)(arg0->entries[j].ptr) + (0x50))) |= 0x80000000;
                (*(s32 *)((s8 *)(arg0->entries[j].ptr) + (0x50))) &= ~0x20000000;
                (*(s32 *)((s8 *)(arg0->entries[j].ptr) + (0x50))) &= ~0x10000000;
                (*(s32 *)((s8 *)(arg0->entries[j].ptr) + (0x50))) &= ~0x08000000;
            }
        }
    }
}

extern void* memset();
s32 fn_80007610(s32, s32);

void fn_80108B3C(void *arg0) {
    s32 temp_r3;
    s32 var_r29;
    s32 var_r30;

    if (arg0 != NULL) {
        memset(arg0 + 8, 0, 0x160);
        var_r29 = 0;
        memset(arg0 + 0x10, 0, 0x160);
        memset((*(s32 *)((s8 *)(arg0) + (4))), 0, (*(s32 *)((s8 *)(arg0) + (0))) * 0x160);
        if (var_r29 < (s32) (*(s32 *)((s8 *)(arg0) + (0)))) {
            var_r30 = 0;
            do {
                var_r29 += 1;
                fn_80007610(arg0 + 8, (*(s32 *)((s8 *)(arg0) + (4))) + var_r30);
                var_r30 += 0xA8;
            } while (var_r29 < (s32) (*(s32 *)((s8 *)(arg0) + (0))));
        }
    }
}

s32 fn_80095630(void *, s32, s32, s32);
s32 ForcedBackCode(void *);
void fn_801044F0(void *);
extern f32 lbl_801A9924[];
extern f32 lbl_801A9928[];
extern f32 lbl_804098AC;

void fn_80101520(void *arg0) {
    f32 temp_f13;
    f32 var_f1;
    void *temp_r11;
    void *temp_r4;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;
    void *temp_r9_4;

    ResetCode();
    if ((s32) (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) != 0x1B) {
        var_f1 = lbl_804098AC;
    } else {
        var_f1 = lbl_801A9924[0];
    }
    ApplyGravity(arg0, 0, 0, var_f1, lbl_801A9928[0]);
    (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7B4))) = 1;
    temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    (*(s32 *)((s8 *)(arg0) + (0xF24))) = (s32) ((*(s32 *)((s8 *)(arg0) + (0xF24))) | 0x10000000);
    if ((s32) (*(u8 *)((s8 *)(temp_r11) + (0x7DF))) == 0x26) {
        f32 k = lbl_801A9924[0];
        temp_f13 = (*(f32 *)((s8 *)(temp_r11) + (0x714)));
        if (temp_f13 > k) {
            (*(f32 *)((s8 *)(temp_r11) + (0x714))) = (f32) (temp_f13 - lbl_80408E9C);
        }
        temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if (((*(f32 *)((s8 *)(temp_r9) + (0x714))) <= k) && ((s32) (*(u8 *)((s8 *)(arg0) + (0x285))) != 0)) {
            (*(s8 *)((s8 *)(temp_r9) + (0x7DF))) = -1;
            (*(f32 *)((s8 *)(arg0) + (0x78))) = k;
            (*(f32 *)((s8 *)(arg0) + (0x80))) = k;
        }
    }
    ForcedBackCode(arg0);
    temp_r4 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if (((s32) (*(u8 *)((s8 *)(temp_r4) + (0x7DF))) == 0x1B) && (((s32) *(s32 *)(((*(s16 *)((s8 *)(temp_r4) + (0x7A6))) * 4) + (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) == 0) || (fn_80095630(arg0 + 8, (*(s16 *)((s8 *)(temp_r4) + (0x7A6))), 1, 0) != 0))) {
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_2) + (0x714))) = (f32) ((*(f32 *)((s8 *)(temp_r9_2) + (0x714))) - lbl_80408E9C);
        temp_f13 = lbl_801A9924[0];
        temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if ((*(f32 *)((s8 *)(temp_r9_3) + (0x714))) <= temp_f13) {
            (*(s8 *)((s8 *)(temp_r9_3) + (0x7DF))) = -1;
        } else if ((u32) ((*(u16 *)((s8 *)(arg0) + (0x1074))) - 0x1D) <= 1U) {
            fn_801044F0(arg0);
        }
    }
    temp_r9_4 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)));
    fn_800F99D4(arg0, (*(s32 *)((s8 *)(temp_r9_4) + (8))) & 0x40, 0, 0, 0, 1);
    if ((*(s32 *)((s8 *)(arg0) + (0x20C))) & 0x01000000) {
        ForceCode(arg0, 0, 0, -1, 0);
    }
}

extern f32 lbl_801A9758[];

void fn_800FF1D8(void *arg0) {
    s16 temp_r11_2;
    s32 var_r10;
    s8 temp_r0;
    void *temp_r10;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(s8 *)((s8 *)(temp_r9) + (0x7DF)));
    if ((temp_r0 == 0x19) || (temp_r0 == 0x1B)) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if (temp_r0 == 0x20) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else {
        var_r10 = 0;
        if ((lbl_80409740[temp_r0] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
            var_r10 = 1;
        }
        if (var_r10 != 0) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
        } else if (((*(s32 *)((s8 *)(arg0) + (0xEF8))) != 0) && ((*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xBC))) != 0)) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
        } else {
            fn_800FC4F4(arg0);
            if ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) != 6) {
                temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)));
                if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A9758[0])) {
                    fn_800F8414(arg0, 1, 0);
                }
            }
            fn_800F8618(arg0);
        }
    }
    fn_800A5C64(arg0);
    temp_r11_2 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_2 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_2 == 0x2C) || (temp_r11_2 == 0x5A) || (temp_r11_2 == 0x5B))) || ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10) + (0x7B4))) == 0) && (temp_r11_2 == 0x54))) {
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_2) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_2) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10) + (0x778))) = (f32) lbl_801A9758[0];
}

extern f32 lbl_801A995C[];

void fn_801020C8(void *arg0) {
    s16 temp_r11_2;
    s32 var_r10;
    s8 temp_r0;
    void *temp_r10;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(s8 *)((s8 *)(temp_r9) + (0x7DF)));
    if ((temp_r0 == 0x19) || (temp_r0 == 0x1B)) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if (temp_r0 == 0x20) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else {
        var_r10 = 0;
        if ((lbl_80409740[temp_r0] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
            var_r10 = 1;
        }
        if (var_r10 != 0) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
        } else if (((*(s32 *)((s8 *)(arg0) + (0xEF8))) != 0) && ((*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xBC))) != 0)) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
        } else {
            fn_800FC4F4(arg0);
            if ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) != 6) {
                temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)));
                if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A995C[0])) {
                    (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0;
                }
            }
            fn_800F8618(arg0);
        }
    }
    fn_800A5C64(arg0);
    temp_r11_2 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_2 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_2 == 0x2C) || (temp_r11_2 == 0x5A) || (temp_r11_2 == 0x5B))) || ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10) + (0x7B4))) == 0) && (temp_r11_2 == 0x54))) {
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_2) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_2) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10) + (0x778))) = (f32) lbl_801A995C[0];
}

s32 fn_800956C0(void *);
extern const f32 lbl_801A9108[];
extern const f32 lbl_801A910C[];

void fn_800ED4A0(void *arg0, s32 arg1) {
    f32 temp_f31;
    s16 var_r11;
    u8 temp_r30;
    void *temp_r11;
    void *temp_r11_2;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;
    void *temp_r9_4;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0x25) {
        if (((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7C3))) != 0) && (arg1 == 0)) {
            (*(u8 *)((s8 *)(temp_r9) + (0x7C3))) = (u8) arg1;
        }
        if ((((s32) *(s32 *)(((*(s16 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7A6))) * 4) + (*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) == 0) || (fn_800956C0(arg0 + 8) == (s16) (*(s16 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7A6))))) && (temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC))), temp_f31 = lbl_801A9108[0], (*(f32 *)((s8 *)(temp_r11) + (0x714))) = (f32) ((*(f32 *)((s8 *)(temp_r11) + (0x714))) - lbl_80408E9C), temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC))), ((*(f32 *)((s8 *)(temp_r9_2) + (0x714))) <= temp_f31))) {
            (*(s8 *)((s8 *)(temp_r9_2) + (0x7DF))) = -1;
            if (arg1 != 0) {
                temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                temp_r30 = (*(u8 *)((s8 *)(temp_r9_3) + (0x7C3)));
                if ((s32) temp_r30 == 0) {
                    (*(u8 *)((s8 *)(temp_r9_3) + (0x7DF))) = 0x25U;
                    fn_80095558(arg0 + 8, -1);
                    temp_r9_4 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                    var_r11 = 0x57;
                    if ((s32) (*(u8 *)((s8 *)(temp_r9_4) + (0x7B4))) != 0) {
                        var_r11 = 0xE;
                    }
                    (*(s16 *)((s8 *)(temp_r9_4) + (0x7A6))) = var_r11;
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = AnimDuration((*(s16 *)((s8 *)(arg0) + (0x1074))), (*(s16 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7A6))), temp_f31, temp_f31, 1);
                    temp_r11_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                    if ((*(f32 *)((s8 *)(temp_r11_2) + (0x714))) <= temp_f31) {
                        (*(f32 *)((s8 *)(temp_r11_2) + (0x714))) = (f32) lbl_801A910C[0];
                    }
                    (*(s16 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7A0))) = -1;
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6C8))) = temp_f31;
                    (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7C3))) = 1U;
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x728))) = temp_f31;
                    (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7D8))) = temp_r30;
                    PlayJumpSfx(arg0, 2);
                    return;
                }
            }
            (*(f32 *)((s8 *)(arg0) + (0x78))) = (f32) lbl_801A9108[0];
            (*(f32 *)((s8 *)(arg0) + (0x80))) = (f32) lbl_801A9108[0];
        }
    }
}

extern f32 lbl_801A9764[];
extern f32 lbl_801A9768[];

void fn_800FF584(void *arg0) {
    f32 temp_f11;
    s16 temp_r11_3;
    s32 var_r10;
    s8 temp_r0;
    s8 temp_r0_2;
    s8 temp_r0_3;
    void *temp_r10;
    void *temp_r11;
    void *temp_r11_2;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;
    void *temp_r9_4;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(s8 *)((s8 *)(temp_r9) + (0x7DF)));
    if ((temp_r0 == 0x19) || (temp_r0 == 0x1B)) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if (temp_r0 == 0x20) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else if (((*(s32 *)((s8 *)(arg0) + (0xEF8))) != 0) && ((*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xBC))) != 0)) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else {
        fn_800FC4F4(arg0);
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        temp_r0_2 = (*(s8 *)((s8 *)(temp_r9_2) + (0x7DF)));
        if (temp_r0_2 == 0xB) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9_2) + (0x79C)));
        } else {
            var_r10 = 0;
            if ((lbl_80409740[temp_r0_2] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
                var_r10 = 1;
            }
            if (var_r10 != 0) {
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6U;
            } else {
                temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                temp_r0_3 = (*(s8 *)((s8 *)(temp_r9_3) + (0x7DF)));
                if (temp_r0_3 == 0x11) {
                    (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x27U;
                } else if (temp_r0_3 == 0x10) {
                    (*(u16 *)((s8 *)(arg0) + (0x4E))) = 4U;
                } else if ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) != 6) {
                    temp_r11 = (*(void **)((s8 *)(temp_r9_3) + (0x61C)));
                    if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000)) {
                        temp_f11 = (*(f32 *)((s8 *)(temp_r11) + (0x28)));
                        if (temp_f11 > lbl_801A9764[0]) {
                            temp_r11_2 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (0x20)));
                            if (temp_f11 <= (((*(f32 *)((s8 *)(temp_r11_2) + (0x10))) + (*(f32 *)((s8 *)(temp_r11_2) + (0x14)))) * lbl_801A9768[0])) {
                                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 5U;
                            } else {
                                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0;
                            }
                        }
                    }
                }
            }
        }
        fn_800F8618(arg0);
    }
    fn_800A5C64(arg0);
    temp_r11_3 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_3 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_3 == 0x2C) || (temp_r11_3 == 0x5A) || (temp_r11_3 == 0x5B))) || ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10) + (0x7B4))) == 0) && (temp_r11_3 == 0x54))) {
        temp_r9_4 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_4) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_4) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10) + (0x778))) = (f32) lbl_801A9764[0];
}

extern f32 lbl_801A97C0[];

void fn_800FFF50(void *arg0) {
    s16 temp_r11_2;
    s32 var_r30;
    s32 var_r8;
    s8 temp_r0;
    void *temp_r10;
    void *temp_r10_2;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;

    var_r30 = 0;
    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(s8 *)((s8 *)(temp_r9) + (0x7DF)));
    if (temp_r0 == 0x19) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if (temp_r0 == 0x1B) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if (temp_r0 == 0x20) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2F;
    } else if ((*(s32 *)((s8 *)(arg0) + (0xEF8))) != 0) {
        switch ((*(u8 *)((s8 *)(temp_r9) + (0x7D7)))) {
        case 1:
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x5B;
            break;
        case 2:
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x5A;
            break;
        case 3:
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2C;
            break;
        default:
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6;
            break;
        }
    } else {
        fn_800FC4F4(arg0);
        if ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) == 6) {
            switch ((*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7D7)))) {
            case 1:
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x5B;
                break;
            case 2:
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x5A;
                break;
            case 3:
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2C;
                break;
            default:
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6;
                break;
            }
            var_r30 = 1;
        } else {
            switch ((*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7D7)))) {
            case 1:
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0xA;
                break;
            case 2:
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x16;
                break;
            case 3:
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x11;
                break;
            default:
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 1;
                break;
            }
        }
        var_r8 = 0;
        if ((lbl_80409740[(*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF)))] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
            var_r8 = 1;
        }
        if (var_r8 != 0) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6;
        } else {
            temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            if ((s32) (*(u8 *)((s8 *)(temp_r11) + (0x7DF))) == 0xC) {
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = (*(u16 *)((s8 *)(temp_r11) + (0x7A6)));
            } else if (var_r30 == 0) {
                temp_r10 = (*(void **)((s8 *)(temp_r11) + (0x61C)));
                if (!((*(s32 *)((s8 *)(temp_r10) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r10) + (0x28))) > lbl_801A97C0[0])) {
                    switch ((*(u8 *)((s8 *)(temp_r11) + (0x7D7)))) {
                    case 1:
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x21;
                        break;
                    case 2:
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x1A;
                        break;
                    case 3:
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 4;
                        break;
                    default:
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0;
                        break;
                    }
                }
            }
        }
    }
    fn_800A5C64(arg0);
    temp_r11_2 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_2 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_2 == 0x2C) || (temp_r11_2 == 0x5A) || (temp_r11_2 == 0x5B))) || ((temp_r10_2 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10_2) + (0x7B4))) == 0) && (temp_r11_2 == 0x54))) {
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_2) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_2) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10_2) + (0x778))) = (f32) lbl_801A97C0[0];
}

extern f32 lbl_801A9950[];

void fn_80101D10(void *arg0) {
    s16 temp_r11_2;
    s32 var_r10;
    s8 temp_r0;
    s8 temp_r0_2;
    s8 temp_r0_3;
    void *temp_r10;
    void *temp_r11;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;

    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(s8 *)((s8 *)(temp_r9) + (0x7DF)));
    if (temp_r0 == 0x19) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = (u16) (*(u16 *)((s8 *)(temp_r9) + (0x7A6)));
    } else if (temp_r0 == 0x20) {
        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2FU;
    } else {
        fn_800FC4F4(arg0);
        temp_r0_2 = (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF)));
        if (temp_r0_2 == 0x17) {
            (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x4E;
        } else {
            var_r10 = 0;
            if ((lbl_80409740[temp_r0_2] & 8) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x18))) != 0)) {
                var_r10 = 1;
            }
            if (var_r10 != 0) {
                (*(u16 *)((s8 *)(arg0) + (0x4E))) = 6;
            } else if (fn_800FB26C(arg0) == 0) {
                temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                temp_r0_3 = (*(s8 *)((s8 *)(temp_r9_2) + (0x7DF)));
                if (temp_r0_3 == 0xB) {
                    (*(u16 *)((s8 *)(arg0) + (0x4E))) = (*(u16 *)((s8 *)(temp_r9_2) + (0x79C)));
                } else if (temp_r0_3 == 0x14) {
                    (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x2E;
                } else if ((temp_r0_3 == -1) && ((s32) (*(u8 *)((s8 *)(temp_r9_2) + (0x7CB))) == 1)) {
                    if ((*(f32 *)((s8 *)((*(void **)((s8 *)(temp_r9_2) + (0x61C)))) + (0x28))) > lbl_801A9950[0]) {
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x29;
                    } else {
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0x11;
                    }
                } else if ((s16) (*(u16 *)((s8 *)(arg0) + (0x4E))) != 6) {
                    temp_r11 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)));
                    if (!((*(s32 *)((s8 *)(temp_r11) + (0x58))) & 0x4000) && ((*(f32 *)((s8 *)(temp_r11) + (0x28))) > lbl_801A9950[0])) {
                        (*(u16 *)((s8 *)(arg0) + (0x4E))) = 0;
                    }
                }
            }
        }
        fn_800F8618(arg0);
    }
    fn_800A5C64(arg0);
    temp_r11_2 = (s16) (*(u16 *)((s8 *)(arg0) + (0x4E)));
    if ((temp_r11_2 == 6) || (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x20) && ((temp_r11_2 == 0x2C) || (temp_r11_2 == 0x5A) || (temp_r11_2 == 0x5B))) || ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), (((s16) (*(s16 *)((s8 *)(arg0) + (0x1074))) == 0x4A) != 0)) && ((s32) (*(u8 *)((s8 *)(temp_r10) + (0x7B4))) == 0) && (temp_r11_2 == 0x54))) {
        temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(f32 *)((s8 *)(temp_r9_3) + (0x778))) = (f32) ((*(f32 *)((s8 *)(temp_r9_3) + (0x778))) + lbl_80408E9C);
        return;
    }
    (*(f32 *)((s8 *)(temp_r10) + (0x778))) = (f32) lbl_801A9950[0];
}

extern s32 lbl_801A5EB0[];
extern s32 lbl_801A5EC0[];
extern s32 lbl_801A5ECC[];
extern s32 lbl_801A5EDC[];
extern s32 lbl_801A5EE8[];
extern s32 lbl_801A5EF4[];
extern s32 lbl_801A5F00[];
extern s32 lbl_801A5F0C[];
extern f32 lbl_801A5F18[];

void PlayFootStepSfx(void *arg0) {
    s32 *var_r3;
    s32 *var_r3_2;
    s16 temp_r11;
    s32 var_r10;

    if (((*(s32 *)((s8 *)(lbl_80409410) + (0x198))) == 0x45) && ((s32) (*(u8 *)((s8 *)(arg0) + (0x601))) == 1)) {
        var_r10 = 1;
    } else if (((*(s32 *)((s8 *)(lbl_80409410) + (0x198))) == 0x46) && ((s32) (*(u8 *)((s8 *)(arg0) + (0x601))) == 8) && ((*(f32 *)((s8 *)(arg0) + (0x62C))) > lbl_801A5F18[0])) {
        var_r10 = 1;
    } else if ((*(s32 *)((s8 *)(lbl_80409410) + (0x19C))) == 7) {
        var_r10 = 1;
    } else {
        var_r10 = (*(s32 *)((s8 *)((*(void **)((s8 *)(lbl_80409410) + (0x1A8)))) + (4))) & 0x20000;
    }
    if ((s32) (*(u16 *)((s8 *)(arg0) + (0x28A))) != 0) {
        fn_8006DCD4(lbl_801A5EB0, arg0 + 0x1AC);
        return;
    }
    temp_r11 = (*(s16 *)((s8 *)(arg0) + (0x1074)));
    if (temp_r11 == 0x2B) {
        fn_8006DCD4(lbl_801A5EC0, arg0 + 0x1AC);
        return;
    }
    if (temp_r11 == 0x5D) {
        fn_8006DCD4(lbl_801A5ECC, arg0 + 0x1AC);
        return;
    }
    if ((temp_r11 == 0x2E) || (temp_r11 == 0x34)) {
        fn_8006DCD4(lbl_801A5EDC, arg0 + 0x1AC);
        return;
    }
    if (((*(s32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (4))) & 0x10) || (temp_r11 == 0x4A)) {
        if (var_r10 != 0) {
            var_r3 = lbl_801A5EE8;
        } else {
            var_r3 = lbl_801A5EF4;
        }
        fn_8006DCD4(var_r3, arg0 + 0x1AC);
        return;
    }
    if (var_r10 != 0) {
        var_r3_2 = lbl_801A5F00;
    } else {
        var_r3_2 = lbl_801A5F0C;
    }
    fn_8006DCD4(var_r3_2, arg0 + 0x1AC);
}

s32 fn_800956C0(void *);
extern const s32 lbl_801A95B8[];
extern const s32 lbl_801A95C4[];
extern const f32 lbl_801A95D0[];
extern const f32 lbl_801A95D4[];

void RollCode(void *arg0) {
    f32 temp_f31;
    s8 temp_r0;
    void *temp_r30;
    void *temp_r3;
    void *temp_r7;

    temp_r7 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_r0 = (*(s8 *)((s8 *)(temp_r7) + (0x7DF)));
    if (temp_r0 == 0x11) {
        (*(f32 *)((s8 *)(temp_r7) + (0x714))) = (f32) ((*(f32 *)((s8 *)(temp_r7) + (0x714))) - lbl_80408E9C);
        temp_r3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if ((*(f32 *)((s8 *)(temp_r3) + (0x714))) <= lbl_801A95D0[0]) {
            (*(s8 *)((s8 *)(temp_r3) + (0x7DF))) = -1;
        }
    } else if (temp_r0 == 0x10) {
        temp_r30 = arg0 + 0x90;
        fn_8006DCD4(lbl_801A95B8, temp_r30);
        if (((s16) (*(s16 *)((s8 *)(arg0) + (0x4A))) == 4) && ((s32) (*(u8 *)((s8 *)(arg0) + (0x45))) == 0)) {
            temp_f31 = lbl_801A95D0[0];
            if ((*(f32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x61C)))) + (0x28))) == temp_f31) {
                fn_80095558(arg0 + 8, -1);
                if (((s32) (*(u8 *)((s8 *)(arg0) + (0x285))) != 0) && ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0x9C))) != 0)) {
                    (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) = 0x11U;
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = AnimDuration((*(s16 *)((s8 *)(arg0) + (0x1074))), 0x27, temp_f31, temp_f31, 1);
                    fn_8006DCD4(lbl_801A95C4, temp_r30);
                } else {
                    (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) = -1;
                }
            }
        }
    } else if (temp_r0 == -1) {
        if ((s32) (*(u8 *)((s8 *)(arg0) + (0x285))) != 0) {
            if ((*(f32 *)((s8 *)((*(void **)((s8 *)(temp_r7) + (0x61C)))) + (0x28))) >= ((*(f32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x64)))) + (0x20)))) + (0x18))) - lbl_801A95D4[0])) {
                (*(u8 *)((s8 *)(temp_r7) + (0x7DF))) = 0x10U;
            } else if (fn_800956C0(arg0 + 8) == 6) {
                (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) = 0x11U;
                (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = AnimDuration((*(s16 *)((s8 *)(arg0) + (0x1074))), 0x27, lbl_801A95D0[0], lbl_801A95D0[0], 1);
                fn_8006DCD4(lbl_801A95C4, arg0 + 0x90);
            }
        }
    }
}

s32 fn_800956C0(void *);
extern s32 lbl_801A95D8[];
extern f32 lbl_801A95E4[];
extern f32 lbl_801A95E8[];
extern f32 lbl_801A95EC[];

void ShieldCode(void *arg0) {
    f32 temp_f12;
    f32 var_f31;
    s32 temp_r3;
    void *temp_r11;

    var_f31 = lbl_801A95E4[0];
    if (((s32) (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7D6))) != 0) && ((temp_r3 = fn_800956C0(arg0 + 8), ((s32) (*(u8 *)((s8 *)(arg0) + (0x285))) != 0)) || ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6E8))) > var_f31)) && (temp_r3 != 4) && (temp_r3 != 0x27) && (temp_r3 != 6) && ((*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) != 0x10) && ((*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) != 0x11) && ((*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) != 0x1B)) {
        var_f31 = lbl_801A95E8[0];
    }
    temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    temp_f12 = (*(f32 *)((s8 *)(temp_r11) + (0x6AC)));
    if (temp_f12 > var_f31) {
        (*(f32 *)((s8 *)(temp_r11) + (0x6AC))) = (f32) (temp_f12 - (lbl_80408E9C * lbl_801A95EC[0]));
        if ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6AC))) < var_f31) {
            (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6AC))) = var_f31;
        }
    } else if (temp_f12 < var_f31) {
        (*(f32 *)((s8 *)(temp_r11) + (0x6AC))) = (f32) ((lbl_80408E9C * lbl_801A95EC[0]) + temp_f12);
        if ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6AC))) > var_f31) {
            (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6AC))) = var_f31;
        }
    }
    if (((temp_f12 == lbl_801A95E4[0]) && ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6AC))) > lbl_801A95E4[0])) || ((temp_f12 == lbl_801A95E8[0]) && ((*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6AC))) < lbl_801A95E8[0]))) {
        fn_8006DCD4(lbl_801A95D8, arg0 + 0x90);
    }
}

s32 fn_800ABA78(void *);
s32 NewRumble(s32, f32);
s32 fn_800B9ED4();
s32 KillRumble(void *);
s32 fn_800D99AC();
s32 CancelHint();
void SetHeadTarget(void *, void *, s32, f32, f32, f32);
s32 fn_801261FC(void *, s32);
extern f32 lbl_801A93D0[];
extern f32 lbl_801A93D4[];
extern f32 lbl_801A93D8[];
extern f32 lbl_801A93DC[];
extern s32 lbl_80409008;
extern f32 lbl_804097CC;
extern f32 lbl_804097D0;
extern f32 lbl_804097D4;
extern f32 lbl_804097D8;

void fn_800F68D8(void *arg0) {
    f32 temp_f13;
    f32 temp_f31;
    f32 var_f30;
    f32 var_f31;
    s32 temp_r30;
    s32 temp_r30_2;
    u8 temp_r0;
    void *temp_r10;
    void *temp_r11;
    void *temp_r11_2;
    void *temp_r11_3;
    void *temp_r9;
    void *temp_r9_2;
    void *temp_r9_3;
    void *temp_r9_4;
    void *temp_r9_5;
    void *temp_r9_6;
    void *temp_r9_7;
    void *temp_r9_8;
    void *temp_r9_9;

    if ((s32) lbl_80409008 != 0) {
        var_f31 = lbl_804097D4;
        var_f30 = lbl_804097D8;
    } else {
        var_f31 = lbl_804097CC;
        var_f30 = lbl_804097D0;
    }
    temp_r9 = (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650)));
    if ((temp_r9 != NULL) && ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x28F))) != 0)) {
        fn_800ABA78(arg0);
    }
    temp_r11 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r11) + (0x7DF))) == 0x22) {
        (*(s8 *)((s8 *)(temp_r11) + (0x7DA))) = 1;
        temp_r11_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(void **)((s8 *)(temp_r11_2) + (0x6A4))) = (void *) (*(void **)((s8 *)(temp_r11_2) + (0x650)));
        (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7B3))) = 1;
        SetHeadTarget(arg0, (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650))) + 0x90, 1, 1.0f, 0.0f, 0.0f);
        temp_r30 = *(s32 *)(*(s32 **)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650)))) + (0xEEC)))) + (0x61C)));
        NewRumble(temp_r30, (f32) (f64) fn_800B9ED4() * lbl_801A93D0[0] * lbl_801A93D4[0]);
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        temp_f31 = 0.0f;
        temp_f13 = (*(f32 *)((s8 *)(temp_r9_2) + (0x718)));
        if (temp_f13 > temp_f31) {
            (*(f32 *)((s8 *)(temp_r9_2) + (0x718))) = (f32) (temp_f13 - lbl_80408E9C);
            temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            if ((*(f32 *)((s8 *)(temp_r9_3) + (0x718))) <= temp_f31) {
                if (ObjHitObj(arg0, (*(void **)((s8 *)(temp_r9_3) + (0x650))), 1) == 2) {
                    KillRumble((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650))));
                }
                (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) = -1;
                (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x73C))) = temp_f31;
                (*(s8 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650)))) + (0xEEC)))) + (0x7DF))) = -1;
                (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650))) = NULL;
            }
        } else {
            (*(f32 *)((s8 *)(temp_r9_2) + (0x714))) = (f32) ((*(f32 *)((s8 *)(temp_r9_2) + (0x714))) + lbl_80408E9C);
            temp_r9_4 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            if (((*(f32 *)((s8 *)(temp_r9_4) + (0x714))) >= var_f30) || (temp_r9_5 = (*(void **)((s8 *)(temp_r9_4) + (0x650))), (((s32) (*(u8 *)((s8 *)(temp_r9_5) + (0x28F))) == 0) == 0)) || ((s32) (*(u8 *)((s8 *)((*(void **)((s8 *)(temp_r9_5) + (0xEEC)))) + (0x7DF))) != 0x23)) {
                (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x718))) = (f32) lbl_801A93D8[0];
            }
        }
    } else {
        temp_r9_6 = (*(void **)((s8 *)(temp_r11) + (0x650)));
        if (temp_r9_6 != NULL) {
            temp_r30_2 = *(s32 *)(*(s32 **)((s8 *)((*(void **)((s8 *)(temp_r9_6) + (0xEEC)))) + (0x61C)));
            NewRumble(temp_r30_2, (f32) (f64) fn_800B9ED4() * lbl_801A93D0[0] * lbl_801A93DC[0]);
            (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DA))) = 1;
            temp_r11_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            (*(void **)((s8 *)(temp_r11_3) + (0x6A4))) = (void *) (*(void **)((s8 *)(temp_r11_3) + (0x650)));
            (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7B3))) = 1;
            SetHeadTarget(arg0, (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650))) + 0x90, 1, 1.0f, 0.0f, 0.0f);
            temp_r9_7 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            (*(f32 *)((s8 *)(temp_r9_7) + (0x73C))) = (f32) ((*(f32 *)((s8 *)(temp_r9_7) + (0x73C))) + lbl_80408E9C);
            temp_r9_8 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            if (((*(f32 *)((s8 *)(temp_r9_8) + (0x73C))) >= var_f31) && ((s32) (*(u8 *)((s8 *)(arg0) + (0x285))) != 0) && ((temp_r0 = (*(u8 *)((s8 *)(temp_r9_8) + (0x7DF))), (((s8) temp_r0 == -1) != 0)) || (lbl_80409740[(s8) temp_r0] & 4) || ((s8) temp_r0 == 7) || ((s8) temp_r0 == 8)) && ((temp_r10 = (*(void **)((s8 *)(arg0) + (0xEEC))), temp_r9_9 = (*(void **)((s8 *)(temp_r10) + (0x650))), (((s32) (*(u8 *)((s8 *)(temp_r9_9) + (0x285))) == 0) == 0)) || ((s32) (*(u8 *)((s8 *)((*(void **)((s8 *)(temp_r9_9) + (0xEEC)))) + (0x7CB))) == 1))) {
                (*(u8 *)((s8 *)(temp_r10) + (0x7DF))) = 0x22U;
                (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = (f32) 0.0f;
                (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x718))) = (f32) 0.0f;
                fn_801261FC((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650))), 0);
                (*(s8 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650)))) + (0xEEC)))) + (0x7DF))) = 0x23;
                (*(s8 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650)))) + (0xEEC)))) + (0x7CB))) = 0;
                if (fn_800D99AC() == 0x26B) {
                    CancelHint();
                }
            }
        } else {
            (*(f32 *)((s8 *)(temp_r11) + (0x73C))) = (f32) 0.0f;
        }
    }
}

struct Fn80103680Vec {
    s32 x, y, z;
};

void SetHeadTarget(void *arg0, void *arg1, s32 arg2, f32 base, f32 lo, f32 hi) {
    if (arg1 != NULL) {
        void *cur = *(void **)((s8 *)arg0 + 0xFFC);
        if (cur == NULL || (arg1 != cur && arg2 >= *(s8 *)((s8 *)arg0 + 0x1088))) {
            f32 t = (f32)fn_800B9ED4() * 1.5259022e-05f;
            f32 v;
            *(void **)((s8 *)arg0 + 0xFFC) = arg1;
            *(s8 *)((s8 *)arg0 + 0x1088) = arg2;
            v = hi * t + lo * (1.0f - t);
            *(f32 *)((s8 *)arg0 + 0x1010) = v;
            *(f32 *)((s8 *)arg0 + 0x100C) = base + v;
        }
        *(struct Fn80103680Vec *)((s8 *)arg0 + 0x1000) = **(struct Fn80103680Vec **)((s8 *)arg0 + 0xFFC);
        return;
    }
    *(void **)((s8 *)arg0 + 0xFFC) = NULL;
    *(s8 *)((s8 *)arg0 + 0x1088) = 0;
    *(f32 *)((s8 *)arg0 + 0x100C) = 0.0f;
    *(f32 *)((s8 *)arg0 + 0x1010) = 0.0f;
}

void fn_800ED214(void *arg0) {
    void *e = *(void **)((s8 *)arg0 + 0xEEC);
    void *e2;
    void *e3;
    f32 v;

    switch (*(u8 *)((s8 *)e + 0x7CC)) {
    case 1:
        v = *(f32 *)((s8 *)e + 0x6BC);
        if (v > 1.0f) {
            *(f32 *)((s8 *)e + 0x6BC) = v - lbl_80408E9C * 5.0f;
            if (*(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6BC) < 1.0f) {
                *(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6BC) = 1.0f;
            }
        } else if (v < 1.0f) {
            *(f32 *)((s8 *)e + 0x6BC) = lbl_80408E9C * 5.0f + v;
            if (*(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6BC) > 1.0f) {
                *(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6BC) = 1.0f;
            }
        }
        e2 = *(void **)((s8 *)arg0 + 0xEEC);
        if (*(f32 *)((s8 *)e2 + 0x6BC) >= 1.0f) {
            *(u8 *)((s8 *)e2 + 0x7CC) = 0;
            *(s8 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x7B4) = 1;
        }
        break;
    case 2:
        v = *(f32 *)((s8 *)e + 0x6BC);
        if (v > 0.0f) {
            *(f32 *)((s8 *)e + 0x6BC) = v - lbl_80408E9C * 5.0f;
            if (*(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6BC) < 0.0f) {
                *(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6BC) = 0.0f;
            }
        } else if (v < 0.0f) {
            *(f32 *)((s8 *)e + 0x6BC) = lbl_80408E9C * 5.0f + v;
            if (*(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6BC) > 0.0f) {
                *(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6BC) = 0.0f;
            }
        }
        e3 = *(void **)((s8 *)arg0 + 0xEEC);
        if (*(f32 *)((s8 *)e3 + 0x6BC) <= 0.0f) {
            *(u8 *)((s8 *)e3 + 0x7CC) = 0;
            *(s8 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x7B4) = 0;
        }
        break;
    }
}

s32 fn_800F8618(void *arg0) {
    u16 t;
    void *e;
    void *e2;

    if (fn_80094F98() != 1) {
        if (!(*(s16 *)((s8 *)arg0 + 0x4E) == 5 || *(s16 *)((s8 *)arg0 + 0x4E) == 0x45 ||
              *(s16 *)((s8 *)arg0 + 0x4E) == 0 || *(s16 *)((s8 *)arg0 + 0x4E) == 0x46 ||
              *(s16 *)((s8 *)arg0 + 0x4E) == 4 || *(s16 *)((s8 *)arg0 + 0x4E) == 0x1A)) {
            *(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x6C4) =
                *(f32 *)((s8 *)*(void **)((s8 *)*(void **)((s8 *)arg0 + 0x64) + 0x20) + 0x50);
        }
        if ((*(s16 *)((s8 *)arg0 + 0x4E) == 5 || *(s16 *)((s8 *)arg0 + 0x4E) == 0x45 ||
             *(s16 *)((s8 *)arg0 + 0x4E) == 0 || *(s16 *)((s8 *)arg0 + 0x4E) == 0x46 ||
             *(s16 *)((s8 *)arg0 + 0x4E) == 4 || *(s16 *)((s8 *)arg0 + 0x4E) == 0x1A ||
             *(s16 *)((s8 *)arg0 + 0x4E) == 1 || *(s16 *)((s8 *)arg0 + 0x4E) == 0x1C) &&
            (*(s16 *)((s8 *)arg0 + 0x4C) == 5 || *(s16 *)((s8 *)arg0 + 0x4C) == 0x45 ||
             *(s16 *)((s8 *)arg0 + 0x4C) == 0 || *(s16 *)((s8 *)arg0 + 0x4C) == 0x46 ||
             *(s16 *)((s8 *)arg0 + 0x4C) == 4 || *(s16 *)((s8 *)arg0 + 0x4C) == 0x1A)) {
            e = *(void **)((s8 *)arg0 + 0xEEC);
            if (*(s16 *)((s8 *)e + 0x79E) != -1) {
                *(f32 *)((s8 *)e + 0x72C) = *(f32 *)((s8 *)e + 0x72C) - lbl_80408E9C;
                e2 = *(void **)((s8 *)arg0 + 0xEEC);
                if (*(f32 *)((s8 *)e2 + 0x72C) <= 0.0f) {
                    *(s16 *)((s8 *)e2 + 0x79E) = -1;
                } else {
                    *(s16 *)((s8 *)arg0 + 0x4E) = *(s16 *)((s8 *)e2 + 0x79E);
                }
            } else {
                *(f32 *)((s8 *)e + 0x72C) = 0.1f;
                t = *(s16 *)((s8 *)arg0 + 0x4C);
                if (((s16)t == 4 || (s16)t == 0x1A) && *(s16 *)((s8 *)arg0 + 0x4E) != 4 && (s16)t != 0x1A) {
                    *(s16 *)((s8 *)arg0 + 0x4E) = t;
                    *(s16 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x79E) = t;
                }
            }
        } else {
            *(s16 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x79E) = -1;
            *(f32 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x72C) = 0.1f;
        }
    }
}

void fn_800EC0A0(void *arg0) {
    void *e;
    s8 *p;
    s32 i;
    f32 dz;
    f32 dx;

    e = *(void **)((s8 *)arg0 + 0xEEC);
    if (*(u8 *)((s8 *)e + 0x7DF) == 0x15) {
        p = *(s8 **)(lbl_80409410 + 0x230) + *(u8 *)((s8 *)e + 0x7CF) * 0x30;
        if (*(f32 *)((s8 *)arg0 + 0x198) < *(f32 *)(p + 0x18)) {
            *(s8 *)((s8 *)e + 0x7DF) = -1;
            return;
        }
        if (*(f32 *)((s8 *)arg0 + 0x18C) > *(f32 *)(p + 0x1C)) {
            *(s8 *)((s8 *)e + 0x7DF) = -1;
            return;
        }
        dz = *(f32 *)((s8 *)arg0 + 0x98) - *(f32 *)(p + 0x14);
        dx = *(f32 *)((s8 *)arg0 + 0x90) - *(f32 *)(p + 0x10);
        if (dx * dx + dz * dz > *(f32 *)(p + 0x20)) {
            *(s8 *)((s8 *)e + 0x7DF) = -1;
        }
    } else {
        p = *(s8 **)(lbl_80409410 + 0x230);
        if (p != NULL) {
            for (i = 0; i < *(s32 *)(lbl_80409410 + 0x234); i++) {
                if (*(u8 *)(p + 0x2C) != 0 && !(*(f32 *)((s8 *)arg0 + 0x198) < *(f32 *)(p + 0x18)) &&
                    !(*(f32 *)((s8 *)arg0 + 0x18C) > *(f32 *)(p + 0x1C))) {
                    dz = *(f32 *)((s8 *)arg0 + 0x98) - *(f32 *)(p + 0x14);
                    dx = *(f32 *)((s8 *)arg0 + 0x90) - *(f32 *)(p + 0x10);
                    if (dx * dx + dz * dz < *(f32 *)(p + 0x20)) {
                        *(u8 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x7DF) = 0x15;
                        *(u8 *)((s8 *)*(void **)((s8 *)arg0 + 0xEEC) + 0x7CF) = i;
                        return;
                    }
                }
                p += 0x30;
            }
        }
    }
}

void fn_800EC1C8(void *arg0, void *arg1) {
    s32 i;
    f32 t;
    s8 *v;

    lbl_803FFE2C[*(s8 *)((s8 *)arg1 + 0x284)].obj = arg0;
    lbl_803FFE2C[*(s8 *)((s8 *)arg1 + 0x284)].timer = 0.0f;
    for (i = 0; i < 3; i++) {
        lbl_803FFE2C[*(s8 *)((s8 *)arg1 + 0x284)].t[i] = (f32)fn_800B9ED4() * 1.5259022e-05f * 0.4f + 0.4f;
        t = lbl_803FFE2C[*(s8 *)((s8 *)arg1 + 0x284)].t[i];
        v = (s8 *)(i * 12 + *(s8 *)((s8 *)arg1 + 0x284) * 0x38);
        v += (s32)lbl_803FFE2C[0].pos;
        *(f32 *)(v + 0) = *(f32 *)((s8 *)arg0 + 0x90);
        *(f32 *)(v + 8) = *(f32 *)((s8 *)arg0 + 0x98);
        *(f32 *)(v + 4) = (*(f32 *)((s8 *)arg0 + 0x198) - *(f32 *)((s8 *)arg0 + 0x18C)) * t + *(f32 *)((s8 *)arg0 + 0x18C);
    }
}

s32 fn_800EC68C(void *, void *);

struct Fn800EC9B8Flags {
    u32 pad : 7;
    u32 bit24 : 1;
    u32 rest : 24;
};
#define FN_800EC9B8_BIT(p) (((struct Fn800EC9B8Flags *)((s8 *)(p) + 0x20C))->bit24)

#define FN_800EC9B8_OK(p) ((p) != NULL && (*(u32 *)((s8 *)(p) + 0x20C) & 0x80080000) == 0x80080000 && \
    (p) != arg0 && *(u8 *)((s8 *)(p) + 0x28F) == 0 && !(*(u32 *)((s8 *)(p) + 0x20C) & 0x01000000) && \
    !(*(s32 *)((s8 *)(p) + 0xF24) & 8))

s32 fn_800EC9B8(void *arg0, void *arg1) {
    s32 i;
    void *p;
    u32 a;
    u32 b;

    if (!FN_800EC9B8_OK(arg1)) {
        arg1 = NULL;
    }
    if (arg1 == NULL) {
        for (i = 0; i < 8; i++) {
            p = gPlayer1Pointer[i];
            if (FN_800EC9B8_OK(p)) {
                arg1 = p;
                break;
            }
        }
    }
    if (arg1 != NULL) {
        a = FN_800EC9B8_BIT(arg0);
        b = FN_800EC9B8_BIT(arg1);
        if (fn_800EC68C(arg0, arg1) != 0) {
            FN_800EC9B8_BIT(arg1) = a;
            FN_800EC9B8_BIT(arg0) = b;
            return 1;
        }
    }
    return 0;
}
#undef FN_800EC9B8_OK
#undef FN_800EC9B8_BIT

s32 fn_80013890(s32, void *);

struct Fn80105194Obj {
    u8 pad0[0x4C];
    f32 f4C; /* 0x4C */
    s32 flags; /* 0x50 */
};

struct Fn80105194Entry {
    u8 pad0[0xC];
    struct Fn80105194Obj *obj; /* 0xC */
    u8 pad10[0x20];
    u8 flags; /* 0x30 */
    u8 pad31[3];
};

struct Fn80105194Self {
    u8 pad0[0x1E];
    u8 count; /* 0x1E */
    u8 pad1F[0x89];
    struct Fn80105194Entry entries[1]; /* 0xA8 */
};

s32 fn_80105194(struct Fn80105194Self *arg0) {
    s32 n;
    s32 i;
    s32 j;
    char *p;
    struct Fn80105194Obj *obj;
    s32 fl;
    if (arg0 != NULL) {
        n = 0;
        for (i = 0; i < arg0->count; i++) {
            if ((arg0->entries[i].flags & 0x18) == 0) {
                obj = arg0->entries[i].obj;
                if (obj != NULL) {
                    fl = obj->flags;
                    if (!(fl & 0x10000000)) {
                        if (fl & 0x80000000) return 0;
                        if (obj->f4C <= 1.0f) return 0;
                    } else {
                        n++;
                    }
                } else {
                    j = 0;
                    p = *(char **)((char *)lbl_80409410 + 0x5D4);
                    while (p != 0 && j < *(s32 *)((char *)lbl_80409410 + 0x5D8)) {
                        if (fn_80013890(*(s32 *)p + 4, &arg0->entries[i]) != 0) {
                            if (!(*(u16 *)(*(char **)p + 0x20) & 0x100)) return 0;
                            p = 0;
                        } else {
                            j++;
                            p += *(u16 *)(p + 8);
                        }
                    }
                }
            }
        }
        if (n < arg0->count) return 1;
    }
    return 0;
}

s32 fn_80108DA8(void *, s32);

struct Fn8010443CEntry {
    u8 pad0[4];
    u8 *obj; /* 0x4 */
    u8 flag; /* 0x8 */
    s8 a; /* 0x9 */
    s8 b; /* 0xA */
    u8 padB;
};

void fn_8010443C(u8 *arg0) {
    struct Fn8010443CEntry *e;
    s32 i;

    if (arg0 != NULL) {
        e = *(struct Fn8010443CEntry **)(arg0 + 0x228);
        if (e != NULL) {
            for (i = 0; i < *(s32 *)(arg0 + 0x22C); i++, e++) {
                if ((e->obj == NULL || (*(u16 *)(e->obj + 0x20) & 0x101) != 0) &&
                    (e->a == -1 || fn_80108DA8(arg0, e->a) == e->b)) {
                    e->flag = 1;
                } else {
                    e->flag = 0;
                }
            }
        }
    }
}

struct Fn800F10B4Tbl {
    u8 pad0[0xD2];
    s8 e[1][2]; /* 0xD2 */
};

struct Fn800F10B4Arr {
    u8 pad0[0x14];
    s32 v[1]; /* 0x14 */
};

s32 fn_800F10B4(u8 *arg0, s32 *arg1, s32 arg2, s32 *arg3) {
    s32 local;
    u8 *q;
    s8 st;
    s32 *out;

    if (*(u8 *)(arg0 + 0x290) == 0) return 0;
    q = *(u8 **)(arg0 + 0xEEC);
    if (*(u8 *)(q + 0x7DE) == 0) return 0;
    if (arg3 == NULL) {
        out = &local;
    } else {
        out = arg3;
    }
    if (*(s16 *)(arg0 + 0x1074) == 8 &&
        (((st = *(s8 *)(q + 0x7DF)) == 5 &&
          (*(s16 *)(q + 0x798) == 0x33 || *(s16 *)(q + 0x798) == 0x34 || *(s16 *)(q + 0x798) == 0x36 ||
           *(s16 *)(q + 0x798) == 0x37 || *(s16 *)(q + 0x798) == 0x38 || *(s16 *)(q + 0x798) == 0x39 ||
           *(s16 *)(q + 0x798) == 0x3B || *(s16 *)(q + 0x798) == 0x3D || *(s16 *)(q + 0x798) == 0x3E ||
           *(s16 *)(q + 0x798) == 0x41)) ||
         (st == 0 && *(u8 *)(q + 0x7BE) == 4))) {
        if (arg2 == 1) {
            *out = 0;
        } else {
            *out = 1;
        }
    } else {
        *out = arg2;
    }
    arg1[0] = ((struct Fn800F10B4Tbl *)*(u8 **)(*(u8 **)(arg0 + 0x64) + 0x20))->e[*out][0];
    if (arg1[0] == -1 || (*(struct Fn800F10B4Arr **)(arg0 + 0x60))->v[arg1[0]] == 0) return 0;
    arg1[1] = ((struct Fn800F10B4Tbl *)*(u8 **)(*(u8 **)(arg0 + 0x64) + 0x20))->e[*out][1];
    if (arg1[1] == -1 || (*(struct Fn800F10B4Arr **)(arg0 + 0x60))->v[arg1[1]] == 0) return 0;
    return 1;
}

extern f32 lbl_80408E9C;

f32 fn_801038E8(f32 a, f32 b, f32 t) {
    t *= lbl_80408E9C;
    if (t > 1.0f) {
        t = 1.0f;
    }
    return (b - a) * t + a;
}

s32 fn_80103494(u8 *arg0) {
    u8 *q = *(u8 **)(arg0 + 0xEEC);
    if (*(u8 *)(q + 0x7DF) == 0x22) {
        return 2;
    }
    if (*(s32 *)(q + 0x650) != 0) {
        return 1;
    }
    return 0;
}

s32 fn_80103944(u8 *arg0) {
    if (*(u8 *)(arg0 + 0x287) != 1) {
        return 0;
    }
    if (*(f32 *)(arg0 + 0x198) < *(f32 *)(arg0 + 0x224)) {
        return 0;
    }
    if (*(f32 *)(arg0 + 0x18C) > *(f32 *)(arg0 + 0x224)) {
        return 0;
    }
    return 1;
}

void fn_801044F0(void *arg0) {
    u8 *q = *(u8 **)((u8 *)arg0 + 0xEEC);
    if (*(u8 *)(q + 0x7D7) == 3) {
        *(s16 *)(q + 0x7A6) = 0x48;
    } else if (*(u8 *)(q + 0x7D7) == 2) {
        *(s16 *)(q + 0x7A6) = 0x49;
    } else if (*(u8 *)(q + 0x7D7) == 1) {
        *(s16 *)(q + 0x7A6) = 0x4A;
    }
}

void fn_801045E4(void *arg0) {
    u8 *q = *(u8 **)((u8 *)arg0 + 0xEEC);
    if (*(u8 *)(q + 0x7D7) == 3) {
        *(s16 *)(q + 0x7A6) = 0x4B;
    } else if (*(u8 *)(q + 0x7D7) == 2) {
        *(s16 *)(q + 0x7A6) = 0x4C;
    } else if (*(u8 *)(q + 0x7D7) == 1) {
        *(s16 *)(q + 0x7A6) = 0x4D;
    }
}

s32 fn_80103914(u8 *arg0) {
    if (*(u8 *)(arg0 + 0x287) != 1) {
        return 0;
    }
    return *(f32 *)(arg0 + 0x94) <= *(f32 *)(arg0 + 0x224);
}

void fn_80108878(struct Fn80105194Self *arg0) {
    s32 i;
    for (i = 0; i < arg0->count; i++) {
        if ((arg0->entries[i].flags & 0x18) == 0 && arg0->entries[i].obj != NULL) {
            arg0->entries[i].obj->flags |= 0x80000000;
            arg0->entries[i].obj->f4C = 3.4028235e38f;
        }
    }
    *(s32 *)((u8 *)arg0 + 0x18) |= 0xA8;
}

struct Fn800DF25CArg {
    u8 pad0[0x44];
    f32 f44; /* 0x44 */
    f32 f48; /* 0x48 */
};

extern s32 lbl_8040B1E8;
extern f32 lbl_8040B1F8;
extern f32 lbl_804095A8;
extern f32 lbl_804095AC;

s32 fn_800DF25C(struct Fn800DF25CArg *arg0) {
    f32 lim;
    if (arg0->f44 > arg0->f48) {
        arg0->f44 = 0.0f;
        lbl_8040B1E8++;
        lbl_8040B1F8 = 1.0f;
        return 1;
    }
    lim = arg0->f48 - lbl_804095A8;
    if (arg0->f44 > lim - lbl_804095AC && arg0->f44 <= lim) {
        lbl_8040B1F8 = (arg0->f48 - arg0->f44 - lbl_804095A8) / lbl_804095AC;
    }
    return 0;
}

struct Fn800E2770Player {
    u8 pad0[0x20C];
    u32 b20C_0 : 7;
    u32 b20C_7 : 1; /* 0x20C bit 7 */
    u32 b20C_8 : 24;
    u8 pad210[0xE64];
    s16 s1074; /* 0x1074 */
};

extern void *gPlayer1Pointer[];
struct Fn800E2770Pair {
    s16 h[2]; /* 0x0 */
    u8 b[2]; /* 0x4, also referenced as lbl_8040B0DC */
};
extern struct Fn800E2770Pair lbl_8040B0D8;

void fn_800E2770(void) {
    s32 i;
    struct Fn800E2770Player **pp;
    struct Fn800E2770Player *p;
    s16 *h;
    u8 *b;
    s16 none;
    none = -1;
    pp = (struct Fn800E2770Player **)gPlayer1Pointer;
    b = lbl_8040B0D8.b;
    h = lbl_8040B0D8.h;
    for (i = 0; i < 2; i++, pp++, b++, h++) {
        p = *pp;
        if (p != NULL) {
            *h = p->s1074;
            *b = p->b20C_7;
        } else {
            *h = none;
            *b = 0;
        }
    }
}

struct Fn800E27CCA {
    u8 pad0[0x44];
    s8 idx; /* 0x44 */
    u8 pad45[0x63];
};

struct Fn800E27CCB {
    s16 *x0;
    u8 pad4[0x28];
    s16 x2C;
    u8 pad2E[2];
    s16 x30;
    u8 flags; /* 0x32 */
    u8 pad33[8];
    s8 x3B;
    s8 x3C;
    u8 pad3D[0x2B];
};

struct Fn800E27CCC {
    u8 pad0[2];
    u8 superkitTotal; /* 0x2, also referenced as gSuperkitTotal */
    u8 pad3[0x10E];
    u8 e[1][8]; /* 0x111 */
    u8 pad119[0xEF];
    u32 bits[1]; /* 0x208 */
};

extern s32 lbl_8040B1B0;
extern struct Fn800E27CCA lbl_801D68E0[];
extern struct Fn800E27CCB lbl_801DC074[];
extern struct Fn800E27CCC lbl_803E0298;
extern s32 lbl_8040B1FC;
extern s32 lbl_8040A6DC;

void fn_800E27CC(void) {
    s32 idx;
    struct Fn800E27CCA *a;
    struct Fn800E27CCB *b;
    a = &lbl_801D68E0[lbl_8040B1B0];
    idx = a->idx;
    b = &lbl_801DC074[idx];
    if (b->flags & 1) {
        lbl_8040B1FC = 0;
    } else if (lbl_803E0298.e[idx][0] != 0) {
        lbl_8040B1FC = 1;
    } else {
        lbl_8040B1FC = 0;
    }
    lbl_8040A6DC = 0;
}

extern f32 lbl_804098A0;
extern f32 lbl_804098A4;
extern f32 lbl_804098A8;

f32 fn_80104250(u8 *arg0) {
    f32 r;
    r = lbl_804098A0;
    if ((s8)arg0[0x284] > 1) {
        return r;
    }
    if (*(s32 *)(lbl_80409410 + 0x198) != 0x4C) {
        r = lbl_804098A4;
    }
    if (gPlayer1Pointer[0] != NULL && (*(s32 *)((u8 *)gPlayer1Pointer[0] + 0x20C) & 0x01000000) &&
        gPlayer1Pointer[1] != NULL && (*(s32 *)((u8 *)gPlayer1Pointer[1] + 0x20C) & 0x01000000)) {
        if (arg0 == gPlayer1Pointer[0]) {
            r += lbl_804098A8;
        } else {
            r -= lbl_804098A8;
        }
    }
    return r;
}

struct Fn800DF1ACEntry {
    f32 f0;
    f32 f4;
    f32 f8;
    f32 fC;
    u8 b10; /* 0x10 */
    u8 pad11[3];
};

struct Fn800DF1ACData {
    u8 pad0[2];
    u8 x2;
    u8 pad3[2];
    u8 x5;
    s8 x6;
    u8 pad7[5];
    s16 xC;
    s16 xE;
    u8 pad10[0x3C];
    u8 flags[0x20]; /* 0x4C */
    struct Fn800DF1ACEntry e[2]; /* 0x6C, also referenced as lbl_801DCFC8 */
};

extern struct Fn800DF1ACData lbl_801DCF5C;

void fn_800DF1AC(s32 arg0, f32 arg1, f32 arg2, s32 arg3) {
    s32 i;
    struct Fn800DF1ACEntry *e;
    e = lbl_801DCF5C.e;
    for (i = 0; i < 2; i++, e++) {
        if (lbl_801DCF5C.flags[i] != 0 && (arg0 == -1 || i == arg0)) {
            if (arg1 > 0.0f && (e->f4 <= 0.0f || arg1 > e->f0 * (e->f4 / e->f8))) {
                e->f0 = arg1;
                e->f8 = arg1;
                e->f4 = arg1;
            }
            if (arg2 > e->fC) {
                e->fC = arg2;
            }
            if (arg3 > 0) {
                arg3++;
                if (arg3 > e->b10) {
                    e->b10 = arg3;
                }
            }
        }
    }
}

extern s32 lbl_8040B1F0;

s32 fn_800DF2D4(struct Fn800DF25CArg *arg0) {
    f32 lim;
    if (lbl_8040B1F0 == lbl_803E0298.superkitTotal) {
        return 1;
    }
    if (arg0->f44 > arg0->f48) {
        arg0->f44 = 0.0f;
        lbl_8040B1F0++;
        lbl_8040B1F8 = 1.0f;
    } else {
        lim = arg0->f48 - lbl_804095A8;
        if (arg0->f44 > lim - lbl_804095AC && arg0->f44 <= lim) {
            lbl_8040B1F8 = (arg0->f48 - arg0->f44 - lbl_804095A8) / lbl_804095AC;
        }
    }
    return 0;
}

s32 fn_8007BC74(s32, s32, s32, s32, s32);
extern s32 lbl_80408C98;
extern s32 lbl_80409698;

void fn_800E3300(s32 arg0, s32 arg1, s32 arg2) {
    lbl_80408C98 = 1;
    lbl_80409698 = fn_8007BC74(arg0, arg0, arg1, arg2, 1);
}

struct Fn800E6338Entry {
    s8 state;
    u8 pad1[3];
    s32 i4; /* 0x4 */
    f32 t8;
    s8 countC;
    u8 padD[3];
    f32 t10;
    u8 pad14[4];
    s32 x18;
};

extern struct Fn800E6338Entry lbl_802E18F4[0x20];
extern s32 lbl_8040A718;

void fn_800E6338(void) {
    s32 i;
    memset(lbl_802E18F4, 0, sizeof(lbl_802E18F4));
    lbl_8040A718 = 0;
    for (i = 0; i < 0x20; i++) {
        lbl_802E18F4->i4 = -1;
    }
}

void fn_80013200(void *, s32);

void fn_80108A34(struct Fn80105194Self *arg0, s32 arg1) {
    s32 i;
    for (i = 0; i < arg0->count; i++) {
        if ((arg0->entries[i].flags & 0x18) == 0) {
            fn_80013200(&arg0->entries[i], arg1);
        }
    }
}

s32 RotDiff(s32, s32);
extern u16 gCameraAngles[];

s32 fn_801041D0(u8 *arg0) {
    u8 *q;
    s32 d;
    q = *(u8 **)(*(u8 **)(arg0 + 0xEEC) + 0x61C);
    if (*(s32 *)(q + 0x54) != 0 && *(f32 *)(q + 0x28) != 0.0f) {
        d = RotDiff((u16)(*(u16 *)(q + 0x26) + gCameraAngles[0]), *(u16 *)(arg0 + 0x27E));
        if (d < -0x4000 || d > 0x4000) {
            return 1;
        }
    }
    return 0;
}

s32 fn_800FB26C(void *arg0) {
    u8 *q;
    u16 v;
    s8 s;
    q = *(u8 **)((u8 *)arg0 + 0xEEC);
    v = *(u16 *)(q + 0x7A2);
    if ((s16)v != -1) {
        *(s16 *)((u8 *)arg0 + 0x4E) = v;
    } else {
        s = *(s8 *)(q + 0x7DF);
        if (s == 1) {
            *(s16 *)((u8 *)arg0 + 0x4E) = 8;
        } else if (s == 0x1D) {
            *(s16 *)((u8 *)arg0 + 0x4E) = 0x50;
        } else if (s == 2) {
            if (*(u8 *)(q + 0x7CE) != 0 && *(s32 *)(*(u8 **)(*(u8 **)((u8 *)arg0 + 0x60) + 0xC) + 0x5C) != 0) {
                *(s16 *)((u8 *)arg0 + 0x4E) = 0x17;
            } else {
                *(s16 *)((u8 *)arg0 + 0x4E) = 0xC;
            }
        } else if (s == 3) {
            *(s16 *)((u8 *)arg0 + 0x4E) = 0xF;
        } else if (s == 0xE) {
            *(s16 *)((u8 *)arg0 + 0x4E) = 0x24;
        } else if (s == 0xF) {
            *(s16 *)((u8 *)arg0 + 0x4E) = 0x26;
        } else if (s == 4) {
            if (*(u8 *)(q + 0x7BF) != 0) {
                *(s16 *)((u8 *)arg0 + 0x4E) = 0xF;
            } else {
                *(s16 *)((u8 *)arg0 + 0x4E) = 0x15;
            }
        } else {
            return 0;
        }
    }
    return 1;
}

void fn_80103794(f32 *arg0, f32 *arg1, f32 *arg2) {
    f32 t[3];
    t[0] = arg2[0] * lbl_80408E9C;
    if (t[0] > 1.0f) {
        t[0] = 1.0f;
    }
    t[1] = arg2[1] * lbl_80408E9C;
    if (t[1] > 1.0f) {
        t[1] = 1.0f;
    }
    t[2] = arg2[2] * lbl_80408E9C;
    if (t[2] > 1.0f) {
        t[2] = 1.0f;
    }
    if (arg1 != NULL) {
        arg0[0] = (arg1[0] - arg0[0]) * t[0] + arg0[0];
        arg0[1] = (arg1[1] - arg0[1]) * t[1] + arg0[1];
        arg0[2] = (arg1[2] - arg0[2]) * t[2] + arg0[2];
    } else {
        arg0[0] = arg0[0] - arg0[0] * t[0];
        arg0[1] = arg0[1] - arg0[1] * t[1];
        arg0[2] = arg0[2] - arg0[2] * t[2];
    }
}

typedef struct Fn800ED36CVec { f32 x, y, z; } Fn800ED36CVec;
typedef struct Fn800ED36CNode { Fn800ED36CVec v; u8 pad[0x34]; } Fn800ED36CNode;
typedef struct Fn800ED36CEec { u8 pad[0x1C4]; Fn800ED36CNode nodes[8]; } Fn800ED36CEec;
typedef struct Fn800ED36CList { u8 pad[0x14]; void *items[8]; } Fn800ED36CList;

void fn_800ED36C(void *arg0, Fn800ED36CVec *pos, Fn800ED36CVec *out) {
    u8 *p = arg0;
    s32 i;

    if (*(s16 *)(p + 0x1074) == 0x4A) {
        pos->x = *(f32 *)(p + 0x90);
        pos->z = *(f32 *)(p + 0x98);
    } else {
        i = *(s8 *)(*(u8 **)(*(u8 **)(p + 0x64) + 0x20) + 0xD2);
        if (i != -1 && *(u8 *)(p + 0x290) != 0 && *(Fn800ED36CList **)(p + 0x60) != NULL &&
            (*(Fn800ED36CList **)(p + 0x60))->items[i] != NULL) {
            pos->x = (*(Fn800ED36CEec **)(p + 0xEEC))->nodes[i].v.x;
            pos->z = (*(Fn800ED36CEec **)(p + 0xEEC))->nodes[i].v.z;
        } else {
            pos->x = *(f32 *)(p + 0x6C);
            pos->z = *(f32 *)(p + 0x74);
        }
    }
    pos->y = *(f32 *)(p + 0x220);
    if (out == NULL) {
        return;
    }
    i = *(s8 *)(*(u8 **)(*(u8 **)(p + 0x64) + 0x20) + 0xD3);
    if (i != -1 && *(u8 *)(p + 0x290) != 0 && *(Fn800ED36CList **)(p + 0x60) != NULL &&
        (*(Fn800ED36CList **)(p + 0x60))->items[i] != NULL) {
        *out = ((Fn800ED36CNode *)(*(u8 **)(p + 0xEEC) + 0x1C4))[i].v;
    } else {
        *out = *pos;
    }
}

void CatchUpCode(void *arg0, s32 arg1, f32 arg2, f32 arg3) {
    u8 *p = arg0;
    u8 *other;
    u8 *tbl;
    s32 team;
    u8 wrap;
    f32 off;
    f32 f;
    f32 d;

    if (*(s8 *)(p + 0x284) == -1) {
        return;
    }
    off = 0.0f;
    *(f32 *)(*(u8 **)(p + 0xEEC) + 0x724) = off;
    team = *(s8 *)(p + 0x601);
    if (team == -1) {
        return;
    }
    tbl = *(u8 **)(lbl_80409410 + 0x2DC);
    if (tbl == NULL) {
        return;
    }
    if (arg1 != 0 && !(*(s32 *)(p + 0x20C) & 0x01000000)) {
        return;
    }
    wrap = *(u8 *)(*(u8 **)tbl + team * 0x104 + 0x2F);
    other = NULL;
    if (p == gPlayer1Pointer[0]) {
        other = gPlayer1Pointer[1];
        if (other == NULL) {
            return;
        }
        if (!(*(s32 *)(p + 0x20C) & 0x01000000) || (*(s32 *)(other + 0x20C) & 0x01000000)) {
            goto check;
        }
        return;
    } else if (p == gPlayer1Pointer[1]) {
        other = gPlayer1Pointer[0];
        if (other == NULL) {
            return;
        }
        if ((*(s32 *)(p + 0x20C) & 0x01000000) && !(*(s32 *)(other + 0x20C) & 0x01000000)) {
            return;
        }
    }
check:
    if (other == NULL) {
        return;
    }
    if (*(s8 *)(other + 0x601) != team) {
        return;
    }
    if (arg1 != 0 && !(*(s32 *)(other + 0x20C) & 0x01000000)) {
        return;
    }
    f = *(f32 *)(other + 0x630);
    if (off > 0.0f) {
        f += off;
        if (f >= 1.0f) {
            if (wrap) {
                f -= 1.0f;
            } else {
                f = 1.0f;
            }
        }
    }
    d = *(f32 *)(p + 0x630) - f;
    if (wrap) {
        if (d >= 0.5f) {
            d -= 1.0f;
        } else if (d <= -0.5f) {
            d += 1.0f;
        }
    }
    if (d < 0.0f) {
        *(f32 *)(*(u8 **)(p + 0xEEC) + 0x724) = -d / arg2;
        if (*(f32 *)(*(u8 **)(p + 0xEEC) + 0x724) > arg3) {
            *(f32 *)(*(u8 **)(p + 0xEEC) + 0x724) = arg3;
        }
    }
}

extern void* memset();
typedef struct Fn800E333CState {
    s32 a;
    s32 b;
    s32 c;
    void *d;
    s32 e;
    s32 f;
    void *g;
    s32 h;
    s32 i;
    u8 pad[0x1380 - 0x24];
} Fn800E333CState;
extern Fn800E333CState lbl_803DED88;
extern s32 lbl_8040969C;
extern s32 lbl_804096A0;
extern s32 lbl_804096A4;
extern s32 lbl_804096A8;
extern s32 lbl_804096AC;
extern s32 lbl_804096B0;
extern s32 lbl_804096B4;
extern u8 lbl_801DD000[];
extern u8 lbl_801DD080[];

void fn_800E333C(void) {
    memset(&lbl_803DED88, 0, sizeof(lbl_803DED88));
    lbl_803DED88.a = lbl_8040969C;
    lbl_803DED88.b = lbl_804096A0;
    lbl_803DED88.c = lbl_804096A4;
    lbl_803DED88.d = lbl_801DD000;
    lbl_803DED88.e = lbl_804096A8;
    lbl_803DED88.f = lbl_804096AC;
    lbl_803DED88.g = lbl_801DD080;
    lbl_803DED88.h = lbl_804096B0;
    lbl_803DED88.i = lbl_804096B4;
}

typedef struct Fn801039DCMtx { f32 m[16]; } Fn801039DCMtx;
void fn_8000E498(Fn801039DCMtx *, u32);

void fn_801039DC(void *arg0, Fn801039DCMtx *arg1) {
    *arg1 = *(Fn801039DCMtx *)((u8 *)arg0 + 0xC8);
    fn_8000E498(arg1, 0x8000);
}

extern s32 lbl_801A997C[];

void fn_80104E24(void *arg0) {
    u8 *p = arg0;
    s8 s = *(s8 *)(*(u8 **)(p + 0xEEC) + 0x7DF);

    if (s == 0x1B) {
        *(s16 *)(p + 0x4E) = 0x47;
    } else if (s == 0x23 && *(s32 *)(*(u8 **)(*(u8 **)(p + 0x60) + 0xC) + 0x18) != 0) {
        *(s16 *)(p + 0x4E) = 6;
    } else {
        *(s16 *)(p + 0x4E) = 1;
    }
    if (*(f32 *)(*(u8 **)(*(u8 **)(p + 0xEEC) + 0x61C) + 0x28) > 0.0f) {
        fn_8006DCD4(lbl_801A997C, p + 0x90);
    }
}

extern s32 lbl_8040AFB8;
extern u8 lbl_801D5988[];
void NuVecRotateY(f32 *, void *, s32);

s32 fn_800E652C(void *arg0) {
    u8 *p = arg0;
    f32 v[3];
    f32 h;
    f32 d;

    if (lbl_8040AFB8 == 0 && (*(s32 *)(p + 0x20C) & 0x01000000) &&
        *(f32 *)(*(u8 **)(*(u8 **)(p + 0xEEC) + 0x61C) + 0x28) > 0.0f &&
        !(*(u8 *)(p + 0x285) & 2) && *(u8 *)(p + 0x107C) != 0) {
        h = *(f32 *)(p + 0x1F0) / 0.21f;
        d = *(f32 *)(p + 0x18C) - *(f32 *)(p + 0x220);
        if (d > h * 0.035f && d < h * 0.125f) {
            NuVecRotateY(v, lbl_801D5988, *(u16 *)(p + 0x1068));
            if ((*(f32 *)(p + 0x638) - *(f32 *)(p + 0x6C)) * v[0] +
                    (*(f32 *)(p + 0x640) - *(f32 *)(p + 0x74)) * v[2] > 0.0f) {
                return 1;
            }
        }
    }
    return 0;
}

s32 fn_800B9ED4();

void fn_80103DC0(void *arg0) {
    u8 *p = arg0;
    s32 opts[3];
    s32 n = 0;
    s32 v;

    if (*(s32 *)(*(u8 **)(*(u8 **)(p + 0x60) + 0xC) + 0x74) != 0) {
        opts[n++] = 0x1D;
    }
    if (*(s32 *)(*(u8 **)(*(u8 **)(p + 0x60) + 0xC) + 0x78) != 0) {
        opts[n++] = 0x1E;
    }
    if (*(s32 *)(*(u8 **)(*(u8 **)(p + 0x60) + 0xC) + 0x7C) != 0) {
        opts[n++] = 0x1F;
    }
    if (n > 1) {
    retry:
        v = opts[fn_800B9ED4() / (0xFFFF / n + 1)];
        if (v == *(s16 *)(*(u8 **)(p + 0xEEC) + 0x796)) {
            goto retry;
        }
    } else {
        v = opts[0];
    }
    *(s16 *)(*(u8 **)(p + 0xEEC) + 0x796) = v;
}

typedef struct Fn801034C4Entry { u8 pad[0x10]; } Fn801034C4Entry;
typedef struct Fn801034C4Eec { u8 pad0[0x650]; s32 x650; u8 pad654[0x18B]; u8 x7DF; } Fn801034C4Eec;
extern s32 lbl_804091F8;
extern Fn801034C4Entry *lbl_804097DC;
extern Fn801034C4Entry lbl_801DCB5C[];
Fn801034C4Entry *fn_800D9878(s32);
void fn_800D875C(s32, s32);

void fn_801034C4(void *arg0, s32 arg1) {
    Fn801034C4Eec *q = *(Fn801034C4Eec **)((u8 *)arg0 + 0xEEC);
    s32 idx;

    if (q->x7DF == 0x22 || q->x650 != 0) {
        return;
    }
    q->x650 = arg1;
    if (lbl_804091F8 != 0 || lbl_80409008 != 0) {
        return;
    }
    if (lbl_804097DC == NULL) {
        lbl_804097DC = fn_800D9878(0x26B);
        if (lbl_804097DC == NULL) {
            return;
        }
    }
    idx = lbl_804097DC - lbl_801DCB5C;
    if (!((lbl_803E0298.bits[idx / 32] >> (idx & 31)) & 1)) {
        fn_800D875C(0x26B, 0);
    }
}

s32 PauseMenu_HandleEntry1(s32 *);

void fn_800E6394(s32 *arg0, s32 arg1, f32 arg2, s32 arg3, f32 arg4) {
    if (arg1 == 1 && arg2 == 0.0f) {
        fn_8006DE5C(PauseMenu_HandleEntry1(arg0), arg3);
        return;
    }
    if (arg2 > 0.0f) {
        lbl_802E18F4[lbl_8040A718].state = 1;
    } else {
        lbl_802E18F4[lbl_8040A718].state = 2;
    }
    lbl_802E18F4[lbl_8040A718].i4 = PauseMenu_HandleEntry1(arg0);
    lbl_802E18F4[lbl_8040A718].t8 = arg2;
    lbl_802E18F4[lbl_8040A718].countC = arg1;
    lbl_802E18F4[lbl_8040A718].t10 = arg4;
    lbl_802E18F4[lbl_8040A718].x18 = arg3;
    lbl_8040A718 = (lbl_8040A718 + 1) & 0x1F;
}

void fn_800AB428(s32, s32, s32);
void fn_80044FD4(void);
extern s32 lbl_804093B0;
extern s32 lbl_80408F1C;
extern s32 lbl_80408F18;
extern s32 lbl_80408F2C;
extern s32 lbl_804093FC;

void fn_800DF0D0(s32 arg0) {
    struct Fn800DF1ACData *d = &lbl_801DCF5C;

    fn_800AB428(1, d->xC, d->xE);
    if (d->x5 == 0x14) {
        if (d->x2 != 0) {
            lbl_804093B0 = 0x7B;
        } else {
            lbl_804093B0 = 0x7D;
            lbl_80408F18 = 1;
            lbl_80408F1C = 1;
        }
    } else {
        lbl_804093B0 = 0x7B;
        if (arg0 == 0 && d->x6 != -1) {
            lbl_804093B0 = *lbl_801DC074[d->x6].x0;
            if (lbl_801DC074[d->x6].flags & 4) {
                lbl_80408F2C = 1;
            } else {
                lbl_804093FC = 1;
            }
        }
    }
    fn_80044FD4();
}

void fn_800E346C(void) {
    struct Fn800E6338Entry *e = lbl_802E18F4;
    s32 i;

    for (i = 0; i < 0x20; i++, e++) {
        switch (e->state) {
        case 0:
            break;
        case 1:
            if (e->t8 > 0.0f) {
                e->t8 -= lbl_80408E9C;
            } else {
                e->state = 2;
            }
            break;
        case 2:
            fn_8006DE5C(e->i4, e->x18);
            if (--e->countC > 0) {
                e->state = 3;
                e->t8 = e->t10;
            } else {
                memset(e, 0, sizeof(*e));
                e->state = 0;
            }
            break;
        case 3:
            if (e->t8 > 0.0f) {
                e->t8 -= lbl_80408E9C;
            } else {
                e->state = 2;
            }
            break;
        }
    }
}

extern u8 *lbl_80408DF0;
extern f32 lbl_80409760[2];
extern f32 lbl_80409768[];
extern f32 lbl_80409770[2];
f32 fn_800215F0(void *);

void fn_800E82B8(void) {
    f32 len;
    s32 i;
    void *x;

    i = *(s16 *)(*(u8 **)(lbl_80408DF0 + 0x1C) + 0x44);
    if (i != -1 &&
        (x = *(void **)(*(u8 **)(*(u8 **)(lbl_80408DF0 + 0x18) + i * 0x54 + 0xC) + 4)) != NULL) {
        len = fn_800215F0(x);
    } else {
        len = 0.0f;
    }
    for (i = 0; i < 2; i++) {
        lbl_80409760[i] = 0.0f;
        lbl_80409768[i] = 0.0f;
        lbl_80409770[i] = 1.0f;
        if (len > 0.0f) {
            lbl_80409770[i] += (f32)fn_800B9ED4() * 1.5259022e-05f * (len - 1.0f);
        }
    }
}

typedef struct Fn800E2BA4Out {
    char *x0;
    char *x4;
    char *x8;
    char *xC;
} Fn800E2BA4Out;
extern s16 lbl_801DC9C4[][8];
typedef struct Fn800E2BA4Strs { char *s[0x4E0]; } Fn800E2BA4Strs;
extern char lbl_801A5D2C[];
extern int sprintf();
void NuStrUpr(char *, char *);

void fn_800E2BA4(s32 arg0, s32 *arg1, Fn800E2BA4Out *out, char *buf1, char *buf2) {
    s32 c;
    s32 b;
    char **t;

    if (arg0 != -1) {
        b = lbl_801DC074[arg0].x3B;
        c = lbl_801DC074[arg0].x3C;
        *arg1 = lbl_801DC074[arg0].x30;
    } else {
        *arg1 = arg0;
        c = -1;
        b = -1;
    }
    out->x0 = NULL;
    out->x4 = NULL;
    out->x8 = NULL;
    out->xC = NULL;
    if (arg0 == 0x14) {
        char **u = (char **)&lbl_803DED88;
        out->x0 = u[0x8EC / 4];
        out->x4 = u[0x8F0 / 4];
        return;
    }
    if (b == -1) {
        return;
    }
    if (lbl_801DC9C4[b][0] != -1) {
        out->x0 = ((Fn800E2BA4Strs *)&lbl_803DED88)->s[lbl_801DC9C4[b][0]];
    }
    if (lbl_801DC9C4[b][1] != -1) {
        out->x4 = ((Fn800E2BA4Strs *)&lbl_803DED88)->s[lbl_801DC9C4[b][1]];
    }
    if (c == -1) {
        return;
    }
    t = (char **)&lbl_803DED88;
    sprintf(buf1, lbl_801A5D2C, t[0x4B0 / 4], c + 1);
    out->x8 = buf1;
    if (lbl_801DC074[arg0].x2C != -1) {
        NuStrUpr(buf2, t[lbl_801DC074[arg0].x2C]);
        out->xC = buf2;
    }
}

u16 SeekRot(u16, u16, f32);
f32 *fn_800ABA50(void *);

typedef struct { u8 pad0[0x78]; f32 x78; f32 x7C; f32 x80; u8 pad84[0xF3C - 0x84]; f32 xF3C; f32 xF40; f32 xF44; } Fn800EA4B4Obj;

void MovePlayer_PIVOT(void *arg0) {
    u8 *p = arg0;
    u8 *q;
    Fn800EA4B4Obj *o = arg0;
    f32 *v;
    f32 k[3];
    f32 t;
    f32 dt;
    f32 a, b, c, d;

    q = *(u8 **)(*(u8 **)(p + 0xEEC) + 0x61C);
    *(u8 *)(p + 0x107E) = 3;
    *(u16 *)(p + 0x27C) = SeekRot(*(u16 *)(p + 0x27C), (s32)(*(f32 *)(q + 0x30) * 5461.0f), 8.0f);
    *(u16 *)(*(u8 **)(p + 0xEEC) + 0x7AE) =
        SeekRot(*(u16 *)(*(u8 **)(p + 0xEEC) + 0x7AE), (s32)(*(f32 *)(q + 0x2C) * 5461.0f), 8.0f);
    if (*(f32 *)(p + 0x210) != 0.0f || *(f32 *)(p + 0x218) != 0.0f) {
        o->xF3C = *(f32 *)(p + 0x210);
        o->xF44 = *(f32 *)(p + 0x218);
        *(f32 *)(p + 0x218) = 0.0f;
        *(f32 *)(p + 0x210) = 0.0f;
    } else {
        v = fn_800ABA50(p);
        o->xF3C = (v[0] - *(f32 *)(p + 0x6C)) * 5.0f;
        o->xF44 = (v[2] - *(f32 *)(p + 0x74)) * 5.0f;
    }
    k[2] = 10.0f;
    k[1] = 10.0f;
    k[0] = 10.0f;
    dt = lbl_80408E9C;
    a = o->x78;
    b = o->xF3C;
    t = dt * k[0];
    if (t > 1.0f) {
        t = 1.0f;
    }
    c = o->x80;
    d = o->xF44;
    o->x78 = (b - a) * t + a;
    t = dt * k[2];
    if (t > 1.0f) {
        t = 1.0f;
    }
    o->x80 = (d - c) * t + c;
}

void *fn_8008F0C8(void *, void *, s32);

void *fn_80108AAC(void *arg0, void *arg1, s32 arg2, s32 arg3) {
    u8 *p;

    p = fn_8008F0C8(arg0, arg1, 0x1C);
    if (p != NULL && arg2 != 0) {
        memset(p, 0, 0x1C);
        *(void **)(p + 4) = fn_8008F0C8(arg0, arg1, arg3 * 0x160);
        if (*(void **)(p + 4) != NULL) {
            *(s32 *)(p + 0x18) = arg2;
            *(s32 *)(p + 0) = arg3;
            fn_80108B3C(p);
        }
    }
    return p;
}

typedef struct Fn8010439CEntry {
    void *obj;
    f32 v[3];
    u8 x10[4];
    s16 x14;
    u8 pad16[2];
} Fn8010439CEntry;

typedef struct Fn8010439CList {
    Fn8010439CEntry e[64];
    s32 count;
} Fn8010439CList;

extern Fn8010439CList lbl_802E1D1C;
void fn_800136CC(void *, f32 *, void *);
void *fn_800133FC(void *);
void NuVecMtxTransform(f32 *, f32 *, void *);

void fn_8010439C(void *obj, s16 arg1) {
    void *m;

    if (lbl_802E1D1C.count < 0x40) {
        lbl_802E1D1C.e[lbl_802E1D1C.count].obj = obj;
        lbl_802E1D1C.e[lbl_802E1D1C.count].x14 = arg1;
        fn_800136CC(obj, lbl_802E1D1C.e[lbl_802E1D1C.count].v, lbl_802E1D1C.e[lbl_802E1D1C.count].x10);
        m = fn_800133FC(obj);
        NuVecMtxTransform(lbl_802E1D1C.e[lbl_802E1D1C.count].v, lbl_802E1D1C.e[lbl_802E1D1C.count].v, m);
        lbl_802E1D1C.count++;
    }
}

extern f32 lbl_8040B000;
void NuVecRotateX(f32 *, f32 *, s32);
void NuVecAdd(f32 *, f32 *, f32 *);

f32 fn_801042D4(void *arg0) {
    u8 *p = arg0;
    f32 r = 0.0f;
    f32 v[3];

    if (*(s8 *)(p + 0x601) != -1 && lbl_8040B000 != 0.0f) {
        r = lbl_8040B000 * lbl_8040AFE8;
        r *= *(f32 *)(*(u8 **)(p + 0xEEC) + 0x724) + 1.0f;
        v[1] = 0.0f;
        v[0] = 0.0f;
        v[2] = r;
        NuVecRotateX(v, v, *(u16 *)(p + 0x624));
        NuVecRotateY(v, v, *(u16 *)(p + 0x626));
        NuVecAdd((f32 *)(p + 0xF3C), (f32 *)(p + 0xF3C), v);
    }
    return r;
}

s32 fn_8000ABDC(f32, f32);
void ResetPlayerContexts(void *);
extern f32 lbl_804097BC;

void fn_80103ED8(void *arg0, f32 *arg1) {
    u8 *p = arg0;

    *(u16 *)(p + 0x1068) = fn_8000ABDC(arg1[0], arg1[2]) - 0x8000;
    *(f32 *)(p + 0x7C) = 2.0f;
    *(u8 *)(p + 0x285) = 0;
    *(u16 *)(p + 0x1064) = 0;
    fn_80095558(p + 8, -1);
    fn_800ABA78(p);
    ResetPlayerContexts(*(void **)(p + 0xEEC));
    *(u8 *)(*(u8 **)(p + 0xEEC) + 0x7DF) = 0x26;
    *(f32 *)(*(u8 **)(p + 0xEEC) + 0x714) = lbl_804097BC;
}

s32 fn_800130F4(void *);
void fn_8000DC80(void *, f32 *);
void NuMtxPreScale(void *, f32 *);
extern f32 lbl_8040904C;

void fn_80103F84(void *obj) {
    f32 t[3];
    f32 s[3];
    u8 *q;
    void *m;

    if (obj != NULL && fn_800130F4(obj) != 0) {
        if (lbl_8040973C != 0) {
            fn_80013200(obj, 1);
            m = fn_800133FC(obj);
            q = (u8 *)lbl_8040973C;
            t[0] = *(f32 *)(q + 0x1AC);
            t[1] = *(f32 *)(q + 0x220) + lbl_8040904C;
            t[2] = *(f32 *)(q + 0x1B4);
            s[2] = lbl_8040A720;
            s[1] = lbl_8040A720;
            s[0] = lbl_8040A720;
            fn_8000DC80(m, t);
            NuMtxPreScale(m, s);
        } else {
            fn_80013200(obj, 0);
        }
    }
}

void fn_800AFF58(char *);
void *fn_80013380(void *);
void fn_800B01C4(void *, void *, f32, f32, f32, f32, s32, s32);
void fn_800B0120(void *, s32, f32, f32, f32, f32);

void fn_80106128(void *arg0) {
    u8 *p = arg0;
    s32 i;
    s32 found;
    u8 *e;
    u8 *h;
    void *d;

    if (*(void **)(p + 0x6C) != NULL && *(f32 *)(p + 0x7C) - *(f32 *)(p + 0x78) > 0.0f) {
        if (*(f32 *)(p + 0x78) == 0.0f && *(f32 *)(p + 0x80) == 0.0f) {
            fn_800AFF58(MenuHeader);
        }
        found = -1;
        h = *(u8 **)(p + 0x6C);
        for (i = 0; i < *(u8 *)(p + 0x1E) && found == -1;) {
            if (p[0xD8 + i * 0x34] & 0x20) {
                found = *(u8 *)(p + 0x1E);
            } else {
                i++;
            }
        }
        if (found != -1) {
            d = *(void **)(h + 8);
            fn_800B01C4(d, fn_80013380(p + 0xA8 + i * 0x34), *(f32 *)(p + 0x78), *(f32 *)(p + 0x7C),
                        *(f32 *)(p + 0x80), *(f32 *)(p + 0x84), 1, 1);
        } else {
            fn_800B0120(h, 1, *(f32 *)(p + 0x78), *(f32 *)(p + 0x7C), *(f32 *)(p + 0x80), *(f32 *)(p + 0x84));
        }
    }
}

extern f32 lbl_80409878;
extern f32 lbl_80409874;
s32 fn_800AC020(s32, f32);
extern s32 lbl_801A9620[];

void fn_800FC39C(void *arg0) {
    u8 *p = arg0;

    if (*(s8 *)(p + 0x284) == -1) {
        return;
    }
    if (!(*(u32 *)(*(u8 **)(p + 0xEEC) + 0x7B0) & 0x4000000) && *(u8 *)(p + 0x289) == 0x11 &&
        *(f32 *)(p + 0x18C) - *(f32 *)(p + 0x220) < lbl_80409878) {
        fn_800AC020(**(s32 **)(*(u8 **)(p + 0xEEC) + 0x61C), 0.1f);
        NewRumble(**(s32 **)(*(u8 **)(p + 0xEEC) + 0x61C), 0.5f);
        *(f32 *)(*(u8 **)(p + 0xEEC) + 0x708) = lbl_80409874;
        *(f32 *)(*(u8 **)(p + 0xEEC) + 0x774) = 1.0f;
        if (*(s32 *)(*(u8 **)(p + 0xEEC) + 0x7B0) >= 0) {
            fn_8006DCD4(lbl_801A9620, p + 0x90);
            *(u32 *)(*(u8 **)(p + 0xEEC) + 0x7B0) |= 0x80000000;
        }
    } else {
        *(u32 *)(*(u8 **)(p + 0xEEC) + 0x7B0) &= ~0x80000000;
        if (*(f32 *)(*(u8 **)(p + 0xEEC) + 0x774) > 0.0f) {
            *(f32 *)(*(u8 **)(p + 0xEEC) + 0x774) -= lbl_80408E9C;
            if (*(f32 *)(*(u8 **)(p + 0xEEC) + 0x774) < 0.0f) {
                *(f32 *)(*(u8 **)(p + 0xEEC) + 0x774) = 0.0f;
            }
        }
    }
    if (*(f32 *)(*(u8 **)(p + 0xEEC) + 0x708) > 1.0f) {
        NewRumble(**(s32 **)(*(u8 **)(p + 0xEEC) + 0x61C), *(f32 *)(*(u8 **)(p + 0xEEC) + 0x708) - 1.0f);
    }
}

f32 fn_8000CD08(void *, void *, void *);

s32 ForcedBackCode(void *arg0) {
    u8 *p = arg0;
    f32 d;
    s32 ok;
    f32 r;

    if (lbl_8040973C != 0) {
        d = fn_8000CD08(p + 0x90, (u8 *)lbl_8040973C + 0x90, NULL);
    } else {
        d = 1000000.0f;
    }
    if (*(u8 *)(*(u8 **)(p + 0xEEC) + 0x7DF) == 0x28) {
        if (*(f32 *)(*(u8 **)(p + 0xEEC) + 0x714) > 0.0f) {
            *(f32 *)(*(u8 **)(p + 0xEEC) + 0x714) -= lbl_80408E9C;
            if (*(f32 *)(*(u8 **)(p + 0xEEC) + 0x714) < 0.0f) {
                *(f32 *)(*(u8 **)(p + 0xEEC) + 0x714) = 0.0f;
            }
        }
        if (lbl_8040973C != 0) {
            r = lbl_8040A720 + 0.2f;
            ok = 0;
            if (d > r * r) {
                ok = 1;
            }
        } else {
            ok = 1;
        }
        if (ok && *(f32 *)(*(u8 **)(p + 0xEEC) + 0x714) <= 0.0f) {
            *(s8 *)(*(u8 **)(p + 0xEEC) + 0x7DF) = -1;
            *(f32 *)(*(u8 **)(p + 0xEEC) + 0x6E8) = 0.0f;
        }
    } else if (lbl_8040973C != 0 && (s32)p != lbl_8040973C) {
        r = lbl_8040A720 - 0.2f;
        if (d < r * r) {
        fn_800ABA78(p);
        ResetPlayerContexts(*(void **)(p + 0xEEC));
        *(u8 *)(*(u8 **)(p + 0xEEC) + 0x7DF) = 0x28;
        *(f32 *)(*(u8 **)(p + 0xEEC) + 0x714) = 0.5f;
        }
    }
}

void FallLandCode(void *arg0) {
    u8 *p = arg0;
    u8 *e = *(u8 **)(p + 0xEEC);

    if (*(s8 *)(e + 0x7DF) == -1 && *(u8 *)(e + 0x7CB) == 0 && *(u8 *)(p + 0x285) != 0 &&
        *(u8 *)(p + 0x286) == 0 && *(f32 *)(*(u8 **)(e + 0x61C) + 0x28) == 0.0f &&
        *(f32 *)(e + 0x778) >= lbl_8040987C) {
        PlayLandSfx(p, 0);
        if (*(s16 *)(p + 0x1074) == 0x4A && *(u8 *)(*(u8 **)(p + 0xEEC) + 0x7B4) == 0) {
            *(s16 *)(*(u8 **)(p + 0xEEC) + 0x7A2) = 0x59;
        } else if (*(s16 *)(p + 0x1074) != 7 && *(s32 *)(*(u8 **)(*(u8 **)(p + 0x60) + 0xC) + 0x30) != 0) {
            *(s16 *)(*(u8 **)(p + 0xEEC) + 0x7A2) = 0xC;
        } else {
            *(s16 *)(*(u8 **)(p + 0xEEC) + 0x7A2) = 8;
        }
        if (*(s16 *)(*(u8 **)(p + 0xEEC) + 0x7A2) != -1 &&
            (*(s32 **)(*(u8 **)(p + 0x60) + 0xC))[*(s16 *)(*(u8 **)(p + 0xEEC) + 0x7A2)] == 0) {
            *(s16 *)(*(u8 **)(p + 0xEEC) + 0x7A2) = -1;
            return;
        }
        *(u8 *)(*(u8 **)(p + 0xEEC) + 0x7DF) = 1;
        fn_80095558(p + 8, -1);
        *(f32 *)(*(u8 **)(p + 0xEEC) + 0x778) = 0.0f;
        *(f32 *)(*(u8 **)(p + 0xEEC) + 0x6E4) =
            AnimDuration(*(s16 *)(p + 0x1074), *(s16 *)(*(u8 **)(p + 0xEEC) + 0x7A2), 0.0f, 0.0f, 1);
    }
}

extern f32 lbl_801A9478[];
extern f32 lbl_801A947C[];

s32 fn_800F8414(void *arg0, s32 arg1, s32 arg2)
{
  f32 temp_f0;
  f32 temp_f0_2;
  f32 temp_f13;
  f32 temp_f13_2;
  s32 var_r6;
  void *temp_r11;
  void *temp_r11_2;
  void *temp_r7;
  void *temp_r8;
  void *temp_r9;
  var_r6 = 0;
  if (((s16) (*((s16 *) (((s8 *) arg0) + 0x1074)))) == 0x4A)
  {
    var_r6 = (*((u8 *) (((s8 *) (*((void **) (((s8 *) arg0) + 0xEEC)))) + 0x7B4))) == 0;
  }
  temp_r7 = *((void **) (((s8 *) arg0) + 0xEEC));
  temp_r8 = *((void **) (((s8 *) arg0) + 0x64));
  if (arg1 != 0)
  {
    temp_r11 = *((void **) (((s8 *) temp_r8) + 0x20));
    temp_f13 = *((f32 *) (((s8 *) (*((void **) (((s8 *) temp_r7) + 0x61C)))) + 0x28));
    temp_f0 = ((*((f32 *) (((s8 *) temp_r11) + 0x10))) + (*((f32 *) (((s8 *) temp_r11) + 0x14)))) * lbl_801A9478[0];
    if (temp_f13 <= temp_f0)
    {
      if ((var_r6 != 0) && (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x144)))) != 0))
      {
        *((s16 *) (((s8 *) arg0) + 0x4E)) = 0x51;
        return;
      }
      if (((arg2 != 0) && ((*((f32 *) (((s8 *) temp_r7) + 0x6BC))) == lbl_801A947C[0])) && (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x114)))) != 0))
      {
        *((s16 *) (((s8 *) arg0) + 0x4E)) = 0x45;
        return;
      }
      *((s16 *) (((s8 *) arg0) + 0x4E)) = 5;
      return;
    }
  }
  temp_r9 = *((void **) (((s8 *) temp_r8) + 0x20));
  temp_f13_2 = *((f32 *) (((s8 *) (*((void **) (((s8 *) temp_r7) + 0x61C)))) + 0x28));
  temp_f0_2 = ((*((f32 *) (((s8 *) temp_r9) + 0x14))) + (*((f32 *) (((s8 *) temp_r9) + 0x18)))) * lbl_801A9478[0];
  if (temp_f13_2 <= temp_f0_2)
  {
    ;
    if ((var_r6 != 0) && (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x148)))) != 0))
    {
      *((s16 *) (((s8 *) arg0) + 0x4E)) = 0x52;
      return;
    }
    if (((arg2 != 0) && ((*((f32 *) (((s8 *) temp_r7) + 0x6BC))) == lbl_801A947C[0])) && (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x118)))) != 0))
    {
      *((s16 *) (((s8 *) arg0) + 0x4E)) = 0x46;
      return;
    }
    if ((((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x17C)))) != 0) && ((*((s32 *) (((s8 *) arg0) + 0xF28))) & 0x2000))
    {
      *((s16 *) (((s8 *) arg0) + 0x4E)) = 0x5F;
      return;
    }
    *((s16 *) (((s8 *) arg0) + 0x4E)) = 0;
    return;
  }
  if ((var_r6 != 0) && (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x14C)))) != 0))
  {
    *((s16 *) (((s8 *) arg0) + 0x4E)) = 0x53;
    return;
  }
  if (((arg2 != 0) && ((*((f32 *) (((s8 *) temp_r7) + 0x6BC))) == lbl_801A947C[0])) && (((s32) (*((s32 *) (((s8 *) (*((void **) (((s8 *) (*((void **) (((s8 *) arg0) + 0x60)))) + 0xC)))) + 0x68)))) != 0))
  {
    *((s16 *) (((s8 *) arg0) + 0x4E)) = 0x1A;
    return;
  }
  *((s16 *) (((s8 *) arg0) + 0x4E)) = 4;
}
