/* Ghidra decompiler reference output. Do not commit as matching source. */
/* Function: fn_8018199C @ 8018199c */


void fn_8018199C(uint *param_1,undefined4 param_2)

{
  uint uVar1;
  uint *puVar2;
  int unaff__r13;
  
  puVar2 = (uint *)(**(code **)(*(int *)(unaff__r13 + -17000) + 0x4cc))(param_2);
  __GXFlushTextureState();
  uRamcc008000 = *puVar2;
  __GXFlushTextureState();
  *param_1 = *puVar2 & 0x3ff | *param_1 & 0xfffffc00;
  uVar1 = param_1[1];
  puVar2[1] = *param_1;
  puVar2[2] = uVar1;
  puVar2[3] = param_1[2];
  return;
}


