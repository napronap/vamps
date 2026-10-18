// Requested 140d10460; entry 140d10460; auto-decompiled, types/names provisional.

bool FUN_140d10460(longlong param_1,undefined8 param_2)

{
  double dVar1;
  double dVar2;
  char cVar3;
  longlong lVar4;
  longlong lVar5;
  
  lVar4 = FUN_140c3ea40(param_1 + 9000);
  if (((lVar4 != 0) &&
      (DAT_141317b80 < *(float *)(lVar4 + 0x8c) || DAT_141317b80 == *(float *)(lVar4 + 0x8c))) &&
     (lVar5 = FUN_1402352f0(param_1 + 9000), lVar5 != 0)) {
    dVar2 = (double)FUN_1402fba60(lVar5 + 0xd0,param_2,0);
    dVar1 = (double)FUN_1402fce60();
    if ((float)(dVar2 / dVar1) < *(float *)(lVar4 + 0x8c)) {
      return true;
    }
  }
  cVar3 = FUN_140d105f0(param_1 + 0x2d68,param_2);
  return cVar3 != '\0';
}

