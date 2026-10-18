// Requested 140a15ac0; entry 140a15ac0; auto-decompiled, types/names provisional.

ulonglong FUN_140a15ac0(longlong *param_1,undefined8 param_2,undefined1 *param_3,uint *param_4)

{
  char cVar1;
  byte bVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  longlong lVar5;
  undefined1 *puVar6;
  
  *param_3 = 0xff;
  *param_4 = 0;
  uVar4 = 0;
  uVar3 = FUN_140a16110();
  if ((int)uVar3 < 0) {
    uVar4 = uVar3 & 0xffffffffffffff00;
  }
  else {
    lVar5 = FUN_140a160f0(param_1,param_2);
    cVar1 = FUN_140a16a00((longlong)(int)uVar3 * 0x2e0 + *param_1);
    if ((((cVar1 == '\0') && (lVar5 != 0)) && (*(longlong *)(lVar5 + 0x40) == 0)) &&
       (puVar6 = (undefined1 *)FUN_140a1ac90(lVar5), puVar6 != (undefined1 *)0x0)) {
      *param_3 = *puVar6;
      bVar2 = FUN_140a1a8a0(lVar5);
      puVar6[0x1c] = puVar6[0x1c] & 0xfe;
      puVar6[0x1c] = puVar6[0x1c] | bVar2 & 1;
      if ((puVar6[0x1c] & 1) != 0) {
        *param_4 = *param_4 | 1;
      }
      cVar1 = FUN_140a1ad80(lVar5);
      if (cVar1 != '\0') {
        *param_4 = *param_4 | 2;
      }
      uVar4 = 1;
    }
  }
  return uVar4;
}

