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

typedef struct {
    s32 count;
    s32 max_joints;
    f32* data;
} NuPortalAnimBuff;
typedef struct {
    u8 pad0[0x50];
    u8 parent_ix;
    u8 pad51[0xf];
} NuPortalJoint;
typedef struct {
    u8 pad0[0x14];
    NuPortalJoint* joints;
    u8 pad18[0x64];
    u8 num_joints;
} NuPortalHGobj;
extern f32 lbl_801905DC[];
void NuAnimData2Apply(void* anim, NuPortalAnimBuff* buffer, s32 count, s32 first, f32 time);
void NuAnimData2CalcMatrix(void* anim, NuPortalAnimBuff* buffer, s32 count, s32 first, NuPortalVec3* root, f32 time);

void NuAnimBuffEvaluate(NuPortalAnimBuff* buffer, void* anim, f32 time, s32 apply, f32 blend, s32 start,
                        NuPortalHGobj* hgobj, NuPortalVec3* root)
{
    s32 end;
    if (buffer == NULL)
        buffer = lbl_80407C38;
    buffer->count = hgobj->num_joints;
    if (start != 0) {
        NuPortalJoint* joints = hgobj->joints;
        u8 parent = joints[start].parent_ix;
        for (end = start + 1; end < hgobj->num_joints; end++) {
            if (joints[end].parent_ix == parent)
                break;
        }
    } else {
        end = hgobj->num_joints;
    }
    if (apply) {
        NuAnimData2Apply(anim, buffer, end - start, start, time - lbl_801905DC[0]);
        if (root != NULL) {
            root->x = buffer->data[0];
            root->y = buffer->data[1];
            root->z = -buffer->data[2];
        }
    } else {
        NuAnimData2CalcMatrix(anim, buffer, end - start, start, root, time - lbl_801905DC[0]);
    }
}

void* fn_8001BFC4(void* data, void* source, f32 time)
{
    void* buffer = ((void**)lbl_80407C38)[2];
    NuAnimData2Clone(data, buffer, source, time - lbl_801905E8[0]);
    return buffer;
}

#define _STDARG_H
#include "sn/include/va-ppc.h"
typedef __gnuc_va_list va_list;
extern char lbl_80407C40[8];
extern char* lbl_8040AD34;
extern char* lbl_8040AD38;
extern char lbl_803132D0[];
extern s32 lbl_80407C48;
extern char lbl_8019063C[];
extern char lbl_80190684[];
s32 vsprintf(char*, char*, va_list);
s32 NuStrLen(char*);
void fn_80005A4C(s32, char*);

void fn_8001C00C(char* fmt, ...)
{
    va_list args;
    if (fmt == NULL || *fmt == 0)
        fmt = lbl_80407C40;
    va_start(args, fmt);
    vsprintf(lbl_8040AD34, fmt, args);
    lbl_8040AD34 += NuStrLen(lbl_8040AD34);
    if ((u32)lbl_8040AD34 > (u32)lbl_8040AD38) {
        fn_80005A4C(lbl_80407C48, lbl_803132D0);
        lbl_8040AD34 = lbl_803132D0;
        lbl_8040AD38 = lbl_803132D0 + 0xc00;
    }
    va_end(args);
}

