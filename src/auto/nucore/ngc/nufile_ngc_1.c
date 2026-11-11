#include "nucore/ngc/nufile_ngc.h"

extern void OSReport(const char* fmt, ...);
extern char lbl_8018E2B4[];

void fn_800099D4(char* fmt)
{
    OSReport(fmt);
}

void fn_800099F8(s32 arg)
{
    OSReport(lbl_8018E2B4, arg);
}

s32 OSCreateThread(s32 *, s32 *, s32, s32, s32, s32, s32);
s32 OSGetCurrentThread();
s32 OSInitSemaphore(s32 *, s32);
s32 OSResumeThread(s32 *);
s32 OSSignalSemaphore(s32 *);
s32 OSWaitSemaphore(s32 *);
s32 VIWaitForRetrace();
s32 fn_80038210();
extern s32 fn_8000A724[];
extern s32 lbl_8018E3A0[];
extern s32 lbl_80207DC0[];
extern s32 lbl_80207DCC[];
extern s32 lbl_80209DF0[];
extern s32 lbl_80407AF0;
extern s32 lbl_80407B04;
extern s32 lbl_80407B08;
extern void *lbl_80407B0C;
extern s32 lbl_80407F70;
extern s32 lbl_8040A2DC;

s32 fn_80009C00(s32 arg0)
{
  s32 temp_r9;
  s32 *new_var;
  struct nudvdfile_s *temp_r30;
  temp_r30 = &lbl_80207AC0[arg0];
  if (((s32) lbl_80407B08) == 0)
  {
    new_var = lbl_80207DCC;
    temp_r9 = ((s32) (((s8 *) new_var) + 0x1F)) & 0xFFFFFFE0;
    lbl_8040A2DC = temp_r9;
    OSCreateThread(lbl_80209DF0, fn_8000A724, 0, temp_r9 + 0x1FFF, 0x2000, 0x14, 0);
    lbl_80407B08 = 1;
  }
  if (((s32) lbl_80407B04) == 0)
  {
    OSInitSemaphore(lbl_80207DC0, 1);
    lbl_80407B04 = 1;
  }
  OSWaitSemaphore(lbl_80207DC0);
  if (OSGetCurrentThread() == ((s32) lbl_80407F70))
  {
    lbl_80407B0C = temp_r30;
    OSResumeThread(lbl_80209DF0);
    if (((void *) lbl_80407B0C) != ((void *) 0))
    {
      do
      {
        OSReport(lbl_8018E3A0);
        fn_80038210();
        VIWaitForRetrace();
      }
      while (((void *) lbl_80407B0C) != ((void *) 0));
    }
  }
  else
  {
    DVDClose(&temp_r30->file);
  }
  temp_r30->available = 1;
  if (((s32) lbl_80407AF0) == arg0)
  {
    lbl_80407AF0 = -1;
  }
  OSSignalSemaphore(lbl_80207DC0);
  return 0;
}

s32 DVDGetDriveStatus(void);
void PADControlMotor(s32, u32);
void fn_800356FC(void);
void fn_80036858(void);
void fn_80036D50(void);
void fn_80037188(void);
void fn_800372D4(s32);
extern s32 lbl_80407B00;
extern s32 lbl_80407F98;

void fn_80009D40(struct nudvdfile_s* file)
{
    s32 state = 0;
    s32 zero = 0;
    s32 thread;
    s32 status;
    for (;;) {
        thread = OSGetCurrentThread();
        status = DVDGetDriveStatus();
        lbl_80407B00 = status;
        if ((u32)(status - 4) <= 2 || status == 11) {
            if (state == 0) {
                PADControlMotor(0, 2);
                PADControlMotor(1, 2);
                PADControlMotor(2, 2);
                PADControlMotor(3, 2);
                if (thread == lbl_80407F70) {
                    lbl_80407F98 = 1;
                    state = 2;
                    VIWaitForRetrace();
                    VIWaitForRetrace();
                    do {
                        fn_80037188();
                        fn_80036D50();
                        fn_800356FC();
                        fn_80036858();
                    } while (--state);
                    VIWaitForRetrace();
                    VIWaitForRetrace();
                    lbl_80407F98 = zero;
                }
                state = 1;
            }
            VIWaitForRetrace();
pump:
            fn_80038210();
            continue;
        } else {
            if (state == 1) {
                if (thread == lbl_80407F70) {
                    lbl_80407F98 = state;
                    fn_800372D4(0);
                    state = 2;
                    VIWaitForRetrace();
                    VIWaitForRetrace();
                    do {
                        fn_80037188();
                        fn_80036D50();
                        fn_800356FC();
                        fn_80036858();
                    } while (--state);
                    VIWaitForRetrace();
                    VIWaitForRetrace();
                    fn_800372D4(1);
                    lbl_80407F98 = zero;
                }
                state = 0;
            }
            if (lbl_80407B00) goto pump;
            if (file->readSize < 0) goto pump;
            break;
        }
    }
}

