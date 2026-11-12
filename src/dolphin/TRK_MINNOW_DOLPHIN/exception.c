/* SN debugger vectors, exception context and transport assembly reconstructed
 * from GL5E4F. See docs/sn_runtime.md for entry boundaries and ABI evidence.
 * Vector zero words are embedded data copied with the handler templates.
 */

extern void DCFlushRange(void);
extern void DCInvalidateRange(void);
extern void ICInvalidateRange(void);
extern void OSDisableInterrupts(void);
extern void OSReport(void);
extern void OSRestoreInterrupts(void);
extern void PPCHalt(void);
extern void __OSMaskInterrupts(void);
extern void __OSSetInterruptHandler(void);
extern void __OSUnmaskInterrupts(void);
extern unsigned char _SDA2_BASE_[];
extern unsigned char _SDA_BASE_[];
extern unsigned char lbl_801E19C0[];
extern unsigned char lbl_801E19D4[];
extern unsigned char lbl_801E1AA0[];
extern unsigned char lbl_801E1AD5[];
extern unsigned char lbl_801E1B32[];
extern unsigned char lbl_802E9C28[];
extern unsigned char lbl_802EAC60[];
extern unsigned char lbl_802F3480[];
extern void fn_80150418(void);
extern void fn_801507E0(void);
extern void fn_80150F34(void);
extern void fn_8015100C(void);
extern void fn_801510BC(void);
extern void fn_801510E8(void);
extern void fn_80151350(void);
extern void fn_80151510(void);
extern void fn_80151724(void);
extern void fn_80151778(void);
extern void fn_80151828(void);
extern void fn_80151C90(void);
extern void fn_80151CA8(void);
extern void fn_80151CC4(void);
extern void fn_80151DD4(void);
extern void fn_80152038(void);
extern void fn_80152070(void);
extern void fn_80152410(void);
extern void fn_801525A8(void);
extern void fn_801526D4(void);
extern void fn_80152708(void);
extern void fn_80152710(void);
extern void fn_80152738(void);
extern void fn_80152740(void);
extern void fn_80152748(void);
extern void fn_80152750(void);
extern void fn_80152758(void);
extern void fn_80152CE0(void);
extern void fn_80152D34(void);
extern void fn_80152D4C(void);
extern void fn_80152D60(void);
extern void fn_80152D88(void);
extern void fn_80152D9C(void);
extern void fn_80152DDC(void);
extern void fn_80152E10(void);
extern void fn_80152E4C(void);
extern void fn_80152E7C(void);
extern void fn_80152ECC(void);
extern void fn_80152F3C(void);
extern void fn_80152FA4(void);
extern void fn_80153008(void);
extern void fn_80153078(void);
extern void fn_801530E0(void);
extern void lbl_8015175C(void);
extern void lbl_801517D8(void);
extern void lbl_8015180C(void);
extern void lbl_801518FC(void);
extern void lbl_80151AFC(void);
extern void lbl_80151D1C(void);
extern void lbl_80151DE4(void);
extern void lbl_80151F0C(void);
extern void lbl_8015247C(void);
extern void lbl_80152524(void);
extern void lbl_80152544(void);
extern void lbl_80152578(void);
extern void lbl_8015258C(void);
extern void lbl_80152610(void);
extern void lbl_80152700(void);
extern void lbl_80152718(void);
extern void lbl_80152720(void);
extern void lbl_80152728(void);
extern void lbl_80152730(void);
extern void lbl_80152760(void);
extern void lbl_801528B4(void);
extern void lbl_801528FC(void);
extern void lbl_80152984(void);
extern void lbl_80152B40(void);
extern void lbl_80152CD8(void);
extern void lbl_80152CDC(void);
extern void PCwriteAsyncInit(void);
extern void PCreadAsyncInit(void);
extern void CompleteAsync(void);
extern void lbl_802F3368(void);

asm void fn_80151724(void)
{
    nofralloc
    stw r31, 0xdf8(r0)
    mfsrr0 r31
    stw r31, 0xdf4(r0)
    mfsrr1 r31
    stw r31, 0xdf0(r0)
    mfmsr r31
    ori r31, r31, 0x30
    mtsrr1 r31
    lis r31, fn_80151828@h
    ori r31, r31, fn_80151828@l
    mtsrr0 r31
    mflr r31
    stw r31, 0xdec(r0)
    bl lbl_8015175C
    entry lbl_8015175C
    mflr r31
    stw r31, 0xde8(r0)
    rfi
    opword 0x00000000
    opword 0x00000000
    opword 0x00000000
    opword 0x00000000
}

asm void fn_80151778(void)
{
    nofralloc
    stw r31, 0xdf8(r0)
    mfsrr0 r31
    stw r31, 0xdf4(r0)
    mfcr r31
    mtsprg 0, r31
    mfxer r31
    mtsprg 1, r31
    mfsrr1 r31
    stw r31, 0xdf0(r0)
    extrwi r31, r31, 6, 10
    cmpwi r31, 0x10
    bne lbl_801517D8
    lis r31, lbl_801E19D4+0x4@h
    ori r31, r31, lbl_801E19D4+0x4@l
    clrlwi r31, r31, 4
    lwz r31, 0x0(r31)
    cmpwi r31, 0x0
    bne L_801517D8
    mfsprg r31, 0
    mtcrf 255, r31
    mfsprg r31, 1
    mtxer r31
    lwz r31, 0xdf8(r0)
    b lbl_802F3368
    entry lbl_801517D8
L_801517D8:
    mfsprg r31, 0
    mtcrf 255, r31
    mfsprg r31, 1
    mtxer r31
    mfmsr r31
    ori r31, r31, 0x30
    mtsrr1 r31
    lis r31, fn_80151828@h
    ori r31, r31, fn_80151828@l
    mtsrr0 r31
    mflr r31
    stw r31, 0xdec(r0)
    bl lbl_8015180C
    entry lbl_8015180C
    mflr r31
    stw r31, 0xde8(r0)
    rfi
    opword 0x00000000
    opword 0x00000000
    opword 0x00000000
    opword 0x00000000
}

