// Requested 1406b3060; entry 1406b3060; auto-decompiled, types/names provisional.

void FUN_1406b3060(longlong *param_1,undefined4 param_2,undefined8 param_3)

{
  void *_Src;
  void *_Dst;
  code *pcVar1;
  longlong lVar2;
  longlong lVar3;
  int iVar4;
  undefined1 auStack_78 [32];
  undefined8 local_58;
  longlong *local_48;
  longlong *local_40;
  undefined4 local_38 [2];
  ulonglong local_30;
  
  local_30 = DAT_14198f720 ^ (ulonglong)auStack_78;
  local_38[0] = param_2;
  FUN_1406ba8b0(param_1 + 3,&local_48,local_38);
  lVar3 = 0;
  if (local_48 != local_40) {
    do {
      local_48 = (longlong *)*local_48;
      lVar3 = lVar3 + 1;
    } while (local_48 != local_40);
    if (lVar3 != 0) goto LAB_1406b3146;
  }
  FUN_1401a7fe0(param_1,local_38);
  lVar3 = *(longlong *)param_1[4];
  local_58 = param_3;
  lVar2 = FUN_1406a7ab0(param_1 + 4,lVar3,*(undefined8 *)(lVar3 + 8),local_38);
  if (param_1[5] == 0x7fffffffffffffe) {
    std::_Xlength_error("list<T> too long");
    pcVar1 = (code *)swi(3);
    (*pcVar1)();
    return;
  }
  param_1[5] = param_1[5] + 1;
  *(longlong *)(lVar3 + 8) = lVar2;
  **(longlong **)(lVar2 + 8) = lVar2;
  FUN_1406a8640(param_1 + 3,&local_48,*(longlong *)param_1[4] + 0x10);
LAB_1406b3146:
  lVar3 = *param_1;
  iVar4 = (int)(param_1[1] - lVar3 >> 2) + -0x80;
  if (iVar4 < 0) {
    iVar4 = 0;
  }
  if (0 < iVar4) {
    if (lVar3 != param_1[1]) {
      do {
        if ((longlong)iVar4 <= lVar3 - *param_1 >> 2) break;
        FUN_1406baab0(param_1 + 3,lVar3);
        lVar3 = lVar3 + 4;
      } while (lVar3 != param_1[1]);
    }
    _Dst = (void *)*param_1;
    _Src = (void *)((longlong)_Dst + (longlong)iVar4 * 4);
    if (_Src == (void *)param_1[1]) {
      param_1[1] = (longlong)_Dst;
    }
    else if (_Dst != _Src) {
      lVar3 = param_1[1];
      memmove(_Dst,_Src,lVar3 - (longlong)_Src);
      param_1[1] = (lVar3 - (longlong)_Src) + (longlong)_Dst;
    }
  }
  __security_check_cookie(local_30 ^ (ulonglong)auStack_78);
  return;
}

