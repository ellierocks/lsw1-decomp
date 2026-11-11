#include "types.h"
#include "nucore/numem.h"
#include "nucore/nufile.h"

struct numem_req_s {
    s32 result;
    s32 size;
};

void NuMemAllocFn(struct numem_req_s* req)
{
    s32 size = req->size;
    req->result = 0;
    fn_80005764(size);
}

/* NuMemAlloc/NuMemFree: 0x20-byte debug header, then the user payload. */
struct numem_block_s {
    struct numem_block_s* next;
    struct numem_block_s* prev;
    s32 line;
    s32 serial;
    s32 size;
    u8 guard[12];
};

struct numem_arena_s {
    u8 unknown00[4];
    s32 remaining;
    u8* cursor;
};

extern struct numem_block_s* lbl_80407A98;
extern s32 lbl_80407A9C;
extern struct numem_arena_s* lbl_80407AA0;
extern s32 lbl_80407AA4;
extern struct memexternal_s* lbl_80407AA8;
extern void* lbl_80407AAC;
extern s32 lbl_80407AB0;
extern s32 lbl_80407AB4;

extern char lbl_8018D5DC[];
extern char lbl_8018D610[];
extern char lbl_8018D630[];
extern char lbl_8018D658[];
void (*fn_80009948(char* file, s32 line))();
void (*fn_80009970(char* file, s32 line))();
extern char lbl_8018D3E8[];

/* Historical symbol: walk and validate the debug allocation list. */
void NuTexAnimProgAssembleEnd(s32 minimumSize)
{
    struct numem_block_s* block = lbl_80407A98;
    char* name;
    while (block) {
        name = (char*)block + block->size + 0x2c;
        if (block->size >= minimumSize) {
            fn_80009970(lbl_8018D3E8, 0x2c6)(lbl_8018D5DC, name, block->line, block->serial, block->size);
        }
        if (block->next == (struct numem_block_s*)0xfefefefe) {
            fn_80009948(lbl_8018D3E8, 0x2c9)(lbl_8018D610);
        }
        if (block->next && block->next->next == (struct numem_block_s*)0xfefefefe) {
            fn_80009948(lbl_8018D3E8, 0x2cd)(lbl_8018D630);
        }
        block = block->next;
    }
    fn_80009970(lbl_8018D3E8, 0x2d1)(lbl_8018D658, lbl_80407A9C);
}

void fn_80008CD8(void* heap, void* block);
void lbl_801556E4(void* block);
s32 memcmp(void* a, void* b, u32 size);
extern char lbl_8018D4E4[];
extern char lbl_8018D508[];
extern char lbl_8018D52C[];
extern char lbl_8018D554[];
struct numem_guard_s { s32 a, b, c; };
extern struct numem_guard_s lbl_801B0640[];
extern struct numem_guard_s lbl_801B0650[];
extern char str_NuMem_CannotFreeWhileExternalActive[];

void NuMemFree(void* data, char* file, s32 line)
{
    struct numem_block_s* block;
    if (lbl_80407AA8) {
        fn_80009948(lbl_8018D3E8, 0x268)(str_NuMem_CannotFreeWhileExternalActive);
    }
    if (!data) {
        fn_80009970(lbl_8018D3E8, 0x26b)(lbl_8018D4E4, file, line);
    } else {
        block = (struct numem_block_s*)((u8*)data - 0x20);
        if (memcmp((u8*)data + block->size, lbl_801B0650, 12)) {
            fn_80009970(lbl_8018D3E8, 0x273)(lbl_8018D508, file, line);
        }
        if (memcmp((u8*)data - 12, lbl_801B0640, 12)) {
            fn_80009970(lbl_8018D3E8, 0x277)(lbl_8018D52C, file, line);
        }
        if (!block->next) {
            if (!block->prev) {
                fn_80009970(lbl_8018D3E8, 0x27a)(lbl_8018D554, file, line);
            }
            if (!block->next) goto unlinked;
        }
        block->next->prev = block->prev;
unlinked:
        if (block->prev) {
            block->prev->next = block->next;
        } else {
            lbl_80407A98 = block->next;
        }
        {
            s32 remaining = lbl_80407A9C - block->size;
            block->next = 0;
            block->prev = 0;
            lbl_80407A9C = remaining;
        }
        if (lbl_80407AAC) {
            fn_80008CD8(lbl_80407AAC, block);
        } else {
            lbl_801556E4(block);
        }
    }
}

extern char lbl_8018D43C[];
extern char lbl_8018D484[];
extern char str_NuMem_AllocFailed[];
void* fn_80008E54(void* heap, s32 size, s32 align);
void* lbl_801556A8(s32 size);
u32 strlen(char* s);
void* memset(void* p, s32 v, u32 size);
char* strcpy(char* dst, char* src);

void* NuMemAlloc(s32 size, char* file, s32 line)
{
    struct numem_block_s* block;
    u8* payload;
    u8* result;
    s32 end;
    if (size <= 0) {
        fn_80009948(lbl_8018D3E8, 0x1a4)(str_NuMem_AllocFailed, size, file, line);
    }
    if (lbl_80407AA8) {
        lbl_80407AA8->ptr->addr = (lbl_80407AA8->ptr->addr + 15) & ~15;
        if ((u32)(lbl_80407AA8->end.addr - lbl_80407AA8->ptr->addr) >= (u32)size) {
            result = lbl_80407AA8->ptr->bytes;
            lbl_80407AA8->ptr->bytes = result + size;
            return result;
        }
        fn_80009948(lbl_8018D3E8, 0x1b9)(lbl_8018D484, size, file, line);
failed:
        return 0;
    }
    if (lbl_80407AA0) {
        u8* cursor;
        size = (size + 15) & ~15;
        if (size >= lbl_80407AA0->remaining) goto failed;
        cursor = lbl_80407AA0->cursor;
        lbl_80407AA0->remaining -= size;
        lbl_80407AA0->cursor = cursor + size;
        return cursor;
    }
    if (lbl_80407AAC) {
        block = fn_80008E54(lbl_80407AAC, strlen(file) + size + 0x2d, 4);
    } else {
        block = lbl_801556A8(strlen(file) + size + 0x2d);
    }
    result = (u8*)block;
    end = block == 0;
    if (end) {
        fn_80009970(lbl_8018D3E8, 0x1d1)(str_NuMem_AllocFailed, size, file, line);
        fn_80009948(lbl_8018D3E8, 0x1d2)(str_NuMem_AllocFailed, size, file, line);
    } else {
        struct numem_guard_s* tail;
        payload = (u8*)(block + 1);
        memset(block, 0, strlen(file) + size + 0x2d);
        lbl_80407A9C += size;
        block->size = size;
        *(struct numem_guard_s*)block->guard = lbl_801B0640[0];
        tail = (struct numem_guard_s*)(payload + size);
        *tail = lbl_801B0650[0];
        strcpy((char*)(tail + 1), file);
        block->line = line;
        block->serial = lbl_80407AA4++;
        block->next = lbl_80407A98;
        if (lbl_80407A98) lbl_80407A98->prev = block;
        block->prev = 0;
        lbl_80407A98 = block;
        result = payload;
    }
    end = (s32)result + size;
    if (end > lbl_80407AB0) lbl_80407AB0 = end;
    if (end > lbl_80407AB4) lbl_80407AB4 = end;
    if (end > 0x2000000) {
        fn_80009970(lbl_8018D3E8, 0x1fa)(lbl_8018D43C, size, result);
    }
    return result;
}
