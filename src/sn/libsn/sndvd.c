/*
 * LSW1 libsn sndvd: DVD emulation for the SN ProDG debugger. A data
 * breakpoint on the DI command register traps the game's DVD commands in a
 * DSI handler, which serves reads from the host PC through FSasync.
 * Reconstructed from the GL5E4F retail image.
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;

typedef struct DVDContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    u8 pad90[0x198 - 0x90];
    u32 srr0;
    u32 srr1;
} DVDContext;

typedef void (*FSCallback)(int result);

extern int PCreadAsyncInit(int handle, void *buf, u32 len, FSCallback cb, int offset, int doAck);
extern void OSReport(const char *fmt, ...);
extern void OSLoadContext(DVDContext *ctx);
extern void *__OSSetExceptionHandler(int exception, void *handler);
extern u32 lbl_801E19CC;

#define DI_REG(n) (*(volatile u32 *)(0xCC006000 + (n)))

int g_hDVD = 0;
/* Unreferenced zero word that retail links after g_hDVD in this unit's .data. */
int lbl_801E1C84 = 0;

int g_nDvdAudioCfg;
u32 g_aBuffer[8] __attribute__((aligned(32)));
static int g_nDvdError;
static int g_nDvdCurrOffset;
int g_nDvdReadLength;
int g_nBaseOffset;
static u8 g_nEmuState;
u8 g_nLidState;

void DSIHandler(int exception, DVDContext *ctx, u32 dsisr, u32 dar);

void DisDvdBP(void)
{
    u32 dabr = 0;

    asm volatile("lwz 3,%0\n\tmtspr 1013,3\n\tisync" : : "m"(dabr) : "r3");
}

void EnaDvdBP(void)
{
    u32 dabr = 0xCC00601E;

    asm volatile("lwz 3,%0\n\tmtspr 1013,3\n\tisync" : : "m"(dabr) : "r3");
}

static void ForceDvdTcIrq(void *addr)
{
    DisDvdBP();
    DI_REG(0x08) = 0x12000000;
    DI_REG(0x0C) = 0;
    DI_REG(0x10) = 0x20;
    DI_REG(0x18) = 0x20;
    DI_REG(0x14) = (u32)addr;
    DI_REG(0x1C) = 3;
    EnaDvdBP();
}

void ForceDvdDeIrq(void)
{
    DisDvdBP();
    DI_REG(0x08) = 0;
    DI_REG(0x1C) = 1;
    EnaDvdBP();
}

static void DvdCallback(int result)
{
    int len;

    len = (result + 0x1F) & ~0x1F;
    if (len == g_nDvdReadLength) {
        g_nDvdError = 0;
        g_nDvdCurrOffset = g_nDvdCurrOffset + len;
        ForceDvdTcIrq(g_aBuffer);
    } else {
        g_nDvdCurrOffset = -1;
        g_nDvdError = 0x31100;
        ForceDvdDeIrq();
    }
}

int CheckSeekOffset(int offset)
{
    if (offset > 0x5705FFFF) {
        return 0x52100;
    }
    return 0;
}

asm("\n\
	.text\n\
	.align 2\n\
	.type DSIExcHandler,@function\n\
DSIExcHandler:\n\
	mfdsisr 5\n\
	extrwi. 5,5,1,9\n\
	beq 1f\n\
	stw 0,0(4)\n\
	stw 1,4(4)\n\
	stw 2,8(4)\n\
	stmw 6,24(4)\n\
	mfspr 0,913\n\
	stw 0,424(4)\n\
	mfspr 0,914\n\
	stw 0,428(4)\n\
	mfspr 0,915\n\
	stw 0,432(4)\n\
	mfspr 0,916\n\
	stw 0,436(4)\n\
	mfspr 0,917\n\
	stw 0,440(4)\n\
	mfspr 0,918\n\
	stw 0,444(4)\n\
	mfspr 0,919\n\
	stw 0,448(4)\n\
	mfdsisr 5\n\
	mfdar 6\n\
	b DSIHandler\n\
1:\n\
	lwz 3,128(4)\n\
	mtcrf 255,3\n\
	lwz 3,132(4)\n\
	mtlr 3\n\
	lwz 3,136(4)\n\
	mtctr 3\n\
	lwz 3,140(4)\n\
	mtxer 3\n\
	lwz 3,408(4)\n\
	mtsrr0 3\n\
	lwz 3,412(4)\n\
	mtsrr1 3\n\
	lwz 3,12(4)\n\
	lwz 5,20(4)\n\
	lwz 4,16(4)\n\
	b fn_80151CA8\n\
	li 3,0\n\
	li 4,0\n\
	li 5,0\n\
	li 6,0\n\
	bl DSIHandler\n\
	.size DSIExcHandler,.-DSIExcHandler\n\
");

