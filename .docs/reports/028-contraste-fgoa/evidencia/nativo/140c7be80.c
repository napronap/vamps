// Requested 140c7be80; entry 140c7be80; auto-decompiled, types/names provisional.

void FUN_140c7be80(longlong *param_1,int param_2)

{
  uint uVar1;
  char cVar2;
  char cVar3;
  undefined4 uVar4;
  uint *puVar5;
  float fVar6;
  double dVar7;
  undefined1 auStack_58 [32];
  undefined8 local_38;
  ulonglong local_30;
  
  local_30 = DAT_14198f720 ^ (ulonglong)auStack_58;
  if (((*(byte *)(param_1 + 8) & 1) == 0) && ((*(uint *)(*param_1 + 0x34b0) & 0x100) == 0)) {
    cVar2 = FUN_140c3ae10();
    local_38 = 0;
    cVar3 = FUN_140c6b510((float)(int)*(short *)(*param_1 + 0x2826) * DAT_1412f2620,&local_38,
                          *param_1,cVar2);
    if (cVar3 != '\0') {
      if (cVar2 == '\0') {
        puVar5 = (uint *)FUN_140c6b460(param_1[3]);
        uVar1 = *puVar5;
        dVar7 = fabs((double)(local_38._4_4_ * local_38._4_4_ + (float)local_38 * (float)local_38));
        if (DAT_1412c6f50 < (float)dVar7) {
          if (((uVar1 & 0x40) == 0) || (param_2 != 3)) {
            uVar4 = FUN_140ccdc60(*param_1);
          }
          else {
            uVar4 = 0;
          }
          if ((uVar1 & 0x40) != 0) {
            cVar2 = FUN_140ccf490(*param_1,uVar4,1);
            if (cVar2 != '\0') {
              fVar6 = atan2f(local_38._4_4_,(float)local_38);
              if (((param_2 == 3) || (cVar2 = FUN_140c3aae0(*param_1,fVar6), cVar2 == '\0')) ||
                 (true < (fVar6 <= 0.0))) {
                FUN_140c7ad10(param_1 + 4,&local_38);
              }
              else {
                FUN_140c7afc0(param_1 + 4,fVar6,fVar6 > 0.0,1);
              }
              goto LAB_140c7c02d;
            }
          }
          FUN_140c7b190(param_1 + 4,&local_38);
        }
      }
      else {
        FUN_140c7ad10(param_1 + 4,&local_38);
      }
    }
  }
LAB_140c7c02d:
  __security_check_cookie(local_30 ^ (ulonglong)auStack_58);
  return;
}

