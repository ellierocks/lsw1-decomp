#include "nucore/nufile.h"

void fn_80005764(s32 handle);

s32 NuFileExists(char* name)
{
    s32 file = NuFileOpen(name, NUFILE_READ);
    if (file != 0) {
        fn_80005764(file);
        return 1;
    }
    return 0;
}
