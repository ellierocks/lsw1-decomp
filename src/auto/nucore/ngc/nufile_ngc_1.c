#include "types.h"

extern void OSReport(const char* fmt, ...);
extern char lbl_8018E2B4[];

void fn_800099D4(char* fmt)
{
    OSReport(fmt);
}

void fn_800099F8(s32 arg)
{
    OSReport(lbl_8018E2B4, arg);
}
