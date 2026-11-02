#include "types.h"

struct nufileentry_s {
    s32 unk0;
    s32 unk4;
    char unk8[0x21c];
};
extern struct nufileentry_s lbl_8030FE80[];
extern struct nufileentry_s *lbl_80407A68;
extern s32 lbl_8040ACE8;

struct nufileentry_s *fn_8000556C(s32 arg0, s32 arg1)
{
    s32 i;
    if (arg0 == -3) return lbl_80407A68;
    if (arg0 == -2 || arg0 == -1) arg0 = 2;
    for (i = 0; i < lbl_8040ACE8; i++) {
        if (lbl_8030FE80[i].unk0 == arg0 &&
            (arg1 < 0 || arg1 == lbl_8030FE80[i].unk4))
            return &lbl_8030FE80[i];
    }
    return 0;
}

void (*fn_80009948(char *, s32))(char *, ...);
extern char lbl_8018CB9C[];
extern char lbl_8018CBE0[];
extern char lbl_8030FF1C[];
void strcpy(char *, char *);

struct nufileentry_s *fn_800034A0(struct nufileentry_s *arg0)
{
    if (lbl_8040ACE8 > 15)
        fn_80009948(lbl_8018CB9C, 0x329)(lbl_8018CBE0);
    lbl_8030FE80[lbl_8040ACE8] = *arg0;
    strcpy(lbl_8030FF1C + lbl_8040ACE8 * 0x224, (char *)lbl_80407A68 + 0x9c);
    strcpy(lbl_8030FF1C + lbl_8040ACE8 * 0x224 + 0x80, (char *)lbl_80407A68 + 0x11c);
    strcpy(lbl_8030FF1C + lbl_8040ACE8 * 0x224 + 0x100, (char *)lbl_80407A68 + 0x19c);
    lbl_8040ACE8++;
    return (struct nufileentry_s *)(lbl_8030FF1C + lbl_8040ACE8 * 0x224 - 0x2c0);
}

void (*fn_80009970(char *, s32))(char *, ...);
s32 NuFilePos(s32);

s32 NuStrCpy(char *, char *);
void NuStrCat(char *, char *);
extern char lbl_8018CC48[];
void fn_800055EC(char *path)
{
    char separator[2] = "\\";
    s32 len;
    s32 first = (s8)*path;
    if (first) {
        len = NuStrCpy((char *)lbl_80407A68 + 0x9c, path);
        if (len && path[len - 1] != '\\' && path[len - 1] != '/') {
            separator[0] = *((char *)lbl_80407A68 + 0x10);
            NuStrCat((char *)lbl_80407A68 + 0x9c, separator);
        }
    } else {
        *((char *)lbl_80407A68 + 0x9c) = first;
    }
    fn_80009970(lbl_8018CB9C, 0x50c)(lbl_8018CC48);
    fn_80009970(lbl_8018CB9C, 0x50d)((char *)lbl_80407A68 + 0x9c);
}

struct nublkview_s { s32 tag, size, pos; };
extern struct nublkview_s lbl_801FD080[];
extern s32 lbl_80407A40;
extern char str_NuFileBeginBlkRead_BlockHeaderMismatch[];
void NuFileRead(s32, void *, s32);
s32 NuFileBeginBlkRead(s32 file, s32 tag)
{
    s32 index = lbl_80407A40++;
    s32 pos = NuFilePos(file);
    if (index > 1 && pos >= lbl_801FD080[index - 1].pos + lbl_801FD080[index - 1].size) {
        lbl_80407A40--;
        return 0;
    }
    lbl_801FD080[index].pos = NuFilePos(file);
    NuFileRead(file, &lbl_801FD080[index], 8);
    if (lbl_801FD080[index].size < 0) lbl_801FD080[index].size = -lbl_801FD080[index].size;
    if (tag && tag != lbl_801FD080[index].tag)
        fn_80009948(lbl_8018CB9C, 0xa3c)(str_NuFileBeginBlkRead_BlockHeaderMismatch);
    return lbl_801FD080[index].tag;
}
struct nudfnodeview_s { short child, sibling; char *text; };
struct nudatheadview_s { char unk0[16]; struct nudfnodeview_s *tree; };
char *NuStrStr(char *, char *);
s32 NuStrICmp(char *, char *);
extern char lbl_8018CC44[];
static inline void normalize(char *p, char separator)
{

    if (*p) do {
        switch ((s8)*p) {
        case 'a' ... 'z': *p -= 32; break;
        case '/': case '\\': *p = separator; break;
        }
        p++;
    } while (*p);
}
s32 NuDatFileSeek(struct nudatheadview_s *header, char *name)
{
    char path[256];
    char *slash;
    s32 index;
    if (*name == '@') name += 4;
    NuStrCpy(path, name);
    normalize(path, '\\');
    name = path;
    index = header->tree[0].child;
    slash = NuStrStr(name, lbl_8018CC44);
    if (slash) *slash = 0;
    do {
        if (NuStrICmp(header->tree[index].text, name) == 0) {
            if (!slash) {
                s32 child = header->tree[index].child;
                if (child <= 0) return -child;
                return -1;
            }
            name = slash + 1;
            index = header->tree[index].child;
            slash = NuStrStr(name, lbl_8018CC44);
            if (slash) *slash = 0;
        } else index = header->tree[index].sibling;
    } while (index);
    return -1;
}

