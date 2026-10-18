// Requested 140c525a0; entry 140c525a0; auto-decompiled, types/names provisional.

undefined8 FUN_140c525a0(longlong param_1)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  
  cVar1 = FUN_140ccef70();
  if ((cVar1 != '\0') || ((*(uint *)(param_1 + 0x2720) >> 3 & 1) != 0)) {
    uVar3 = FUN_140c52460(param_1);
    return uVar3;
  }
  iVar2 = FUN_140c53a80(param_1,2);
  if (iVar2 != 0) {
    uVar3 = FUN_140c4c500(param_1 + 0x2318);
    iVar2 = FUN_140c4c460(uVar3);
    if ((iVar2 == 2) || (iVar2 - 0xbU < 2)) {
      return 1;
    }
  }
  return 0;
}

