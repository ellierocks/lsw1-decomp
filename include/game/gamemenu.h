#ifndef GAME_GAMEMENU_H
#define GAME_GAMEMENU_H

#include "types.h"

/* Runtime menu stack entry (gMenuStack, 0xE0 bytes each); see docs/menu_system.md.
 * Fields are named only where apimenu/apisave accesses confirm them. */
typedef struct MenuFrame {
    f32 t0;         /* 0x00: transition value, zeroed when a menu becomes current */
    f32 t4;         /* 0x04 */
    s16 sel8;       /* 0x08: horizontal selection, saved to MenuDef.sel8 on close */
    s16 selA;       /* 0x0A: vertical selection, saved to MenuDef.selA on close */
    s16 minC;       /* 0x0C */
    s16 minE;       /* 0x0E */
    s16 max10;      /* 0x10 */
    s16 max12;      /* 0x12 */
    s16 id;         /* 0x14: gRegisteredMenuDefinitions index, -1 when none */
    u8 prev16;      /* 0x16 */
    s8 sub17;       /* 0x17 */
    u8 unk18[0x50 - 0x18];
    s32 unk50;      /* 0x50 */
    u8 unk54[0xB4 - 0x54];
    s32 unkB4;      /* 0xB4 */
    s32 unkB8;      /* 0xB8 */
    s32 unkBC;      /* 0xBC */
    s32 unkC0;      /* 0xC0 */
    s32 unkC4;      /* 0xC4 */
    u8 unkC8[0xE0 - 0xC8];
} MenuFrame;

/* Registered menu descriptor (gRegisteredMenuDefinitions, 0x1C bytes each). The
 * callbacks are called without a prototype (crclr cr1eq before blrl). */
typedef struct MenuDef {
    s32 id;
    void (*enter)();
    void (*build)();
    void (*select)();
    void (*close)();
    s16 sel8;       /* 0x14: saved MenuFrame.sel8, -1 when not remembered */
    s16 selA;       /* 0x16: saved MenuFrame.selA, -1 when not remembered */
    s32 flags;
} MenuDef;

extern MenuFrame gMenuStack[];
extern MenuDef gRegisteredMenuDefinitions[];
extern s32 gMenuDepth;

/* Menu text draw. Parameter kinds and order follow the Mac LSW1 demo's mangled name
 * (docs/symbol_donors/mac_lsw1_demo_prototypes.tsv); ProDG loads arguments in this
 * order, so a class-sorted prototype schedules the float moves differently
 * (fn_80077038). Nine integer arguments: the last is passed on the stack. */
void fn_8007B298(char *text, f32 x, f32 y, f32 scale, f32 r, f32 g, f32 b, s32 a7, u8 u8_0, u8 u8_1,
                 u8 u8_2, f32 f6, s32 i8, void *p, s32 i10, s32 i11);
/* Same Mac-demo evidence: the shorter text draw without the scale/flags tail. */
void fn_8007AC80(char *text, f32 x, f32 y, f32 scale, f32 r, f32 g, f32 b, s32 a7, u8 u8_0,
                 u8 u8_1, u8 u8_2, u8 u8_3);

#endif
