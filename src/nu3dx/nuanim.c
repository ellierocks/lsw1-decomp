#include "types.h"
#include "nu3dx/nuanim.h"

extern u8 ObjectAnim[256];
extern f32 fn_8000B264(f32);

struct nuani4hdr_s {
    s32 magic;
    u8 unknown04[6];
    u16 nframes;
};

void NuAnimData2CalcTime(struct nuanimdata2_s* data, struct nuanimtime_s* out, f32 time)
{
    s32 tmp;
    s32 byte;

    if (*(s32*)data == 0x414E4934) {
        if (time < 1.0f) {
            out->time = 1.0f;
        } else {
            u32 n = ((struct nuani4hdr_s*)data)->nframes;
            if (time >= (f32)(s32)n) {
                out->time = (f32)n - 0.01f;
            } else {
                out->time = time;
            }
        }
        return;
    }
    if (time >= data->endframe) {
        if (data->endframe == 1.0f) {
            out->time = 1.0f;
            out->chunk = 0;
            out->time_byte = 0;
            out->time_mask = 1;
            return;
        }
        out->time = data->endframe - 0.01f;
    } else if (time < 1.0f) {
        out->time = 1.0f;
    } else {
        out->time = time;
    }
    out->chunk = ((s32)out->time - 1) >> 5;
    if (out->chunk >= data->nchunks) {
        out->chunk = data->nchunks - 1;
    }
    out->time_offset = out->time - (f32)(out->chunk * 32);
    tmp = (s32)fn_8000B264(out->time_offset) - 1;
    out->unk14 = tmp;
    byte = tmp / 8;
    out->time_byte = (u8)byte;
    out->time_mask = (u8)((1 << (tmp - byte * 8 + 1)) - 1);
}

f32 NuAnimCurveCalcVal2(struct nuanimcurve_s* curve, struct nuanimtime_s* time)
{
    s32 idx = 0;
    u8* mask_bytes = (u8*)&curve->mask;

    switch (time->time_byte) {
    case 0:
        idx = ObjectAnim[mask_bytes[3] & time->time_mask];
        break;
    case 1:
        idx = ObjectAnim[mask_bytes[3]];
        idx += ObjectAnim[mask_bytes[2] & time->time_mask];
        break;
    case 2:
        idx = ObjectAnim[mask_bytes[3]] + ObjectAnim[mask_bytes[2]];
        idx += ObjectAnim[mask_bytes[1] & time->time_mask];
        break;
    case 3:
        idx = ObjectAnim[mask_bytes[3]] + ObjectAnim[mask_bytes[2]] + ObjectAnim[mask_bytes[1]];
        idx += ObjectAnim[mask_bytes[0] & time->time_mask];
        break;
    }

    {
        struct nuanimkey_s* prev = curve->animkeys + (idx - 1);
        struct nuanimkey_s* curr = curve->animkeys + idx;
        u32 flags = curve->flags;

        if (flags & 1) {
            if ((flags & 2) && idx - 1 < curve->numkeys) {
                f32 d1 = time->time - prev->time;
                f32 d2 = curr->time - time->time;
                if (d1 > d2) return curr->d;
            }
            return prev->d;
        }

        {
            f32 t1 = curr->time - prev->time;
            f32 t2 = prev->d - curr->d;
            f32 t4 = prev->c;
            f32 t5 = curr->c;
            f32 t6 = prev->dtime;
            f32 t3 = time->time - prev->time;

            f32 t7 = t4 * t1;
            f32 t8 = t2 * 2.0f;
            f32 t9 = t5 * t1;
            f32 t10 = t8 + t7;
            f32 t11 = t3 * t6;
            f32 t12 = t2 * -3.0f;
            f32 t13 = t10 + t9;
            f32 t14 = t7 * 2.0f;
            f32 t15 = t11 * t13 + t12;
            f32 t16 = t15 - t14;
            f32 t17 = t16 - t9;
            f32 t18 = t11 * t17 + t7;
            return t11 * t18 + prev->d;
        }
    }
}

f32 NuAnimCurve2CalcVal(struct nuanimcurve2_s* curves, struct nuanimtime_s* time, s32 type)
{
    return 0.0f;
}
