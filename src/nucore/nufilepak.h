#ifndef NUFILEPAK_H
#define NUFILEPAK_H

#include "nucore/nufile.h"

struct nupakitem_s {
    s32 nameOffset;
    s32 dataOffset;
    s32 size;
    s32 alignment;
    u32 required : 1;
    u32 removed : 1;
    u32 flags : 30;
    s32 pad14;
    s32 pad18;
};

struct nupakhdr_s {
    u32 type;
    u32 itemCount;
    s32 size;
    u32 checksum;
    u32 flags;
    u32 decodedChecksum;
};

static inline struct nupakitem_s* GetItems(struct nupakhdr_s* hdr)
{
    switch (hdr->type) {
        case 0x12345678:
            return (struct nupakitem_s*)((u8*)hdr + 0xc);
        case 0x12345679:
        default:
            return (struct nupakitem_s*)((u8*)hdr + 0x14);
        case 0x1234567a:
            return (struct nupakitem_s*)((u8*)hdr + 0x18);
    }
}

struct nupakhdr_s* fn_80008E78(char* name, char** buffer, char** end, s32 alignment, u8* key, s32 keySize);
struct nupakhdr_s* fn_800092F0(char* name, char** buffer, char** end, s32 alignment);
s32 fn_80009324(struct nupakhdr_s* hdr, char* name);
s32 NuLightStoreCurrentLights(struct nupakhdr_s* hdr);
s32 fn_80009408(struct nupakhdr_s* hdr, s32 handle, void** addr, s32* size);
s32 fn_800094C4(struct nupakhdr_s* hdr, s32 handle, s32 required);

struct nufreeblock_s {
    u32 size;
    struct nufreeblock_s* next;
    struct nufreeblock_s* prev;
};

struct nufileheap_s {
    char* base;
    u32 size;
    s32 count;
    s32 used;
};

static inline u32 block_used(struct nufreeblock_s* block)
{
    if (block->size & 0x80000000) return 1;
    return 0;
}

static inline void unlink_block(struct nufreeblock_s* block)
{
    if (block->prev) block->prev->next = block->next;
    if (block->next) block->next->prev = block->prev;
}

static inline void merge_blocks(struct nufreeblock_s* a, struct nufreeblock_s* b)
{
    u32 end;
    a->size = (a->size & 0x80000000) | ((a->size & 0x7fffffff) + (b->size & 0x7fffffff) + 16);
    end = a->size & 0x7fffffff;
    end += (u32)a;
    *(struct nufreeblock_s**)(end + 12) = a;
}

#endif
