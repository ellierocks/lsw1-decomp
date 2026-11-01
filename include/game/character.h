#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

#include "types.h"

struct NUGAMEOBJECT_s;

struct NUCHARACTERDATA_s {
    char unk0[0x1C];
    void (*drawFn)(struct NUGAMEOBJECT_s*);
};

#endif
