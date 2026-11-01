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
