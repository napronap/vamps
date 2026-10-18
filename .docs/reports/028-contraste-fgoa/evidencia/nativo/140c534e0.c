// Requested 140c534e0; entry 140c534e0; auto-decompiled, types/names provisional.

undefined8 FUN_140c534e0(longlong param_1,int *param_2)

{
  char cVar1;
  undefined8 uVar2;
  
  if (((*(int *)(param_1 + 0x35ec) != *param_2) || (*(int *)(param_1 + 0x35ec) == -1)) &&
     (param_2[0xf] != 2)) {
    cVar1 = FUN_140ccf0e0(param_1,*(undefined8 *)(param_2 + 2),*param_2,param_2[9]);
    if (cVar1 == '\0') {
      uVar2 = FUN_140ccdc00(param_1);
      return uVar2;
    }
  }
  return 0;
}

