//===-- MipsMachineFunctionInfo.cpp - Private data used for Mips ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MipsBaseInfo.h"
#include "MipsInstrInfo.h"
#include "MipsMachineFunction.h"
#include "MipsSubtarget.h"
#include "MipsTargetMachine.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static cl::opt<bool>
FixGlobalBaseReg("mips-fix-global-base-reg", cl::Hidden, cl::init(true),
                 cl::desc("Always use $gp as the global base register."));

// class MipsCallEntry.
MipsCallEntry::MipsCallEntry(StringRef N) : PseudoSourceValue(MipsPSV) {
#ifndef NDEBUG
  Name = N;
  Val = nullptr;
#endif
}

MipsCallEntry::MipsCallEntry(const GlobalValue *V)
    : PseudoSourceValue(MipsPSV) {
#ifndef NDEBUG
  Val = V;
#endif
}

bool MipsCallEntry::isConstant(const MachineFrameInfo *) const {
  return false;
}

bool MipsCallEntry::isAliased(const MachineFrameInfo *) const {
  return false;
}

bool MipsCallEntry::mayAlias(const MachineFrameInfo *) const {
  return false;
}

void MipsCallEntry::printCustom(raw_ostream &O) const {
  O << "MipsCallEntry: ";
#ifndef NDEBUG
  if (Val)
    O << Val->getName();
  else
    O << Name;
#endif
}

MipsFunctionInfo::~MipsFunctionInfo() {}

bool MipsFunctionInfo::globalBaseRegSet() const {
  return GlobalBaseReg;
}

unsigned MipsFunctionInfo::getGlobalBaseReg() {
  // Return if it has already been initialized.
  if (GlobalBaseReg)
    return GlobalBaseReg;

  MipsSubtarget const &STI =
      static_cast<const MipsSubtarget &>(MF.getSubtarget());

  const TargetRegisterClass *RC =
      STI.inMips16Mode()
          ? &Mips::CPU16RegsRegClass
          : STI.inMicroMipsMode()
                ? &Mips::GPRMM16RegClass
                : static_cast<const MipsTargetMachine &>(MF.getTarget())
                          .getABI()
                          .IsN64()
                      ? &Mips::GPR64RegClass
                      : &Mips::GPR32RegClass;
  return GlobalBaseReg = MF.getRegInfo().createVirtualRegister(RC);
}

bool MipsFunctionInfo::mips16SPAliasRegSet() const {
  return Mips16SPAliasReg;
}
unsigned MipsFunctionInfo::getMips16SPAliasReg() {
  // Return if it has already been initialized.
  if (Mips16SPAliasReg)
    return Mips16SPAliasReg;

  const TargetRegisterClass *RC = &Mips::CPU16RegsRegClass;
  return Mips16SPAliasReg = MF.getRegInfo().createVirtualRegister(RC);
}

void MipsFunctionInfo::createEhDataRegsFI() {
  for (int I = 0; I < 4; ++I) {
    const TargetRegisterClass *RC =
        static_cast<const MipsTargetMachine &>(MF.getTarget()).getABI().IsN64()
            ? &Mips::GPR64RegClass
            : &Mips::GPR32RegClass;

    EhDataRegFI[I] = MF.getFrameInfo()->CreateStackObject(RC->getSize(),
        RC->getAlignment(), false);
  }
}

bool MipsFunctionInfo::isEhDataRegFI(int FI) const {
  return CallsEhReturn && (FI == EhDataRegFI[0] || FI == EhDataRegFI[1]
                        || FI == EhDataRegFI[2] || FI == EhDataRegFI[3]);
}

MachinePointerInfo MipsFunctionInfo::callPtrInfo(StringRef Name) {
  std::unique_ptr<const MipsCallEntry> &E = ExternalCallEntries[Name];

  if (!E)
    E = llvm::make_unique<MipsCallEntry>(Name);

  return MachinePointerInfo(E.get());
}

MachinePointerInfo MipsFunctionInfo::callPtrInfo(const GlobalValue *Val) {
  std::unique_ptr<const MipsCallEntry> &E = GlobalCallEntries[Val];

  if (!E)
    E = llvm::make_unique<MipsCallEntry>(Val);

  return MachinePointerInfo(E.get());
}

int MipsFunctionInfo::getMoveF64ViaSpillFI(const TargetRegisterClass *RC) {
  if (MoveF64ViaSpillFI == -1) {
    MoveF64ViaSpillFI = MF.getFrameInfo()->CreateStackObject(
        RC->getSize(), RC->getAlignment(), false);
  }
  return MoveF64ViaSpillFI;
}

void MipsFunctionInfo::anchor() { }
