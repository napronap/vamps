// Requested 140c7b9e0; entry 140c7b9e0; auto-decompiled, types/names provisional.

void FUN_140c7b9e0(longlong param_1)

{
  if (*(longlong *)(param_1 + 0x38) == 0) {
    return;
  }
  if ((*(byte *)(param_1 + 0x40) & 1) != 0) {
    thunk_FUN_140c7e1c0(*(longlong *)(param_1 + 0x38),0);
    return;
  }
  thunk_FUN_140c7e130();
  return;
}

