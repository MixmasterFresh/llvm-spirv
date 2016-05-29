//===- lib/MC/MCSymbolELF.cpp ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/Support/ELF.h"

namespace llvm {

namespace {
enum {
  ELF_STT_Shift = 0, // Shift value for STT_* flags
  ELF_STB_Shift = 4, // Shift value for STB_* flags
  ELF_STV_Shift = 8, // Shift value for STV_* flags
  ELF_STO_Shift = 10 // Shift value for STO_* flags
};
}

void MCSymbolELF::setBinding(unsigned Binding) const {
  BindingSet = true;
  assert(Binding == ELF::STB_LOCAL || Binding == ELF::STB_GLOBAL ||
         Binding == ELF::STB_WEAK || Binding == ELF::STB_GNU_UNIQUE);
  uint32_t OtherFlags = getFlags() & ~(0xf << ELF_STB_Shift);
  setFlags(OtherFlags | (Binding << ELF_STB_Shift));
}

unsigned MCSymbolELF::getBinding() const {
  if (isBindingSet()) {
    uint32_t Binding = (getFlags() & (0xf << ELF_STB_Shift)) >> ELF_STB_Shift;
    assert(Binding == ELF::STB_LOCAL || Binding == ELF::STB_GLOBAL ||
           Binding == ELF::STB_WEAK || Binding == ELF::STB_GNU_UNIQUE);
    return Binding;
  }

  if (isDefined())
    return ELF::STB_LOCAL;
  if (isUsedInReloc())
    return ELF::STB_GLOBAL;
  if (isWeakrefUsedInReloc())
    return ELF::STB_WEAK;
  if (isSignature())
    return ELF::STB_LOCAL;
  return ELF::STB_GLOBAL;
}

void MCSymbolELF::setType(unsigned Type) const {
  assert(Type == ELF::STT_NOTYPE || Type == ELF::STT_OBJECT ||
         Type == ELF::STT_FUNC || Type == ELF::STT_SECTION ||
         Type == ELF::STT_COMMON || Type == ELF::STT_TLS ||
         Type == ELF::STT_GNU_IFUNC);

  uint32_t OtherFlags = getFlags() & ~(0xf << ELF_STT_Shift);
  setFlags(OtherFlags | (Type << ELF_STT_Shift));
}

unsigned MCSymbolELF::getType() const {
  uint32_t Type = (getFlags() & (0xf << ELF_STT_Shift)) >> ELF_STT_Shift;
  assert(Type == ELF::STT_NOTYPE || Type == ELF::STT_OBJECT ||
         Type == ELF::STT_FUNC || Type == ELF::STT_SECTION ||
         Type == ELF::STT_COMMON || Type == ELF::STT_TLS ||
         Type == ELF::STT_GNU_IFUNC);
  return Type;
}

// Visibility is stored in the first two bits of st_other
// st_other values are stored in the second byte of get/setFlags
void MCSymbolELF::setVisibility(unsigned Visibility) {
  assert(Visibility == ELF::STV_DEFAULT || Visibility == ELF::STV_INTERNAL ||
         Visibility == ELF::STV_HIDDEN || Visibility == ELF::STV_PROTECTED);

  uint32_t OtherFlags = getFlags() & ~(0x3 << ELF_STV_Shift);
  setFlags(OtherFlags | (Visibility << ELF_STV_Shift));
}

unsigned MCSymbolELF::getVisibility() const {
  unsigned Visibility = (getFlags() & (0x3 << ELF_STV_Shift)) >> ELF_STV_Shift;
  assert(Visibility == ELF::STV_DEFAULT || Visibility == ELF::STV_INTERNAL ||
         Visibility == ELF::STV_HIDDEN || Visibility == ELF::STV_PROTECTED);
  return Visibility;
}

// Other is stored in the last six bits of st_other
// st_other values are stored in the second byte of get/setFlags
void MCSymbolELF::setOther(unsigned Other) {
  uint32_t OtherFlags = getFlags() & ~(0x3f << ELF_STO_Shift);
  setFlags(OtherFlags | (Other << ELF_STO_Shift));
}

unsigned MCSymbolELF::getOther() const {
  unsigned Other = (getFlags() & (0x3f << ELF_STO_Shift)) >> ELF_STO_Shift;
  return Other;
}

void MCSymbolELF::setUsedInReloc() const {
  UsedInReloc = true;
}

bool MCSymbolELF::isUsedInReloc() const {
  return UsedInReloc;
}

void MCSymbolELF::setIsWeakrefUsedInReloc() const { WeakrefUsedInReloc = true; }

bool MCSymbolELF::isWeakrefUsedInReloc() const { return WeakrefUsedInReloc; }

void MCSymbolELF::setIsSignature() const { IsSignature = true; }

bool MCSymbolELF::isSignature() const { return IsSignature; }
}
