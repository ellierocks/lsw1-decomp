#include "types.h"

extern s32 lbl_80407F30;

void fn_800451B4(s32 a)
{
    lbl_80407F30 = a;
}

struct ngcsound_mix_s { u16 value[18]; };
struct ngcsound_voice_s {
    void *voice;
    s32 state;
    void *position;
    s32 requested;
    s32 stream;
    struct ngcsound_mix_s mix;
    f32 minDistance;
    f32 maxDistance;
};
extern struct ngcsound_voice_s lbl_802AFC20[];
extern f32 lbl_804082AC, lbl_804082B0;
extern void AXSetVoiceMix(void *, void *);
void fn_800451BC(s32 index)
{
    struct ngcsound_mix_s mix;
    f32 volume = lbl_804082AC * lbl_804082B0;
    mix = lbl_802AFC20[index].mix;
    mix.value[0] = lbl_802AFC20[index].mix.value[0] * volume;
    mix.value[2] = lbl_802AFC20[index].mix.value[2] * volume;
    AXSetVoiceMix(lbl_802AFC20[index].voice, &mix);
}

void (*fn_8000995C(s32 *, s32))(s32 *, ...);
extern s32 lbl_8019416C[];
extern s32 lbl_801941C0[];
extern s32 lbl_801941D4[];
extern s32 lbl_801941FC[];

void fn_80045F70(s32 arg0) {
    struct ngcsound_voice_s *voice;
    u32 count;
    s32 index;

    voice = lbl_802AFC20;
    count = 0;
    index = 0;
    do {
        if (voice->voice == (void *)arg0) {
            voice->state = 0;
            voice->voice = 0;
            fn_8000995C(lbl_8019416C, 0x16B)(lbl_801941C0, index);
            if (count++ != 0) {
                fn_8000995C(lbl_8019416C, 0x16F)(lbl_801941D4);
            }
        }
        index += 1;
        voice++;
    } while (index <= 0x3F);
    if (count == 0) {
        fn_8000995C(lbl_8019416C, 0x174)(lbl_801941FC);
    }
}

void AXFreeVoice(void *);
void AXSetVoiceState(void *, u16);
struct nusound_vec_s {
    f32 x, y, z;
};
void NuGobjCalcDims(s32, struct nusound_vec_s *, struct nusound_vec_s *);
void NuRndrShadPolys();
void fn_80046500();
extern s32 lbl_80313298[];

void fn_800455A4(void)
{
    s32 index;
    struct ngcsound_voice_s *base;
    struct ngcsound_voice_s *voice;
    u32 state;
    s32 mixIndex;
    u16 active;
    void **voiceSlot;
    void **positionSlot;
    void *position;
    void *direction;

    index = 0;
    do {
        voice = &lbl_802AFC20[index];
        state = voice->state;
        switch (state) {
        case 2:
            AXSetVoiceState(voice->voice, 1);
            voice->state = 3;
            break;
        case 4:
            AXSetVoiceState(voice->voice, 0);
            AXFreeVoice(voice->voice);
            voice->state = 0;
            voice->voice = NULL;
            break;
        case 3:
            active = *(u16 *)((s8 *)voice->voice + 0x146);
            if (active == 0) {
                AXSetVoiceState(voice->voice, 0);
                voiceSlot = &voice->voice;
                AXFreeVoice(*voiceSlot);
                voice->state = active;
                *voiceSlot = (void *)active;
            }
            break;
        }
        index++;
    } while (index <= 63);
    position = lbl_80313298;
    direction = (s8 *)position - 0x10;
    base = lbl_802AFC20;
    positionSlot = (void **)((s8 *)base + 8);
    mixIndex = 0;
    do {
        if (((s32 *)positionSlot)[-1] == 3) {
            if (*positionSlot != NULL) {
                NuGobjCalcDims(mixIndex, position, direction);
            } else {
                fn_800451BC(mixIndex);
            }
        }
        positionSlot = (void **)((s8 *)positionSlot + 64);
        mixIndex++;
    } while (mixIndex <= 63);
    fn_80046500();
    NuRndrShadPolys();
}

extern f32 NuFsqrt(f32);
extern f32 lbl_801941A8[];
extern f32 lbl_801941AC[];
extern f32 lbl_801941B0[];
void NuGobjCalcDims(s32 index, struct nusound_vec_s *listener, struct nusound_vec_s *direction)
{
    struct ngcsound_mix_s mix;
    struct nusound_vec_s delta;
    struct nusound_vec_s *position = lbl_802AFC20[index].position;
    f32 distance;
    f32 falloff;
    f32 left, right;
    f32 volume;
    delta.x = position->x - listener->x;
    delta.y = position->y - listener->y;
    delta.z = position->z - listener->z;
    distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
    if (distance > lbl_802AFC20[index].maxDistance * lbl_802AFC20[index].maxDistance)
        return;
    if (distance > lbl_801941A8[0])
        distance = NuFsqrt(distance);
    else
        distance = lbl_801941A8[0];
    if (distance > lbl_802AFC20[index].minDistance)
        falloff = (lbl_802AFC20[index].maxDistance - distance) / (lbl_802AFC20[index].maxDistance - lbl_802AFC20[index].minDistance);
    else
        falloff = lbl_801941AC[0];
    right = lbl_801941A8[0];
    if (distance > right) {
        f32 inv = lbl_801941AC[0] / distance;
        f32 pan;
        delta.z *= inv;
        delta.x *= inv;
        delta.y *= inv;
        pan = delta.x * direction->z + delta.z * direction->x;
        if (pan > right) {
            left = lbl_801941AC[0] - pan * lbl_801941B0[0];
            right = lbl_801941AC[0];
        } else {
            left = lbl_801941AC[0];
            right = left - -pan * lbl_801941B0[0];
        }
    } else {
        left = right = lbl_801941AC[0];
    }
    volume = lbl_804082AC * lbl_804082B0;
    left *= falloff;
    right *= falloff;
    left *= volume;
    right *= volume;
    mix = lbl_802AFC20[index].mix;
    mix.value[0] = lbl_802AFC20[index].mix.value[0] * left;
    mix.value[2] = lbl_802AFC20[index].mix.value[2] * right;
    AXSetVoiceMix(lbl_802AFC20[index].voice, &mix);
}
