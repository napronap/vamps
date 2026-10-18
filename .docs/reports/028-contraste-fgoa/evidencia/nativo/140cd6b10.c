// Requested 140cd6b10; entry 140cd6b10; auto-decompiled, types/names provisional.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140cd6b10(longlong param_1,short param_2,char param_3)

{
  int iVar1;
  float fVar2;
  float fVar3;
  uint uVar4;
  short sVar5;
  short sVar6;
  float fVar7;
  
  fVar7 = (float)FUN_140c6a0a0(param_1 + 0x33e0);
  fVar3 = DAT_141315cbc;
  fVar2 = DAT_1412f2620;
  sVar6 = (short)(int)(fVar7 * DAT_141315cbc);
  sVar5 = (short)(int)(((float)(int)param_2 * DAT_1412f2620 -
                       (float)(int)*(short *)(param_1 + 0x2826) * DAT_1412f2620) * DAT_141315cbc);
  if ((param_3 != '\0') &&
     ((_DAT_1414c5340 < (float)(int)sVar5 * DAT_1412f2620 ||
      ((float)(int)sVar5 * DAT_1412f2620 < _DAT_1414ccca8)))) {
    sVar6 = (short)(int)((float)(int)sVar6 * _DAT_1414ccc98 * DAT_141315cbc);
  }
  iVar1 = *(int *)(param_1 + 0x24e4);
  if (iVar1 == 1) {
    sVar6 = 0x71c;
  }
  else if (iVar1 == 2) {
    sVar6 = 0x1c71;
  }
  else if (iVar1 == 3) {
    sVar6 = 0x2aaa;
  }
  if ((float)(int)sVar5 * DAT_1412f2620 <= (float)(int)sVar6 * DAT_1412f2620) {
    fVar7 = (float)(int)sVar6 * _DAT_141378c6c;
    if (fVar7 <= (float)(int)sVar5 * DAT_1412f2620) {
      sVar6 = sVar5;
      if ((*(char *)(param_1 + 0x3604) == '\x01') && (1 < *(int *)(param_1 + 0x3628))) {
        *(undefined1 *)(param_1 + 0x3604) = 2;
      }
    }
    else {
      sVar6 = (short)(int)(fVar7 * DAT_141315cbc);
    }
  }
  uVar4 = (uint)(((float)(int)*(short *)(param_1 + 0x274c) + (float)(int)sVar6) * fVar2 * fVar3);
  FUN_140cd46a0(param_1,uVar4 & 0xffff);
  FUN_140cd1220(param_1,(float)(int)(short)uVar4 * fVar2);
  return;
}

