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

void fn_800E356C(void *arg0, s32 arg1) {
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

s32 fn_800E6638(s32, s32, s32, f32, f32);
s32 fn_800E6BF8();
extern f32 lbl_801A9A14[];

void fn_80103988(s32 arg0) {
    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9A14[0], lbl_801A9A14[0]);
}

s32 fn_80074934(void *);

void fn_801045B0(void *arg0) {
    (*(s32 *)((s8 *)(arg0) + (0x104))) = (s32) (((*(s32 *)((s8 *)(arg0) + (0x104))) | 0x80) & 0xFFFFBFFF);
    fn_80074934(arg0);
}

s32 fn_80095750(f32, f32);
s32 fn_80095A44(s32, s32, s32, s32, void *);
s32 fn_800A5C64();
extern f32 lbl_801A9AA0[];
extern f32 lbl_80408E9C;

void fn_80104628(void *arg0) {
    s32 temp_r30;

    (*(s16 *)((s8 *)(arg0) + (0x4E))) = 1;
    fn_800A5C64();
    if ((s32) (*(u8 *)((s8 *)(arg0) + (0x290))) != 0) {
        temp_r30 = fn_80095750(lbl_801A9AA0[0], lbl_80408E9C);
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

s32 fn_800E6638(s32, s32, s32, f32, f32);
s32 fn_800E6BF8();
extern f32 lbl_801A9AE4[];

void fn_80104EB4(s32 arg0) {
    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9AE4[0], lbl_801A9AE4[0]);
}

s32 fn_800E6638(s32, s32, s32, f32, f32);
s32 fn_800E6BF8();
extern f32 lbl_801A9AEC[];

void fn_80104F50(s32 arg0) {
    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9AEC[0], lbl_801A9AEC[0]);
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
s32 fn_80095750(f32, f32);
s32 fn_800959D4(s32, s32, void *, s32, s32, u16, void *);
extern s32 lbl_801A9720[];
extern s32 lbl_801A972C[];
extern f32 lbl_801A9734[];
extern f32 lbl_801A9738[];
extern f32 lbl_801A973C[];
extern f32 lbl_801A9740[];
extern f32 lbl_80408E9C;

void fn_800FEA64(void *arg0) {
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
            temp_r3 = fn_80095750(var_f1, lbl_80408E9C);
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

f32 fn_80092E18(s16, s32, f32, f32, s32);
s32 fn_80095558(void *, s32);
extern f32 lbl_801A9A58[];

void fn_80104054(void *arg0) {
    void *temp_r9;

    if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xC0))) != 0) {
        temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(s8 *)((s8 *)(temp_r9) + (0x7DF))) = 0x29;
        (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = fn_80092E18((*(s16 *)((s8 *)(arg0) + (0x1074))), 0x30, lbl_801A9A58[0], lbl_801A9A58[0], 1);
        fn_80095558(arg0 + 8, -1);
    }
}

f32 fn_80092E18(s16, s32, f32, f32, s32);
s32 fn_80095558(void *, s32);
extern f32 lbl_801A9A5C[];

void fn_801040CC(void *arg0) {
    void *temp_r9;

    if ((s32) (*(s32 *)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0x60)))) + (0xC)))) + (0xC4))) != 0) {
        temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        (*(s8 *)((s8 *)(temp_r9) + (0x7DF))) = 0x2A;
        (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = fn_80092E18((*(s16 *)((s8 *)(arg0) + (0x1074))), 0x31, lbl_801A9A5C[0], lbl_801A9A5C[0], 1);
        fn_80095558(arg0 + 8, -1);
    }
}

s32 fn_800D407C(s32, void *, s32);
s32 fn_800D4EC8(void *, f32);
extern f32 lbl_801A9A60[];
extern f32 lbl_801A9A64[];
extern const f32 lbl_80409890;

