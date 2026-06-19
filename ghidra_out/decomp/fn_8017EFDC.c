/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_8017EFDC @ 8017efdc */


void fn_8017EFDC(void)

{
  int iVar1;
  ulonglong uVar2;
  int unaff__r13;
  
  iVar1 = 0;
  uVar2 = (ulonglong)*(byte *)(*(int *)(unaff__r13 + -17000) + 0x5ab);
  do {
    if ((uVar2 & 1) != 0) {
      uRamcc008000 = *(undefined4 *)(*(int *)(unaff__r13 + -17000) + iVar1 + 0x5c);
    }
    uVar2 = uVar2 >> 1;
    iVar1 = iVar1 + 4;
  } while (uVar2 != 0);
  *(undefined1 *)(*(int *)(unaff__r13 + -17000) + 0x5ab) = 0;
  return;
}