extern char lbl_8018E304[];
extern char lbl_8018E338[];
extern char lbl_8018E360[];
extern char lbl_8018E380[];
void (*fn_80009948(char*, s32))(char*, ...);
void (*fn_8000995C(char*, s32))(char*, ...);
void* memset();
s32 DVDOpen(char*, struct nudvdinfo_s*);

static inline struct nudvdfile_s* alloc_file(void)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (lbl_80207AC0[i].available) {
            struct nudvdfile_s* file = &lbl_80207AC0[i];
            memset(&file->file, 0, sizeof(file->file));
            return file;
        }
    }
    return 0;
}

s32 NuRndrTri3d(char* path, s32 mode)
{
    char name[256];
    char* dest;
    char* src;
    struct nudvdfile_s* file;
    s32 result;
    if (!lbl_80407B04) {
        OSInitSemaphore(lbl_80207DC0, 1);
        lbl_80407B04 = 1;
    }
    OSWaitSemaphore(lbl_80207DC0);
    src = path;
    dest = name;
    while (*src) {
        *dest = *src;
        if (*src == '\\') *dest = '/';
        dest++;
        src++;
    }
    *dest = 0;
    if (mode == 1 || mode == 2) {
        fn_80009948(lbl_8018E304, 0x148)(lbl_8018E338);
        OSSignalSemaphore(lbl_80207DC0);
        result = -1;
    } else {
        file = alloc_file();
        if (!file) {
            fn_80009948(lbl_8018E304, 0x153)(lbl_8018E360);
            OSSignalSemaphore(lbl_80207DC0);
            result = -1;
        } else {
            file->available = 0;
            if (DVDOpen(name, &file->file)) {
                file->position = 0;
                OSSignalSemaphore(lbl_80207DC0);
                result = file->index;
            } else {
                fn_8000995C(lbl_8018E304, 0x160)(lbl_8018E380, name);
                file->available = 1;
                OSSignalSemaphore(lbl_80207DC0);
                result = -1;
            }
        }
    }
    return result;
}

s32 NuStrLen(char*);
s32 NuStrCpy(char*, char*);
void NuStrCat(char*, char*);
s32 fn_800076E4(char*, char*, s32);
void fn_80005BC0(struct nufileentry_s*, char*);

s32 fn_8000A268(struct nufileentry_s* device, char* dest, char* path, s32 size)
{
    char buffer[512];
    char* root = device->prefix;
    char* remainder;
    char* base;
    char* src;
    char separator;
    s32 len;
    s32 result;
    if (!fn_800076E4(path, root, NuStrLen(root))) {
        len = NuStrLen(root);
        remainder = path + len;
        if (path[len] == '\\' || *remainder == '/') {
            NuStrCpy(buffer, path);
        } else {
            NuStrCpy(buffer, device->baseDirectory);
            NuStrCat(buffer, device->currentDirectory);
            NuStrCat(buffer, remainder);
        }
    } else {
        NuStrCpy(buffer, device->baseDirectory);
        NuStrCat(buffer, device->currentDirectory);
        NuStrCat(buffer, path);
    }
    base = device->baseDirectory;
    src = buffer + NuStrLen(base);
    separator = device ? device->dirSeparator : '\\';
    while (*src) {
        if (*src == '/' || *src == '\\') *src = separator;
        src++;
    }
    fn_80005BC0(device, buffer);
    if (NuStrLen(buffer) < size) {
        NuStrCpy(dest, buffer);
        return 1;
    }
    return 0;
}

u32 ARGetBaseAddress(void);
u32 ARGetSize(void);
s32 ARQPostRequest(s32 *, s32, s32, s32, void *, void *, u32, s32 *);
void DCFlushRange(void *, u32);
s32 DVDReadAsyncPrio(struct nudvdinfo_s *, void *, u32, s32, s32 *, s32);
void *memcpy(void *, void *, ...);
extern s32 fn_8000A754[];
extern s32 fn_8000A760[];
extern s8 lbl_8018E3B4[];
extern s32 lbl_8020A108[];
extern void *lbl_80407AF4;
extern void *lbl_80407AF8;
extern u32 lbl_80407AFC;
extern s32 lbl_80407B10;
extern u32 lbl_8040A2D4;
extern u32 lbl_8040A2D8;

