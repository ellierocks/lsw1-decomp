#ifndef NUMEM_H
#define NUMEM_H

#include "types.h"

/* Integer view keeps cursor alignment distinct from pointer dereferences. */
union variptr_u {
    u32 addr;
    void* ptr;
    u8* bytes;
};

struct memexternal_s {
    union variptr_u* ptr;
    union variptr_u end;
};

/* Retail allocation/free preserve file and line in r4/r5 for diagnostics. */
void* NuMemAlloc(s32 size, char* file, s32 line);
void NuMemFree(void* data, char* file, s32 line);

#endif
