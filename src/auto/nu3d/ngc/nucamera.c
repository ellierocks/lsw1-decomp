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
