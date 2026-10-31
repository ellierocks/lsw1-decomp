#include "types.h"

extern u8 lbl_80313298[];
extern void fn_8000CD08(s32 a, u8* b, s32 c);
#include "numath/numath.h"

extern struct numtx_s lbl_80313268;
extern struct numtx_s lbl_801B1638;
extern struct numtx_s lbl_801B17F8;

struct nuanimdata2_s {
    s32 magic;
    s16 end;
};

struct numtx_s* NuCameraGetMtx(void)
{
    return &lbl_80313268;
}

struct numtx_s* NuCameraGetViewMtx(void)
{
    return &lbl_80313268;
}

struct numtx_s* NuCameraGetProjectionMtx(void)
{
    return &lbl_801B1638;
}

struct numtx_s* NuCameraGetClippingMtx(void)
{
    return &lbl_801B17F8;
}

s32 NuAnimData2GetEnd(struct nuanimdata2_s* d)
{
    if (d->magic == 0x414e4934) {
        return (u16)d->end;
    }
    return d->end;
}

s32 fn_8002171C(u8* p)
{
    u8* q = *(u8**)(p + 0x1a0);
    if (q != 0) {
        return *(s32*)(q + 0x6c);
    }
    return *(s32*)(p + 0x20);
}

void NuCameraDistSqr(s32 a)
{
    fn_8000CD08(a, lbl_80313298, 0);
}
