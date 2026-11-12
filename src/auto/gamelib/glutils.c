#include "nu3dx/nu3dx.h"

extern s32 fn_80024DC8(char* name, u32* buffer, u32* end);
extern struct numtl_s* fn_80024C18(s32 count);
extern void fn_80024CD4(struct numtl_s* material);

struct numtl_s* fn_80040A88(u32* buffer, u32* end, char* name, s32 disableDepthWrite,
                           u8 alphaMode, s16 sortPriority, u8 depthMode)
{
    u32 bufferEnd;
    s32 texture;
    struct numtl_s* material;

    bufferEnd = *end;
    texture = fn_80024DC8(name, buffer, &bufferEnd);
    *buffer = (*buffer + 15) & ~15;
    material = fn_80024C18(1);
    material->unknownD0[2] = material->unknownD0[1] = material->unknownD0[0] = 1.0f;
    material->unknownBD = 2;
    material->unknownBE = depthMode;
    material->unknownF0 = 0.999f;
    material->unknownB8 = alphaMode;
    if (disableDepthWrite != 0) {
        material->unknownC3[2] = 1;
    }
    material->tid = texture;
    *(s16*)material->unknownF8 = sortPriority;
    fn_80024CD4(material);
    return material;
}

#include "numath/VuMath.h"

/* Unreferenced pools from stripped functions and subsequent TUs in this split. */
static const char unk_801936F8[] = "../nu2api.legogc/gamelib/ngc_obj/../listman.c";
static const char unk_80193728[] = "../nu2api.legogc/numath/ngc/VuMath.h";
static const char unk_80193750[] = "VuMtxInitRot NOT fully implemented!";
static const char unk_80193774[] = "../nu2api.legogc/gamelib/ngc/ngc_obj/../specterr.c";
static const char unk_801937A8[] = "CreateSpecialTerrainInstanceArray: invalid instance ix in special terrain object";
static const char unk_801937FC[] = "SpecialTerrainInstanceCollideCheck: bad special terrain type <%d>";
static const u32 unk_80193840[3] = { 0, 0x3F800000, 0x3F000000 };
static const char unk_8019384C[] = "../nu2api.legogc/numath/ngc/VuMath.h";
static const char unk_80193874[] = "VuMtxInitRot NOT fully implemented!";
