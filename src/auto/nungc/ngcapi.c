#include "types.h"

extern void* memset();

extern void* memset(void* p, s32 c, u32 n);

extern s32 lbl_804080F8;

extern u8 lbl_80340588[];

void fn_80037C1C(void)
{
}

void fn_80037DA8(u8* p)
{
    *(void**)(p + 0x1cc) = lbl_80340588;
}

void fn_800385A4(void)
{
    if (lbl_804080F8 == 0) {
        lbl_804080F8 = 4;
    }
}

s32 fn_800385BC(void)
{
    if (lbl_804080F8 == 0) {
        return 0;
    }
    return 1;
}

void fn_80037DB8(void)
{
    memset(lbl_80340588, 0xff, 0x271);
}

void fn_80037DE8(void)
{
    memset(lbl_80340588, 0, 0x271);
}

s32 OSGetResetButtonState();
extern s32 lbl_804080F0;
extern s32 lbl_804080F8;

void fn_800385D4(void) {
    s32 temp_r3;

    if ((s32) lbl_804080F8 == 0) {
        temp_r3 = OSGetResetButtonState();
        if ((temp_r3 == 0) && ((s32) lbl_804080F0 == 1) && ((s32) lbl_804080F8 == 0)) {
            lbl_804080F8 = lbl_804080F0;
        }
        lbl_804080F0 = temp_r3;
    }
}

void AXFXSetHooks(s32, s32, f32);
void NuRndrFootPrints(s32);
void fn_80025C38(s32, s32, s32, s32, s32, s32);
void fn_800274BC(s32);
void fn_80027564();
extern f32 lbl_80192B78[];
extern s32 lbl_8040A420;

void fn_80037FEC(void) {
    s32 temp_r30;

    temp_r30 = lbl_8040A420;
    fn_800274BC(0);
    AXFXSetHooks(0x100, 0, lbl_80192B78[0]);
    fn_80025C38(0x400, 0x64C, 0x2000, 0x10C, -1, temp_r30);
    fn_80027564();
    NuRndrFootPrints(0);
}

typedef struct nuroom_s {
    s16* instanceIndices;
    u8 unknown4[8];
    s16 instanceCount;
} nuroom_s;

void fn_80037D3C(nuroom_s* room)
{
    s32 i;
    s16 instance;

    for (i = 0; i < room->instanceCount; i++) {
        instance = room->instanceIndices[i];
        lbl_80340588[instance >> 3] |= 1 << (s16)(instance % 8);
    }
}

typedef struct nuportalscene_s {
    u8 unknown0[0x188];
    u8* spheres;
    u8* boxes;
    u8 unknown190[0x10];
    struct nuportaldisplay_s* display;
} nuportalscene_s;

typedef struct nuportaldisplay_s {
    u8 unknown0[0x88];
    u8* overrides;
} nuportaldisplay_s;

typedef struct nufrustum_s {
    u8 unknown0[0x80];
    void* planes;
    u8 unknown84[0x18];
    s16 planeCount;
} nufrustum_s;

extern s32 clipTestSphere(void* sphere, nufrustum_s* frustum);
extern s32 clipTestBox(void* first, void* second, void* planes, s32 count);

void clipRoomAgainstFrustrum(nuportalscene_s* scene, nuroom_s* room, nufrustum_s* frustum)
{
    /* Retail reserves this unused local area. */
    volatile u8 scratch[0x30];
    u32 i;
    u32 instance;
    s32 result;

    for (i = 0; i < (u32)room->instanceCount; i++) {
        instance = room->instanceIndices[i];
        if ((scene->display->overrides[instance >> 3] >> (instance & 7)) & 1) {
            lbl_80340588[instance >> 3] |= 1 << (instance & 7);
        } else if (!((lbl_80340588[instance >> 3] >> (instance & 7)) & 1)) {
            result = clipTestSphere(scene->spheres + instance * 16, frustum);
            switch (result) {
            case 0:
                break;
            case 1:
                lbl_80340588[instance >> 3] |= 1 << (instance & 7);
                break;
            case 2:
                if (clipTestBox(scene->boxes + instance * 32, scene->boxes + instance * 32 + 16,
                                frustum->planes, frustum->planeCount) != 0) {
                    lbl_80340588[instance >> 3] |= 1 << (instance & 7);
                }
                break;
            }
        }
    }
}
