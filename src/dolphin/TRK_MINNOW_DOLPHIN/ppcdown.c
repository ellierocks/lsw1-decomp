/* SN debugger private-ABI assembly, reconstructed from GL5E4F's ppcdown bodies.
 * MSSB (CC0), commit 407dbcbe70f8be5d167d0b1f0a229f38e0359019,
 * src/Dolphin/TRK_MINNOW_DOLPHIN was reviewed as a donor: it implements a
 * different debugger and supplies no bodies here. See docs/sn_runtime.md.
 * r31 is the debugger workspace; commands keep return addresses in r28-r30.
 * Ordinary C prologues would corrupt that exception/command calling convention.
 */

extern void DCFlushRange(void);
extern void DCInvalidateRange(void);
extern void EnableMetroTRKInterrupts(void);
extern void ICInvalidateRange(void);
extern void InitializeUART(void);
extern void OSReport(void);
extern void PPCHalt(void);
extern void WriteUARTN(void);
extern void __RAS_OSDisableInterrupts_begin(void);
extern void __RAS_OSDisableInterrupts_end(void);
extern void fn_801503A0(void);
extern void fn_80150418(void);
extern void fn_80150428(void);
extern void fn_8015057C(void);
extern void fn_801507CC(void);
extern void fn_801507E0(void);
extern void fn_801508CC(void);
extern void fn_801508F4(void);
extern void fn_801508F8(void);
extern void fn_80150938(void);
extern void fn_80150978(void);
extern void fn_801509C8(void);
extern void fn_80150A64(void);
extern void fn_80150B54(void);
extern void fn_80150B9C(void);
extern void fn_80150C3C(void);
extern void fn_80150D64(void);
extern void fn_80150D80(void);
extern void fn_80150D94(void);
extern void fn_80150DBC(void);
extern void fn_80150DD4(void);
extern void fn_80150DE8(void);
extern void fn_80150EAC(void);
extern void fn_80150F34(void);
extern void fn_8015100C(void);
extern void fn_801510BC(void);
extern void fn_801510E8(void);
extern void fn_8015114C(void);
extern void fn_80151190(void);
extern void fn_80151350(void);
extern void fn_80151510(void);
extern void lbl_80150F04(void);
extern void lbl_80150F4C(void);
extern void lbl_8015110C(void);
extern void lbl_80151AFC(void);
extern void lbl_80151DE4(void);
extern void lbl_80151F0C(void);
extern void lbl_80152610(void);
extern void lbl_80152700(void);
extern void PCrwAsyncNextPh(void);
extern void SNInitEXI2TCHandler(void);
extern void SNDVDEmuInit(void);
extern void SNDVDEmuInitDSIHandler(void);
extern void SNDVDEmuControl(void);
extern unsigned char lbl_801E19C0[];
extern unsigned char lbl_801E19C8[];
extern unsigned char lbl_801E19CC[];
extern unsigned char lbl_801E19D0[];
extern unsigned char lbl_801E19D4[];
extern unsigned char lbl_801E19DC[];
extern unsigned char lbl_801E19E0[];
extern unsigned char lbl_801E19FC[];
extern unsigned char lbl_801E1A18[];
extern unsigned char lbl_801E1A34[];
extern unsigned char lbl_801E1A50[];
extern unsigned char lbl_801E1AA0[];
extern unsigned char lbl_801E1AA8[];
extern unsigned char lbl_801E1ABC[];
extern unsigned char lbl_801E1AD5[];
extern unsigned char lbl_801E1B32[];
extern int g_nRWasyncPhase;
extern unsigned char lbl_802EAC60[];
extern unsigned char lbl_802F3480[];

asm void fn_801503A0(void)
{
    nofralloc
    li r6, 0x1
    stw r6, 0x6ec(r31)
    andi. r6, r3, 0x3
    bne L_801503EC
    andi. r6, r4, 0x3
    bne L_801503EC
    andi. r6, r5, 0x3
    bne L_801503EC
    subi r3, r3, 0x4
    subi r4, r4, 0x4
    add r5, r5, r4
L_801503CC:
    lwzu r6, 0x4(r4)
    stwu r6, 0x4(r3)
    cmplw r4, r5
    blt L_801503CC
    li r6, 0x0
    stw r6, 0x6ec(r31)
    li r3, 0x1
    blr
L_801503EC:
    subi r3, r3, 0x1
    subi r4, r4, 0x1
    add r5, r5, r4
L_801503F8:
    lbzu r6, 0x1(r4)
    stbu r6, 0x1(r3)
    cmplw r4, r5
    blt L_801503F8
    li r6, 0x0
    stw r6, 0x6ec(r31)
    li r3, 0x1
    blr
}

asm void fn_80150418(void)
{
    nofralloc
    li r6, 0x0
    stw r6, 0x6ec(r31)
    li r3, 0x0
    blr
}

