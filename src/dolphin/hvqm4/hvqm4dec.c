/*
 * HVQM4 1.5 video/sound decoder (Hudson Soft SDK library, linked into LSW1).
 *
 * Provenance
 *  - Function bodies were matched against the retail GL5E4F code with Nintendo
 *    CodeWarrior GC/1.2.5 and the dolsdk2004 release flags (-O4,p -inline auto).
 *  - Starting points for several routines (setHVQPlaneDesc, HVQM4BuffSize,
 *    HVQM4SetBuffer, WeightImBlock, the half-pel motion helpers, IpicBlockDec,
 *    IpicLineDec and the plane/border bookkeeping) were adapted from the CC0
 *    mssb-decomp HVQM4/hvqm4dec.c (https://github.com/nlopez99/mssb-decomp,
 *    PR #27), which targets the 1.3 library. LSW1 ships the 1.5 layout, so
 *    the structs differ (32-bit word bit reader, 0x1008-byte Huffman trees,
 *    0x6CD8-byte VideoState).
 *  - Bodies, inline helpers and local types also adapt projectPiki/pikmin
 *    (https://github.com/projectPiki/pikmin), commit
 *    9a2163626a269ae7c699fc195cf91a5ca791044d, CC0-1.0,
 *    src/hvqm4dec/hvqm4dec.c and include/hvqm4.h. GL5E4F accesses retain
 *    the LSW1 types; donor Matching status is not LSW1 verification.
 *  - Algorithms and field meanings follow Tilka/hvqm4 (LGPLv2+,
 *    https://github.com/Tilka/hvqm4); no code from it is reproduced verbatim,
 *    only the documented bitstream and layout facts.
 */
#include <dolphin/types.h>
#include <dolphin/os/OSAlloc.h>

typedef struct HVQTree {
    /* 0x000 */ s32 pos;
    /* 0x004 */ s32 root;
    /* 0x008 */ s32 array[2][0x200];
} HVQTree; // size: 0x1008

typedef struct HVQBitBuffer {
    /* 0x0 */ u32* ptr;
    /* 0x4 */ u32 size;
    /* 0x8 */ u32 value;
    /* 0xC */ s32 bit;
} HVQBitBuffer; // size: 0x10

typedef struct HVQBitTree {
    /* 0x00 */ HVQBitBuffer buf;
    /* 0x10 */ HVQTree* tree;
} HVQBitTree; // size: 0x14

typedef struct HVQBlock {
    /* 0x0 */ u8 value;
    /* 0x1 */ u8 type;
} HVQBlock;

typedef struct HVQPlane {
    /* 0x00 */ HVQBlock* border;
    /* 0x04 */ HVQBlock* payload;
    /* 0x08 */ u16 h_blocks;
    /* 0x0A */ u16 v_blocks;
    /* 0x0C */ u16 h_blocks_safe;
    /* 0x0E */ u16 v_blocks_safe;
    /* 0x10 */ u16 mcb_offset[4];
    /* 0x18 */ s32 pb_offset[4];
    /* 0x28 */ u16 width;
    /* 0x2A */ u16 height;
    /* 0x2C */ s32 size;
    /* 0x30 */ u8 width_shift;
    /* 0x31 */ u8 height_shift;
    /* 0x32 */ u8 pb_per_mcb_x;
    /* 0x33 */ u8 pb_per_mcb_y;
    /* 0x34 */ u8 blocks_per_mcb;
    /* 0x35 */ u8 pad[3];
} HVQPlane; // size: 0x38

typedef struct HVQMCPlane {
    /* 0x00 */ s32 rle;
    /* 0x04 */ s32 pb_dc;
    /* 0x08 */ HVQBlock* payload_cur_blk;
    /* 0x0C */ HVQBlock* payload_cur_row;
    /* 0x10 */ s32 present;
    /* 0x14 */ u8* top;
    /* 0x18 */ u8* target;
    /* 0x1C */ s32 past;
    /* 0x20 */ s32 future;
    /* 0x24 */ u16 h_mcb_stride;
    /* 0x28 */ s32 v_mcb_stride;
    /* 0x2C */ s32 pb_per_mcb_x;
    /* 0x30 */ s32 stride;
} HVQMCPlane; // size: 0x34

typedef struct HVQState {
    /* 0x0000 */ HVQPlane planes[3];
    /* 0x00A8 */ HVQTree trees[6];
    /* 0x60D8 */ HVQBitTree dc_values[3];
    /* 0x6114 */ HVQBitTree dc_rle[3];
    /* 0x6150 */ HVQBitTree bufTree0[3];
    /* 0x618C */ HVQBitTree basis_num[2];
    /* 0x61B4 */ HVQBitTree basis_num_run[2];
    /* 0x61DC */ HVQBitBuffer fixvl[3];
    /* 0x620C */ HVQBitTree mv_h;
    /* 0x6220 */ HVQBitTree mv_v;
    /* 0x6234 */ HVQBitTree mcb_proc;
    /* 0x6248 */ HVQBitTree mcb_type;
    /* 0x625C */ u16 nest_stride;
    /* 0x625E */ u16 nest_height;
    /* 0x6260 */ u8 is_landscape;
    /* 0x6261 */ u8 nest_data[70 * 38];
    /* 0x6CC5 */ u8 pad[3];
    /* 0x6CC8 */ s32 dc_max;
    /* 0x6CCC */ s32 dc_min;
    /* 0x6CD0 */ u8 unk_shift;
    /* 0x6CD1 */ u8 dc_shift;
    /* 0x6CD2 */ u8 mc_bits_h[2];
    /* 0x6CD4 */ u8 mc_bits_v[2];
    /* 0x6CD6 */ u8 pad2[2];
} HVQState; // size: 0x6CD8

typedef struct HVQSeqObj {
    /* 0x0 */ HVQState* state;
    /* 0x4 */ u16 width;
    /* 0x6 */ u16 height;
    /* 0x8 */ u8 h_samp;
    /* 0x9 */ u8 v_samp;
} HVQSeqObj;

typedef struct HVQVideoInfo {
    /* 0x0 */ u16 width;
    /* 0x2 */ u16 height;
    /* 0x4 */ u8 h_samp;
    /* 0x5 */ u8 v_samp;
} HVQVideoInfo;

typedef struct HVQStack {
    /* 0x00 */ s32 plane_idx;
    /* 0x04 */ HVQBlock* line_prev;
    /* 0x08 */ HVQBlock* line_curr;
    /* 0x0C */ HVQBlock* line_next;
    /* 0x10 */ HVQBlock next;
    /* 0x12 */ HVQBlock curr;
    /* 0x14 */ u8 value_prev;
} HVQStack;

typedef struct HVQSound {
    /* 0x0 */ u8 type;
    /* 0x1 */ u8 pad;
    /* 0x2 */ u8 format;
    /* 0x3 */ u8 pad2[5];
    /* 0x8 */ u8 data[0x18];
} HVQSound; // size: 0x20

typedef struct HVQSoundState {
    s32 pred0;
    s32 idx0;
    s32 pred1;
    s32 idx1;
} HVQSoundState;

/* .data */
char HVQM4Version[] = "HVQM4 1.5";

