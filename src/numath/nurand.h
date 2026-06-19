#ifndef NURAND_H
#define NURAND_H

#include "types.h"

struct nunrand_s {
    s32 idum;
};

u32 NuRandInt(void);
u32 NuRandIntPtr(u32* seed);
void NuRandSeed(u32 seed);
void NuRandSetSeed(struct nunrand_s* nrand, s32 seed);
s32 NuRand(struct nunrand_s* nrand);

#endif
