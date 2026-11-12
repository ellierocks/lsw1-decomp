/*
 * LSW1 libsn FSasync: asynchronous PC file-server reads over the EXI2
 * debugger transport (SN ProDG TRK link). Reconstructed from the GL5E4F
 * retail image; names follow the SN libsn object list.
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef void (*FSCallback)(int result);

typedef struct FSHeader {
    u8 cmd;
    u8 pad1;
    u16 size;
    u32 pad4;
    u8 mode;
    u8 pad9[3];
    int handle;
    int offset;
    int len;
} FSHeader;

typedef struct FSAck {
    u8 cmd;
    u8 pad1;
    u16 size;
    u32 pad4;
    int result;
    u8 pad[0x14];
} FSAck;

extern int lbl_801528FC(void);
extern void lbl_80152984(void *buf, int len);
extern void lbl_80152B40(void *buf, int len);
extern void fn_80152E7C(void);
extern void fn_80152ECC(void *buf, int len);
extern void fn_80152F3C(void *buf, int len);
extern void fn_80152FA4(int len);
extern void fn_801530E0(void *buf, int len);

extern int OSDisableInterrupts(void);
extern int OSRestoreInterrupts(int level);
extern void *__OSSetInterruptHandler(int irq, void *handler);
extern u32 __OSUnmaskInterrupts(u32 mask);

#define EXI2_CR (*(volatile u32 *)0xCC006828)
#define EXI2_SR (*(volatile u32 *)0xCC006834)
#define PI_INTSR (*(volatile u32 *)0xCC003000)

volatile int g_nRWasyncPhase = 0;
static FSCallback g_FSCBFunc = 0;
volatile int g_nEXI2TCCnt = 0;
volatile int g_nDbgFsAsyncCnt = 0;

static int g_hHandle;
static u8 *g_pBuffer;
volatile int g_nBlockCnt;
volatile int g_nRemainderCnt;
volatile u32 g_nTotalBytesRemaining;
volatile int g_bDoFSACK;
int g_nFSLastError;
FSAck g_FsResult __attribute__((aligned(32)));

int CompleteAsync(void);
void PCrwSyncFSACK(void);

int PCwriteAsyncInit(void)
{
    return -1;
}

static void DoFSReadHeader(int handle, void *buf, int len, int offset)
{
    FSHeader hdr;
    u8 *p;
    int i;
    int size;

    size = 0x10;
    p = (u8 *)&hdr;
    for (i = 0; i < (int)sizeof(hdr); i++) {
        *p++ = 0;
    }
    hdr.cmd = 9;
    hdr.size = size << 8;
    if (g_bDoFSACK) {
        hdr.mode = 9;
    } else {
        hdr.mode = 0xB;
    }
    hdr.handle = handle;
    hdr.offset = offset;
    hdr.len = len;
    fn_80152FA4(sizeof(hdr));
    fn_801530E0(&hdr, sizeof(hdr));
}

void InitReadCounts(void)
{
    u32 n;

    n = g_nTotalBytesRemaining > 0x1FC00 ? 0x1FC00 : g_nTotalBytesRemaining;
    g_nTotalBytesRemaining = g_nTotalBytesRemaining - n;
    g_nRWasyncPhase = 2;
    g_nBlockCnt = n >> 10;
    g_nRemainderCnt = n & 0x3FF;
}

void PCreadAsyncNext(void)
{
    u8 *buf;
    int len;

    buf = g_pBuffer;
    PI_INTSR = 0x1000;
    InitReadCounts();
    if (g_nBlockCnt != 0) {
        len = 0x400;
        g_pBuffer = g_pBuffer + 0x400;
        g_nBlockCnt = g_nBlockCnt - 1;
    } else {
        len = g_nRemainderCnt;
        g_nRemainderCnt = 0;
    }
    fn_80152E7C();
    fn_80152ECC((void *)buf, len);
}

int PCreadAsyncInit(int handle, void *buf, u32 len, FSCallback cb, int offset, int doAck)
{
    if (((u32)buf & 0x1F) == 0 && (len & 0x1F) == 0) {
        CompleteAsync();
        g_bDoFSACK = doAck;
        g_FSCBFunc = cb;
        DoFSReadHeader(handle, buf, len, offset);
        g_nTotalBytesRemaining = len;
        g_hHandle = handle;
        g_pBuffer = buf;
        g_nRWasyncPhase = 1;
        return 0;
    }
    return -1;
}

void ReadSyncNext(void)
{
    u8 *buf;
    int len;

    buf = g_pBuffer;
    while (lbl_801528FC() == 0) {
    }
    PI_INTSR = 0x1000;
    InitReadCounts();
    if (g_nBlockCnt != 0) {
        len = 0x400;
        g_nBlockCnt = g_nBlockCnt - 1;
        g_pBuffer = g_pBuffer + 0x400;
    } else {
        len = g_nRemainderCnt;
        g_nRemainderCnt = 0;
    }
    fn_80152E7C();
    fn_80152F3C((void *)buf, len);
}

void CompletePCreadAsync(void)
{
    if (g_nRWasyncPhase == 1) {
        ReadSyncNext();
    }
    while (EXI2_SR & 1) {
    }
    for (;;) {
        g_nBlockCnt--;
        if (g_nBlockCnt == -1) {
            break;
        }
        fn_80152F3C((void *)g_pBuffer, 0x400);
        g_pBuffer = g_pBuffer + 0x400;
    }
    if (g_nRemainderCnt != 0) {
        fn_80152F3C((void *)g_pBuffer, g_nRemainderCnt);
        g_nRemainderCnt = 0;
    }
    EXI2_CR = 0;
    while (g_nTotalBytesRemaining != 0) {
        ReadSyncNext();
        for (;;) {
            g_nBlockCnt--;
            if (g_nBlockCnt == -1) {
                break;
            }
            fn_80152F3C((void *)g_pBuffer, 0x400);
            g_pBuffer = g_pBuffer + 0x400;
        }
        if (g_nRemainderCnt != 0) {
            fn_80152F3C((void *)g_pBuffer, g_nRemainderCnt);
            g_nRemainderCnt = 0;
        }
        EXI2_CR = 0;
    }
    PCrwSyncFSACK();
}

void PCrwAsyncFSACK(void)
{
    FSAck *ack;
    int n;

    ack = &g_FsResult;
    while ((n = lbl_801528FC()) == 0) {
    }
    lbl_80152984(ack, n);
    ack->size = 0;
    g_nFSLastError = ack->result;
    lbl_80152B40(ack, 8);
    g_nRWasyncPhase = 0;
    if (g_FSCBFunc != 0) {
        g_FSCBFunc(g_nFSLastError);
        g_FSCBFunc = 0;
    }
    PI_INTSR = 0x1000;
}

void PCrwSyncFSACK(void)
{
    FSAck *ack;
    int n;

    ack = &g_FsResult;
    if (g_bDoFSACK) {
        while ((n = lbl_801528FC()) == 0) {
        }
        lbl_80152984(ack, n);
        ack->size = 0;
        g_nFSLastError = ack->result;
        lbl_80152B40(ack, 8);
        g_nRWasyncPhase = 0;
        if (g_FSCBFunc != 0) {
            g_FSCBFunc(g_nFSLastError);
            g_FSCBFunc = 0;
        }
    }
    g_FSCBFunc = 0;
    EXI2_CR = 8;
    PI_INTSR = 0x1000;
    g_nRWasyncPhase = 0;
}

int CompleteAsync(void)
{
    if (g_nRWasyncPhase == 0) {
        return 0;
    }
    if (g_nRWasyncPhase == 5) {
        PCrwSyncFSACK();
    } else {
        CompletePCreadAsync();
    }
    return g_nFSLastError;
}

void PCrwAsyncNextPh(u32 phase)
{
    switch (phase) {
    case 1:
        PCreadAsyncNext();
        break;
    case 2:
    case 4:
        g_nDbgFsAsyncCnt++;
        break;
    case 3:
        break;
    case 5:
        PCrwAsyncFSACK();
        break;
    }
}

void EXI2TCHandler(void)
{
    u8 *buf;
    int len;
    int doAck;

    g_nEXI2TCCnt++;
    buf = g_pBuffer;
    EXI2_CR = 0x88;
    if (g_nRWasyncPhase == 0) {
        return;
    }
    if (g_nBlockCnt != 0) {
        if (g_nRWasyncPhase == 4) {
            g_pBuffer = buf + 0x200;
            len = 0x200;
        } else {
            g_pBuffer = buf + 0x400;
            len = 0x400;
        }
        g_nBlockCnt = g_nBlockCnt - 1;
    } else if (g_nRemainderCnt != 0) {
        len = g_nRemainderCnt;
        g_nRemainderCnt = 0;
    } else {
        EXI2_CR = 0;
        if (g_nTotalBytesRemaining != 0) {
            if (g_nRWasyncPhase != 4) {
                g_nRWasyncPhase = 1;
            }
            return;
        }
        doAck = g_bDoFSACK;
        if (doAck) {
            g_nRWasyncPhase = 5;
            return;
        }
        g_nRWasyncPhase = doAck;
        g_nFSLastError = doAck;
        if (g_FSCBFunc != 0) {
            g_FSCBFunc(0);
            g_FSCBFunc = (FSCallback)doAck;
        }
        return;
    }
    fn_80152ECC((void *)buf, len);
}

void SNInitEXI2TCHandler(void)
{
    int level;

    level = OSDisableInterrupts();
    __OSSetInterruptHandler(0x10, EXI2TCHandler);
    __OSUnmaskInterrupts(0x8000);
    EXI2_CR |= 8;
    EXI2_CR &= ~4;
    OSRestoreInterrupts(level);
}
