#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

#include "types.h"

struct NUGAMEOBJECT_s;

struct NUCHARACTERDATA_s {
    char unk0[4];
    u32 flags04;
    char unk08[0x14];
    void (*drawFn)(struct NUGAMEOBJECT_s*);
    void* config;
    char unk24[8];
    f32 value2C;
    char unk30[8];
    f32 value38;
    char unk3C[4];
    u32 flags40;
    char unk44[4];
};

#endif
