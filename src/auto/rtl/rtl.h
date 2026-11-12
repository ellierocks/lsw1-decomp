#ifndef LSW1_RTL_H
#define LSW1_RTL_H

#include "numath/numath.h"

/* GC stride 0x8c: allocation, template copy and set traversal agree. */
struct rtl_s {
    struct nuvec_s position;
    struct nuvec_s direction;
    struct nuvec_s colour;
    struct nuvec_s primaryColour;
    struct nuvec_s secondaryColour;
    f32 innerRadius;
    f32 outerRadius;
    f32 onTime;
    f32 onRandom;
    f32 offTime;
    f32 offRandom;
    f32 timer;
    u8 type;                      /* 0x58; disabled is byte 0x59 bit 7 on GC. */
    u32 disabled : 1;
    u32 pad59 : 7;
    s16 pitch;
    s16 yaw;
    u16 field5e;
    u16 field60;
    u8 pad62[2];
    f32 period;
    u8 pad68[2];
    u16 uid;                      /* 0x6a; cached-ID check in fn_80071720. */
    f32 intensity;
    u8 pad70[8];
    u8 pad78[0x14];
};

/* GC stack allocation is 0x148 in fn_80071720. */
struct rtldata_s {
    struct rtl_s* directional[3];
    f32 directionalStrength[3];
    struct rtl_s* ambient[3];
    f32 ambientStrength[3];
    struct rtl_s* anti[3];
    f32 antiStrength[3];
    s32 antiCount;
    struct rtl_s* cachedLight;    /* 0x4c */
    struct nuvec_s shadowDirection; /* 0x50 */
    f32 cachedValue;
    struct nuvec_s previousShadowDirection;
    f32 previousValue;
    f32 shadowBlend;
    u16 cachedUid;                /* 0x74 */
    u8 pad76[2];
    struct nuvec_s intensity[3];
    struct nuvec_s direction[3];
    struct nuvec_s colour;
    u8 padcc[0x54];
    f32 field120;
    struct nuvec_s blendedShadowDirection; /* 0x124 */
    f32 blendedValue;
    u8 pad134[0xc];
    f32 field140;                 /* 0x140 */
    u8 pad144[4];
};

#endif
