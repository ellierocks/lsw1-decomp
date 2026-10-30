/* Retail DOL section addresses and startup SDA bases.
 * RE4's ProDG link-script pattern; no input bytes or replacement code.
 * _stack_end names the retail .sbss clear end used by __start.
 */
SECTIONS
{
 .init 0x80003100 : { *(.init) }
 .text 0x800034A0 : { *(.text) }
 .rodata 0x8018CB00 : { *(.rodata) }
 .data 0x801B0640 : { *(.data) }
 .bss 0x801FD080 : { *(.bss) *(COMMON) }
 .sdata 0x80407A40 : { *(.sdata) }
 .sbss 0x8040A2C0 : { *(.sbss) }
 .sdata2 0x8040B300 : { *(.sdata2) }
 _SDA_BASE_ = 0x8040FA40;
 _SDA2_BASE_ = 0x8041FA40;
 _stack_end = 0x8040B2E4;
}
