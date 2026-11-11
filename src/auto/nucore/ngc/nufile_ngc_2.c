#include "nucore/ngc/nufile_ngc.h"

extern void (*fn_80009948(char* file, s32 line))(char* fmt, ...);
extern char lbl_8018E304[];
extern char lbl_8018E3E0[];

extern char lbl_8018E40C[];
extern char lbl_8018E430[];
extern struct nufileentry_s lbl_801B0680;
extern struct nufileentry_s lbl_801B08A4;
extern s32 lbl_8040ACE8;
extern s32 lbl_8040ACEC;
void* memset();

static inline struct nudir_s* alloc_dir(void)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (lbl_80207D20[i].available) return &lbl_80207D20[i];
    }
    return 0;
}

s32 fn_8000A40C(char* path)
{
    struct nudir_s* dir = alloc_dir();
    s32 result;
    if (!dir) {
        fn_80009948(lbl_8018E304, 0x3cc)(lbl_8018E40C);
        return -1;
    }
    if (!DVDOpenDir(path, &dir->dir)) {
        result = dir->index;
    } else {
        fn_80009948(lbl_8018E304, 0x3d6)(lbl_8018E430, path);
        dir->available = 1;
        result = -1;
    }
    return result;
}

void fn_8000A500(s32 id)
{
    DVDCloseDir(&lbl_80207D20[id].dir);
}

void NuTexCreate(s32 device)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        lbl_80207AC0[i].index = i;
        lbl_80207AC0[i].available = 1;
    }
    for (i = 0; i < 8; i++) {
        lbl_80207D20[i].index = i;
        lbl_80207D20[i].available = 1;
    }
    memset(lbl_8030FE80, 0, 0x2240);
    lbl_8040ACE8 = 0;
    lbl_8040ACEC = 0;
    fn_800034A0(&lbl_801B0680);
    fn_800034A0(&lbl_801B08A4);
    lbl_80407A68 = fn_8000556C(device, -1);
}

s32 fn_8000A604(void)
{
    fn_80009948(lbl_8018E304, 0x38b)(lbl_8018E3E0);
    return -1;
}
