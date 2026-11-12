/*
 * HVQM4 16-bit PCM copiers (hvqm4decsnd.a).
 *
 * Provenance: adapted from ShulkMaster/mk-da (CC0-1.0), commit
 * 3d6f17751309eb5e7cb5b6162c21360cbb5a35bf, src/hvqm4dec/, which matches the
 * same HVQM4 1.5 build with GC/1.2.5. Symbols use the GL5E4F names in
 * config/GL5E4F/symbols.txt. See docs/sdk_import.md#hvqm4-sound-tus.
 */
#include "hvqm4/hvqm4snd.h"

void HVQM4DecodePcm16Ch1(const u8* code, u8* out, u32 flags, int samples,
                       int track) {
  int bytes = samples * 2;
  bytes *= track;
  code += bytes;
  for (; samples > 0; samples--) {
    out[0] = out[2] = code[0];
    out[1] = out[3] = code[1];
    code += 2;
    out += 4;
  }
}

void HVQM4DecodePcm16Ch2(const u8* code, u8* out, u32 flags, int samples,
                       int track) {
  int bytes = samples * 4;
  bytes *= track;
  code += bytes;
  for (; samples > 0; samples--) {
    out[0] = code[0];
    out[1] = code[1];
    out[2] = code[2];
    out[3] = code[3];
    code += 4;
    out += 4;
  }
}