void DSIHandler(int exception, DVDContext *ctx, u32 dsisr, u32 dar)
{
    u32 insn;
    u32 *ea;
    u32 regs[10];
    int i;
    int offset;

    insn = *(u32 *)ctx->srr0;
    ea = (u32 *)(insn & 0xFFFF);
    if (insn & 0x8000) {
        ea = (u32 *)((u32)ea | 0xFFFF0000);
    }
    ea = (u32 *)((u32)ea + ctx->gpr[(insn >> 16) & 0x1F]);
    if (dar != 0xCC00601C) {
        goto passthru;
    }
    for (i = 0; i < 10; i++) {
        regs[i] = *(volatile u32 *)((0x0C006000 + i * 4) | 0xC0000000);
    }
    offset = g_nBaseOffset + (regs[3] << 2);
    switch ((int)(regs[2] >> 16)) {
    case 0x1200:
        ForceDvdTcIrq((void *)regs[5]);
        break;
    case 0xA800:
        if (regs[6] == 0) {
            ForceDvdTcIrq(g_aBuffer);
        } else if ((g_nDvdError = CheckSeekOffset(offset)) == 0) {
            g_nDvdReadLength = regs[6];
            if (PCreadAsyncInit(g_hDVD, (void *)regs[5], regs[6], DvdCallback, offset, 1) == -1) {
                g_nDvdError = 0x31100;
            }
        }
        if (g_nDvdError != 0) {
            ForceDvdDeIrq();
        }
        break;
    case 0xE000:
        DI_REG(0x20) = g_nDvdError;
        g_nDvdError = g_nDvdError & 0xFF000000;
        ForceDvdTcIrq(g_aBuffer);
        break;
    case 0xE100:
    case 0xE200:
    case 0xE401:
        if (g_nDvdAudioCfg != 0) {
            goto passthru;
        }
    case 0xAB00:
    case 0xE300:
        ForceDvdTcIrq(g_aBuffer);
        break;
    default:
        OSReport("Unknown DVD cmd (%08X)\n", regs[2]);
        asm volatile(".long 1");
        g_nDvdError = 0x52000;
        ForceDvdDeIrq();
        break;
    }
    goto done;
passthru:
    DisDvdBP();
    *ea = ctx->gpr[(insn >> 21) & 0x1F];
    EnaDvdBP();
done:
    ctx->srr0 = ctx->srr0 + 4;
    OSLoadContext(ctx);
}

void SNDVDEmuInit(int handle)
{
    if (handle != -0x8000) {
        return;
    }
    g_hDVD = handle;
    lbl_801E19CC = 0;
}

extern void DSIExcHandler(void);

void SNDVDEmuInitDSIHandler(void)
{
    if (g_hDVD == -0x8000) {
        g_nDvdAudioCfg = 0;
        g_nDvdError = 0;
        g_nDvdCurrOffset = -1;
        g_nBaseOffset = 0;
        g_nLidState = 1;
        g_nEmuState = 1;
        EnaDvdBP();
        __OSSetExceptionHandler(2, DSIExcHandler);
    }
}

u32 SNDVDEmuControl(int cmd)
{
    u32 result;

    result = 0;
    if (g_hDVD != -0x8000) {
        return 0;
    }
    switch (cmd) {
    case 0:
        g_nLidState = 0;
        break;
    case 1:
        g_nLidState = 1;
        break;
    case 2:
        g_nEmuState = 0;
        DisDvdBP();
        result = 0x80000000;
        break;
    case 3:
        g_nEmuState = 1;
        EnaDvdBP();
        result = 0x80000000;
        break;
    }
    return result | (g_nEmuState << 8) | g_nLidState;
}
