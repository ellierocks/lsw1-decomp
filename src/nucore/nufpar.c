#include "nucore/nufpar.h"
#include "nucore/nufile.h"

extern struct nufpar_s* lbl_80407A88;
extern char* lbl_80407A8C;
extern struct nufpar_s lbl_802015C8[NUFPAR_COUNT];
extern char lbl_802025AC[NUFPAR_COUNT * NUFPAR_BUFFER_SIZE];
extern char lbl_8018D224[];
extern char lbl_8018D320[];
extern char lbl_8018D340[];
extern char lbl_8018D360[];

/* Retail clears CR1 before memset calls, requiring the unprototyped declaration. */
void* memset();
void (*fn_80009948(char* file, s32 line))();
void fn_80005764(s32 handle);
void NuFileSeek(s32 handle, s32 offset, s32 origin);
s32 NuStrCpy(char* dest, char* source);
void NuUnicodeToAscii(char* dest, u16* source);
s32 NuHexStringToI(char* text);
s32 NuAToI(char* text);
f32 fn_800077E8(char* text);
/* The zero literal is shared with the following retail constant pool. */
extern f32 lbl_8018D394[];

struct nufpar_s* NuAllocFPar(void)
{
    s32 i;
    if (lbl_80407A88 == 0) {
        lbl_80407A88 = lbl_802015C8;
        memset(lbl_80407A88, 0, 0xFE4);
        if (lbl_80407A88 == 0) {
            (*fn_80009948(lbl_8018D224, 923))(lbl_8018D320);
        }
        lbl_80407A8C = lbl_802025AC;
        if (lbl_80407A8C == 0) {
            (*fn_80009948(lbl_8018D224, 928))(lbl_8018D340);
        }
    }
    for (i = 0; i < NUFPAR_COUNT; i++) {
        if (lbl_80407A88[i].fileBuffer == 0) {
            memset(&lbl_80407A88[i], 0, sizeof(struct nufpar_s));
            lbl_80407A88[i].fileBuffer = lbl_80407A8C + i * NUFPAR_BUFFER_SIZE;
            memset(lbl_80407A88[i].fileBuffer, 0, NUFPAR_BUFFER_SIZE);
            return &lbl_80407A88[i];
        }
    }
    (*fn_80009948(lbl_8018D224, 951))(lbl_8018D360, NUFPAR_COUNT);
    return 0;
}

struct nufpar_s* NuFParCreate(char* filename)
{
    s32 file;
    struct nufpar_s* parser;
    s32 first;

    if (NuFileExists(filename) == 0) {
        goto fail;
    }
    file = NuFileOpen(filename, NUFILE_READ);
    if (file == 0) {
        goto fail;
    }
    parser = NuAllocFPar();
    if (parser != 0) {
        parser->file = file;
        parser->bufferEnd = -1;
        parser->lineNum = -1;
        parser->commandPos = -1;
        first = NuFileReadWChar(file);
        parser->isUtf16 = 0;
        parser->isUtf8 = 0;
        if (first == 0xFFFE) {
            parser->isUtf16 = 2;
        } else if (first == 0xFEFF) {
            parser->isUtf16 = 1;
        } else if (first == 0xBBEF) {
            if ((u8)NuFileReadChar(file) == 0xBF) {
                parser->isUtf8 = 1;
            }
        } else if (first == 0xEFBB) {
            if ((u8)NuFileReadChar(file) == 0xBF) {
                parser->isUtf8 = 2;
            }
        }
        if (parser->isUtf16 == 0 && parser->isUtf8 == 0) {
            NuFileSeek(file, 0, 0);
        }
        parser->fileSize = NuFileOpenSize(file);
        if (parser != 0) {
            NuStrCpy((char*)parser + 8, filename);
            return parser;
        }
    }
    fn_80005764(file);
fail:
    return 0;
}

s32 NuFParPushCom(struct nufpar_s* parser, struct nufpcom_s* commands)
{
    if (parser->commandPos > 6) {
        return -1;
    }
    parser->commandPos++;
    parser->commandStack[parser->commandPos] = commands;
    return parser->commandPos;
}

void NuFParPopCom(struct nufpar_s* parser)
{
    if (parser->commandPos >= 0) {
        parser->commandPos--;
    }
}


s32 NuFParGetInt(struct nufpar_s* parser)
{
    char buffer[64];
    NuFParGetWord(parser);
    if (parser->isUtf16) {
        NuUnicodeToAscii(buffer, (u16*)((char*)parser + 0x30A));
    } else {
        NuStrCpy(buffer, (char*)parser + 0x30A);
    }
    if (buffer[0] != 0) {
        if (buffer[0] == '$') {
            return NuHexStringToI(buffer + 1);
        } else if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X')) {
            return NuHexStringToI(buffer + 2);
        } else {
            return NuAToI(buffer);
        }
    }
    return 0;
}

f32 NuFParGetFloat(struct nufpar_s* parser)
{
    char buffer[64];
    NuFParGetWord(parser);
    if (parser->isUtf16) {
        NuUnicodeToAscii(buffer, (u16*)((char*)parser + 0x30A));
    } else {
        NuStrCpy(buffer, (char*)parser + 0x30A);
    }
    if (buffer[0] != 0) {
        return fn_800077E8(buffer);
    }
    return lbl_8018D394[0];
}
