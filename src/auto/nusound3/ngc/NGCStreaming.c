#include "types.h"

extern u8 lbl_802B0C20[];
extern s32 OSSendMessage(void* q, s32 m, s32 f);

extern void fn_800455A4(void);
extern void AXRegisterCallback(void (*cb)(void));

struct ngcstream_ent_s {
    s32 active;
    s32 state;
    s32 id; /* fn_80046124 compares +0x08. */
    s32 requested;
    s32 stream;
    u8 unk14[0x2c];
};

extern struct ngcstream_ent_s lbl_802AFC20[];

extern s32 lbl_804082D8;

extern s32 lbl_804082E0;

extern s32 lbl_804082B4;

void fn_800472FC(void)
{
}

void fn_8004605C(void)
{
    lbl_804082B4 = 0;
}

void fn_800472F0(s32 a)
{
    lbl_804082E0 = a << 1;
}

void fn_80047300(void)
{
    lbl_804082D8 = 0;
}

void fn_80046090(void)
{
    s32 i;
    for (i = 0; i < 64; i++) {
        if (lbl_802AFC20[i].state == 3) {
            lbl_802AFC20[i].state = 4;
        }
    }
}

void fn_80046068(void)
{
    AXRegisterCallback(fn_800455A4);
}

void fn_80047260(void)
{
    OSSendMessage(lbl_802B0C20, 6, 1);
}

void fn_80047290(void)
{
    OSSendMessage(lbl_802B0C20, 5, 1);
}

void fn_800472C0(void)
{
    OSSendMessage(lbl_802B0C20, 7, 1);
}

s32 fn_80046124(s32 id)
{
    struct ngcstream_ent_s *p = lbl_802AFC20;
    s32 i;
    for (i = 0; i < 64; p++, i++) {
        if (p->active && p->id == id)
            return i;
    }
    return -1;
}

extern void (*fn_8000995C(char *, s32))(char *, ...);
extern char lbl_801943BC[];
extern char lbl_801943F8[];
/* fn_80047078 initializes 64 records with a 0x14-byte stride. */
struct stream_voice {
    void *voice;
    s32 unk4;
    s32 state;
    s32 index;
    s32 status;
};
extern struct stream_voice lbl_8034C080[];
/* fn_8004730C reads the callback index at +0x18. */
struct stream_callback {
    u8 unk0[0x18];
    s32 index;
};
void fn_8004730C(struct stream_callback *p)
{
    fn_8000995C(lbl_801943BC, 0x1A8)(lbl_801943F8);
    lbl_8034C080[p->index].voice = 0;
    lbl_8034C080[p->index].state = 0;
}

extern void OSInitMessageQueue(void *, void *, s32);
extern s32 OSCreateThread(void *, void *, s32, void *, s32, s32, s32);
extern void OSResumeThread(void *);
extern void (*fn_80009948(char *, s32))(char *, ...);
extern void fn_80047404(void);
extern char lbl_80194470[];
extern char lbl_802B0C40[], lbl_802B0DA0[];
extern char lbl_8034C580[], lbl_8035C580[], lbl_8036E57F[];
extern void *lbl_8040A4BC, *lbl_8040A4C0;
extern s32 lbl_8040A4E0, lbl_8040A4E4;
extern s32 lbl_804082D4, lbl_804082DC;
void fn_80047078(s32 *p)
{
    struct stream_voice *v;
    s32 n;
    OSInitMessageQueue(lbl_802B0C20, lbl_802B0C40, 8);
    lbl_8040A4BC = lbl_8034C580;
    lbl_8040A4C0 = lbl_8035C580;
    lbl_8040A4E0 = *p;
    *p += 0x10000;
    lbl_8040A4E4 = *p;
    *p += 0x10000;
    lbl_804082D4 = 0;
    v = lbl_8034C080;
    for (n = 64; n; n--, v++) {
        v->index = -1;
        v->voice = 0;
        v->state = 0;
        v->status = -59;
    }
    if (!OSCreateThread(lbl_802B0DA0, fn_80047404, 0, lbl_8036E57F, 0x2000, lbl_804082DC, 0))
        fn_80009948(lbl_801943BC, 0x3c4)(lbl_80194470);
    OSResumeThread(lbl_802B0DA0);
}

extern s32 lbl_804082C8;
extern f32 lbl_804082F8;
void fn_80047198(s32 id, s32 volume, s32 a, s32 b) {
    if (id >= 0) {
        lbl_804082C8 = id;
        lbl_804082E0 = (s32)((f32)volume * lbl_804082F8);
        if (b) {
            if (a)
                OSSendMessage(lbl_802B0C20, 4, 1);
            else
                OSSendMessage(lbl_802B0C20, 2, 1);
        } else {
            if (a)
                OSSendMessage(lbl_802B0C20, 3, 1);
            else
                OSSendMessage(lbl_802B0C20, 1, 1);
        }
    }
}

