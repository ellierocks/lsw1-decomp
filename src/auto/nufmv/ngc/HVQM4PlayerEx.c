/*
 * auto/nufmv/ngc/HVQM4PlayerEx.c (0x80049AFC..0x80049DC0)
 *
 * Built with SN ProDG 3.9.3 GCC as part of nu2api (the OSPanic path is
 * "../nu2api.legogc/nufmv/ngc/ngc_obj/../HVQM4PlayerEx.c"); the prologues save
 * GPRs with stmw before storing LR, the GCC order, unlike the MWCC-built
 * HVQM4 decoder library at 0x80187xxx.
 *
 * Provenance: the player layout, field names and decv_init logic follow
 * ShulkMaster/mk-da (https://github.com/ShulkMaster/mk-da), commit
 * 3d6f17751309eb5e7cb5b6162c21360cbb5a35bf, CC0-1.0,
 * src/hvqm4/HVQM4PlayerEx.c and include/hvqm4player.h/hvqm4buf.h. Offsets
 * were checked against GL5E4F assembly; donor Matching status is not LSW1
 * verification. The first four functions are Nu2 movie glue that the split
 * places in this unit.
 */
#include "types.h"

typedef struct HVQM4BufvNode {
    u8 unk0[0x10];
    void* data;
} HVQM4BufvNode;

typedef struct HVQM4Bufv HVQM4Bufv;

typedef struct SeqObj {
    void* ws;
    u16 frame_width;
    u16 frame_height;
    u8 h_samp;
    u8 v_samp;
    u8 unkA[2];
} SeqObj;

typedef struct MovieFrame {
    s32 valid;
    s32 ready;
    u32 timestamp;
    void* pixels;
    HVQM4BufvNode* node;
} MovieFrame;

typedef struct HVQM4PlayerEx {
    u8 unk0[0x48];
    SeqObj decoder;
    s32 width;
    s32 height;
    s32 bufferCount;
    void* work;
    HVQM4Bufv* video;
    u8 unk68[0x3E4 - 0x68];
    u32 frameSize;
    MovieFrame* previous;
    MovieFrame* current;
    MovieFrame* next;
    MovieFrame frames[3];
} HVQM4PlayerEx;

typedef struct GXTexObj {
    u32 dummy[8];
} GXTexObj;

extern void* memset();
extern void GXInitTexObj(GXTexObj* obj, void* image, u16 width, u16 height, s32 format, s32 wrapS,
                         s32 wrapT, u8 mipmap);
extern void GXInitTexObjLOD(GXTexObj* obj, s32 minFilt, s32 magFilt, f32 minLod, f32 maxLod,
                            f32 lodBias, u8 biasClamp, u8 edgeLod, s32 maxAniso);
extern void GXLoadTexObj(GXTexObj* obj, s32 id);
extern void VIWaitForRetrace(void);
extern void OSPanic(const char* file, s32 line, const char* msg, ...);
extern s32 HVQM4PlayerExControlExit(HVQM4PlayerEx* player);
extern HVQM4Bufv* fn_8004B2B0(u32 size, s32 count);
extern HVQM4BufvNode* HVQM4BufvGetFree(HVQM4Bufv* bufv);
extern void fn_80048998(void);

extern GXTexObj lbl_802B1240;
extern s32 lbl_8040834C;
extern s32 lbl_80408350;
extern s32 lbl_8040A534;
extern void* lbl_8040A544;
extern s32 lbl_8040A548;
extern u16 lbl_8040A54E;
extern u16 lbl_8040A552;
extern HVQM4PlayerEx* lbl_8040A554;
extern s32 lbl_8040A558;

void fn_80049AFC(HVQM4PlayerEx* player, s32 arg1, s32 arg2)
{
    lbl_80408350 = arg2;
}

void fn_80049B04(void)
{
    if (lbl_8040A548 != 0) {
        GXInitTexObj(&lbl_802B1240, lbl_8040A544, lbl_8040A54E, lbl_8040A552, 6, 0, 0, 0);
    } else {
        memset(lbl_8040A544, 0, 0x400);
        GXInitTexObj(&lbl_802B1240, lbl_8040A544, 0x10, 0x10, 6, 0, 0, 0);
    }
    GXInitTexObjLOD(&lbl_802B1240, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&lbl_802B1240, 0);
}

void fn_80049BD8(void)
{
    lbl_8040A534 = 0;
}

void fn_80049BE4(void)
{
    s32 i;

    lbl_8040A558 = 1;
    if (lbl_8040834C == 1) {
        lbl_8040834C = 0;
        return;
    }
    if (lbl_8040834C == 2) {
        HVQM4PlayerExControlExit(lbl_8040A554);
    }
    while (lbl_8040834C != 0) {
        for (i = 10; i != 0; i--) {
            fn_80048998();
        }
        VIWaitForRetrace();
    }
}

#include "numath/VuMath.h"

s32 fn_80049C5C(HVQM4PlayerEx* player, s32* error)
{
    SeqObj* decoder = &player->decoder;
    u32 horizontalSampling = decoder->h_samp;
    u32 width = decoder->frame_width;
    u32 height = decoder->frame_height;
    u32 verticalSampling = decoder->v_samp;
    u32 sampling = horizontalSampling * verticalSampling;
    u32 pixels = width * height;

    player->frameSize = pixels * (sampling + 2) / sampling;
    player->video = fn_8004B2B0(player->frameSize, player->bufferCount);
    if (player->video == NULL) {
        *error = 9;
        return 0;
    }
    if ((player->frames[0].node = HVQM4BufvGetFree(player->video)) == NULL) {
        OSPanic("../nu2api.legogc/nufmv/ngc/ngc_obj/../HVQM4PlayerEx.c", 244,
                "decv_open(), buff[0].bufv set error !\n");
        *error = 9;
        return 0;
    }
    if ((player->frames[1].node = HVQM4BufvGetFree(player->video)) == NULL) {
        OSPanic("../nu2api.legogc/nufmv/ngc/ngc_obj/../HVQM4PlayerEx.c", 249,
                "decv_open(), buff[1].bufv set error !\n");
        *error = 9;
        return 0;
    }
    if ((player->frames[2].node = HVQM4BufvGetFree(player->video)) == NULL) {
        OSPanic("../nu2api.legogc/nufmv/ngc/ngc_obj/../HVQM4PlayerEx.c", 254,
                "decv_open(), buff[2].bufv set error !\n");
        *error = 9;
        return 0;
    }
    player->frames[0].valid = 0;
    player->frames[0].ready = -1;
    player->frames[1].valid = 0;
    player->frames[1].ready = -1;
    player->frames[2].valid = 0;
    player->frames[2].ready = -1;
    player->frames[0].pixels = player->frames[0].node->data;
    player->frames[1].pixels = player->frames[1].node->data;
    player->frames[2].pixels = player->frames[2].node->data;
    player->previous = &player->frames[0];
    player->current = &player->frames[1];
    player->next = &player->frames[2];
    return 1;
}
