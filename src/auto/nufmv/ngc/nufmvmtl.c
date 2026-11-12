#include "types.h"

extern u8 lbl_802B10C0[];
extern s32 OSSendMessage(void* q, s32 m, s32 f);

struct nufmv_ent_s {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
};

extern struct nufmv_ent_s lbl_8034C080[];

extern s32 lbl_8040831C;

extern s32 lbl_80408314;

void fn_80049A10(void)
{
}

void fn_80048528(s32 a)
{
    lbl_80408314 = a << 1;
}

void fn_80048534(void)
{
    lbl_8040831C = 0;
}

void fn_80048540(s32* p)
{
    lbl_8034C080[p[6]].a = 0;
    lbl_8034C080[p[6]].c = 0;
}

void fn_80048498(void)
{
    OSSendMessage(lbl_802B10C0, 6, 1);
}

void fn_800484C8(void)
{
    OSSendMessage(lbl_802B10C0, 5, 1);
}

void fn_800484F8(void)
{
    OSSendMessage(lbl_802B10C0, 7, 1);
}

s32 OSInitMessageQueue(s32 *, s32 *, s32);
extern s32 lbl_802B10E0[];
extern s32 lbl_8036E580[];
extern s32 lbl_8037E580[];
extern volatile s32 lbl_80408310;
extern s32 *lbl_8040A500;
extern s32 *lbl_8040A504;
extern s32 lbl_8040A51C;
extern s32 lbl_8040A520;
extern s32 lbl_8040AE44;

void fn_80048348(s32* buffer)
{
    s32 next;

    OSInitMessageQueue(lbl_802B10C0, lbl_802B10E0, 8);
    lbl_8040A500 = lbl_8036E580;
    lbl_8040A504 = lbl_8037E580;
    lbl_8040A51C = *buffer;
    next = *buffer + 0x10000;
    *buffer = next;
    lbl_8040A520 = next;
    *buffer += 0x10000;
    lbl_80408310 = 0;
    lbl_8040AE44 = 0;
}

typedef void (*AIDMACallback)(void);
extern AIDMACallback AIRegisterDMACallback(AIDMACallback callback);
s32 AISetDSPSampleRate(s32);
s32 AIStartDMA();
s32 HVQM4PlayerExGetAudioSamples();
s32 (*fn_80009948(s32 *, s32))();
s32 (*fn_80009970(s32 *, s32))();
void fn_800485EC(void);
extern s32 lbl_80194624[];
extern s32 lbl_801946B0[];
extern s32 lbl_801946C4[];
extern s32 lbl_80408364;
extern s32 lbl_80408368;
extern AIDMACallback lbl_8040AE48;

void fn_80049A14(void) {
    s32 temp_r3;

    if ((s32) lbl_80408368 != 0) {
        temp_r3 = HVQM4PlayerExGetAudioSamples();
        if ((u32) (temp_r3 + 0xFFFF4456) <= 1U) {
            AISetDSPSampleRate(1);
            goto block_8;
        }
        if (temp_r3 == 0x7D1C) {
            AISetDSPSampleRate(0);
            goto block_8;
        }
        if (temp_r3 == 0) {
            fn_80009970(lbl_80194624, 0xFD)(lbl_801946B0);
            return;
        }
        fn_80009948(lbl_80194624, 0x100)(lbl_801946C4, temp_r3);
block_8:
        lbl_80408364 = 0;
        lbl_8040AE48 = AIRegisterDMACallback(fn_800485EC);
        fn_800485EC();
        AIStartDMA();
    }
}

s32 (*fn_8000995C(s32 *, s32))();

extern s32 lbl_80194658[];
extern u32 lbl_8040A560;
extern u32 lbl_8040A564[1];

s32 NuFmvNgc_Alloc(s32 size)
{
    s32 allocation;
    u32 next;

    allocation = (lbl_8040A564[0] + 0x1F) & 0xFFFFFFE0;
    next = allocation + size;
    lbl_8040A564[0] = next;
    if (next > lbl_8040A560) {
        fn_8000995C(lbl_80194624, 0x6B)(lbl_80194658);
    }
    return allocation;
}

extern void AXSetVoiceMix(s32 voice, void* mix);
extern s16 lbl_801BA5C0[36];
extern s32 lbl_8040A528;
extern s32 lbl_8040A52C;

