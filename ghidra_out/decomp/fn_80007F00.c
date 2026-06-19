/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_80007F00 @ 80007f00 */


void fn_80007F00(char *param_1,char *param_2)

{
  char cVar1;
  
  cVar1 = *param_2;
  while (cVar1 != '\0') {
    cVar1 = NuToUpper();
    *param_1 = cVar1;
    param_1 = param_1 + 1;
    param_2 = param_2 + 1;
    cVar1 = *param_2;
  }
  *param_1 = *param_2;
  return;
}


