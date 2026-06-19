/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_80016154 @ 80016154 */


void fn_80016154(void)

{
  int iVar1;
  uint uVar2;
  int unaff__r13;
  
  uVar2 = fn_80006E1C();
  iVar1 = *(int *)(unaff__r13 + -0x5748);
  *(int *)(iVar1 + 0xa8 + uVar2 * 4) = (int)*(short *)(iVar1 + 0x1b4);
  *(uint *)(iVar1 + 300) = *(uint *)(iVar1 + 300) | 1 << (uVar2 & 0x3f);
  return;
}