s32 fn_80009E94(s32 slot, void *data, s32 size) {
    s32 position;
    s32 startPosition;
    s32 prefixSize;
    s32 result;
    s32 bytesRead;
    s32 alignedPosition;
    s32 padding;
    s32 remaining;
    s32 inAram;
    s32 chunkSize;
    s32 directSize;
    struct nudvdfile_s *file;
    u32 fileSize;
    u32 aramBase;
    u32 transferSize;
    u32 available;
    u32 readSize;
    void *nextDestination;
    void *destination;
    remaining = size;
    destination = data;
    file = &lbl_80207AC0[slot];
    inAram = 0;
    if ((s32) lbl_80407B04 == 0) {
        OSInitSemaphore(lbl_80207DC0, 1);
        lbl_80407B04 = 1;
    }
    if ((u32)destination > ARGetBaseAddress()) {
        aramBase = ARGetBaseAddress();
        if ((u32)destination < (u32) (aramBase + ARGetSize())) {
            inAram = 1;
        }
    }
    fileSize = file->file.length;
    startPosition = file->position;
    if ((u32) remaining > fileSize) {
        remaining = (s32) fileSize;
    }
    if ((inAram == 0) && !(startPosition & 0x1F) && !((s32) data & 0x1F) && !(remaining & 0x1F)) {
        available = fileSize - startPosition;
        if (!(available & 0x1F)) {
            if ((u32) startPosition >= fileSize) {
                return 0;
            }
            directSize = remaining;
            if ((u32) remaining > available) {
                directSize = (s32) available;
            }
            file->readSize = -1;
            if (DVDReadAsyncPrio(&file->file, data, (u32) directSize, startPosition, fn_8000A760, 2) == 0) {
                fn_8000995C(lbl_8018E304, 0x2E4)(lbl_8018E3B4, remaining);
                return 0;
            }
            fn_80009D40(file);
            result = file->readSize;
            file->position += result;
            return result;
        }
    }
    OSWaitSemaphore(lbl_80207DC0);
loop_21:
    position = file->position;
    if ((u32) position < fileSize) {
        if ((slot == (s32) lbl_80407AF0) && ((u32) position >= (u32) lbl_8040A2D4) && ((u32) position < (u32) lbl_8040A2D8) && (inAram == 0)) {
            available = lbl_8040A2D8 - position;
            chunkSize = remaining;
            if ((u32) remaining > available) {
                chunkSize = (s32) available;
            }
            remaining -= chunkSize;
            memcpy(destination, lbl_80407AF4 + (position - lbl_8040A2D4), (u32) chunkSize);
            DCFlushRange(destination, (u32) chunkSize);
            destination += chunkSize;
            goto block_42;
        }
        alignedPosition = position & 0xFFFFFFFC;
        prefixSize = position - alignedPosition;
        readSize = (fileSize - position) - prefixSize;
        if (readSize >= (u32) lbl_80407AFC) {
            readSize = lbl_80407AFC;
        }
        file->readSize = -1;
        do {
            if (DVDReadAsyncPrio(&file->file, lbl_80407AF4, (readSize + 0x1F) & 0xFFFFFFE0, alignedPosition, fn_8000A760, 2) != 0) {
            fn_80009D40(file);
            bytesRead = file->readSize;
            lbl_80407AF0 = slot;
            lbl_8040A2D4 = (u32) alignedPosition;
            chunkSize = bytesRead - prefixSize;
            lbl_8040A2D8 = alignedPosition + bytesRead;
            if (chunkSize > remaining) {
                chunkSize = remaining;
            }
            if (inAram == 0) {
                nextDestination = destination + chunkSize;
                remaining -= chunkSize;
                memcpy(destination, lbl_80407AF4 + prefixSize, (u32) chunkSize);
                DCFlushRange(destination, (u32) chunkSize);
            } else {
                remaining -= chunkSize;
                nextDestination = destination + chunkSize;
                do {

                } while ((s32) lbl_80407B10 != 0);
                transferSize = (chunkSize + 0x1F) & 0xFFFFFFE0;
                memcpy(lbl_80407AF8, lbl_80407AF4 + prefixSize, (u32) chunkSize);
                padding = transferSize - chunkSize;
                if (padding > 0) {
                    memset(lbl_80407AF4 + chunkSize, 0, padding);
                }
                DCFlushRange(lbl_80407AF8, transferSize);
                lbl_80407B10 = 1;
                ARQPostRequest(lbl_8020A108, 0, 0, 1, lbl_80407AF8, destination, transferSize, fn_8000A754);
            }
            destination = nextDestination;
block_42:
            file->position += chunkSize;
            if (remaining <= 0) {
                goto block_43;
            }
            goto loop_21;
            }
        } while (0);
        fn_8000995C(lbl_8018E304, 0x341)(lbl_8018E3B4, remaining);
        OSSignalSemaphore(lbl_80207DC0);
        return 0;
    }
block_43:
    if (inAram != 0) {
        do {

        } while ((s32) lbl_80407B10 != 0);
    }
    OSSignalSemaphore(lbl_80207DC0);
    return file->position - startPosition;
}
