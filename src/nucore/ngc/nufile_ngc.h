#ifndef NUFILE_NGC_H
#define NUFILE_NGC_H

#include "nucore/nufile.h"
struct nudvdinfo_s {
    u8 commandBlock[0x30];
    u32 startAddress;
    u32 length;
    void (*callback)(s32, struct nudvdinfo_s*);
};

struct nudvddir_s {
    u32 entry;
    u32 location;
    u32 next;
};

s32 DVDClose(struct nudvdinfo_s* file);
s32 DVDOpenDir(char* name, struct nudvddir_s* dir);
s32 DVDCloseDir(struct nudvddir_s* dir);

struct nudvdfile_s {
    struct nudvdinfo_s file;
    s32 position;
    s32 index;
    s32 available;
    s32 readSize;
};

struct nudir_s {
    struct nudvddir_s dir;
    s32 index;
    s32 available;
};

extern struct nudvdfile_s lbl_80207AC0[];
extern struct nudir_s lbl_80207D20[];

s32 NuRndrTri3d(char* name, s32 mode);
s32 fn_80009C00(s32 slot);
s32 fn_80009E94(s32 slot, void* data, s32 size);
s32 fn_8000A64C(s32 slot, s32 offset, s32 origin);
void fn_8000A714(s32 slot, s32 command, s32* result);
void NuTexCreate(s32 device);
struct nufileentry_s* fn_800034A0(struct nufileentry_s* device);
struct nufileentry_s* fn_8000556C(s32 id, s32 device);

#endif
