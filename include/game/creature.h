#ifndef GAME_CREATURE_H
#define GAME_CREATURE_H

#include "game/character.h"

struct NUGAMEOBJECT_s {
    char unk0[0x60];
    void* characterModel;
    struct NUCHARACTERDATA_s* characterData;
    char unk68[0x1A0];
    u32 motionFlags;
    char unk20C[0x80];
    u8 drawResult;
    char unk28D[3];
    u8 drawUpdated;
    char unk291[0xC5B];
    void* playerPacket;
    char unkEF0[0x38];
    u32 flagsF28;
    char unkF2C[0x134];
    u32 drawFlags;
    char unk1064[0x1C];
    u8 drawMatrixEnabled;
    char unk1081[0x17];
};

#endif
