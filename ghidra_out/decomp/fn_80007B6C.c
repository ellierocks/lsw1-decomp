/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_80007B6C @ 80007b6c */


int fn_80007B6C(char *param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  
  iVar2 = 0;
  if (param_2 == (char *)0x0) {
    *param_1 = '\0';
  }
  else {
    do {
      iVar2 = iVar2 + 1;
      *param_1 = *param_2;
      param_1 = param_1 + 1;
      cVar1 = *param_2;
      param_2 = param_2 + 1;
    } while (cVar1 != '\0');
  }
  return iVar2;
}


