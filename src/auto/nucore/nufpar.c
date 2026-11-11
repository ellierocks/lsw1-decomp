#include "types.h"
#include "nucore/nufpar.h"
#include "nucore/nufile.h"

extern s32 lbl_8040A2C4;
s32 fn_80006830(struct nufpar_s* p);

void fn_80006010(s32 id);
s32 NuFileSeek(s32 handle, s32 pos, s32 whence);
s32 NuDatFileFindTree(s32 id, s32 off, s32 whence);

struct nudathdr_s* fn_8000605C(struct nudathdr_s* v)
{
    struct nudathdr_s* old = lbl_80407A4C;
    lbl_80407A4C = v;
    return old;
}

s32 fn_80005D7C(s32 id)
{
    return (s32)lbl_803120C0[id - 0x400].curr;
}

s32 fn_80005FF4(s32 id)
{
    s32 i = id - 0x800;
    struct nudatfile_s* p = &lbl_80312250[i];
    return p->len;
}

s32 fn_80005E5C(s32 id)
{
    s32 i = id - 0x800;
    struct nudatfile_s* p = &lbl_80312250[i];
    return p->start - p->position;
}

void fn_80006010(s32 id)
{
    s32 i = id - 0x800;
    s32 z = 0;
    lbl_80312250[i].ndh->dfhandle = z;
    lbl_80312250[i].used = z;
}

struct nudathdr_s* fn_80006038(char* a, void** b, s32* c)
{
    return NuDatOpenEx(a, b, c, 0);
}

void fn_80005D34(s32 id)
{
    if (id > 0x7ff) {
        fn_80006010(id);
    } else {
        id -= 0x400;
        lbl_803120C0[id].active = 0;
    }
}

s32 fn_80005CC4(char* a, s32 b, u32 c)
{
    s32 i;
    if (b > 0 && c <= 1) {
        for (i = 0; i < 20; i++) {
            if (lbl_803120C0[i].active == 0) {
                lbl_803120C0[i].end = a + b - 1;
                lbl_803120C0[i].start = a;
                lbl_803120C0[i].curr = a;
                lbl_803120C0[i].used = c;
                lbl_803120C0[i].active = 1;
                return i + 0x400;
            }
        }
    }
    return 0;
}

s32 fn_80006830(struct nufpar_s* p)
{
    u16* line = p->line.w;
    u16* word = p->word.w;
    s32 len = 0;
    s32 quoted = 0;
    lbl_8040A2C4 = p->linePos;
    while (line[p->linePos & 0xff] != 0) {
        u16 c = line[p->linePos];
        switch (c & 0xff) {
            case ' ':
            case ',':
            case '\t':
                if (!quoted) {
                    if (len != 0) {
                        word[len & 0xff] = 0;
                        return len;
                    }
                    break;
                }
            default:
                if (c == '"') {
                    quoted = 1 - quoted;
                } else {
                    word[len & 0xff] = c;
                    len++;
                }
                break;
        }
        p->linePos++;
    }
    word[len & 0xff] = 0;
    return len;
}

s32 NuDatFileFindTree(s32 id, s32 off, s32 whence)
{
    s32 i = id - 0x800;
    struct nudatfile_s* p = &lbl_80312250[i];
    s32 target;
    switch (whence) {
        case 0:
        default:
            target = p->position + off;
            break;
        case 1:
            target = p->start + off;
            break;
        case 2:
            target = p->position + p->len - off;
            break;
    }
    p->ndh->filePosition = NuFileSeek(p->ndh->fh, target, 0);
    p->start = p->ndh->filePosition;
    return p->start;
}

s32 fn_80005D98(s32 id, s32 off, s32 whence)
{
    if (id > 0x7ff) {
        return NuDatFileFindTree(id, off, whence);
    }
    id -= 0x400;
    switch (whence) {
        case 0:
        default:
            lbl_803120C0[id].curr = lbl_803120C0[id].start + off;
            break;
        case 1:
            lbl_803120C0[id].curr = lbl_803120C0[id].curr + off;
            break;
        case 2:
            lbl_803120C0[id].curr = lbl_803120C0[id].end - off;
            break;
    }
    return lbl_803120C0[id].curr - lbl_803120C0[id].start;
}