/* [89][8] s32 deltas, then [89][8] u8 next-index */
static u32 adpcmTable[0x37A] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000004, 0x00000007, 0x00000008, 0x0000000A, 0x0000000B,
    0x00000001, 0x00000003, 0x00000005, 0x00000007, 0x00000009, 0x0000000B, 0x0000000D, 0x0000000F,
    0x00000001, 0x00000003, 0x00000005, 0x00000007, 0x0000000A, 0x0000000C, 0x0000000E, 0x00000010,
    0x00000001, 0x00000003, 0x00000006, 0x00000008, 0x0000000B, 0x0000000D, 0x00000010, 0x00000012,
    0x00000001, 0x00000003, 0x00000006, 0x00000008, 0x0000000C, 0x0000000E, 0x00000011, 0x00000013,
    0x00000001, 0x00000004, 0x00000007, 0x0000000A, 0x0000000D, 0x00000010, 0x00000013, 0x00000016,
    0x00000001, 0x00000004, 0x00000007, 0x0000000A, 0x0000000E, 0x00000011, 0x00000014, 0x00000017,
    0x00000001, 0x00000004, 0x00000008, 0x0000000B, 0x0000000F, 0x00000012, 0x00000016, 0x00000019,
    0x00000002, 0x00000006, 0x0000000A, 0x0000000E, 0x00000012, 0x00000016, 0x0000001A, 0x0000001E,
    0x00000002, 0x00000006, 0x0000000A, 0x0000000E, 0x00000013, 0x00000017, 0x0000001B, 0x0000001F,
    0x00000002, 0x00000006, 0x0000000B, 0x0000000F, 0x00000015, 0x00000019, 0x0000001E, 0x00000022,
    0x00000002, 0x00000007, 0x0000000C, 0x00000011, 0x00000017, 0x0000001C, 0x00000021, 0x00000026,
    0x00000002, 0x00000007, 0x0000000D, 0x00000012, 0x00000019, 0x0000001E, 0x00000024, 0x00000029,
    0x00000003, 0x00000009, 0x0000000F, 0x00000015, 0x0000001C, 0x00000022, 0x00000028, 0x0000002E,
    0x00000003, 0x0000000A, 0x00000011, 0x00000018, 0x0000001F, 0x00000026, 0x0000002D, 0x00000034,
    0x00000003, 0x0000000A, 0x00000012, 0x00000019, 0x00000022, 0x00000029, 0x00000031, 0x00000038,
    0x00000004, 0x0000000C, 0x00000015, 0x0000001D, 0x00000026, 0x0000002E, 0x00000037, 0x0000003F,
    0x00000004, 0x0000000D, 0x00000016, 0x0000001F, 0x00000029, 0x00000032, 0x0000003B, 0x00000044,
    0x00000005, 0x0000000F, 0x00000019, 0x00000023, 0x0000002E, 0x00000038, 0x00000042, 0x0000004C,
    0x00000005, 0x00000010, 0x0000001B, 0x00000026, 0x00000032, 0x0000003D, 0x00000048, 0x00000053,
    0x00000006, 0x00000012, 0x0000001F, 0x0000002B, 0x00000038, 0x00000044, 0x00000051, 0x0000005D,
    0x00000006, 0x00000013, 0x00000021, 0x0000002E, 0x0000003D, 0x0000004A, 0x00000058, 0x00000065,
    0x00000007, 0x00000016, 0x00000025, 0x00000034, 0x00000043, 0x00000052, 0x00000061, 0x00000070,
    0x00000008, 0x00000018, 0x00000029, 0x00000039, 0x0000004A, 0x0000005A, 0x0000006B, 0x0000007B,
    0x00000009, 0x0000001B, 0x0000002D, 0x0000003F, 0x00000052, 0x00000064, 0x00000076, 0x00000088,
    0x0000000A, 0x0000001E, 0x00000032, 0x00000046, 0x0000005A, 0x0000006E, 0x00000082, 0x00000096,
    0x0000000B, 0x00000021, 0x00000037, 0x0000004D, 0x00000063, 0x00000079, 0x0000008F, 0x000000A5,
    0x0000000C, 0x00000024, 0x0000003C, 0x00000054, 0x0000006D, 0x00000085, 0x0000009D, 0x000000B5,
    0x0000000D, 0x00000027, 0x00000042, 0x0000005C, 0x00000078, 0x00000092, 0x000000AD, 0x000000C7,
    0x0000000E, 0x0000002B, 0x00000049, 0x00000066, 0x00000084, 0x000000A1, 0x000000BF, 0x000000DC,
    0x00000010, 0x00000030, 0x00000051, 0x00000071, 0x00000092, 0x000000B2, 0x000000D3, 0x000000F3,
    0x00000011, 0x00000034, 0x00000058, 0x0000007B, 0x000000A0, 0x000000C3, 0x000000E7, 0x0000010A,
    0x00000013, 0x0000003A, 0x00000061, 0x00000088, 0x000000B0, 0x000000D7, 0x000000FE, 0x00000125,
    0x00000015, 0x00000040, 0x0000006B, 0x00000096, 0x000000C2, 0x000000ED, 0x00000118, 0x00000143,
    0x00000017, 0x00000046, 0x00000076, 0x000000A5, 0x000000D5, 0x00000104, 0x00000134, 0x00000163,
    0x0000001A, 0x0000004E, 0x00000082, 0x000000B6, 0x000000EB, 0x0000011F, 0x00000153, 0x00000187,
    0x0000001C, 0x00000055, 0x0000008F, 0x000000C8, 0x00000102, 0x0000013B, 0x00000175, 0x000001AE,
    0x0000001F, 0x0000005E, 0x0000009D, 0x000000DC, 0x0000011C, 0x0000015B, 0x0000019A, 0x000001D9,
    0x00000022, 0x00000067, 0x000000AD, 0x000000F2, 0x00000139, 0x0000017E, 0x000001C4, 0x00000209,
    0x00000026, 0x00000072, 0x000000BF, 0x0000010B, 0x00000159, 0x000001A5, 0x000001F2, 0x0000023E,
    0x0000002A, 0x0000007E, 0x000000D2, 0x00000126, 0x0000017B, 0x000001CF, 0x00000223, 0x00000277,
    0x0000002E, 0x0000008A, 0x000000E7, 0x00000143, 0x000001A1, 0x000001FD, 0x0000025A, 0x000002B6,
    0x00000033, 0x00000099, 0x000000FF, 0x00000165, 0x000001CB, 0x00000231, 0x00000297, 0x000002FD,
    0x00000038, 0x000000A8, 0x00000118, 0x00000188, 0x000001F9, 0x00000269, 0x000002D9, 0x00000349,
    0x0000003D, 0x000000B8, 0x00000134, 0x000001AF, 0x0000022B, 0x000002A6, 0x00000322, 0x0000039D,
    0x00000044, 0x000000CC, 0x00000154, 0x000001DC, 0x00000264, 0x000002EC, 0x00000374, 0x000003FC,
    0x0000004A, 0x000000DF, 0x00000175, 0x0000020A, 0x000002A0, 0x00000335, 0x000003CB, 0x00000460,
    0x00000052, 0x000000F6, 0x0000019B, 0x0000023F, 0x000002E4, 0x00000388, 0x0000042D, 0x000004D1,
    0x0000005A, 0x0000010F, 0x000001C4, 0x00000279, 0x0000032E, 0x000003E3, 0x00000498, 0x0000054D,
    0x00000063, 0x0000012A, 0x000001F1, 0x000002B8, 0x0000037F, 0x00000446, 0x0000050D, 0x000005D4,
    0x0000006D, 0x00000148, 0x00000223, 0x000002FE, 0x000003D9, 0x000004B4, 0x0000058F, 0x0000066A,
    0x00000078, 0x00000168, 0x00000259, 0x00000349, 0x0000043B, 0x0000052B, 0x0000061C, 0x0000070C,
    0x00000084, 0x0000018D, 0x00000296, 0x0000039F, 0x000004A8, 0x000005B1, 0x000006BA, 0x000007C3,
    0x00000091, 0x000001B4, 0x000002D8, 0x000003FB, 0x0000051F, 0x00000642, 0x00000766, 0x00000889,
    0x000000A0, 0x000001E0, 0x00000321, 0x00000461, 0x000005A2, 0x000006E2, 0x00000823, 0x00000963,
    0x000000B0, 0x00000210, 0x00000371, 0x000004D1, 0x00000633, 0x00000793, 0x000008F4, 0x00000A54,
    0x000000C2, 0x00000246, 0x000003CA, 0x0000054E, 0x000006D2, 0x00000856, 0x000009DA, 0x00000B5E,
    0x000000D5, 0x0000027F, 0x0000042A, 0x000005D4, 0x00000780, 0x0000092A, 0x00000AD5, 0x00000C7F,
    0x000000EA, 0x000002BF, 0x00000495, 0x0000066A, 0x00000840, 0x00000A15, 0x00000BEB, 0x00000DC0,
    0x00000102, 0x00000306, 0x0000050B, 0x0000070F, 0x00000914, 0x00000B18, 0x00000D1D, 0x00000F21,
    0x0000011C, 0x00000354, 0x0000058C, 0x000007C4, 0x000009FC, 0x00000C34, 0x00000E6C, 0x000010A4,
    0x00000138, 0x000003A8, 0x00000619, 0x00000889, 0x00000AFB, 0x00000D6B, 0x00000FDC, 0x0000124C,
    0x00000157, 0x00000406, 0x000006B5, 0x00000964, 0x00000C14, 0x00000EC3, 0x00001172, 0x00001421,
    0x0000017A, 0x0000046E, 0x00000762, 0x00000A56, 0x00000D4A, 0x0000103E, 0x00001332, 0x00001626,
    0x0000019F, 0x000004DE, 0x0000081E, 0x00000B5D, 0x00000E9E, 0x000011DD, 0x0000151D, 0x0000185C,
    0x000001C9, 0x0000055C, 0x000008EF, 0x00000C82, 0x00001015, 0x000013A8, 0x0000173B, 0x00001ACE,
    0x000001F7, 0x000005E5, 0x000009D4, 0x00000DC2, 0x000011B1, 0x0000159F, 0x0000198E, 0x00001D7C,
    0x00000229, 0x0000067C, 0x00000ACF, 0x00000F22, 0x00001375, 0x000017C8, 0x00001C1B, 0x0000206E,
    0x00000260, 0x00000721, 0x00000BE3, 0x000010A4, 0x00001567, 0x00001A28, 0x00001EEA, 0x000023AB,
    0x0000029D, 0x000007D8, 0x00000D14, 0x0000124F, 0x0000178B, 0x00001CC6, 0x00002202, 0x0000273D,
    0x000002E0, 0x000008A1, 0x00000E63, 0x00001424, 0x000019E6, 0x00001FA7, 0x00002569, 0x00002B2A,
    0x0000032A, 0x0000097F, 0x00000FD4, 0x00001629, 0x00001C7E, 0x000022D3, 0x00002928, 0x00002F7D,
    0x0000037B, 0x00000A72, 0x00001169, 0x00001860, 0x00001F57, 0x0000264E, 0x00002D45, 0x0000343C,
    0x000003D4, 0x00000B7D, 0x00001326, 0x00001ACF, 0x00002279, 0x00002A22, 0x000031CB, 0x00003974,
    0x00000436, 0x00000CA3, 0x00001511, 0x00001D7E, 0x000025EC, 0x00002E59, 0x000036C7, 0x00003F34,
    0x000004A2, 0x00000DE7, 0x0000172C, 0x00002071, 0x000029B7, 0x000032FC, 0x00003C41, 0x00004586,
    0x00000519, 0x00000F4B, 0x0000197E, 0x000023B0, 0x00002DE3, 0x00003815, 0x00004248, 0x00004C7A,
    0x0000059B, 0x000010D2, 0x00001C0A, 0x00002741, 0x0000327A, 0x00003DB1, 0x000048E9, 0x00005420,
    0x0000062B, 0x00001281, 0x00001ED8, 0x00002B2E, 0x00003786, 0x000043DC, 0x00005033, 0x00005C89,
    0x000006C9, 0x0000145B, 0x000021EE, 0x00002F80, 0x00003D14, 0x00004AA6, 0x00005839, 0x000065CB,
    0x00000777, 0x00001665, 0x00002553, 0x00003441, 0x00004330, 0x0000521E, 0x0000610C, 0x00006FFA,
    0x00000836, 0x000018A2, 0x0000290F, 0x0000397B, 0x000049E8, 0x00005A54, 0x00006AC1, 0x00007B2D,
    0x00000908, 0x00001B19, 0x00002D2A, 0x00003F3B, 0x0000514C, 0x0000635D, 0x0000756E, 0x0000877F,
    0x000009EF, 0x00001DCE, 0x000031AE, 0x0000458D, 0x0000596D, 0x00006D4C, 0x0000812C, 0x0000950B,
    0x00000AEE, 0x000020CA, 0x000036A6, 0x00004C82, 0x0000625F, 0x0000783B, 0x00008E17, 0x0000A3F3,
    0x00000C05, 0x00002410, 0x00003C1C, 0x00005427, 0x00006C34, 0x0000843F, 0x00009C4B, 0x0000B456,
    0x00000D39, 0x000027AC, 0x00004220, 0x00005C93, 0x00007707, 0x0000917A, 0x0000ABEE, 0x0000C661,
    0x00000E8C, 0x00002BA4, 0x000048BD, 0x000065D5, 0x000082EE, 0x0000A006, 0x0000BD1F, 0x0000DA37,
    0x00000FFF, 0x00002FFE, 0x00004FFE, 0x00006FFD, 0x00008FFE, 0x0000AFFD, 0x0000CFFD, 0x0000EFFC,
    0x00000000, 0x02040608, 0x00000000, 0x03050709, 0x01010101, 0x0406080A, 0x02020202, 0x0507090B,
    0x03030303, 0x06080A0C, 0x04040404, 0x07090B0D, 0x05050505, 0x080A0C0E, 0x06060606, 0x090B0D0F,
    0x07070707, 0x0A0C0E10, 0x08080808, 0x0B0D0F11, 0x09090909, 0x0C0E1012, 0x0A0A0A0A, 0x0D0F1113,
    0x0B0B0B0B, 0x0E101214, 0x0C0C0C0C, 0x0F111315, 0x0D0D0D0D, 0x10121416, 0x0E0E0E0E, 0x11131517,
    0x0F0F0F0F, 0x12141618, 0x10101010, 0x13151719, 0x11111111, 0x1416181A, 0x12121212, 0x1517191B,
    0x13131313, 0x16181A1C, 0x14141414, 0x17191B1D, 0x15151515, 0x181A1C1E, 0x16161616, 0x191B1D1F,
    0x17171717, 0x1A1C1E20, 0x18181818, 0x1B1D1F21, 0x19191919, 0x1C1E2022, 0x1A1A1A1A, 0x1D1F2123,
    0x1B1B1B1B, 0x1E202224, 0x1C1C1C1C, 0x1F212325, 0x1D1D1D1D, 0x20222426, 0x1E1E1E1E, 0x21232527,
    0x1F1F1F1F, 0x22242628, 0x20202020, 0x23252729, 0x21212121, 0x2426282A, 0x22222222, 0x2527292B,
    0x23232323, 0x26282A2C, 0x24242424, 0x27292B2D, 0x25252525, 0x282A2C2E, 0x26262626, 0x292B2D2F,
    0x27272727, 0x2A2C2E30, 0x28282828, 0x2B2D2F31, 0x29292929, 0x2C2E3032, 0x2A2A2A2A, 0x2D2F3133,
    0x2B2B2B2B, 0x2E303234, 0x2C2C2C2C, 0x2F313335, 0x2D2D2D2D, 0x30323436, 0x2E2E2E2E, 0x31333537,
    0x2F2F2F2F, 0x32343638, 0x30303030, 0x33353739, 0x31313131, 0x3436383A, 0x32323232, 0x3537393B,
    0x33333333, 0x36383A3C, 0x34343434, 0x37393B3D, 0x35353535, 0x383A3C3E, 0x36363636, 0x393B3D3F,
    0x37373737, 0x3A3C3E40, 0x38383838, 0x3B3D3F41, 0x39393939, 0x3C3E4042, 0x3A3A3A3A, 0x3D3F4143,
    0x3B3B3B3B, 0x3E404244, 0x3C3C3C3C, 0x3F414345, 0x3D3D3D3D, 0x40424446, 0x3E3E3E3E, 0x41434547,
    0x3F3F3F3F, 0x42444648, 0x40404040, 0x43454749, 0x41414141, 0x4446484A, 0x42424242, 0x4547494B,
    0x43434343, 0x46484A4C, 0x44444444, 0x47494B4D, 0x45454545, 0x484A4C4E, 0x46464646, 0x494B4D4F,
    0x47474747, 0x4A4C4E50, 0x48484848, 0x4B4D4F51, 0x49494949, 0x4C4E5052, 0x4A4A4A4A, 0x4D4F5153,
    0x4B4B4B4B, 0x4E505254, 0x4C4C4C4C, 0x4F515355, 0x4D4D4D4D, 0x50525456, 0x4E4E4E4E, 0x51535557,
    0x4F4F4F4F, 0x52545658, 0x50505050, 0x53555758, 0x51515151, 0x54565858, 0x52525252, 0x55575858,
    0x53535353, 0x56585858, 0x54545454, 0x57585858, 0x55555555, 0x58585858, 0x56565656, 0x58585858,
    0x57575757, 0x58585858,
};

static s32 adpcmTable4_0[0x100] = {
    0, 4, 8, 12, 16, 20, 24, 29,
    35, 40, 46, 52, 58, 65, 72, 80,
    88, 96, 105, 115, 125, 135, 146, 158,
    170, 183, 197, 211, 227, 243, 260, 278,
    297, 317, 338, 360, 384, 409, 435, 463,
    493, 524, 557, 591, 628, 667, 707, 750,
    796, 844, 895, 948, 1005, 1064, 1127, 1194,
    1264, 1338, 1416, 1499, 1586, 1678, 1776, 1879,
    1987, 2102, 2223, 2350, 2485, 2628, 2778, 2937,
    3105, 3282, 3469, 3666, 3875, 4095, 4327, 4573,
    4832, 5105, 5394, 5699, 6021, 6361, 6720, 7099,
    7500, 7923, 8369, 8840, 9338, 9864, 10419, 11004,
    11623, 12276, 12966, 13694, 14463, 15275, 16132, 17038,
    17994, 19003, 20068, 21194, 22382, 23637, 24961, 26360,
    27837, 29397, 31044, 32783, 34619, 36557, 38604, 40765,
    43048, 45457, 48002, 50689, 53526, 56521, 59684, 63000,
    0, -63000, -59684, -56521, -53526, -50689, -48002, -45457,
    -43048, -40765, -38604, -36557, -34619, -32783, -31044, -29397,
    -27837, -26360, -24961, -23637, -22382, -21194, -20068, -19003,
    -17994, -17038, -16132, -15275, -14463, -13694, -12966, -12276,
    -11623, -11004, -10419, -9864, -9338, -8840, -8369, -7923,
    -7500, -7099, -6720, -6361, -6021, -5699, -5394, -5105,
    -4832, -4573, -4327, -4095, -3875, -3666, -3469, -3282,
    -3105, -2937, -2778, -2628, -2485, -2350, -2223, -2102,
    -1987, -1879, -1776, -1678, -1586, -1499, -1416, -1338,
    -1264, -1194, -1127, -1064, -1005, -948, -895, -844,
    -796, -750, -707, -667, -628, -591, -557, -524,
    -493, -463, -435, -409, -384, -360, -338, -317,
    -297, -278, -260, -243, -227, -211, -197, -183,
    -170, -158, -146, -135, -125, -115, -105, -96,
    -88, -80, -72, -65, -58, -52, -46, -40,
    -35, -29, -24, -20, -16, -12, -8, -4,
};

