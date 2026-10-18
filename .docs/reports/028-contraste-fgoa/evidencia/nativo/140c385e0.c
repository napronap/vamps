// Requested 140c385e0; entry 140c385e0; auto-decompiled, types/names provisional.

undefined8 FUN_140c385e0(longlong param_1,undefined8 *param_2)

{
  undefined1 auVar1 [16];
  char cVar2;
  undefined4 uVar3;
  longlong lVar4;
  undefined8 *puVar5;
  ulonglong uVar6;
  bool bVar7;
  float fVar8;
  undefined8 local_28;
  undefined4 local_20;
  
  if (*(char *)((longlong)param_2 + 0x25) == '\0') {
    bVar7 = *(int *)(param_1 + 0x5e0) != 2;
    if (bVar7) {
      uVar3 = FUN_140c36ae0();
      *(undefined4 *)(param_2 + 0x6bd) = uVar3;
    }
    if (*(int *)(param_1 + 0x5e0) == 3) {
      FUN_140a1be70(*param_2);
      FUN_140d108e0(param_2);
    }
    local_28 = *param_2;
    local_20 = 0;
    lVar4 = FUN_1406bff80(&local_28);
    if (lVar4 != 0) {
      uVar3 = *(undefined4 *)(param_2 + 0x6bd);
      FUN_1406b3200(lVar4,uVar3,*(undefined4 *)(param_1 + 0x5f8));
      puVar5 = (undefined8 *)FUN_1401a0b10(lVar4 + 0x1b0,uVar3);
      if (puVar5 != (undefined8 *)0x0) {
        *(undefined4 *)(param_1 + 0x5e8) = uVar3;
        *(undefined8 *)(param_1 + 0x5ec) = *puVar5;
      }
    }
    if (bVar7) {
      uVar6 = 0;
      cVar2 = FUN_140745120();
      if (cVar2 == '\0') {
        if (*(char *)(param_1 + 0x5ec) < '\x01') {
          lVar4 = 0;
          fVar8 = DAT_1412ee85c;
          if ((DAT_1412ee86c <= DAT_1412ee85c) &&
             (fVar8 = DAT_1412ee85c - DAT_1412ee86c, fVar8 < DAT_1412ee86c)) {
            lVar4 = -0x8000000000000000;
          }
          uVar6 = (longlong)fVar8 + lVar4;
        }
      }
      else {
        lVar4 = FUN_1400dab90();
        uVar6 = *(longlong *)(lVar4 + 0x288) * 0x78;
        auVar1._8_8_ = 0;
        auVar1._0_8_ = uVar6;
        lVar4 = SUB168(ZEXT816(0x624dd2f1a9fbe77) * auVar1,8);
        uVar6 = (uVar6 - lVar4 >> 1) + lVar4 >> 9;
        if (6 < uVar6) {
          uVar6 = 6;
        }
      }
      fVar8 = (float)(longlong)uVar6;
      if ((longlong)uVar6 < 0) {
        fVar8 = fVar8 + DAT_1412fb414;
      }
      *(float *)(param_1 + 0x63c) = fVar8;
      *(undefined4 *)(param_1 + 0x640) = 0;
    }
  }
  else {
    *(undefined4 *)(param_2 + 0x6bd) = *(undefined4 *)(param_1 + 0x5e8);
    local_28 = *param_2;
    local_20 = 0;
    lVar4 = FUN_1406bff80(&local_28);
    if (lVar4 != 0) {
      FUN_1406b31f0(lVar4,*(undefined4 *)(param_2 + 0x6bd),param_1 + 0x5ec);
      return 1;
    }
  }
  return 1;
}

