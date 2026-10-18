// Requested 140c38f90; entry 140c38f90; auto-decompiled, types/names provisional.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140c38f90(longlong param_1,undefined8 *param_2,longlong param_3)

{
  longlong lVar1;
  char cVar2;
  longlong lVar3;
  longlong *plVar4;
  longlong lVar5;
  undefined8 *puVar6;
  longlong *plVar7;
  undefined8 *puVar8;
  float fVar9;
  float fVar10;
  undefined1 auStack_128 [32];
  undefined4 local_108;
  undefined4 local_100;
  undefined4 local_f8;
  undefined4 local_f0;
  float local_e8;
  undefined1 local_e0;
  undefined8 *local_d8;
  longlong local_d0;
  undefined1 *local_c8;
  undefined1 *local_c0;
  undefined8 *local_b8;
  longlong local_b0;
  undefined8 local_a8;
  undefined1 local_a0 [8];
  undefined8 local_98;
  undefined8 uStack_90;
  undefined8 local_88;
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78 [2];
  ulonglong local_70;
  
  local_a8 = 0xfffffffffffffffe;
  local_70 = DAT_14198f720 ^ (ulonglong)auStack_128;
  local_c8 = &DAT_1412c21a0;
  local_c0 = &DAT_1412c21a0;
  lVar3 = (**(code **)(param_2[0x465] + 8))(param_2 + 0x465,&local_c8);
  if (lVar3 == 0) goto LAB_140c39291;
  if (*(int *)(param_1 + 8) == 0) {
    FUN_140c6c060(param_2 + 0x4dc);
    fVar10 = 0.0;
    puVar8 = (undefined8 *)(param_1 + 0x648);
    local_d8 = puVar8;
    if (0xf < *(ulonglong *)(param_1 + 0x660)) {
      local_d8 = (undefined8 *)*puVar8;
    }
    local_d0 = (longlong)local_d8 + *(longlong *)(param_1 + 0x658);
    plVar4 = (longlong *)FUN_14034ab10(&local_d8);
    if (plVar4 != (longlong *)0x0) {
      plVar7 = (longlong *)*plVar4;
      if (plVar7 != (longlong *)plVar4[1]) {
        do {
          if (*(int *)(*plVar7 + 8) == 0xc) break;
          plVar7 = plVar7 + 1;
        } while (plVar7 != (longlong *)plVar4[1]);
      }
      if (((plVar7 != (longlong *)plVar4[1]) && (*plVar7 != 0)) &&
         (lVar1 = *(longlong *)(*plVar7 + 0x38), lVar1 != 0)) {
        lVar5 = 0;
        if (plVar7 != (longlong *)plVar4[1]) {
          lVar5 = lVar1;
        }
        fVar10 = (float)*(int *)(lVar5 + 8);
      }
    }
    FUN_1402fce60();
    fVar9 = DAT_1415ac6c0;
    if (0.0 < *(float *)(param_1 + 0x670)) {
      fVar9 = *(float *)(param_1 + 0x670) + fVar10;
    }
    puVar6 = (undefined8 *)operator_new(0x50);
    local_d8 = puVar6;
    if (puVar6 != (undefined8 *)0x0) {
      if (0xf < *(ulonglong *)(param_1 + 0x660)) {
        puVar8 = (undefined8 *)*puVar8;
      }
      local_b0 = (longlong)puVar8 + *(longlong *)(param_1 + 0x658);
      local_e0 = 0;
      local_f0 = 0;
      local_f8 = 0;
      local_100 = 1;
      local_108 = DAT_1415ac590;
      local_e8 = fVar9;
      local_b8 = puVar8;
      puVar6 = (undefined8 *)FUN_140c27610(puVar6,&local_b8);
    }
    thunk_FUN_140c3ef40(lVar3,puVar6,0);
    FUN_140c3c970(param_1 + 0x5c0);
    *(undefined8 *)(param_1 + 0x10) = *(undefined8 *)(param_3 + 0x50);
    local_a0[0] = 0;
    local_98 = _DAT_1412ca140;
    uStack_90 = _UNK_1412ca148;
    local_88 = 0xffffffffffffffff;
    local_80 = 0xffffffff;
    local_7c = 0;
    memset(local_78,0,4);
    local_a0[0] = 1;
    local_98 = *(undefined8 *)(param_3 + 0x50);
    uStack_90 = *param_2;
    local_88 = *(undefined8 *)(param_1 + 0x5c8);
    local_80 = *(undefined4 *)(param_1 + 0x5d0);
    local_7c = *(undefined4 *)(param_1 + 0x5d4);
    local_78[0] = *(undefined4 *)(param_1 + 0x5d8);
    FUN_140cb0cc0(local_a0);
    FUN_140cd0b00(param_2,0x16);
  }
  else {
    if ((*(int *)(param_1 + 8) != 1) || (*(char *)((longlong)param_2 + 0x25) != '\0'))
    goto LAB_140c39291;
    lVar3 = FUN_140ccd5a0(param_2);
    if (lVar3 != 0) {
      FUN_140c3c8a0(param_1 + 0x18,param_2);
    }
    FUN_140c3c840(param_1 + 0x18,param_2 + 0x4dc);
    FUN_140c6afe0(param_2 + 0x4dc,1);
    lVar3 = FUN_140c6b4a0(param_2 + 0x4dc);
    if ((lVar3 == 0) || (cVar2 = FUN_140c79ef0(param_1 + 0x28,param_2,param_3,lVar3), cVar2 == '\0')
       ) goto LAB_140c39291;
  }
  *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 1;
LAB_140c39291:
  __security_check_cookie(local_70 ^ (ulonglong)auStack_128);
  return;
}

