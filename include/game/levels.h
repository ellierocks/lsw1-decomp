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
    char *name;                   /* NuSplineFind compares it */
    char *pts;
};

/* One 0x94-byte entry of lbl_80409410->unk240 (unk244 entries). Consumer: InitDoors,
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
    void *unk1AC;                 /* episode_s; also s32 at +0x18/+0x1C, pointer at +0x40 */
    u8 pad1B0[0x1B8 - 0x1B0];
    s32 unk1B8;                   /* first argument of fn_800958F8 (Maul_A) */
    s32 unk1BC;                   /* first argument of fn_8010B780 (Maul_A) */
    s32 unk1C0;                   /* scene handle: first argument of NuSpecialFind */
    u8 pad1C4[0x240 - 0x1C4];
    struct lbl_80409410_240_s *unk240;
    s32 unk244;
    u8 pad248[0x1974 - 0x248];
    struct nugspline_s *unk1974;  /* fn_800CCDE4 family */
};

extern struct lbl_80409410_s *lbl_80409410;

/* Animated scene-instance state: OpenSaga `nuinstanim_s` (0x60 bytes). LSW1 reads anim_ix
 * with `lbz 0x5c` where the donor has a u16. `flags` < 0 (bit 31, the donor's `playing`
 * bitfield) while the animation runs. Consumers: the Retake_D/Retake_E callbacks. */
struct nuinstanim_s {
    u8 pad0[0x4C];
    f32 ltime;                    /* current frame */
    s32 flags;                    /* < 0: playing */
    u8 pad54[0x5C - 0x54];
    u8 anim_ix;                   /* index into nugscn_s.instanim_data */
    u8 pad5D[0x60 - 0x5D];
};

/* Scene behind a special handle. instanim_data[i] points at animation data whose first
 * f32 is its end frame (OpenSaga NuSpecialGetAnimEndFrame). */
struct nugscn_s {
    u8 pad0[0x2C];
    s32 numsplines;               /* NuSplineFind */
    struct nugspline_s *splines;
    u8 pad34[0x50 - 0x34];
    f32 **instanim_data;
};

/* Special handle filled by NuSpecialFind (12 bytes: OpenSaga nuhspecial_s). */
struct nuhspecial_s {
    struct nugscn_s *scene;
    void *special;
    void *display;
};

/* fn_800130F4 is OpenSaga NuSpecialExistsFn and fn_8001384C NuSpecialGetInstAnim; the unit
 * still declares both with older m2c prototypes, so callers cast them locally. */

/* Current episode behind lbl_80409410->unk1AC: an entry of lbl_801DC088. Confirmed live
 * in Maul_F (dolphin.py, 2026-11-08): levels -> 44, 45, 46, 47..., count 7. Consumer:
 * MaulFUpdate, which searches the chapters for the first with level-table flag 0x4000. */
struct episode_s {
    s16 *levels;                  /* level ids of the chapters */
    u8 pad4[0x3A - 0x4];
    u8 count;
};

/* Resolved position on the sock (camera rail) system: OpenSaga SOCKPOSITION, 0x38 bytes
 * (fn_800AAC04's caller frames reserve 0x38). Confirmed live in Cruiser_E (dolphin.py,
 * 2026-11-08): player objects keep one at +0x600; the active player read sock 2,
 * segment 0, ratio = distance = 0.287, midpoint beside the player's +0x6C position; an
 * inactive player reads sock -1. The level exit callbacks (level table +0x24) route on
 * `sock` and `distance`. */
struct sockposition_s {
    u8 unk0;
    s8 sock;                      /* -1: not on a sock */
    s16 segment;
    f32 ratio;
    struct nuvec_s midpoint;
    struct nuvec_s camera;
    s16 camera_rot[2];
    s16 midpoint_rot[2];
    s16 next_segment;
    s8 candidate_count;
    u8 flags;
    f32 distance;                 /* along the sock from its start */
    f32 normalized_distance;
    u32 candidate_mask;
};

/* creature.s: clears *pos (memset 0xC), averages the players into it with NuVecAdd and
 * NuVecScale; on success passes `out` to fn_80098190 (OpenSaga Players_AveragePos and
 * ComplexSockPosition), on failure stores -1 at out->sock. */
s32 fn_800AAC04(struct nuvec_s *pos, struct sockposition_s *out);

#endif
