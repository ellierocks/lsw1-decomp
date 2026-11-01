#include "types.h"


s32 fn_80042E20(void)
{
    return 0;
}

struct PoolNode;
struct Pool {
    struct PoolNode *free_head, *free_tail, *used_head, *used_tail;
    u16 count, size, stride, used;
    s32 owner;
};
struct PoolNode { struct Pool* pool; struct PoolNode *next, *prev; u32 flags; };
extern s32 lbl_8040810C;
extern void (*lbl_80407B1C)(void), (*lbl_80407B20)(void);

void* fn_80040C24(struct Pool* pool)
{
    struct PoolNode* node;
    s32 owner;
    if (pool->free_head) {
        owner = lbl_8040810C;
        if (pool->owner != owner) lbl_80407B1C();
        node = pool->free_head;
        pool->free_head = node->next;
        if (!pool->free_head) pool->free_tail = 0;
        else pool->free_head->prev = 0;
        node->prev = pool->used_tail;
        if (pool->used_tail) pool->used_tail->next = node;
        else pool->used_head = node;
        node->next = 0;
        pool->used_tail = node;
        node->flags |= 0x8000;
        pool->used++;
        if (pool->owner != owner) lbl_80407B20();
        return node + 1;
    } else return 0;
}

void fn_80040D00(void* data)
{
    struct PoolNode* node = (struct PoolNode*)data;
    s32 owner = lbl_8040810C;
    struct Pool* pool = (--node)->pool;
    if (pool->owner != owner) lbl_80407B1C();
    if (node->next) node->next->prev = node->prev;
    else pool->used_tail = node->prev;
    if (node->prev) node->prev->next = node->next;
    else pool->used_head = node->next;
    node->prev = pool->free_tail;
    node->next = 0;
    if (pool->free_tail) pool->free_tail->next = node;
    else { pool->free_head = node; pool->free_tail = node; }
    pool->free_tail = node;
    pool->used--;
    node->flags &= ~0x8000;
    if (pool->owner != owner) lbl_80407B20();
}
