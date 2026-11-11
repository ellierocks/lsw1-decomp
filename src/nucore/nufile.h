#ifndef NUFILE_H
#define NUFILE_H

#include "types.h"

struct nudatinfo_s {
    s32 foffset;
    s32 flen;
    s32 uplen;
    s32 ppack : 1;
};

struct nudfnode_s {
    short childix;
    short sibix;
    char* txt;
};

struct nudathdr_s {
    s32 ver;
    s32 nfiles;
    struct nudatinfo_s* finfo;
    s32 treecnt;
    struct nudfnode_s* filetree;
    s32 leafnamesize;
    char* leafnames;
    s32 dfhandle;
    s32 fh;
    short intalloc;
    short openmode;
    s32 start_lsn;
    void* memdatptr;
    s32 filePosition;
};

struct nudatfile_s {
    struct nudathdr_s* ndh;
    s32 position;
    s32 start;
    s32 len;
    s32 unpackedSize;
    s32 fileIndex;
    s32 used;
    u32 packed : 1;
    u32 flags : 31;
};

struct nufileslot_s {
    s32 handle;
    s32 position;
    s32 devicePosition;
    s32 size;
    s32 bufferStart;
    s32 bufferSize;
    s32 buffered;
    s32 unknown1C;
    s32 mode;
    struct nufilebuffer_s* buffer;
};

struct nufilebuffer_s {
    struct nufileslot_s* owner;
    s32 age;
    u8 data[0x400];
};

struct nufileentry_s {
    s32 id;
    s32 device;
    u8 unknown08[8];
    char dirSeparator;
    u8 unknown11[0x23];
    char prefix[0x10];
    s32 prefixLength;
    s32 nameLength;
    char baseDirectory[0x50];
    char currentDirectory[0x80];
    char path11C[0x80];
    char path19C[0x80];
    s32 (*formatName)(struct nufileentry_s*, char*, char*, s32);
    s32 unknown220;
};

struct numemfile_s {
    char* start;
    char* end;
    char* curr;
    s32 used;
    s32 active;
};

struct fileinfo_s {
    s32 offset;
    s32 size;
    s32 pad;
};

struct filebuff_s {
    void* unk;
};

struct nuiffhdr_s {
    s32 blk;
    s32 size;
};

struct BlockInfo {
    struct nuiffhdr_s hdr;
    s32 pos;
};

enum nufilemode_e {
    NUFILE_READ = 0,
    NUFILE_WRITE = 1,
};

s32 NuFileOpen(char* path, enum nufilemode_e mode);
s32 NuFileExists(char* name);
s32 NuFileOpenSize(s32 handle);
s32 NuFileRead(s32 handle, void* data, s32 size);
s32 NuFileReadInt(s32 handle);
s16 NuFileReadShort(s32 handle);
s8 NuFileReadChar(s32 handle);
u16 NuFileReadWChar(s32 handle);
s32 NuFileClose(char* name);
void fn_80005764(s32 handle);
s32 fn_8000584C(s32 handle);
s32 fn_80005894(char* name, char** buffer, char** end);
s32 NuFilePos(s32 handle);
s32 NuFileLoadBuffer(char* name, void* buffer, s32 size);
s32 NuFileBeginBlkRead(s32 handle, s32 tag);
void NuFileEndBlockRead(s32 handle);
struct nudathdr_s* NuDatOpenEx(char* path, void** buffer, s32* size, s32 mode);
s32 NuDatFileLoadBuffer(struct nudathdr_s* ndh, char* name, void* buffer, s32 size);
s32 NuDatFileLoadBufferLsn(struct nudathdr_s* ndh, char* name, void* buffer, s32 size);
s32 NuDatFileOpen(struct nudathdr_s* ndh, char* path, s32 mode);
s32 NuDatFileSeek(struct nudathdr_s* ndh, char* name);

extern struct nufileentry_s lbl_8030FE80[];
extern struct nufileentry_s* lbl_80407A68;
extern struct nufileslot_s lbl_80200080[];
extern struct nufilebuffer_s lbl_802005A8[];
extern struct numemfile_s lbl_803120C0[];
extern struct nudatfile_s lbl_80312250[];
extern struct nudathdr_s* lbl_80407A4C;
extern struct BlockInfo lbl_801FD080[];

#endif
