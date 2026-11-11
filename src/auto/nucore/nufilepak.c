#include "nucore/nufilepak.h"

extern s32 lbl_80407AD8;

s32 fn_80009408(struct nupakhdr_s* hdr, s32 handle, void** addr, s32* size)
{
    struct nupakitem_s* item;
    lbl_80407AD8 = 0;
    handle -= 1;
    if (handle > -1 && handle < hdr->itemCount) {
        item = GetItems(hdr);
        item = &item[handle];
        if (!item->removed) {
            if (addr != 0) {
                *addr = (void*)((u8*)hdr + item->dataOffset);
            }
            if (size != 0) {
                *size = item->size;
            }
            return 1;
        }
    }
    return 0;
}

s32 fn_800094C4(struct nupakhdr_s* hdr, s32 handle, s32 required)
{
    lbl_80407AD8 = 0;
    handle -= 1;
    if (handle >= 0 && handle < hdr->itemCount) {
        struct nupakitem_s* items = GetItems(hdr);
        items = &items[handle];
        if (!items->removed) {
            items->required = required;
        }
        return 1;
    }
    return 0;
}

extern void NuHeap(s32 a, s32 b, s32 c, s32 d);
struct nupakhdr_s* fn_80008E78(char*, char**, char**, s32, u8*, s32);

void fn_80008E54(s32 a, s32 b, s32 c)
{
    NuHeap(a, b, c, 0);
}

struct nupakhdr_s* fn_800092F0(char* name, char** buffer, char** end, s32 alignment)
{
    char* limit = *end;
    return fn_80008E78(name, buffer, &limit, alignment, 0, 0);
}

s32 strcasecmp(char*, char*);
void fn_8000759C(void*, void*, s32);

s32 fn_80009324(struct nupakhdr_s* hdr, char* name)
{
    u32 i;
    struct nupakitem_s* items;
    lbl_80407AD8 = 0;
    items = GetItems(hdr);
    for (i = 0; i < hdr->itemCount; i++) {
        if (strcasecmp((char*)hdr + items[i].nameOffset, name) == 0) {
            if (items[i].removed) return 0;
            return i + 1;
        }
    }
    return 0;
}

s32 NuLightStoreCurrentLights(struct nupakhdr_s* hdr)
{
    struct nupakitem_s* items;
    s32 removed;
    u32 i;
    s32 alignment;
    s32 aligned;
    lbl_80407AD8 = 0;
    items = GetItems(hdr);
    removed = 0;
    for (i = 0; i < hdr->itemCount; i++) {
        if (!items[i].removed) {
            if (items[i].required) {
                if (removed) {
                    if (items[i].alignment) removed &= -items[i].alignment;
                    if (removed) {
                        fn_8000759C((char*)hdr + items[i].dataOffset - removed,
                            (char*)hdr + items[i].dataOffset, items[i].size);
                        items[i].dataOffset -= removed;
                    }
                }
            } else {
                items[i].removed = 1;
                removed += items[i].size;
                alignment = items[i].alignment;
                aligned = removed - 1;
                aligned += alignment;
                removed = aligned & -alignment;
            }
        }
    }
    hdr->size -= removed;
    return removed;
}

#define _STDARG_H
#include "sn/include/va-ppc.h"
typedef __gnuc_va_list va_list;

extern char lbl_802055AC[];
extern char lbl_802059AC[];
extern char* lbl_8040A2C8;
extern s32 lbl_8040A2CC;
extern char lbl_8018E180[];
extern char lbl_8018E19C[];
extern char lbl_8018E198[];
extern char str_NuDebugMsg_Format[];
extern char lbl_801B0660[];
s32 sprintf(char*, char*, ...);
s32 lbl_8015525C(char*, char*, va_list);
void NuStrCat(char*, char*);
char* strstr(char*, char*);
void fn_800099A0(char*);
void fn_800099D4(char*);
void fn_800099F8(char*);

void fn_80009564(char* fmt, ...)
{
    va_list args;
    sprintf(lbl_802059AC, lbl_8018E180, lbl_8040A2C8, lbl_8040A2CC);
    va_start(args, fmt);
    lbl_8015525C(lbl_802055AC, fmt, args);
    NuStrCat(lbl_802059AC, lbl_802055AC);
    NuStrCat(lbl_802059AC, lbl_8018E198);
    fn_800099A0(lbl_802059AC);
    va_end(args);
}

void fn_8000965C(char* fmt, ...)
{
    va_list args;
    sprintf(lbl_802059AC, lbl_8018E19C, lbl_8040A2C8, lbl_8040A2CC);
    va_start(args, fmt);
    lbl_8015525C(lbl_802055AC, fmt, args);
    NuStrCat(lbl_802055AC, lbl_8018E198);
    fn_800099D4(lbl_802055AC);
    va_end(args);
}

void fn_80009748(char* fmt, ...)
{
    va_list args;
    if (!*lbl_801B0660 || strstr(lbl_8040A2C8, lbl_801B0660)) {
        sprintf(lbl_802059AC, str_NuDebugMsg_Format, lbl_8040A2C8, lbl_8040A2CC);
        va_start(args, fmt);
        lbl_8015525C(lbl_802055AC, fmt, args);
        NuStrCat(lbl_802055AC, lbl_8018E198);
        fn_800099F8(lbl_802055AC);
    }
    va_end(args);
}

