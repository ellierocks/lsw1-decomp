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
