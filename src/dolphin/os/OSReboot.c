#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

extern void* SaveStart;
extern void* SaveEnd;

void __OSReboot(u32 resetCode, u32 bootDol) {
    OSContext exceptionContext;
    char* argvToPass;

	OSDisableInterrupts();
    OSSetArenaLo((void*)0x81280000);
    OSSetArenaHi((void*)0x812f0000);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    argvToPass = NULL;
    __OSBootDol(bootDol, resetCode | 0x80000000, &argvToPass);
}

void OSGetSaveRegion(void** start, void** end) {
    *start = SaveStart;
    *end = SaveEnd;
}
