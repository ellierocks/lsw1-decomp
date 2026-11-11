#include "types.h"

s32 OSYieldThread();
s32 VIWaitForRetrace();
s32 fn_800466E4();
s32 fn_80047978();

void fn_80047404(void) {
    for (;;) {
        OSYieldThread();
        fn_800466E4();
        fn_80047978();
        VIWaitForRetrace();
    }
}

extern volatile s32 lbl_804082D4;
extern void *lbl_8040A4EC, *lbl_8040A4F0;
extern u16 lbl_801BA578[];
extern void AXSetVoiceMix(void *, void *);
void fn_80047384(void) {
    if (lbl_804082D4 == 1 || lbl_804082D4 == 3) {
        void *voice = lbl_8040A4EC;
        lbl_801BA578[0] = 0;
        lbl_801BA578[2] = 0;
        lbl_801BA578[20] = 0;
        lbl_801BA578[18] = 0;
        if (voice)
            AXSetVoiceMix(voice, lbl_801BA578);
        if (lbl_8040A4F0)
            AXSetVoiceMix(lbl_8040A4F0, lbl_801BA578 + 18);
    }
}

struct stream_voice {
    void *voice;
    s32 unk4;
    s32 state;
    s32 index;
    s32 status;
};
void AXFreeVoice(void *);
void AXSetVoiceAdpcmLoop(void *, void *);
void AXSetVoiceState(void *, u16);
extern s32 lbl_802B1230[];
extern s32 lbl_802B1236[];
extern s32 lbl_8034C580[];
extern s32 lbl_8040A518;
extern s32 lbl_8040A51C;
extern void *lbl_8040A528;
extern void *lbl_8040A52C;
extern s32 lbl_8040A530;

void NuRndrShadPolys(void) {
    struct stream_voice *record;
    void **secondary;
    u32 state;
    s32 finished;
    u32 address;
    void *voice;

    record = (struct stream_voice *)lbl_8034C580;
    while (record > (struct stream_voice *)lbl_8034C580 - 64) {
        secondary = &lbl_8040A52C;
        --record;
        voice = record->voice;
        if ((voice != NULL) && ((s32) record->index >= 0) && ((s32) record->status == 1)) {
            state = record->state;
            finished = 0;
            switch (state) {
            case 0:
                break;
            case 1:
                AXSetVoiceState(voice, 1);
                AXSetVoiceState(lbl_8040A52C, 1);
                record->state = 2;
                break;
            case 2:
                record->state = 3;
                break;
            case 4:
                AXSetVoiceState(voice, 0);
                AXSetVoiceState(lbl_8040A52C, 0);
                record->state = 0;
                break;
            case 3:
                if ((s32) (*(u16 *)((s8 *)(voice) + (0x146))) != 0) {
                    address = ((*(u16 *)((s8 *)(voice) + (0x1B2))) << 0x10) | (*(u16 *)((s8 *)(voice) + (0x1B4)));
                    lbl_8040A530 = ((s32) address / 2) - lbl_8040A51C;
                    if ((s32) lbl_8040A518 != 0) {
                        lbl_8040A518 = 0;
                        if (lbl_8040A528 != NULL) {
                            AXSetVoiceAdpcmLoop(lbl_8040A528, lbl_802B1230);
                        }
                        if (lbl_8040A52C != NULL) {
                            AXSetVoiceAdpcmLoop(lbl_8040A52C, lbl_802B1236);
                        }
                    }
                } else {
                case 5:
                    finished = 1;
                }
                break;
            }
            if (finished != 0) {
                AXSetVoiceState(record->voice, 0);
                AXSetVoiceState(*secondary, 0);
                AXFreeVoice(record->voice);
                AXFreeVoice(*secondary);
                record->voice = 0;
                *secondary = 0;
                lbl_8040A528 = 0;
                record->state = 0;
            }
        }
    }
}

extern void *lbl_8040A500;
extern s32 NuFilePos(s32);
extern s32 NuFileRead(s32, void *, s32);
extern s32 NuFileSeek(s32, s32, s32);
extern s32 memset();
extern void DCFlushRange(void *, s32);
extern s32 lbl_8040AE44;
extern s32 lbl_80408300;
extern s32 lbl_80408308;
extern s32 lbl_8040830C;
extern s32 lbl_8040A508[2];
extern s32 lbl_8040A4F8;
extern s32 lbl_8040A4FC;
extern s32 lbl_8040A510;
void fn_800475E0(void)
{
    void **buffer;
    s32 *sequence;
    s32 i;
    s32 *sizes = lbl_8040A508;
    s32 capacity = 0x10000;
    s32 *serial = &lbl_8040A510;
    sequence = &lbl_8040A4F8;
    buffer = &lbl_8040A500;
    i = 0;
    do {
        if (*(s32 *)(i + (s32)sizes) == 0) {
            if (NuFilePos(lbl_8040AE44) >= lbl_80408300) {
                if (!lbl_80408308) {
                    memset(*buffer, 0, 0x10000);
                    DCFlushRange(*buffer, 0x10000);
                    *sequence = (*serial)++;
                    *(s32 *)(i + (s32)sizes) = capacity;
                    lbl_8040830C++;
                    return;
                }
                NuFileSeek(lbl_8040AE44, 0xc0, 0);
            }
            *(s32 *)(i + (s32)sizes) = NuFileRead(lbl_8040AE44, *buffer, 0x10000);
            *sequence = (*serial)++;
            return;
        }
        sequence++;
        buffer++;
        i += 4;
    } while ((s32)sequence <= (s32)&lbl_8040A4FC);
}
