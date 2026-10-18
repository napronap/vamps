// Requested 140c5680c; entry 140c56770; auto-decompiled, types/names provisional.

undefined8 FUN_140c56770(undefined8 *param_1,undefined8 *param_2,uint *param_3,undefined8 *param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  char cVar5;
  char cVar6;
  char cVar7;
  undefined4 uVar8;
  longlong lVar9;
  undefined8 uVar10;
  undefined8 local_28;
  undefined4 local_20;
  
  uVar8 = FUN_140ccdb60(param_1,*(undefined4 *)((longlong)param_4 + 0x24));
  local_28 = *param_1;
  local_20 = uVar8;
  lVar9 = FUN_1406bff80(&local_28);
  if (lVar9 == 0) {
    return 0;
  }
  *(undefined4 *)param_2 = *(undefined4 *)((longlong)param_4 + 0x3c);
  *(undefined4 *)(param_2 + 3) = *(undefined4 *)param_4;
  uVar1 = *(undefined4 *)(param_4 + 2);
  uVar2 = *(undefined4 *)((longlong)param_4 + 0x14);
  uVar3 = *(undefined4 *)(param_4 + 3);
  uVar4 = *(undefined4 *)((longlong)param_4 + 0x1c);
  *(undefined4 *)(param_2 + 6) = uVar8;
  *(undefined4 *)(param_2 + 4) = uVar1;
  *(undefined4 *)((longlong)param_2 + 0x24) = uVar2;
  *(undefined4 *)(param_2 + 5) = uVar3;
  *(undefined4 *)((longlong)param_2 + 0x2c) = uVar4;
  *(uint *)((longlong)param_2 + 0x3c) = *param_3;
  cVar5 = FUN_1406c07f0(lVar9);
  cVar6 = FUN_1406c07e0(lVar9);
  FUN_1406c5ff0(lVar9,*(undefined4 *)(param_2 + 7),param_2);
  cVar7 = FUN_1406c07e0(lVar9);
  if (cVar7 == '\0') {
    if (cVar6 == '\0') goto LAB_140c56830;
  }
  else if (cVar6 == '\0') {
    *param_3 = *param_3 | 0x80;
    goto LAB_140c56830;
  }
  *param_3 = *param_3 | 0x20000;
LAB_140c56830:
  *(uint *)((longlong)param_2 + 0x3c) = *param_3;
  if (*(char *)((longlong)param_2 + 0x52) != '\0') {
    FUN_140c7e960(param_2);
  }
  *(undefined4 *)(param_2 + 6) = *(undefined4 *)((longlong)param_4 + 0x24);
  uVar10 = param_4[1];
  param_1[0x5ca] = *param_4;
  param_1[0x5cb] = uVar10;
  uVar10 = param_4[3];
  param_1[0x5cc] = param_4[2];
  param_1[0x5cd] = uVar10;
  uVar10 = param_4[5];
  param_1[0x5ce] = param_4[4];
  param_1[0x5cf] = uVar10;
  uVar10 = param_4[7];
  param_1[0x5d0] = param_4[6];
  param_1[0x5d1] = uVar10;
  uVar10 = param_4[9];
  param_1[0x5d2] = param_4[8];
  param_1[0x5d3] = uVar10;
  uVar10 = param_4[0xb];
  param_1[0x5d4] = param_4[10];
  param_1[0x5d5] = uVar10;
  uVar10 = param_2[1];
  param_1[0x5d6] = *param_2;
  param_1[0x5d7] = uVar10;
  uVar10 = param_2[3];
  param_1[0x5d8] = param_2[2];
  param_1[0x5d9] = uVar10;
  uVar10 = param_2[5];
  param_1[0x5da] = param_2[4];
  param_1[0x5db] = uVar10;
  uVar10 = param_2[7];
  param_1[0x5dc] = param_2[6];
  param_1[0x5dd] = uVar10;
  uVar8 = *(undefined4 *)((longlong)param_2 + 0x44);
  uVar1 = *(undefined4 *)(param_2 + 9);
  uVar2 = *(undefined4 *)((longlong)param_2 + 0x4c);
  *(undefined4 *)(param_1 + 0x5de) = *(undefined4 *)(param_2 + 8);
  *(undefined4 *)((longlong)param_1 + 0x2ef4) = uVar8;
  *(undefined4 *)(param_1 + 0x5df) = uVar1;
  *(undefined4 *)((longlong)param_1 + 0x2efc) = uVar2;
  param_1[0x5e0] = param_2[10];
  *(undefined1 *)(param_1 + 0x5e1) = 1;
  if ((((param_2[2] == -1) || (*(int *)(param_2 + 6) != 0)) ||
      (cVar6 = FUN_1406c07f0(lVar9), cVar6 == '\0')) || (cVar5 != '\0')) {
    uVar10 = 0;
  }
  else {
    uVar10 = 1;
  }
  return uVar10;
}

