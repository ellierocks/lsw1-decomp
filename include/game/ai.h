#ifndef GAME_AI_H
#define GAME_AI_H

#include "game/apiobject.h"

struct NUAIPACKET_s {
    char unk0[0x50];
    struct NUAPIOBJECT_s *owner;
    struct NUAPIOBJECT_s *nearestOpponent;
    f32 nearestOpponentRange;
    char unk5C[8];
    struct NUAPIOBJECT_s *opponent;
    f32 opponentRange;
    char unk6C[0x8C];
    struct NUAIAREA_s *triggerArea;
    struct NUAILOCATOR_s *locator;
    char unk100[0x18];
    u32 pathFlags;
    char unk11C[0x84];
    u32 movementFlags;
    char unk1A4[0x1C];
    f32 timeOffPath;
};

struct NUAISCRIPTPROCESS_s {
    char unk0[0x24];
    f32 timer;
};

#endif
