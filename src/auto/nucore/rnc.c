#include "types.h"
#include "nucore/nufile.h"

/* NuPPLoadBuffer reads three 32-bit header words before byte-swapping sizes. */
struct rnc_header_s {
    u32 signature;
    u32 unpackedSize;
    u32 packedSize;
};

s32 NuFileSeek(s32 handle, s32 offset, s32 whence);
void NuPPUnpack(void* packed, s8* output);

void (*fn_80009948(char* file, s32 line))();
void (*fn_80009970(char* file, s32 line))();
extern char lbl_8018DD64[];
extern char lbl_8018DE00[];
extern char lbl_8018DE40[];
extern char lbl_8018DE88[];
extern char lbl_8018DEC0[];
extern char str_NuPPLoadBuffer1_SeekFailureRetrying[];
extern char str_NuPPLoadBuffer2_ReadFailureRetrying[];
extern char str_NuPPLoadBuffer3_SeekFailureRetrying[];
extern char str_NuPPLoadBuffer4_ReadFailureRetrying[];
extern char str_NuPPLoadBuffer5_SeekFailureRetrying[];
extern char str_NuPPLoadBuffer6_ReadFailureRetrying[];

static inline u32 RncSwap(u32 word)
{
    return (word >> 24) | ((word >> 8) & 0xff00) | ((word << 8) & 0xff0000) | (word << 24);
}

s32 NuPPLoadBuffer(s32 handle, u8* output, s32 capacity)
{
    struct rnc_header_s header;
    s32 result;
    u8* packed;
    u8* source;
    /* Retail passes an uninitialized timing register to the final diagnostic.
     * Keep this nonmatching scaffold defined while that source shape is unresolved. */
    s32 unusedTime = 0;
    do {
        while (NuFileSeek(handle, 0, 0) < 0) {
            fn_80009970(lbl_8018DD64, 0x96)(str_NuPPLoadBuffer1_SeekFailureRetrying);
        }
        result = NuFileRead(handle, &header, 12);
        if (result < 0) {
            fn_80009970(lbl_8018DD64, 0x9a)(str_NuPPLoadBuffer2_ReadFailureRetrying);
        }
    } while (result < 0);
    if (header.signature == 0x02434e52) {
        header.unpackedSize = RncSwap(header.unpackedSize);
        header.packedSize = RncSwap(header.packedSize) + 18;
        packed = (u8*)((u32)output + (header.unpackedSize + 0x100) - header.packedSize);
        source = packed - 4;
        do {
            fn_80009970(lbl_8018DD64, 0xaa)(lbl_8018DE00);
            while (NuFileSeek(handle, 4, 0) < 0) {
                fn_80009970(lbl_8018DD64, 0xac)(str_NuPPLoadBuffer3_SeekFailureRetrying);
            }
            fn_80009970(lbl_8018DD64, 0xaf)(lbl_8018DE40);
            result = NuFileRead(handle, packed, header.packedSize - 4);
            if (result < 0) {
                fn_80009970(lbl_8018DD64, 0xb2)(str_NuPPLoadBuffer4_ReadFailureRetrying, result);
            }
        } while (result < 0);
        NuPPUnpack(source, (s8*)output);
        fn_80009970(lbl_8018DD64, 0xba)(lbl_8018DE88, header.packedSize, header.unpackedSize, unusedTime);
        result = header.unpackedSize;
    } else {
        result = NuFileOpenSize(handle);
        if (result > capacity) {
            fn_80009948(lbl_8018DD64, 0xc0)(lbl_8018DEC0, capacity, result);
        }
        do {
            while (NuFileSeek(handle, 0, 0) < 0) {
                fn_80009970(lbl_8018DD64, 0xc6)(str_NuPPLoadBuffer5_SeekFailureRetrying);
            }
            if (NuFileRead(handle, output, result) >= 0) break;
            fn_80009970(lbl_8018DD64, 0xca)(str_NuPPLoadBuffer6_ReadFailureRetrying);
        } while (1);
    }
    return result;
}
