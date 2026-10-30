#ifndef NUFPAR_H
#define NUFPAR_H

#include "types.h"

#define NUFPAR_COUNT 3
#define NUFPAR_BUFFER_SIZE 0x1000

/* GC accesses and opaque ranges: docs/symbol_donors/opensaga_matching.md. */
struct nufpar_s {
    char* fileBuffer;
    fileHandle file;
    u8 unknown008[0x504];
    s32 lineNum;
    u8 unknown510[0xC];
    s32 commandPos;
    u8 unknown520[0x20];
    s32 bufferEnd;
    u32 fileSize;
    u8 isUtf16;
    u8 isUtf8;
    u8 unknown54A[2];
};

#endif
