#ifndef NUFPAR_H
#define NUFPAR_H

#include "types.h"

#define NUFPAR_COUNT 3
#define NUFPAR_BUFFER_SIZE 0x1000

struct nufpar_s;

struct nufpcom_s {
    char* name;
    void (*function)(struct nufpar_s*);
};

/* GC accesses and opaque ranges: docs/symbol_donors/opensaga_matching.md. */
struct nufpar_s {
    char* fileBuffer;
    fileHandle file;
    u8 unknown008[0x504];
    s32 lineNum;
    s32 linePos;
    s32 charPos;
    s32 bufferStart;
    s32 bufferEnd;
    struct nufpcom_s* commandStack[8];
    s32 commandPos;
    u32 fileSize;
    u8 isUtf16;
    u8 isUtf8;
    u8 unknown54A[2];
};

struct nufpar_s* NuAllocFPar(void);
struct nufpar_s* NuFParCreate(char* filename);
s32 NuFParGetWord(struct nufpar_s* parser);
s32 NuFParGetInt(struct nufpar_s* parser);
f32 NuFParGetFloat(struct nufpar_s* parser);
s32 NuFParPushCom(struct nufpar_s* parser, struct nufpcom_s* commands);
void NuFParPopCom(struct nufpar_s* parser);

#endif
