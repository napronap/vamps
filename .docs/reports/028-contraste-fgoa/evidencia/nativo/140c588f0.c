// Requested 140c588f0; entry 140c588f0; auto-decompiled, types/names provisional.

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140c588f0(undefined8 *param_1,int *param_2,uint *param_3,undefined ***param_4)

{
  int iVar1;
  undefined **ppuVar2;
  uint uVar3;
  longlong lVar4;
  char cVar5;
  char cVar6;
  undefined1 uVar7;
  int iVar8;
  undefined4 uVar9;
  size_t sVar10;
  undefined8 uVar11;
  undefined ***pppuVar12;
  uint uVar13;
  longlong lVar14;
  undefined **ppuVar15;
  undefined1 auStack_1b8 [32];
  uint *local_198;
  uint *local_190;
  int local_188;
  undefined ***local_180;
  char local_178;
  uint local_174;
  undefined8 local_170;
  undefined ***local_168;
  uint local_160 [2];
  undefined ***local_158;
  undefined1 *local_150;
  uint local_148;
  uint local_144;
  undefined8 local_140;
  undefined8 *puStack_138;
  undefined8 local_130;
  int local_128;
  int local_124;
  uint local_120 [2];
  longlong local_118;
  longlong local_110;
  longlong local_108;
  undefined **local_f8;
  undefined8 local_f0;
  int local_e8;
  int local_e4;
  uint local_e0;
  undefined1 local_dc;
  undefined1 local_d8 [16];
  undefined8 local_c8;
  undefined8 local_c0;
  undefined1 local_b8 [16];
  undefined8 local_a8;
  undefined8 local_a0;
  undefined1 local_98 [16];
  undefined8 local_88;
  undefined8 local_80;
  undefined4 local_78;
  ulonglong local_68;
  
  local_170 = 0xfffffffffffffffe;
  local_68 = DAT_14198f720 ^ (ulonglong)auStack_1b8;
  local_168 = param_4;
  cVar5 = FUN_140ccf190();
  if (cVar5 != '\0') {
    ppuVar15 = param_4[7];
    if (ppuVar15 != (undefined **)0x0) {
      if (*(char *)((longlong)param_2 + 0x46) == '\0') {
        local_f0 = *(undefined8 *)(param_2 + 2);
      }
      else {
        local_f0 = 0xffffffffffffffff;
      }
      local_e4 = param_2[9];
      local_e8 = *param_2;
      local_f8 = RobActParamHitReaction::vftable;
      local_e0 = *param_3;
      local_dc = 0;
      local_c0 = 0xf;
      local_c8 = 0;
      local_d8[0] = 0;
      local_a0 = 0xf;
      local_a8 = 0;
      local_b8[0] = 0;
      local_80 = 0xf;
      local_88 = 0;
      local_98[0] = 0;
      local_78 = 0xffffffff;
      local_168 = &local_f8;
      (**(code **)(*ppuVar15 + 0x10))(ppuVar15,param_3,&local_168);
      sVar10 = strlen("hit_01");
      FUN_140070d00(local_d8,"hit_01",sVar10);
      FUN_140c3c970(&local_f8,param_1);
      FUN_140070640(local_98);
      FUN_140070640(local_b8);
      FUN_140070640(local_d8);
    }
    goto LAB_140c59124;
  }
  cVar5 = FUN_140c52730(param_1);
  cVar6 = FUN_140c52770(param_1);
  uVar13 = 0;
  local_174 = param_2[0xd];
  if (cVar6 != '\0') {
    local_174 = uVar13;
  }
  local_178 = cVar6;
  memset(local_160,0,4);
  local_158 = (undefined ***)FUN_140c53a70(param_2);
  uVar3 = local_160[0];
  if (((local_158 != (undefined ***)0x0) && (((ulonglong)*local_158 & 0x20000000000) != 0)) &&
     (uVar3 = local_160[0] | 0x20, ((uint)param_2[0x12] >> 7 & 1) != 0)) {
    uVar3 = local_160[0] | 0x60;
  }
  local_160[0] = uVar3;
  iVar8 = FUN_140c516e0();
  local_148 = 0;
  local_144 = -(uint)(cVar5 != '\0') & 7;
  local_140 = CONCAT44(local_140._4_4_,param_2[0xf]);
  if (param_2[0xe] == 0) {
LAB_140c58add:
    local_148 = 1;
    uVar3 = 1;
LAB_140c58ae6:
    uVar13 = uVar3;
    if (cVar6 != '\0') {
      uVar13 = uVar13 | 2;
      local_148 = uVar13;
    }
  }
  else {
    uVar3 = 0;
    if ((*(uint *)(param_1 + 0x11) >> 1 & 1) == 0) goto LAB_140c58ae6;
    if (cVar6 != '\0') goto LAB_140c58add;
  }
  if ((local_160[0] >> 6 & 1) != 0) {
    local_148 = uVar13 | 4;
  }
  local_118 = 0;
  local_110 = 0;
  local_108 = 0;
  puStack_138 = param_1;
  FUN_140c4e8f0(&local_118,(longlong)iVar8);
  lVar4 = local_110;
  uVar13 = local_174;
  for (lVar14 = local_118; lVar14 != lVar4; lVar14 = lVar14 + 0x58) {
    local_198 = &local_148;
    FUN_140c52a60(lVar14,uVar13,iVar8,param_2);
  }
  pppuVar12 = (undefined ***)0x0;
  *param_3 = *param_3 | 1;
  lVar14 = FUN_140ccd770(param_1);
  if ((lVar14 != 0) || (param_1[0x4a5] != 0)) {
    *param_3 = *param_3 | 4;
  }
  if (((*(uint *)(param_1 + 0x47e) >> 6 & 1) != 0) &&
     ((cVar5 = FUN_140d10460(param_1,param_1[0xe]), cVar5 != '\0' ||
      (cVar5 = FUN_140c6bc70(param_1 + 0x4dc), cVar5 != '\0')))) {
    *param_3 = *param_3 | 0x2000;
  }
  uVar11 = FUN_140c4c500(param_1 + 0x463);
  iVar8 = FUN_140c4c460(uVar11);
  if ((iVar8 == 0xf) && (cVar5 = FUN_140d10460(param_1,param_1[0xe]), cVar5 != '\0')) {
    *param_3 = *param_3 | 0x4000;
  }
  cVar5 = FUN_140c524e0(param_1,param_2 + 0x12,param_3);
  if (cVar5 != '\0') {
    *param_3 = *param_3 | 8;
  }
  cVar5 = FUN_140c55d90(param_1);
  if (cVar5 != '\0') {
    *param_3 = *param_3 | 0x80000;
  }
  cVar5 = FUN_140c55df0(param_1,param_2);
  if (cVar5 != '\0') {
    *param_3 = *param_3 | 0x180000;
  }
  *param_3 = *param_3 | local_160[0];
  if ((local_158 != (undefined ***)0x0) && (((ulonglong)*local_158 & 0x10000000000) != 0)) {
    *param_3 = *param_3 | 0x10;
  }
  FUN_140ccf420(param_1,param_2[9]);
  FUN_140c584b0(param_2,param_3);
  cVar5 = local_178;
  if (local_178 != '\0') {
    *param_3 = *param_3 | 0x800;
  }
  uVar7 = 0;
  if ((*(char *)((longlong)param_1 + 0x25) == '\0') &&
     (cVar6 = thunk_FUN_1406c2e90(), cVar6 != '\0')) {
    uVar9 = FUN_140ccdb60(param_1,param_2[9]);
    local_158 = (undefined ***)*param_1;
    local_150 = (undefined1 *)CONCAT44(local_150._4_4_,uVar9);
    lVar14 = FUN_1406bff80(&local_158);
    if (lVar14 != 0) {
      if ((((param_1[0x5d7] != *(longlong *)(param_2 + 2)) ||
           (*(int *)(param_1 + 0x5d9) != *param_2)) || (*(int *)(param_1 + 0x5d9) == -1)) &&
         ((*(byte *)(lVar14 + 0x2a4) & 4) != 0)) {
        *(int *)(lVar14 + 0x290) = *(int *)(lVar14 + 0x290) + 1;
      }
      if (0 < *(int *)(lVar14 + 0x290)) {
        *param_3 = *param_3 | 0x100;
      }
      if (3 < *(int *)(lVar14 + 0x290)) {
        *param_3 = *param_3 | 0x200;
      }
      uVar7 = FUN_140c56920(param_1,&local_118,param_3,param_2);
    }
  }
  if ((cVar5 == '\0') && ((*param_3 >> 9 & 1) == 0)) {
    if ((*param_3 >> 0x13 & 1) == 0) {
      *(int *)(param_1 + 0x6be) = *param_2;
    }
    uVar13 = *param_3;
    uVar3 = uVar13 >> 6;
    if ((*(uint *)(param_1 + 0x11) >> 1 & 1) == 0) {
      cVar5 = FUN_140ccf340(param_1);
      if (cVar5 == '\0') {
        uVar13 = *param_3;
        if ((uVar13 >> 0x13 & 1) == 0) goto LAB_140c59022;
        ppuVar15 = local_168[7];
        if (ppuVar15 != (undefined **)0x0) {
          if (*(char *)((longlong)param_2 + 0x46) == '\0') {
            local_f0 = *(undefined8 *)(param_2 + 2);
          }
          else {
            local_f0 = 0xffffffffffffffff;
          }
          local_e4 = param_2[9];
          local_e8 = *param_2;
          local_f8 = RobActParamHitReaction::vftable;
          local_dc = 0;
          local_c0 = 0xf;
          local_c8 = 0;
          local_d8[0] = 0;
          local_a0 = 0xf;
          local_a8 = 0;
          local_b8[0] = 0;
          local_80 = 0xf;
          local_88 = 0;
          local_98[0] = 0;
          local_78 = 0xffffffff;
          local_158 = &local_f8;
          local_e0 = uVar13;
          (**(code **)(*ppuVar15 + 0x10))(ppuVar15,param_3,&local_158);
          FUN_140c3c970(&local_f8,param_1);
          FUN_140c3ccf0(&local_f8);
          FUN_140070640(local_98);
          FUN_140070640(local_b8);
          FUN_140070640(local_d8);
        }
        if ((*param_3 >> 0x14 & 1) == 0) {
          FUN_140c59150(param_1);
        }
        local_148 = local_148 & 0xffffff00;
        local_140 = _DAT_1412ca140;
        puStack_138 = (undefined8 *)_UNK_1412ca148;
        local_130 = 0xffffffffffffffff;
        local_128 = 0xffffffff;
        local_124 = 0;
        memset(local_120,0,4);
        local_148 = CONCAT31(local_148._1_3_,5);
        local_140 = param_1[0xe];
        puStack_138 = (undefined8 *)*param_1;
        if (*(char *)((longlong)param_2 + 0x46) == '\0') {
          local_130 = *(undefined8 *)(param_2 + 2);
        }
        else {
          local_130 = 0xffffffffffffffff;
        }
        local_128 = *param_2;
        local_124 = param_2[9];
        local_120[0] = *param_3;
        FUN_140cb0cc0(&local_148);
      }
      else if (*(char *)((longlong)param_1 + 0x25) == '\0') {
        local_158 = (undefined ***)operator_new(0xa0);
        if (local_158 != (undefined ***)0x0) {
          if (*(char *)((longlong)param_2 + 0x46) == '\0') {
            ppuVar15 = *(undefined ***)(param_2 + 2);
          }
          else {
            ppuVar15 = (undefined **)0xffffffffffffffff;
          }
          ppuVar2 = (undefined **)param_1[0xe];
          uVar13 = *param_3;
          iVar8 = param_2[9];
          iVar1 = *param_2;
          *local_158 = RobActParam::vftable;
          *(undefined4 *)(local_158 + 1) = 0x18;
          local_158[2] = RobActParamHitReaction::vftable;
          local_158[3] = ppuVar15;
          *(int *)(local_158 + 4) = iVar1;
          *(int *)((longlong)local_158 + 0x24) = iVar8;
          *(uint *)(local_158 + 5) = uVar13;
          *(undefined1 *)((longlong)local_158 + 0x2c) = 0;
          pppuVar12 = local_158 + 6;
          local_158[9] = (undefined **)0xf;
          local_158[8] = (undefined **)0x0;
          if ((undefined **)0xf < local_158[9]) {
            pppuVar12 = (undefined ***)*pppuVar12;
          }
          *(undefined1 *)pppuVar12 = 0;
          pppuVar12 = local_158 + 10;
          local_158[0xd] = (undefined **)0xf;
          local_158[0xc] = (undefined **)0x0;
          if ((undefined **)0xf < local_158[0xd]) {
            pppuVar12 = (undefined ***)*pppuVar12;
          }
          *(undefined1 *)pppuVar12 = 0;
          pppuVar12 = local_158 + 0xe;
          local_158[0x11] = (undefined **)0xf;
          local_158[0x10] = (undefined **)0x0;
          if ((undefined **)0xf < local_158[0x11]) {
            pppuVar12 = (undefined ***)*pppuVar12;
          }
          *(undefined1 *)pppuVar12 = 0;
          *(undefined4 *)(local_158 + 0x12) = 0xffffffff;
          *local_158 = RobActParamPetrifyReaction::vftable;
          local_158[2] = RobActParamPetrifyReaction::vftable;
          local_158[0x13] = ppuVar2;
          pppuVar12 = local_158;
        }
        ppuVar15 = local_168[7];
        if (ppuVar15 != (undefined **)0x0) {
          if (pppuVar12 == (undefined ***)0x0) {
            local_168 = (undefined ***)0x0;
          }
          else {
            local_168 = pppuVar12 + 2;
          }
          (**(code **)(*ppuVar15 + 0x10))(ppuVar15,param_3,&local_168);
        }
        local_158 = (undefined ***)&DAT_1412c21a0;
        local_150 = &DAT_1412c21a0;
        FUN_140c4c560(param_1 + 0x463,pppuVar12,&local_158);
      }
    }
    else {
LAB_140c59022:
      if ((uVar13 >> 2 & 1) == 0) {
        iVar8 = param_2[0x13];
      }
      else {
        iVar8 = param_2[0x14];
      }
      if ((*(uint *)(param_1 + 0x11) >> 1 & 1) == 0) {
        local_180 = (undefined ***)((ulonglong)local_180 & 0xffffffff00000000);
        local_188 = 0;
        local_190 = (uint *)((ulonglong)local_190 & 0xffffffff00000000);
        local_198._0_4_ = param_2[0xb];
        FUN_140cf0140(&local_158,*(undefined2 *)(param_1 + 3),param_2[10],
                      ((uVar3 & 1) != 0) + '\x01');
        uVar11 = FUN_140c57b00(param_2);
        local_180 = local_168;
        local_198 = (uint *)CONCAT44(local_198._4_4_,param_2[9]);
        local_190 = param_3;
        local_188 = iVar8;
        FUN_140c57590(param_1,&local_158,uVar11,*param_2);
      }
      else {
        uVar11 = FUN_140c57b00(param_2);
        local_180 = local_168;
        local_188 = CONCAT31(local_188._1_3_,uVar7);
        local_190 = (uint *)CONCAT44(local_190._4_4_,iVar8);
        local_198 = param_3;
        FUN_140c57260(param_1,uVar11,*param_2,param_2[9]);
      }
      FUN_140c59150(param_1,param_2);
    }
  }
  if (local_118 != 0) {
    FUN_140c53350(&local_118,local_118,(local_108 - local_118) / 0x58);
  }
LAB_140c59124:
  __security_check_cookie(local_68 ^ (ulonglong)auStack_1b8);
  return;
}