void fn_80104144(void *arg0, s32 arg1, s32 arg2) {
    void *temp_r11;
    void *temp_r3;

    if (arg1 != 0) {
        fn_800D407C(0, arg0, 1);
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
void fn_800AC0C0(f32, f32);
void fn_800AFFD8(void *, s32, s32, f32);
s32 fn_800B96A8(s32, s32);
extern char MenuHeader[];
extern f32 lbl_801A9A88[];
extern f32 lbl_801A9A8C[];
extern f32 lbl_801A9A90[];

void fn_80104534(s32 arg0, s32 arg1) {
    fn_800958F8((*(s32 *)((s8 *)(lbl_80409410) + (0x1B8))), 0x1A, arg0 + 0x30);
    fn_800AFFD8(MenuHeader, 0, 0, lbl_801A9A88[0]);
    fn_800B96A8(arg0, arg1);
    fn_800AC0C0(lbl_801A9A8C[0], lbl_801A9A90[0]);
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

s32 fn_800E6BF8();
s32 fn_800F3A84(void *, s32, s32, s32, s32);
extern f32 lbl_801A9AD0[];
extern f32 lbl_80408E9C;

void fn_80104C20(void *arg0) {
    void *temp_r9;
    void *temp_r9_2;

    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9AD0[0], lbl_801A9AD0[0]);
    temp_r9 = (*(void **)((s8 *)(arg0) + (0xEEC)));
    if ((s32) (*(u8 *)((s8 *)(temp_r9) + (0x7DF))) == 0x19) {
        (*(f32 *)((s8 *)(temp_r9) + (0x714))) = (f32) ((*(f32 *)((s8 *)(temp_r9) + (0x714))) - lbl_80408E9C);
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        if ((*(f32 *)((s8 *)(temp_r9_2) + (0x714))) <= lbl_801A9AD0[0]) {
            (*(s8 *)((s8 *)(temp_r9_2) + (0x7DF))) = -1;
        }
    }
    if ((*(s32 *)((s8 *)(arg0) + (0x20C))) & 0x01000000) {
        fn_800F3A84(arg0, 0, 0, -1, 0);
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

s32 fn_800E6BF8();
s32 fn_800F3A84(void *, s32, s32, s32, s32);
s32 fn_800F99D4(void *, s32, s32, s32, s32, s32);
s32 fn_800FB6B0(void *);
s32 fn_800FBB08(void *);
extern f32 lbl_801A9970[];
extern f32 lbl_80408E9C;

void fn_801025A8(void *arg0) {
    void *temp_r11;
    void *temp_r11_2;
    void *temp_r9;

    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9970[0], lbl_801A9970[0]);
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
        fn_800F3A84(arg0, 0, 0, -1, 0);
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

void fn_800E6208(void *arg0, s32 arg1) {
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

s32 fn_800D407C(s32, void *, s32);
extern f32 lbl_801A97D8[];
extern f32 lbl_801A97DC[];
extern f32 lbl_801A97E0[];
extern f32 lbl_801A97E4[];
extern f32 lbl_803E8420[];
extern const f32 lbl_80409890;
extern f32 lbl_80409894;
extern f32 lbl_8040A71C;

void fn_80100378(void *arg0) {
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
                    fn_800D407C(0, arg0, 1);
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

s32 fn_800E6BF8();
s32 fn_800F3A84(void *, s32, s32, s32, s32);
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
    fn_800E6BF8();
    fn_800E6638(arg0, 0, 0, lbl_801A9AC8[0], lbl_801A9AC8[0]);
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
        fn_800F3A84(arg0, 0, 0, -1, 0);
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
s32 fn_800F77A4(void *);
s32 fn_801044F0(void *);
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

    fn_800E6BF8();
    if ((s32) (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DF))) != 0x1B) {
        var_f1 = lbl_804098AC;
    } else {
        var_f1 = lbl_801A9924[0];
    }
    fn_800E6638(arg0, 0, 0, var_f1, lbl_801A9928[0]);
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
    fn_800F77A4(arg0);
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
        fn_800F3A84(arg0, 0, 0, -1, 0);
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
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = fn_80092E18((*(s16 *)((s8 *)(arg0) + (0x1074))), (*(s16 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7A6))), temp_f31, temp_f31, 1);
                    temp_r11_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
                    if ((*(f32 *)((s8 *)(temp_r11_2) + (0x714))) <= temp_f31) {
                        (*(f32 *)((s8 *)(temp_r11_2) + (0x714))) = (f32) lbl_801A910C[0];
                    }
                    (*(s16 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7A0))) = -1;
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x6C8))) = temp_f31;
                    (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7C3))) = 1U;
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x728))) = temp_f31;
                    (*(u8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7D8))) = temp_r30;
                    fn_800E356C(arg0, 2);
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

void fn_800E36B4(void *arg0) {
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

void fn_800FB364(void *arg0) {
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
                    (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = fn_80092E18((*(s16 *)((s8 *)(arg0) + (0x1074))), 0x27, temp_f31, temp_f31, 1);
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
                (*(f32 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x714))) = fn_80092E18((*(s16 *)((s8 *)(arg0) + (0x1074))), 0x27, lbl_801A95D0[0], lbl_801A95D0[0], 1);
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

void fn_800FB548(void *arg0) {
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
s32 fn_800ABFCC(s32, f32);
s32 fn_800B9ED4();
s32 fn_800D4F18(void *);
s32 fn_800D99AC();
s32 fn_800D9A60();
void fn_80103680(void *, void *, s32, f32, f32, f32);
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
        fn_80103680(arg0, (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650))) + 0x90, 1, 1.0f, 0.0f, 0.0f);
        temp_r30 = *(s32 *)(*(s32 **)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650)))) + (0xEEC)))) + (0x61C)));
        fn_800ABFCC(temp_r30, (f32) (f64) fn_800B9ED4() * lbl_801A93D0[0] * lbl_801A93D4[0]);
        temp_r9_2 = (*(void **)((s8 *)(arg0) + (0xEEC)));
        temp_f31 = 0.0f;
        temp_f13 = (*(f32 *)((s8 *)(temp_r9_2) + (0x718)));
        if (temp_f13 > temp_f31) {
            (*(f32 *)((s8 *)(temp_r9_2) + (0x718))) = (f32) (temp_f13 - lbl_80408E9C);
            temp_r9_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            if ((*(f32 *)((s8 *)(temp_r9_3) + (0x718))) <= temp_f31) {
                if (fn_800D407C(arg0, (*(void **)((s8 *)(temp_r9_3) + (0x650))), 1) == 2) {
                    fn_800D4F18((*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650))));
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
            fn_800ABFCC(temp_r30_2, (f32) (f64) fn_800B9ED4() * lbl_801A93D0[0] * lbl_801A93DC[0]);
            (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7DA))) = 1;
            temp_r11_3 = (*(void **)((s8 *)(arg0) + (0xEEC)));
            (*(void **)((s8 *)(temp_r11_3) + (0x6A4))) = (void *) (*(void **)((s8 *)(temp_r11_3) + (0x650)));
            (*(s8 *)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x7B3))) = 1;
            fn_80103680(arg0, (*(void **)((s8 *)((*(void **)((s8 *)(arg0) + (0xEEC)))) + (0x650))) + 0x90, 1, 1.0f, 0.0f, 0.0f);
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
                    fn_800D9A60();
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

void fn_80103680(void *arg0, void *arg1, s32 arg2, f32 base, f32 lo, f32 hi) {
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