void fn_80009858(s32 unused, char* fmt, ...)
{
    va_list args;
    sprintf(lbl_802059AC, str_NuDebugMsg_Format, lbl_8040A2C8, lbl_8040A2CC);
    va_start(args, fmt);
    lbl_8015525C(lbl_802055AC, fmt, args);
    NuStrCat(lbl_802055AC, lbl_8018E198);
    fn_800099F8(lbl_802059AC);
    fn_800099F8(lbl_802055AC);
    va_end(args);
}

void fn_80008CD8(struct nufileheap_s* heap, void* data)
{
    struct nufreeblock_s* head = (struct nufreeblock_s*)(heap->base + 16);
    struct nufreeblock_s* block;
    struct nufreeblock_s* next;
    struct nufreeblock_s* prev;
    if (data && (char*)data >= heap->base && (char*)data <= heap->base + heap->size) {
        block = (struct nufreeblock_s*)((char*)data - 12);
        block->size &= 0x7fffffff;
        heap->count--;
        heap->used -= block->size & 0x7fffffff;
        heap->used -= 16;
        if (head->next) head->next->prev = block;
        block->next = head->next;
        block->prev = head;
        head->next = block;
        next = (struct nufreeblock_s*)((char*)block + ((block->size & 0x7fffffff) + 16));
        if (!block_used(next)) {
            unlink_block(next);
            merge_blocks(block, next);
        }
        prev = *(struct nufreeblock_s**)((char*)block - 4);
        if (!block_used(prev)) {
            unlink_block(block);
            merge_blocks(prev, block);
        }
    }
}

s32 fn_80005894(char*, char**, char**);
void (*fn_80009970(char*, s32))(char*, ...);
extern char lbl_8018E034[];
extern char lbl_8018E064[];
extern char lbl_8018E094[];
extern char lbl_8018E0C4[];
extern char lbl_8018E0D4[];
extern char lbl_8018E0FC[];
extern char lbl_8018E10C[];

static inline u32 checksum(void* data, s32 size)
{
    u8* bytes = data;
    u32 sum = 0x12345678;
    while (size) {
        size--;
        sum += *bytes++;
    }
    return sum;
}

static inline void decrypt(struct nupakhdr_s* hdr, u32 size, u8* key, s32 keySize)
{
    u8* data = (u8*)(hdr + 1);
    s32 index = 0;
    size -= sizeof(*hdr);
    while (size) {
        size--;
        *data -= key[index++];
        if (index >= keySize) index = 0;
        data++;
    }
    hdr->flags &= ~1;
}

struct nupakhdr_s* fn_80008E78(char* name, char** buffer, char** end, s32 alignment, u8* key, s32 keySize)
{
    char* limit = *end;
    struct nupakhdr_s* hdr;
    s32 size;
    u32 sum;
    s32 error;
    lbl_80407AD8 = 0;
    if (alignment > 0x800) {
        fn_80009970(lbl_8018E034, 0x9c)(lbl_8018E064);
        error = 5;
        goto failed;
    }
    if (alignment < 4) alignment = 4;
    {
        u32 aligned = (u32)*buffer - 1;
        aligned += alignment;
        *buffer = (char*)(aligned & -alignment);
    }
    hdr = (struct nupakhdr_s*)*buffer;
    size = fn_80005894(name, buffer, &limit);
    error = 1;
    if (!size) goto failed;
    if (size != hdr->size) {
        fn_80009970(lbl_8018E034, 0xac)(lbl_8018E094);
        error = 5;
        goto failed;
    }
    switch (hdr->type) {
        case 0x12345679:
            sum = hdr->checksum;
            hdr->checksum = 0;
            do {
                if (sum != checksum(hdr, size)) {
                fn_80009970(lbl_8018E034, 0xba)(lbl_8018E0C4);
                error = 3;
                goto failed;
                }
            } while (0);
        case 0x12345678:
            return hdr;
        case 0x1234567a:
            sum = hdr->checksum;
            hdr->checksum = 0;
            if (sum != checksum(hdr, size)) {
                fn_80009970(lbl_8018E034, 0xc5)(lbl_8018E0C4);
                error = 3;
                goto failed;
            }
            if (hdr->flags & 1) {
                if (!key) {
                    fn_80009970(lbl_8018E034, 0xcc)(lbl_8018E0D4);
                    error = 6;
                    goto failed;
                }
                decrypt(hdr, size, key, keySize);
                sum = hdr->decodedChecksum;
                hdr->decodedChecksum = 0;
                if (sum != checksum(hdr, size)) {
                    fn_80009970(lbl_8018E034, 0xd5)(lbl_8018E0FC);
                    error = 6;
                    goto failed;
                }
            }
            return hdr;
        default:
            fn_80009970(lbl_8018E034, 0xde)(lbl_8018E10C, hdr->type);
            error = 4;
            goto failed;
    }
failed:
    lbl_80407AD8 = error;
    return 0;
}
