// Requested 140c27430; entry 140c27430; auto-decompiled, types/names provisional.

void FUN_140c27430(longlong *param_1,longlong param_2)

{
  longlong *plVar1;
  undefined8 *puVar2;
  char cVar3;
  
  while( true ) {
    param_1[10] = *(longlong *)(param_2 + 0x70);
    *(uint *)(param_1 + 0xb) = *(uint *)(param_1 + 0xb) & 0xfffffffe;
    plVar1 = (longlong *)*param_1;
    if (plVar1 != (longlong *)0x0) {
      cVar3 = (**(code **)(*plVar1 + 8))(plVar1,param_2,param_1);
      if (cVar3 == '\0') {
        puVar2 = (undefined8 *)*param_1;
        if (puVar2 != (undefined8 *)0x0) {
          (**(code **)*puVar2)(puVar2,1);
        }
      }
      else {
        puVar2 = (undefined8 *)param_1[1];
        if (puVar2 != (undefined8 *)0x0) {
          (**(code **)*puVar2)(puVar2,1);
        }
        param_1[1] = *param_1;
        *(uint *)(param_1 + 0xb) = *(uint *)(param_1 + 0xb) | 1;
      }
      *param_1 = 0;
    }
    plVar1 = (longlong *)param_1[1];
    if (plVar1 == (longlong *)0x0) break;
    (**(code **)(*plVar1 + 0x10))(plVar1,param_2,param_1);
    cVar3 = FUN_140c261c0(param_1);
    if (cVar3 == '\0') {
      return;
    }
  }
  return;
}

