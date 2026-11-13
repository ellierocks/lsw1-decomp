#ifndef GAME_APIOBJECT_H
#define GAME_APIOBJECT_H

#include "types.h"

struct NUAPIOBJECT_s {
    char unk0[0x78];
    f32 velocityX;
    f32 velocityY;
    f32 velocityZ;
    f32 unk84;
    char unk88[4];
    f32 unk8C;
    f32 collisionX;
    f32 collisionY;
    f32 collisionZ;
    char unk9C[0xC];
    f32 positionX;
    f32 positionY;
    f32 positionZ;
    f32 mass;
    char unkB8[0x130];
    f32 stuckTime;
    f32 collisionRadius;
    f32 collisionHeight;
    char unk1F4[4];
    u64 identityMask;
    u64 contactMask;
    u32 flags208;
    u32 flags20C;
    f32 responseX;
    char unk214[4];
    f32 responseZ;
    char unk21C[0x18];
    f32 unk234;
    char unk238[4];
    f32 unk23C;
    f32 unk240;
    f32 unk244;
    char unk248[0x49];
    u8 sightIndex;
};

#endif
