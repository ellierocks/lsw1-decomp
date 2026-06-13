#ifndef NUSOUND_H
#define NUSOUND_H

#include "types.h"
#include "numath/numath.h"

struct NuSndLoopInfo_s {
    struct nuvec_s* pos;
    s16 playing, channel, timer, pad, vol_l, vol_r;
};

void NuSoundInit(void);
s32 NuSoundPlay(s32 bank, s32 id, s32 vol, s32 pan);
s32 NuSoundPlayLoop(s32 bank, s32 id, s32 vol, s32 pan);
void NuSoundUpdate(void);
void NuSoundKillAllAudio(void);

#endif
