/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_80007B2C @ 80007b2c */


void fn_80007B2C(char *param_1,char *param_2)

{
  char cVar1;
  
  cVar1 = *param_1;
  while (cVar1 != '\0') {
    param_1 = param_1 + 1;
    cVar1 = *param_1;
  }
  if (param_2 != (char *)0x0) {
    do {
      *param_1 = *param_2;
      param_1 = param_1 + 1;
      cVar1 = *param_2;
      param_2 = param_2 + 1;
    } while (cVar1 != '\0');
    return;
  }
  return;
}


