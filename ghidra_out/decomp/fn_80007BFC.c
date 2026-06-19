/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_80007BFC @ 80007bfc */


int fn_80007BFC(char *param_1)

{
  char cVar1;
  int iVar2;
  
  iVar2 = 0;
  cVar1 = *param_1;
  while (cVar1 != '\0') {
    param_1 = param_1 + 1;
    iVar2 = iVar2 + 1;
    cVar1 = *param_1;
  }
  return iVar2;
}


