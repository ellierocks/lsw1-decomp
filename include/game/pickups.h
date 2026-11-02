#ifndef GAME_PICKUPS_H
#define GAME_PICKUPS_H

#include "types.h"

struct NUPICKUP_s {
    char unk0[4];
    f32 y; /* fn_8013D9FC clamps +0x04. */
    char unk8[0x10];
    s32 link18;
    s32 link1C;
    f32 baseY;
    f32 previousY;
    char unk28[4];
    f32 value2C;
    char unk30[6];
    s16 kind36;
    u8 flags38;
    u8 active39;
    char unk3A[4];
    s8 kind3E;
    char unk3F;
};

struct NUPICKUPPOOL_s {
    char unk0[0x184];
    s32 buffer184; /* fn_8013EF38 aligns and advances the pool buffer. */
    s32 bufferEnd188;
    char unk18C[0x2c];
    void *effect1B8; /* fn_8013F0AC passes +0x1B8 to the pickup effect helper. */
    char unk1BC[0x554];
    s32 state710;
    s32 buffers[2]; /* fn_8013EF38 stores aligned buffer starts. */
    struct NUPICKUP_s* groups[6];
    s32 counts[6];
    char unk74C[4];
    s32 state750;
};

struct NUPICKUPVISUAL_s {
    char unk0[0x100];
    f32 value100;
    char unk104[0x1E];
    s16 kind122;
};

#endif