s32 NuFParGetWord(struct nufpar_s* p)
{
    s32 len;
    s32 quoted;
    s32 found = 0;
    s8* line;
    u8* lineBuf;
    s8* word;
    u8* wordBuf;
    if (p->isUtf16) {
        len = fn_80006830(p);
    } else {
        line = (s8*)p->line.b;
        lineBuf = p->line.b;
        word = (s8*)p->word.b;
        wordBuf = p->word.b;
        len = 0;
        quoted = 0;
        lbl_8040A2C4 = p->linePos;
        do {
            s8 c;
            if (lineBuf[p->linePos & 0x1ff] == 0) break;
            c = line[p->linePos];
            switch (c) {
                case ' ':
                case ',':
                case '\t':
                    if (!quoted) {
                        if (len != 0) {
                            word[len & 0x1ff] = 0;
                            return len;
                        }
                        break;
                    }
                default:
                    if (c == '"') {
                        quoted = 1 - quoted;
                        found = 1;
                    } else {
                        word[len & 0x1ff] = c;
                        len++;
                    }
                    break;
            }
            p->linePos++;
        } while (!found || quoted != 0 || len != 0);
        wordBuf[len & 0x1ff] = 0;
    }
    return len;
}

extern s32 lbl_8040A2C0;
s32 fn_8000A834(s32 mutex);
s32 fn_8000A86C(s32 mutex);
/* Historical symbol: this body reads an open archive entry. */
s32 NuQuatToMtx(s32 id, void* data, s32 size)
{
    s32 result;
    struct nudatfile_s* slot;
    s32 left;
    fn_8000A834(lbl_8040A2C0);
    id -= 0x800;
    slot = &lbl_80312250[id];
    if (slot->start != slot->ndh->filePosition) {
        NuFileSeek(slot->ndh->fh, slot->start, 0);
    }
    left = slot->len + slot->position - slot->start;
    if (left < 0) left = 0;
    if (left > size) left = size;
    if (left) {
        result = NuFileRead(((struct nudatfile_s*)((id * 32) + (u32)lbl_80312250))->ndh->fh, data, left);
        if (result >= 0) slot->start += result;
        fn_8000A86C(lbl_8040A2C0);
        return result;
    }
    fn_8000A86C(lbl_8040A2C0);
    return 0;
}

s32 fn_8000606C(struct nufpar_s* p);
s32 fn_800061D8(struct nufpar_s* p)
{
    s32 len = 0;
    u16* line = p->line.w;
    s32 quoted = 0;
    s32 found = 0;
    s32 c;
    p->linePos = 0;
    p->lineNum++;
    for (;;) {
        c = fn_8000606C(p);
        switch (c) {
        case '\r':
            fn_8000606C(p);
        case '\n':
            if (!len || !found) {
                found = 0;
                p->lineNum++;
                continue;
            }
        case 0:
            line[len] = 0;
            return len;
        case '"':
            quoted = 1 - quoted;
            line[len & 0xff] = c;
            found = 1;
            len++;
            break;
        case ';':
            if (!quoted) {
                if (!len || !found) {
                    s32 done = 0;
                    do {
                        switch (fn_8000606C(p)) {
                        case '\r': fn_8000606C(p);
                        case '\n': case 0: done = 1; break;
                        }
                    } while (!done);
                    len = 0;
                    p->linePos = 0;
                    found = 0;
                    p->lineNum++;
                    continue;
                }
                for (;;) {
                    switch (fn_8000606C(p)) {
                    case '\r': fn_8000606C(p);
                    case '\n': case 0:
                        line[len] = 0;
                        return len;
                    }
                }
            }
        default:
            found = 1;
        case ' ': case '\t':
            line[len & 0xff] = c;
            len++;
            break;
        }
    }
}

