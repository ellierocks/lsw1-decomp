#ifndef NUMEM_H
#define NUMEM_H

#include "types.h"

struct memexternal_s {
    union variptr_u* ptr;
    union variptr_u* end;
};

void* NuMemAlloc(s32 size);
void NuMemFree(void* data);

#endif
