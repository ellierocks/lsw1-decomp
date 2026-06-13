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
};

struct nudatfile_s {
    struct nudathdr_s* ndh;
    s32 start;
    s32 len;
    s32 fix;
    s32 used;
};

struct numemfile_s {
    char* start;
    char* end;
    char* curr;
    s32 used;
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
void NuFileClose(s32 handle);
s32 NuFilePos(s32 handle);
s32 NuFileLoadBuffer(s32 handle, void* buffer, s32 offset, s32 size, s32 blockalign);
s32 NuFileBeginBlkRead(s32 handle, void* dest, struct nuiffhdr_s* hdr);
void NuFileEndBlockRead(s32 handle);
s32 NuDatOpenEx(struct nudathdr_s** out, char* path, char* basepath, s32 flag);
s32 NuDatFileLoadBuffer(struct nudathdr_s* ndh, s32 offset, void* buffer, s32 size);
s32 NuDatFileLoadBufferLsn(struct nudathdr_s* ndh, s32 lsn, s32 offset, void* buffer, s32 size);
s32 NuDatFileOpen(struct nudathdr_s* ndh, char* path, struct nudatfile_s* df, s32 flag);

#endif
