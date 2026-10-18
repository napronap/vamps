// Requested 140c573e0; entry 140c573e0; auto-decompiled, types/names provisional.

void FUN_140c573e0(longlong param_1,undefined8 param_2,undefined8 param_3,undefined4 param_4,
                  undefined4 param_5,undefined4 *param_6,float param_7,longlong param_8)

{
  longlong *plVar1;
  longlong lVar2;
  longlong lVar3;
  longlong *plVar4;
  float fVar5;
  float fVar6;
  undefined1 *local_80;
  undefined1 *local_78;
  
  FUN_140c529f0();
  fVar6 = 0.0;
  plVar1 = (longlong *)FUN_14034ab10(param_2);
  if (plVar1 != (longlong *)0x0) {
    plVar4 = (longlong *)*plVar1;
    if (plVar4 != (longlong *)plVar1[1]) {
      do {
        if (*(int *)(*plVar4 + 8) == 0xc) break;
        plVar4 = plVar4 + 1;
      } while (plVar4 != (longlong *)plVar1[1]);
    }
    if (((plVar4 != (longlong *)plVar1[1]) && (*plVar4 != 0)) &&
       (lVar3 = *(longlong *)(*plVar4 + 0x38), lVar3 != 0)) {
      lVar2 = 0;
      if (plVar4 != (longlong *)plVar1[1]) {
        lVar2 = lVar3;
      }
      fVar6 = (float)*(int *)(lVar2 + 8);
    }
  }
  fVar5 = (float)FUN_140ce5620(param_2);
  if ((0.0 < fVar5) && (param_7 <= fVar5 - fVar6)) {
    param_7 = fVar5 - fVar6;
  }
  FUN_140d10790(param_1,param_7);
  if (*(char *)(param_1 + 0x25) == '\0') {
    local_80 = (undefined1 *)operator_new(200);
    if (local_80 == (undefined1 *)0x0) {
      lVar3 = 0;
    }
    else {
      lVar3 = FUN_140c4e990(local_80,param_2,*(undefined8 *)(param_1 + 0x70),param_7,param_3,param_4
                            ,param_5,*param_6);
    }
    plVar1 = *(longlong **)(param_8 + 0x38);
    if (plVar1 != (longlong *)0x0) {
      if (lVar3 == 0) {
        local_80 = (undefined1 *)0x0;
      }
      else {
        local_80 = (undefined1 *)(lVar3 + 0x10);
      }
      (**(code **)(*plVar1 + 0x10))(plVar1,param_6,&local_80);
    }
    local_80 = &DAT_1412c21a0;
    local_78 = &DAT_1412c21a0;
    FUN_140c4c560(param_1 + 0x2318,lVar3,&local_80);
  }
  return;
}

