#include "types.h"
#include "nucore/nufile.h"

// Local view of the GC file device; only the separator at 0x10 is confirmed.
struct nufiledeviceview_s {
    u8 unknown00[0x10];
    char dirSeparator;
};

extern struct numemfile_s lbl_803120C0[];
extern struct BlockInfo lbl_801FD080[];
extern s32 lbl_80407A40;
extern char lbl_8018CB9C[];
extern char lbl_8018CBC8[];
extern char str_NuFileEndBlockRead_ReadPastEnd[];

extern void* memcpy();
extern s32 strlen(char* s);
extern s32 fn_8003A224(s32 handle, void* data, s32 size, s32 flag);
extern s32 fn_8000A604(s32 slot, void* data, s32 size);
extern s32 NuFileSeek(s32 handle, s32 offset, s32 origin);
extern void (*fn_80009948(char* file, s32 line))();
extern void (*fn_80009970(char* file, s32 line))();

inline s32 fn_800059A0(s32 handle, void* data, s32 size)
{
    if (handle >= 0x1000) {
        return fn_8003A224(handle - 0x1000, data, size, 0);
    } else if (handle >= 0x400) {
        s32 n = size;
        s32 avail = lbl_803120C0[handle - 0x400].end - lbl_803120C0[handle - 0x400].curr + 1;
        if (n > avail) {
            n = avail;
        }
        if (n != 0) {
            memcpy(lbl_803120C0[handle - 0x400].curr, data, n);
            lbl_803120C0[handle - 0x400].curr += n;
        }
        return n;
    } else {
        return fn_8000A604(handle - 1, data, size);
    }
}

s32 fn_80005A4C(s32 handle, char* str)
{
    return fn_800059A0(handle, str, strlen(str));
}

void NuFileEndBlockRead(s32 handle)
{
    s32 pos;
    s32 i = --lbl_80407A40;

    pos = NuFilePos(handle);
    if (pos != lbl_801FD080[i].pos + lbl_801FD080[i].hdr.size) {
        if (pos > lbl_801FD080[i].pos + lbl_801FD080[i].hdr.size) {
            fn_80009970(lbl_8018CB9C, 0xA59)(str_NuFileEndBlockRead_ReadPastEnd);
        }
        NuFileSeek(handle, lbl_801FD080[i].pos + lbl_801FD080[i].hdr.size, 0);
    }
}

void fn_80005BC0(struct nufiledeviceview_s* dev, char* path)
{
    char* start = path;
    char* src = path;
    char* dst = path;
    char sep;

    while (*src != 0) {
        if (*src == dev->dirSeparator && src[1] == '.' && src[2] == '.' && (sep = src[3]) == *src) {
            dst = src;
            while (dst > start) {
                if (*--dst == sep) {
                    src += 3;
                    break;
                }
                if (*dst == ':') {
                    dst++;
                    src += 4;
                    break;
                }
            }
            if (dst <= start) {
                fn_80009948(lbl_8018CB9C, 0x2F0)(lbl_8018CBC8);
            }
        }
        *dst++ = *src++;
    }
    *dst = 0;
}
