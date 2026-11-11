#include "types.h"

/* Three-word headers plus a four-byte footer give 16 bytes of overhead.
 * The high size bit marks allocation; the footer follows the payload. */
struct nuheap_block_s {
    u32 sizeFlags;
    struct nuheap_block_s* next;
    struct nuheap_block_s* prev;
};

struct nuheap_s {
    struct nuheap_s* base;
    u32 size;
    s32 allocations;
    u32 used;
};

struct nuheap_s* fn_80008A60(u8** cursor, s32 unused, u32 size);
void* NuHeap(struct nuheap_s* heap, s32 size, s32 alignment);

struct nuheap_s* fn_80008A60(u8** cursor, s32 unused, u32 size)
{
    struct nuheap_s* heap;
    struct nuheap_block_s* head;
    struct nuheap_block_s* start;
    struct nuheap_block_s* end;
    struct nuheap_block_s* freeBlock;
    u8* limit;
    if (size > 0x4f) {
        heap = (struct nuheap_s*)*cursor;
        limit = (u8*)heap + size;
        head = (struct nuheap_block_s*)((u8*)heap + 0x10);
        *cursor = limit;
        freeBlock = (struct nuheap_block_s*)((u8*)heap + 0x30);
        head->sizeFlags = 0;
        start = (struct nuheap_block_s*)((u8*)heap + 0x20);
        head->next = freeBlock;
        end = (struct nuheap_block_s*)(limit - 0x10);
        head->prev = 0;
        *(struct nuheap_block_s**)(((head->sizeFlags & 0x7fffffff) + (u32)head) + 12) = head;
        start->sizeFlags = 0x80000000;
        start->next = 0;
        start->prev = 0;
        *(struct nuheap_block_s**)(((start->sizeFlags & 0x7fffffff) + (u32)start) + 12) = start;
        end->sizeFlags = 0x80000000;
        end->next = 0;
        end->prev = 0;
        *(struct nuheap_block_s**)(((end->sizeFlags & 0x7fffffff) + (u32)end) + 12) = end;
        freeBlock->sizeFlags = size - 0x50;
        freeBlock->prev = head;
        freeBlock->next = 0;
        *(struct nuheap_block_s**)(((freeBlock->sizeFlags & 0x7fffffff) + (u32)freeBlock) + 12) = freeBlock;
        heap->size = size;
        heap->used = 0;
        heap->base = heap;
        heap->allocations = 0;
        return heap;
    }
    return 0;
}

void (*fn_8000995C(char* file, s32 line))();
extern char lbl_8018DFA4[];
extern char lbl_8018DFD0[];

void* NuHeap(struct nuheap_s* heap, s32 size, s32 alignment)
{
    struct nuheap_block_s* head = (struct nuheap_block_s*)((u8*)heap->base + 0x10);
    struct nuheap_block_s* block;
    struct nuheap_block_s* allocated;
    s32 remaining;
    s32 request;
    u32 delta;
    u8* tail;
    if (!size) return 0;
    request = size + alignment;
    block = head;
    if (block) goto search;
failed:
    fn_8000995C(lbl_8018DFA4, 0x1be)(lbl_8018DFD0);
    return 0;
search:
    for (;;) {
        if ((block->sizeFlags & 0x7fffffff) > (u32)(request + 0x10)) break;
        block = block->next;
        if (!block) goto failed;
    }
    if (block->prev) block->prev->next = block->next;
    if (block->next) block->next->prev = block->prev;
    remaining = (block->sizeFlags & 0x7fffffff) - request - 8;
    tail = (u8*)((u32)block + (remaining + 4));
    remaining -= 8;
    allocated = (struct nuheap_block_s*)(tail + 4);
    if (alignment) {
        delta = (((u32)tail + 15 + alignment) & -alignment) - ((u32)tail + 16);
        request -= delta;
        allocated = (struct nuheap_block_s*)((u8*)allocated + delta);
        remaining += delta;
    }
    allocated->sizeFlags = request | 0x80000000;
    allocated->next = 0;
    allocated->prev = 0;
    *(struct nuheap_block_s**)(((allocated->sizeFlags & 0x7fffffff) + (u32)allocated) + 12) = allocated;
    block->prev = 0;
    block->sizeFlags = remaining;
    block->next = 0;
    *(struct nuheap_block_s**)(((remaining & 0x7fffffff) + (u32)block) + 12) = block;
    if (head->next) head->next->prev = block;
    block->prev = head;
    block->next = head->next;
    head->next = block;
    heap->allocations++;
    heap->used += 0x10 + (allocated->sizeFlags & 0x7fffffff);
    return (u8*)allocated + 12;
}
