#include "types.h"

struct numem_req_s {
    s32 result;
    s32 size;
};

extern s32 fn_80005764(s32 size);

s32 NuMemAllocFn(struct numem_req_s* req)
{
    s32 size = req->size;
    req->result = 0;
    return fn_80005764(size);
}
