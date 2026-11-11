#include "nucore/nufile.h"

extern void fn_8000A714(s32 id, s32 a, s32* out);

void fn_8003A22C(s32, s32);
void fn_80005D34(s32);
s32 fn_80009C00(s32);
void (*fn_80009948(char*, s32))(char*, ...);
void (*fn_80009970(char*, s32))(char*, ...);
void* memset();
extern char lbl_8018CB9C[];
extern char str_NuFileClose_CloseFailedRetrying[];

void fn_80005764(s32 file)
{
    struct nufileslot_s* info;
    if (file >= 0x1000) fn_8003A22C(file - 0x1000, 0);
    else if (file >= 0x400) fn_80005D34(file);
    else {
        file--;
        while (fn_80009C00(file) < 0) {
            fn_80009970(lbl_8018CB9C, 0x626)(str_NuFileClose_CloseFailedRetrying);
            fn_80009948(lbl_8018CB9C, 0x627)(str_NuFileClose_CloseFailedRetrying);
        }
        info = &lbl_80200080[file];
        if (info->buffer) info->buffer->owner = 0;
        memset(info, 0, sizeof(*info));
    }
}

s32 fn_8000584C(s32 id)
{
    s32 out = 0;
    if (id <= 0x3ff) {
        fn_8000A714(id - 1, 0, &out);
        return out;
    }
    return 0;
}

s32 fn_80005894(char* handle, char** pos, char** end)
{
    s32 n = NuFileLoadBuffer(handle, *pos, *end - *pos);
    *pos = *pos + n;
    return n;
}
