// Requested 140c5c870; entry 140c5c870; auto-decompiled, types/names provisional.

void FUN_140c5c870(ulonglong *param_1,ulonglong *param_2)

{
  longlong *plVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 *puVar4;
  undefined4 *puVar5;
  code *pcVar6;
  undefined8 uVar7;
  int *piVar8;
  char cVar9;
  char cVar10;
  undefined8 *puVar11;
  undefined8 *puVar12;
  longlong lVar13;
  longlong *plVar14;
  undefined8 *puVar15;
  ulonglong uVar16;
  longlong *plVar17;
  int *piVar18;
  int iVar19;
  ulonglong uVar20;
  longlong *plVar21;
  ulonglong uVar22;
  undefined4 *puVar23;
  ulonglong *puVar24;
  ulonglong uVar25;
  char cVar26;
  undefined4 *puVar27;
  char cVar28;
  bool bVar29;
  float fVar30;
  float fVar31;
  float fVar32;
  float fVar33;
  float fVar34;
  float fVar35;
  float fVar36;
  float fVar37;
  float fVar38;
  float fVar39;
  float fVar40;
  undefined1 auStack_298 [32];
  float *local_278;
  undefined1 local_268;
  undefined1 local_267;
  undefined1 local_266;
  undefined1 local_265;
  uint local_264;
  uint local_260;
  int local_25c;
  int local_258;
  ulonglong local_250;
  int local_248;
  int local_244;
  int local_240;
  ulonglong *local_238;
  ulonglong local_230;
  undefined8 local_228;
  float local_220;
  undefined8 local_218;
  float local_210;
  ulonglong *local_208;
  ulonglong local_200;
  ulonglong local_1f8;
  uint local_1f0;
  undefined8 local_1e8;
  float local_1d8;
  float fStack_1d4;
  float local_1d0;
  undefined8 local_1c8;
  float local_1c0;
  undefined8 local_1b8;
  float local_1b0;
  undefined8 local_1a8;
  float local_1a0;
  undefined8 local_198;
  undefined4 local_190;
  undefined8 local_188;
  undefined4 local_180;
  undefined8 local_178;
  float local_170;
  undefined8 local_168;
  undefined4 local_160;
  undefined8 local_158;
  undefined4 local_150;
  undefined1 local_148 [8];
  float local_140;
  float local_130;
  undefined8 local_128;
  float local_120 [2];
  longlong *local_118;
  longlong *plStack_110;
  longlong local_108;
  int *local_100;
  int *piStack_f8;
  longlong local_f0;
  ulonglong local_e8;
  
  local_128 = 0xfffffffffffffffe;
  local_e8 = DAT_14198f720 ^ (ulonglong)auStack_298;
  local_238 = param_1;
  local_208 = param_2;
  if (((*(char *)((longlong)param_1 + 0x2d) != *(char *)((longlong)param_2 + 0x2d)) &&
      (param_1[0x50b] != param_1[0x50c])) && (param_2[0x50b] != param_2[0x50c])) {
    uVar16 = param_1[0x567];
    uVar22 = param_2[0x567];
    uVar20 = 0;
    uVar25 = (ulonglong)(uVar16 != 0);
    if (uVar16 != 0) {
      puVar4 = *(undefined8 **)(uVar16 + 8);
      cVar10 = *(char *)((longlong)puVar4[1] + 0x19);
      puVar12 = puVar4;
      puVar11 = (undefined8 *)puVar4[1];
      while (cVar10 == '\0') {
        if ((longlong)puVar11[4] < (longlong)*param_2) {
          puVar15 = (undefined8 *)puVar11[2];
          puVar11 = puVar12;
        }
        else {
          puVar15 = (undefined8 *)*puVar11;
        }
        puVar12 = puVar11;
        puVar11 = puVar15;
        cVar10 = *(char *)((longlong)puVar15 + 0x19);
      }
      if ((puVar12 == puVar4) || ((longlong)*param_2 < (longlong)puVar12[4])) {
        puVar12 = puVar4;
      }
      uVar25 = (ulonglong)(uVar16 != 0);
      if (puVar12 != puVar4) {
        uVar25 = uVar20;
      }
    }
    cVar10 = (char)uVar25;
    uVar25 = (ulonglong)(uVar22 != 0);
    if (uVar22 != 0) {
      puVar4 = *(undefined8 **)(uVar22 + 8);
      cVar26 = *(char *)((longlong)puVar4[1] + 0x19);
      puVar12 = puVar4;
      puVar11 = (undefined8 *)puVar4[1];
      while (cVar26 == '\0') {
        if ((longlong)puVar11[4] < (longlong)*param_1) {
          puVar15 = (undefined8 *)puVar11[2];
          puVar11 = puVar12;
        }
        else {
          puVar15 = (undefined8 *)*puVar11;
        }
        puVar12 = puVar11;
        puVar11 = puVar15;
        cVar26 = *(char *)((longlong)puVar15 + 0x19);
      }
      if ((puVar12 == puVar4) || ((longlong)*param_1 < (longlong)puVar12[4])) {
        puVar12 = puVar4;
      }
      uVar25 = (ulonglong)(uVar22 != 0);
      if (puVar12 != puVar4) {
        uVar25 = uVar20;
      }
    }
    cVar26 = (char)uVar25;
    if (((*(byte *)((longlong)param_1 + 0x34b4) & 0x10) == 0) && (cVar10 != '\0')) {
      cVar9 = FUN_140c59350(param_1,param_2,0,CONCAT71((int7)(uVar16 >> 8),1));
      if (cVar9 != '\0') {
        cVar10 = '\0';
      }
    }
    if ((((*(byte *)((longlong)param_2 + 0x34b4) & 0x10) == 0) && (cVar26 != '\0')) &&
       (cVar9 = FUN_140c59350(param_2,param_1,0,1), cVar9 != '\0')) {
      cVar26 = '\0';
    }
    if ((cVar10 != '\0') || (cVar26 != '\0')) {
      local_1f8 = (longlong)(param_1[0x50c] - param_1[0x50b]) / 0x74;
      local_250 = (longlong)(param_2[0x50c] - param_2[0x50b]) / 0x74;
      uVar22 = (longlong)(param_1[0x512] - param_1[0x511]) / 0x74 + local_1f8;
      uVar16 = (longlong)(param_2[0x512] - param_2[0x511]) / 0x74 + local_250;
      local_100 = (int *)0x0;
      piStack_f8 = (int *)0x0;
      local_f0 = 0;
      local_230 = uVar16;
      local_200 = uVar22;
      FUN_140c577f0(&local_100,uVar22);
      if (uVar22 != 0) {
        local_1e8 = ((longlong)piStack_f8 - (longlong)local_100) / 0x28;
        plVar17 = (longlong *)(local_100 + 2);
        uVar22 = uVar20;
        do {
          if (local_1e8 <= uVar22) {
            std::_Xout_of_range("invalid vector<T> subscript");
            pcVar6 = (code *)swi(3);
            (*pcVar6)();
            return;
          }
          iVar19 = (int)uVar20;
          uVar16 = (ulonglong)iVar19;
          bVar29 = local_1f8 <= uVar16;
          *(bool *)(plVar17 + 3) = bVar29;
          if (bVar29) {
            uVar16 = uVar16 - local_1f8;
            uVar25 = local_238[0x511];
            if ((ulonglong)((longlong)(local_238[0x512] - uVar25) / 0x74) <= uVar16) {
              std::_Xout_of_range("invalid vector<T> subscript");
              pcVar6 = (code *)swi(3);
              (*pcVar6)();
              return;
            }
          }
          else {
            uVar25 = local_238[0x50b];
            if ((ulonglong)((longlong)(local_238[0x50c] - uVar25) / 0x74) <= uVar16) {
              std::_Xout_of_range("invalid vector<T> subscript");
              pcVar6 = (code *)swi(3);
              (*pcVar6)();
              return;
            }
          }
          *(int *)(plVar17 + -1) = iVar19;
          uVar20 = local_230;
          if (bVar29) {
            uVar20 = local_250;
          }
          if ((ulonglong)((plVar17[2] - *plVar17) / 0x38) < uVar20) {
            if (0x492492492492492 < uVar20) {
              std::_Xlength_error("vector<T> too long");
              pcVar6 = (code *)swi(3);
              (*pcVar6)();
              return;
            }
            FUN_140c4f600(plVar17,uVar20);
          }
          cVar9 = '\0';
          if (*(char *)(uVar16 * 0x74 + uVar25 + 0x61) != '\0') {
            cVar9 = cVar10;
          }
          *(char *)((longlong)plVar17 + 0x19) = cVar9;
          uVar20 = (ulonglong)(iVar19 + 1U);
          uVar22 = uVar22 + 1;
          plVar17 = plVar17 + 5;
          uVar16 = local_230;
        } while ((ulonglong)(longlong)(int)(iVar19 + 1U) < local_200);
      }
      uVar22 = 0;
      local_118 = (longlong *)0x0;
      plStack_110 = (longlong *)0x0;
      local_108 = 0;
      FUN_140c577f0(&local_118,uVar16);
      if (uVar16 != 0) {
        lVar13 = (longlong)plStack_110 - (longlong)local_118;
        plVar17 = local_118 + 1;
        uVar16 = uVar22;
        do {
          if ((ulonglong)(lVar13 / 0x28) <= uVar16) {
            std::_Xout_of_range("invalid vector<T> subscript");
            pcVar6 = (code *)swi(3);
            (*pcVar6)();
            return;
          }
          iVar19 = (int)uVar22;
          uVar22 = (ulonglong)iVar19;
          bVar29 = local_250 <= uVar22;
          *(bool *)(plVar17 + 3) = bVar29;
          if (bVar29) {
            uVar22 = uVar22 - local_250;
            uVar25 = param_2[0x511];
            if ((ulonglong)((longlong)(param_2[0x512] - uVar25) / 0x74) <= uVar22) {
              std::_Xout_of_range("invalid vector<T> subscript");
              pcVar6 = (code *)swi(3);
              (*pcVar6)();
              return;
            }
          }
          else {
            uVar25 = param_2[0x50b];
            if ((ulonglong)((longlong)(param_2[0x50c] - uVar25) / 0x74) <= uVar22) {
              std::_Xout_of_range("invalid vector<T> subscript");
              pcVar6 = (code *)swi(3);
              (*pcVar6)();
              return;
            }
          }
          *(int *)(plVar17 + -1) = iVar19;
          uVar20 = local_200;
          if (bVar29) {
            uVar20 = local_1f8;
          }
          if ((ulonglong)((plVar17[2] - *plVar17) / 0x38) < uVar20) {
            if (0x492492492492492 < uVar20) {
              std::_Xlength_error("vector<T> too long");
              pcVar6 = (code *)swi(3);
              (*pcVar6)();
              return;
            }
            FUN_140c4f600(plVar17,uVar20);
          }
          cVar10 = '\0';
          if (*(char *)(uVar22 * 0x74 + uVar25 + 0x61) != '\0') {
            cVar10 = cVar26;
          }
          *(char *)((longlong)plVar17 + 0x19) = cVar10;
          uVar22 = (ulonglong)(iVar19 + 1U);
          uVar16 = uVar16 + 1;
          plVar17 = plVar17 + 5;
        } while ((ulonglong)(longlong)(int)(iVar19 + 1U) < local_230);
      }
      bVar29 = false;
      local_267 = 0;
      local_265 = 0;
      local_266 = 0;
      local_268 = 0;
      local_244 = -1;
      local_258 = -1;
      local_25c = -1;
      local_1e8 = CONCAT44(local_1e8._4_4_,(uint)local_238[0x47f] >> 10) & 0xffffffffffffff01;
      local_1f0 = (uint)param_2[0x47f] >> 10 & 0xffffff01;
      if (local_238[0x598] == *param_2) {
        local_248 = (int)local_238[0x599];
      }
      else {
        local_248 = -1;
      }
      if (param_2[0x598] == *local_238) {
        local_244 = (int)param_2[0x599];
      }
      if (local_100 != piStack_f8) {
        plVar17 = (longlong *)(local_100 + 4);
        piVar18 = local_100;
        puVar24 = local_238;
        fVar39 = DAT_1412ee464;
        fVar40 = DAT_1412ee464;
        do {
          uVar16 = (ulonglong)*piVar18;
          if ((char)plVar17[2] == '\0') {
            uVar22 = puVar24[0x50b];
            if ((ulonglong)((longlong)(puVar24[0x50c] - uVar22) / 0x74) <= uVar16) {
              std::_Xout_of_range("invalid vector<T> subscript");
              pcVar6 = (code *)swi(3);
              (*pcVar6)();
              return;
            }
          }
          else {
            uVar16 = uVar16 - local_1f8;
            uVar22 = puVar24[0x511];
            if ((ulonglong)((longlong)(puVar24[0x512] - uVar22) / 0x74) <= uVar16) {
              std::_Xout_of_range("invalid vector<T> subscript");
              pcVar6 = (code *)swi(3);
              (*pcVar6)();
              return;
            }
          }
          puVar27 = (undefined4 *)(uVar16 * 0x74 + uVar22);
          if ((((char)plVar17[2] != '\0') ||
              (cVar10 = FUN_140ccf2a0(puVar24,puVar27[3]), cVar10 == '\0')) &&
             (local_240 = puVar27[3], local_118 != plStack_110)) {
            plVar14 = plStack_110;
            plVar21 = local_118 + 2;
            do {
              if (((char)plVar17[2] == '\0') || ((char)plVar21[2] == '\0')) {
                uVar16 = (ulonglong)(int)plVar21[-2];
                if ((char)plVar21[2] == '\0') {
                  uVar22 = param_2[0x50b];
                  if ((ulonglong)((longlong)(param_2[0x50c] - uVar22) / 0x74) <= uVar16) {
                    std::_Xout_of_range("invalid vector<T> subscript");
                    pcVar6 = (code *)swi(3);
                    (*pcVar6)();
                    return;
                  }
                }
                else {
                  uVar16 = uVar16 - local_250;
                  uVar22 = param_2[0x511];
                  if ((ulonglong)((longlong)(param_2[0x512] - uVar22) / 0x74) <= uVar16) {
                    std::_Xout_of_range("invalid vector<T> subscript");
                    pcVar6 = (code *)swi(3);
                    (*pcVar6)();
                    return;
                  }
                }
                puVar23 = (undefined4 *)(uVar16 * 0x74 + uVar22);
                if (((char)plVar21[2] != '\0') ||
                   (cVar10 = FUN_140ccf2a0(param_2,puVar23[3]), plVar14 = plStack_110,
                   cVar10 == '\0')) {
                  local_200 = CONCAT44(local_200._4_4_,puVar23[3]);
                  local_228 = 0;
                  local_220 = 0.0;
                  local_218 = 0;
                  local_210 = 0.0;
                  local_278 = local_120;
                  cVar10 = thunk_FUN_140c54d90(puVar27,puVar23,&local_228,&local_218);
                  plVar14 = plStack_110;
                  if (cVar10 != '\0') {
                    uVar2 = *(undefined8 *)(puVar27 + 5);
                    local_1c0 = (float)puVar27[7];
                    fVar33 = local_1c0 + local_220;
                    local_1c8._4_4_ = (float)((ulonglong)uVar2 >> 0x20);
                    fVar35 = local_1c8._4_4_ + local_228._4_4_;
                    local_1c8._0_4_ = (float)uVar2;
                    fVar34 = (float)local_1c8 + (float)local_228;
                    uVar3 = *(undefined8 *)(puVar23 + 5);
                    local_1b0 = (float)puVar23[7];
                    fVar36 = local_1b0 + local_210;
                    local_1b8._4_4_ = (float)((ulonglong)uVar3 >> 0x20);
                    fVar37 = local_1b8._4_4_ + local_218._4_4_;
                    local_1b8._0_4_ = (float)uVar3;
                    fVar38 = (float)local_1b8 + (float)local_218;
                    local_1a0 = fVar36 - fVar33;
                    fVar32 = fVar37 - fVar35;
                    fVar31 = fVar38 - fVar34;
                    fVar30 = (float)puVar27[0x17] / ((float)puVar27[0x17] + (float)puVar23[0x17]);
                    local_1d0 = fVar33 + local_1a0 * fVar30;
                    fStack_1d4 = fVar35 + fVar32 * fVar30;
                    local_1d8 = fVar31 * fVar30 + fVar34;
                    local_264 = 0;
                    local_260 = 0;
                    local_1a8 = CONCAT44(fVar32,fVar31);
                    local_198 = *(undefined8 *)(puVar23 + 0x11);
                    local_190 = puVar23[0x13];
                    local_188 = *(undefined8 *)(puVar27 + 0x11);
                    local_180 = puVar27[0x13];
                    local_1c8 = uVar2;
                    local_1b8 = uVar3;
                    local_130 = local_1a0;
                    fVar31 = (float)FUN_140c50e70(&local_188,&local_198,&local_1a8);
                    fVar30 = DAT_1415ac570;
                    local_230 = CONCAT44(local_230._4_4_,fVar31);
                    if (DAT_1415ac570 < fVar31) {
                      *(undefined1 *)((longlong)plVar17 + 0x13) = 1;
                      local_264 = 4;
                    }
                    local_170 = fVar33 - fVar36;
                    local_178 = CONCAT44(fVar35 - fVar37,fVar34 - fVar38);
                    local_168 = *(undefined8 *)(puVar27 + 0x11);
                    local_160 = puVar27[0x13];
                    local_158 = *(undefined8 *)(puVar23 + 0x11);
                    local_150 = puVar23[0x13];
                    local_140 = local_170;
                    fVar31 = (float)FUN_140c50e70(&local_158,&local_168,&local_178);
                    if (fVar30 < fVar31) {
                      *(undefined1 *)((longlong)plVar21 + 0x13) = 1;
                      local_260 = 4;
                    }
                    cVar10 = *(char *)((longlong)plVar17 + 0x11);
                    cVar26 = cVar10;
                    if ((char)local_1f0 == '\0') {
                      if ((cVar10 == '\0') ||
                         (((uint)puVar27[0x19] >> (puVar23[1] & 0x1f) & 1) == 0)) {
                        cVar26 = '\0';
                      }
                      else {
                        cVar26 = '\x01';
                      }
                    }
                    cVar9 = *(char *)((longlong)plVar21 + 0x11);
                    cVar28 = cVar9;
                    if ((char)local_1e8 == '\0') {
                      if ((cVar9 == '\0') || (((uint)puVar23[0x19] >> (puVar27[1] & 0x1f) & 1) == 0)
                         ) {
                        cVar28 = '\0';
                      }
                      else {
                        cVar28 = '\x01';
                      }
                    }
                    if ((cVar10 != '\0') && (cVar9 != '\0')) {
                      if ((local_264 & 4) != 0) {
                        cVar26 = '\x01';
                      }
                      if ((local_260 & 4) != 0) {
                        cVar28 = '\x01';
                      }
                    }
                    if (cVar26 != '\0') {
                      local_264 = local_264 + 1;
                      local_260 = local_260 + 2;
                      *(undefined1 *)((longlong)plVar17 + 0x12) = 1;
                      local_267 = 1;
                    }
                    if (cVar10 != '\0') {
                      local_264 = local_264 + 8;
                      local_260 = local_260 + 0x10;
                      *(undefined1 *)((longlong)plVar17 + 0x14) = 1;
                      local_266 = 1;
                    }
                    if ((cVar26 != '\0') || (cVar10 != '\0')) {
                      if ((int)local_200 == local_248) {
                        local_258 = puVar23[3];
                        fVar40 = local_120[0];
                      }
                      else if (local_258 < 0) {
                        local_258 = puVar23[3];
                        fVar40 = local_120[0];
                      }
                      else if (local_258 != puVar23[3]) {
                        cVar10 = FUN_140ccf190(local_208,local_258);
                        if (cVar10 == '\0') {
                          cVar10 = FUN_140ccf190(local_208,puVar23[3]);
                          if (cVar10 == '\0') goto LAB_140c5d3d2;
                        }
                        else {
                          cVar10 = FUN_140ccf190(local_208,puVar23[3]);
                          if (cVar10 == '\0') {
                            local_258 = puVar23[3];
                            fVar40 = local_120[0];
                          }
                          else {
LAB_140c5d3d2:
                            if (local_120[0] < fVar40) {
                              local_258 = puVar23[3];
                              fVar40 = local_120[0];
                            }
                          }
                        }
                      }
                    }
                    puVar24 = local_238;
                    if (cVar28 != '\0') {
                      local_264 = local_264 + 2;
                      local_260 = local_260 + 1;
                      *(undefined1 *)((longlong)plVar21 + 0x12) = 1;
                      local_265 = 1;
                    }
                    if (cVar9 != '\0') {
                      local_264 = local_264 + 0x10;
                      local_260 = local_260 + 8;
                      *(undefined1 *)((longlong)plVar21 + 0x14) = 1;
                      local_268 = 1;
                    }
                    if ((cVar28 != '\0') || (cVar9 != '\0')) {
                      if (local_240 == local_244) {
                        local_25c = puVar27[3];
                      }
                      if ((cVar28 != '\0') || (cVar9 != '\0')) {
                        if (local_240 == local_244) {
                          local_25c = puVar27[3];
                          fVar39 = local_120[0];
                        }
                        else if (local_25c < 0) {
                          local_25c = puVar27[3];
                          fVar39 = local_120[0];
                        }
                        else if (local_25c != puVar27[3]) {
                          cVar10 = FUN_140ccf190(local_238,local_25c);
                          if (cVar10 == '\0') {
                            cVar10 = FUN_140ccf190(puVar24,puVar27[3]);
                            if ((cVar10 == '\0') && (local_120[0] < fVar39)) {
                              local_25c = puVar27[3];
                              fVar39 = local_120[0];
                            }
                          }
                          else {
                            cVar10 = FUN_140ccf190(puVar24,puVar27[3]);
                            if (cVar10 == '\0') {
                              local_25c = puVar27[3];
                              fVar39 = local_120[0];
                            }
                            else if (local_120[0] < fVar39) {
                              local_25c = puVar27[3];
                              fVar39 = local_120[0];
                            }
                          }
                        }
                      }
                    }
                    if (*plVar17 == plVar17[1]) {
                      FUN_140c4faa0(plVar17 + -1,1);
                    }
                    fVar33 = local_1d0;
                    fVar32 = local_210;
                    uVar7 = local_218;
                    fVar30 = local_220;
                    uVar3 = local_228;
                    puVar5 = (undefined4 *)*plVar17;
                    uVar2 = CONCAT44(fStack_1d4,local_1d8);
                    if (puVar5 != (undefined4 *)0x0) {
                      lVar13 = plVar21[2];
                      *puVar5 = *puVar23;
                      *(char *)(puVar5 + 1) = (char)lVar13;
                      *(undefined8 *)(puVar5 + 2) = local_228;
                      puVar5[4] = local_220;
                      *(undefined8 *)(puVar5 + 5) = local_218;
                      puVar5[7] = local_210;
                      *(undefined8 *)(puVar5 + 8) = uVar2;
                      puVar5[10] = local_1d0;
                      puVar5[0xb] = local_120[0];
                      puVar5[0xc] = local_264;
                      puVar5[0xd] = (undefined4)local_230;
                    }
                    *plVar17 = *plVar17 + 0x38;
                    if (*plVar21 == plVar21[1]) {
                      FUN_140c4faa0(plVar21 + -1,1);
                    }
                    puVar23 = (undefined4 *)*plVar21;
                    if (puVar23 != (undefined4 *)0x0) {
                      lVar13 = plVar17[2];
                      *puVar23 = *puVar27;
                      *(char *)(puVar23 + 1) = (char)lVar13;
                      *(undefined8 *)(puVar23 + 2) = uVar7;
                      puVar23[4] = fVar32;
                      *(undefined8 *)(puVar23 + 5) = uVar3;
                      puVar23[7] = fVar30;
                      *(undefined8 *)(puVar23 + 8) = uVar2;
                      puVar23[10] = fVar33;
                      puVar23[0xb] = local_120[0];
                      puVar23[0xc] = local_260;
                      puVar23[0xd] = fVar31;
                    }
                    *plVar21 = *plVar21 + 0x38;
                    bVar29 = true;
                    plVar14 = plStack_110;
                    param_2 = local_208;
                  }
                }
              }
              plVar1 = plVar21 + 3;
              plVar21 = plVar21 + 5;
              puVar24 = local_238;
            } while (plVar1 != plVar14);
          }
          piVar18 = piVar18 + 10;
          plVar17 = plVar17 + 5;
        } while (piVar18 != piStack_f8);
        if (bVar29) {
          local_250 = *param_2;
          local_278 = (float *)FUN_140c4d2c0(puVar24 + 0x568,&local_250,&local_100);
          FUN_140c4de70(puVar24 + 0x568,local_148,0,(longlong)local_278 + 0x20);
          local_250 = *puVar24;
          local_278 = (float *)FUN_140c4d2c0(param_2 + 0x568,&local_250,&local_118);
          FUN_140c4de70(param_2 + 0x568,local_148,0,(longlong)local_278 + 0x20);
          local_278._0_4_ = local_258;
          FUN_140c50890(puVar24,param_2,local_266,local_267);
          local_278 = (float *)CONCAT44(local_278._4_4_,local_25c);
          FUN_140c50890(param_2,puVar24,local_268,local_265);
        }
      }
      piVar8 = piStack_f8;
      piVar18 = local_100;
      plVar17 = local_118;
      if (local_118 != (longlong *)0x0) {
        FUN_140c4d4a0(local_118,plStack_110,&local_118,local_268);
        FUN_140c53100(&local_118,plVar17,(local_108 - (longlong)plVar17) / 0x28);
      }
      if (piVar18 != (int *)0x0) {
        FUN_140c4d4a0(piVar18,piVar8,&local_100,local_268);
        FUN_140c53100(&local_100,piVar18,(local_f0 - (longlong)piVar18) / 0x28);
      }
    }
  }
  __security_check_cookie(local_e8 ^ (ulonglong)auStack_298);
  return;
}

