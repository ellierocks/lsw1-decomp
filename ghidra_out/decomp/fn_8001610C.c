/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_8001610C @ 8001610c */


void fn_8001610C(void)

{
  int iVar1;
  uint uVar2;
  int unaff__r13;
  
  uVar2 = fn_80006E1C();
  iVar1 = *(int *)(unaff__r13 + -0x5748);
  *(int *)(iVar1 + 0x28 + uVar2 * 4) = (int)*(short *)(iVar1 + 0x1b4);
  *(uint *)(iVar1 + 0x128) = *(uint *)(iVar1 + 0x128) | 1 << (uVar2 & 0x3f);
  return;
}


