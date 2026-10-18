// Requested 140c388f0; entry 140c388f0; auto-decompiled, types/names provisional.

undefined8 * FUN_140c388f0(undefined8 *param_1,undefined8 param_2)

{
  undefined **local_68 [7];
  undefined ***local_30;
  undefined8 local_28;
  undefined8 *local_20;
  
  local_28 = 0xfffffffffffffffe;
  *(undefined4 *)(param_1 + 1) = 0;
  param_1[2] = 0xffffffffffffffff;
  param_1[4] = 0;
  *(undefined1 *)(param_1 + 4) = 0;
  *(undefined4 *)((longlong)param_1 + 0x24) = 0;
  local_20 = param_1;
  FUN_140c2b4a0(param_1 + 5);
  *param_1 = RobActStateGuard::vftable;
  param_1[3] = RobActStateGuard::vftable;
  param_1[5] = RobActStateGuard::vftable;
  FUN_140c38830(param_1 + 0xb6,param_2);
  FUN_140c7de80(param_1 + 5);
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverWalk,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderWalk_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2ed80(local_68,param_1 + 0x16);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverTurn,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderTurn_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2eba0(local_68,param_1 + 0x66);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverRun,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderRun_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e8d0(local_68,param_1 + 0x1e);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverStep,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderStep_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e9c0(local_68,param_1 + 0x3e);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverIdle,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderIdle_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e600(local_68,param_1 + 0x36);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverAttack,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderAttack_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e060(local_68,param_1 + 0x26);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverDead,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderDead_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e150(local_68,param_1 + 0x46);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverStun,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderStun_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2eab0(local_68,param_1 + 0x4e);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverDonotAct,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderDonotAct_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e240(local_68,param_1 + 0x56);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverPetrify,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderPetrify_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e7e0(local_68,param_1 + 0x5e);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverEyeAttack,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderEyeAttack_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e330(local_68,param_1 + 0x6e);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverGoUnderground,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderGoUnderground_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e510(local_68,param_1 + 0x76);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverGoAboveground,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderGoAboveground_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2e420(local_68,param_1 + 0x7e);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])
              (local_30,CONCAT71((int7)((ulonglong)local_68 >> 8),local_30 != local_68));
  }
  local_68[0] = std::
                _Func_impl<class_RobCtrlOrderReceiver::ReceiverUndergroundIdle,class_std::allocator<int>,bool,class_RobImpl&___ptr64,class_RobAction&___ptr64,class_RobCtrlOrderUndergroundIdle_const&___ptr64>
                ::vftable;
  local_30 = local_68;
  FUN_140c2ec90(local_68,param_1 + 0xae);
  if (local_30 != (undefined ***)0x0) {
    (*(code *)(*local_30)[4])(local_30,local_30 != local_68);
  }
  return param_1;
}

