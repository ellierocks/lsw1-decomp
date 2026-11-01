#ifndef GAME_PICKUPS_H
#define GAME_PICKUPS_H

#include "types.h"

struct NUPICKUP_s {
    char unk0[0x18];
    s32 link18;
    s32 link1C;
    char unk20[0xC];
    f32 value2C;
    char unk30[8];
    u8 flags38;
    u8 active39;
    char unk3A[6];
};

struct NUPICKUPPOOL_s {
    char unk0[0x710];
    s32 state710;
    char unk714[8];
    struct NUPICKUP_s* groups[6];
    s32 counts[6];
};

struct NUPICKUPVISUAL_s {
    char unk0[0x100];
    f32 value100;
    char unk104[0x1E];
    s16 kind122;
};

#endif
