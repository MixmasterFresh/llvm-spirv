//===- UseListOrder.cpp - Implement Use List Order ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implement structures and command-line options for preserving use-list order.
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/UseListOrder.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool> PreserveBitcodeUseListOrder(
    "preserve-bc-uselistorder",
    cl::desc("Preserve use-list order when writing LLVM bitcode."),
    cl::init(false), cl::Hidden);

static cl::opt<bool> PreserveAssemblyUseListOrder(
    "preserve-ll-uselistorder",
    cl::desc("Preserve use-list order when writing LLVM assembly."),
    cl::init(false), cl::Hidden);

bool llvm::shouldPreserveBitcodeUseListOrder() {
  return PreserveBitcodeUseListOrder;
}

bool llvm::shouldPreserveAssemblyUseListOrder() {
  return PreserveAssemblyUseListOrder;
}

void llvm::setPreserveBitcodeUseListOrder(bool ShouldPreserve) {
  PreserveBitcodeUseListOrder = ShouldPreserve;
}

void llvm::setPreserveAssemblyUseListOrder(bool ShouldPreserve) {
  PreserveAssemblyUseListOrder = ShouldPreserve;
}
