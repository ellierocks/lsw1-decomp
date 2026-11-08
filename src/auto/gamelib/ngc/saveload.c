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

s32 fn_800424F4();
extern s32 lbl_8040A434;

void fn_80042E28(void) {
    if (((s32) lbl_8040A434 == 0xB) || ((s32) lbl_8040A434 == 0x1A) || ((s32) lbl_8040A434 == 0x17)) {
        lbl_8040A434 = 0x11;
        fn_800424F4();
    }
}

void OSReport(void *, ...);
extern s32 lbl_80193898[];
extern s32 lbl_8019389C[];
extern s32 lbl_801938B0[];
extern s32 lbl_801938C4[];
extern s32 lbl_801938DC[];
extern s32 lbl_801938F0[];
extern s32 lbl_80193908[];
extern s32 lbl_80193920[];
extern s32 lbl_80193938[];
extern s32 lbl_80193954[];
extern s32 lbl_8019396C[];
extern s32 lbl_80193988[];
extern s32 lbl_8019399C[];
extern s32 lbl_801939B0[];
extern s32 lbl_801939C4[];
extern s32 lbl_801939DC[];
extern s32 lbl_801939F8[];
extern s32 lbl_80193A18[];
extern s32 lbl_80193A30[];
extern s32 lbl_80193A4C[];
extern s32 lbl_80193A64[];
extern s32 lbl_80193A80[];
extern s32 lbl_80193A9C[];
extern s32 lbl_80193AB4[];
extern s32 lbl_80193ACC[];
extern s32 lbl_80193AE4[];
extern s32 lbl_80193B00[];
extern s32 lbl_80193B18[];
extern s32 lbl_8040824C;
extern s32 lbl_8040A434;

void fn_80040E48(void) {
    s32 *var_r4;

    if ((s32) lbl_8040A434 != (s32) lbl_8040824C) {
        var_r4 = lbl_80193898;
        switch ((s32) lbl_8040A434) {               /* irregular */
        case 1:
            var_r4 = lbl_8019389C;
            break;
        case 2:
            var_r4 = lbl_801938B0;
            break;
        case 3:
            var_r4 = lbl_801938C4;
            break;
        case 4:
            var_r4 = lbl_801938DC;
            break;
        case 5:
            var_r4 = lbl_801938F0;
            break;
        case 6:
            var_r4 = lbl_80193908;
            break;
        case 7:
            var_r4 = lbl_80193920;
            break;
        case 8:
            var_r4 = lbl_80193938;
            break;
        case 9:
            var_r4 = lbl_80193954;
            break;
        case 10:
            var_r4 = lbl_8019396C;
            break;
        case 11:
            var_r4 = lbl_80193988;
            break;
        case 12:
            var_r4 = lbl_8019399C;
            break;
        case 13:
            var_r4 = lbl_801939B0;
            break;
        case 14:
            var_r4 = lbl_801939C4;
            break;
        case 15:
            var_r4 = lbl_801939DC;
            break;
        case 16:
            var_r4 = lbl_801939F8;
            break;
        case 17:
            var_r4 = lbl_80193A18;
            break;
        case 18:
            var_r4 = lbl_80193A30;
            break;
        case 19:
            var_r4 = lbl_80193A4C;
            break;
        case 20:
            var_r4 = lbl_80193A64;
            break;
        case 21:
            var_r4 = lbl_80193A80;
            break;
        case 22:
            var_r4 = lbl_80193A9C;
            break;
        case 23:
            var_r4 = lbl_80193AB4;
            break;
        case 24:
            var_r4 = lbl_80193ACC;
            break;
        case 25:
            var_r4 = lbl_80193AE4;
            break;
        case 26:
            var_r4 = lbl_80193B00;
            break;
        }
        OSReport(lbl_80193B18, var_r4);
        lbl_8040824C = lbl_8040A434;
    }
}

s32 (*fn_80009948(s32 *, s32))();
s32 fn_800424F4();
extern s32 lbl_80193B1C[];
extern s32 lbl_80193B78[];
extern s32 lbl_8040A434;
extern s32 lbl_8040A494;
extern s32 lbl_8040A498;
extern s32 lbl_8040A49C;

void fn_80042E6C(s32 arg0, s32 arg1, s32 arg2) {
    if (arg2 > 0x1C00) {
        fn_80009948(lbl_80193B1C, 0x73E)(lbl_80193B78);
        return;
    }
    if ((s32) lbl_8040A434 == 0xB) {
        lbl_8040A494 = arg0;
        lbl_8040A434 = 0xC;
        lbl_8040A498 = arg1;
        lbl_8040A49C = arg2;
        fn_800424F4();
    }
}

s32 (*fn_80009948(s32 *, s32))();
s32 fn_800424F4();
extern s32 lbl_80193B1C[];
extern s32 lbl_80193B78[];
extern s32 lbl_8040A434;
extern s32 lbl_8040A494;
extern s32 lbl_8040A498;
extern s32 lbl_8040A49C;
extern s32 lbl_8040A4A0;

void fn_80042EE0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (arg2 > 0x1C00) {
        fn_80009948(lbl_80193B1C, 0x76B)(lbl_80193B78);
        return;
    }
    if ((s32) lbl_8040A434 == 0xB) {
        lbl_8040A494 = arg0;
        lbl_8040A434 = 0xD;
        lbl_8040A498 = arg1;
        lbl_8040A49C = arg2;
        lbl_8040A4A0 = arg3;
        fn_800424F4();
    }
}

void *fn_80040DE8(void *arg0, s32 arg1) {
    void *temp_r3;

    temp_r3 = arg0 + (((*(s16 *)((s8 *)(arg0) + (0x14))) * arg1) + 0x20);
    if ((*(s32 *)((s8 *)(temp_r3) + (0xC))) & 0x8000) {
        return temp_r3 + 0x10;
    }
    return NULL;
}
