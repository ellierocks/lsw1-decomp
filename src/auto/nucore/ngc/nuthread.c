#include "types.h"

struct nuthread_slot_s {
    u8 pad0[0x34];
    s32 base;
    s32 pad38;
    s32 value;
    u8 pad40[0xc];
};

struct nusem_s {
    s32 count;
    s32 queue[2];
};

extern struct nuthread_slot_s lbl_80207AC0[];
extern struct nusem_s lbl_80207DC0;
extern struct nusem_s lbl_8020A140[];
extern u8 lbl_80209DF0[];
extern s32 lbl_80407B04;
extern s32 lbl_80407B0C;
extern s32 lbl_80407B10;
extern s32 lbl_80407B18;
extern u8 lbl_8040A2E0[2];
extern char lbl_8018E518[];
extern char lbl_8018E54C[];

void OSInitSemaphore(void* sem, s32 count);
void OSWaitSemaphore(void* sem);
void OSSignalSemaphore(void* sem);
void OSSuspendThread(void* thread);
s32 DVDClose(s32 file);
void* fn_80009948(char* file, s32 line);

s32 fn_8000A64C(s32 idx, s32 val, s32 mode)
{
    struct nuthread_slot_s* t = &lbl_80207AC0[idx];
    s32 base;
    if (mode == 1 && val == 0) {
        return t->value;
    }
    if (lbl_80407B04 == 0) {
        OSInitSemaphore(&lbl_80207DC0, 1);
        lbl_80407B04 = 1;
    }
    OSWaitSemaphore(&lbl_80207DC0);
    base = t->base;
    switch (mode) {
    case 0:
        t->value = val;
        break;
    case 1:
        base = t->value;
    case 2:
        t->value = base + val;
        break;
    }
    OSSignalSemaphore(&lbl_80207DC0);
    return t->value;
}

s32 fn_8000A714(s32 a, s32 b, s32* out)
{
    *out = 0;
    return 0;
}

void fn_8000A724(void)
{
    while (1) {
        DVDClose(lbl_80407B0C);
        lbl_80407B0C = 0;
        OSSuspendThread(lbl_80209DF0);
    }
}

void fn_8000A754(void)
{
    lbl_80407B10 = 0;
}

void fn_8000A760(s32 a, s32* b)
{
    b[0x48 / 4] = a;
}

s32 fn_8000A768(s32* p)
{
    p[3] = 1;
    return 1;
}

s32 fn_8000A778(s32* p)
{
    p[3] = 1;
    return 1;
}

s32 fn_8000A788(void)
{
    s32 i;
    if (lbl_80407B18 == 0) {
        *(u16*)lbl_8040A2E0 = 0;
        lbl_80407B18 = 1;
    }
    for (i = 0; i < 2; i++) {
        if (lbl_8040A2E0[i] == 0) {
            lbl_8040A2E0[i] = 1;
            OSInitSemaphore(&lbl_8020A140[i], 1);
            return i;
        }
    }
    ((void (*)(char*, ...))fn_80009948(lbl_8018E518, 0x39))(lbl_8018E54C);
    return -1;
}