static s32 adpcmTable4_1[0x100] = {
    0, 2, 4, 6, 8, 10, 12, 15,
    17, 20, 23, 26, 29, 33, 36, 40,
    44, 48, 53, 57, 62, 68, 73, 79,
    85, 92, 98, 106, 114, 122, 130, 139,
    149, 159, 169, 180, 192, 205, 218, 232,
    246, 262, 278, 296, 314, 334, 354, 375,
    398, 422, 448, 474, 502, 532, 564, 597,
    632, 669, 708, 750, 793, 839, 888, 939,
    994, 1051, 1112, 1175, 1243, 1314, 1389, 1469,
    1553, 1641, 1735, 1833, 1937, 2047, 2164, 2287,
    2416, 2553, 2697, 2850, 3011, 3181, 3360, 3550,
    3750, 3961, 4184, 4420, 4669, 4932, 5209, 5502,
    5812, 6138, 6483, 6847, 7231, 7637, 8066, 8519,
    8997, 9502, 10034, 10597, 11191, 11819, 12481, 13180,
    13919, 14698, 15522, 16391, 17309, 18279, 19302, 20383,
    21524, 22729, 24001, 25345, 26763, 28261, 29842, 31500,
    0, -31500, -29842, -28261, -26763, -25345, -24001, -22729,
    -21524, -20383, -19302, -18279, -17309, -16391, -15522, -14698,
    -13919, -13180, -12481, -11819, -11191, -10597, -10034, -9502,
    -8997, -8519, -8066, -7637, -7231, -6847, -6483, -6138,
    -5812, -5502, -5209, -4932, -4669, -4420, -4184, -3961,
    -3750, -3550, -3360, -3181, -3011, -2850, -2697, -2553,
    -2416, -2287, -2164, -2047, -1937, -1833, -1735, -1641,
    -1553, -1469, -1389, -1314, -1243, -1175, -1112, -1051,
    -994, -939, -888, -839, -793, -750, -708, -669,
    -632, -597, -564, -532, -502, -474, -448, -422,
    -398, -375, -354, -334, -314, -296, -278, -262,
    -246, -232, -218, -205, -192, -180, -169, -159,
    -149, -139, -130, -122, -114, -106, -98, -92,
    -85, -79, -73, -68, -62, -57, -53, -48,
    -44, -40, -36, -33, -29, -26, -23, -20,
    -17, -15, -12, -10, -8, -6, -4, -2,
};

static s32 adpcmTable4_2[0x100] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    9, 10, 12, 13, 15, 17, 18, 20,
    22, 24, 27, 29, 31, 34, 37, 39,
    43, 46, 49, 53, 57, 61, 65, 70,
    74, 79, 85, 90, 96, 103, 109, 116,
    123, 131, 139, 148, 157, 167, 177, 188,
    199, 211, 224, 237, 251, 266, 282, 298,
    316, 334, 354, 375, 397, 420, 444, 470,
    497, 526, 556, 588, 621, 657, 695, 734,
    776, 821, 868, 917, 969, 1024, 1082, 1143,
    1208, 1277, 1349, 1425, 1505, 1590, 1680, 1775,
    1875, 1981, 2092, 2210, 2335, 2466, 2605, 2751,
    2906, 3069, 3241, 3423, 3616, 3819, 4033, 4260,
    4499, 4751, 5017, 5298, 5596, 5910, 6241, 6590,
    6959, 7349, 7761, 8196, 8655, 9140, 9651, 10191,
    10762, 11364, 12001, 12673, 13382, 14130, 14921, 15750,
    0, -15750, -14921, -14130, -13382, -12673, -12001, -11364,
    -10762, -10191, -9651, -9140, -8655, -8196, -7761, -7349,
    -6959, -6590, -6241, -5910, -5596, -5298, -5017, -4751,
    -4499, -4260, -4033, -3819, -3616, -3423, -3241, -3069,
    -2906, -2751, -2605, -2466, -2335, -2210, -2092, -1981,
    -1875, -1775, -1680, -1590, -1505, -1425, -1349, -1277,
    -1208, -1143, -1082, -1024, -969, -917, -868, -821,
    -776, -734, -695, -657, -621, -588, -556, -526,
    -497, -470, -444, -420, -397, -375, -354, -334,
    -316, -298, -282, -266, -251, -237, -224, -211,
    -199, -188, -177, -167, -157, -148, -139, -131,
    -123, -116, -109, -103, -96, -90, -85, -79,
    -74, -70, -65, -61, -57, -53, -49, -46,
    -43, -39, -37, -34, -31, -29, -27, -24,
    -22, -20, -18, -17, -15, -13, -12, -10,
    -9, -7, -6, -5, -4, -3, -2, -1,
};

static s32* adpcm4Table[4] = { adpcmTable4_0, adpcmTable4_1, adpcmTable4_2, 0 };

#define ADPCM_DELTA(idx, n) (*(s32*)((u8*)adpcmTable + ((idx) << 5) + (n) * 4))
#define ADPCM_NEXT(idx, n) (((u8*)adpcmTable)[0xB20 + (idx) * 8 + (n)])

/* .sbss, in retail order (8040ACB0..8040ACD0) */
static s32 readTree_signed;
static s32 readTree_scale;
static s32 sbssUnused_ACB8;
static s32 sbssUnused_ACBC;
static s32 adpcmStride1;
static s32 adpcmStride2;
static s32 sbssUnused_ACC8;
static s32 adpcmStride4;

typedef struct HVQSoundHead {
    u32 w[2];
} HVQSoundHead;

typedef struct HVQSoundFrame {
    /* 0x0 */ u16 pad;
    /* 0x2 */ u16 value;
} HVQSoundFrame;

typedef struct HVQIpicHeader {
    /* 0x00 */ u8 dc_shift;
    /* 0x01 */ u8 unk_shift;
    /* 0x02 */ u8 pad[2];
    /* 0x04 */ u16 width;
    /* 0x06 */ u16 height;
    /* 0x08 */ u32 offset[16];
} HVQIpicHeader; // size: 0x48

typedef struct HVQBpicHeader {
    /* 0x00 */ u8 dc_shift;
    /* 0x01 */ u8 unk_shift;
    /* 0x02 */ u8 mc_bits_h0;
    /* 0x03 */ u8 mc_bits_v0;
    /* 0x04 */ u8 mc_bits_h1;
    /* 0x05 */ u8 mc_bits_v1;
    /* 0x06 */ u8 pad[2];
    /* 0x08 */ u32 offset[17];
} HVQBpicHeader; // size: 0x4C

void HVQM4InitDecoder(void);
void setHVQPlaneDesc(HVQSeqObj* info, s32 plane_idx, u8 h_samp, u8 v_samp);
s16 _readTree(HVQTree* tree, HVQBitBuffer* buf);
s16 getByte(HVQBitBuffer* buf);
void Ipic_BasisNumDec(HVQState* state);
void IpicDcvDec(HVQState* state);
s32 getBit(HVQBitBuffer* buf);
s32 decodeHuff(HVQBitTree* bt);
void MakeNest(HVQState* state, s32 x, s32 y);
void WeightImBlock(u8* dst, s32 stride, u8 value, u8 top, u8 bottom, u8 left, u8 right);
void BasisBlockDec(HVQState* state, u8* dst, s32 stride, u8 value, u8 type, s32 plane_idx);
s32 ReadBasisByte(HVQState* state, u8* basis, s32* bits, u8* nest, s32 nest_stride, s32 plane_idx);
void IpicBlockDec(HVQState* state, u8* dst, s32 stride, HVQStack* stack);
void IpicLineDec(HVQState* state, u8* dst, s32 stride, HVQStack* stack, s32 count);
void IpicPlaneDec(HVQState* state, s32 plane_idx, u8* dst);
void initMCHandler(HVQPlane* plane, HVQMCPlane* mc, s32 a, s32 b, s32 c);
void MotionComp(HVQPlane* plane, HVQMCPlane* mc, s32 x, s32 y);
void _MotionComp_00(u8* dst, s32 dst_stride, u8* src, s32 src_stride);
void _MotionComp_10(u8* dst, s32 dst_stride, u8* src, s32 src_stride);
void _MotionComp_01(u8* dst, s32 dst_stride, u8* src, s32 src_stride);
void _MotionComp_11(u8* dst, s32 dst_stride, u8* src, s32 src_stride);
void decode_PB_cc(HVQState* state, HVQMCPlane* mc, s32 type, s32 base);
void MCBlockDecBasis(HVQState* state, u8* dst, u8* src, u16 stride, u32 type, s32 base, u16 luma_stride, s32 plane_idx, s32 fx, s32 fy);
s32 ReadBasisNibble(HVQState* state, u8* basis, s32* bits, u8* base, s32 nest_stride, s32 plane_idx);
void MCBlockDecMCNest(HVQState* state, HVQMCPlane* mc, s32 x, s32 y);
void MCBlockDecDCNest(HVQState* state, HVQMCPlane* mc);
void PpicPlaneDec(HVQSeqObj* obj, HVQMCPlane* mc);
void BpicPlaneDec(HVQSeqObj* obj, s32 present, s32 past, s32 future);
void fn_80188758(void);
void HVQM4InitSeqObj(HVQSeqObj* obj, HVQVideoInfo* info);
u32 HVQM4BuffSize(HVQSeqObj* obj);
void HVQM4SetBuffer(HVQSeqObj* obj, HVQState* state);
void HVQM4DecodeIpic(HVQSeqObj* obj, HVQIpicHeader* hdr, s32 dst);
void HVQM4DecodePpic(HVQSeqObj* obj, void* data, s32 present, s32 past);
void HVQM4DecodeBpic(HVQSeqObj* obj, void* data, s32 present, s32 past, s32 future);
HVQSound* HVQM4DecSoundCreate(HVQSound* src);
s32 HVQM4DecSoundClose(HVQSound* snd);
s32 HVQM4DecSoundDecode(HVQSound* snd, HVQSoundFrame* hdr, u32 size, u8* in, s32 arg, u8* out);
void fn_80189934(HVQSoundState* st, u8* in, s16* out, u32 type, u32 count, s32 frame);
void fn_80189B30(HVQSoundState* st, u8* in, s16* out, u32 type, u32 count, s32 frame);
void HVQM4DecodePcm16Ch1(u8* in, u8* out, u16 value, s32 samples, s32 frame);
void HVQM4DecodePcm16Ch2(u8* in, u8* out, u16 value, s32 samples, s32 frame);
void fn_80189FFC(HVQSoundState* st, u8* in, s16* out, u32 type, u32 count, s32 frame);

/* .bss: 0x200-byte saturation table, then divTable[0x10] and mcdivTable[0x200] */
static u8 satTable[0x200];
static s32 divTable[0x10];
static s32 mcdivTable[0x200];

/* .rodata */
static const u8 hvqm4decTag[0x18] = {0, 0, 0, 0, 'h', 'v', 'q', 'm', '4', 'd', 'e', 'c',
                                     ' ', '1', '.', '2', ' ', ' ', ' ', ' ', 0, 0, 0, 0};
static const s32 mcbTypeNext[9] = {1, 2, 0, 2, 0, 1, 0, 0, 0};

static inline u8 saturate(s32 x)
{
    if (x < 0) {
        return 0;
    } else if (x > 0xFF) {
        return 0xFF;
    } else {
        return x;
    }
}

void HVQM4InitDecoder(void)
{
    int i;
    int n;

    for (i = 0, n = -0x80; i < 0x200; i++, n++) {
        satTable[i] = saturate(n);
    }

    divTable[0] = 0;
    for (i = 1; i < 0x10; i++) {
        divTable[i] = 0x1000 / (i * 16) * 16;
    }

    mcdivTable[0] = 0;
    for (i = 1; i < 0x200; i++) {
        mcdivTable[i] = 0x1000 / i;
    }
}

void setHVQPlaneDesc(HVQSeqObj* info, s32 plane_idx, u8 h_samp, u8 v_samp) {
    HVQPlane* plane = &info->state->planes[plane_idx];

    plane->width_shift = (h_samp == 2) ? 1 : 0;
    plane->width = info->width >> plane->width_shift;
    plane->height_shift = (v_samp == 2) ? 1 : 0;
    plane->height = info->height >> plane->height_shift;
    plane->size = plane->width * plane->height;
    plane->pb_per_mcb_x = 2 >> plane->width_shift;
    plane->pb_per_mcb_y = 2 >> plane->height_shift;
    plane->blocks_per_mcb = plane->pb_per_mcb_x * plane->pb_per_mcb_y;
    plane->h_blocks = info->width / (h_samp * 4);
    plane->v_blocks = info->height / (v_samp * 4);
    plane->h_blocks_safe = plane->h_blocks + 2;
    plane->v_blocks_safe = plane->v_blocks + 2;
    plane->mcb_offset[0] = 0;
    plane->mcb_offset[3] = 1;
    plane->mcb_offset[2] = (plane->mcb_offset[1] = plane->h_blocks_safe) + 1;
    plane->pb_offset[0] = 0;
    plane->pb_offset[3] = 4;
    plane->pb_offset[2] = (plane->pb_offset[1] = plane->width * 4) + 4;
}

