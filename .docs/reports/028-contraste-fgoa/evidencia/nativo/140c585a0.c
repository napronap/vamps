// Requested 140c585a0; entry 140c585a0; auto-decompiled, types/names provisional.

void FUN_140c585a0(longlong *param_1,longlong *param_2,longlong param_3,uint *param_4)

{
  undefined8 *puVar1;
  longlong lVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  longlong lVar7;
  undefined8 *puVar8;
  undefined8 *puVar9;
  undefined8 *puVar10;
  bool bVar11;
  double dVar12;
  float fVar13;
  float _Y;
  float _X;
  undefined1 auStack_e8 [32];
  longlong local_c8;
  undefined4 local_c0;
  longlong local_b8;
  float local_b0;
  longlong local_a8;
  longlong local_a0;
  uint local_98;
  ulonglong local_88;
  
  local_88 = DAT_14198f720 ^ (ulonglong)auStack_e8;
  lVar7 = FUN_140ccd580();
  if (lVar7 != 0) {
    puVar1 = *(undefined8 **)(lVar7 + 8);
    lVar2 = *param_2;
    cVar3 = *(char *)((longlong)puVar1[1] + 0x19);
    puVar9 = puVar1;
    puVar8 = (undefined8 *)puVar1[1];
    while (cVar3 == '\0') {
      if ((longlong)puVar8[4] < lVar2) {
        puVar10 = (undefined8 *)puVar8[2];
        puVar8 = puVar9;
      }
      else {
        puVar10 = (undefined8 *)*puVar8;
      }
      puVar9 = puVar8;
      puVar8 = puVar10;
      cVar3 = *(char *)((longlong)puVar10 + 0x19);
    }
    if ((puVar9 == puVar1) || (lVar2 < (longlong)puVar9[4])) {
      puVar9 = puVar1;
    }
    if (puVar9 == puVar1) {
      local_a0 = CONCAT44(local_a0._4_4_,*param_4);
      local_a8 = lVar2;
      local_c8 = FUN_140c4d280(lVar7 + 8,&local_a8);
      FUN_140c4dd10(lVar7 + 8,&local_a8,0,local_c8 + 0x20);
    }
    else {
      *(uint *)(puVar9 + 5) = *param_4;
    }
    *(uint *)(lVar7 + 0x18) = *(uint *)(lVar7 + 0x18) | *param_4;
    *(uint *)((longlong)param_1 + 0x2e2c) = *(uint *)((longlong)param_1 + 0x2e2c) | *param_4;
    if (param_1[0x598] == *param_2) {
      *(uint *)(lVar7 + 0x1c) = *param_4;
      *(uint *)(param_1 + 0x5c5) = *param_4;
      param_1[0x5c8] = param_1[0xe];
    }
    *(longlong *)(lVar7 + 0x20) = param_1[0xe];
    FUN_140c50220(param_1,param_4);
    cVar3 = FUN_140c55cb0(param_2,param_4,*(undefined4 *)(param_3 + 0x24));
    bVar11 = false;
    if (cVar3 != '\0') {
      iVar4 = FUN_140c53ab0(param_4);
      if (iVar4 < 3) {
        iVar5 = FUN_140ccda70(param_1);
        lVar7 = FUN_140ccd5e0(param_1);
        bVar11 = false;
        if (((ulonglong)(longlong)iVar5 <
             (ulonglong)
             (param_1[(longlong)iVar4 * 3 + 0x4a7] - param_1[(longlong)iVar4 * 3 + 0x4a6] >> 3)) &&
           (lVar2 = *(longlong *)(param_1[(longlong)iVar4 * 3 + 0x4a6] + (longlong)iVar5 * 8),
           lVar2 != 0)) {
          uVar6 = FUN_140c515f0(lVar2,lVar7);
          fVar13 = (float)(int)*(short *)((longlong)param_1 + 0x2826) * DAT_1412f2620;
          if (*(int *)(lVar2 + 0x34) == 1) {
            local_b8 = param_2[0x4fb];
            local_b0 = *(float *)(param_2 + 0x4fc);
            local_a8 = param_1[0x4fb];
            _Y = (float)local_b8 - (float)local_a8;
            local_a0 = CONCAT44(local_a0._4_4_,*(float *)(param_1 + 0x4fc));
            _X = local_b0 - *(float *)(param_1 + 0x4fc);
            dVar12 = fabs((double)(_Y * _Y + _X * _X));
            if (DAT_1412c6f50 < (float)dVar12) {
              fVar13 = atan2f(_Y,_X);
            }
          }
          if ((0.0 < *(float *)(lVar2 + 0x14) || *(float *)(lVar2 + 0x14) == 0.0) ||
             (0.0 < *(float *)(lVar2 + 0x18) || *(float *)(lVar2 + 0x18) == 0.0)) {
            local_a8 = param_1[0x4fb];
            local_a0 = CONCAT44(local_a0._4_4_,(int)param_1[0x4fc]);
            local_c8 = lVar7;
            local_c0 = uVar6;
            FUN_140c56fd0(param_2,lVar2,fVar13,&local_a8);
            bVar11 = false;
            if (*(char *)(lVar2 + 0x30) != '\0') {
              bVar11 = true;
            }
          }
          else {
            local_c8 = CONCAT44(local_c8._4_4_,uVar6);
            FUN_140c56df0(param_2,lVar2,fVar13,lVar7);
            bVar11 = *(char *)(lVar2 + 0x30) != '\0';
          }
        }
      }
    }
    if (*(char *)(param_3 + 0x46) == '\0') {
      local_a8 = *param_1;
    }
    else {
      local_a8 = -1;
    }
    local_a0 = *param_2;
    local_98 = *param_4;
    if (bVar11) {
      local_98 = local_98 | 0x400;
    }
    FUN_140caf050(&local_a8);
  }
  __security_check_cookie(local_88 ^ (ulonglong)auStack_e8);
  return;
}

