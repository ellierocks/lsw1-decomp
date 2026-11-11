/* HVQM4 decoder types. Adapted from ShulkMaster/mk-da (CC0-1.0),
 * commit 3d6f17751309eb5e7cb5b6162c21360cbb5a35bf, include/. */
#ifndef _HVQM4SND_H
#define _HVQM4SND_H

#include <dolphin/types.h>

typedef struct HVQM4SoundInfo {
  u8 channels;
  u8 unknown01;
  u8 format;
  u8 unknown03;
  u32 sampleRate;
} HVQM4SoundInfo;

typedef struct HVQM4AdpcmState {
  int sample;
  int index;
} HVQM4AdpcmState;

typedef struct HVQM4SoundContext {
  HVQM4SoundInfo info;
  HVQM4AdpcmState channel[2];
} HVQM4SoundContext;

typedef struct HVQM4SoundFrameHeader {
  u16 recordType;
  u16 flags;
  u32 dataSize;
} HVQM4SoundFrameHeader;

void fn_80189934(HVQM4AdpcmState* state, const u8* code, s16* out,
                       u32 flags, u32 samples, int track);
void fn_80189B30(HVQM4AdpcmState* state, const u8* code, s16* out,
                       u32 flags, u32 samples, int track);
void HVQM4DecodePcm16Ch1(const u8* code, u8* out, u32 flags, int samples,
                       int track);
void HVQM4DecodePcm16Ch2(const u8* code, u8* out, u32 flags, int samples,
                       int track);
void fn_80189FFC(HVQM4AdpcmState* state, const u8* code, s16* out,
                       u32 flags, u32 samples, int track);
HVQM4SoundContext* HVQM4DecSoundCreate(const HVQM4SoundInfo* info);
int HVQM4DecSoundClose(HVQM4SoundContext* context);
int HVQM4DecSoundDecode(HVQM4SoundContext* context,
                       const HVQM4SoundFrameHeader* header, u32 samples,
                       const u8* code, int track, void* out);

#endif
