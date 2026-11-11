/* SN startup assembly reconstructed from GL5E4F. The SDK and MSSB startup
 * donors were reviewed; their register clearing and ROM-copy startup differ.
 * The entry owns r1/r2/r13, preserves argc/argv in r30/r31 and enters the
 * debugger through rfi. fn_80003254 returns argc and argv in r3/r4.
 */
#include <dolphin.h>

extern char _stack_addr[];
extern char _SDA_BASE_[];
extern char _SDA2_BASE_[];
extern unsigned char lbl_801FD080[];
extern unsigned char lbl_8040A2C0[];
extern unsigned char lbl_801E1990[];
extern unsigned char lbl_801E19B4[];
extern unsigned char lbl_801E1970[];
extern void fn_8015057C(void);
extern void fn_801638A0(void);
extern void Menu_FullReset(void);
extern void __init_fpu(void);
extern void __init_hardware(void);
extern void __init_user(void);
extern void exit(int);
__declspec(section ".init") void fn_80003254(void);
__declspec(section ".init") void fn_800032C0(void);
__declspec(section ".init") void __init_registers(void);

__declspec(section ".init") asm void __start(void)
{
    nofralloc
    lis r1, _stack_addr@h
    ori r1, r1, _stack_addr@l
    lis r2, _SDA2_BASE_@h
    ori r2, r2, _SDA2_BASE_@l
    lis r13, _SDA_BASE_@h
    ori r13, r13, _SDA_BASE_@l
    bl __init_registers
    bl __init_fpu
    bl __init_hardware
    li r0, -0x1
    stwu r1, -0x8(r1)
    stw r0, 0x4(r1)
    stw r0, 0x0(r1)
    lis r3, lbl_801FD080@h
    ori r3, r3, lbl_801FD080@l
    li r4, 0x0
    lis r5, 0x8040
    ori r5, r5, 0x7a3c
    subf r5, r3, r5
    bl memset
    lis r3, lbl_8040A2C0@h
    ori r3, r3, lbl_8040A2C0@l
    li r4, 0x0
    lis r5, 0x8040
    ori r5, r5, 0xb2e4
    subf r5, r3, r5
    bl memset
    li r4, 0x0
    lis r5, 0x8000
    stw r4, 0x44(r5)
    li r29, 0x0
    lis r6, 0x8000
    ori r6, r6, 0xf4
    lwz r6, 0x0(r6)
    cmpwi r6, 0x0
    beq loc_800031A4
    lwz r3, 0xc(r6)
    cmplwi r3, 0x2
    blt loc_800031A4
    bl fn_8015057C
    li r29, 0x1
loc_800031A4:
    bl fn_80003254
    mr r30, r3
    mr r31, r4
    bl DBInit
    bl OSInit
    lis r4, 0x8000
    lhz r3, 0x30e6(r4)
    andi. r4, r3, 0x8000
    beq loc_800031D4
    andi. r3, r3, 0x7fff
    cmplwi r3, 0x1
    bne loc_800031D8
loc_800031D4:
    bl fn_800032C0
loc_800031D8:
    bl __init_user
    mr r3, r30
    mr r4, r31
    bl __init_user
    bl fn_801638A0
    cmpwi r29, 0x0
    beq loc_80003244
    lis r3, lbl_801E1990@h
    ori r3, r3, lbl_801E1990@l
    lis r4, lbl_801E19B4@h
    ori r4, r4, lbl_801E19B4@l
    lwz r4, 0x0(r4)
    bl OSReport
    lis r3, lbl_801E1970@h
    ori r3, r3, lbl_801E1970@l
    bl OSReport
    mfmsr r5
    ori r4, r5, 0x8000
    xori r4, r4, 0x8000
    mtmsr r4
    sync
    ori r5, r5, 0x200
    mtsrr1 r5
    lis r4, __start+0x144@h
    ori r4, r4, __start+0x144@l
    mtsrr0 r4
    rfi
loc_80003244:
    mr r3, r30
    mr r4, r31
    bl Menu_FullReset
    b exit
}

__declspec(section ".init") asm void fn_80003254(void)
{
    nofralloc
    lis r6, 0x8000
    ori r6, r6, 0xf4
    lwz r5, 0x0(r6)
    cmpwi r5, 0x0
    bne loc_80003274
loc_80003268:
    li r3, 0x0
    li r4, 0x0
    blr
loc_80003274:
    lwz r6, 0x8(r5)
    cmpwi r6, 0x0
    beq loc_80003268
    add r6, r5, r6
    lwz r3, 0x0(r6)
    cmpwi r3, 0x0
    beq loc_80003268
    addi r4, r6, 0x4
    mtctr r3
loc_80003298:
    addi r6, r6, 0x4
    lwz r7, 0x0(r6)
    add r7, r7, r5
    stw r7, 0x0(r6)
    bdnz loc_80003298
    lis r5, 0x8000
    ori r5, r5, 0x34
    clrrwi r7, r4, 5
    stw r7, 0x0(r5)
    blr
}

__declspec(section ".init") void fn_800032C0(void)
{
    if ((*(volatile u16*)0x800030E4 & 0xEEF) == 0xEEF) {
        OSResetSystem(0, 0, FALSE);
    }
}

__declspec(section ".init") asm void __init_registers(void)
{
    nofralloc
    li r3, 0x0
    mtspr 912, r3
    mtspr 913, r3
    mtspr 914, r3
    mtspr 915, r3
    mtspr 916, r3
    mtspr 917, r3
    mtspr 918, r3
    mtspr 919, r3
    blr
}
