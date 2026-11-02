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

