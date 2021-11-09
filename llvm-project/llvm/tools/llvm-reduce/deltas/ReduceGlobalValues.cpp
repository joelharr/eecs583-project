//===- ReduceGlobalValues.cpp - Specialized Delta Pass --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a function which calls the Generic Delta pass to reduce
// global value attributes/specifiers.
//
//===----------------------------------------------------------------------===//

#include "ReduceGlobalValues.h"
#include "llvm/IR/GlobalValue.h"

using namespace llvm;

static bool shouldReduceDSOLocal(GlobalValue &GV) {
  return GV.isDSOLocal() && !GV.isImplicitDSOLocal();
}

static bool shouldReduceVisibility(GlobalValue &GV) {
  return GV.getVisibility() != GlobalValue::VisibilityTypes::DefaultVisibility;
}

static bool shouldReduceUnnamedAddress(GlobalValue &GV) {
  return GV.getUnnamedAddr() != GlobalValue::UnnamedAddr::None;
}

static bool shouldReduceDLLStorageClass(GlobalValue &GV) {
  return GV.getDLLStorageClass() !=
         GlobalValue::DLLStorageClassTypes::DefaultStorageClass;
}

static bool shouldReduceThreadLocal(GlobalValue &GV) {
  return GV.isThreadLocal();
}

static void reduceGVs(Oracle &O, Module &Program) {
  for (auto &GV : Program.global_values()) {
    if (shouldReduceDSOLocal(GV) && !O.shouldKeep())
      GV.setDSOLocal(false);
    if (shouldReduceVisibility(GV) && !O.shouldKeep()) {
      bool IsImplicitDSOLocal = GV.isImplicitDSOLocal();
      GV.setVisibility(GlobalValue::VisibilityTypes::DefaultVisibility);
      if (IsImplicitDSOLocal)
        GV.setDSOLocal(false);
    }
    if (shouldReduceUnnamedAddress(GV) && !O.shouldKeep())
      GV.setUnnamedAddr(GlobalValue::UnnamedAddr::None);
    if (shouldReduceDLLStorageClass(GV) && !O.shouldKeep())
      GV.setDLLStorageClass(
          GlobalValue::DLLStorageClassTypes::DefaultStorageClass);
    if (shouldReduceThreadLocal(GV) && !O.shouldKeep())
      GV.setThreadLocal(false);
  }
}

static int countGVs(Module &Program) {
  int DSOLocalCount = count_if(Program.global_values(), [](GlobalValue &GV) {
    return shouldReduceDSOLocal(GV);
  });
  int VisibilityCount = count_if(Program.global_values(), [](GlobalValue &GV) {
    return shouldReduceVisibility(GV);
  });
  int UnnamedAddrCount = count_if(Program.global_values(), [](GlobalValue &GV) {
    return shouldReduceUnnamedAddress(GV);
  });
  int DLLStorageClassCount =
      count_if(Program.global_values(),
               [](GlobalValue &GV) { return shouldReduceDLLStorageClass(GV); });
  int ThreadLocalCount = count_if(Program.global_values(), [](GlobalValue &GV) {
    return shouldReduceThreadLocal(GV);
  });
  return DSOLocalCount + VisibilityCount + UnnamedAddrCount +
         DLLStorageClassCount + ThreadLocalCount;
}

void llvm::reduceGlobalValuesDeltaPass(TestRunner &Test) {
  outs() << "*** Reducing GlobalValues...\n";
  int GVCount = countGVs(Test.getProgram());
  runDeltaPass(Test, GVCount, reduceGVs);
}