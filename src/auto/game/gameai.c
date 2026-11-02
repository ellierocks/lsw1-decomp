#include "types.h"

struct fn_8012CD28_s {
    s32 x;
    s32 y;
    s32 z;
};
extern struct fn_8012CD28_s lbl_804017EC[];
extern s32 lbl_8040B010;

void fn_8012CD28(s32 a, struct fn_8012CD28_s* b)
{
    lbl_8040B010 = a;
    lbl_804017EC[0] = *b;
}

extern void *lbl_80408DF0;

s32 fn_8012CEF4(s32 arg0) {
    if ((arg0 != 0xFF) && (arg0 < (s32) (*(s32 *)((s8 *)(lbl_80408DF0) + (0))))) {
        return (*(s32 *)((s8 *)(((arg0 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0xC)));
    }
    return 0;
}

s32 fn_80083A3C(s32 *);
s32 fn_80083A44(s32 *);
s32 fn_8008F224(s32 *);
s32 fn_8008F22C(s32 *);
s32 fn_8008F234(s32 *);
s32 fn_8008F23C(s32 *);
s32 fn_8008F244(s32 *);
s32 fn_8008F24C(s32 *);
s32 fn_8008F254(s32 *);
s32 fn_8008F25C(s32 *);
s32 fn_8008F264(s32 *);
s32 fn_8008F26C(s32 *);
s32 fn_8008F274(s32 *);
s32 fn_8008F27C(s32 *);
s32 fn_8008F284(s32 *);
s32 fn_8008F28C(s32 *);
s32 fn_8008F434(s32 *);
s32 fn_8008F43C(s32 *);
s32 fn_8008F444(s32 *);
s32 fn_8008F44C(s32 *);
s32 fn_8008F454(s32 *);
extern s32 fn_800AB85C[];
extern s32 fn_8012A138[];
extern s32 fn_8012A2EC[];
extern s32 fn_8012A3F4[];
extern s32 fn_8012A8C8[];
extern s32 fn_8012AD98[];
extern s32 fn_8012CD50[];
extern s32 fn_8012CE08[];
extern s32 fn_8012CE4C[];
extern s32 fn_8012CE74[];
extern s32 fn_8012CF28[];
extern s32 fn_8012D068[];
extern s32 fn_8012D0A4[];
extern s32 fn_8012D13C[];
extern s32 fn_8012D1CC[];
extern s32 fn_8012D1E8[];
extern s32 fn_8012D204[];
extern s32 fn_8012D220[];
extern f32 lbl_801ACF04[];
extern f32 lbl_801ACF08[];
extern s32 lbl_801E0D08[];
extern s32 lbl_801E1158[];
extern f32 lbl_80408CDC;
extern s8 lbl_80408CE0;
extern s8 lbl_80408CE1;
extern s8 lbl_80408CE2;
extern f32 lbl_80408CE4;
extern f32 lbl_80408CE8;

void fn_8012A5D0(void)
{
  fn_80083A3C(lbl_801E1158);
  fn_80083A44(lbl_801E0D08);
  fn_8008F224(fn_8012CD50);
  fn_8008F22C(fn_8012CE08);
  fn_8008F234(fn_8012CE4C);
  fn_8008F23C(fn_8012CE74);
  fn_8008F244(fn_8012CEF4);
  fn_8008F24C(fn_8012CF28);
  fn_8008F254(fn_8012D068);
  fn_8008F25C(fn_800AB85C);
  fn_8008F264(fn_8012D13C);
  fn_8008F26C(fn_8012D1CC);
  fn_8008F274(fn_8012D1E8);
  fn_8008F27C(fn_8012D204);
  fn_8008F284(fn_8012D220);
  fn_8008F454(fn_8012A138);
  fn_8008F434(fn_8012AD98);
  fn_8008F43C(fn_8012A8C8);
  fn_8008F444(fn_8012D0A4);
  fn_8008F44C(fn_8012A2EC);
  fn_8008F28C(fn_8012A3F4);
  lbl_80408CE0 = 1;
  lbl_80408CDC = lbl_801ACF04[0];
  lbl_80408CE2 = (lbl_80408CE1 = 0);
  lbl_80408CE8 = (lbl_80408CE4 = lbl_801ACF08[0]);
}
