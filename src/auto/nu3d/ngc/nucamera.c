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

s32 NuAnimCurveSetApplyBlendToJoint2(s32 *, s32);
extern s32 lbl_80314354[];
extern s32 lbl_80407C58;

void NuCameraTransformScreenClip(void) {
    if ((s32) lbl_80407C58 == 1) {
        NuAnimCurveSetApplyBlendToJoint2(lbl_80314354, 0);
    }
}

s32 NuAnimCurveSetApplyBlendToJoint2(s32 *, s32);
extern s32 lbl_803143BC[];
extern s32 lbl_80407C58;

void NuPs2CreateSkin(void) {
    if ((s32) lbl_80407C58 == 1) {
        NuAnimCurveSetApplyBlendToJoint2(lbl_803143BC, 0);
    }
}

void NuCameraCalcFrustrumPlanes(s32 *arg0) {
    NuAnimCurveSetApplyBlendToJoint2(arg0, 0);
}

void fn_80021738(void *arg0, void *arg1, s32 arg2) {
    void *temp_r11;

    temp_r11 = (*(void **)((s8 *)(arg1) + (0x1A0)));
    if (temp_r11 != NULL) {
        (*(void **)((s8 *)(arg0) + (0))) = arg1;
        (*(s32 *)((s8 *)(arg0) + (4))) = 0;
        (*(void **)((s8 *)(arg0) + (8))) = (void *) ((*(s32 *)((s8 *)((*(void **)((s8 *)(arg1) + (0x1A0)))) + (0x70))) + (arg2 * 0xD0));
        return;
    }
    (*(void **)((s8 *)(arg0) + (0))) = arg1;
    (*(s32 *)((s8 *)(arg0) + (4))) = (s32) ((*(s32 *)((s8 *)(arg1) + (0x24))) + (arg2 * 0x50));
    (*(void **)((s8 *)(arg0) + (8))) = temp_r11;
}
