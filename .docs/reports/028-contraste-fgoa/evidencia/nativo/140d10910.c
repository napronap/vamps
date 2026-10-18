// Requested 140d10910; entry 140d10910; auto-decompiled, types/names provisional.

undefined1 FUN_140d10910(longlong param_1,undefined8 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  float fVar2;
  
  uVar1 = DAT_1412ee864;
  if ((*(uint *)(param_1 + 0x34b0) & 0x800) != 0) {
    return 0;
  }
  fVar2 = *(float *)(param_1 + 0x2d84) * DAT_1415ac564;
  *(float *)(param_1 + 0x2d8c) = *(float *)(param_1 + 0x2d8c) - fVar2;
  *(float *)(param_1 + 0x2d8c) = *(float *)(param_1 + 0x34a4) + *(float *)(param_1 + 0x2d8c);
  FUN_140d10390(param_1 + 0x2da8,fVar2,uVar1);
  *(undefined4 *)(param_1 + 0x2dd8) = param_3;
  return 1;
}

