// Requested 140c521d0; entry 140c521d0; auto-decompiled, types/names provisional.

ulonglong FUN_140c521d0(undefined8 *param_1,longlong param_2,uint *param_3)

{
  char cVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  ulonglong uVar5;
  longlong lVar6;
  ulonglong uVar7;
  
  uVar5 = FUN_140c52340();
  if ((((char)uVar5 == '\0') ||
      (((uVar5 = FUN_140c53a70(param_2), uVar5 != 0 && ((*(byte *)(uVar5 + 5) & 2) != 0)) ||
       (*(char *)(param_2 + 0x44) != '\0')))) ||
     (uVar5 = FUN_140c55df0(param_1,param_2), (char)uVar5 != '\0')) {
    return uVar5 & 0xffffffffffffff00;
  }
  cVar1 = FUN_140c55d70(param_1);
  if ((cVar1 == '\0') && (cVar2 = FUN_140c52560(param_1,param_2), cVar2 == '\0')) {
    uVar5 = FUN_140c52580(param_1,param_2);
    if (((char)uVar5 != '\0') || (uVar5 = FUN_140a1b8d0(*param_1), (char)uVar5 != '\0')) {
LAB_140c52310:
      return uVar5 & 0xffffffffffffff00;
    }
    cVar2 = FUN_140c525a0(param_1);
    if (cVar2 == '\0') {
      lVar6 = FUN_140ccd770(param_1);
      if (lVar6 != 0) {
        uVar5 = FUN_140a1b970(*param_1);
        if ((char)uVar5 == '\0') goto LAB_140c52310;
        if ((*(int *)(lVar6 + 0x54) - 1U & 0xfffffffd) == 0) goto LAB_140c52314;
      }
      lVar6 = FUN_140c4c500(param_1 + 0x463);
      if ((((lVar6 == 0) || (iVar3 = FUN_140c27050(lVar6), iVar3 != 0xb)) ||
          ((*(longlong *)(lVar6 + 8) == 0 || (*(char *)(*(longlong *)(lVar6 + 8) + 0x20) == '\0'))))
         && ((((uVar7 = FUN_140c4c500(param_1 + 0x463), uVar5 = uVar7, uVar7 == 0 ||
               (uVar5 = FUN_140c27050(uVar7), (int)uVar5 != 0xc)) ||
              (uVar5 = *(ulonglong *)(uVar7 + 8), uVar5 == 0)) || (*(char *)(uVar5 + 0x20) == '\0'))
            )) goto LAB_140c52310;
    }
  }
LAB_140c52314:
  *param_3 = *param_3 | 2;
  uVar4 = *param_3;
  if (cVar1 != '\0') {
    uVar4 = uVar4 | 0x200000;
    *param_3 = uVar4;
  }
  return CONCAT71((uint7)(uint3)(uVar4 >> 8),1);
}

