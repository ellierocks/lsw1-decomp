#ifndef GAME_APIOBJECT_H
#define GAME_APIOBJECT_H

#include "types.h"

struct NUAPIOBJECT_s {
    char unk0[0x78];
    f32 velocityX;
    f32 velocityY;
    f32 velocityZ;
    char unk84[0xC];
    f32 collisionX;
    f32 collisionY;
    f32 collisionZ;
    char unk9C[0x14C];
    f32 stuckTime;
    f32 collisionRadius;
    f32 collisionHeight;
    char unk1F4[4];
    u32 identityMask[2];
    u32 contactMask[2];
    u32 flags208;
    u32 flags20C;
    f32 responseX;
    char unk214[4];
    f32 responseZ;
};

#endif