void AXFreeVoice(void *);
void AXSetVoiceAdpcmLoop(void *, void *);
void AXSetVoiceState(void *, u16);
extern s32 lbl_80194408[];
extern s32 lbl_802B0D90[];
extern s32 lbl_802B0D96[];
extern s32 lbl_8040A4DC;
extern s32 lbl_8040A4E0;
extern void *lbl_8040A4EC;
extern void *lbl_8040A4F0;
extern s32 lbl_8040A4F4;

void fn_80046500(void) {
    struct stream_voice *record;
    void **secondary;
    u32 state;
    s32 finished;
    u32 address;
    void *voice;

    record = (struct stream_voice *)lbl_8034C580;
    while (record > (struct stream_voice *)lbl_8034C580 - 64) {
        secondary = &lbl_8040A4F0;
        --record;
        voice = record->voice;
        if ((voice != NULL) && ((s32) record->index >= 0) && ((s32) record->status == 0)) {
            state = record->state;
            finished = 0;
            switch (state) {
            case 0:
                break;
            case 1:
                AXSetVoiceState(voice, 1);
                AXSetVoiceState(lbl_8040A4F0, 1);
                record->state = 2;
                break;
            case 2:
                record->state = 3;
                break;
            case 4:
                AXSetVoiceState(voice, 0);
                AXSetVoiceState(lbl_8040A4F0, 0);
                record->state = 0;
                break;
            case 3:
                if ((s32) (*(u16 *)((s8 *)(voice) + (0x146))) != 0) {
                    address = ((*(u16 *)((s8 *)(voice) + (0x1B2))) << 0x10) | (*(u16 *)((s8 *)(voice) + (0x1B4)));
                    lbl_8040A4F4 = ((s32) address / 2) - lbl_8040A4E0;
                    if ((s32) lbl_8040A4DC != 0) {
                        lbl_8040A4DC = 0;
                        if (lbl_8040A4EC != NULL) {
                            AXSetVoiceAdpcmLoop(lbl_8040A4EC, lbl_802B0D90);
                        }
                        if (lbl_8040A4F0 != NULL) {
                            AXSetVoiceAdpcmLoop(lbl_8040A4F0, lbl_802B0D96);
                        }
                    }
                } else {
                case 5:
                    finished = 1;
                }
                break;
            }
            if (finished != 0) {
                fn_8000995C(lbl_801943BC, 0x1EF)(lbl_80194408);
                AXSetVoiceState(record->voice, 0);
                AXSetVoiceState(*secondary, 0);
                AXFreeVoice(record->voice);
                AXFreeVoice(*secondary);
                record->voice = 0;
                *secondary = 0;
                lbl_8040A4EC = 0;
                record->state = 0;
            }
        }
    }
}

extern s32 NuFilePos(s32);
extern s32 NuFileRead(s32, void *, s32);
extern s32 NuFileSeek(s32, s32, s32);
extern s32 memset();
extern void DCFlushRange(void *, s32);
extern s32 lbl_804082F0;
extern s32 lbl_804082C4;
extern s32 lbl_804082CC;
extern s32 lbl_804082D0;
extern s32 lbl_8040A4C4[2];
extern s32 lbl_8040A4CC;
extern s32 lbl_8040A4D0;
extern s32 lbl_8040A4D4;
void fn_80046168(void)
{
    void **buffer;
    s32 *sequence;
    s32 i;
    s32 *sizes = lbl_8040A4C4;
    s32 capacity = 0x10000;
    s32 *serial = &lbl_8040A4D4;
    sequence = &lbl_8040A4CC;
    buffer = &lbl_8040A4BC;
    i = 0;
    do {
        if (*(s32 *)(i + (s32)sizes) == 0) {
            if (NuFilePos(lbl_804082F0) >= lbl_804082C4) {
                if (!lbl_804082CC) {
                    memset(*buffer, 0, 0x10000);
                    DCFlushRange(*buffer, 0x10000);
                    *sequence = (*serial)++;
                    *(s32 *)(i + (s32)sizes) = capacity;
                    lbl_804082D0++;
                    return;
                }
                NuFileSeek(lbl_804082F0, 0xc0, 0);
            }
            *(s32 *)(i + (s32)sizes) = NuFileRead(lbl_804082F0, *buffer, 0x10000);
            *sequence = (*serial)++;
            return;
        }
        sequence++;
        buffer++;
        i += 4;
    } while ((s32)sequence <= (s32)&lbl_8040A4D0);
}
