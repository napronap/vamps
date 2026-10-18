// Requested 140c7c050; entry 140c7c050; auto-decompiled, types/names provisional.

undefined8 FUN_140c7c050(longlong param_1,int param_2)

{
  char cVar1;
  
  if ((((*(byte *)(param_1 + 0x40) & 1) == 0) || (param_2 != 0)) &&
     (cVar1 = FUN_140c7c260(), cVar1 != '\0')) {
    return 1;
  }
  if ((((*(uint *)(param_1 + 0x40) >> 10 & 1) != 0) && (param_2 != 0)) &&
     (cVar1 = FUN_140c7c0b0(param_1), cVar1 != '\0')) {
    return 1;
  }
  return 0;
}