asm void fn_80151828(void)
{
    nofralloc
    lis r31, lbl_802EAC60@h
    ori r31, r31, lbl_802EAC60@l
    stw r0, 0x0(r31)
    stw r1, 0x8(r31)
    stw r2, 0x10(r31)
    stw r3, 0x18(r31)
    stw r4, 0x20(r31)
    stw r5, 0x28(r31)
    stw r6, 0x30(r31)
    stw r7, 0x38(r31)
    stw r8, 0x40(r31)
    stw r9, 0x48(r31)
    stw r10, 0x50(r31)
    stw r11, 0x58(r31)
    stw r12, 0x60(r31)
    stw r13, 0x68(r31)
    stw r14, 0x70(r31)
    stw r15, 0x78(r31)
    stw r16, 0x80(r31)
    stw r17, 0x88(r31)
    stw r18, 0x90(r31)
    stw r19, 0x98(r31)
    stw r20, 0xa0(r31)
    stw r21, 0xa8(r31)
    stw r22, 0xb0(r31)
    stw r23, 0xb8(r31)
    stw r24, 0xc0(r31)
    stw r25, 0xc8(r31)
    stw r26, 0xd0(r31)
    stw r27, 0xd8(r31)
    stw r28, 0xe0(r31)
    stw r29, 0xe8(r31)
    stw r30, 0xf0(r31)
    lis r1, lbl_802E9C28+0x1018@h
    ori r1, r1, lbl_802E9C28+0x1018@l
    mfcr r5
    stw r5, 0x360(r31)
    mfmsr r5
    stw r5, 0x368(r31)
    bl fn_80151510
    bl fn_80151350
    mfmsr r9
    xori r0, r9, 0x10
    mtmsr r0
    sync
    lwz r4, 0xdf8(r0)
    lwz r5, 0xdec(r0)
    lwz r6, 0xdf4(r0)
    lwz r7, 0xdf0(r0)
    lwz r8, 0xde8(r0)
    srwi r8, r8, 8
    mtmsr r9
    sync
    entry lbl_801518FC
    lis r2, _SDA2_BASE_@h
    ori r2, r2, _SDA2_BASE_@l
    lis r13, _SDA_BASE_@h
    ori r13, r13, _SDA_BASE_@l
    stw r4, 0xf8(r31)
    stw r5, 0x358(r31)
    stw r6, 0x370(r31)
    ori r7, r7, 0x600
    xori r7, r7, 0x600
    stw r7, 0x378(r31)
    stw r8, 0x580(r31)
    lis r4, 0xcc00
    lhz r5, 0x4010(r4)
    stw r5, 0x6c8(r31)
    li r5, 0xff
    sth r5, 0x4010(r4)
    li r4, 0x0
    mtspr 1013, r4
    cmpwi r8, 0xd
    bne L_8015199C
    lwz r4, 0x6f0(r31)
    cmpwi r4, 0x0
    beq L_8015196C
    li r4, 0x0
    stw r4, 0x6f0(r31)
    lwz r4, 0x6f4(r31)
    stw r4, 0x450(r31)
    b fn_80150F34
L_8015196C:
    lwz r4, 0x6c0(r31)
    cmpwi r4, 0x0
    beq L_80151AC4
    lwz r4, 0x6c4(r31)
    add r5, r31, r4
    lwz r7, 0x0(r5)
    ori r7, r7, 0x600
    xori r7, r7, 0x600
    stw r7, 0x0(r5)
    li r4, 0x0
    stw r4, 0x6c0(r31)
    b L_80151AC4
L_8015199C:
    bl CompleteAsync
    lwz r8, 0x580(r31)
    cmpwi r8, 0x7
    bne L_80151A6C
    lwz r4, 0x370(r31)
    lwz r3, 0x0(r4)
    cmpwi r3, 0x5f
    bgt L_80151ABC
    cmpwi r3, 0x50
    bge L_80151ABC
    cmpwi r3, 0x1
    bne L_80151A00
    li r5, 0x1e
    stw r5, 0x580(r31)
    addi r4, r4, 0x4
    stw r4, 0x370(r31)
    lis r3, lbl_801E1AD5+0x20@h
    ori r3, r3, lbl_801E1AD5+0x20@l
    lwz r4, 0x6d0(r31)
    cmpwi r4, 0x0
    bne L_801519F8
    bl OSReport
    b L_801519FC
L_801519F8:
    bl fn_801510E8
L_801519FC:
    b L_80151ABC
L_80151A00:
    cmpwi r3, 0x4
    beq fn_801510BC
    cmpwi r3, 0x3
    beq fn_8015100C
    cmpwi r3, 0x2
    bne L_80151A40
    addi r4, r4, 0x4
    stw r4, 0x370(r31)
    lwz r0, 0x69c(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    beq fn_80150F34
    li r4, 0x1
    stw r4, 0x6bc(r31)
    b L_80151AC4
L_80151A40:
    cmpwi r3, 0x10
    blt L_80151ABC
    cmpwi r3, 0x16
    ble L_80151A64
    lwz r4, 0x6cc(r31)
    cmpwi r4, 0x4
    bne L_80151ABC
    cmpwi r3, 0x1b
    bgt L_80151ABC
L_80151A64:
    andi. r3, r3, 0xf
    b fn_80152410
L_80151A6C:
    cmpwi r8, 0x3
    bne L_80151ABC
    lwz r5, 0x440(r31)
    extrwi. r5, r5, 1, 9
    beq L_80151ABC
    bl fn_80152070
    cmpwi r3, 0x0
    beq L_80151AB4
    lwz r3, 0x378(r31)
    ori r3, r3, 0x400
    stw r3, 0x378(r31)
    lwz r3, 0x450(r31)
    stw r3, 0x6f4(r31)
    li r3, 0x0
    stw r3, 0x450(r31)
    li r3, 0x1
    stw r3, 0x6f0(r31)
    b fn_80150F34
L_80151AB4:
    li r5, 0x1f
    stw r5, 0x580(r31)
L_80151ABC:
    li r4, 0x0
    stw r4, 0x6bc(r31)
L_80151AC4:
    lis r5, lbl_801E19C0@h
    ori r5, r5, lbl_801E19C0@l
    li r4, 0x1
    stw r4, 0x0(r5)
    lwz r4, 0x6bc(r31)
    cmpwi r4, 0x0
    bne fn_801507E0
    lis r3, lbl_801E1AA0@h
    ori r3, r3, lbl_801E1AA0@l
    li r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    b fn_801507E0
}

asm void lbl_80151AFC(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x8(r1)
    lis r5, 0x8000
    stw r4, 0x44(r5)
    li r0, 0x1
    stw r0, 0x40(r5)
    rlwinm. r4, r3, 0, 29, 29
    beq L_80151B50
    lis r6, fn_80151CA8@h
    ori r6, r6, fn_80151CA8@l
    lis r4, 0x8000
    ori r4, r4, 0x300
    lis r5, 0x6000
    stw r5, 0x0(r4)
    addi r5, r4, 0x4
    subf r6, r5, r6
    lis r5, 0x4800
    or r6, r6, r5
    stw r6, 0x4(r4)
    rlwinm r3, r3, 0, 30, 28
L_80151B50:
    lis r5, 0x8000
    ori r5, r5, 0x100
    li r6, 0x1
L_80151B5C:
    and. r4, r3, r6
    beq L_80151B84
    lis r7, fn_80151510+0x210@h
    ori r7, r7, fn_80151510+0x210@l
    subi r8, r5, 0x4
    addi r9, r7, 0x54
L_80151B74:
    lwzu r4, 0x4(r7)
    stwu r4, 0x4(r8)
    cmplw r7, r9
    blt L_80151B74
L_80151B84:
    addi r5, r5, 0x100
    slwi. r6, r6, 1
    bne L_80151B5C
    lis r7, fn_80151724+0x50@h
    ori r7, r7, fn_80151724+0x50@l
    lis r8, 0x8000
    ori r8, r8, 0x6fc
    addi r9, r7, 0xb0
L_80151BA4:
    lwzu r4, 0x4(r7)
    stwu r4, 0x4(r8)
    cmplw r7, r9
    blt L_80151BA4
    lis r7, 0x8000
    ori r7, r7, 0x4fc
    lis r8, lbl_802EAC60+0x8704@h
    ori r8, r8, lbl_802EAC60+0x8704@l
    addi r9, r7, 0x104
L_80151BC8:
    lwzu r4, 0x4(r7)
    stwu r4, 0x4(r8)
    cmplw r7, r9
    blt L_80151BC8
    lis r3, 0x3860
    ori r3, r3, 0x4
    lis r4, lbl_802EAC60+0x8704@h
    ori r4, r4, lbl_802EAC60+0x8704@l
    addi r5, r4, 0x104
L_80151BEC:
    lwzu r6, 0x4(r4)
    cmplw r3, r6
    beq L_80151C24
    cmplw r4, r5
    blt L_80151BEC
    lis r3, lbl_801E1AD5+0x37@h
    ori r3, r3, lbl_801E1AD5+0x37@l
    lwz r4, 0x6d0(r31)
    cmpwi r4, 0x0
    bne L_80151C1C
    bl OSReport
    b PPCHalt
L_80151C1C:
    bl fn_801510E8
    b PPCHalt
L_80151C24:
    subi r4, r4, 0x4
    lis r6, 0x3860
    ori r6, r6, 0x6
    stwu r6, 0x4(r4)
    lis r3, 0x8000
    ori r3, r3, 0x700
    lwz r4, 0x5c(r3)
    lis r5, fn_80151778@h
    ori r5, r5, fn_80151778@l
    subf r5, r3, r5
    add r4, r4, r5
    stw r4, 0x5c(r3)
    lis r3, 0x8000
    ori r3, r3, 0x100
    li r4, 0x2000
    bl DCFlushRange
    lis r3, 0x8000
    ori r3, r3, 0x100
    li r4, 0x2000
    bl ICInvalidateRange
    li r3, 0x0
    li r4, 0x2000
    bl ICInvalidateRange
    addi r1, r1, 0x8
    lwz r0, 0x4(r1)
    mtlr r0
    blr
}

asm void fn_80151C90(void)
{
    nofralloc
    mtsprg 1, r31
    li r31, 0x4
    mtsprg 2, r31
    lis r31, lbl_802EAC60@h
    ori r31, r31, lbl_802EAC60@l
    b lbl_80151D1C
}

asm void fn_80151CA8(void)
{
    nofralloc
    mtsprg 1, r31
    mfsrr0 r31
    mtsprg 0, r31
    lis r31, fn_80151CC4@h
    ori r31, r31, fn_80151CC4@l
    mtsrr0 r31
    rfi
}

asm void fn_80151CC4(void)
{
    nofralloc
    mfcr r31
    mtsprg 3, r31
    lis r31, lbl_802EAC60@h
    ori r31, r31, lbl_802EAC60@l
    lwz r31, 0x6ec(r31)
    cmplwi r31, 0x0
    beq L_80151CFC
    mfsprg r31, 3
    mtcrf 255, r31
    lis r31, fn_80150418@h
    ori r31, r31, fn_80150418@l
    mtsrr0 r31
    mfsprg r31, 1
    rfi
L_80151CFC:
    mfsprg r31, 3
    mtcrf 255, r31
    mfsprg r31, 0
    mtsrr0 r31
    li r31, 0x3
    mtsprg 2, r31
    lis r31, lbl_802EAC60@h
    ori r31, r31, lbl_802EAC60@l
    entry lbl_80151D1C
    stw r0, 0x0(r31)
    stw r1, 0x8(r31)
    stw r2, 0x10(r31)
    stw r3, 0x18(r31)
    stw r4, 0x20(r31)
    stw r5, 0x28(r31)
    stw r6, 0x30(r31)
    stw r7, 0x38(r31)
    stw r8, 0x40(r31)
    stw r9, 0x48(r31)
    stw r10, 0x50(r31)
    stw r11, 0x58(r31)
    stw r12, 0x60(r31)
    stw r13, 0x68(r31)
    stw r14, 0x70(r31)
    stw r15, 0x78(r31)
    stw r16, 0x80(r31)
    stw r17, 0x88(r31)
    stw r18, 0x90(r31)
    stw r19, 0x98(r31)
    stw r20, 0xa0(r31)
    stw r21, 0xa8(r31)
    stw r22, 0xb0(r31)
    stw r23, 0xb8(r31)
    stw r24, 0xc0(r31)
    stw r25, 0xc8(r31)
    stw r26, 0xd0(r31)
    stw r27, 0xd8(r31)
    stw r28, 0xe0(r31)
    stw r29, 0xe8(r31)
    stw r30, 0xf0(r31)
    lis r1, lbl_802E9C28+0x1018@h
    ori r1, r1, lbl_802E9C28+0x1018@l
    mflr r28
    mfcr r5
    stw r5, 0x360(r31)
    mfmsr r5
    stw r5, 0x368(r31)
    bl fn_80151510
    bl fn_80151350
    mfsprg r4, 1
    mr r5, r28
    mfsrr0 r6
    mfsrr1 r7
    mfsprg r8, 2
    b lbl_801518FC
}

asm void fn_80151DD4(void)
{
    nofralloc
    lis r3, lbl_802EAC60@h
    ori r3, r3, lbl_802EAC60@l
    lwz r3, 0x6d0(r3)
    blr
}

asm void lbl_80151DE4(void)
{
    nofralloc
    mr r4, r3
    lhz r5, 0x1a2(r4)
    clrlwi. r5, r5, 31
    beq L_80151F08
    lfd fp0, 0x190(r4)
    mtfsf 255, fp0
    mfspr r5, 920
    extrwi. r5, r5, 1, 2
    beq L_80151E88
    psq_l fp0, 0x1c8(r4), 0, 0
    psq_l fp1, 0x1d0(r4), 0, 0
    psq_l fp2, 0x1d8(r4), 0, 0
    psq_l fp3, 0x1e0(r4), 0, 0
    psq_l fp4, 0x1e8(r4), 0, 0
    psq_l fp5, 0x1f0(r4), 0, 0
    psq_l fp6, 0x1f8(r4), 0, 0
    psq_l fp7, 0x200(r4), 0, 0
    psq_l fp8, 0x208(r4), 0, 0
    psq_l fp9, 0x210(r4), 0, 0
    psq_l fp10, 0x218(r4), 0, 0
    psq_l fp11, 0x220(r4), 0, 0
    psq_l fp12, 0x228(r4), 0, 0
    psq_l fp13, 0x230(r4), 0, 0
    psq_l fp14, 0x238(r4), 0, 0
    psq_l fp15, 0x240(r4), 0, 0
    psq_l fp16, 0x248(r4), 0, 0
    psq_l fp17, 0x250(r4), 0, 0
    psq_l fp18, 0x258(r4), 0, 0
    psq_l fp19, 0x260(r4), 0, 0
    psq_l fp20, 0x268(r4), 0, 0
    psq_l fp21, 0x270(r4), 0, 0
    psq_l fp22, 0x278(r4), 0, 0
    psq_l fp23, 0x280(r4), 0, 0
    psq_l fp24, 0x288(r4), 0, 0
    psq_l fp25, 0x290(r4), 0, 0
    psq_l fp26, 0x298(r4), 0, 0
    psq_l fp27, 0x2a0(r4), 0, 0
    psq_l fp28, 0x2a8(r4), 0, 0
    psq_l fp29, 0x2b0(r4), 0, 0
    psq_l fp30, 0x2b8(r4), 0, 0
    psq_l fp31, 0x2c0(r4), 0, 0
L_80151E88:
    lfd fp0, 0x90(r4)
    lfd fp1, 0x98(r4)
    lfd fp2, 0xa0(r4)
    lfd fp3, 0xa8(r4)
    lfd fp4, 0xb0(r4)
    lfd fp5, 0xb8(r4)
    lfd fp6, 0xc0(r4)
    lfd fp7, 0xc8(r4)
    lfd fp8, 0xd0(r4)
    lfd fp9, 0xd8(r4)
    lfd fp10, 0xe0(r4)
    lfd fp11, 0xe8(r4)
    lfd fp12, 0xf0(r4)
    lfd fp13, 0xf8(r4)
    lfd fp14, 0x100(r4)
    lfd fp15, 0x108(r4)
    lfd fp16, 0x110(r4)
    lfd fp17, 0x118(r4)
    lfd fp18, 0x120(r4)
    lfd fp19, 0x128(r4)
    lfd fp20, 0x130(r4)
    lfd fp21, 0x138(r4)
    lfd fp22, 0x140(r4)
    lfd fp23, 0x148(r4)
    lfd fp24, 0x150(r4)
    lfd fp25, 0x158(r4)
    lfd fp26, 0x160(r4)
    lfd fp27, 0x168(r4)
    lfd fp28, 0x170(r4)
    lfd fp29, 0x178(r4)
    lfd fp30, 0x180(r4)
    lfd fp31, 0x188(r4)
L_80151F08:
    blr
}

asm void lbl_80151F0C(void)
{
    nofralloc
    mr r5, r3
    lhz r3, 0x1a2(r5)
    ori r3, r3, 0x1
    sth r3, 0x1a2(r5)
    stfd fp0, 0x90(r5)
    stfd fp1, 0x98(r5)
    stfd fp2, 0xa0(r5)
    stfd fp3, 0xa8(r5)
    stfd fp4, 0xb0(r5)
    stfd fp5, 0xb8(r5)
    stfd fp6, 0xc0(r5)
    stfd fp7, 0xc8(r5)
    stfd fp8, 0xd0(r5)
    stfd fp9, 0xd8(r5)
    stfd fp10, 0xe0(r5)
    stfd fp11, 0xe8(r5)
    stfd fp12, 0xf0(r5)
    stfd fp13, 0xf8(r5)
    stfd fp14, 0x100(r5)
    stfd fp15, 0x108(r5)
    stfd fp16, 0x110(r5)
    stfd fp17, 0x118(r5)
    stfd fp18, 0x120(r5)
    stfd fp19, 0x128(r5)
    stfd fp20, 0x130(r5)
    stfd fp21, 0x138(r5)
    stfd fp22, 0x140(r5)
    stfd fp23, 0x148(r5)
    stfd fp24, 0x150(r5)
    stfd fp25, 0x158(r5)
    stfd fp26, 0x160(r5)
    stfd fp27, 0x168(r5)
    stfd fp28, 0x170(r5)
    stfd fp29, 0x178(r5)
    stfd fp30, 0x180(r5)
    stfd fp31, 0x188(r5)
    mffs fp0
    stfd fp0, 0x190(r5)
    lfd fp0, 0x90(r5)
    mfspr r3, 920
    extrwi. r3, r3, 1, 2
    beq L_80152034
    psq_st fp0, 0x1c8(r5), 0, 0
    psq_st fp1, 0x1d0(r5), 0, 0
    psq_st fp2, 0x1d8(r5), 0, 0
    psq_st fp3, 0x1e0(r5), 0, 0
    psq_st fp4, 0x1e8(r5), 0, 0
    psq_st fp5, 0x1f0(r5), 0, 0
    psq_st fp6, 0x1f8(r5), 0, 0
    psq_st fp7, 0x200(r5), 0, 0
    psq_st fp8, 0x208(r5), 0, 0
    psq_st fp9, 0x210(r5), 0, 0
    psq_st fp10, 0x218(r5), 0, 0
    psq_st fp11, 0x220(r5), 0, 0
    psq_st fp12, 0x228(r5), 0, 0
    psq_st fp13, 0x230(r5), 0, 0
    psq_st fp14, 0x238(r5), 0, 0
    psq_st fp15, 0x240(r5), 0, 0
    psq_st fp16, 0x248(r5), 0, 0
    psq_st fp17, 0x250(r5), 0, 0
    psq_st fp18, 0x258(r5), 0, 0
    psq_st fp19, 0x260(r5), 0, 0
    psq_st fp20, 0x268(r5), 0, 0
    psq_st fp21, 0x270(r5), 0, 0
    psq_st fp22, 0x278(r5), 0, 0
    psq_st fp23, 0x280(r5), 0, 0
    psq_st fp24, 0x288(r5), 0, 0
    psq_st fp25, 0x290(r5), 0, 0
    psq_st fp26, 0x298(r5), 0, 0
    psq_st fp27, 0x2a0(r5), 0, 0
    psq_st fp28, 0x2a8(r5), 0, 0
    psq_st fp29, 0x2b0(r5), 0, 0
    psq_st fp30, 0x2b8(r5), 0, 0
    psq_st fp31, 0x2c0(r5), 0, 0
L_80152034:
    blr
}

asm void fn_80152038(void)
{
    nofralloc
    li r12, 0xff
    li r0, 0xf0
    cmpwi r10, 0x0
    beq L_80152060
    li r12, 0xf0
    li r0, 0xc0
    andi. r10, r10, 0x1
    bne L_80152060
    li r12, 0xc0
    li r0, 0x80
L_80152060:
    extrwi. r10, r4, 1, 16
    beq L_8015206C
    mr r12, r0
L_8015206C:
    blr
}

asm void fn_80152070(void)
{
    nofralloc
    mflr r30
    lbz r3, 0x4e4(r31)
    cmpwi r3, 0x0
    beq L_801523FC
    xori r3, r3, 0xff
    lwz r4, 0x370(r31)
    lwz r4, 0x0(r4)
    srwi r5, r4, 26
    extrwi r6, r4, 10, 21
    lwz r11, 0x438(r31)
    lwz r8, 0x450(r31)
    clrrwi r8, r8, 3
    subf r7, r8, r11
    extrwi. r9, r4, 5, 11
    bne L_801520B4
    li r9, 0x0
    b L_801520BC
L_801520B4:
    slwi r9, r9, 3
    lwzx r9, r9, r31
L_801520BC:
    extrwi r10, r4, 5, 16
    slwi r10, r10, 3
    lwzx r10, r10, r31
    cmpwi r5, 0x4
    beq L_801521F0
    cmpwi r5, 0x1f
    beq L_80152238
    extsh r8, r4
    add r8, r8, r9
    cmpwi r5, 0x22
    beq L_801523C0
    cmpwi r5, 0x23
    beq L_801523C0
    cmpwi r5, 0x26
    beq L_801523C0
    cmpwi r5, 0x27
    beq L_801523C0
    cmpwi r5, 0x2a
    beq L_801523C8
    cmpwi r5, 0x2b
    beq L_801523C8
    cmpwi r5, 0x28
    beq L_801523C8
    cmpwi r5, 0x29
    beq L_801523C8
    cmpwi r5, 0x2c
    beq L_801523C8
    cmpwi r5, 0x2d
    beq L_801523C8
    cmpwi r5, 0x20
    beq L_801523D0
    cmpwi r5, 0x21
    beq L_801523D0
    cmpwi r5, 0x24
    beq L_801523D0
    cmpwi r5, 0x25
    beq L_801523D0
    cmpwi r5, 0x30
    beq L_801523D0
    cmpwi r5, 0x31
    beq L_801523D0
    cmpwi r5, 0x34
    beq L_801523D0
    cmpwi r5, 0x35
    beq L_801523D0
    cmpwi r5, 0x32
    beq L_801523D8
    cmpwi r5, 0x33
    beq L_801523D8
    cmpwi r5, 0x36
    beq L_801523D8
    cmpwi r5, 0x37
    beq L_801523D8
    cmpwi r5, 0x2e
    beq L_80152394
    cmpwi r5, 0x2f
    beq L_80152394
    slwi r8, r4, 20
    srawi r8, r8, 20
    add r8, r8, r9
    extrwi r9, r4, 3, 17
    slwi r9, r9, 3
    addi r9, r9, 0x208
    lwzx r9, r9, r31
    extrwi r10, r9, 3, 13
    bl fn_80152038
    cmpwi r5, 0x38
    beq L_801523DC
    cmpwi r5, 0x39
    beq L_801523DC
    clrlwi r11, r9, 29
    bl fn_80152038
    cmpwi r5, 0x3c
    beq L_801523DC
    cmpwi r5, 0x3d
    beq L_801523DC
    b L_801523FC
L_801521F0:
    add r8, r9, r10
    extrwi r9, r4, 3, 17
    slwi r9, r9, 3
    addi r9, r9, 0x208
    lwzx r9, r9, r31
    extrwi r10, r9, 3, 13
    bl fn_80152038
    cmpwi r6, 0x6
    beq L_801523DC
    cmpwi r6, 0x26
    beq L_801523DC
    clrlwi r11, r9, 29
    bl fn_80152038
    cmpwi r6, 0x7
    beq L_801523DC
    cmpwi r6, 0x27
    beq L_801523DC
    b L_801523FC
L_80152238:
    add r8, r9, r10
    cmpwi r6, 0x77
    beq L_801523C0
    cmpwi r6, 0x57
    beq L_801523C0
    cmpwi r6, 0xf7
    beq L_801523C0
    cmpwi r6, 0xd7
    beq L_801523C0
    cmpwi r6, 0x177
    beq L_801523C8
    cmpwi r6, 0x157
    beq L_801523C8
    cmpwi r6, 0x137
    beq L_801523C8
    cmpwi r6, 0x117
    beq L_801523C8
    cmpwi r6, 0x1b7
    beq L_801523C8
    cmpwi r6, 0x197
    beq L_801523C8
    cmpwi r6, 0x316
    beq L_801523C8
    cmpwi r6, 0x396
    beq L_801523C8
    cmpwi r6, 0x37
    beq L_801523D0
    cmpwi r6, 0x17
    beq L_801523D0
    cmpwi r6, 0xb7
    beq L_801523D0
    cmpwi r6, 0x97
    beq L_801523D0
    cmpwi r6, 0x216
    beq L_801523D0
    cmpwi r6, 0x296
    beq L_801523D0
    cmpwi r6, 0x237
    beq L_801523D0
    cmpwi r6, 0x217
    beq L_801523D0
    cmpwi r6, 0x2b7
    beq L_801523D0
    cmpwi r6, 0x297
    beq L_801523D0
    cmpwi r6, 0x3d7
    beq L_801523D0
    cmpwi r6, 0x14
    beq L_801523D0
    cmpwi r6, 0x96
    beq L_801523D0
    cmpwi r6, 0x277
    beq L_801523D8
    cmpwi r6, 0x257
    beq L_801523D8
    cmpwi r6, 0x2f7
    beq L_801523D8
    cmpwi r6, 0x2d7
    beq L_801523D8
    cmpwi r6, 0x215
    beq L_8015235C
    cmpwi r6, 0x295
    beq L_8015235C
    mr r8, r9
    cmpwi r6, 0x255
    beq L_8015234C
    cmpwi r6, 0x2d5
    beq L_8015234C
    b L_801523FC
L_8015234C:
    extrwi. r4, r4, 5, 16
    bne L_80152364
    li r4, 0x20
    b L_80152364
L_8015235C:
    lwz r4, 0x348(r31)
    clrlwi r5, r4, 25
L_80152364:
    subf. r5, r8, r11
    beq L_80152374
    subf r4, r5, r4
    mr r8, r11
L_80152374:
    li r12, 0xff
    cmpwi r4, 0x8
    bge L_801523DC
    li r5, 0x8
    subf r4, r4, r5
    slw r12, r12, r4
    andi. r12, r12, 0xff
    b L_801523DC
L_80152394:
    subf. r5, r8, r11
    beq L_801523D8
    mr r8, r11
    extrwi r6, r4, 5, 6
    li r4, 0x20
    subf r4, r6, r4
    slwi r4, r4, 2
    subf r4, r5, r4
    cmpwi r4, 0x4
    beq L_801523D0
    b L_801523D8
L_801523C0:
    li r12, 0x80
    b L_801523DC
L_801523C8:
    li r12, 0xc0
    b L_801523DC
L_801523D0:
    li r12, 0xf0
    b L_801523DC
L_801523D8:
    li r12, 0xff
L_801523DC:
    subf r8, r8, r11
    slw r4, r12, r8
    srw r4, r4, r7
    and. r3, r3, r4
    bne L_801523FC
    li r3, 0x1
    mtlr r30
    blr
L_801523FC:
    li r3, 0x0
    mtlr r30
    blr
    opword 0x00000000
    opword 0x00000000
}

asm void fn_80152410(void)
{
    nofralloc
    cmpwi r3, 0x7
    bge fn_801525A8
    slwi r7, r3, 2
    lis r6, lbl_801E1B32+0xE@h
    ori r6, r6, lbl_801E1B32+0xE@l
    lwz r3, 0x18(r31)
    lwz r4, 0x20(r31)
    lwz r5, 0x28(r31)
    lwzx r6, r7, r6
    lis r7, lbl_801E1B32+0x8E@h
    ori r7, r7, lbl_801E1B32+0x8E@l
    stb r6, 0x8(r7)
    li r8, 0x10
    li r6, 0x2
    sthbrx r8, r6, r7
    stw r3, 0xc(r7)
    stw r4, 0x10(r7)
    stw r5, 0x14(r7)
    mr r3, r7
    li r4, 0x18
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    li r4, 0x0
    lis r3, lbl_801E1B32+0xA6@h
    ori r3, r3, lbl_801E1B32+0xA6@l
    stw r4, 0x0(r3)
    entry lbl_8015247C
L_8015247C:
    li r3, 0x0
    stw r3, 0x6b4(r31)
L_80152484:
    lwz r0, 0x69c(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    beq L_80152484
    stw r3, 0x6b4(r31)
    cmplwi r3, 0x8008
    ble L_801524AC
    lis r3, 0x0
    ori r3, r3, 0x8008
L_801524AC:
    mr r4, r3
    stw r4, 0x6b8(r31)
    addi r3, r31, 0x700
    lwz r0, 0x6a0(r31)
    mtlr r0
    blrl
    cmpwi r3, 0x0
    beq L_801524F0
    lis r3, lbl_801E1B32+0xAA@h
    ori r3, r3, lbl_801E1B32+0xAA@l
L_801524D4:
    lwz r4, 0x6d0(r31)
    cmpwi r4, 0x0
    bne L_801524E8
    bl OSReport
    b L_8015247C
L_801524E8:
    bl fn_801510E8
    b L_8015247C
L_801524F0:
    lbz r3, 0x700(r31)
    cmplwi r3, 0x14
    blt L_80152508
    lis r3, lbl_801E1B32+0xC6@h
    ori r3, r3, lbl_801E1B32+0xC6@l
    b L_801524D4
L_80152508:
    slwi r3, r3, 2
    lis r4, lbl_801E1B32+0x2A@h
    ori r4, r4, lbl_801E1B32+0x2A@l
    lwzx r5, r3, r4
    mtlr r5
    blrl
    b L_8015247C
}

asm void lbl_80152524(void)
{
    nofralloc
    li r4, 0x0
    sth r4, 0x702(r31)
    addi r3, r31, 0x700
    li r4, 0x8
    lwz r0, 0x6a4(r31)
    mtlr r0
    blrl
    lwz r3, 0x708(r31)
    entry lbl_80152544
    stw r3, 0x18(r31)
    lwz r3, 0x370(r31)
    addi r3, r3, 0x4
    stw r3, 0x370(r31)
    lis r3, lbl_801E1B32+0xA6@h
    ori r3, r3, lbl_801E1B32+0xA6@l
    lwz r3, 0x0(r3)
    cmpwi r3, 0x0
    beq fn_80150F34
    lwz r3, 0x378(r31)
    ori r3, r3, 0x400
    stw r3, 0x378(r31)
    b fn_80150F34
}

asm void lbl_80152578(void)
{
    nofralloc
    li r4, 0x1
    lis r3, lbl_801E1B32+0xA6@h
    ori r3, r3, lbl_801E1B32+0xA6@l
    stw r4, 0x0(r3)
    b lbl_8015247C
}

asm void lbl_8015258C(void)
{
    nofralloc
    li r4, 0x1
    lis r3, lbl_801E1B32+0xA6@h
    ori r3, r3, lbl_801E1B32+0xA6@l
    stw r4, 0x0(r3)
    lis r3, 0xffff
    ori r3, r3, 0xffff
    b lbl_80152544
}

asm void fn_801525A8(void)
{
    nofralloc
    mr r30, r3
    lwz r3, 0x18(r31)
    lwz r4, 0x20(r31)
    lwz r5, 0x28(r31)
    lwz r6, 0x30(r31)
    li r7, -0x1
    li r8, 0x0
    cmpwi r30, 0x7
    beq L_801525F8
    cmpwi r30, 0xa
    beq L_801525E8
    cmpwi r30, 0xb
    beq L_801525F0
    li r8, 0x1
    cmpwi r30, 0x8
    bne L_801525F0
L_801525E8:
    bl PCreadAsyncInit
    b L_801525FC
L_801525F0:
    bl PCwriteAsyncInit
    b L_801525FC
L_801525F8:
    bl CompleteAsync
L_801525FC:
    stw r3, 0x18(r31)
    lwz r3, 0x370(r31)
    addi r3, r3, 0x4
    stw r3, 0x370(r31)
    b fn_80150F34
}

asm void lbl_80152610(void)
{
    nofralloc
    li r3, 0x10
    lis r4, lbl_80152700@h
    ori r4, r4, lbl_80152700@l
    stw r3, 0x0(r4)
    li r3, 0x11
    lis r4, fn_80152708@h
    ori r4, r4, fn_80152708@l
    stw r3, 0x0(r4)
    li r3, 0x12
    lis r4, fn_80152710@h
    ori r4, r4, fn_80152710@l
    stw r3, 0x0(r4)
    li r3, 0x13
    lis r4, lbl_80152718@h
    ori r4, r4, lbl_80152718@l
    stw r3, 0x0(r4)
    li r3, 0x14
    lis r4, lbl_80152720@h
    ori r4, r4, lbl_80152720@l
    stw r3, 0x0(r4)
    li r3, 0x15
    lis r4, lbl_80152728@h
    ori r4, r4, lbl_80152728@l
    stw r3, 0x0(r4)
    li r3, 0x16
    lis r4, lbl_80152730@h
    ori r4, r4, lbl_80152730@l
    stw r3, 0x0(r4)
    li r3, 0x17
    lis r4, fn_80152738@h
    ori r4, r4, fn_80152738@l
    stw r3, 0x0(r4)
    li r3, 0x18
    lis r4, fn_80152740@h
    ori r4, r4, fn_80152740@l
    stw r3, 0x0(r4)
    li r3, 0x19
    lis r4, fn_80152748@h
    ori r4, r4, fn_80152748@l
    stw r3, 0x0(r4)
    li r3, 0x1a
    lis r4, fn_80152750@h
    ori r4, r4, fn_80152750@l
    stw r3, 0x0(r4)
    li r3, 0x1b
    lis r4, fn_80152758@h
    ori r4, r4, fn_80152758@l
    stw r3, 0x0(r4)
    blr
}

asm void fn_801526D4(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x8(r1)
    lis r3, lbl_801E1B32+0xDF@h
    ori r3, r3, lbl_801E1B32+0xDF@l
    bl OSReport
    li r3, -0x1
    addi r1, r1, 0x8
    lwz r0, 0x4(r1)
    mtlr r0
    blr
}

asm void lbl_80152700(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void fn_80152708(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void fn_80152710(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void lbl_80152718(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void lbl_80152720(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void lbl_80152728(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void lbl_80152730(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void fn_80152738(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void fn_80152740(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void fn_80152748(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void fn_80152750(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void fn_80152758(void)
{
    nofralloc
    b fn_801526D4
    blr
}

asm void lbl_80152760(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x10(r1)
    stw r31, 0x8(r1)
    stw r30, 0xc(r1)
    addi r31, r4, 0x0
    addi r30, r3, 0x0
    bl OSDisableInterrupts
    lis r0, lbl_801E1B32+0x136@h
    ori r0, r0, lbl_801E1B32+0x136@l
    lis r4, lbl_801E1B32+0x132@h
    ori r4, r4, lbl_801E1B32+0x132@l
    stw r0, 0x0(r4)
    stw r4, 0x0(r30)
    addi r30, r3, 0x0
    lis r3, lbl_801E1B32+0x12E@h
    ori r3, r3, lbl_801E1B32+0x12E@l
    stw r31, 0x0(r3)
    lis r3, 0x1
    ori r3, r3, 0x8000
    bl __OSMaskInterrupts
    li r0, 0x0
    lis r3, 0xcc00
    stw r0, 0x6828(r3)
L_801527C0:
    bl fn_80152D34
L_801527C4:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_801527C4
L_801527D0:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_801527D0
    bl fn_80152D60
    li r3, 0x0
    bl fn_80152DDC
    li r3, 0x0
    bl fn_80152DDC
    li r3, 0x4
    bl fn_80152DDC
    li r3, 0x80
    bl fn_80152DDC
    li r3, 0x0
    bl fn_80152DDC
    bl fn_80152D60
L_80152810:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152810
L_8015281C:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_8015281C
    bl fn_80152D60
    bl fn_80152E10
    mr r5, r3
    bl fn_80152E10
    mr r6, r3
    li r3, 0xf
    mtctr r3
L_80152848:
    bdnz L_80152848
    bl fn_80152D4C
    lis r4, 0x534e
    ori r4, r4, 0x444c
    subf r5, r4, r5
    cmpwi r5, 0x0
    bne L_801527C0
    lis r3, lbl_802F3480+0x20@h
    ori r3, r3, lbl_802F3480+0x20@l
    stw r6, 0x0(r3)
    li r5, 0x1
    slw r5, r5, r6
    stw r5, 0x4(r3)
    li r5, -0x1
    subfic r4, r6, 0x20
    srw r5, r5, r4
    clrrwi r4, r5, 2
    stw r5, 0x8(r3)
    stw r4, 0xc(r3)
    mr r3, r30
    bl OSRestoreInterrupts
    lwz r31, 0x8(r1)
    lwz r30, 0xc(r1)
    lwz r0, 0x14(r1)
    addi r1, r1, 0x10
    mtlr r0
    blr
}

asm void lbl_801528B4(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x8(r1)
    lis r3, 0x1
    ori r3, r3, 0x8000
    bl __OSMaskInterrupts
    li r3, 0x40
    bl __OSMaskInterrupts
    lis r4, fn_80152CE0@h
    ori r4, r4, fn_80152CE0@l
    li r3, 0x19
    bl __OSSetInterruptHandler
    li r3, 0x40
    bl __OSUnmaskInterrupts
    lwz r0, 0xc(r1)
    addi r1, r1, 0x8
    mtlr r0
    blr
}

asm void lbl_801528FC(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x8(r1)
    bl fn_80152D34
L_8015290C:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_8015290C
L_80152918:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152918
    bl fn_80152D60
    li r3, 0x1
    bl fn_80152DDC
    bl fn_80152D60
L_80152938:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152938
L_80152944:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152944
    bl fn_80152D60
    bl fn_80152E10
    mr r5, r3
    li r3, 0xf
    mtctr r3
L_80152968:
    bdnz L_80152968
    bl fn_80152D4C
    mr r3, r5
    lwz r0, 0xc(r1)
    addi r1, r1, 0x8
    mtlr r0
    blr
}

asm void lbl_80152984(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x8(r1)
    stw r30, 0xc(r1)
    stw r29, 0x10(r1)
    stw r28, 0x14(r1)
    mr r31, r4
    mr r30, r3
    bl DCInvalidateRange
    bl fn_80152D34
L_801529B0:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_801529B0
L_801529BC:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_801529BC
    bl fn_80152D60
    li r3, 0x2
    bl fn_80152DDC
    lis r3, lbl_802F3480+0x20@h
    ori r3, r3, lbl_802F3480+0x20@l
    lwz r4, 0x0(r3)
    srw r28, r31, r4
    cmpwi r28, 0x0
    bne L_80152A00
    lwz r4, 0x8(r3)
    and. r28, r31, r4
    beq L_80152B0C
    b L_80152A6C
L_80152A00:
    bl fn_80152D60
L_80152A04:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152A04
L_80152A10:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152A10
    bl fn_80152D60
    lis r5, lbl_802F3480+0x20@h
    ori r5, r5, lbl_802F3480+0x20@l
    lwz r5, 0x4(r5)
    lis r29, 0xcc00
    stw r30, 0x682c(r29)
    stw r5, 0x6830(r29)
    li r3, 0x3
    stw r3, 0x6834(r29)
    bl fn_80152D88
    add r30, r30, r5
    subi r28, r28, 0x1
    cmpwi r28, 0x0
    bne L_80152A00
    lis r5, lbl_802F3480+0x20@h
    ori r5, r5, lbl_802F3480+0x20@l
    lwz r5, 0x8(r5)
    and. r28, r31, r5
    beq L_80152B0C
L_80152A6C:
    bl fn_80152D60
L_80152A70:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152A70
L_80152A7C:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152A7C
    bl fn_80152D60
    lis r5, lbl_802F3480+0x20@h
    ori r5, r5, lbl_802F3480+0x20@l
    lwz r5, 0xc(r5)
    and r29, r31, r5
    andi. r29, r29, 0xffe0
    beq L_80152AC8
    lis r28, 0xcc00
    stw r30, 0x682c(r28)
    stw r29, 0x6830(r28)
    li r3, 0x3
    stw r3, 0x6834(r28)
    add r30, r30, r29
    andi. r29, r29, 0x1f
    bl fn_80152D88
L_80152AC8:
    andi. r28, r31, 0x1c
    beq L_80152AEC
    li r29, 0x0
L_80152AD4:
    bl fn_80152E10
    stwx r3, r30, r29
    addi r29, r29, 0x4
    cmpw r29, r28
    blt L_80152AD4
    add r30, r30, r29
L_80152AEC:
    andi. r28, r31, 0x3
    beq L_80152B0C
    li r29, 0x0
L_80152AF8:
    bl fn_80152D9C
    stbx r3, r30, r29
    addi r29, r29, 0x1
    cmpw r29, r28
    blt L_80152AF8
L_80152B0C:
    li r3, 0xf
    mtctr r3
L_80152B14:
    bdnz L_80152B14
    bl fn_80152D4C
    li r3, 0x0
    lwz r31, 0x8(r1)
    lwz r30, 0xc(r1)
    lwz r29, 0x10(r1)
    lwz r28, 0x14(r1)
    lwz r0, 0x1c(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

asm void lbl_80152B40(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x8(r1)
    stw r30, 0xc(r1)
    stw r29, 0x10(r1)
    stw r28, 0x14(r1)
    mr r31, r4
    mr r30, r3
    bl DCFlushRange
    bl fn_80152D34
L_80152B6C:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152B6C
L_80152B78:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152B78
    bl fn_80152D60
    li r3, 0x3
    bl fn_80152DDC
    srwi r3, r31, 8
    bl fn_80152DDC
    mr r3, r31
    bl fn_80152DDC
    srwi r28, r31, 9
    cmpwi r28, 0x0
    bne L_80152BBC
    andi. r28, r31, 0x1ff
    beq L_80152CA4
    b L_80152C14
L_80152BBC:
    bl fn_80152D60
L_80152BC0:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152BC0
L_80152BCC:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152BCC
    bl fn_80152D60
    lis r29, 0xcc00
    stw r30, 0x682c(r29)
    li r5, 0x200
    stw r5, 0x6830(r29)
    li r3, 0x7
    stw r3, 0x6834(r29)
    bl fn_80152D88
    add r30, r30, r5
    subi r28, r28, 0x1
    cmpwi r28, 0x0
    bne L_80152BBC
    andi. r28, r31, 0x1ff
    beq L_80152CA4
L_80152C14:
    bl fn_80152D60
L_80152C18:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152C18
L_80152C24:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152C24
    bl fn_80152D60
    andi. r29, r31, 0x1e0
    beq L_80152C60
    lis r28, 0xcc00
    stw r30, 0x682c(r28)
    stw r29, 0x6830(r28)
    li r3, 0x7
    stw r3, 0x6834(r28)
    add r30, r30, r29
    andi. r29, r29, 0x1f
    bl fn_80152D88
L_80152C60:
    andi. r28, r31, 0x1c
    beq L_80152C84
    li r29, 0x0
L_80152C6C:
    lwzx r3, r30, r29
    bl fn_80152E4C
    addi r29, r29, 0x4
    cmpw r29, r28
    blt L_80152C6C
    add r30, r30, r29
L_80152C84:
    andi. r28, r31, 0x3
    beq L_80152CA4
    li r29, 0x0
L_80152C90:
    lbzx r3, r30, r29
    bl fn_80152DDC
    addi r29, r29, 0x1
    cmpw r29, r28
    blt L_80152C90
L_80152CA4:
    li r3, 0xf
    mtctr r3
L_80152CAC:
    bdnz L_80152CAC
    bl fn_80152D4C
    li r3, 0x0
    lwz r31, 0x8(r1)
    lwz r30, 0xc(r1)
    lwz r29, 0x10(r1)
    lwz r28, 0x14(r1)
    lwz r0, 0x1c(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

asm void lbl_80152CD8(void)
{
    nofralloc
    blr
}

asm void lbl_80152CDC(void)
{
    nofralloc
    blr
}

asm void fn_80152CE0(void)
{
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x8(r1)
    li r0, 0x1000
    lis r5, 0xcc00
    stw r0, 0x3000(r5)
    lis r5, lbl_801E1B32+0x12E@h
    ori r5, r5, lbl_801E1B32+0x12E@l
    lwz r5, 0x0(r5)
    cmpwi r5, 0x0
    beq L_80152D24
    mtlr r5
    lis r5, lbl_801E1B32+0x136@h
    ori r5, r5, lbl_801E1B32+0x136@l
    li r0, 0x1
    stb r0, 0x0(r5)
    blrl
L_80152D24:
    lwz r0, 0xc(r1)
    addi r1, r1, 0x8
    mtlr r0
    blr
}

asm void fn_80152D34(void)
{
    nofralloc
    lis r4, 0xcc00
    lwz r3, 0x6828(r4)
    andi. r3, r3, 0x5
    ori r3, r3, 0xd0
    stw r3, 0x6828(r4)
    blr
}

asm void fn_80152D4C(void)
{
    nofralloc
    lis r4, 0xcc00
    lwz r3, 0x6828(r4)
    andi. r3, r3, 0x5
    stw r3, 0x6828(r4)
    blr
}

asm void fn_80152D60(void)
{
    nofralloc
    lis r4, 0xcc00
    lwz r5, 0x6828(r4)
    andi. r5, r5, 0x5
    stw r5, 0x6828(r4)
    li r3, 0x6
    mtctr r3
L_80152D78:
    bdnz L_80152D78
    ori r5, r5, 0xd0
    stw r5, 0x6828(r4)
    blr
}

asm void fn_80152D88(void)
{
    nofralloc
    lis r4, 0xcc00
L_80152D8C:
    lwz r3, 0x6834(r4)
    clrlwi. r3, r3, 31
    bne L_80152D8C
    blr
}

asm void fn_80152D9C(void)
{
    nofralloc
    mflr r4
    stw r4, 0x4(r1)
    stwu r1, -0x8(r1)
    lis r4, 0xcc00
    li r3, 0x0
    stw r3, 0x6838(r4)
    li r3, 0x1
    stw r3, 0x6834(r4)
    bl fn_80152D88
    lis r4, 0xcc00
    lwz r3, 0x6838(r4)
    srwi r3, r3, 24
    lwz r4, 0xc(r1)
    addi r1, r1, 0x8
    mtlr r4
    blr
}

asm void fn_80152DDC(void)
{
    nofralloc
    mflr r4
    stw r4, 0x4(r1)
    stwu r1, -0x8(r1)
    slwi r3, r3, 24
    lis r4, 0xcc00
    stw r3, 0x6838(r4)
    li r3, 0x5
    stw r3, 0x6834(r4)
    bl fn_80152D88
    lwz r4, 0xc(r1)
    addi r1, r1, 0x8
    mtlr r4
    blr
}

asm void fn_80152E10(void)
{
    nofralloc
    mflr r4
    stw r4, 0x4(r1)
    stwu r1, -0x8(r1)
    lis r4, 0xcc00
    li r3, 0x0
    stw r3, 0x6838(r4)
    li r3, 0x31
    stw r3, 0x6834(r4)
    bl fn_80152D88
    lis r4, 0xcc00
    lwz r3, 0x6838(r4)
    lwz r4, 0xc(r1)
    addi r1, r1, 0x8
    mtlr r4
    blr
}

asm void fn_80152E4C(void)
{
    nofralloc
    mflr r4
    stw r4, 0x4(r1)
    stwu r1, -0x8(r1)
    lis r4, 0xcc00
    stw r3, 0x6838(r4)
    li r3, 0x35
    stw r3, 0x6834(r4)
    bl fn_80152D88
    addi r1, r1, 0x8
    lwz r4, 0x4(r1)
    mtlr r4
    blr
}

asm void fn_80152E7C(void)
{
    nofralloc
    mflr r0
    stwu r1, -0x18(r1)
    stw r0, 0x1c(r1)
    stmw r28, 0x8(r1)
    bl fn_80152D34
L_80152E90:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152E90
L_80152E9C:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152E9C
    bl fn_80152D60
    li r3, 0x2
    bl fn_80152DDC
    lwz r0, 0x1c(r1)
    mtlr r0
    lmw r28, 0x8(r1)
    addi r1, r1, 0x18
    blr
}

asm void fn_80152ECC(void)
{
    nofralloc
    mflr r0
    stwu r1, -0x18(r1)
    stw r0, 0x1c(r1)
    stmw r28, 0x8(r1)
    mr r31, r4
    mr r30, r3
    bl fn_80152D60
L_80152EE8:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152EE8
L_80152EF4:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152EF4
    bl fn_80152D60
    lis r29, 0xcc00
    stw r30, 0x682c(r29)
    stw r31, 0x6830(r29)
    li r3, 0x3
    stw r3, 0x6834(r29)
    lwz r3, 0x6828(r29)
    ori r3, r3, 0xc
    stw r3, 0x6828(r29)
    lwz r0, 0x1c(r1)
    mtlr r0
    lmw r28, 0x8(r1)
    addi r1, r1, 0x18
    blr
}

asm void fn_80152F3C(void)
{
    nofralloc
    mflr r0
    stwu r1, -0x18(r1)
    stw r0, 0x1c(r1)
    stmw r28, 0x8(r1)
    mr r31, r4
    mr r30, r3
    bl fn_80152D60
L_80152F58:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152F58
L_80152F64:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152F64
    bl fn_80152D60
    lis r29, 0xcc00
    stw r30, 0x682c(r29)
    stw r31, 0x6830(r29)
    li r3, 0x3
    stw r3, 0x6834(r29)
    bl fn_80152D88
    lwz r0, 0x1c(r1)
    mtlr r0
    lmw r28, 0x8(r1)
    addi r1, r1, 0x18
    blr
}

asm void fn_80152FA4(void)
{
    nofralloc
    mflr r0
    stwu r1, -0x18(r1)
    stw r0, 0x1c(r1)
    stmw r28, 0x8(r1)
    mr r31, r3
    bl fn_80152D34
L_80152FBC:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80152FBC
L_80152FC8:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80152FC8
    bl fn_80152D60
    li r3, 0x3
    bl fn_80152DDC
    srwi r3, r31, 8
    bl fn_80152DDC
    mr r3, r31
    bl fn_80152DDC
    lwz r0, 0x1c(r1)
    mtlr r0
    lmw r28, 0x8(r1)
    addi r1, r1, 0x18
    blr
}

asm void fn_80153008(void)
{
    nofralloc
    mflr r0
    stwu r1, -0x18(r1)
    stw r0, 0x1c(r1)
    stmw r28, 0x8(r1)
    mr r31, r4
    mr r30, r3
    bl fn_80152D60
L_80153024:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80153024
L_80153030:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80153030
    bl fn_80152D60
    lis r29, 0xcc00
    stw r30, 0x682c(r29)
    stw r31, 0x6830(r29)
    li r3, 0x7
    stw r3, 0x6834(r29)
    lwz r3, 0x6828(r29)
    ori r3, r3, 0xc
    stw r3, 0x6828(r29)
    lwz r0, 0x1c(r1)
    mtlr r0
    lmw r28, 0x8(r1)
    addi r1, r1, 0x18
    blr
}

asm void fn_80153078(void)
{
    nofralloc
    mflr r0
    stwu r1, -0x18(r1)
    stw r0, 0x1c(r1)
    stmw r28, 0x8(r1)
    mr r31, r4
    mr r30, r3
    bl fn_80152D60
L_80153094:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_80153094
L_801530A0:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_801530A0
    bl fn_80152D60
    lis r29, 0xcc00
    stw r30, 0x682c(r29)
    stw r31, 0x6830(r29)
    li r3, 0x7
    stw r3, 0x6834(r29)
    bl fn_80152D88
    lwz r0, 0x1c(r1)
    mtlr r0
    lmw r28, 0x8(r1)
    addi r1, r1, 0x18
    blr
}

asm void fn_801530E0(void)
{
    nofralloc
    mflr r0
    stwu r1, -0x18(r1)
    stw r0, 0x1c(r1)
    stmw r28, 0x8(r1)
    mr r31, r4
    mr r30, r3
    bl fn_80152D60
L_801530FC:
    bl fn_80152D9C
    cmpwi r3, 0x0
    beq L_801530FC
L_80153108:
    bl fn_80152D60
    bl fn_80152D9C
    cmpwi r3, 0xff
    beq L_80153108
    bl fn_80152D60
    andi. r28, r31, 0x1c
    beq L_80153140
    li r29, 0x0
L_80153128:
    lwzx r3, r30, r29
    bl fn_80152E4C
    addi r29, r29, 0x4
    cmpw r29, r28
    blt L_80153128
    add r30, r30, r29
L_80153140:
    andi. r28, r31, 0x3
    beq L_80153160
    li r29, 0x0
L_8015314C:
    lbzx r3, r30, r29
    bl fn_80152DDC
    addi r29, r29, 0x1
    cmpw r29, r28
    blt L_8015314C
L_80153160:
    li r3, 0xf
    mtctr r3
L_80153168:
    bdnz L_80153168
    bl fn_80152D4C
    lwz r0, 0x1c(r1)
    mtlr r0
    lmw r28, 0x8(r1)
    addi r1, r1, 0x18
    blr
}
