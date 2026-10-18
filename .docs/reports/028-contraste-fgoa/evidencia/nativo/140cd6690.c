// Requested 140cd6690; entry 140cd6690; auto-decompiled, types/names provisional.

void FUN_140cd6690(longlong param_1)

{
  int iVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  char cVar4;
  char cVar5;
  int iVar6;
  undefined8 uVar7;
  longlong lVar8;
  longlong lVar9;
  undefined8 *puVar10;
  short sVar11;
  uint uVar12;
  undefined8 *puVar13;
  float fVar14;
  undefined2 local_res10 [4];
  
  cVar4 = FUN_140c30bb0();
  if (cVar4 != '\0') {
    return;
  }
  if ((double)*(float *)(param_1 + 0x273c) <= DAT_1415ac558) {
    return;
  }
  if ((*(uint *)(param_1 + 0x34b0) & 0x400) != 0) {
    return;
  }
  if ((*(byte *)(param_1 + 0x24e8) & 1) != 0) {
    return;
  }
  cVar4 = FUN_140ccf340(param_1);
  if (cVar4 != '\0') {
    return;
  }
  uVar7 = FUN_140c4c500(param_1 + 0x2318);
  iVar6 = FUN_140c4c460(uVar7);
  if (iVar6 == 0x15) {
    return;
  }
  if (*(int *)(param_1 + 0x24e0) == 1) {
    return;
  }
  if (*(int *)(param_1 + 0x24e0) == 2) {
    FUN_140cd6b10(param_1,(int)(*(float *)(param_1 + 0x2cec) * DAT_141315cbc),0);
    return;
  }
  if ((*(uint *)(param_1 + 0x23f0) >> 6 & 1) != 0) {
    if (*(longlong *)(param_1 + 0x2cc0) == -1) {
      return;
    }
    lVar8 = FUN_140cdfdb0();
    if (lVar8 == 0) {
      return;
    }
    lVar8 = FUN_140ccd770(param_1);
    if (lVar8 == 0) {
      return;
    }
    local_res10[0] = (undefined2)(int)(*(float *)(param_1 + 0x2cec) * DAT_141315cbc);
    cVar4 = FUN_140cd44a0(param_1,local_res10);
    if (cVar4 == '\0') {
      return;
    }
    FUN_140cd6b10(param_1,local_res10[0],0);
    return;
  }
  lVar8 = -1;
  *(undefined1 *)(param_1 + 0x3604) = 0;
  *(undefined4 *)(param_1 + 0x3614) = 0xffffffff;
  *(undefined8 *)(param_1 + 0x360c) = 0;
  *(undefined4 *)(param_1 + 0x3608) = 0;
  *(undefined4 *)(param_1 + 0x3618) = 0x40000000;
  *(undefined8 *)(param_1 + 0x361c) = 0x42340000;
  *(undefined1 *)(param_1 + 0x3624) = 0;
  *(undefined4 *)(param_1 + 0x3628) = 0;
  uVar7 = FUN_140c4c500(param_1 + 0x2318);
  iVar6 = FUN_140c4c460(uVar7);
  if ((1 < iVar6 - 0xbU) && (iVar6 != 8)) {
    lVar8 = FUN_1403c8140(param_1);
    if (lVar8 != 0) {
      cVar4 = FUN_140cd43d0(param_1,*(undefined8 *)(param_1 + 0x2cc0));
      if (cVar4 == '\0') {
        return;
      }
      FUN_140cd6b10(param_1,(int)(*(float *)(param_1 + 0x2ce8) * DAT_141315cbc),0);
      return;
    }
    if ((*(uint *)(param_1 + 0x24e8) >> 4 & 1) == 0) {
      return;
    }
    cVar4 = FUN_140cd43d0(param_1,*(undefined8 *)(param_1 + 0x2cc0));
    if (cVar4 == '\0') {
      return;
    }
    FUN_140cd6b10(param_1,(int)(*(float *)(param_1 + 0x2ce8) * DAT_141315cbc),1);
    return;
  }
  cVar4 = *(char *)(param_1 + 0x2f08);
  *(undefined1 *)(param_1 + 0x2f08) = 0;
  if (*(char *)(param_1 + 0x25) != '\0') {
    return;
  }
  if (*(char *)(param_1 + 0x2e96) == '\0') {
    lVar8 = *(longlong *)(param_1 + 0x2e58);
  }
  lVar9 = FUN_140cdff20(lVar8);
  if (lVar9 == 0) {
    return;
  }
  if (lVar8 == *(longlong *)(param_1 + 0x2cc0)) {
    iVar6 = *(int *)(param_1 + 0x2cc8);
  }
  else {
    iVar6 = FUN_140ccdad0(lVar9);
  }
  iVar1 = *(int *)(param_1 + 0x2e7c);
  if (((lVar8 != -1) && (lVar9 = FUN_140cdfdb0(lVar8), lVar9 != 0)) &&
     ((*(uint *)(lVar9 + 0x24e8) >> 3 & 1) != 0)) {
    return;
  }
  if ((*(uint *)(param_1 + 0x2eec) & 0xa00) != 0) {
    return;
  }
  if (*(int *)(param_1 + 0x2eb0) == 2) {
    if (cVar4 == '\0') {
      return;
    }
    fVar14 = *(float *)(param_1 + 0x2e80);
  }
  else {
    cVar5 = FUN_140cd43d0(param_1,lVar8);
    if (cVar5 == '\0') {
      return;
    }
    puVar2 = *(undefined8 **)(param_1 + 0x2d10);
    cVar5 = *(char *)((longlong)puVar2[1] + 0x19);
    puVar10 = puVar2;
    puVar3 = (undefined8 *)puVar2[1];
    while (cVar5 == '\0') {
      if (((longlong)puVar3[4] < lVar8) ||
         (((longlong)puVar3[4] <= lVar8 && (*(int *)(puVar3 + 5) < iVar6)))) {
        puVar13 = (undefined8 *)puVar3[2];
      }
      else {
        puVar13 = (undefined8 *)*puVar3;
        puVar10 = puVar3;
      }
      puVar3 = puVar13;
      cVar5 = *(char *)((longlong)puVar13 + 0x19);
    }
    if (((puVar10 == puVar2) || (lVar8 < (longlong)puVar10[4])) ||
       ((lVar8 <= (longlong)puVar10[4] && (iVar6 < *(int *)(puVar10 + 5))))) {
      puVar10 = puVar2;
    }
    if (puVar10 == puVar2) {
      return;
    }
    uVar12 = (uint)(*(float *)(puVar10 + 6) * DAT_141315cbc);
    sVar11 = (short)uVar12;
    if (iVar1 == 1) {
      fVar14 = (float)(int)sVar11 * DAT_1412f2620 + DAT_1415ac668;
    }
    else if (iVar1 == 2) {
      fVar14 = (float)(int)sVar11 * DAT_1412f2620 + DAT_1412f729c;
    }
    else {
      if (iVar1 != 3) goto LAB_140cd6a89;
      fVar14 = (float)(int)sVar11 * DAT_1412f2620 - DAT_1412f729c;
    }
  }
  uVar12 = (uint)(fVar14 * DAT_141315cbc);
LAB_140cd6a89:
  if ((*(uint *)(param_1 + 0x24e8) >> 4 & 1) != 0) {
    uVar12 = (uint)(*(float *)(param_1 + 0x2ce8) * DAT_141315cbc);
  }
  if (((*(longlong *)(param_1 + 0x2c68) != 0) || (*(longlong *)(param_1 + 0x2c78) != 0)) ||
     (cVar4 != '\0')) {
    FUN_140cd6b10(param_1,uVar12 & 0xffff,cVar4);
  }
  return;
}