inline s32 getBit(HVQBitBuffer* buf)
{
    u32 value;
    s32 bit;

    if ((bit = buf->bit) < 0) {
        value = buf->value = *buf->ptr++;
        bit               = 31;
    } else {
        value = buf->value;
    }

    value      = value >> bit & 1;
    buf->bit = bit - 1;
    return value;
}

inline s16 getByte(HVQBitBuffer* buf)
{
    u32 value;
    s32 bit;
    if ((bit = buf->bit) >= 7) {
        value = buf->value;
        value >>= bit - 7;
        bit -= 8;
    } else {
        value = buf->value;
        value <<= 7 - bit;
        buf->value = *buf->ptr++;
        value |= buf->value >> (bit + 25);
        bit += 24;
    }
    buf->bit = bit;
    value &= 0xFF;
    return value;
}

inline s32 decodeHuff(HVQBitTree* code)
{
    HVQTree* tree     = code->tree;
    HVQBitBuffer* buf = &code->buf;
    s32 point      = tree->root;

    while (point >= 0x100) {
        point = tree->array[getBit(buf)][point];
    }

    return tree->array[0][point];
}

static inline s32 decodeSOvfSym(HVQBitTree* buf, s32 range_min, s32 range_max)
{
    s32 ret = 0;
    s32 num;
    do {
        num = decodeHuff(buf);
        ret += num;
    } while (!(range_min < num && num < range_max));
    return ret;
}

static inline s32 decodeUOvfSym(HVQBitTree* code, s32 range_max)
{
    s32 ret = 0;
    s32 num;

    do {
        num = decodeHuff(code);
        ret += num;
    } while (num >= range_max);

    return ret;
}

static inline s32 getDeltaDC(HVQState* state, s32 c, s32* runln)
{
    s32 d;

    if (*runln == 0) {
        d = decodeSOvfSym(&state->dc_values[c], state->dc_min, state->dc_max);

        if (d == 0)
            *runln = decodeHuff(&state->dc_rle[c]);
        return d;
    } else {
        (*runln)--;
        return 0;
    }
}

s16 _readTree(HVQTree* const dst, HVQBitBuffer* const buf)
{
    s32 node;
    s16 no;
    s16 val;
    int unused[2];

    if (getBit(buf)) {

        node               = dst->pos++;
        dst->array[0][node] = _readTree(dst, buf);
        dst->array[1][node] = _readTree(dst, buf);
        return node;
    } else {

        no  = getByte(buf);
        val = no;

        if (readTree_signed && no > 0x7F) {
            val -= 0x100;
        }

        val <<= readTree_scale;
        dst->array[0][no] = val;
        return no;
    }
}

void Ipic_BasisNumDec(HVQState* state)
{
    s16 label;
    s32 runln;
    HVQBlock* ydat;
    HVQBlock* udat;
    HVQBlock* vdat;
    s32 i, j;
    s32 h_block, v_block;
    HVQBitTree* symcode;
    HVQBitTree* runcode;

    runln   = 0;
    ydat    = state->planes[0].payload;
    symcode = &state->basis_num[0];
    runcode = &state->basis_num_run[0];
    h_block = state->planes[0].h_blocks;
    v_block = state->planes[0].v_blocks;

    for (j = v_block; j > 0; j--) {
        for (i = h_block; i > 0; i--) {
            if (runln == 0) {
                label = decodeHuff(symcode);
                if (label == 0)
                    runln = decodeHuff(runcode);
                ydat++->type = label & 0xFF;
            } else {
                ydat++->type = 0;
                runln--;
            }
        }

        ydat += 2;
    }

    runln   = 0;
    udat    = state->planes[1].payload;
    vdat    = state->planes[2].payload;
    symcode = &state->basis_num[1];
    runcode = &state->basis_num_run[1];
    h_block = state->planes[1].h_blocks;
    v_block = state->planes[1].v_blocks;
    for (i = v_block; i > 0; i--) {
        for (j = h_block; j > 0; j--) {
            if (runln == 0) {
                label = decodeHuff(symcode);
                if (label == 0)
                    runln = decodeHuff(runcode);
                udat++->type = (label >> 0) & 0xF;
                vdat++->type = (label >> 4) & 0xF;
            } else {
                udat++->type = 0;
                vdat++->type = 0;
                runln--;
            }
        }

        udat += 2;
        vdat += 2;
    }
}

void IpicDcvDec(HVQState* state)
{
    s32 c;
    HVQBlock* curr;
    HVQBlock* prev;
    s32 run;
    s32 pred;
    s32 h_block, v_block;
    s32 h_blocks_safe;
    s32 i, j;

    for (c = 0; c < 3; c++) {
        h_block    = state->planes[c].h_blocks;
        v_block    = state->planes[c].v_blocks;
        h_blocks_safe = state->planes[c].h_blocks_safe;
        curr       = state->planes[c].payload;
        run        = 0;
        for (j = v_block; j > 0; j--) {

            prev = curr - h_blocks_safe;
            !prev;

            pred = prev->value;
            for (i = h_block; i > 0; i--) {
                u8 curr_dcv;
                u8 pb_dc;

                curr_dcv = pred + getDeltaDC(state, c, &run);
                prev++;
                pb_dc    = prev->value;
                curr++->value = curr_dcv;

                pred = (pb_dc + curr_dcv + 1) >> 1;
            }

            curr += 2;
        }
    }
}

void MakeNest(HVQState* state, s32 x, s32 y)
{
    int h_block, h_blocks_safe, v_block;
    HVQBlock* src;
    int h_dc_orderly, v_dc_orderly;
    int h_dc_reverse, v_dc_reverse;
    int h_nest_blank, v_nest_blank;
    int i, j;
    u8* nestP;
    u8* backP;
    HVQBlock* dP;
    u8* nP;

    h_block    = state->planes[0].h_blocks;
    h_blocks_safe = state->planes[0].h_blocks_safe;
    v_block    = state->planes[0].v_blocks;
    src        = state->planes[0].payload + (y * h_blocks_safe + x);

    if (h_block < state->nest_stride) {

        h_dc_orderly = h_block;
        if ((h_dc_reverse = state->nest_stride - h_block) > h_block)
            h_dc_reverse = h_block;
        h_nest_blank = state->nest_stride - (h_dc_orderly + h_dc_reverse);
    } else {
        h_dc_orderly = state->nest_stride;
        h_dc_reverse = h_nest_blank = 0;
    }

    if (v_block < state->nest_height) {

        v_dc_orderly = v_block;
        if ((v_dc_reverse = state->nest_height - v_block) > v_block)
            v_dc_reverse = v_block;
        v_nest_blank = state->nest_height - (v_dc_orderly + v_dc_reverse);
    } else {
        v_dc_orderly = state->nest_height;
        v_dc_reverse = v_nest_blank = 0;
    }

    nP = state->nest_data;
    for (j = v_dc_orderly; j > 0; j--) {
        dP = src;
        for (i = h_dc_orderly; i > 0; i--) {
            *nP++ = (dP->value >> 4) & 0xF;
            dP++;
        }

        for (i = h_dc_reverse; i > 0; i--) {
            dP--;
            *nP++ = (dP->value >> 4) & 0xF;
        }

        for (i = h_nest_blank; i > 0; i--)
            *nP++ = 0;
        src += h_blocks_safe;
    }

    nestP = nP - state->nest_stride;
    for (j = v_dc_reverse; j > 0; j--) {
        backP = nestP;
        for (i = state->nest_stride; i > 0; i--) {
            *nP++ = *backP++;
        }
        nestP -= state->nest_stride;
    }

    for (j = v_nest_blank; j > 0; j--) {
        for (i = state->nest_stride; i > 0; i--) {
            *nP++ = 0;
        }
    }
}

void WeightImBlock(u8* block, s32 blockWidth, u8 c, u8 u, u8 d, u8 l, u8 r)
{

    s32 u_d = u - d;
    s32 l_r = l - r;

    s32 v1 = u_d + l_r;
    s32 v2 = u_d - l_r;

    s32 c2 = c * 2;
    s32 c3 = c * 8 + 4;

    s32 t1 = u + l - c2;
    s32 t2 = u + r - c2;
    s32 t3 = d + r - c2;
    s32 t4 = l + d - c2;

    s32 t5 = u - l;
    s32 t6 = u - r;
    s32 t7 = d - r;
    s32 t8 = d - l;

    block[0] = satTable[(c3 + v1 + t1 >> 3) + 0x80];
    block[1] = satTable[(c3 + v1 + t5 >> 3) + 0x80];
    block[2] = satTable[(c3 + v2 + t6 >> 3) + 0x80];
    block[3] = satTable[(c3 + v2 + t2 >> 3) + 0x80];
    block += blockWidth;

    block[0] = satTable[(c3 + v1 - t5 >> 3) + 0x80];
    block[1] = satTable[(c3 - t3 >> 3) + 0x80];
    block[2] = satTable[(c3 - t4 >> 3) + 0x80];
    block[3] = satTable[(c3 + v2 - t6 >> 3) + 0x80];
    block += blockWidth;

    block[0] = satTable[(c3 - v2 - t8 >> 3) + 0x80];
    block[1] = satTable[(c3 - t2 >> 3) + 0x80];
    block[2] = satTable[(c3 - t1 >> 3) + 0x80];
    block[3] = satTable[(c3 - v1 - t7 >> 3) + 0x80];
    block += blockWidth;

    block[0] = satTable[(c3 - v2 + t4 >> 3) + 0x80];
    block[1] = satTable[(c3 - v2 + t8 >> 3) + 0x80];
    block[2] = satTable[(c3 - v1 + t7 >> 3) + 0x80];
    block[3] = satTable[(c3 - v1 + t3 >> 3) + 0x80];
    block += blockWidth;
}

static inline void OrgBlock(HVQState* state, u8* block, s32 blockWidth, s32 p)
{

    HVQBitBuffer* buf;
    u32* ptr;
    u32 temp1;
    u32 temp2;
    u32 temp3;

    buf                          = &state->fixvl[p];
    ptr                          = (u32*)buf->ptr;
    temp1                        = ptr[1];
    temp2                        = ptr[2];
    temp3                        = ptr[3];
    *(u32*)(block)               = ptr[0];
    *(u32*)(block += blockWidth) = temp1;
    *(u32*)(block += blockWidth) = temp2;
    *(u32*)(block += blockWidth) = temp3;
    buf->ptr = ptr + 4;
}

static inline void MotionBlock(u8* cP, int width, u8* tP, int hx, int hy) {
    if (hy) {
        if (hx) _MotionComp_11(cP, width, tP, width);
        else _MotionComp_01(cP, width, tP, width);
    } else if (hx) _MotionComp_10(cP, width, tP, width);
    else _MotionComp_00(cP, width, tP, width);
}

void BasisBlockDec(HVQState* state, u8* blk, s32 blkWidth, u8 value, u8 nbasis, s32 p) {
    s32 v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15;
    int mns;
    s32 avr;
    if (nbasis == 6) {
        OrgBlock(state, blk, blkWidth, p);
    } else {
        mns = state->unk_shift;
        avr = value << mns;
        if (nbasis == 1) {
            u8 bas[16];
            s32 scale = 0;
            s32 factor = ReadBasisByte(state, bas, &scale, state->nest_data, state->nest_stride, p);
            v0 = factor * bas[0];
            v1 = factor * bas[1];
            v2 = factor * bas[2];
            v3 = factor * bas[3];
            v4 = factor * bas[4];
            v5 = factor * bas[5];
            v6 = factor * bas[6];
            v7 = factor * bas[7];
            v8 = factor * bas[8];
            v9 = factor * bas[9];
            v10 = factor * bas[10];
            v11 = factor * bas[11];
            v12 = factor * bas[12];
            v13 = factor * bas[13];
            v14 = factor * bas[14];
            v15 = factor * bas[15];

        } else {
            u8 bas[16];
            s32 scale = 0;
            s32 factor;
            int nestWidth = state->nest_stride;
            v0 = 0;
            v1 = 0;
            v2 = 0;
            v3 = 0;
            v4 = 0;
            v5 = 0;
            v6 = 0;
            v7 = 0;
            v8 = 0;
            v9 = 0;
            v10 = 0;
            v11 = 0;
            v12 = 0;
            v13 = 0;
            v14 = 0;
            v15 = 0;

            while (nbasis--) {
                factor = ReadBasisByte(state, bas, &scale, state->nest_data, nestWidth, p);
                v0 += factor * bas[0];
                v1 += factor * bas[1];
                v2 += factor * bas[2];
                v3 += factor * bas[3];
                v4 += factor * bas[4];
                v5 += factor * bas[5];
                v6 += factor * bas[6];
                v7 += factor * bas[7];
                v8 += factor * bas[8];
                v9 += factor * bas[9];
                v10 += factor * bas[10];
                v11 += factor * bas[11];
                v12 += factor * bas[12];
                v13 += factor * bas[13];
                v14 += factor * bas[14];
                v15 += factor * bas[15];

            }
        }
        avr -= (v0 + (v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9 + v10 + v11 + v12 + v13 + v14 + v15)) >> 4;
        blk[0] = satTable[((v0 + avr) >> mns) + 0x80];
        blk[1] = satTable[((v1 + avr) >> mns) + 0x80];
        blk[2] = satTable[((v2 + avr) >> mns) + 0x80];
        blk[3] = satTable[((v3 + avr) >> mns) + 0x80];
        blk += blkWidth;
        blk[0] = satTable[((v4 + avr) >> mns) + 0x80];
        blk[1] = satTable[((v5 + avr) >> mns) + 0x80];
        blk[2] = satTable[((v6 + avr) >> mns) + 0x80];
        blk[3] = satTable[((v7 + avr) >> mns) + 0x80];
        blk += blkWidth;
        blk[0] = satTable[((v8 + avr) >> mns) + 0x80];
        blk[1] = satTable[((v9 + avr) >> mns) + 0x80];
        blk[2] = satTable[((v10 + avr) >> mns) + 0x80];
        blk[3] = satTable[((v11 + avr) >> mns) + 0x80];
        blk += blkWidth;
        blk[0] = satTable[((v12 + avr) >> mns) + 0x80];
        blk[1] = satTable[((v13 + avr) >> mns) + 0x80];
        blk[2] = satTable[((v14 + avr) >> mns) + 0x80];
        blk[3] = satTable[((v15 + avr) >> mns) + 0x80];
        blk += blkWidth;

    }
}

