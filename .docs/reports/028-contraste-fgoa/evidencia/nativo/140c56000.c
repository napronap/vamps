// Requested 140c56000; entry 140c56000; auto-decompiled, types/names provisional.

longlong * FUN_140c56000(longlong *param_1,longlong *param_2,undefined4 param_3,undefined4 *param_4)

{
  undefined4 uVar1;
  longlong lVar2;
  longlong lVar3;
  longlong *plVar4;
  longlong lVar5;
  
  plVar4 = (longlong *)FUN_140ccd580();
  if (plVar4 != (longlong *)0x0) {
    lVar2 = *plVar4;
    if (lVar2 == 0) {
      plVar4 = (longlong *)((ulonglong)plVar4 & 0xffffffffffffff00);
    }
    else {
      *(longlong *)(param_4 + 2) = *param_1;
      *param_4 = (int)param_1[0x6bd];
      lVar5 = FUN_140c53a70(param_4);
      if (lVar5 != 0) {
        *(byte *)((longlong)param_4 + 0x46) = *(byte *)(lVar5 + 6) >> 1 & 1;
      }
      lVar5 = param_1[0x599];
      *(longlong *)(param_4 + 4) = param_1[0x598];
      *(longlong *)(param_4 + 6) = lVar5;
      *(bool *)(param_4 + 8) = param_1[0x598] == *param_2;
      lVar5 = *param_1;
      lVar3 = param_2[0x598];
      param_4[9] = param_3;
      *(bool *)((longlong)param_4 + 0x21) = lVar3 == lVar5;
      param_4[10] = *(undefined4 *)(lVar2 + 0x30);
      param_4[0xb] = 0;
      param_4[0xd] = (int)plVar4[0xb];
      param_4[0xe] = *(undefined4 *)(lVar2 + 0x20);
      param_4[0xf] = 1;
      param_4[0x10] = *(undefined4 *)(lVar2 + 0x1c);
      *(byte *)(param_4 + 0x11) = (byte)(*(uint *)(lVar2 + 0x34) >> 1) & 1;
      param_4[0x12] = *(undefined4 *)(lVar2 + 0x34);
      param_4[0x13] = (float)(int)*(float *)((longlong)plVar4 + 0x44);
      param_4[0x14] = (float)(int)*(float *)((longlong)plVar4 + 0x44);
      param_4[0x15] = (float)(int)*(float *)((longlong)plVar4 + 0x44);
      uVar1 = *(undefined4 *)((longlong)plVar4 + 0x3c);
      param_4[0x16] = uVar1;
      plVar4 = (longlong *)CONCAT71((uint7)(uint3)((uint)uVar1 >> 8),1);
    }
  }
  return plVar4;
}

