//===-- NVPTXTargetTransformInfo.h - NVPTX specific TTI ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file a TargetTransformInfo::Concept conforming object specific to the
/// NVPTX target machine. It uses the target's detailed information to
/// provide more precise answers to certain TTI queries, while letting the
/// target independent and default TTI implementations handle the rest.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NVPTX_NVPTXTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_NVPTX_NVPTXTARGETTRANSFORMINFO_H

#include "NVPTX.h"
#include "NVPTXTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Target/TargetLowering.h"

namespace llvm {

class NVPTXTTIImpl : public BasicTTIImplBase<NVPTXTTIImpl> {
  typedef BasicTTIImplBase<NVPTXTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;

  const NVPTXTargetLowering *TLI;

public:
  explicit NVPTXTTIImpl(const NVPTXTargetMachine *TM)
      : BaseT(TM), TLI(TM->getSubtargetImpl()->getTargetLowering()) {}

  // Provide value semantics. MSVC requires that we spell all of these out.
  NVPTXTTIImpl(const NVPTXTTIImpl &Arg)
      : BaseT(static_cast<const BaseT &>(Arg)), TLI(Arg.TLI) {}
  NVPTXTTIImpl(NVPTXTTIImpl &&Arg)
      : BaseT(std::move(static_cast<BaseT &>(Arg))), TLI(std::move(Arg.TLI)) {}
  NVPTXTTIImpl &operator=(const NVPTXTTIImpl &RHS) {
    BaseT::operator=(static_cast<const BaseT &>(RHS));
    TLI = RHS.TLI;
    return *this;
  }
  NVPTXTTIImpl &operator=(NVPTXTTIImpl &&RHS) {
    BaseT::operator=(std::move(static_cast<BaseT &>(RHS)));
    TLI = std::move(RHS.TLI);
    return *this;
  }

  bool hasBranchDivergence() { return true; }

  unsigned getArithmeticInstrCost(
      unsigned Opcode, Type *Ty,
      TTI::OperandValueKind Opd1Info = TTI::OK_AnyValue,
      TTI::OperandValueKind Opd2Info = TTI::OK_AnyValue,
      TTI::OperandValueProperties Opd1PropInfo = TTI::OP_None,
      TTI::OperandValueProperties Opd2PropInfo = TTI::OP_None);
};

} // end namespace llvm

#endif