/* decodeHuff with the bit read expanded, for callers where retail inlines both */

#define BASIS_SAMPLE(i, expr)  \
    v = (expr);                \
    basis[i] = v;              \
    if ((s32)v > (s32)hi) {    \
        hi = v;                \
    } else if ((s32)v < (s32)lo) { \
        lo = v;                \
    }
s32 ReadBasisByte(HVQState* state, u8* basisOut, s32* pscl, u8* nestTop, s32 nestWidth, s32 p)
{
    u16 code;
    s32 step_x, step_y;
    s32 min, max;
    u8* nP;
    u8 value;

    code = *((u8*)state->fixvl[p].ptr)++ << 8;
    code |= *((u8*)state->fixvl[p].ptr)++;

    if (state->is_landscape) {
        nestTop += ((code >> 6) & 0x1F) * nestWidth + (code & 0x3F);
        step_x = 1 << ((code >> 11) & 1);
        step_y = nestWidth << ((code >> 12) & 1);
    } else {
        nestTop += (code & 0x3F) * nestWidth + ((code >> 6) & 0x1F);
        step_x = 1 << ((code >> 12) & 1);
        step_y = nestWidth << ((code >> 11) & 1);
    }

    nP = nestTop;

    value       = *nP;
    basisOut[0] = value;
    min = max = value;

    nP += step_x;

    value       = *nP;
    basisOut[1] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value       = *nP;
    basisOut[2] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value       = *nP;
    basisOut[3] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nestTop += step_y;
    nP = nestTop;

    value       = *nP;
    basisOut[4] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value       = *nP;
    basisOut[5] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value       = *nP;
    basisOut[6] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value       = *nP;
    basisOut[7] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nestTop += step_y;
    nP = nestTop;

    value       = *nP;
    basisOut[8] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value       = *nP;
    basisOut[9] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value        = *nP;
    basisOut[10] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value        = *nP;
    basisOut[11] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nestTop += step_y;
    nP = nestTop;

    value        = *nP;
    basisOut[12] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value        = *nP;
    basisOut[13] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value        = *nP;
    basisOut[14] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value        = *nP;
    basisOut[15] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    *pscl += decodeHuff(&state->bufTree0[p]);
    if (code & 0x8000) {
        return -divTable[max - min] * (*pscl + ((code >> 13) & 3));
    } else {
        return divTable[max - min] * (*pscl + ((code >> 13) & 3));
    }
}

void IpicBlockDec(HVQState* state, u8* dst, s32 stride, HVQStack* stack) {
    if (stack->curr.type == 0) {
        u8 value = stack->curr.value;
        u8 top = (stack->line_prev->type & 0x77) ? value : stack->line_prev->value;
        u8 bottom = (stack->line_next->type & 0x77) ? value : stack->line_next->value;
        u8 right = (stack->next.type & 0x77) ? value : stack->next.value;
        WeightImBlock(dst, stride, value, top, bottom, stack->value_prev, right);
        stack->value_prev = value;
    } else if (stack->curr.type == 8) {
        u8 value = stack->curr.value;
        u8* p;
        dst[0] = dst[1] = dst[2] = dst[3] = value;
        p = dst + stride;
        p[0] = p[1] = p[2] = p[3] = value;
        p += stride;
        p[0] = p[1] = p[2] = p[3] = value;
        p += stride;
        p[0] = p[1] = p[2] = p[3] = value;
        stack->value_prev = value;
    } else {
        BasisBlockDec(state, dst, stride, stack->curr.value, stack->curr.type, stack->plane_idx);
        stack->value_prev = stack->next.value;
    }
    stack->line_prev++;
    stack->line_next++;
}

void IpicLineDec(HVQState* state, u8* dst, s32 stride, HVQStack* stack, s32 count) {
    stack->next = *stack->line_curr;
    stack->value_prev = stack->line_curr->value;
    for (count--; count > 0; count--) {
        stack->curr = stack->next;
        stack->next = *++stack->line_curr;
        IpicBlockDec(state, dst, stride, stack);
        dst += 4;
    }
    stack->curr = stack->next;
    stack->line_curr += 3;
    IpicBlockDec(state, dst, stride, stack);
    stack->line_prev += 2;
    stack->line_next += 2;
}

void IpicPlaneDec(HVQState* state, s32 plane_idx, u8* dst_) {
    HVQBlock** payload;
    s32 stride;
    s32 stride4;
    s32 width;
    s32 y;
    u8* dst;
    HVQPlane* plane;
    HVQBlock* p;
    HVQStack stack;

    plane = &state->planes[plane_idx];
    payload = &plane->payload;
    y = plane->v_blocks;
    width = plane->h_blocks;
    dst = dst_;
    stride = plane->width;
    stack.plane_idx = plane_idx;
    p = plane->payload;
    stack.line_prev = p;
    stack.line_curr = p;
    stack.line_next = p + plane->h_blocks_safe;
    stride4 = stride * 4;
    if (y > 0) {
        IpicLineDec(state, dst, stride, &stack, width);
        dst += stride4;
        y--;
    }
    stack.line_prev = *payload;
    for (; y > 1; y--) {
        IpicLineDec(state, dst, stride, &stack, width);
        dst += stride4;
    }
    if (y > 0) {
        stack.line_next = stack.line_curr;
        IpicLineDec(state, dst, stride, &stack, width);
    }
}

void initMCHandler(HVQPlane* plane, HVQMCPlane* mc, s32 a, s32 b, s32 c) {
    s32 i;

    for (i = 0; i < 3; i++) {
        mc->rle = 0;
        mc->pb_dc = 0x7F;
        mc->present = a;
        mc->past = b;
        mc->future = c;
        mc->payload_cur_row = mc->payload_cur_blk = plane->payload;
        mc->h_mcb_stride = 8 >> plane->width_shift;
        mc->v_mcb_stride = (8 >> plane->height_shift) * plane->width;
        mc->pb_per_mcb_x = plane->pb_per_mcb_x;
        mc->stride = plane->h_blocks_safe * plane->pb_per_mcb_y;
        mc++;
        a += plane->size;
        b += plane->size;
        c += plane->size;
        plane++;
    }
}

void MotionComp(HVQPlane* planes_, HVQMCPlane* mc_, s32 x, s32 y) {
    HVQMCPlane* mc;
    HVQPlane* plane;
    int c;
    int i;
    s32* iofsP;
    int blocks;
    int hx;
    int hy;
    u8* t_top;
    int px, py;
    int offset;
    u8* cP;
    u8* tP;
    int width;
    int twidth;
    mc = mc_; plane = planes_;
    for (c = 0; c < 3; c++) {
        px = x >> plane->width_shift;
        py = y >> plane->height_shift;
        iofsP = plane->pb_offset;
        i = 0;
        hx = px & 1;
        hy = py & 1;
        blocks = plane->blocks_per_mcb;
        t_top = (px >> 1) + ((py >> 1) * plane->width + mc->target);
        for (; i < blocks; i++) {
            offset = *iofsP++;
            width = twidth = plane->width;
            cP = mc->top + offset;
            tP = t_top + offset;
            if (hy) {
                if (hx) _MotionComp_11(cP, width, tP, twidth);
                else _MotionComp_01(cP, width, tP, twidth);
            } else if (hx) _MotionComp_10(cP, width, tP, twidth);
            else _MotionComp_00(cP, width, tP, twidth);
        }
        mc++; plane++;
    }
}

void _MotionComp_00(u8* dst, s32 dst_stride, u8* src, s32 src_stride) {
    u8* a = src + src_stride;
    u8* d = dst + dst_stride;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    d[0] = a[0];
    d[1] = a[1];
    d[2] = a[2];
    d[3] = a[3];
    a += src_stride;
    d += dst_stride;
    d[0] = a[0];
    d[1] = a[1];
    d[2] = a[2];
    d[3] = a[3];
    a += src_stride;
    d += dst_stride;
    d[0] = a[0];
    d[1] = a[1];
    d[2] = a[2];
    d[3] = a[3];
}

void _MotionComp_10(u8* dst, s32 dst_stride, u8* src, s32 src_stride) {
    u8* a = src + src_stride;
    u8* d = dst + dst_stride;
    dst[0] = (src[0] + src[1] + 1) >> 1;
    dst[1] = (src[1] + src[2] + 1) >> 1;
    dst[2] = (src[2] + src[3] + 1) >> 1;
    dst[3] = (src[3] + src[4] + 1) >> 1;
    d[0] = (a[0] + a[1] + 1) >> 1;
    d[1] = (a[1] + a[2] + 1) >> 1;
    d[2] = (a[2] + a[3] + 1) >> 1;
    d[3] = (a[3] + a[4] + 1) >> 1;
    a += src_stride;
    d += dst_stride;
    d[0] = (a[0] + a[1] + 1) >> 1;
    d[1] = (a[1] + a[2] + 1) >> 1;
    d[2] = (a[2] + a[3] + 1) >> 1;
    d[3] = (a[3] + a[4] + 1) >> 1;
    a += src_stride;
    d += dst_stride;
    d[0] = (a[0] + a[1] + 1) >> 1;
    d[1] = (a[1] + a[2] + 1) >> 1;
    d[2] = (a[2] + a[3] + 1) >> 1;
    d[3] = (a[3] + a[4] + 1) >> 1;
}

void _MotionComp_01(u8* dst, s32 dst_stride, u8* src, s32 src_stride) {
    u8* a;
    u8* b = src + src_stride;
    u8* d = dst + dst_stride;
    dst[0] = (src[0] + b[0] + 1) >> 1;
    dst[1] = (src[1] + b[1] + 1) >> 1;
    dst[2] = (src[2] + b[2] + 1) >> 1;
    dst[3] = (src[3] + b[3] + 1) >> 1;
    a = b;
    b += src_stride;
    d[0] = (a[0] + b[0] + 1) >> 1;
    d[1] = (a[1] + b[1] + 1) >> 1;
    d[2] = (a[2] + b[2] + 1) >> 1;
    d[3] = (a[3] + b[3] + 1) >> 1;
    a += src_stride;
    b += src_stride;
    d += dst_stride;
    d[0] = (a[0] + b[0] + 1) >> 1;
    d[1] = (a[1] + b[1] + 1) >> 1;
    d[2] = (a[2] + b[2] + 1) >> 1;
    d[3] = (a[3] + b[3] + 1) >> 1;
    a += src_stride;
    b += src_stride;
    d += dst_stride;
    d[0] = (a[0] + b[0] + 1) >> 1;
    d[1] = (a[1] + b[1] + 1) >> 1;
    d[2] = (a[2] + b[2] + 1) >> 1;
    d[3] = (a[3] + b[3] + 1) >> 1;
}

void _MotionComp_11(u8* dst, s32 dst_stride, u8* src, s32 src_stride) {
    u8* a;
    u8* b = src + src_stride;
    u8* d = dst + dst_stride;
    dst[0] = (src[0] + src[1] + b[0] + b[1] + 2) >> 2;
    dst[1] = (src[1] + src[2] + b[1] + b[2] + 2) >> 2;
    dst[2] = (src[2] + src[3] + b[2] + b[3] + 2) >> 2;
    dst[3] = (src[3] + src[4] + b[3] + b[4] + 2) >> 2;
    a = b;
    b += src_stride;
    d[0] = (a[0] + a[1] + b[0] + b[1] + 2) >> 2;
    d[1] = (a[1] + a[2] + b[1] + b[2] + 2) >> 2;
    d[2] = (a[2] + a[3] + b[2] + b[3] + 2) >> 2;
    d[3] = (a[3] + a[4] + b[3] + b[4] + 2) >> 2;
    a += src_stride;
    b += src_stride;
    d += dst_stride;
    d[0] = (a[0] + a[1] + b[0] + b[1] + 2) >> 2;
    d[1] = (a[1] + a[2] + b[1] + b[2] + 2) >> 2;
    d[2] = (a[2] + a[3] + b[2] + b[3] + 2) >> 2;
    d[3] = (a[3] + a[4] + b[3] + b[4] + 2) >> 2;
    a += src_stride;
    b += src_stride;
    d += dst_stride;
    d[0] = (a[0] + a[1] + b[0] + b[1] + 2) >> 2;
    d[1] = (a[1] + a[2] + b[1] + b[2] + 2) >> 2;
    d[2] = (a[2] + a[3] + b[2] + b[3] + 2) >> 2;
    d[3] = (a[3] + a[4] + b[3] + b[4] + 2) >> 2;
}

