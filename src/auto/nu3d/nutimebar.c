#include "types.h"

struct nutimebar_s {
    u8 pad[0x1b4];
    s16 count;
    u8 pad1b6[4];
    s16 ids[1];
};

extern struct nutimebar_s* lbl_8040A2F8;

void fn_800162A0(void)
{
    struct nutimebar_s* t = lbl_8040A2F8;
    t->ids[t->count++] = 12;
}

void fn_80016360(void)
{
    struct nutimebar_s* t = lbl_8040A2F8;
    t->ids[t->count++] = 14;
}

void fn_80016388(void)
{
    struct nutimebar_s* t = lbl_8040A2F8;
    t->ids[t->count++] = 16;
}

s32 NuFParGetInt(void*);
void fn_800162C8(void* parser)
{
    s32 first = NuFParGetInt(parser);
    s32 second;
    struct nutimebar_s* t;
    if (first == 0) {
        first = 0x7fffffff;
    }
    second = NuFParGetInt(parser);
    t = lbl_8040A2F8;
    t->ids[t->count++] = 13;
    t->ids[t->count++] = first;
    t->ids[t->count++] = second;
}

void NuFParGetWord(void*);
char* strcpy(char*, const char*);
void fn_800163B0(u8* parser)
{
    NuFParGetWord(parser);
    parser[0x32a] = 0;
    strcpy((u8*)lbl_8040A2F8 + 8, parser + 0x30a);
}

struct nutimebar_copy_s {
    u32 words[16];
};
extern char lbl_8018FF7C[];
extern struct nutimebar_copy_s lbl_801B1598;
void* NuMemAlloc(s32, char*, s32);
void memset();

void* fn_800163F4(u8** arena, s32 unused, struct nutimebar_copy_s* data)
{
    u8* result;
    if (arena) {
        result = *arena;
        *arena = result + 0x214;
        *(s32*)result = 1;
    } else {
        result = NuMemAlloc(0x214, lbl_8018FF7C, 0x80);
        *(s32*)result = 0;
    }
    memset(result, 0, 0x214);
    if (data) {
        *(struct nutimebar_copy_s*)(result + 0x144) = *data;
    } else {
        *(struct nutimebar_copy_s*)(result + 0x144) = lbl_801B1598;
    }
    return result;
}
