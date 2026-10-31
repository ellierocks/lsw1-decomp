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
