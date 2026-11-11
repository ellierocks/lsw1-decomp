#include "nucore/nufile.h"

extern s32 lbl_8040ACE8;

struct nufileentry_s *fn_8000556C(s32 arg0, s32 arg1)
{
    s32 i;
    if (arg0 == -3) return lbl_80407A68;
    if (arg0 == -2 || arg0 == -1) arg0 = 2;
    for (i = 0; i < lbl_8040ACE8; i++) {
        if (lbl_8030FE80[i].id == arg0 &&
            (arg1 < 0 || arg1 == lbl_8030FE80[i].device))
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
    strcpy(lbl_8030FF1C + lbl_8040ACE8 * 0x224, lbl_80407A68->currentDirectory);
    strcpy(lbl_8030FF1C + lbl_8040ACE8 * 0x224 + 0x80, lbl_80407A68->path11C);
    strcpy(lbl_8030FF1C + lbl_8040ACE8 * 0x224 + 0x100, lbl_80407A68->path19C);
    lbl_8040ACE8++;
    return (struct nufileentry_s *)(lbl_8030FF1C + lbl_8040ACE8 * 0x224 - 0x2c0);
}

void (*fn_80009970(char *, s32))(char *, ...);
s32 NuFilePos(s32);

s32 NuStrCpy(char *, char *);
void NuStrCat(char *, char *);
extern char lbl_8018CC48[];
extern char lbl_8018CC44[];
void fn_800055EC(char *path)
{
    char separator[2];
    s32 len;
    s32 first = (s8)*path;
    *(u16*)separator = *(u16*)lbl_8018CC44;
    if (first) {
        len = NuStrCpy(lbl_80407A68->currentDirectory, path);
        if (len && path[len - 1] != '\\' && path[len - 1] != '/') {
            separator[0] = lbl_80407A68->dirSeparator;
            NuStrCat(lbl_80407A68->currentDirectory, separator);
        }
    } else {
        *(lbl_80407A68->currentDirectory) = first;
    }
    fn_80009970(lbl_8018CB9C, 0x50c)(lbl_8018CC48);
    fn_80009970(lbl_8018CB9C, 0x50d)(lbl_80407A68->currentDirectory);
}

extern s32 lbl_80407A40;
extern char str_NuFileBeginBlkRead_BlockHeaderMismatch[];
s32 NuFileBeginBlkRead(s32 file, s32 tag)
{
    s32 index = lbl_80407A40++;
    s32 pos = NuFilePos(file);
    if (index > 1 && pos >= lbl_801FD080[index - 1].pos + lbl_801FD080[index - 1].hdr.size) {
        lbl_80407A40--;
        return 0;
    }
    lbl_801FD080[index].pos = NuFilePos(file);
    NuFileRead(file, &lbl_801FD080[index], 8);
    if (lbl_801FD080[index].hdr.size < 0) lbl_801FD080[index].hdr.size = -lbl_801FD080[index].hdr.size;
    if (tag && tag != lbl_801FD080[index].hdr.blk)
        fn_80009948(lbl_8018CB9C, 0xa3c)(str_NuFileBeginBlkRead_BlockHeaderMismatch);
    return lbl_801FD080[index].hdr.blk;
}
char *NuStrStr(char *, char *);
s32 NuStrICmp(char *, char *);
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
s32 NuDatFileSeek(struct nudathdr_s *header, char *name)
{
    char path[256];
    char *slash;
    s32 index;
    if (*name == '@') name += 4;
    NuStrCpy(path, name);
    normalize(path, '\\');
    name = path;
    index = header->filetree[0].childix;
    slash = NuStrStr(name, lbl_8018CC44);
    if (slash) *slash = 0;
    do {
        if (NuStrICmp(header->filetree[index].txt, name) == 0) {
            if (!slash) {
                s32 child = header->filetree[index].childix;
                if (child <= 0) return -child;
                return -1;
            }
            name = slash + 1;
            index = header->filetree[index].childix;
            slash = NuStrStr(name, lbl_8018CC44);
            if (slash) *slash = 0;
        } else index = header->filetree[index].sibix;
    } while (index);
    return -1;
}

extern s32 lbl_80407A5C;
extern char str_NuFileClose_CloseFailedRetrying[];
s32 NuFileSeek(s32, s32, s32);
void fn_80005D34(s32);
s32 fn_80009C00(s32);
void fn_8003A22C(s32, s32);
void *memset(void *, int, unsigned int, ...);
static inline void close_file(s32 file)
{
    struct nufileslot_s *info;
    if (file >= 0x1000) fn_8003A22C(file - 0x1000, 0);
    else if (file >= 0x400) fn_80005D34(file);
    else {
        file--;
        while (fn_80009C00(file) < 0) {
            fn_80009970(lbl_8018CB9C, 0x626)(str_NuFileClose_CloseFailedRetrying);
            fn_80009948(lbl_8018CB9C, 0x627)(str_NuFileClose_CloseFailedRetrying);
        }
        info = &lbl_80200080[file];
        if (info->buffer) info->buffer->owner = 0;
        memset(info, 0, 0x28);
    }
}
s32 NuFileClose(char *name)
{
    s32 saved = lbl_80407A5C;
    s32 result;
    s32 file;
    lbl_80407A5C = 0;
    if (lbl_80407A4C) {
        s32 index = NuDatFileSeek((struct nudathdr_s *)lbl_80407A4C, (char *)name);
        if (index) return lbl_80407A4C->finfo[index].uplen;
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
    struct nufileslot_s *info;
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
    if (info->buffered) return info->position;
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
    struct nufileslot_s *info;
    if (file >= 0x1000) return fn_8003A234(file - 0x1000, offset, whence, 0);
    if (file >= 0x400) return memseek(file, offset, whence);
    file--;
    info = &lbl_80200080[file];
    if (info->buffered) {
        switch (whence) {
        case 0:
        default: info->position = offset; break;
        case 1: info->position += offset; break;
        case 2: info->position = info->size - offset; break;
        }
        return info->position;
    }
    return fn_8000A64C(file, offset, whence);
}


void NuTexCreate(void);
s32 fn_8000A788(void);
extern s32 lbl_8040A2C0;
s32 NuDatFileOpenSize(s32 arg0)
{
    NuTexCreate();
    memset(&lbl_803120C0, 0, 0x190);
    memset(&lbl_80312250, 0, 0x280);
    lbl_8040A2C0 = fn_8000A788();
    return arg0;
}

extern s32 lbl_80407A54;
extern s32 lbl_8040A2C0;
void fn_8000A834(s32);
void fn_8000A86C(s32);
extern s32 lbl_80312248[];
extern char str_NuDatFileOpen_FileNotFound[];
extern char str_NuDatFileOpen_NoFreeHandles[];
extern char str_NuDatFileOpen_Opened[];
extern char str_NuDatFileOpen_SeekErrorRetrying[];

s32 NuDatFileOpen(struct nudathdr_s* ndh, char* path, s32 mode)
{
    s32 handle;
    s32 slot;
    s32 seek;
    s32 i;
    s32 index;
    if (!mode && !ndh->dfhandle) {
        handle = -1;
        fn_8000A834(lbl_8040A2C0);
        for (i = 0; i < 20; i++) {
            if (!lbl_80312250[i].used) {
                lbl_80312250[i].used = 1;
                handle = i;
                break;
            }
        }
        fn_8000A86C(lbl_8040A2C0);
        if (handle != -1) {
            index = NuDatFileSeek(ndh, path);
            if (index >= 0) {
                slot = handle;
                lbl_80312250[handle].ndh = ndh;
                lbl_80312250[handle].position = lbl_80312250[handle].start = ndh->finfo[index].foffset;
                lbl_80312250[handle].len = ndh->finfo[index].flen;
                if (!lbl_80312250[handle].len) {
                    lbl_80312250[handle].used = 0;
                    return 0;
                }
                lbl_80312250[handle].unpackedSize = ndh->finfo[index].uplen;
                lbl_80312250[handle].packed = ndh->finfo[index].ppack;
                handle += 0x800;
                do {
                    seek = NuFileSeek(ndh->fh, ndh->finfo[index].foffset, 0);
                    if (seek < 0) fn_80009970(lbl_8018CB9C, 0xcfb)(str_NuDatFileOpen_SeekErrorRetrying);
                } while (seek < 0);
                ndh->dfhandle = handle;
                ndh->filePosition = lbl_80312250[slot].start;
                fn_80009970(lbl_8018CB9C, 0xd00)(str_NuDatFileOpen_Opened, path);
                return handle;
            }
            lbl_80312250[handle].used = 0;
            fn_80009970(lbl_8018CB9C, 0xd06)(str_NuDatFileOpen_FileNotFound, path);
        } else {
            lbl_80312248[0] = 0;
            fn_80009970(lbl_8018CB9C, 0xd0b)(str_NuDatFileOpen_NoFreeHandles);
        }
    }
    return 0;
}

s32 NuPPUnpack(void*, void*);
extern char str_NuDatFileLoadBuffer_LsnReadingFile[];
extern char str_NuDatFileLoadBuffer_LsnReadingPacked[];
extern char str_NuDatFileLoadBuffer_NotAligned[];

s32 NuDatFileLoadBufferLsn(struct nudathdr_s* ndh, char* name, void* buffer, s32 size)
{
    struct nudatinfo_s* info;
    s32 index;
    s32 packed;
    s32 length;
    s32 readSize;
    s32 offset;
    void* src;
    if (lbl_80407A54 && ndh->start_lsn && size > 0x7ff && !ndh->dfhandle) {
        index = NuDatFileSeek(ndh, name);
        if (index >= 0) {
            info = index + ndh->finfo;
            if (info->ppack) {
                packed = info->flen;
                length = info->uplen;
                offset = packed - 256;
                offset = length - offset;
                readSize = (packed + 0x7ff) & ~0x7ff;
                offset += (64 - ((s32)buffer + offset & 63)) & 63;
                src = (char*)buffer + offset;
                if (readSize + offset > size) goto overflow;
                fn_80009970(lbl_8018CB9C, 0xbda)(str_NuDatFileLoadBuffer_LsnReadingPacked, name, packed, length);
                NuPPUnpack(src, buffer);
                return length;
            } else {
                length = info->flen;
                readSize = (length + 0x7ff) & ~0x7ff;
                if (readSize > size) goto overflow;
                fn_80009970(lbl_8018CB9C, 0xbec)(str_NuDatFileLoadBuffer_LsnReadingFile, name, length);
                if ((s32)buffer & 63) {
                    fn_80009970(lbl_8018CB9C, 0xbee)(str_NuDatFileLoadBuffer_NotAligned);
                }
            }
            return length;
        }
    }
    return 0;
overflow:
    return -1;
}

s32 NuQuatToMtx(s32, void*, s32);
s32 fn_8003A21C(s32, void*, s32, s32);
s32 fn_80009E94(s32, void*, s32);
void* memcpy();
extern s32 lbl_80407A58;

static inline s32 read_memory(s32 handle, void* data, s32 size)
{
    s32 n = size;
    s32 avail;
    if (handle >= 0x800) return NuQuatToMtx(handle, data, size);
    handle -= 0x400;
    avail = lbl_803120C0[handle].end - lbl_803120C0[handle].curr + 1;
    if (size > avail) n = avail;
    if (n) {
        memcpy(data, lbl_803120C0[handle].curr, n);
        lbl_803120C0[handle].curr += n;
    }
    return n;
}

static inline void allocate_buffer(struct nufileslot_s* file)
{
    s32 i;
    s32 age = lbl_80407A58;
    s32 index = 0;
    struct nufilebuffer_s* buffer;
    for (i = 0; i < 4; i++) {
        if (lbl_802005A8[i].age < age) {
            age = lbl_802005A8[i].age;
            index = i;
        }
    }
    buffer = &lbl_802005A8[index];
    if (buffer->owner) buffer->owner->buffer = 0;
    buffer->owner = file;
    buffer->age = lbl_80407A58++;
    file->buffer = buffer;
    if (file->bufferSize) {
        fn_8000A64C(file->handle, -file->bufferSize, 1);
        fn_80009E94(file->handle, buffer->data, file->bufferSize);
    }
}

s32 NuFileRead(s32 handle, void* data, s32 size)
{
    struct nufileslot_s* file;
    s32 read;
    s32 offset;
    s32 avail;
    s32 n;
    char* dest;
    if (handle >= 0x1000) return fn_8003A21C(handle - 0x1000, data, size, 0);
    if (handle >= 0x400) return read_memory(handle, data, size);
    handle--;
    file = &lbl_80200080[handle];
    if (file->buffered) {
        if (!file->buffer) allocate_buffer(file);
        dest = data;
        read = 0;
        if (size > 0) do {
            if (file->position >= file->size) break;
            if (file->position < file->bufferStart || file->position >= file->bufferStart + file->bufferSize) {
                if (file->position != file->devicePosition) {
                    fn_8000A64C(file->handle, file->position, 0);
                    file->devicePosition = file->position;
                }
                file->bufferSize = fn_80009E94(file->handle, file->buffer->data, 0x400);
                file->bufferStart = file->devicePosition;
                file->devicePosition += file->bufferSize;
            }
            n = size;
            offset = file->position - file->bufferStart;
            avail = file->bufferSize - offset;
            if (size > avail) n = avail;
            if (n) memcpy(dest, (void*)(offset + (u32)file->buffer + 8), n);
            dest += n;
            read += n;
            size -= n;
            file->position += n;
        } while (size > 0);
        return read;
    }
    return fn_80009E94(handle, data, size);
}

s32 NuRndrTri3d(char*, s32);
s32 fn_800076E4(char*, char*, s32);
s32 fn_8000584C(s32);
s32 fn_8003A214(s32, s32, char*, s32, s32);
s32 fn_80044AE0(s32);
char* strcat(char*, char*);
s32 strlen(char*);
extern char lbl_8018CC68[];
extern char lbl_8018CCAC[];
extern char str_NuFile_OpenFailed[];
extern char str_NuFileOpen_SizeSeekFailedRetrying[];
extern char str_NuFileOpen_SizeSeekRestoreFailedRetrying[];

static inline struct nufileentry_s* device_from_path(char* path)
{
    s32 i;
    s32 j;
    struct nufileentry_s* device = 0;
    for (i = 0; i < 8; i++) if (path[i] == ':') break;
    if (i < 8) {
        for (j = 0; j < lbl_8040ACE8; j++) {
            if (!fn_800076E4(path, lbl_8030FE80[j].prefix, lbl_8030FE80[j].prefixLength)) {
                device = &lbl_8030FE80[j];
                break;
            }
        }
    }
    return device;
}

s32 NuFileOpen(char* path, enum nufilemode_e mode)
{
    char name[256];
    s32 length;
    s32 index;
    s32 handle;
    s32 unknown;
    struct nufileentry_s* device = device_from_path(path);
    if (!device) {
        if (lbl_80407A4C && mode != 1 && mode != 2) {
            s32 archiveHandle = NuDatFileOpen(lbl_80407A4C, path, mode);
            if (archiveHandle) return archiveHandle;
            return 0;
        }
        device = lbl_80407A68;
    }
    device->formatName(device, name, path, 256);
    if (device->id == 2) strcat(name, lbl_8018CC68);
    if (device->id == 1) {
        index = fn_8003A214((s8)name[2] - '0', (s8)name[3] - '0', name + device->nameLength, mode, 0);
        if (index < 0) return 0;
        index += 0x1000;
        return index;
    }
    index = NuRndrTri3d(name, mode);
    if (index < 0) {
        fn_80009970(lbl_8018CB9C, 0x5c3)(str_NuFile_OpenFailed, name);
        return 0;
    }
    handle = index + 1;
    fn_80009970(lbl_8018CB9C, 0x5c7)(lbl_8018CCAC, name, handle);
    memset(&lbl_80200080[index], 0, 0x28);
    lbl_80200080[index].handle = index;
    lbl_80200080[index].mode = mode;
    if (mode != 1) {
        if (index > 16) {
            lbl_80200080[index].size = fn_80044AE0(index);
        } else {
            do {
                length = NuFileSeek(handle, 0, 2);
                if (length < 0) fn_80009970(lbl_8018CB9C, 0x5e1)(str_NuFileOpen_SizeSeekFailedRetrying);
            } while (length < 0);
            lbl_80200080[index].size = length;
            if (mode == 3) while (fn_8000584C(handle)) {}
            do {
                length = NuFileSeek(handle, 0, 0);
                if (length < 0) fn_80009970(lbl_8018CB9C, 0x5ec)(str_NuFileOpen_SizeSeekRestoreFailedRetrying);
            } while (length < 0);
            if (mode == 3) while (fn_8000584C(handle)) {}
        }
    } else lbl_80200080[index].size = 0;
    if (mode == 0) lbl_80200080[index].buffered = lbl_80407A5C;
    if (lbl_80407A54 && (device->id == -2 || device->id == -1)) {
        strlen(device->baseDirectory);
        lbl_80200080[index].unknown1C = unknown;
    } else lbl_80200080[index].unknown1C = 0;
    return handle;
}

s32 NuPPLoadBuffer(s32, void*, s32);
s32 fn_8003A23C(s32);
s32 fn_80005FF4(s32);
extern s32 lbl_80407A60;
extern s32 lbl_80407A6C;
extern char str_NuFileLoadBuffer_ReadFailureRetrying[];
extern char str_NuFileLoadBuffer_SeekFailureRetrying[];
extern char str_NuFileLoadBuffer_Trace[];

static inline s32 file_size(s32 handle)
{
    if (handle >= 0x1000) return fn_8003A23C(handle - 0x1000);
    if (handle >= 0x800) return fn_80005FF4(handle);
    if (handle >= 0x400) return lbl_803120C0[handle - 0x400].end - lbl_803120C0[handle - 0x400].start;
    return lbl_80200080[handle - 1].size;
}

s32 NuFileLoadBuffer(char* name, void* buffer, s32 size)
{
    s32 length;
    s32 handle;
    lbl_80407A6C = 0;
    length = 0;
    if (lbl_80407A4C) {
        length = NuDatFileLoadBuffer(lbl_80407A4C, name, buffer, size);
        lbl_80407A6C = 100;
    }
    if (!length) {
        lbl_80407A6C = 1;
        if (NuFileExists(name)) {
            lbl_80407A6C = 2;
            handle = NuFileOpen(name, 0);
            if (handle) {
                lbl_80407A6C = 3;
                if (lbl_80407A60) {
                    lbl_80407A6C = 4;
                    length = NuPPLoadBuffer(handle, buffer, size);
                } else {
                    lbl_80407A6C = 5;
                    length = file_size(handle);
                    if (length <= size && length) {
                        lbl_80407A6C = 6;
                        while (NuFileRead(handle, buffer, length) < 0) {
                            lbl_80407A6C = 7;
                            fn_80009970(lbl_8018CB9C, 0x7a0)(str_NuFileLoadBuffer_ReadFailureRetrying);
                            while (NuFileSeek(handle, 0, 0) < 0) {
                                fn_80009970(lbl_8018CB9C, 0x7a2)(str_NuFileLoadBuffer_SeekFailureRetrying);
                            }
                        }
                    } else {
                        length = 0;
                        lbl_80407A6C = 8;
                    }
                }
                close_file(handle);
            }
        }
    }
    if (length) fn_80009970(lbl_8018CB9C, 0x7b4)(str_NuFileLoadBuffer_Trace, name, buffer, length);
    return length;
}

void fn_8000A714(s32, s32, s32*);
extern char str_NuDatFileLoadBuffer_LsnBufferOverrun[];
extern char str_NuDatFileLoadBuffer_ReadErrorRetrying[];
extern char str_NuDatFileLoadBuffer_SeekErrorRetrying[];
extern char str_NuDatFileLoadBuffer_Unpacking[];

static inline s32 status(s32 handle)
{
    s32 result = 0;
    if (handle <= 0x3ff) {
        fn_8000A714(handle - 1, 0, &result);
        return result;
    }
    return 0;
}

s32 NuDatFileLoadBuffer(struct nudathdr_s* ndh, char* name, void* buffer, s32 size)
{
    s32 result;
    s32 handle;
    s32 index;
    s32 length;
    s32 packed;
    void* src;
    if (lbl_80407A54 && ndh->start_lsn) {
        result = NuDatFileLoadBufferLsn(ndh, name, buffer, size);
        if (result >= 0) return result;
        fn_80009970(lbl_8018CB9C, 0xc0b)(str_NuDatFileLoadBuffer_LsnBufferOverrun, name);
    }
    handle = NuDatFileOpen(ndh, name, 0);
    if (handle) {
        index = handle - 0x800;
        if (ndh->openmode == 3) while (status(ndh->fh)) {}
        if (lbl_80312250[index].packed) {
            length = lbl_80312250[index].unpackedSize;
            packed = lbl_80312250[index].len;
            src = (char*)buffer + (length + 256);
            src = (char*)src - packed;
            if (length + 256 <= size && length) {
                while (NuQuatToMtx(handle, src, packed) < 0) {
                    fn_80009970(lbl_8018CB9C, 0xc1c)(str_NuDatFileLoadBuffer_ReadErrorRetrying);
                    while (NuDatFileFindTree(handle, 0, 0) < 0) {
                        fn_80009970(lbl_8018CB9C, 0xc1f)(str_NuDatFileLoadBuffer_SeekErrorRetrying);
                    }
                }
                if (ndh->openmode == 3) while (status(ndh->fh)) {}
                close_file(handle);
                fn_80009970(lbl_8018CB9C, 0xc24)(str_NuDatFileLoadBuffer_Unpacking, name);
                NuPPUnpack(src, buffer);
                return length;
            }
        } else {
            length = lbl_80312250[index].len;
            if (length <= size && length) {
                while (NuQuatToMtx(handle, buffer, length) < 0) {
                    fn_80009970(lbl_8018CB9C, 0xc35)(str_NuDatFileLoadBuffer_ReadErrorRetrying);
                    while (NuDatFileFindTree(handle, 0, 0) < 0) {
                        fn_80009970(lbl_8018CB9C, 0xc38)(str_NuDatFileLoadBuffer_SeekErrorRetrying);
                    }
                }
                if (ndh->openmode == 3) while (status(ndh->fh)) {}
                close_file(handle);
                return length;
            }
        }
        close_file(handle);
    }
    return 0;
}

s32 NuStrLen(char*);
/* This retail call retains the allocator diagnostic file and line arguments. */
void* NuMemAlloc(s32, char*, s32);
extern char str_NuDatOpenEx_ReadErrorRetrying[];
extern char str_NuDatOpenEx_SeekErrorRetrying[];

struct nudathdr_s* NuDatOpenEx(char* path, void** buffer, s32* size, s32 mode)
{
    char name[256];
    s32 data[2];
    s32 saved = lbl_80407A5C;
    s32 handle;
    s32 nameSize;
    s32 total;
    s32 i;
    s32 unknown;
    struct nudathdr_s* ndh;
    char* ptr;
    s32* words;
    handle = NuFileOpen(path, mode);
    if (handle) {
        if (mode == 3) while (status(handle)) {}
        while (NuFileRead(handle, data, 8) < 0) {
            fn_80009970(lbl_8018CB9C, 0xd82)(str_NuDatOpenEx_ReadErrorRetrying);
            while (NuFileSeek(handle, 0, 0) < 0) {
                fn_80009970(lbl_8018CB9C, 0xd84)(str_NuDatOpenEx_SeekErrorRetrying);
            }
        }
        if (mode == 3) while (status(handle)) {}
        while (NuFileSeek(handle, data[0], 0) < 0) {
            fn_80009970(lbl_8018CB9C, 0xd8a)(str_NuDatOpenEx_SeekErrorRetrying);
        }
        if (mode == 3) while (status(handle)) {}
        lbl_80407A68->formatName(lbl_80407A68, name, path, 256);
        nameSize = (NuStrLen(name) + 16) & ~15;
        total = nameSize + (data[1] + sizeof(*ndh));
        if (buffer) {
            ndh = *buffer;
            *buffer = (char*)*buffer + total;
            if (size) *size = total;
            ndh->intalloc = 0;
        } else {
            ndh = NuMemAlloc(total, lbl_8018CB9C, 0xda4);
            ndh->intalloc = 1;
        }
        if (ndh) {
            ndh->memdatptr = (char*)(ndh + 1);
            NuStrCpy(ndh->memdatptr, name);
            ptr = (char*)(ndh + 1) + nameSize;
            while (NuFileRead(handle, ptr, data[1]) < 0) {
                fn_80009970(lbl_8018CB9C, 0xdae)(str_NuDatOpenEx_ReadErrorRetrying);
                while (NuFileSeek(handle, data[0], 0) < 0) {
                    fn_80009970(lbl_8018CB9C, 0xdb0)(str_NuDatOpenEx_SeekErrorRetrying);
                }
            }
            words = (s32*)ptr;
            if (mode == 3) while (status(handle)) {}
            ndh->finfo = (struct nudatinfo_s*)(words + 2);
            ndh->ver = words[0];
            ndh->nfiles = words[1];
            words = (s32*)(ndh->finfo + ndh->nfiles);
            ndh->treecnt = *words++;
            ndh->filetree = (struct nudfnode_s*)words;
            words = (s32*)(ndh->filetree + ndh->treecnt);
            ndh->leafnamesize = *words++;
            ndh->leafnames = (char*)words;
            for (i = 0; i < ndh->treecnt; i++) {
                ndh->filetree[i].txt = ndh->leafnames + (s32)ndh->filetree[i].txt;
            }
            ndh->filetree[0].txt = 0;
            ndh->fh = handle;
            ndh->openmode = mode;
            ndh->filePosition = 0;
            ndh->dfhandle = 0;
            if (lbl_80407A54 && lbl_80407A68->id == 2) {
                name[0] = '\\'; name[1] = 0;
                strcat(name, lbl_80407A68->currentDirectory);
                strcat(name, path);
                strcat(name, lbl_8018CC68);
                normalize(name, lbl_80407A68 ? lbl_80407A68->dirSeparator : '\\');
                ndh->start_lsn = unknown;
            } else ndh->start_lsn = 0;
            lbl_80407A5C = saved;
            return ndh;
        }
        close_file(handle);
    }
    lbl_80407A5C = saved;
    return 0;
}