static inline u8 FParReadByte(struct nufpar_s* p)
{
    s32 left = p->bufferEnd;
    s32 read;
    if (left < 0) left = 0;
    if (p->charPos > p->bufferEnd) {
        if (p->bufferEnd + 1 >= (s32)p->fileSize) return 0;
        left = p->fileSize - left;
        read = NuFileRead(p->file, p->fileBuffer, left > 0x1000 ? 0x1000 : left);
        {
            s32 end = p->bufferEnd;
            p->bufferStart = end + 1;
            p->bufferEnd = end + read;
        }
        if (!read) return 0;
    }
    {
        s32 pos = p->charPos;
        u8 c;
        c = p->fileBuffer[pos - p->bufferStart];
        p->charPos = pos + 1;
        return c;
    }
}

s32 fn_8000606C(struct nufpar_s* p)
{
    u16 a = FParReadByte(p);
    u16 b = FParReadByte(p);
    s32 result;
    if (p->isUtf16 == 2) result = (a << 8) | b;
    else result = a | (b << 8);
    return result;
}

s32 fn_80006398(struct nufpar_s* p)
{
    s32 len;
    u8* line;
    s32 quoted = 0;
    s32 found;
    s8 c;
    if (p->isUtf16) return fn_800061D8(p);
    len = 0;
    found = 0;
    line = p->line.b;
    p->linePos = 0;
    {
        s32 nextLine = p->lineNum + 1;
next_line:
        p->lineNum = nextLine;
        for (;;) {
            c = (s8)FParReadByte(p);
            switch (c) {
            case '\r':
                (s8)FParReadByte(p);
            case '\n':
                if (!len || !found) {
                    found = 0;
                    nextLine = p->lineNum + 1;
                    goto next_line;
                }
            case 0:
                line[len] = 0;
                return len;
            case '"':
                quoted = 1 - quoted;
                line[len & 0x1ff] = c;
                found = 1;
                len++;
                break;
            case ';':
                if (!quoted) {
                    if (!len || !found) {
                        s32 done = 0;
                        do {
                            c = (s8)FParReadByte(p);
                            switch (c) {
                            case '\r': (s8)FParReadByte(p);
                            case '\n': case 0: done = 1; break;
                            }
                        } while (!done);
                        len = 0;
                        p->linePos = 0;
                        found = 0;
                        nextLine = p->lineNum + 1;
                        goto next_line;
                    }
                    for (;;) {
                        c = (s8)FParReadByte(p);
                        switch (c) {
                        case '\r': (s8)FParReadByte(p);
                        case '\n': case 0:
                            line[len] = 0;
                            return len;
                        }
                    }
                }
            default:
                found = 1;
            case ' ': case '\t':
                line[len & 0x1ff] = c;
                len++;
                break;
            }
        }
    }
}

s32 NuStrCpy(char* dst, char* src);
void NuUnicodeToAscii(char* dst, u16* src);
s32 strcasecmp(char* a, char* b);
extern void (*lbl_80407A90)(struct nufpar_s*);

/* Historical symbol: dispatch the current parser word through its command table. */
s32 NuWaterRender(struct nufpar_s* p)
{
    char word[64];
    s32 i, j;
    s32 pos;
    struct nufpcom_s** tables;
    if (p->isUtf16) NuUnicodeToAscii(word, p->word.w);
    else NuStrCpy(word, (char*)p->word.b);
    if (word[0]) {
        if (word[0] != ';') goto commands;
        goto done;
success:
        ((struct nufpcom_s*)(j + (u32)tables[p->commandPos]))->function(p);
        return 1;
commands:
        if ((pos = p->commandPos) >= 0) {
            tables = p->commandStack;
            if (tables[pos][0].name) {
                i = 0; j = 0;
                do {
                    if (!strcasecmp(((struct nufpcom_s*)(j + (u32)tables[p->commandPos]))->name, word)) goto success;
                    i += 8; j += 8;
                } while (((struct nufpcom_s*)(i + (u32)tables[p->commandPos]))->name);
            }
        }
        if (lbl_80407A90) lbl_80407A90(p);
    }
done:
    return 0;
}