void decode_PB_cc(HVQState* state, HVQMCPlane* mc, s32 proctype, s32 mcbtype)
{
    u8 cc;
    HVQBlock* dataP;
    HVQBlock* ydat;
    HVQBlock* udat;
    HVQBlock* vdat;
    HVQPlane* sip;
    u16* bofsP;
    s32 p, i, j;
    s32 blocks;
    HVQBitTree* symcode;
    HVQBitTree* runcode;
    s16 sym;

    cc = (mcbtype << 5) | (proctype << 4);
    if (proctype == 1) {
        for (p = 0; p < 3; p++) {
            dataP  = mc[p].payload_cur_blk;
            bofsP  = state->planes[p].mcb_offset;
            blocks = state->planes[p].blocks_per_mcb;
            for (i = 0; i < blocks; i++) {
                dataP[*bofsP++].type = cc;
            }
        }
    } else {

        ydat    = mc[0].payload_cur_blk;
        sip     = &state->planes[0];
        symcode = &state->basis_num[0];
        runcode = &state->basis_num_run[0];
        bofsP   = sip->mcb_offset;
        blocks  = sip->blocks_per_mcb;
        for (i = 0; i < blocks; i++) {
            j = *bofsP++;
            if (mc[0].rle == 0) {
                sym = decodeHuff(symcode);
                if (sym == 0) {
                    ydat[j].type              = cc;
                    mc[0].rle = decodeHuff(runcode);
                } else {
                    ydat[j].type = cc | sym;
                }
            } else {
                ydat[j].type = cc;
                mc[0].rle--;
            }
        }

        udat    = mc[1].payload_cur_blk;
        vdat    = mc[2].payload_cur_blk;
        sip     = &state->planes[1];
        symcode = &state->basis_num[1];
        runcode = &state->basis_num_run[1];
        bofsP   = sip->mcb_offset;
        blocks  = sip->blocks_per_mcb;
        for (i = 0; i < blocks; i++) {
            j = *bofsP++;
            if (mc[1].rle == 0) {
                sym = decodeHuff(symcode);
                if (sym == 0) {
                    udat[j].type              = cc;
                    vdat[j].type              = cc;
                    mc[1].rle = decodeHuff(runcode);
                } else {
                    udat[j].type = cc | ((sym >> 0) & 0xF);
                    vdat[j].type = cc | ((sym >> 4) & 0xF);
                }
            } else {
                udat[j].type = cc;
                vdat[j].type = cc;
                mc[1].rle--;
            }
        }
    }
}

static inline s32 GetMCAotSum(HVQState* state, s32 sum[16], u8 nbasis, u8* nestTop, s32 nestWidth, s32 p)
{
    s32 prev_scl;
    s32 scl;
    u8 bas[16];

    sum[0] = sum[1] = sum[2] = sum[3] = sum[4] = sum[5] = sum[6] = sum[7] = sum[8] = sum[9] = sum[10] = sum[11] = sum[12] = sum[13]
        = sum[14] = sum[15] = prev_scl = 0;

    while (nbasis--) {
        scl = ReadBasisNibble(state, bas, &prev_scl, nestTop, nestWidth, p);
        sum[0] += scl * bas[0];
        sum[1] += scl * bas[1];
        sum[2] += scl * bas[2];
        sum[3] += scl * bas[3];
        sum[4] += scl * bas[4];
        sum[5] += scl * bas[5];
        sum[6] += scl * bas[6];
        sum[7] += scl * bas[7];
        sum[8] += scl * bas[8];
        sum[9] += scl * bas[9];
        sum[10] += scl * bas[10];
        sum[11] += scl * bas[11];
        sum[12] += scl * bas[12];
        sum[13] += scl * bas[13];
        sum[14] += scl * bas[14];
        sum[15] += scl * bas[15];
    }
    return (sum[0] + sum[1] + sum[2] + sum[3] + sum[4] + sum[5] + sum[6] + sum[7] + sum[8] + sum[9] + sum[10] + sum[11] + sum[12] + sum[13]
            + sum[14] + sum[15])
        >> 4;
}

void MCBlockDecBasis(HVQState* state, u8* blk, u8* mblk, u16 blkWidth, u32 nbasis, s32 nestBase, u16 nestWidth, s32 p, s32 hx, s32 hy)
{
    u8* nestPtr = (u8*)nestBase;
    s32 sum[16];
    u8 mvc[16];
    s32 mov[16];
    s32 avr;
    s32 scl;
    s32 dcv;
    s32 value;
    s32 mean;
    s32 max, min;
    s32 i, j;
    s32 mns;

    if ((u8)(nbasis - 1) == 1) {
        avr = GetMCAotSum(state, sum, 1, nestPtr, nestWidth, p);
    } else {
        avr = GetMCAotSum(state, sum, nbasis - 1, nestPtr, nestWidth, p);
    }
    if (hy) {
        if (hx) _MotionComp_11(mvc, 4, mblk, blkWidth);
        else _MotionComp_01(mvc, 4, mblk, blkWidth);
    } else if (hx) _MotionComp_10(mvc, 4, mblk, blkWidth);
    else _MotionComp_00(mvc, 4, mblk, blkWidth);

    mean = (mvc[0] + mvc[1] + mvc[2] + mvc[3] + mvc[4] + mvc[5] + mvc[6] + mvc[7] + mvc[8] + mvc[9] + mvc[10] + mvc[11] + mvc[12] + mvc[13]
            + mvc[14] + mvc[15] + 8)
        >> 4;

    max = min = mvc[0] - mean;
    mov[0]    = min;

    value  = mvc[1] - mean;
    mov[1] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value  = mvc[2] - mean;
    mov[2] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value  = mvc[3] - mean;
    mov[3] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value  = mvc[4] - mean;
    mov[4] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value  = mvc[5] - mean;
    mov[5] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value  = mvc[6] - mean;
    mov[6] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value  = mvc[7] - mean;
    mov[7] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value  = mvc[8] - mean;
    mov[8] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value  = mvc[9] - mean;
    mov[9] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value   = mvc[10] - mean;
    mov[10] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value   = mvc[11] - mean;
    mov[11] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value   = mvc[12] - mean;
    mov[12] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value   = mvc[13] - mean;
    mov[13] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value   = mvc[14] - mean;
    mov[14] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    value   = mvc[15] - mean;
    mov[15] = value;
    if (value < min) {
        min = value;
    } else if (value > max) {
        max = value;
    }

    mns = state->unk_shift;
    avr = (decodeSOvfSym(&state->dc_values[p], state->dc_min, state->dc_max) >> state->dc_shift << mns) - avr;
    scl = (decodeSOvfSym(&state->dc_values[p], state->dc_min, state->dc_max) >> state->dc_shift) * mcdivTable[max - min];

    for (i = 0; i < 16; i += 8) {
        sum[i + 0] += avr + mov[i + 0] * scl;
        sum[i + 1] += avr + mov[i + 1] * scl;
        sum[i + 2] += avr + mov[i + 2] * scl;
        sum[i + 3] += avr + mov[i + 3] * scl;
        sum[i + 4] += avr + mov[i + 4] * scl;
        sum[i + 5] += avr + mov[i + 5] * scl;
        sum[i + 6] += avr + mov[i + 6] * scl;
        sum[i + 7] += avr + mov[i + 7] * scl;
    }

    j = 0;
    for (i = 0; i < 4; i++) {
        blk[0] = satTable[mvc[j] + (sum[j] >> mns) + 0x80];
        j++;
        blk[1] = satTable[mvc[j] + (sum[j] >> mns) + 0x80];
        j++;
        blk[2] = satTable[mvc[j] + (sum[j] >> mns) + 0x80];
        j++;
        blk[3] = satTable[mvc[j] + (sum[j] >> mns) + 0x80];
        j++;
        blk += blkWidth;
    }
}

#define NIB(p) (((*(p)) >> 4) & 0xF)
s32 ReadBasisNibble(HVQState* state, u8* basisOut, s32* pscl, u8* nestTop, s32 nestWidth, s32 p)
{
    u16 code;
    s32 step_x, step_y;
    s32 min, max;
    u8* nP;
    u8 value;

    code = *((u8*)state->fixvl[p].ptr)++ << 8;
    code |= *((u8*)state->fixvl[p].ptr)++;

    if (state->is_landscape) {
        nestTop += ((code >> 6) & 0x1F) * nestWidth + (code & 0x3F);
        step_x = 1 << ((code >> 11) & 1);
        step_y = nestWidth << ((code >> 12) & 1);
    } else {
        nestTop += (code & 0x3F) * nestWidth + ((code >> 6) & 0x1F);
        step_x = 1 << ((code >> 12) & 1);
        step_y = nestWidth << ((code >> 11) & 1);
    }

    nP = nestTop;

    value = (*nP >> 4) & 0xF;
    basisOut[0] = value;
    min = max = value;

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[1] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[2] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[3] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nestTop += step_y;
    nP = nestTop;

    value = (*nP >> 4) & 0xF;
    basisOut[4] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[5] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[6] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[7] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nestTop += step_y;
    nP = nestTop;

    value = (*nP >> 4) & 0xF;
    basisOut[8] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[9] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[10] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[11] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nestTop += step_y;
    nP = nestTop;

    value = (*nP >> 4) & 0xF;
    basisOut[12] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[13] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[14] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    nP += step_x;

    value = (*nP >> 4) & 0xF;
    basisOut[15] = value;
    if (value > max) {
        max = value;
    } else if (value < min) {
        min = value;
    }

    *pscl += decodeHuff(&state->bufTree0[p]);
    if (code & 0x8000) {
        return -divTable[max - min] * (*pscl + ((code >> 13) & 3));
    } else {
        return divTable[max - min] * (*pscl + ((code >> 13) & 3));
    }
}

void MCBlockDecMCNest(HVQState* state, HVQMCPlane* mch, s32 tx, s32 ty)
{
    int c, i;
    u8* nestP = mch[0].target + ((tx >> 1) - 32) + ((ty >> 1) - 16) * state->planes[0].width;
    for (c = 0; c < 3; c++) {
        HVQMCPlane* pmc      = &mch[c];
        HVQPlane* sip = &state->planes[c];
        int plane_dx      = (tx >> sip->width_shift + 1);
        int plane_dy      = (ty >> sip->height_shift + 1);
        u8* t_top         = pmc->target + (plane_dx + plane_dy * sip->width);
        u16* bofsP        = sip->mcb_offset;
        s32* iofsP        = sip->pb_offset;
        int blocks        = sip->blocks_per_mcb;
        for (i = 0; i < blocks; i++) {
            int offset = *iofsP++;
            int j      = *bofsP++;
            u8 nbasis  = pmc->payload_cur_blk[j].type & 0xF;

            u8* cP = pmc->top + offset;
            !nbasis;
            if (nbasis == 6) {
                OrgBlock(state, cP, sip->width, c);
            } else {
                u8* tP = t_top + offset;
                if (nbasis == 0) {
                    MotionBlock(cP, sip->width, tP, tx & 1, ty & 1);
                } else {
                    MCBlockDecBasis(state, cP, tP, sip->width, nbasis, (s32)nestP, state->planes[0].width, c, tx & 1, ty & 1);
                }
            }
        }
    }
}

/* Retail colors the parameters after the loop locals and reserves one unused stack slot,
 * so the parameters are copied into locals declared after j. 107/108: only the plane copy
 * reads r3 instead of the state copy. */
void MCBlockDecDCNest(HVQState* state_, HVQMCPlane* mc_) {
    s32 i;
    HVQBlock* blocks;
    s32 stride;
    s32 hsafe;
    u16* boffs;
    s32* poffs;
    s32 n;
    s32 j;
    HVQState* state;
    HVQMCPlane* mc;
    HVQPlane* plane;
    s32 unused;

    state = state_;
    mc = mc_;
    plane = state->planes;
    for (i = 0; i < 3; i++) {
        blocks = mc->payload_cur_blk;
        boffs = plane->mcb_offset;
        stride = plane->width;
        poffs = plane->pb_offset;
        hsafe = plane->h_blocks_safe;
        n = plane->blocks_per_mcb;

        for (j = 0; j < n; j++) {
            u16 off = *boffs++;
            u8* dst = mc->top + *poffs++;
            HVQBlock* b = &blocks[off];
            u8 value = b->value;
            u8 type = b->type & 0xF;

            if (type == 0) {
                u8 top = (blocks[off - hsafe].type & 0x77) ? value : blocks[off - hsafe].value;
                u8 left = (b[-1].type & 0x77) ? value : b[-1].value;
                u8 right = (b[1].type & 0x77) ? value : b[1].value;
                u8 bottom = (blocks[off + hsafe].type & 0x77) ? value : blocks[off + hsafe].value;
                WeightImBlock(dst, stride, value, top, bottom, left, right);
            } else if (type == 8) {
                u8* p;
                dst[0] = dst[1] = dst[2] = dst[3] = value;
                p = dst + stride;
                p[0] = p[1] = p[2] = p[3] = value;
                p += stride;
                p[0] = p[1] = p[2] = p[3] = value;
                p += stride;
                p[0] = p[1] = p[2] = p[3] = value;
            } else {
                BasisBlockDec(state, dst, stride, value, type, i);
            }
        }
        mc++;
        plane++;
    }
}

