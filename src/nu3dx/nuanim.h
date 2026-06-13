#ifndef NUAANIM_H
#define NUAANIM_H

#include "types.h"
#include "numath/numath.h"

struct nuanimkey_s {
    f32 time;
    f32 dtime;
    f32 c;
    f32 d;
};

struct nuanimcurve_s {
    u32 mask;
    struct nuanimkey_s* animkeys;
    u32 numkeys;
    u32 flags;
};

struct nuanimcurveset_s {
    s32 flags;
    f32* constants;
    struct nuanimcurve_s** set;
    char ncurves;
};

struct nuanimdata_s {
    f32 time;
    char* node_name;
    s32 nchunks;
    struct nuanimdatachunk_s** chunks;
};

struct nuanimdatachunk_s {
    s32 numnodes;
    s32 num_valid_animcurvesets;
    struct nuanimcurveset_s** animcurvesets;
    struct nuanimkey_s* keys;
    struct nuanimcurve_s* curves;
};

struct nuanimtime_s {
    f32 time;
    f32 time_offset;
    s32 chunk;
    u32 time_mask;
    u32 time_byte;
};

struct nuanimdata2_s {
    f32 endframe;
    s16 nnodes;
    s16 ncurves;
    s16 nchunks;
    struct nuanimcurve2_s* curves;
    char* curveflags;
    char* curvesetflags;
};

struct nuanimcurve2_s {
    union {
        f32 constant;
        struct nuanimcurvedata_s* curvedata;
    } data;
};

struct nuanimcurvedata_s {
    u32* mask;
    u16* key_ixs;
    void* key_array;
};

struct NUJOINTANIM_s {
    f32 rx, ry, rz;
    f32 tx, ty, tz;
    f32 sx, sy, sz;
    s16 max_rx, max_ry, max_rz;
    s16 min_rx, min_ry, min_rz;
    u8 joint_id;
    u8 flags;
};

void NuAnimCurve2CalcVal(struct nuanimcurve2_s* curves, f32 time, void* result);

#endif
