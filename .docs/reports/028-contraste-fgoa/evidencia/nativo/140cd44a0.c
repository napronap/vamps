// Requested 140cd44a0; entry 140cd44a0; auto-decompiled, types/names provisional.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined1 FUN_140cd44a0(longlong param_1,short *param_2)

{
  byte *pbVar1;
  byte bVar2;
  longlong *plVar3;
  char cVar4;
  short sVar5;
  short sVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  
  pbVar1 = (byte *)(param_1 + 0x3604);
  if (1 < *pbVar1) {
    if (*(int *)(param_1 + 0x3614) != *(int *)(param_1 + 0x35e8)) {
      *(undefined4 *)(param_1 + 0x3618) = 0x40000000;
      *(undefined8 *)(param_1 + 0x361c) = 0x42340000;
      *(undefined1 *)(param_1 + 0x3624) = 0;
      *(int *)(param_1 + 0x3614) = *(int *)(param_1 + 0x35e8);
    }
    if ((1 < *pbVar1) &&
       (cVar4 = FUN_140cd43d0(param_1,*(undefined8 *)(param_1 + 0x2cc0)), cVar4 == '\0')) {
      return 0;
    }
  }
  bVar2 = *pbVar1;
  if (bVar2 == 0) {
    *(undefined4 *)(param_1 + 0x3618) = 0x40000000;
    *(undefined8 *)(param_1 + 0x361c) = 0x42340000;
    *(undefined1 *)(param_1 + 0x3624) = 0;
    *(undefined4 *)(param_1 + 0x3614) = *(undefined4 *)(param_1 + 0x35e8);
    *pbVar1 = 1;
    *(undefined4 *)(param_1 + 0x3628) = 0;
  }
  else if (bVar2 != 1) {
    if (bVar2 != 2) {
      return 1;
    }
    plVar3 = *(longlong **)(param_1 + 0x24f8);
    if (((plVar3 != (longlong *)0x0) && (*plVar3 != 0)) && ((char)plVar3[1] == '\0')) {
      FUN_140cd6590(pbVar1);
      *(undefined1 *)(plVar3 + 1) = 1;
    }
    fVar8 = *(float *)(param_1 + 0x3618);
    if (DAT_1412ee864 <= *(float *)(param_1 + 0x3618)) {
      fVar8 = DAT_1412ee864;
    }
    if (*(char *)(param_1 + 0x3624) == '\0') {
      fVar9 = *(float *)(param_1 + 0x361c);
      fVar7 = fVar8 + *(float *)(param_1 + 0x3620);
      *(float *)(param_1 + 0x3620) = fVar7;
      if (fVar9 < fVar7) {
        *(float *)(param_1 + 0x3620) = fVar9;
        *(undefined1 *)(param_1 + 0x3624) = 1;
        fVar8 = fVar8 - (fVar7 - fVar9);
      }
    }
    else {
      fVar8 = 0.0;
    }
    fVar9 = (float)(int)*(short *)(param_1 + 0x2826) * DAT_1412f2620;
    sVar6 = (short)(int)(((float)(int)*param_2 * DAT_1412f2620 - fVar9) * DAT_141315cbc);
    sVar5 = (short)(int)(fVar8 * _DAT_1414ccca0);
    fVar8 = (float)(int)sVar6 * DAT_1412f2620;
    if ((fVar8 <= (float)(int)sVar5 * DAT_1412f2620) &&
       (fVar7 = (float)(int)sVar5 * _DAT_141378c6c, sVar5 = sVar6, fVar8 < fVar7)) {
      sVar5 = (short)(int)(fVar7 * DAT_141315cbc);
    }
    sVar5 = (short)(int)(((float)(int)sVar5 * DAT_1412f2620 + fVar9) * DAT_141315cbc);
    *(short *)(param_1 + 0x3606) = sVar5;
    *param_2 = sVar5;
    return 1;
  }
  sVar5 = *param_2;
  *(int *)(param_1 + 0x3628) = *(int *)(param_1 + 0x3628) + 1;
  *(short *)(param_1 + 0x3606) = sVar5;
  return 1;
}