typedef struct RLDecoder { int status; int runlng; } RLDecoder;

static inline void decode_PB_dc(HVQState* state, HVQMCPlane* mch)
{
    int c, i, j;
    int blocks;
    HVQMCPlane* pmc;
    u16* bofsP;

    for (c = 0; c < 3; c++) {
        pmc    = &mch[c];
        bofsP  = state->planes[c].mcb_offset;
        blocks = state->planes[c].blocks_per_mcb;
        for (i = 0; i < blocks; i++) {
            j                = *bofsP++;
            pmc->payload_cur_blk[j].value = pmc->pb_dc = pmc->pb_dc + decodeSOvfSym(&state->dc_values[c], state->dc_min, state->dc_max);
        }
    }
}

static inline void reset_PB_dc(HVQState* state, HVQMCPlane* mch)
{
    int i;
    HVQMCPlane* pmc;

    for (i = 0; i < 3; i++) {
        pmc           = &mch[i];
        pmc->pb_dc = 0x7F;
    }
}

static inline void initMCBproc(HVQBitTree* code, RLDecoder* flag)
{
    if (code->buf.ptr) {
        flag->status = getBit(&code->buf);
        flag->runlng = decodeUOvfSym(code, 0xFF);
    }
}

static inline s32 getMCBproc(HVQBitTree* code, RLDecoder* flag)
{
    if (flag->runlng == 0) {
        flag->status ^= 1;
        flag->runlng = decodeUOvfSym(code, 0xFF);
    }
    flag->runlng--;
    return flag->status;
}

static inline void initMCBtype(HVQBitTree* code, RLDecoder* flag)
{
    int value;
    if (code->buf.ptr) {
        flag->status = getBit(&code->buf) << 1;
        flag->status |= getBit(&code->buf);
        flag->runlng = decodeUOvfSym(code, 0xFF);
    }
}

static inline s32 getMCBtype(HVQBitTree* code, RLDecoder* flag)
{

    if (flag->runlng == 0) {

        flag->status = ((const u32 (*)[3])mcbTypeNext)[getBit(&code->buf)][flag->status];
        flag->runlng = decodeUOvfSym(code, 0xFF);
    }
    flag->runlng--;
    return flag->status;
}

static inline void setMCTop(HVQMCPlane* mch)
{
    s32 i;
    HVQMCPlane* pmc;

    for (i = 0; i < 3; i++) {
        pmc          = &mch[i];
        pmc->top = (u8*)pmc->present;
    }
}

static inline void setMCNextBlk(HVQMCPlane* mch)
{
    s32 i;
    HVQMCPlane* pmc;

    for (i = 0; i < 3; i++) {
        pmc = &mch[i];
        pmc->top += pmc->h_mcb_stride;
        pmc->payload_cur_blk += pmc->pb_per_mcb_x;
    }
}

static inline void setMCDownBlk(HVQMCPlane* mch)
{
    s32 i;
    HVQMCPlane* pmc;

    for (i = 0; i < 3; i++) {
        pmc = &mch[i];
        pmc->present += pmc->v_mcb_stride;
        pmc->payload_cur_blk = pmc->payload_cur_row = pmc->payload_cur_row + pmc->stride;
    }
}

void PpicPlaneDec(HVQSeqObj* obj, HVQMCPlane* mch)
{
    HVQState* state = (HVQState*)obj->state;
    RLDecoder proc;
    RLDecoder type;
    int i, j;

    initMCBproc(&state->mcb_proc, &proc);
    initMCBtype(&state->mcb_type, &type);
    for (j = 0; j < obj->height; j += 8) {
        setMCTop(mch);
        for (i = 0; i < obj->width; i += 8) {
            getMCBtype(&state->mcb_type, &type);
            if (type.status == 0) {
                decode_PB_dc(state, mch);
                decode_PB_cc(state, mch, 0, type.status);
            } else {
                reset_PB_dc(state, mch);
                decode_PB_cc(state, mch, getMCBproc(&state->mcb_proc, &proc), type.status);
            }
            setMCNextBlk(mch);
        }
        setMCDownBlk(mch);
    }
}

static s32 readMVDelta(HVQBitTree* bt, s32 bits) {
    HVQTree* tree = bt->tree;
    s32 node = tree->root;
    s32 v;
    s32 k;

    while (node >= 0x100) {
        node = tree->array[getBit(&bt->buf)][node];
    }
    v = tree->array[0][node] << bits;
    for (k = bits - 1; k >= 0; k--) {
        v += getBit(&bt->buf) << k;
    }
    return v;
}

static inline void resetMCHandler(HVQState* state, HVQMCPlane* mch, u8* present)
{
    s32 i;
    HVQMCPlane* pmc;

    for (i = 0; i < 3; i++) {
        pmc           = &mch[i];
        pmc->present  = (s32)present;
        pmc->payload_cur_row = pmc->payload_cur_blk = state->planes[i].payload;
        present += state->planes[i].size;
    }
}

static inline void setMCTarget(HVQMCPlane* mch, s32 direct)
{
    if (direct == 0) {
        mch[0].target = (u8*)mch[0].past;
        mch[1].target = (u8*)mch[1].past;
        mch[2].target = (u8*)mch[2].past;
    } else {
        mch[0].target = (u8*)mch[0].future;
        mch[1].target = (u8*)mch[1].future;
        mch[2].target = (u8*)mch[2].future;
    }
}

static inline void getMVector(s32* vec, HVQBitTree* code, s32 fcode)
{
    s32 range;
    s32 v;
    s32 i;

    range = 1 << (fcode + 5);

    v = decodeHuff(code) << fcode;

    for (i = fcode - 1; i >= 0; i--) {
        v += getBit(&code->buf) << i;
    }
    *vec += v;

    if (*vec >= range) {
        *vec -= range << 1;
    } else if (*vec < -range) {
        *vec += range << 1;
    }
}

void BpicPlaneDec(HVQSeqObj* seqObj, s32 present, s32 past, s32 future)
{
    HVQState* state;
    HVQMCPlane mch[3];
    s32 vx, vy;
    s32 dir;
    s32 i, j;
    u8 cc;
    u8 mtype;
    int d;

    state = (HVQState*)seqObj->state;
    initMCHandler(state->planes, mch, present, past, future);
    PpicPlaneDec(seqObj, mch);
    resetMCHandler(state, mch, (u8*)present);
    dir = -1;

    for (j = 0; j < seqObj->height; j += 8) {
        setMCTop(mch);
        for (i = 0; i < seqObj->width; i += 8) {
            cc = mch[0].payload_cur_blk->type;

            d = (cc >> 5) & 3;
            if (d == 0) {

                MCBlockDecDCNest(state, mch);
            } else {

                if (--d != dir) {
                    dir = d;
                    setMCTarget(mch, dir);
                    vx = vy = 0;
                }
                getMVector(&vx, &state->mv_h, state->mc_bits_h[dir]);
                getMVector(&vy, &state->mv_v, state->mc_bits_v[dir]);

                mtype = (cc >> 4) & 1;
                if (mtype == 0)
                    MCBlockDecMCNest(state, mch, (i << 1) + vx, (j << 1) + vy);
                else
                    MotionComp(state->planes, mch, (i << 1) + vx, (j << 1) + vy);
            }
            setMCNextBlk(mch);
        }
        setMCDownBlk(mch);
    }
}

void fn_80188758(void) {
    HVQM4InitDecoder();
}

void HVQM4InitSeqObj(HVQSeqObj* obj, HVQVideoInfo* info) {
    obj->width = info->width;
    obj->height = info->height;
    obj->h_samp = info->h_samp;
    obj->v_samp = info->v_samp;
}

u32 HVQM4BuffSize(HVQSeqObj* obj) {
    s32 w = obj->width / 4;
    s32 cw = (obj->h_samp == 2) ? (w >> 1) : w;
    s32 h = obj->height / 4;
    s32 ch = (obj->v_samp == 2) ? (h >> 1) : h;
    return ((w + 2) * (h + 2) + (cw + 2) * (ch + 2) * 2) * 2 + sizeof(HVQState);
}

void HVQM4SetBuffer(HVQSeqObj* obj, HVQState* state) {
    HVQBlock* buf;
    HVQPlane* plane;
    s32 i;

    obj->state = state;
    setHVQPlaneDesc(obj, 0, 1, 1);
    setHVQPlaneDesc(obj, 1, obj->h_samp, obj->v_samp);
    setHVQPlaneDesc(obj, 2, obj->h_samp, obj->v_samp);
    if ((state->is_landscape = obj->width >= obj->height)) {
        state->nest_stride = 0x46;
        state->nest_height = 0x26;
    } else {
        state->nest_stride = 0x26;
        state->nest_height = 0x46;
    }
    state->basis_num[0].tree = state->basis_num[1].tree = &state->trees[3];
    state->basis_num_run[0].tree = state->basis_num_run[1].tree = &state->trees[1];
    state->dc_values[0].tree = state->dc_values[1].tree = state->dc_values[2].tree = &state->trees[0];
    state->dc_rle[0].tree = state->dc_rle[1].tree = state->dc_rle[2].tree = &state->trees[1];
    state->bufTree0[0].tree = state->bufTree0[1].tree = state->bufTree0[2].tree = &state->trees[2];
    state->mv_h.tree = state->mv_v.tree = &state->trees[4];
    state->mcb_proc.tree = state->mcb_type.tree = &state->trees[5];

    buf = (HVQBlock*)(state + 1);
    plane = state->planes;
    for (i = 0; i < 3; i++) {
        HVQBlock* p;
        s32 j;

        plane->border = buf;
        plane->payload = buf + plane->h_blocks_safe + 1;
        buf += plane->h_blocks_safe * plane->v_blocks_safe;
        p = plane->border;
        for (j = plane->h_blocks_safe; j > 0; j--) {
            p->value = 0x7F;
            p->type = 0xFF;
            p++;
        }
        p = buf;
        for (j = plane->h_blocks_safe; j > 0; j--) {
            p--;
            p->value = 0x7F;
            p->type = 0xFF;
        }
        p = plane->border + plane->h_blocks_safe;
        for (j = plane->v_blocks_safe - 2; j > 0; j--) {
            p->value = 0x7F;
            p->type = 0xFF;
            p += plane->h_blocks_safe;
        }
        p = plane->border + plane->h_blocks_safe * 2 - 1;
        for (j = plane->v_blocks_safe - 2; j > 0; j--) {
            p->value = 0x7F;
            p->type = 0xFF;
            p += plane->h_blocks_safe;
        }
        plane++;
    }
}

#define INIT_BITBUF(buf, base, off)           \
    do {                                      \
        u32* src_ = (u32*)((u8*)(base) + (off)); \
        if (((buf).size = *src_) == 0) {      \
            (buf).ptr = NULL;                 \
        } else {                              \
            (buf).ptr = src_ + 1;             \
        }                                     \
        (buf).bit = -1;                       \
    } while (0)

#define READ_TREE(bt, sgn, scale)                 \
    do {                                          \
        HVQTree* t_ = (bt).tree;                  \
        HVQBitBuffer* b_ = &(bt).buf;             \
        readTree_signed = (sgn);                  \
        readTree_scale = (scale);                 \
        t_->pos = 0x100;                          \
        if (b_->size != 0) {                      \
            t_->root = _readTree(t_, b_);         \
        } else {                                  \
            t_->root = 0;                         \
        }                                         \
    } while (0)

/* variable scale: the retail code latches it in a register before loading the tree */
#define READ_TREE_V(bt, sgn, scale)               \
    do {                                          \
        s32 sc_ = (scale);                        \
        HVQTree* t_ = (bt).tree;                  \
        HVQBitBuffer* b_ = &(bt).buf;             \
        readTree_signed = (sgn);                  \
        readTree_scale = sc_;                     \
        t_->pos = 0x100;                          \
        if (b_->size != 0) {                      \
            t_->root = _readTree(t_, b_);         \
        } else {                                  \
            t_->root = 0;                         \
        }                                         \
    } while (0)

#pragma inline_depth(0)
void HVQM4DecodeIpic(HVQSeqObj* obj, HVQIpicHeader* hdr, s32 dst) {
    HVQState* state = obj->state;
    u8* base = (u8*)hdr + 0x48;
    s32 i;
    HVQPlane* plane;

    state->unk_shift = hdr->unk_shift;
    INIT_BITBUF(state->basis_num[0].buf, base, hdr->offset[0]);
    INIT_BITBUF(state->basis_num_run[0].buf, base, hdr->offset[1]);
    INIT_BITBUF(state->basis_num[1].buf, base, hdr->offset[2]);
    INIT_BITBUF(state->basis_num_run[1].buf, base, hdr->offset[3]);
    INIT_BITBUF(state->dc_values[0].buf, base, hdr->offset[4]);
    INIT_BITBUF(state->bufTree0[0].buf, base, hdr->offset[5]);
    INIT_BITBUF(state->fixvl[0], base, hdr->offset[6]);
    INIT_BITBUF(state->dc_values[1].buf, base, hdr->offset[7]);
    INIT_BITBUF(state->bufTree0[1].buf, base, hdr->offset[8]);
    INIT_BITBUF(state->fixvl[1], base, hdr->offset[9]);
    INIT_BITBUF(state->dc_values[2].buf, base, hdr->offset[10]);
    INIT_BITBUF(state->bufTree0[2].buf, base, hdr->offset[11]);
    INIT_BITBUF(state->fixvl[2], base, hdr->offset[12]);
    INIT_BITBUF(state->dc_rle[0].buf, base, hdr->offset[13]);
    INIT_BITBUF(state->dc_rle[1].buf, base, hdr->offset[14]);
    INIT_BITBUF(state->dc_rle[2].buf, base, hdr->offset[15]);
    READ_TREE(state->basis_num[0], 0, 0);
    READ_TREE(state->basis_num_run[0], 0, 0);
    READ_TREE_V(state->dc_values[0], 1, hdr->dc_shift);
    READ_TREE(state->bufTree0[0], 0, 2);
    state->dc_max = 0x7F << hdr->dc_shift;
    state->dc_min = -0x80 << hdr->dc_shift;
    Ipic_BasisNumDec(state);
    IpicDcvDec(state);
    MakeNest(state, hdr->width, hdr->height);
    i = 0;
    plane = state->planes;
    for (; i < 3; i++) {
        IpicPlaneDec(state, i, (u8*)dst);
        dst += plane->size;
        plane++;
    }
}

