// Requested 14030d7d0; entry 14030d7d0; auto-decompiled, types/names provisional.

undefined8 * FUN_14030d7d0(undefined8 param_1,undefined8 param_2)

{
  void *pvVar1;
  undefined8 *puVar2;
  size_t sVar3;
  void *pvVar4;
  undefined8 uVar5;
  byte local_50;
  undefined7 uStack_4f;
  undefined8 local_40;
  ulonglong local_38;
  
  uVar5 = 0xfffffffffffffffe;
  puVar2 = (undefined8 *)operator_new(0x20);
  if (puVar2 == (undefined8 *)0x0) {
    puVar2 = (undefined8 *)0x0;
  }
  else {
    *puVar2 = MhdAttackSub::vftable;
    puVar2[1] = 0xffffffffffffffff;
    puVar2[2] = 0xffffffffffffffff;
    puVar2[3] = 0xffffffffffffffff;
    local_38 = 0xf;
    local_40 = 0;
    local_50 = 0;
    sVar3 = strlen("guard_count");
    FUN_140070d00(&local_50,"guard_count",sVar3);
    FUN_14032e360(param_1,param_2,puVar2 + 1,&local_50,uVar5);
    if (0xf < local_38) {
      pvVar1 = (void *)CONCAT71(uStack_4f,local_50);
      pvVar4 = pvVar1;
      if (0xfff < local_38 + 1) {
        if ((local_50 & 0x1f) != 0) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        pvVar4 = *(void **)((longlong)pvVar1 - 8);
        if (pvVar1 <= pvVar4) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if ((ulonglong)((longlong)pvVar1 - (longlong)pvVar4) < 8) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if (0x27 < (ulonglong)((longlong)pvVar1 - (longlong)pvVar4)) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
      }
      free(pvVar4);
    }
    local_38 = 0xf;
    local_40 = 0;
    local_50 = 0;
    sVar3 = strlen("hit_count");
    FUN_140070d00(&local_50,"hit_count",sVar3);
    FUN_14032e360(param_1,param_2,(longlong)puVar2 + 0xc,&local_50);
    if (0xf < local_38) {
      pvVar1 = (void *)CONCAT71(uStack_4f,local_50);
      pvVar4 = pvVar1;
      if (0xfff < local_38 + 1) {
        if ((local_50 & 0x1f) != 0) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        pvVar4 = *(void **)((longlong)pvVar1 - 8);
        if (pvVar1 <= pvVar4) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if ((ulonglong)((longlong)pvVar1 - (longlong)pvVar4) < 8) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if (0x27 < (ulonglong)((longlong)pvVar1 - (longlong)pvVar4)) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
      }
      free(pvVar4);
    }
    local_38 = 0xf;
    local_40 = 0;
    local_50 = 0;
    sVar3 = strlen("counter_count");
    FUN_140070d00(&local_50,"counter_count",sVar3);
    FUN_14032e360(param_1,param_2,puVar2 + 2,&local_50);
    if (0xf < local_38) {
      pvVar1 = (void *)CONCAT71(uStack_4f,local_50);
      pvVar4 = pvVar1;
      if (0xfff < local_38 + 1) {
        if ((local_50 & 0x1f) != 0) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        pvVar4 = *(void **)((longlong)pvVar1 - 8);
        if (pvVar1 <= pvVar4) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if ((ulonglong)((longlong)pvVar1 - (longlong)pvVar4) < 8) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if (0x27 < (ulonglong)((longlong)pvVar1 - (longlong)pvVar4)) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
      }
      free(pvVar4);
    }
    local_38 = 0xf;
    local_40 = 0;
    local_50 = 0;
    sVar3 = strlen("guard_freeze_frame");
    FUN_140070d00(&local_50,"guard_freeze_frame",sVar3);
    FUN_14032e360(param_1,param_2,(longlong)puVar2 + 0x14,&local_50);
    if (0xf < local_38) {
      pvVar1 = (void *)CONCAT71(uStack_4f,local_50);
      pvVar4 = pvVar1;
      if (0xfff < local_38 + 1) {
        if ((local_50 & 0x1f) != 0) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        pvVar4 = *(void **)((longlong)pvVar1 - 8);
        if (pvVar1 <= pvVar4) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if ((ulonglong)((longlong)pvVar1 - (longlong)pvVar4) < 8) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if (0x27 < (ulonglong)((longlong)pvVar1 - (longlong)pvVar4)) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
      }
      free(pvVar4);
    }
    local_38 = 0xf;
    local_40 = 0;
    local_50 = 0;
    sVar3 = strlen("hit_freeze_frame");
    FUN_140070d00(&local_50,"hit_freeze_frame",sVar3);
    FUN_14032e360(param_1,param_2,puVar2 + 3,&local_50);
    if (0xf < local_38) {
      pvVar1 = (void *)CONCAT71(uStack_4f,local_50);
      pvVar4 = pvVar1;
      if (0xfff < local_38 + 1) {
        if ((local_50 & 0x1f) != 0) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        pvVar4 = *(void **)((longlong)pvVar1 - 8);
        if (pvVar1 <= pvVar4) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if ((ulonglong)((longlong)pvVar1 - (longlong)pvVar4) < 8) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if (0x27 < (ulonglong)((longlong)pvVar1 - (longlong)pvVar4)) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
      }
      free(pvVar4);
    }
    local_38 = 0xf;
    local_40 = 0;
    local_50 = 0;
    sVar3 = strlen("counter_freeze_frame");
    FUN_140070d00(&local_50,"counter_freeze_frame",sVar3);
    FUN_14032e360(param_1,param_2,(longlong)puVar2 + 0x1c,&local_50);
    if (0xf < local_38) {
      pvVar1 = (void *)CONCAT71(uStack_4f,local_50);
      pvVar4 = pvVar1;
      if (0xfff < local_38 + 1) {
        if ((local_50 & 0x1f) != 0) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        pvVar4 = *(void **)((longlong)pvVar1 - 8);
        if (pvVar1 <= pvVar4) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if ((ulonglong)((longlong)pvVar1 - (longlong)pvVar4) < 8) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
        if (0x27 < (ulonglong)((longlong)pvVar1 - (longlong)pvVar4)) {
                    /* WARNING: Subroutine does not return */
          _invalid_parameter_noinfo_noreturn();
        }
      }
      free(pvVar4);
    }
  }
  return puVar2;
}

