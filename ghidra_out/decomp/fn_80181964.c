/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_80181964 @ 80181964 */


void fn_80181964(uint *param_1,uint param_2,uint param_3,undefined2 param_4)

{
  *param_1 = 0;
  *param_1 = (param_3 & 3) << 10 | *param_1 & 0xfffff3ff;
  param_1[1] = param_2 >> 5 & 0x1fffff | param_1[1] & 0xffe00000;
  param_1[1] = param_1[1] & 0xffffff | 0x64000000;
  *(undefined2 *)(param_1 + 2) = param_4;
  return;
}