asm void fn_80150428(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x10(r1)
    stw r31, 0x8(r1)
    stw r4, 0xc(r1)
    lis r6, lbl_801E19C0@h
    ori r6, r6, lbl_801E19C0@l
    lwz r5, 0x0(r6)
    cmpwi r5, 0x0
    bne L_80150504
    lis r3, g_nRWasyncPhase@h
    ori r3, r3, g_nRWasyncPhase@l
    lwz r3, 0x0(r3)
    cmpwi r3, 0x0
    beq L_8015046C
    bl PCrwAsyncNextPh
    b L_80150504
L_8015046C:
    lis r31, lbl_802EAC60@h
    ori r31, r31, lbl_802EAC60@l
    lwz r3, 0x6cc(r31)
    cmpwi r3, 0x2
    beq L_8015049C
    cmpwi r3, 0x3
    beq L_8015049C
    lwz r0, 0x69c(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    beq L_80150504
L_8015049C:
    lwz r4, 0xc(r1)
    lwz r5, 0x19c(r4)
    ori r5, r5, 0x400
    stw r5, 0x19c(r4)
    li r5, 0x1
    stw r5, 0x6bc(r31)
    lwz r10, 0x198(r4)
    lis r7, __RAS_OSDisableInterrupts_begin@h
    ori r7, r7, __RAS_OSDisableInterrupts_begin@l
    cmplw r10, r7
    blt L_801504E0
    lis r8, __RAS_OSDisableInterrupts_end@h
    ori r8, r8, __RAS_OSDisableInterrupts_end@l
    cmplw r10, r8
    bgt L_801504E0
    stw r7, 0x198(r4)
    mr r10, r7
L_801504E0:
    lwz r7, 0x0(r10)
    rlwinm r9, r7, 0, 11, 5
    lis r8, 0x7c00
    ori r8, r8, 0xa6
    cmpw r9, r8
    bne L_80150504
    rlwinm r7, r7, 14, 24, 28
    stw r7, 0x6c4(r31)
    stw r10, 0x6c0(r31)
L_80150504:
    lwz r31, 0x8(r1)
    lwz r0, 0x14(r1)
    addi r1, r1, 0x10
    mtlr r0
    blr
}

asm void EnableMetroTRKInterrupts(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x10(r1)
    stw r31, 0x8(r1)
    lis r31, lbl_802EAC60@h
    ori r31, r31, lbl_802EAC60@l
    li r5, 0x0
    stw r5, 0x6bc(r31)
    lwz r0, 0x698(r31)
    mtlr r0
    blrl
    mr r4, r3
    lis r3, lbl_801E1AA8@h
    ori r3, r3, lbl_801E1AA8@l
    bl OSReport
    lwz r3, 0x6cc(r31)
    cmpwi r3, 0x4
    bne L_80150568
    bl SNInitEXI2TCHandler
    bl SNDVDEmuInitDSIHandler
L_80150568:
    lwz r31, 0x8(r1)
    addi r1, r1, 0x10
    lwz r0, 0x4(r1)
    mtlr r0
    blr
}

asm void fn_8015057C(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x10(r1)
    stw r31, 0x8(r1)
    lis r31, lbl_802EAC60@h
    ori r31, r31, lbl_802EAC60@l
    stw r3, 0x6cc(r31)
    li r5, 0x0
    stw r5, 0x6d0(r31)
    lis r4, lbl_801E19E0@h
    ori r4, r4, lbl_801E19E0@l
    cmpwi r3, 0x2
    beq L_80150640
    lis r4, lbl_801E19FC@h
    ori r4, r4, lbl_801E19FC@l
    cmpwi r3, 0x3
    beq L_80150640
    lis r4, lbl_801E1A18@h
    ori r4, r4, lbl_801E1A18@l
    cmpwi r3, 0x4
    bne L_8015062C
    li r5, 0x1
    stw r5, 0x6d0(r31)
    lis r3, WriteUARTN@h
    ori r3, r3, WriteUARTN@l
    li r5, 0x3
    stw r5, 0x0(r3)
    lis r5, 0x4e80
    ori r5, r5, 0x20
    stw r5, 0x4(r3)
    lis r3, InitializeUART@h
    ori r3, r3, InitializeUART@l
    lis r5, 0x3860
    stw r5, 0x0(r3)
    lis r5, 0x4e80
    ori r5, r5, 0x20
    stw r5, 0x4(r3)
    lis r3, 0x8000
    ori r3, r3, 0xf4
    lwz r3, 0x0(r3)
    subi r3, r3, 0x4
    lwz r3, 0x0(r3)
    bl SNDVDEmuInit
    b L_80150640
L_8015062C:
    li r3, 0x1
    addi r1, r1, 0x10
    lwz r0, 0x4(r1)
    mtlr r0
    blr
L_80150640:
    addi r3, r31, 0x694
    li r5, 0x1c
    bl fn_801503A0
    li r5, 0x0
    stw r5, 0x6bc(r31)
    addi r3, r31, 0x6b0
    stw r5, 0x0(r3)
    lis r4, fn_80150428@h
    ori r4, r4, fn_80150428@l
    lwz r0, 0x694(r31)
    mtlr r0
    blrl
    lwz r0, 0x6a8(r31)
    mtlr r0
    blrl
    lis r3, 0x4
    ori r3, r3, 0x1040
    li r4, 0x1440
    lis r5, lbl_801E19CC@h
    ori r5, r5, lbl_801E19CC@l
    lwz r5, 0x0(r5)
    cmpwi r5, 0x0
    beq L_801506A4
    ori r3, r3, 0x4
    ori r4, r4, 0x4
L_801506A4:
    lis r5, lbl_801E19C8@h
    ori r5, r5, lbl_801E19C8@l
    lwz r5, 0x0(r5)
    cmpwi r5, 0x0
    beq L_801506C0
    ori r3, r3, 0x8
    ori r4, r4, 0x8
L_801506C0:
    lis r5, lbl_801E19D0@h
    ori r5, r5, lbl_801E19D0@l
    lwz r5, 0x0(r5)
    cmpwi r5, 0x0
    beq L_801506DC
    ori r3, r3, 0x20
    ori r4, r4, 0x20
L_801506DC:
    bl lbl_80151AFC
    li r3, 0x0
    stw r3, 0x688(r31)
    stw r3, 0x68c(r31)
    stw r3, 0x690(r31)
    lis r5, PPCHalt@h
    ori r5, r5, PPCHalt@l
    lis r3, lbl_801E1A34@h
    ori r3, r3, lbl_801E1A34@l
L_80150700:
    lwz r4, 0x0(r3)
    cmpwi r4, -0x1
    beq L_80150724
    lwz r6, 0x0(r5)
    cmplw r6, r4
    bne L_80150734
    addi r3, r3, 0x4
    addi r5, r5, 0x4
    b L_80150700
L_80150724:
    lis r3, PPCHalt@h
    ori r3, r3, PPCHalt@l
    li r31, 0x1
    stw r31, 0xc(r3)
L_80150734:
    bl lbl_80152610
    lis r3, WriteUARTN@h
    ori r3, r3, WriteUARTN@l
    li r4, 0x100
    bl DCFlushRange
    lis r3, InitializeUART@h
    ori r3, r3, InitializeUART@l
    li r4, 0x100
    bl DCFlushRange
    lis r3, lbl_80152700@h
    ori r3, r3, lbl_80152700@l
    li r4, 0x100
    bl DCFlushRange
    lis r3, PPCHalt@h
    ori r3, r3, PPCHalt@l
    li r4, 0x100
    bl DCFlushRange
    lis r3, WriteUARTN@h
    ori r3, r3, WriteUARTN@l
    li r4, 0x100
    bl ICInvalidateRange
    lis r3, InitializeUART@h
    ori r3, r3, InitializeUART@l
    li r4, 0x100
    bl ICInvalidateRange
    lis r3, lbl_80152700@h
    ori r3, r3, lbl_80152700@l
    li r4, 0x100
    bl ICInvalidateRange
    lis r3, PPCHalt@h
    ori r3, r3, PPCHalt@l
    li r4, 0x100
    bl ICInvalidateRange
    lwz r31, 0x8(r1)
    addi r1, r1, 0x10
    lwz r0, 0x4(r1)
    mtlr r0
    blr
}

asm void fn_801507CC(void)
{
    nofralloc
    lis r3, lbl_802EAC60@h
    ori r3, r3, lbl_802EAC60@l
    li r4, 0x1
    stw r4, 0x6d4(r3)
    blr
}

asm void fn_801507E0(void)
{
    nofralloc
    li r3, 0x0
    stw r3, 0x6b4(r31)
L_801507E8:
    lwz r0, 0x69c(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    beq L_801507E8
    stw r3, 0x6b4(r31)
    cmplwi r3, 0x8008
    ble L_80150810
    lis r3, 0x0
    ori r3, r3, 0x8008
L_80150810:
    mr r4, r3
    stw r4, 0x6b8(r31)
    addi r3, r31, 0x700
    lwz r0, 0x6a0(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    beq L_80150854
    lis r3, lbl_801E1ABC@h
    ori r3, r3, lbl_801E1ABC@l
    lwz r4, 0x6d0(r31)
    cmpwi r4, 0x0
    bne L_8015084C
    bl OSReport
    b L_801508BC
L_8015084C:
    bl fn_801510E8
    b L_801508BC
L_80150854:
    lwz r3, 0x6b4(r31)
    lwz r4, 0x6b8(r31)
    subf r3, r4, r3
    stw r3, 0x6b4(r31)
    lbz r3, 0x700(r31)
    cmplwi r3, 0x14
    blt L_80150894
    lis r3, lbl_801E1AD5@h
    ori r3, r3, lbl_801E1AD5@l
    lwz r4, 0x6d0(r31)
    cmpwi r4, 0x0
    bne L_8015088C
    bl OSReport
    b L_801508BC
L_8015088C:
    bl fn_801510E8
    b L_801508BC
L_80150894:
    lis r4, lbl_801E19DC@h
    ori r4, r4, lbl_801E19DC@l
    li r5, 0x1
    stw r5, 0x0(r4)
    slwi r3, r3, 2
    lis r4, lbl_801E1A50@h
    ori r4, r4, lbl_801E1A50@l
    lwzx r5, r3, r4
    mtlr r5
    blrl
L_801508BC:
    lwz r5, 0x6bc(r31)
    cmpwi r5, 0x0
    beq fn_801507E0
    b fn_80150F34
}

asm void fn_801508CC(void)
{
    nofralloc
    lis r3, lbl_801E1B32@h
    ori r3, r3, lbl_801E1B32@l
    lwz r4, 0x6d0(r31)
    cmpwi r4, 0x0
    bne L_801508E8
    bl OSReport
    b L_801508EC
L_801508E8:
    bl fn_801510E8
L_801508EC:
    mtlr r30
    blr
}

asm void fn_801508F4(void)
{
    nofralloc
    blr
}

asm void fn_801508F8(void)
{
    nofralloc
    mflr r30
    addi r3, r31, 0x700
    li r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    lis r3, lbl_801E1AA0@h
    ori r3, r3, lbl_801E1AA0@l
    li r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    li r5, 0x0
    stw r5, 0x6bc(r31)
    mtlr r30
    blr
}

asm void fn_80150938(void)
{
    nofralloc
    mflr r30
    li r6, 0x704
    lwbrx r3, r6, r31
    addi r4, r31, 0x0
    add r3, r3, r4
    addi r4, r31, 0x708
    li r6, 0x702
    lhbrx r5, r6, r31
    bl fn_801503A0
    addi r3, r31, 0x700
    li r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    mtlr r30
    blr
}

asm void fn_80150978(void)
{
    nofralloc
    mflr r30
    li r6, 0x704
    lwbrx r4, r6, r31
    addi r5, r31, 0x0
    add r4, r4, r5
    addi r3, r31, 0x708
    li r6, 0x702
    lhbrx r5, r6, r31
    mr r29, r5
    bl fn_801503A0
    addi r3, r31, 0x700
    mr r4, r29
    addi r4, r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    bne fn_801508CC
    mtlr r30
    blr
}

asm void fn_801509C8(void)
{
    nofralloc
    mflr r30
    li r6, 0x704
    lwbrx r3, r6, r31
    addi r4, r31, 0x708
    li r6, 0x702
    lhbrx r5, r6, r31
    lwz r7, 0x390(r31)
    andis. r7, r7, 0x1000
    bne L_80150A1C
    lis r7, 0xe000
    cmplw r4, r7
    blt L_80150A1C
    ori r7, r7, 0x3fff
    cmplw r4, r7
    bgt L_80150A1C
    li r6, 0x702
    li r4, 0x0
    sthbrx r4, r6, r31
    addi r3, r31, 0x700
    li r4, 0x8
    b L_80150A4C
L_80150A1C:
    mr r28, r3
    mr r29, r5
    bl fn_801503A0
    cmplwi r3, 0x1
    beq L_80150A30
L_80150A30:
    mr r3, r28
    mr r4, r29
    bl DCFlushRange
    mr r3, r28
    mr r4, r29
    bl ICInvalidateRange
    addi r3, r31, 0x700
L_80150A4C:
    li r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    mtlr r30
    blr
}

asm void fn_80150A64(void)
{
    nofralloc
    mflr r30
    li r6, 0x702
    lis r3, 0x0
    ori r3, r3, 0x8000
    lhbrx r5, r6, r31
    cmplw r5, r3
    ble L_80150A88
    mr r5, r3
    stwbrx r5, r6, r31
L_80150A88:
    mr r29, r5
    addi r3, r31, 0x708
    li r6, 0x704
    lwbrx r4, r6, r31
    lwz r7, 0x390(r31)
    andis. r7, r7, 0x1000
    bne L_80150B00
    lis r7, 0xe000
    cmplw r4, r7
    blt L_80150AD4
    ori r7, r7, 0x3fff
    cmplw r4, r7
    bgt L_80150AD4
    li r6, 0x702
    li r4, 0x0
    sthbrx r4, r6, r31
    addi r3, r31, 0x700
    li r4, 0x8
    b L_80150B38
L_80150AD4:
    bl fn_801503A0
    cmplwi r3, 0x1
    beq L_80150B2C
    addi r6, r31, 0x708
    mr r3, r29
    li r5, 0xaa
L_80150AEC:
    subi r3, r3, 0x1
    stbx r5, r3, r6
    cmpwi r3, 0x0
    bgt L_80150AEC
    b L_80150B2C
L_80150B00:
    mr r28, r4
    bl fn_801503A0
    lis r7, 0xe000
    cmplw r4, r7
    blt L_80150B20
    ori r7, r7, 0x3fff
    cmplw r4, r7
    ble L_80150B2C
L_80150B20:
    mr r3, r28
    mr r4, r29
    bl DCFlushRange
L_80150B2C:
    addi r3, r31, 0x700
    mr r4, r29
    addi r4, r4, 0x8
L_80150B38:
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    bne fn_801508CC
    mtlr r30
    blr
}

asm void fn_80150B54(void)
{
    nofralloc
    mflr r30
    lwz r3, 0x6bc(r31)
    stw r3, 0x708(r31)
    lwz r5, 0x6d4(r31)
    li r6, 0x704
    stwbrx r5, r6, r31
    li r4, 0x4
    li r6, 0x702
    sthbrx r4, r6, r31
    addi r3, r31, 0x700
    addi r4, r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    bne fn_801508CC
    mtlr r30
    blr
}

asm void fn_80150B9C(void)
{
    nofralloc
    mflr r30
    li r4, 0x0
    addi r5, r31, 0x70c
    lis r6, 0x8000
    lwz r6, 0x30c8(r6)
    cmpwi r6, 0x0
    beq L_80150C00
    li r7, 0x0
    li r8, 0x4
    li r9, 0x8
    li r10, 0xc
L_80150BC8:
    addi r4, r4, 0x1
    stwbrx r6, r7, r5
    lwz r3, 0x0(r6)
    stwbrx r3, r8, r5
    lwz r3, 0x14(r6)
    stwbrx r3, r9, r5
    lwz r3, 0x18(r6)
    stwbrx r3, r10, r5
    addi r5, r5, 0x10
    cmpwi r4, 0x40
    beq L_80150C00
    lwz r6, 0x4(r6)
    cmpwi r6, 0x0
    bne L_80150BC8
L_80150C00:
    addi r3, r31, 0x708
    li r6, 0x0
    stwbrx r4, r6, r3
    li r6, 0x702
    subf r4, r3, r5
    stwbrx r4, r6, r31
    addi r4, r4, 0x8
    addi r3, r31, 0x700
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    bne fn_801508CC
    mtlr r30
    blr
}

asm void fn_80150C3C(void)
{
    nofralloc
    mflr r30
    li r4, 0x0
    addi r5, r31, 0x70c
    lis r6, 0x8000
    lwz r6, 0xdc(r6)
    cmpwi r6, 0x0
    beq L_80150D28
    li r7, 0x0
    li r8, 0x4
    li r9, 0x8
    li r10, 0xc
    li r11, 0x10
    li r12, 0x14
    li r14, 0x18
    li r15, 0x1c
    li r16, 0x20
L_80150C7C:
    addi r4, r4, 0x1
    stwbrx r6, r7, r5
    lwz r3, 0x2c8(r6)
    stwbrx r3, r8, r5
    lwz r3, 0x2cc(r6)
    stwbrx r3, r9, r5
    lwz r3, 0x2d0(r6)
    stwbrx r3, r10, r5
    lwz r3, 0x2d4(r6)
    stwbrx r3, r11, r5
    lwz r3, 0x2d8(r6)
    stwbrx r3, r12, r5
    lwz r3, 0x2f0(r6)
    stwbrx r3, r14, r5
    li r18, 0x0
    lwz r3, 0x2e8(r6)
    addi r17, r5, 0x24
L_80150CC0:
    cmpwi r3, 0x0
    beq L_80150CD8
    stwbrx r3, r18, r17
    addi r18, r18, 0x4
    lwz r3, 0x2e0(r3)
    b L_80150CC0
L_80150CD8:
    add r17, r17, r18
    srwi r18, r18, 2
    stwbrx r18, r15, r5
    li r18, 0x0
    lwz r3, 0x2f4(r6)
L_80150CEC:
    cmpwi r3, 0x0
    beq L_80150D04
    stwbrx r3, r18, r17
    addi r18, r18, 0x4
    lwz r3, 0x10(r3)
    b L_80150CEC
L_80150D04:
    add r17, r17, r18
    srwi r18, r18, 2
    stwbrx r18, r16, r5
    mr r5, r17
    cmpwi r4, 0x40
    beq L_80150D28
    lwz r6, 0x2fc(r6)
    cmpwi r6, 0x0
    bne L_80150C7C
L_80150D28:
    addi r3, r31, 0x708
    li r6, 0x0
    stwbrx r4, r6, r3
    li r6, 0x702
    subf r4, r3, r5
    sthbrx r4, r6, r31
    addi r4, r4, 0x8
    addi r3, r31, 0x700
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    bne fn_801508CC
    mtlr r30
    blr
}

asm void fn_80150D64(void)
{
    nofralloc
    lis r5, 0xcc00
    li r3, 0x4cc8
    li r4, 0x3
    stw r4, 0x3024(r5)
    stw r3, 0x3024(r5)
    nop
    b fn_80150D64
}

asm void fn_80150D80(void)
{
    nofralloc
    lis r4, lbl_801E19DC@h
    ori r4, r4, lbl_801E19DC@l
    li r5, 0x0
    stw r5, 0x0(r4)
    blr
}

asm void fn_80150D94(void)
{
    nofralloc
    mflr r30
    lbz r3, 0x707(r31)
    lis r4, 0x0
    ori r4, r4, 0x704
    lhbrx r4, r4, r31
    lwz r5, 0x708(r31)
    cmplwi r3, 0xff
    beq L_80150DB4
L_80150DB4:
    mtlr r30
    blr
}

asm void fn_80150DBC(void)
{
    nofralloc
    lis r3, 0xcc00
    ori r3, r3, 0x6000
    lwz r4, 0x0(r3)
    andi. r4, r4, 0x14
    li r4, 0x0
    blr
}

asm void fn_80150DD4(void)
{
    nofralloc
    lwz r4, 0x0(r3)
    andi. r4, r4, 0x14
    beq fn_80150DD4
    stw r4, 0x0(r3)
    blr
}

asm void fn_80150DE8(void)
{
    nofralloc
    mflr r30
    bl fn_80150DBC
    bne L_80150E48
    lis r3, lbl_802F3480@h
    ori r3, r3, lbl_802F3480@l
    li r4, 0x20
    bl DCInvalidateRange
    lis r3, 0xcc00
    ori r3, r3, 0x6000
    li r4, 0x14
    stw r4, 0x0(r3)
    lis r4, 0x1200
    stw r4, 0x8(r3)
    li r4, 0x0
    stw r4, 0xc(r3)
    stw r4, 0x10(r3)
    lis r4, lbl_802F3480@h
    ori r4, r4, lbl_802F3480@l
    stw r4, 0x14(r3)
    li r4, 0x20
    stw r4, 0x18(r3)
    li r4, 0x3
    stw r4, 0x1c(r3)
    bl fn_80150DD4
L_80150E48:
    addi r3, r31, 0x708
    stw r4, 0x0(r3)
    lis r4, 0x8000
    ori r4, r4, 0x24
    lwzu r5, 0x4(r4)
    stwu r5, 0x4(r3)
    lwzu r5, 0x4(r4)
    stwu r5, 0x4(r3)
    addi r3, r3, 0x4
    lis r4, lbl_802F3480@h
    ori r4, r4, lbl_802F3480@l
    li r5, 0x20
    bl fn_801503A0
    li r4, 0x2c
    li r5, 0x702
    sthbrx r4, r5, r31
    addi r3, r31, 0x700
    addi r4, r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    bne fn_801508CC
    mtlr r30
    blr
}

asm void fn_80150EAC(void)
{
    nofralloc
    mflr r30
    lbz r3, 0x701(r31)
    bl SNDVDEmuControl
    sth r3, 0x708(r31)
    clrrwi. r3, r3, 31
    beq L_80150ED4
    mfspr r4, 1013
    stw r4, 0x450(r31)
    li r3, 0x0
    mtspr 1013, r3
L_80150ED4:
    li r4, 0x2
    li r5, 0x702
    sthbrx r4, r5, r31
    addi r3, r31, 0x700
    addi r4, r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    bne fn_801508CC
    mtlr r30
    blr
}

asm void lbl_80150F04(void)
{
    nofralloc
    li r6, 0x704
    lwbrx r3, r6, r31
    stw r3, 0x370(r31)
    mfmsr r3
    rlwinm r3, r3, 0, 17, 15
    mtmsr r3
    lis r5, 0xcc00
    li r4, 0x0
    stw r4, 0x3004(r5)
    li r4, 0x0
    lis r3, 0xcc00
    sth r4, 0x2002(r3)
}

asm void fn_80150F34(void)
{
    nofralloc
    lwz r4, 0x6cc(r31)
    cmpwi r4, 0x3
    bne lbl_80150F4C
    lis r5, 0xcc00
    li r4, 0x1000
    stw r4, 0x3000(r5)
    entry lbl_80150F4C
    li r5, 0x0
    stw r5, 0x6bc(r31)
    bl fn_8015114C
    bl fn_80151190
    lwz r5, 0x360(r31)
    mtcrf 255, r5
    lwz r5, 0x358(r31)
    mtlr r5
    lwz r5, 0x6c8(r31)
    lis r4, 0xcc00
    sth r5, 0x4010(r4)
    lis r5, lbl_801E19C0@h
    ori r5, r5, lbl_801E19C0@l
    li r4, 0x0
    stw r4, 0x0(r5)
    lwz r30, 0xf0(r31)
    lwz r29, 0xe8(r31)
    lwz r28, 0xe0(r31)
    lwz r27, 0xd8(r31)
    lwz r26, 0xd0(r31)
    lwz r25, 0xc8(r31)
    lwz r24, 0xc0(r31)
    lwz r23, 0xb8(r31)
    lwz r22, 0xb0(r31)
    lwz r21, 0xa8(r31)
    lwz r20, 0xa0(r31)
    lwz r19, 0x98(r31)
    lwz r18, 0x90(r31)
    lwz r17, 0x88(r31)
    lwz r16, 0x80(r31)
    lwz r15, 0x78(r31)
    lwz r14, 0x70(r31)
    lwz r13, 0x68(r31)
    lwz r12, 0x60(r31)
    lwz r11, 0x58(r31)
    lwz r10, 0x50(r31)
    lwz r9, 0x48(r31)
    lwz r8, 0x40(r31)
    lwz r7, 0x38(r31)
    lwz r6, 0x30(r31)
    lwz r5, 0x28(r31)
    lwz r4, 0x20(r31)
    lwz r2, 0x10(r31)
    lwz r3, 0x18(r31)
    lwz r1, 0x8(r31)
    lwz r0, 0x0(r31)
    lwz r31, 0xf8(r31)
    rfi
}

asm void fn_8015100C(void)
{
    nofralloc
    lwz r5, 0x20(r31)
    cmpwi r5, 0x0
    beq L_801510A4
    lis r3, lbl_801E19D4@h
    ori r3, r3, lbl_801E19D4@l
    lwz r3, 0x0(r3)
    cmpwi r3, 0x0
    beq L_8015107C
    cmpwi r5, 0x1
    bne L_8015107C
    lwz r3, 0x18(r31)
    lbz r3, 0x0(r3)
    cmpwi r3, 0xa
    beq L_8015107C
    cmpwi r3, 0xd
    beq L_8015107C
    lbz r5, 0x6d8(r31)
    add r4, r5, r31
    stb r3, 0x6d9(r4)
    addi r5, r5, 0x1
    stb r5, 0x6d8(r31)
    cmpwi r5, 0x10
    bne L_801510A4
    addi r4, r31, 0x6d9
    bl lbl_8015110C
    li r5, 0x0
    stb r5, 0x6d8(r31)
    b L_801510A4
L_8015107C:
    lbz r5, 0x6d8(r31)
    cmpwi r5, 0x0
    beq L_80151098
    addi r4, r31, 0x6d9
    bl lbl_8015110C
    li r5, 0x0
    stb r5, 0x6d8(r31)
L_80151098:
    lwz r4, 0x18(r31)
    lwz r5, 0x20(r31)
    bl lbl_8015110C
L_801510A4:
    lwz r3, 0x370(r31)
    addi r3, r3, 0x4
    stw r3, 0x370(r31)
    li r3, 0x0
    stw r3, 0x18(r31)
    b fn_80150F34
}

asm void fn_801510BC(void)
{
    nofralloc
    lbz r5, 0x6d8(r31)
    cmpwi r5, 0x0
    beq L_801510D8
    addi r4, r31, 0x6d9
    bl lbl_8015110C
    li r5, 0x0
    stb r5, 0x6d8(r31)
L_801510D8:
    lwz r3, 0x370(r31)
    addi r3, r3, 0x4
    stw r3, 0x370(r31)
    b fn_80150F34
}

asm void fn_801510E8(void)
{
    nofralloc
    mr r5, r3
    lbz r0, 0x0(r5)
    cmpwi r0, 0x0
    beq L_80151104
L_801510F8:
    lbzu r0, 0x1(r5)
    cmpwi r0, 0x0
    bne L_801510F8
L_80151104:
    subf r5, r3, r5
    mr r4, r3
    entry lbl_8015110C
    addi r3, r31, 0x708
    mr r28, r5
    mflr r29
    bl fn_801503A0
    mr r4, r28
    addi r3, r31, 0x700
    li r6, 0xd
    stb r6, 0x0(r3)
    li r6, 0x2
    sthbrx r4, r6, r3
    addi r4, r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    mtlr r29
    blr
}

asm void fn_8015114C(void)
{
    nofralloc
    lwz r5, 0x390(r31)
    mtspr 920, r5
    lwz r5, 0x388(r31)
    mtspr 1009, r5
    lwz r5, 0x380(r31)
    mtspr 1008, r5
    lwz r5, 0x450(r31)
    mtspr 1013, r5
    lwz r5, 0x350(r31)
    mtctr r5
    lwz r5, 0x348(r31)
    mtxer r5
    lwz r5, 0x370(r31)
    mtsrr0 r5
    lwz r5, 0x378(r31)
    mtsrr1 r5
    blr
}

asm void fn_80151190(void)
{
    nofralloc
    mfmsr r5
    ori r5, r5, 0x900
    xori r5, r5, 0x900
    ori r5, r5, 0x2000
    mtmsr r5
    isync
    lfd fp0, 0x200(r31)
    mtfsf 255, fp0
    mfspr r5, 920
    isync
    extrwi. r5, r5, 1, 2
    beq L_8015128C
    li r5, 0x0
    mtspr 912, r5
    isync
    psq_l fp0, 0x248(r31), 0, 0
    psq_l fp1, 0x250(r31), 0, 0
    psq_l fp2, 0x258(r31), 0, 0
    psq_l fp3, 0x260(r31), 0, 0
    psq_l fp4, 0x268(r31), 0, 0
    psq_l fp5, 0x270(r31), 0, 0
    psq_l fp6, 0x278(r31), 0, 0
    psq_l fp7, 0x280(r31), 0, 0
    psq_l fp8, 0x288(r31), 0, 0
    psq_l fp9, 0x290(r31), 0, 0
    psq_l fp10, 0x298(r31), 0, 0
    psq_l fp11, 0x2a0(r31), 0, 0
    psq_l fp12, 0x2a8(r31), 0, 0
    psq_l fp13, 0x2b0(r31), 0, 0
    psq_l fp14, 0x2b8(r31), 0, 0
    psq_l fp15, 0x2c0(r31), 0, 0
    psq_l fp16, 0x2c8(r31), 0, 0
    psq_l fp17, 0x2d0(r31), 0, 0
    psq_l fp18, 0x2d8(r31), 0, 0
    psq_l fp19, 0x2e0(r31), 0, 0
    psq_l fp20, 0x2e8(r31), 0, 0
    psq_l fp21, 0x2f0(r31), 0, 0
    psq_l fp22, 0x2f8(r31), 0, 0
    psq_l fp23, 0x300(r31), 0, 0
    psq_l fp24, 0x308(r31), 0, 0
    psq_l fp25, 0x310(r31), 0, 0
    psq_l fp26, 0x318(r31), 0, 0
    psq_l fp27, 0x320(r31), 0, 0
    psq_l fp28, 0x328(r31), 0, 0
    psq_l fp29, 0x330(r31), 0, 0
    psq_l fp30, 0x338(r31), 0, 0
    psq_l fp31, 0x340(r31), 0, 0
    lwz r5, 0x208(r31)
    mtspr 912, r5
    lwz r5, 0x210(r31)
    mtspr 913, r5
    lwz r5, 0x218(r31)
    mtspr 914, r5
    lwz r5, 0x220(r31)
    mtspr 915, r5
    lwz r5, 0x228(r31)
    mtspr 916, r5
    lwz r5, 0x230(r31)
    mtspr 917, r5
    lwz r5, 0x238(r31)
    mtspr 918, r5
    lwz r5, 0x240(r31)
    mtspr 919, r5
L_8015128C:
    lfd fp0, 0x100(r31)
    lfd fp1, 0x108(r31)
    lfd fp2, 0x110(r31)
    lfd fp3, 0x118(r31)
    lfd fp4, 0x120(r31)
    lfd fp5, 0x128(r31)
    lfd fp6, 0x130(r31)
    lfd fp7, 0x138(r31)
    lfd fp8, 0x140(r31)
    lfd fp9, 0x148(r31)
    lfd fp10, 0x150(r31)
    lfd fp11, 0x158(r31)
    lfd fp12, 0x160(r31)
    lfd fp13, 0x168(r31)
    lfd fp14, 0x170(r31)
    lfd fp15, 0x178(r31)
    lfd fp16, 0x180(r31)
    lfd fp17, 0x188(r31)
    lfd fp18, 0x190(r31)
    lfd fp19, 0x198(r31)
    lfd fp20, 0x1a0(r31)
    lfd fp21, 0x1a8(r31)
    lfd fp22, 0x1b0(r31)
    lfd fp23, 0x1b8(r31)
    lfd fp24, 0x1c0(r31)
    lfd fp25, 0x1c8(r31)
    lfd fp26, 0x1d0(r31)
    lfd fp27, 0x1d8(r31)
    lfd fp28, 0x1e0(r31)
    lfd fp29, 0x1e8(r31)
    lfd fp30, 0x1f0(r31)
    lfd fp31, 0x1f8(r31)
    lis r5, 0x8000
    lwz r3, 0xd4(r5)
    lwz r30, 0xd8(r5)
    cmpw r3, r30
    beq L_8015134C
    mflr r29
    bl lbl_80151F0C
    cmpwi r30, 0x0
    beq L_80151348
    mr r3, r30
    lhz r30, 0x1a2(r3)
    ori r4, r30, 0x1
    sth r4, 0x1a2(r3)
    bl lbl_80151DE4
    sth r30, 0x1a2(r3)
L_80151348:
    mtlr r29
L_8015134C:
    blr
}

asm void fn_80151350(void)
{
    nofralloc
    mfmsr r5
    ori r5, r5, 0x900
    xori r5, r5, 0x900
    ori r5, r5, 0x2000
    mtmsr r5
    isync
    lis r5, 0x8000
    lwz r30, 0xd4(r5)
    lwz r3, 0xd8(r5)
    cmpw r3, r30
    beq L_801513A8
    mflr r29
    cmpwi r3, 0x0
    beq L_8015138C
    bl lbl_80151F0C
L_8015138C:
    mr r3, r30
    lhz r30, 0x1a2(r3)
    ori r4, r30, 0x1
    sth r4, 0x1a2(r3)
    bl lbl_80151DE4
    sth r30, 0x1a2(r3)
    mtlr r29
L_801513A8:
    stfd fp0, 0x100(r31)
    stfd fp1, 0x108(r31)
    stfd fp2, 0x110(r31)
    stfd fp3, 0x118(r31)
    stfd fp4, 0x120(r31)
    stfd fp5, 0x128(r31)
    stfd fp6, 0x130(r31)
    stfd fp7, 0x138(r31)
    stfd fp8, 0x140(r31)
    stfd fp9, 0x148(r31)
    stfd fp10, 0x150(r31)
    stfd fp11, 0x158(r31)
    stfd fp12, 0x160(r31)
    stfd fp13, 0x168(r31)
    stfd fp14, 0x170(r31)
    stfd fp15, 0x178(r31)
    stfd fp16, 0x180(r31)
    stfd fp17, 0x188(r31)
    stfd fp18, 0x190(r31)
    stfd fp19, 0x198(r31)
    stfd fp20, 0x1a0(r31)
    stfd fp21, 0x1a8(r31)
    stfd fp22, 0x1b0(r31)
    stfd fp23, 0x1b8(r31)
    stfd fp24, 0x1c0(r31)
    stfd fp25, 0x1c8(r31)
    stfd fp26, 0x1d0(r31)
    stfd fp27, 0x1d8(r31)
    stfd fp28, 0x1e0(r31)
    stfd fp29, 0x1e8(r31)
    stfd fp30, 0x1f0(r31)
    stfd fp31, 0x1f8(r31)
    mfspr r5, 920
    isync
    extrwi. r5, r5, 1, 2
    beq L_80151504
    mfspr r5, 912
    stw r5, 0x208(r31)
    mfspr r5, 913
    stw r5, 0x210(r31)
    mfspr r5, 914
    stw r5, 0x218(r31)
    mfspr r5, 915
    stw r5, 0x220(r31)
    mfspr r5, 916
    stw r5, 0x228(r31)
    mfspr r5, 917
    stw r5, 0x230(r31)
    mfspr r5, 918
    stw r5, 0x238(r31)
    mfspr r5, 919
    stw r5, 0x240(r31)
    li r5, 0x0
    mtspr 912, r5
    isync
    psq_st fp0, 0x248(r31), 0, 0
    psq_st fp1, 0x250(r31), 0, 0
    psq_st fp2, 0x258(r31), 0, 0
    psq_st fp3, 0x260(r31), 0, 0
    psq_st fp4, 0x268(r31), 0, 0
    psq_st fp5, 0x270(r31), 0, 0
    psq_st fp6, 0x278(r31), 0, 0
    psq_st fp7, 0x280(r31), 0, 0
    psq_st fp8, 0x288(r31), 0, 0
    psq_st fp9, 0x290(r31), 0, 0
    psq_st fp10, 0x298(r31), 0, 0
    psq_st fp11, 0x2a0(r31), 0, 0
    psq_st fp12, 0x2a8(r31), 0, 0
    psq_st fp13, 0x2b0(r31), 0, 0
    psq_st fp14, 0x2b8(r31), 0, 0
    psq_st fp15, 0x2c0(r31), 0, 0
    psq_st fp16, 0x2c8(r31), 0, 0
    psq_st fp17, 0x2d0(r31), 0, 0
    psq_st fp18, 0x2d8(r31), 0, 0
    psq_st fp19, 0x2e0(r31), 0, 0
    psq_st fp20, 0x2e8(r31), 0, 0
    psq_st fp21, 0x2f0(r31), 0, 0
    psq_st fp22, 0x2f8(r31), 0, 0
    psq_st fp23, 0x300(r31), 0, 0
    psq_st fp24, 0x308(r31), 0, 0
    psq_st fp25, 0x310(r31), 0, 0
    psq_st fp26, 0x318(r31), 0, 0
    psq_st fp27, 0x320(r31), 0, 0
    psq_st fp28, 0x328(r31), 0, 0
    psq_st fp29, 0x330(r31), 0, 0
    psq_st fp30, 0x338(r31), 0, 0
    psq_st fp31, 0x340(r31), 0, 0
L_80151504:
    mffs fp0
    stfd fp0, 0x200(r31)
    blr
}

asm void fn_80151510(void)
{
    nofralloc
    mfxer r5
    stw r5, 0x348(r31)
    mfctr r5
    stw r5, 0x350(r31)
    mfspr r5, 1008
    stw r5, 0x380(r31)
    mfspr r5, 1009
    stw r5, 0x388(r31)
    mfibatu r5, 0
    stw r5, 0x398(r31)
    mfibatl r5, 0
    stw r5, 0x3a0(r31)
    mfibatu r5, 1
    stw r5, 0x3a8(r31)
    mfibatl r5, 1
    stw r5, 0x3b0(r31)
    mfibatu r5, 2
    stw r5, 0x3b8(r31)
    mfibatl r5, 2
    stw r5, 0x3c0(r31)
    mfibatu r5, 3
    stw r5, 0x3c8(r31)
    mfibatl r5, 3
    stw r5, 0x3d0(r31)
    mfdbatu r5, 0
    stw r5, 0x3d8(r31)
    mfdbatl r5, 0
    stw r5, 0x3e0(r31)
    mfdbatu r5, 1
    stw r5, 0x3e8(r31)
    mfdbatl r5, 1
    stw r5, 0x3f0(r31)
    mfdbatu r5, 2
    stw r5, 0x3f8(r31)
    mfdbatl r5, 2
    stw r5, 0x400(r31)
    mfdbatu r5, 3
    stw r5, 0x408(r31)
    mfdbatl r5, 3
    stw r5, 0x410(r31)
    mfsr r5, 0
    stw r5, 0x500(r31)
    mfsr r5, 1
    stw r5, 0x508(r31)
    mfsr r5, 2
    stw r5, 0x510(r31)
    mfsr r5, 3
    stw r5, 0x518(r31)
    mfsr r5, 4
    stw r5, 0x520(r31)
    mfsr r5, 5
    stw r5, 0x528(r31)
    mfsr r5, 6
    stw r5, 0x530(r31)
    mfsr r5, 7
    stw r5, 0x538(r31)
    mfsr r5, 8
    stw r5, 0x540(r31)
    mfsr r5, 9
    stw r5, 0x548(r31)
    mfsr r5, 10
    stw r5, 0x550(r31)
    mfsr r5, 11
    stw r5, 0x558(r31)
    mfsr r5, 12
    stw r5, 0x560(r31)
    mfsr r5, 13
    stw r5, 0x568(r31)
    mfsr r5, 14
    stw r5, 0x570(r31)
    mfsr r5, 15
    stw r5, 0x578(r31)
    mfsprg r5, 0
    stw r5, 0x418(r31)
    mfsprg r5, 1
    stw r5, 0x420(r31)
    mfsprg r5, 2
    stw r5, 0x428(r31)
    mfsprg r5, 3
    stw r5, 0x430(r31)
    mfdar r5
    stw r5, 0x438(r31)
    mfdsisr r5
    stw r5, 0x440(r31)
    mfear r5
    stw r5, 0x448(r31)
    mfspr r5, 1013
    stw r5, 0x450(r31)
    mfspr r5, 284
    stw r5, 0x458(r31)
    mfspr r5, 285
    stw r5, 0x460(r31)
    mfspr r5, 1017
    stw r5, 0x468(r31)
    mfdec r5
    stw r5, 0x470(r31)
    mfspr r5, 1010
    stw r5, 0x478(r31)
    mfspr r5, 953
    stw r5, 0x480(r31)
    mfspr r5, 954
    stw r5, 0x488(r31)
    mfspr r5, 957
    stw r5, 0x490(r31)
    mfspr r5, 958
    stw r5, 0x498(r31)
    mfspr r5, 955
    stw r5, 0x4a0(r31)
    mfspr r5, 952
    stw r5, 0x4a8(r31)
    mfspr r5, 956
    stw r5, 0x4b0(r31)
    mfspr r5, 1020
    stw r5, 0x4b8(r31)
    mfspr r5, 1021
    stw r5, 0x4c0(r31)
    mfspr r5, 1022
    stw r5, 0x4c8(r31)
    mfspr r5, 1019
    stw r5, 0x4d0(r31)
    mfsdr1 r5
    stw r5, 0x4d8(r31)
    mfspr r5, 287
    stw r5, 0x4e0(r31)
    mfspr r5, 923
    stw r5, 0x4f0(r31)
    mfspr r5, 922
    stw r5, 0x4e8(r31)
    mfspr r5, 921
    stw r5, 0x4f8(r31)
    mfspr r5, 920
    stw r5, 0x390(r31)
    blr
}
