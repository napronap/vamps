// Requested 140c26230; entry 140c26230; auto-decompiled, types/names provisional.

undefined8 FUN_140c26230(longlong *param_1)

{
  longlong *plVar1;
  undefined8 *puVar2;
  longlong lVar3;
  
  if (param_1[7] != 0) {
    plVar1 = *(longlong **)(*(longlong *)(param_1[6] + 8) + 0x10);
    if (plVar1 != (longlong *)0x0) {
      puVar2 = (undefined8 *)*param_1;
      if (puVar2 != (undefined8 *)0x0) {
        (**(code **)*puVar2)(puVar2,1);
        *param_1 = 0;
        param_1[1] = 0;
      }
      lVar3 = (**(code **)(*plVar1 + 8))(plVar1);
      *param_1 = lVar3;
    }
    FUN_140c263a0(param_1);
    return 1;
  }
  return 0;
}

