// Requested 140c3be90; entry 140c3be90; auto-decompiled, types/names provisional.

void FUN_140c3be90(longlong param_1,undefined8 *param_2,longlong param_3)

{
  undefined1 uVar1;
  undefined8 uVar2;
  char cVar3;
  longlong lVar4;
  longlong lVar5;
  longlong *plVar6;
  undefined8 *puVar7;
  uint uVar8;
  double dVar9;
  double dVar10;
  longlong local_res10;
  undefined1 *local_38;
  undefined1 *local_30;
  
  local_38 = &DAT_1412c21a0;
  local_30 = &DAT_1412c21a0;
  lVar4 = (**(code **)(param_2[0x465] + 8))(param_2 + 0x465,&local_38);
  if (lVar4 != 0) {
    if (*(int *)(param_1 + 8) == 0) {
      FUN_140d10790(param_2,0);
      FUN_140c577c0(param_2 + 0x506);
      if (*(char *)(param_1 + 0x5b9) == '\x02') {
        FUN_140c6c050();
      }
      else {
        FUN_140c6c060(param_2 + 0x4dc);
      }
      cVar3 = *(char *)(param_1 + 0x5b8);
      uVar8 = DAT_1415ac6c0;
      if (cVar3 == '\0') {
        uVar8 = DAT_1415ac590;
      }
      local_res10 = (ulonglong)uVar8 << 0x20;
      puVar7 = (undefined8 *)operator_new(0x20);
      if (puVar7 == (undefined8 *)0x0) {
        puVar7 = (undefined8 *)0x0;
      }
      else {
        uVar1 = *(undefined1 *)(param_1 + 0x5b9);
        *(undefined4 *)(puVar7 + 3) = *(undefined4 *)(param_1 + 0x5c0);
        *puVar7 = rob::StepStateParam::vftable;
        *(longlong *)((longlong)puVar7 + 0xc) = local_res10;
        *(undefined1 *)((longlong)puVar7 + 0x14) = uVar1;
        *(uint *)(puVar7 + 1) = (uint)(cVar3 != '\0');
      }
      FUN_140c3efc0(lVar4,puVar7,1);
      *(undefined8 *)(param_1 + 0x10) = *(undefined8 *)(param_3 + 0x50);
      if ((*(byte *)(param_1 + 0x5c4) & 1) != 0) {
        local_38 = (undefined1 *)*param_2;
        local_30 = (undefined1 *)((ulonglong)local_30 & 0xffffffff00000000);
        lVar4 = FUN_1406bff80(&local_38);
        if (lVar4 != 0) {
          FUN_1406b4760(lVar4,0xe);
        }
      }
      FUN_140cd1320(param_2,*(undefined4 *)(param_1 + 0x5bc));
      if (*(char *)((longlong)param_2 + 0x24) != '\0') {
        FUN_140a517e0(*param_2);
      }
      cVar3 = FUN_140d0e2e0(param_2 + 0x5e2,1);
      if (cVar3 == '\0') {
        FUN_140a3f930(*param_2,0x20);
      }
      *(undefined4 *)(param_1 + 8) = 2;
    }
    else if ((*(int *)(param_1 + 8) == 2) && (*(char *)((longlong)param_2 + 0x25) == '\0')) {
      if (*(char *)(param_1 + 0x5b9) == '\x02') {
        FUN_140c6b150();
      }
      else {
        FUN_140c6b0c0(param_2 + 0x4dc,0);
      }
      lVar5 = FUN_140c6b4a0(param_2 + 0x4dc);
      if ((lVar5 == 0) ||
         (cVar3 = FUN_140c79ef0(param_1 + 0x18,param_2,param_3,lVar5), cVar3 == '\0')) {
        plVar6 = (longlong *)FUN_140235310(lVar4);
        if (plVar6 == (longlong *)0x0) {
          return;
        }
        dVar9 = (double)FUN_1402fba60(plVar6 + 0x1a,*(undefined8 *)(param_3 + 0x50),0);
        dVar10 = (double)(**(code **)(*plVar6 + 0xe8))(plVar6);
        if (dVar9 < dVar10) {
          return;
        }
        puVar7 = (undefined8 *)operator_new(0x28);
        if (puVar7 != (undefined8 *)0x0) {
          uVar2 = *(undefined8 *)(param_3 + 0x50);
          *puVar7 = RobActParamIdle::vftable;
          *(undefined4 *)(puVar7 + 1) = 2;
          puVar7[2] = uVar2;
          *(undefined4 *)(puVar7 + 3) = 0x41000000;
          *(undefined8 *)((longlong)puVar7 + 0x1c) = 1;
          *(undefined4 *)((longlong)puVar7 + 0x24) = 0xbf800000;
          FUN_140c26520(param_3);
          *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 1;
          return;
        }
        FUN_140c26520(param_3,0);
      }
      *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 1;
    }
  }
  return;
}

