#ifndef GAME_LEVELS_H
#define GAME_LEVELS_H

/* Types shared by auto/game/levels.c. Every field below is read by a function in the
 * unit (offsets from the retail assembly); unknown space stays padding. The sweep passes
 * this unit, preprocessed, to m2c as --context, so these types also shape its drafts. */

#include "types.h"
#include "numath/numath.h"

/* Nu2 game-scene spline: points are `ptsize` bytes apart, x/y/z first.
 * Consumers: the fn_800CCDE4 family (`lha 0x2`, `lwz 0x8`, `lfsx pts+ptsize`). */
struct nugspline_s {
    u8 pad0[2];
    s16 ptsize;
    u8 pad4[4];
    char *pts;
};

/* One 0x94-byte entry of lbl_80409410->unk240 (unk244 entries). Consumer: fn_800C9490,
 * which centres each 4-point spline and stores its largest point distance. */
struct lbl_80409410_240_s {
    u8 pad0[0x60];
    struct nugspline_s *spl;
    struct nuvec_s centre;
    f32 radius;
    u8 unk74[0x94 - 0x74];        /* first argument of fn_8000CBF4 */
};

/* Object behind the lbl_80409410 pointer (also read by pickups, obstacle and turret). */
struct lbl_80409410_s {
    u8 pad0[0x198];
    s32 unk198;                   /* compared with lbl_804093B0 and level ids */
    s32 unk19C;                   /* mode: 3, 9, 0xA, 0xB, 0xD, 0x15 (pickups mode19C) */
    u8 pad1A0[0x1A8 - 0x1A0];
    void *unk1A8;                 /* flags word at +4 (0x4000, 0x4002, 0x5000) */
    void *unk1AC;                 /* s32 at +0x18/+0x1C, pointer at +0x40 */
    u8 pad1B0[0x1C0 - 0x1B0];
    s32 unk1C0;                   /* scene handle: first argument of NuSpecialFind */
    u8 pad1C4[0x240 - 0x1C4];
    struct lbl_80409410_240_s *unk240;
    s32 unk244;
    u8 pad248[0x1974 - 0x248];
    struct nugspline_s *unk1974;  /* fn_800CCDE4 family */
};

extern struct lbl_80409410_s *lbl_80409410;

/* creature.s: clears *pos (memset 0xC), averages the players into it with NuVecAdd and
 * NuVecScale; on success passes `out` to fn_80098190, on failure stores -1 at out+1. */
s32 fn_800AAC04(struct nuvec_s *pos, void *out);

#endif