void fn_8004856C(void)
{
    if (lbl_80408310 == 1 || lbl_80408310 == 3) {
        lbl_801BA5C0[0] = 0;
        lbl_801BA5C0[2] = 0;
        lbl_801BA5C0[20] = 0;
        lbl_801BA5C0[18] = 0;
        if (lbl_8040A528 != 0) {
            AXSetVoiceMix(lbl_8040A528, lbl_801BA5C0);
        }
        if (lbl_8040A52C != 0) {
            AXSetVoiceMix(lbl_8040A52C, lbl_801BA5C0 + 18);
        }
    }
}

extern f32 lbl_804082F8;
extern s32 lbl_80408304;

void fn_800483D0(s32 movie, s32 volume, s32 loop, s32 option)
{
    if (movie >= 0) {
        lbl_80408304 = movie;
        lbl_80408314 = (s32)((f32)volume * lbl_804082F8);
        if (option != 0) {
            if (loop != 0) {
                OSSendMessage(lbl_802B10C0, 4, 1);
            } else {
                OSSendMessage(lbl_802B10C0, 2, 1);
            }
        } else if (loop != 0) {
            OSSendMessage(lbl_802B10C0, 3, 1);
        } else {
            OSSendMessage(lbl_802B10C0, 1, 1);
        }
    }
}

s32 AIInitDMA(s32, s32);
s32 AISetDSPSampleRate(s32);
s32 AIStartDMA();
s32 HVQM4PlayerExBufvSetBusyFlag(s32, s32);
s32 HVQM4PlayerExCheckExecute(s32);
s32 HVQM4PlayerExClose(s32, s32 *);
s32 HVQM4PlayerExViRetrace(s32);
s32 OSDestroyHeap(s32);
s32 fn_80046068();
s32 fn_8004876C();
s32 fn_8004ACE0(s32);
s32 fn_8004ACF0(s32);
s32 fn_8004BE2C(s32, s32, s32, s32);
extern s32 lbl_8040834C;
extern s32 lbl_80408350;
extern s32 lbl_8040836C;
extern s32 lbl_80408370;
extern s32 lbl_80408374;
extern s32 lbl_8040A53C;
extern s32 lbl_8040A544;
extern s32 lbl_8040A548;
extern s32 lbl_8040A54C;
extern s32 lbl_8040A550;
extern s32 lbl_8040A554;
extern s32 lbl_8040A558;

void fn_80048998(void)
{
    s32 error;
    s32 reentry;
    s32 frame;
    s32 player;
    s32 finished;
    s32 one;

    reentry = lbl_80408374;
    if (reentry == 0) {
        one = 1;
        lbl_80408374 = one;
        switch (lbl_8040834C) {
        case 0:
            lbl_8040A548 = reentry;
            break;
        case 1:
            if (fn_8004876C() != 0) {
                lbl_8040834C = 2;
            } else {
                goto stop;
            }
            break;
        case 2:
            if (HVQM4PlayerExCheckExecute(lbl_8040A554) == 0) {
                finished = 1;
            } else {
                if (HVQM4PlayerExViRetrace(lbl_8040A554) != 0) {
                    frame = fn_8004ACF0(lbl_8040A554);
                    if (frame != 0) {
                        fn_8004BE2C(fn_8004ACE0(frame), lbl_8040A544, lbl_8040A54C, lbl_8040A550);
                        HVQM4PlayerExBufvSetBusyFlag(frame, 0);
                        lbl_8040A548 = one;
                    }
                }
                finished = 0;
            }
            if (finished != 0) {
                lbl_8040834C = 3;
            }
            break;
        case 3:
            if ((s32) lbl_80408368 != 0) {
                AIInitDMA(lbl_8040836C, lbl_80408370);
                AIStartDMA();
                AIRegisterDMACallback(lbl_8040AE48);
                fn_80046068();
                AISetDSPSampleRate(0);
            }
            player = lbl_8040A554;
            lbl_8040A554 = reentry;
            HVQM4PlayerExClose(player, &error);
            OSDestroyHeap(lbl_8040A53C);
            lbl_80408350 = reentry;
            if ((s32) lbl_8040A558 != 0) {
stop:
                lbl_8040834C = reentry;
            } else {
                lbl_8040834C = one;
            }
            break;
        }
        lbl_80408374 = 0;
    }
}
