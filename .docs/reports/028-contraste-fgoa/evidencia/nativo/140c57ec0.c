// Requested 140c57ec0; entry 140c57ec0; auto-decompiled, types/names provisional.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140c57ec0(ulonglong *param_1,int *param_2,uint *param_3,undefined ***param_4)

{
  int iVar1;
  undefined **ppuVar2;
  ulonglong uVar3;
  char cVar4;
  char cVar5;
  undefined1 uVar6;
  undefined4 uVar7;
  uint uVar8;
  size_t sVar9;
  longlong lVar10;
  int iVar11;
  void *pvVar12;
  void *pvVar13;
  undefined8 uVar14;
  undefined1 auStack_188 [32];
  uint *local_168;
  uint *local_160;
  int local_158;
  undefined ***local_150;
  int local_148;
  int local_144;
  undefined8 local_140;
  undefined ***local_138;
  undefined8 local_130;
  ulonglong local_128;
  ulonglong *puStack_120;
  undefined8 local_118;
  int local_110;
  int local_10c;
  uint local_108 [2];
  void *local_100;
  void *local_f8;
  longlong local_f0;
  undefined **local_e8;
  undefined8 local_e0;
  int local_d8;
  int local_d4;
  uint local_d0;
  undefined1 local_cc;
  undefined1 local_c8 [16];
  undefined8 local_b8;
  undefined8 local_b0;
  undefined1 local_a8 [16];
  undefined8 local_98;
  undefined8 local_90;
  undefined1 local_88 [16];
  undefined8 local_78;
  undefined8 local_70;
  undefined4 local_68;
  ulonglong local_58;
  
  local_140 = 0xfffffffffffffffe;
  local_58 = DAT_14198f720 ^ (ulonglong)auStack_188;
  local_138 = param_4;
  cVar4 = FUN_140ccf190(param_1,param_2[9]);
  if (cVar4 == '\0') {
    if ((*param_3 >> 0x15 & 1) == 0) {
      if (((*(int *)((longlong)param_1 + 0x35ec) != *param_2) ||
          (*(int *)((longlong)param_1 + 0x35ec) == -1)) &&
         (lVar10 = FUN_1406bec40(*(undefined1 *)((longlong)param_1 + 0x1a)), lVar10 != 0)) {
        *(int *)(lVar10 + 0x3c) = *(int *)(lVar10 + 0x3c) + 1;
      }
      uVar7 = FUN_140c534e0(param_1,param_2);
      FUN_140cd1320(param_1,uVar7);
      *(int *)((longlong)param_1 + 0x35ec) = *param_2;
      *(undefined4 *)(param_1 + 0x6be) = 0xffffffff;
    }
    cVar4 = FUN_140c52730(param_1);
    cVar5 = FUN_140c52770(param_1,param_2[9]);
    local_144 = (int)((float)param_2[0xd] * DAT_1412f96e4);
    if (cVar5 != '\0') {
      local_144 = 0;
    }
    local_128 = CONCAT44(local_128._4_4_,1);
    if (cVar4 == '\0') {
      uVar7 = 2;
      if ((*param_3 >> 0x15 & 1) != 0) {
        uVar7 = 3;
      }
    }
    else {
      uVar7 = 7;
    }
    uVar8 = (uint)(param_2[0xe] == 0);
    if (cVar5 != '\0') {
      uVar8 = uVar8 | 2;
    }
    local_130 = CONCAT44(uVar7,uVar8);
    puStack_120 = param_1;
    local_148 = FUN_140c516e0();
    local_100 = (void *)0x0;
    local_f8 = (void *)0x0;
    local_f0 = 0;
    FUN_140c4e8f0(&local_100,(longlong)local_148);
    pvVar13 = local_100;
    iVar11 = local_144;
    iVar1 = local_148;
    pvVar12 = local_100;
    if (local_100 != local_f8) {
      do {
        local_168 = (uint *)&local_130;
        FUN_140c52a60(pvVar12,iVar11,iVar1,param_2);
        pvVar12 = (void *)((longlong)pvVar12 + 0x58);
      } while (pvVar12 != local_f8);
    }
    uVar14 = 0xffffffffffffffff;
    uVar6 = FUN_140ccf420(param_1,param_2[9]);
    FUN_140c584b0(param_2,param_3,uVar6);
    uVar6 = 0;
    if (*(char *)((longlong)param_1 + 0x25) == '\0') {
      cVar4 = thunk_FUN_1406c2e90();
      if (cVar4 != '\0') {
        uVar7 = FUN_140ccdb60(param_1,param_2[9]);
        local_130 = *param_1;
        local_128 = CONCAT44(local_128._4_4_,uVar7);
        lVar10 = FUN_1406bff80(&local_130);
        if (lVar10 != 0) {
          uVar6 = FUN_140c56920(param_1,&local_100,param_3,param_2);
          pvVar13 = local_100;
        }
      }
      FUN_140d0fb10(param_1,param_1[0xe],(float)param_2[0xe] * DAT_1412eeb64);
    }
    if (((uint)param_1[0x11] >> 1 & 1) == 0) {
      if ((*param_3 >> 0x15 & 1) == 0) {
        iVar1 = *(int *)((longlong)param_1 + 0x35e4);
        if (iVar1 == 0) {
          iVar11 = 1;
        }
        else if (iVar1 == 1) {
          iVar11 = 2;
        }
        else if (iVar1 == 2) {
          iVar11 = 3;
        }
        else {
          iVar11 = param_2[0xb];
          if (iVar1 == 3) {
            iVar11 = 0;
          }
        }
        *(int *)((longlong)param_1 + 0x35e4) = iVar11;
        local_150 = (undefined ***)((ulonglong)local_150 & 0xffffffff00000000);
        local_158 = 0;
        local_160 = (uint *)CONCAT44(local_160._4_4_,1);
        local_168._0_4_ = iVar11;
        FUN_140cf0140(&local_130,(short)param_1[3],param_2[10],0);
        if (*(char *)((longlong)param_2 + 0x46) == '\0') {
          uVar14 = *(undefined8 *)(param_2 + 2);
        }
        local_150 = local_138;
        local_158 = param_2[0x16];
        local_168 = (uint *)CONCAT44(local_168._4_4_,param_2[9]);
        local_160 = param_3;
        FUN_140c573e0(param_1,&local_130,uVar14,*param_2);
      }
      else {
        ppuVar2 = local_138[7];
        if (ppuVar2 != (undefined **)0x0) {
          local_e0 = 0xffffffffffffffff;
          if (*(char *)((longlong)param_2 + 0x46) == '\0') {
            local_e0 = *(undefined8 *)(param_2 + 2);
          }
          local_d4 = param_2[9];
          local_d8 = *param_2;
          local_e8 = RobActParamHitReaction::vftable;
          local_cc = 0;
          local_b0 = 0xf;
          local_b8 = 0;
          local_c8[0] = 0;
          local_90 = 0xf;
          local_98 = 0;
          local_a8[0] = 0;
          local_70 = 0xf;
          local_78 = 0;
          local_88[0] = 0;
          local_68 = 0xffffffff;
          local_138 = &local_e8;
          local_d0 = *param_3;
          (**(code **)(*ppuVar2 + 0x10))(ppuVar2,param_3,&local_138);
          FUN_140c3c970(&local_e8);
          FUN_140070640(local_88);
          FUN_140070640(local_a8);
          FUN_140070640(local_c8);
        }
        local_130 = local_130 & 0xffffffffffffff00;
        local_128 = _DAT_1412ca140;
        puStack_120 = (ulonglong *)_UNK_1412ca148;
        local_118 = 0xffffffffffffffff;
        local_110 = 0xffffffff;
        local_10c = 0;
        memset(local_108,0,4);
        local_130 = CONCAT71(local_130._1_7_,6);
        local_128 = param_1[0xe];
        puStack_120 = (ulonglong *)*param_1;
        if (*(char *)((longlong)param_2 + 0x46) == '\0') {
          local_118 = *(undefined8 *)(param_2 + 2);
        }
        else {
          local_118 = 0xffffffffffffffff;
        }
        local_110 = *param_2;
        local_10c = param_2[9];
        local_108[0] = *param_3;
        FUN_140cb0cc0(&local_130);
      }
    }
    else {
      if (*(char *)((longlong)param_2 + 0x46) == '\0') {
        uVar14 = *(undefined8 *)(param_2 + 2);
      }
      local_150 = local_138;
      local_158 = CONCAT31(local_158._1_3_,uVar6);
      local_160 = (uint *)CONCAT44(local_160._4_4_,param_2[0x15]);
      local_168 = param_3;
      FUN_140c57260(param_1,uVar14,*param_2,param_2[9]);
    }
    if (pvVar13 != (void *)0x0) {
      uVar3 = (local_f0 - (longlong)pvVar13) / 0x58;
      if (0x2e8ba2e8ba2e8ba < uVar3) {
                    /* WARNING: Subroutine does not return */
        _invalid_parameter_noinfo_noreturn();
      }
      pvVar12 = pvVar13;
      if (0xfff < uVar3 * 0x58) {
        if (((ulonglong)pvVar13 & 0x1f) != 0) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        pvVar12 = *(void **)((longlong)pvVar13 - 8);
        if (pvVar13 <= pvVar12) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if ((ulonglong)((longlong)pvVar13 - (longlong)pvVar12) < 8) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if (0x27 < (ulonglong)((longlong)pvVar13 - (longlong)pvVar12)) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
      }
      free(pvVar12);
    }
  }
  else {
    ppuVar2 = param_4[7];
    if (ppuVar2 != (undefined **)0x0) {
      local_e0 = 0xffffffffffffffff;
      if (*(char *)((longlong)param_2 + 0x46) == '\0') {
        local_e0 = *(undefined8 *)(param_2 + 2);
      }
      local_d4 = param_2[9];
      local_d8 = *param_2;
      local_e8 = RobActParamHitReaction::vftable;
      local_d0 = *param_3;
      local_cc = 0;
      local_b0 = 0xf;
      local_b8 = 0;
      local_c8[0] = 0;
      local_90 = 0xf;
      local_98 = 0;
      local_a8[0] = 0;
      local_70 = 0xf;
      local_78 = 0;
      local_88[0] = 0;
      local_68 = 0xffffffff;
      local_138 = &local_e8;
      (**(code **)(*ppuVar2 + 0x10))(ppuVar2,param_3,&local_138);
      sVar9 = strlen("hit_01");
      FUN_140070d00(local_c8,"hit_01",sVar9);
      FUN_140c3c970(&local_e8,param_1);
      FUN_140070640(local_88);
      FUN_140070640(local_a8);
      FUN_140070640(local_c8);
    }
  }
  __security_check_cookie(local_58 ^ (ulonglong)auStack_188);
  return;
}

