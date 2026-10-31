#include "types.h"

struct nudir_s {
    s32 pad[5];
};

extern struct nudir_s lbl_80207D20[];
extern void DVDCloseDir(struct nudir_s* dir);

void fn_8000A500(s32 id)
{
    DVDCloseDir(&lbl_80207D20[id]);
}

extern void (*fn_80009948(char* file, s32 line))(char* fmt, ...);
extern char lbl_8018E304[];
extern char lbl_8018E3E0[];

s32 fn_8000A604(void)
{
    fn_80009948(lbl_8018E304, 0x38b)(lbl_8018E3E0);
    return -1;
}
