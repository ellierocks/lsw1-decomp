#include "types.h"

typedef union {
    void* ptr;
    u32 addr;
} NuPortalVariptr;

extern void* lbl_80407C38;
extern f32 lbl_801905E8[];
extern void NuAnimData2Clone(void*, void*, void*, f32);
extern f32* NuCameraGetProjectionMtx(void);
extern f32 lbl_80190824[];
extern f32 lbl_80190828[];
typedef struct { f32 x, y, z, w; } NuPortalVec4;
typedef struct { f32 x, y, z; } NuPortalVec3;
typedef struct { f32 x, y, z, d; } NuPortalPlane;
typedef struct {
    f32 transposed[32];
    NuPortalPlane* planes;
    u8 pad84[0x18];
    s16 plane_count;
} NuPortalFrustum;
extern NuPortalPlane lbl_803145D0;
extern NuPortalVariptr lbl_8040A328;
extern NuPortalVariptr lbl_8040A32C;
extern s32* lbl_8040A330;
extern void** lbl_8040A334;
extern char lbl_80190974[];
extern char lbl_801909A4[];
extern char lbl_801909C4[];
extern f32 lbl_80190A28[];
extern void (*fn_80009948(char*, s32))(char*, ...);

void NuAnimBuffCreate(s32 max_joints, NuPortalVariptr* buffer, NuPortalVariptr buffer_end)
{
    u32* anim_buffer = (u32*)((buffer->addr + 15) & ~15);
    buffer->addr = (u32)(anim_buffer + 3);
    anim_buffer[2] = (u32)(anim_buffer + 4);
    buffer->addr = (buffer->addr + 15 & ~15) + max_joints * 0x24;
    anim_buffer[1] = max_joints;
    anim_buffer[0] = 0;
    lbl_80407C38 = anim_buffer;
}

void* fn_8001BFC4(void* data, void* source, f32 time)
{
    void* buffer = ((void**)lbl_80407C38)[2];
    NuAnimData2Clone(data, buffer, source, time - lbl_801905E8[0]);
    return buffer;
}

void NuAnimCurveEval(f32* out)
{
    f32* projection = NuCameraGetProjectionMtx();
    f32 first = projection[0];
    f32 one = lbl_80190828[0];
    f32 zero = lbl_80190824[0];
    out[0] = -first;
    out[4] = projection[1];
    out[8] = -projection[2];
    out[1] = -projection[4];
    out[5] = projection[5];
    out[9] = -projection[6];
    out[2] = -projection[8];
    out[6] = projection[9];
    out[10] = -projection[10];
    out[15] = one;
    out[3] = zero;
    out[11] = zero;
    out[7] = zero;
}

s32 clipTestSphere(NuPortalVec4* sphere, NuPortalFrustum* frustum)
{
    s32 inside = 0;
    s32 i;
    for (i = 0; i < frustum->plane_count; i++) {
        NuPortalPlane* plane = &frustum->planes[i];
        f32 distance = plane->x * sphere->x + plane->y * sphere->y + plane->z * sphere->z + plane->d;
        if (distance < -sphere->w) return 0;
        if (distance > sphere->w) inside++;
    }
    {
        NuPortalPlane* plane = &lbl_803145D0;
        f32 distance = plane->x * sphere->x + plane->y * sphere->y + plane->z * sphere->z + plane->d;
        if (distance < -sphere->w) return 0;
        if (distance > sphere->w) inside++;
    }
    return inside == frustum->plane_count + 1 ? 1 : 2;
}

NuPortalFrustum* NuFrustrumAlloc(s32 count)
{
    NuPortalFrustum* frustum = (NuPortalFrustum*)lbl_8040A328.ptr;
    lbl_8040A328.addr = (lbl_8040A328.addr + 15 & ~15) + 0xa0;
    if (lbl_8040A328.addr >= lbl_8040A32C.addr)
        fn_80009948(lbl_80190974, 0xdd)(lbl_801909A4);
    frustum->planes = lbl_8040A328.ptr;
    lbl_8040A328.addr += count * 16;
    if (lbl_8040A328.addr >= lbl_8040A32C.addr)
        fn_80009948(lbl_80190974, 0xe3)(lbl_801909A4);
    frustum->plane_count = count;
    {
        s32 index = *lbl_8040A330;
        lbl_8040A334[index] = frustum;
        *lbl_8040A330 = index + 1;
    }
    if (*lbl_8040A330 > 15)
        fn_80009948(lbl_80190974, 0xeb)(lbl_801909C4);
    return frustum;
}

s32 clipTestBox(NuPortalVec3* minimum, NuPortalVec3* maximum, NuPortalPlane* planes, s32 count)
{
    NuPortalVec3 corners[8];
    s32 plane_index;
    s32 inside = 0;
    corners[0].x = minimum->x;
    corners[0].y = minimum->y;
    corners[0].z = minimum->z;
    corners[1].x = maximum->x;
    corners[1].y = minimum->y;
    corners[1].z = minimum->z;
    corners[2].x = minimum->x;
    corners[2].y = maximum->y;
    corners[2].z = minimum->z;
    corners[3].x = maximum->x;
    corners[3].y = maximum->y;
    corners[3].z = minimum->z;
    corners[4].x = minimum->x;
    corners[4].y = minimum->y;
    corners[4].z = maximum->z;
    corners[5].x = maximum->x;
    corners[5].y = minimum->y;
    corners[5].z = maximum->z;
    corners[6].x = minimum->x;
    corners[6].y = maximum->y;
    corners[6].z = maximum->z;
    corners[7].x = maximum->x;
    corners[7].y = maximum->y;
    corners[7].z = maximum->z;
    for (plane_index = 0; plane_index < count; plane_index++) {
        s32 outside = 0;
        s32 corner_index;
        for (corner_index = 0; corner_index < 8; corner_index++) {
            f32 distance = planes->x * corners[corner_index].x + planes->y * corners[corner_index].y + planes->z * corners[corner_index].z + planes->d;
            if (distance < lbl_80190A28[0]) outside++;
            else inside++;
        }
        if (outside == 8) return 0;
        planes++;
    }
    {
        s32 result = 1;
        if (inside != count * 8) result = 2;
        return result;
    }
}
