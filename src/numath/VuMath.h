#ifndef NUMATH_VUMATH_H
#define NUMATH_VUMATH_H

#include "types.h"

// nu2api.legogc/numath/ngc/VuMath.h. Its only retail trace is the unreachable
// VuMtxInitRot stub: every TU that includes the header carries the stub's two
// error strings at the start of its .rodata, because ProDG emits a static
// inline function's string constants where the function is defined.
// fn_80009948 is the Mac demo's NuErrorProlog (MEDIUM pair); it returns the
// reporter that takes the message. The stub's line number is not recoverable.
void (*fn_80009948(char* file, s32 line))(char* fmt, ...);

static inline void VuMtxInitRot(void)
{
    fn_80009948("../nu2api.legogc/numath/ngc/VuMath.h", 0)("VuMtxInitRot NOT fully implemented!");
}

#endif