void HVQM4DecodePpic(HVQSeqObj* obj, void* data, s32 present, s32 past) {
    HVQM4DecodeBpic(obj, data, present, past, present);
}

void HVQM4DecodeBpic(HVQSeqObj* obj, void* data, s32 present, s32 past, s32 future) {
    HVQBpicHeader* hdr = data;
    HVQState* state = obj->state;
    u8* base = (u8*)hdr + 0x4C;

    state->unk_shift = hdr->unk_shift;
    state->dc_shift = hdr->dc_shift;
    state->mc_bits_h[0] = hdr->mc_bits_h0;
    state->mc_bits_v[0] = hdr->mc_bits_v0;
    state->mc_bits_h[1] = hdr->mc_bits_h1;
    state->mc_bits_v[1] = hdr->mc_bits_v1;
    INIT_BITBUF(state->basis_num[0].buf, base, hdr->offset[0]);
    INIT_BITBUF(state->basis_num_run[0].buf, base, hdr->offset[1]);
    INIT_BITBUF(state->basis_num[1].buf, base, hdr->offset[2]);
    INIT_BITBUF(state->basis_num_run[1].buf, base, hdr->offset[3]);
    INIT_BITBUF(state->dc_values[0].buf, base, hdr->offset[4]);
    INIT_BITBUF(state->bufTree0[0].buf, base, hdr->offset[5]);
    INIT_BITBUF(state->fixvl[0], base, hdr->offset[6]);
    INIT_BITBUF(state->dc_values[1].buf, base, hdr->offset[7]);
    INIT_BITBUF(state->bufTree0[1].buf, base, hdr->offset[8]);
    INIT_BITBUF(state->fixvl[1], base, hdr->offset[9]);
    INIT_BITBUF(state->dc_values[2].buf, base, hdr->offset[10]);
    INIT_BITBUF(state->bufTree0[2].buf, base, hdr->offset[11]);
    INIT_BITBUF(state->fixvl[2], base, hdr->offset[12]);
    INIT_BITBUF(state->mv_h.buf, base, hdr->offset[13]);
    INIT_BITBUF(state->mv_v.buf, base, hdr->offset[14]);
    INIT_BITBUF(state->mcb_type.buf, base, hdr->offset[15]);
    INIT_BITBUF(state->mcb_proc.buf, base, hdr->offset[16]);
    READ_TREE(state->basis_num[0], 0, 0);
    READ_TREE(state->basis_num_run[0], 0, 0);
    READ_TREE_V(state->dc_values[0], 1, hdr->dc_shift);
    READ_TREE(state->bufTree0[0], 0, 2);
    READ_TREE(state->mv_h, 1, 0);
    READ_TREE(state->mcb_type, 0, 0);
    state->dc_max = 0x7F << hdr->dc_shift;
    state->dc_min = -0x80 << hdr->dc_shift;
    BpicPlaneDec(obj, present, past, future);
}
#pragma inline_depth(smart)

HVQSound* HVQM4DecSoundCreate(HVQSound* src) {
    HVQSound* snd = OSAllocFromHeap(__OSCurrHeap, 0x20);
    if (snd == NULL) {
        return NULL;
    }
    *(HVQSoundHead*)snd = *(HVQSoundHead*)src;
    return snd;
}

s32 HVQM4DecSoundClose(HVQSound* snd) {
    if (snd == NULL) {
        return 0;
    }
    OSFreeToHeap(__OSCurrHeap, snd);
    return 1;
}

s32 HVQM4DecSoundDecode(HVQSound* snd, HVQSoundFrame* hdr, u32 size, u8* in, s32 arg, u8* out) {
    u8 fmt;
    u16 value;
    u32 samples;
    s32 ret;
    volatile s32 pad[2]; // frame-size padding (target frame is 0x30)

    if (snd == NULL) {
        return 0;
    }
    if (out == NULL) {
        return 0;
    }
    fmt = snd->format;
    value = hdr->value;
    if (fmt & 0x80) {
        samples = size >> 1;
    } else {
        samples = size;
    }
    if (snd->type == 1) {
        ret = size * 4;
        switch (fmt & 0x7F) {
        case 0:
            fn_80189934((HVQSoundState*)snd->data, in, (s16*)out, value, samples, arg);
            break;
        case 1:
            HVQM4DecodePcm16Ch1(in, out, value, samples, arg);
            break;
        default:
            ret = 0;
            break;
        }
    } else {
        ret = size * 4;
        switch (fmt & 0x7F) {
        case 0:
            fn_80189B30((HVQSoundState*)snd->data, in, (s16*)out, value, samples, arg);
            break;
        case 1:
            HVQM4DecodePcm16Ch2(in, out, value, samples, arg);
            break;
        case 4:
            fn_80189FFC((HVQSoundState*)snd->data, in, (s16*)out, value, samples, arg);
            break;
        default:
            ret = 0;
            break;
        }
    }
    return ret;
}

void fn_80189934(HVQSoundState* st, u8* in, s16* out, u32 type, u32 count, s32 frame) {
    s32 pred;
    s32 idx;
    u32 bytes;
    u32 left;

    if (in == NULL) {
        return;
    }
    left = count;
    if (!(left & 1)) {
        bytes = left >> 1;
    } else {
        bytes = (left >> 1) + 1;
    }
    if (type == 1) {
        adpcmStride1 = frame;
        bytes += 2;
    } else if (type == 3) {
        adpcmStride1 = frame;
        bytes += 3;
    }
    if (bytes & 1) {
        bytes += 1;
    }
    bytes *= adpcmStride1;
    in += bytes;
    if (type == 1) {
        u8 hi = in[0];
        u8 lo = in[1];
        left -= 1;
        in += 2;
        st->pred0 = (s16)((hi << 8) | (lo & 0x80));
        st->idx0 = lo & 0x7F;
        out[0] = out[1] = (s16)st->pred0;
        out += 2;
    } else if (type == 3) {
        left -= 1;
        st->pred0 = (s16)((in[0] << 8) | in[1]);
        st->idx0 = in[2];
        in += 3;
        out[0] = out[1] = (s16)st->pred0;
        out += 2;
    }
    pred = st->pred0;
    idx = st->idx0;
    while (left != 0) {
        u8 b = *in;
        s32 n = (b >> 4) & 7;
        if (!(b & 0x80)) {
            pred += ADPCM_DELTA(idx, n);
            if (pred > 0x7FFF) {
                pred = 0x7FFF;
            }
        } else {
            pred -= ADPCM_DELTA(idx, n);
            if (pred < -0x8000) {
                pred = -0x8000;
            }
        }
        idx = ADPCM_NEXT(idx, n);
        out[0] = (s16)pred;
        out[1] = (s16)pred;
        if (left == 1) {
            break;
        }
        n = b & 7;
        if (!(b & 8)) {
            pred += ADPCM_DELTA(idx, n);
            if (pred > 0x7FFF) {
                pred = 0x7FFF;
            }
        } else {
            pred -= ADPCM_DELTA(idx, n);
            if (pred < -0x8000) {
                pred = -0x8000;
            }
        }
        idx = ADPCM_NEXT(idx, n);
        in += 1;
        out[2] = (s16)pred;
        left -= 2;
        out[3] = (s16)pred;
        out += 4;
    }
    st->pred0 = pred;
    st->idx0 = idx;
}

void fn_80189B30(HVQSoundState* st, u8* in, s16* out, u32 type, u32 count, s32 frame) {
    s32 pred0;
    s32 idx0;
    s32 pred1;
    s32 idx1;
    u32 bytes;
    u32 left;

    if (in == NULL) {
        return;
    }
    left = count;
    bytes = left;
    if (type == 1) {
        adpcmStride2 = frame;
        bytes += 4;
    } else if (type == 3) {
        adpcmStride2 = frame;
        bytes += 6;
    }
    if (bytes & 1) {
        bytes += 1;
    }
    bytes *= adpcmStride2;
    in += bytes;
    if (type == 1) {
        u8 lo;
        u8 lo1;
        left -= 1;
        lo = in[1];
        st->pred0 = (s16)((in[0] << 8) | (lo & 0x80));
        st->idx0 = lo & 0x7F;
        lo1 = in[3];
        st->pred1 = (s16)((in[2] << 8) | (lo1 & 0x80));
        st->idx1 = lo1 & 0x7F;
        in += 4;
        out[1] = (s16)st->pred0;
        out[0] = (s16)st->pred1;
        out += 2;
    } else if (type == 3) {
        left -= 1;
        st->pred0 = (s16)((in[0] << 8) | in[1]);
        st->idx0 = in[2];
        st->pred1 = (s16)((in[3] << 8) | in[4]);
        st->idx1 = in[5];
        in += 6;
        out[1] = (s16)st->pred0;
        out[0] = (s16)st->pred1;
        out += 2;
    }
    pred0 = st->pred0;
    idx0 = st->idx0;
    pred1 = st->pred1;
    idx1 = st->idx1;
    while (left != 0) {
        u8 b = *in;
        s32 n = (b >> 4) & 7;
        s32 m;
        if (!(b & 0x80)) {
            pred0 += ADPCM_DELTA(idx0, n);
            if (pred0 > 0x7FFF) {
                pred0 = 0x7FFF;
            }
        } else {
            pred0 -= ADPCM_DELTA(idx0, n);
            if (pred0 < -0x8000) {
                pred0 = -0x8000;
            }
        }
        idx0 = ADPCM_NEXT(idx0, n);
        out[1] = (s16)pred0;
        m = b & 7;
        if (!(b & 8)) {
            pred1 += ADPCM_DELTA(idx1, m);
            if (pred1 > 0x7FFF) {
                pred1 = 0x7FFF;
            }
        } else {
            pred1 -= ADPCM_DELTA(idx1, m);
            if (pred1 < -0x8000) {
                pred1 = -0x8000;
            }
        }
        idx1 = ADPCM_NEXT(idx1, m);
        in += 1;
        out[0] = (s16)pred1;
        out += 2;
        left -= 1;
    }
    st->pred0 = pred0;
    st->idx0 = idx0;
    st->pred1 = pred1;
    st->idx1 = idx1;
}

void HVQM4DecodePcm16Ch1(u8* in, u8* out, u16 value, s32 samples, s32 frame) {
    s32 off = samples * 2;
    u8* src = in + off * frame;
    u8* dst = out;
    s32 i;
    for (i = samples; i > 0; i--) {
        dst[0] = dst[2] = src[0];
        dst[1] = dst[3] = src[1];
        src += 2;
        dst += 4;
    }
}

void HVQM4DecodePcm16Ch2(u8* in, u8* out, u16 value, s32 samples, s32 frame) {
    s32 off = samples * 4;
    u8* src = in + off * frame;
    u8* dst = out;
    s32 i;
    for (i = samples; i > 0; i--) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        src += 4;
        dst += 4;
    }
}

void fn_80189FFC(HVQSoundState* st, u8* in, s16* out, u32 type, u32 count, s32 frame) {
    s32* tbl;
    s32 hasHdr;
    s32 bytes;
    s32 p0;
    s32 p1;
    u32 n;

    if (in == NULL) {
        return;
    }
    hasHdr = (type & 0x80) ? 1 : 0;
    tbl = adpcm4Table[type & 3];
    bytes = count * 2;
    if (hasHdr != 0) {
        adpcmStride4 = frame;
        bytes += 4;
    }
    bytes *= adpcmStride4;
    in += bytes;
    if (hasHdr != 0) {
        st->pred0 = (s16)((in[0] << 8) | in[1]);
        st->pred1 = (s16)((in[2] << 8) | in[3]);
        in += 4;
    }
    p0 = st->pred0;
    p1 = st->pred1;
    for (n = count; n != 0; n--) {
        {
            p0 += tbl[in[0]];
            if (p0 < -0x8000) {
                p0 = -0x8000;
            }
            if (p0 > 0x7FFF) {
                p0 = 0x7FFF;
            }
            out[1] = (s16)p0;
            p1 += tbl[in[1]];
            if (p1 < -0x8000) {
                p1 = -0x8000;
            }
            if (p1 > 0x7FFF) {
                p1 = 0x7FFF;
            }
            out[0] = (s16)p1;
            in += 2;
            out += 2;
        }
    }
    st->pred0 = p0;
    st->pred1 = p1;
}
