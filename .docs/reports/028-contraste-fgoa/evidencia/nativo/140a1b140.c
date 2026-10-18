// Requested 140a1b140; entry 140a1b140; auto-decompiled, types/names provisional.

void FUN_140a1b140(longlong param_1,char *param_2,ulonglong param_3)

{
  byte bVar1;
  longlong lVar2;
  size_t sVar3;
  
  FUN_140a1b2f0(param_1,param_3 >> 3 & 0x1fffff01);
  lVar2 = FUN_140a1ac90(param_1);
  if (lVar2 != 0) {
    if (*param_2 == '\0') {
      sVar3 = 0;
    }
    else {
      sVar3 = strlen(param_2);
    }
    FUN_140070d00(param_1 + 0x30,param_2,sVar3);
    *(byte *)(param_1 + 0x50) = (byte)param_3 & 1;
    if ((*(char *)(param_1 + 9) == '\0') && ((*(byte *)(lVar2 + 0x1c) & 1) != 0)) {
      *(undefined2 *)(param_1 + 9) = 0x101;
    }
    if ((*(byte *)(lVar2 + 0x1c) & 2) == 0) {
      *(char *)(lVar2 + 0x18) = ((param_3 & 0x10) != 0) + '\x01';
      bVar1 = FUN_140a1a7d0(param_1);
      *(byte *)(lVar2 + 0x1c) = *(byte *)(lVar2 + 0x1c) & 0xfb;
      *(byte *)(lVar2 + 0x1c) = *(byte *)(lVar2 + 0x1c) | (bVar1 & 1) << 2 | 2;
    }
  }
  return;
}

