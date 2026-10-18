// Requested 140c36ae0; entry 140c36ae0; auto-decompiled, types/names provisional.

uint FUN_140c36ae0(void)

{
  char cVar1;
  longlong lVar2;
  uint uVar3;
  byte bVar4;
  
  bVar4 = 0xff;
  lVar2 = FUN_1400dab90();
  cVar1 = FUN_140284560(lVar2);
  if (cVar1 != '\0') {
    bVar4 = *(byte *)(lVar2 + 0x2c4);
  }
  uVar3 = DAT_141988d24 & 0xffffff;
  DAT_141988d24 = DAT_141988d24 + 1;
  if (0xfffffe < DAT_141988d24) {
    FUN_140c36cb0();
  }
  return (uint)bVar4 << 0x18 | uVar3;
}

