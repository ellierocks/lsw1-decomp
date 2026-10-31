#include "types.h"

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
};

extern s32 lbl_80407AD8;

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
