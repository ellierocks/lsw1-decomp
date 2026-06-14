#include "types.h"
#include "nu3dx/nuanim.h"

extern u8 ObjectAnim[256];
extern f32 fn_8000B264(f32);

void fn_80016A98(void* data, struct nuanimtime_s* out, f32 time)
{
    f32 f0;
    s32 nchunks, tmp, chunk;
    f0 = 1.0f;
    if (*(s32*)data == 0x414E4934) {
        if (time >= f0) {
            nchunks = *(s16*)((s8*)data + 0x0A);
            if (time > (f32)nchunks) {
                out->time = (f32)(*(u16*)((s8*)data + 0x0A)) - 0.01f;
            } else {
                out->time = time;
            }
            chunk = ((s32)(out->time - 1.0f) >> 5);
            out->chunk = chunk;
            nchunks = *(s16*)((s8*)data + 0x08);
            if (chunk >= nchunks) {
                out->chunk = nchunks - 1;
            }
            f0 = out->time;
            f0 -= (f32)(out->chunk * 32);
            out->time_offset = f0;
            tmp = (s32)fn_8000B264(f0);
            out->unk14 = tmp - 1;
            tmp = out->unk14;
            if (tmp < 0) tmp += 7;
            out->time_byte = (u8)(tmp >> 3);
            out->time_mask = (u8)((1 << (out->unk14 - (out->time_byte << 3) + 1)) - 1);
        } else {
            out->time = f0;
        }
    } else {
        f32 endframe;
        endframe = *(f32*)data;
        if (time >= endframe) {
            if (endframe == 0.0f) {
                out->time = 0.0f;
                out->chunk = 0;
                out->time_mask = 1;
                out->unk14 = 0;
            } else {
                out->time = endframe - 1.0f;
                goto common;
            }
        } else {
            if (time < 1.0f) {
                out->time = 1.0f;
            } else {
                out->time = time;
            }
        }
common:;
        chunk = ((s32)(out->time - 1.0f) >> 5);
        out->chunk = chunk;
        nchunks = *(s16*)((s8*)data + 0x08);
        if (chunk >= nchunks) out->chunk = nchunks - 1;
        f0 = out->time;
        f0 -= (f32)(out->chunk * 32);
        out->time_offset = f0;
        tmp = (s32)fn_8000B264(f0);
        out->unk14 = tmp - 1;
        tmp = out->unk14;
        if (tmp < 0) tmp += 7;
        out->time_byte = (u8)(tmp >> 3);
        out->time_mask = (u8)((1 << (out->unk14 - (out->time_byte << 3) + 1)) - 1);
    }
}

f32 fn_80016C84(struct nuanimcurve_s* curve, struct nuanimtime_s* time)
{
    u32 type = time->time_byte;
    s32 idx = 0;
    u8* objanim = ObjectAnim;
    u8* mask_bytes = (u8*)&curve->mask;

    if (type == 1) {
        idx = objanim[mask_bytes[2] & time->time_mask] + objanim[mask_bytes[3]];
    } else if (type == 0) {
        idx = objanim[mask_bytes[3] & time->time_mask];
    } else if (type == 2) {
        idx = objanim[mask_bytes[3]] + objanim[mask_bytes[2]] + objanim[mask_bytes[1] & time->time_mask];
    } else if (type == 3) {
        idx = objanim[mask_bytes[3]] + objanim[mask_bytes[2]] + objanim[mask_bytes[1]] + objanim[mask_bytes[0] & time->time_mask];
    }

    {
        struct nuanimkey_s* keys = curve->animkeys;
        struct nuanimkey_s* prev = &keys[idx - 1];
        struct nuanimkey_s* curr = &keys[idx];
        u32 flags = curve->flags;

        if (flags & 1) {
            if ((flags & 2) && idx - 1 < curve->numkeys) {
                f32 d1 = time->time - prev->time;
                f32 d2 = curr->time - time->time;
                if (d1 > d2) return curr->d;
            }
            return prev->d;
        }

        // Hermite interpolation
        {
            f32 t1 = curr->time - prev->time;
            f32 t2 = prev->d - curr->d;
            f32 t3 = time->time - prev->time;
            f32 t4 = prev->c;
            f32 t5 = curr->c;
            f32 t6 = prev->dtime;

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
