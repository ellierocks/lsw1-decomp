#include "game/turret.h"

extern void* memset();
extern void (*fn_80009948(char*, s32))(char*, ...);
extern char str_SourceFile_Turret[];
extern char str_Assert_Turret_OutOfMemory[];

struct NUTURRET_s* fn_8014068C(char** cursor, char** end, u32 value)
{
    struct NUTURRET_s* turret;
    *cursor = (char*)(((u32)*cursor + 3) & ~3);
    turret = (struct NUTURRET_s*)*cursor;
    if (*cursor + sizeof(*turret) >= *end) {
        fn_80009948(str_SourceFile_Turret, 0xaa)(str_Assert_Turret_OutOfMemory);
        return NULL;
    }
    *cursor += sizeof(*turret);
    if (turret != NULL) {
        memset(turret, 0, sizeof(*turret));
        turret->valueF04 = value;
    }
    return turret;
}

s32 fn_800716B0(s32);
s32 fn_80071720(s32, s32, s32, s32, s32, f32);
extern f32 lbl_801AEF54[];
extern void *lbl_80409410;

void fn_8013FB54(s32 arg0, s32 arg1) {
    fn_800716B0(arg1);
    if (arg0 != 0) {
        fn_80071720((*(s32 *)((s8 *)(lbl_80409410) + (0x760))), arg1, arg0, 0, -1, lbl_801AEF54[0]);
    }
}

void fn_8002D9E8(void*, void*, void*, void*, void*, void*);
void fn_8002DAD0(void*);
void fn_8002DB1C(f32*);
void fn_800716D4(void*);
void fn_80071D90(s32);
extern char lbl_801B0ECC[];
extern char lbl_801B0EF0[];
extern f32 lbl_801AEF4C[];
extern f32 lbl_801AEF50[];

void fn_8013FA74(void) {
    struct { char unk0[0xC0]; f32 v[3]; char unkCC[0x78]; } buf;
    f32 out[4];

    memset(&buf, 0, 0x144);
    fn_80071720(*(s32*)((char*)lbl_80409410 + 0x760), (s32)&buf, 0, 0, 0x10, lbl_801AEF4C[0]);
    fn_800716D4(&buf);
    out[0] = buf.v[0];
    out[1] = buf.v[1];
    out[2] = buf.v[2];
    out[3] = lbl_801AEF50[0];
    fn_8002DB1C(out);
}

void fn_8013FBB4(void) {
    char buf[0xC];

    memset(buf, 0, 0xC);
    fn_8002D9E8(lbl_801B0EF0, buf, lbl_801B0EF0, buf, lbl_801B0EF0, buf);
    fn_8002DAD0(lbl_801B0ECC);
}

void fn_8013FC1C(s32 arg0, f32 farg0) {
    char buf[0x144];

    memset(buf, 0, 0x144);
    fn_80071D90(0);
    fn_80071720(*(s32*)((char*)lbl_80409410 + 0x760), (s32)buf, arg0, 0, -1, farg0);
    fn_80071D90(1);
    fn_800716D4(buf);
}