void fn_8001C110(char* fmt, ...)
{
    va_list args;
    if (fmt == NULL || *fmt == 0)
        fmt = lbl_80407C40;
    fn_8001C00C(lbl_8019063C, 0x25);
    va_start(args, fmt);
    vsprintf(lbl_8040AD34, fmt, args);
    lbl_8040AD34 += NuStrLen(lbl_8040AD34);
    fn_8001C00C(lbl_80190684);
    va_end(args);
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
    out[11] = zero;
    out[7] = zero;
    out[3] = zero;
    out[15] = one;
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

typedef struct {
    u8 pad0[0x1c];
    u16 count;
} NuPortalSet;
typedef struct {
    u8 pad0[0x14];
    NuPortalSet* set;
    u8 pad18[0xc];
    s32 unk24;
    u32 flags0 : 4;
    u32 enabled : 1;
    u32 flags5 : 27;
} NuPortalOwner;
extern s32 lbl_80407C80;
extern s32 lbl_80407C88;
void fn_8001D0D8(void);
void fn_8001D25C(void);
void fn_8001D7EC(NuPortalSet* set, s32 index, s32 arg2, s32 arg3);

void fn_8001DC4C(NuPortalOwner* owner, u8* scene)
{
    s32 i;
    owner->enabled = owner->set != NULL && lbl_80407C80 != 0;
    if (owner->enabled) {
        if (lbl_80407C88 == 0)
            fn_8001D0D8();
        fn_8001D25C();
        i = owner->set->count;
        while (--i >= 0)
            fn_8001D7EC(owner->set, i, *(s32*)(scene + 0x1a0), owner->unk24);
    }
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
    lbl_8040A334[(*lbl_8040A330)++] = frustum;
    if (*lbl_8040A330 > 15)
        fn_80009948(lbl_80190974, 0xeb)(lbl_801909C4);
    return frustum;
}

typedef struct {
    f32 x, y, z, w;
    u32 colour;
    f32 u, v;
} NuPortalLineVertex2D;
extern f32 lbl_801909EC[];
extern f32 lbl_801909F0[];
void fn_80027824(NuPortalLineVertex2D* lines, void* unused, s32 count);

void drawSquare(NuPortalVec3* minimum, NuPortalVec3* maximum, u32 colour)
{
    NuPortalLineVertex2D lines[8];
    f32 left, top, right, bottom;
    left = minimum->x * lbl_801909EC[0] + lbl_801909EC[0];
    top = -minimum->y * lbl_801909EC[0] + lbl_801909EC[0];
    bottom = -maximum->y * lbl_801909EC[0] + lbl_801909EC[0];
    right = maximum->x * lbl_801909EC[0] + lbl_801909EC[0];
    lines[0].x = left;
    lines[0].y = top;
    lines[0].z = lbl_801909F0[0];
    lines[0].colour = colour;
    lines[1].x = left;
    lines[1].y = bottom;
    lines[1].z = lbl_801909F0[0];
    lines[1].colour = colour;
    lines[2].x = left;
    lines[2].y = bottom;
    lines[2].z = lbl_801909F0[0];
    lines[2].colour = colour;
    lines[3].x = right;
    lines[3].y = bottom;
    lines[3].z = lbl_801909F0[0];
    lines[3].colour = colour;
    lines[4].x = right;
    lines[4].y = bottom;
    lines[4].z = lbl_801909F0[0];
    lines[4].colour = colour;
    lines[5].x = right;
    lines[5].y = top;
    lines[5].z = lbl_801909F0[0];
    lines[5].colour = colour;
    lines[6].x = right;
    lines[6].y = top;
    lines[6].z = lbl_801909F0[0];
    lines[6].colour = colour;
    lines[7].x = left;
    lines[7].y = top;
    lines[7].z = lbl_801909F0[0];
    lines[7].colour = colour;
    fn_80027824(lines, 0, 8);
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

void transposeClipPlanes(void *arg0) {
    s16 temp_r0;
    void *temp_r11;

    (*(f32 *)((s8 *)(arg0) + (0xC))) = (f32) lbl_803145D0.x;
    (*(f32 *)((s8 *)(arg0) + (0x1C))) = (f32) lbl_803145D0.y;
    (*(f32 *)((s8 *)(arg0) + (0x2C))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x3C))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0x40))) = (f32) lbl_803145D0.x;
    (*(f32 *)((s8 *)(arg0) + (0x50))) = (f32) lbl_803145D0.y;
    (*(f32 *)((s8 *)(arg0) + (0x60))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x70))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0x44))) = (f32) lbl_803145D0.x;
    (*(f32 *)((s8 *)(arg0) + (0x54))) = (f32) lbl_803145D0.y;
    (*(f32 *)((s8 *)(arg0) + (0x64))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x74))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0x48))) = (f32) lbl_803145D0.x;
    (*(f32 *)((s8 *)(arg0) + (0x58))) = (f32) lbl_803145D0.y;
    (*(f32 *)((s8 *)(arg0) + (0x68))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x78))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0x4C))) = (f32) lbl_803145D0.x;
    temp_r11 = (*(void **)((s8 *)(arg0) + (0x80)));
    (*(f32 *)((s8 *)(arg0) + (0x5C))) = (f32) lbl_803145D0.y;
    temp_r0 = (*(s16 *)((s8 *)(arg0) + (0x9C)));
    (*(f32 *)((s8 *)(arg0) + (0x6C))) = (f32) lbl_803145D0.z;
    (*(f32 *)((s8 *)(arg0) + (0x7C))) = (f32) lbl_803145D0.d;
    (*(f32 *)((s8 *)(arg0) + (0))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0)));
    (*(f32 *)((s8 *)(arg0) + (0x10))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (4)));
    (*(f32 *)((s8 *)(arg0) + (0x20))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (8)));
    (*(f32 *)((s8 *)(arg0) + (0x30))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0xC)));
    (*(f32 *)((s8 *)(arg0) + (4))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x10)));
    (*(f32 *)((s8 *)(arg0) + (0x14))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x14)));
    (*(f32 *)((s8 *)(arg0) + (0x24))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x18)));
    (*(f32 *)((s8 *)(arg0) + (0x34))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x1C)));
    (*(f32 *)((s8 *)(arg0) + (8))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x20)));
    (*(f32 *)((s8 *)(arg0) + (0x18))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x24)));
    (*(f32 *)((s8 *)(arg0) + (0x28))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x28)));
    (*(f32 *)((s8 *)(arg0) + (0x38))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x2C)));
    if (temp_r0 > 3) {
        (*(f32 *)((s8 *)(arg0) + (0xC))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x30)));
        (*(f32 *)((s8 *)(arg0) + (0x1C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x34)));
        (*(f32 *)((s8 *)(arg0) + (0x2C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x38)));
        (*(f32 *)((s8 *)(arg0) + (0x3C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x3C)));
        if (temp_r0 > 4) {
            (*(f32 *)((s8 *)(arg0) + (0x40))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x40)));
            (*(f32 *)((s8 *)(arg0) + (0x50))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x44)));
            (*(f32 *)((s8 *)(arg0) + (0x60))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x48)));
            (*(f32 *)((s8 *)(arg0) + (0x70))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x4C)));
            if (temp_r0 > 5) {
                (*(f32 *)((s8 *)(arg0) + (0x44))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x50)));
                (*(f32 *)((s8 *)(arg0) + (0x54))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x54)));
                (*(f32 *)((s8 *)(arg0) + (0x64))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x58)));
                (*(f32 *)((s8 *)(arg0) + (0x74))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x5C)));
                if (temp_r0 > 6) {
                    (*(f32 *)((s8 *)(arg0) + (0x48))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x60)));
                    (*(f32 *)((s8 *)(arg0) + (0x58))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x64)));
                    (*(f32 *)((s8 *)(arg0) + (0x68))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x68)));
                    (*(f32 *)((s8 *)(arg0) + (0x78))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x6C)));
                    if (temp_r0 > 7) {
                        (*(f32 *)((s8 *)(arg0) + (0x4C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x70)));
                        (*(f32 *)((s8 *)(arg0) + (0x5C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x74)));
                        (*(f32 *)((s8 *)(arg0) + (0x6C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x78)));
                        (*(f32 *)((s8 *)(arg0) + (0x7C))) = (f32) (*(f32 *)((s8 *)(temp_r11) + (0x7C)));
                    }
                }
            }
        }
    }
}
