// Requested 14030ba30; entry 14030ba30; auto-decompiled, types/names provisional.

void FUN_14030ba30(longlong param_1,longlong param_2,float param_3)

{
  int iVar1;
  float fVar2;
  double dVar3;
  float fVar4;
  float fVar5;
  
  iVar1 = *(int *)(param_2 + 0x14);
  fVar4 = (float)iVar1;
  if (0.0 < (float)*(int *)(param_2 + 8)) {
    *(float *)(param_1 + 0x3c) = (float)*(int *)(param_2 + 8);
  }
  fVar2 = DAT_1412c6f50;
  if ((0.0 < fVar4) || (dVar3 = fabs((double)iVar1), (float)dVar3 <= fVar2)) {
    *(float *)(param_1 + 0x40) = fVar4;
  }
  if (((0.0 < param_3) || (dVar3 = fabs((double)param_3), (float)dVar3 <= fVar2)) &&
     (param_3 < *(float *)(param_1 + 0x40))) {
    *(float *)(param_1 + 0x40) = param_3;
  }
  iVar1 = *(int *)(param_2 + 0x18);
  fVar5 = (float)*(int *)(param_2 + 0xc);
  fVar4 = (float)iVar1;
  if (0.0 < fVar5) {
    *(float *)(param_1 + 0x44) = fVar5;
  }
  if ((0.0 < fVar4) || (dVar3 = fabs((double)iVar1), (float)dVar3 <= fVar2)) {
    *(float *)(param_1 + 0x48) = fVar4;
  }
  if (((0.0 < param_3) || (dVar3 = fabs((double)param_3), (float)dVar3 <= fVar2)) &&
     (param_3 < *(float *)(param_1 + 0x48))) {
    *(float *)(param_1 + 0x48) = param_3;
  }
  if (0.0 < fVar5) {
    *(float *)(param_1 + 0x4c) = fVar5;
  }
  if ((0.0 < fVar4) || (dVar3 = fabs((double)iVar1), (float)dVar3 <= fVar2)) {
    *(float *)(param_1 + 0x50) = fVar4;
  }
  if (((0.0 < param_3) || (dVar3 = fabs((double)param_3), (float)dVar3 <= fVar2)) &&
     (param_3 < *(float *)(param_1 + 0x50))) {
    *(float *)(param_1 + 0x50) = param_3;
  }
  iVar1 = *(int *)(param_2 + 0x1c);
  fVar4 = (float)iVar1;
  if (0.0 < (float)*(int *)(param_2 + 0x10)) {
    *(float *)(param_1 + 0x4c) = (float)*(int *)(param_2 + 0x10);
  }
  if ((0.0 < fVar4) || (dVar3 = fabs((double)iVar1), (float)dVar3 <= fVar2)) {
    *(float *)(param_1 + 0x50) = fVar4;
  }
  if ((param_3 <= 0.0) && (dVar3 = fabs((double)param_3), fVar2 < (float)dVar3)) {
    return;
  }
  if (param_3 < *(float *)(param_1 + 0x50)) {
    *(float *)(param_1 + 0x50) = param_3;
  }
  return;
}