struct nufileslotview_s { s32 unk0, unk4, unk8, unkC, unk10, unk14, unk18; char unk1C[8]; s32 *unk24; };
struct nudatinfoview_s { s32 unk0, unk4, unk8, unkC; };
struct nudatview_s { s32 unk0, unk4; struct nudatinfoview_s *unk8; };
extern struct nufileslotview_s lbl_80200080[];
extern struct nudatview_s *lbl_80407A4C;
extern s32 lbl_80407A5C;
extern char str_NuFileClose_CloseFailedRetrying[];
s32 NuFileOpen(u8 *, s32);
s32 NuFileSeek(s32, s32, s32);
void fn_80005D34(s32);
s32 fn_80009C00(s32);
void fn_8003A22C(s32, s32);
void *memset(void *, int, unsigned int, ...);
static inline void close_file(s32 file)
{
    struct nufileslotview_s *info;
    if (file >= 0x1000) fn_8003A22C(file - 0x1000, 0);
    else if (file >= 0x400) fn_80005D34(file);
    else {
        file--;
        while (fn_80009C00(file) < 0) {
            fn_80009970(lbl_8018CB9C, 0x626)(str_NuFileClose_CloseFailedRetrying);
            fn_80009948(lbl_8018CB9C, 0x627)(str_NuFileClose_CloseFailedRetrying);
        }
        info = &lbl_80200080[file];
        if (info->unk24) *info->unk24 = 0;
        memset(info, 0, 0x28);
    }
}
s32 NuFileClose(u8 *name)
{
    s32 saved = lbl_80407A5C;
    s32 result;
    s32 file;
    lbl_80407A5C = 0;
    if (lbl_80407A4C) {
        s32 index = NuDatFileSeek((struct nudatheadview_s *)lbl_80407A4C, (char *)name);
        if (index) return lbl_80407A4C->unk8[index].unk8;
    }
    result = -1;
    if (name && *name) {
        file = NuFileOpen(name, 0);
        if (file) {
            while (NuFileSeek(file, 0, 2) < 0) {}
            result = NuFilePos(file);
            close_file(file);
        }
    }
    lbl_80407A5C = saved;
    return result;
}

struct numemview_s { s32 start, end, curr, unkC, unk10; };
extern struct numemview_s lbl_803120C0[];
s32 fn_80005E5C(s32);
s32 fn_8003A234(s32, s32, s32, s32);
s32 fn_8000A64C(s32, s32, s32);
extern char str_NuFilePos_SeekFailed[];
extern char str_NuFilePos_SeekFailedRetrying[];
static inline s32 mempos(s32 file)
{
    if (file >= 0x800) return fn_80005E5C(file);
    file -= 0x400;
    return lbl_803120C0[file].curr - lbl_803120C0[file].start;
}

s32 NuFilePos(s32 file)
{
    s32 pos;
    struct nufileslotview_s *info;
    if (file >= 0x1000) {
        pos = fn_8003A234(file - 0x1000, 0, 2, 0);
        if (pos < 0) fn_80009970(lbl_8018CB9C, 0x6e7)(str_NuFilePos_SeekFailed, pos);
        return pos;
    }
    if (file >= 0x400) {
        return mempos(file);
    }
    file--;
    info = &lbl_80200080[file];
    if (info->unk18) return info->unk4;
    while ((pos = fn_8000A64C(file, 0, 1)) < 0)
        fn_80009970(lbl_8018CB9C, 0x6f4)(str_NuFilePos_SeekFailedRetrying, pos);
    return pos;
}

s32 NuDatFileFindTree(s32, s32, s32);
static inline s32 memseek(s32 file, s32 offset, s32 whence)
{
    if (file >= 0x800) return NuDatFileFindTree(file, offset, whence);
    file -= 0x400;
    switch (whence) {
    case 0:
    default: lbl_803120C0[file].curr = lbl_803120C0[file].start + offset; break;
    case 1: lbl_803120C0[file].curr += offset; break;
    case 2: lbl_803120C0[file].curr = lbl_803120C0[file].end - offset; break;
    }
    offset = lbl_803120C0[file].curr - lbl_803120C0[file].start;
    return offset;
}
s32 NuFileSeek(s32 file, s32 offset, s32 whence)
{
    struct nufileslotview_s *info;
    if (file >= 0x1000) return fn_8003A234(file - 0x1000, offset, whence, 0);
    if (file >= 0x400) return memseek(file, offset, whence);
    file--;
    info = &lbl_80200080[file];
    if (info->unk18) {
        switch (whence) {
        case 0:
        default: info->unk4 = offset; break;
        case 1: info->unk4 += offset; break;
        case 2: info->unk4 = info->unkC - offset; break;
        }
        return info->unk4;
    }
    return fn_8000A64C(file, offset, whence);
}


void NuTexCreate(void);
s32 fn_8000A788(void);
extern char lbl_80312250[];
extern s32 lbl_8040A2C0;
s32 NuDatFileOpenSize(s32 arg0)
{
    NuTexCreate();
    memset(&lbl_803120C0, 0, 0x190);
    memset(&lbl_80312250, 0, 0x280);
    lbl_8040A2C0 = fn_8000A788();
    return arg0;
}
