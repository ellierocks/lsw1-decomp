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
s32 fn_8012CE08(s32 arg0);
u8 fn_8012CE4C(s32 arg0);
extern s32 fn_8012CE74[];
extern s32 fn_8012CF28[];
s32 fn_8012D068(s32 arg0);
extern s32 GameAIActionParseSpeed[];
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
  fn_8008F444(GameAIActionParseSpeed);
  fn_8008F44C(fn_8012A2EC);
  fn_8008F28C(fn_8012A3F4);
  lbl_80408CE0 = 1;
  lbl_80408CDC = lbl_801ACF04[0];
  lbl_80408CE2 = (lbl_80408CE1 = 0);
  lbl_80408CE8 = (lbl_80408CE4 = lbl_801ACF08[0]);
}

extern s32 lbl_804093EC;

s32 fn_8012CE08(s32 arg0) {
    u8 temp_r3;

    if ((arg0 != 0xFF) && ((s32) lbl_804093EC != 0)) {
        temp_r3 = *(u8 *)(((arg0 * 2) + lbl_804093EC));
        if ((s32) temp_r3 != 0xFF) {
            return (*(s32 *)((s8 *)(((temp_r3 * 0x48) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x20))))) + (0xC)));
        }
    }
    return 0;
}

u8 fn_8012CE4C(s32 arg0) {
    if ((arg0 != 0xFF) && ((s32) lbl_804093EC != 0)) {
        return *(u8 *)(((arg0 * 2) + lbl_804093EC));
    }
    return 0xFFU;
}

s32 fn_8012D068(s32 arg0) {
    s16 temp_r3;

    if (arg0 != 0xFF) {
        temp_r3 = *(s16 *)(((arg0 * 2) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x1C)))));
        if (temp_r3 != -1) {
            return (*(s32 *)((s8 *)(((temp_r3 * 0x54) + (*(s32 *)((s8 *)(lbl_80408DF0) + (0x18))))) + (4)));
        }
    }
    return 0;
}

void *fn_8008F0C8(void *, void *, u32);
s32 fn_8012CB04(void *);
s32 memset();

void *fn_8012CA84(s32 arg0, s32 arg1, s32 arg2) {
    void *temp_r3;
    void *temp_r3_2;

    temp_r3 = fn_8008F0C8(arg0, arg1, 0x18);
    if (temp_r3 != NULL) {
        memset(temp_r3, 0, 0x18);
        temp_r3_2 = fn_8008F0C8(arg0, arg1, arg2 * 0xA8);
        (*(void **)((s8 *)(temp_r3) + (4))) = temp_r3_2;
        if (temp_r3_2 != NULL) {
            (*(s32 *)((s8 *)(temp_r3) + (0))) = arg2;
            fn_8012CB04(temp_r3);
        }
    }
    return temp_r3;
}

f32 fn_8000CD08(void *, void *, s32 *);
extern f32 lbl_801ACDB0[];

void *fn_801272FC(void *arg0, void *arg1, s32 arg2, s32 arg3) {
    char sp8[0x10];
    f32 temp_f1;
    f32 var_f31;
    s16 temp_r9;
    s32 temp_r0;
    s32 var_r28;
    void *var_r27;
    void *var_r31;
    var_r27 = NULL;
    if ((s32) (*(s32 *)((s8 *)(arg0) + (0x57C))) != 0) {
        return NULL;
    }
    var_f31 = lbl_801ACDB0[0];
    var_r28 = 0;
    var_r31 = (*(void **)((s8 *)(arg0) + (0x58C)));
    if (var_r28 < (s32) (*(s32 *)((s8 *)(arg0) + (0x590)))) {
        do {
            if (!((*(u16 *)((s8 *)(var_r31) + (0x20))) & 3) && ((arg2 == 0) || ((temp_r0 = (*(s32 *)((s8 *)((*(void **)((s8 *)(var_r31) + (0x4C)))) + (0x34))), (((temp_r0 & 0x02000000) == 0) == 0)) && (temp_r0 & 0x10000000) && ((*(s32 *)((s8 *)((*(void **)((s8 *)(arg1) + (0x64)))) + (4))) & 4) && ((temp_r9 = (*(s16 *)((s8 *)(var_r31) + (0x22))), ((temp_r9 == -1) != 0)) || (temp_r9 != (s16) (*(s16 *)((s8 *)(arg1) + (0x282))))))) && ((temp_f1 = fn_8000CD08(var_r31 + 0x78, arg1 + 0x90, sp8), ((arg3 == 0) == 0)) || (temp_f1 < (*(f32 *)((s8 *)(var_r31) + (0x74))))) && (temp_f1 < var_f31)) {
                var_f31 = temp_f1;
                var_r27 = var_r31;
            }
            var_r28 += 1;
            var_r31 += 0xF4;
        } while (var_r28 < (*(s32 *)((s8 *)(arg0) + (0x590))));
    }
    return var_r27;
}

struct forceobj_s {
    char *name;
    u8 id[0xF0];
};

extern s32 NuStrICmp(const char *a, const char *b);
extern s32 fn_80013890(void *a, void *b);

struct forceobj_s *FindForceObject(void *world, char *name, void *id) {
    struct forceobj_s *obj = *(struct forceobj_s **)((s8 *)world + 0x58C);
    s32 i;

    if (*(s32 *)((s8 *)world + 0x57C) == 0) {
        for (i = 0; i < *(s32 *)((s8 *)world + 0x590); i++, obj++) {
            if (id != NULL) {
                if (fn_80013890(obj->id, id) != 0) return obj;
            } else if (NuStrICmp(obj->name, name) == 0) {
                return obj;
            }
        }
    }
    return NULL;
}
