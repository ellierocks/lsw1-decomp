/* Retail DOL section addresses and startup SDA bases.
 * RE4's ProDG link-script pattern; no input bytes or replacement code.
 * _stack_addr and _stack_end are the retail stack top and end (stack top - 0x10000)
 * that dtk names in split objects; the SDK's OS code references them.
 * ai.o carries an empty 8-aligned .sbss (extern OSTime statics); it is placed
 * first so that its alignment cannot pad the end of .sbss and the DOL bss size.
 */
SECTIONS
{
 .init 0x80003100 : { *(.init) }
 .text 0x800034A0 : { *(.text) }
 .rodata 0x8018CB00 : { *(.rodata) }
 .data 0x801B0640 : { *(.data) }
 .bss 0x801FD080 : { *(.bss) *(COMMON) }
 .sdata 0x80407A40 : { *(.sdata) }
 .sbss 0x8040A2C0 : { *ai.o(.sbss) *(.sbss) }
 .sdata2 0x8040B300 : { *(.sdata2) }
 _SDA_BASE_ = 0x8040FA40;
 _SDA2_BASE_ = 0x8041FA40;
 _stack_addr = 0x8041BD40;
 _stack_end = 0x8040BD40;
}
