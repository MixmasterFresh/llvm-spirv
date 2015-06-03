//===- MCSymbolELF.h -  -----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_MC_MCSYMBOLELF_H
#define LLVM_MC_MCSYMBOLELF_H

#include "llvm/MC/MCSymbol.h"

namespace llvm {
class MCSymbolELF : public MCSymbol {
  /// An expression describing how to calculate the size of a symbol. If a
  /// symbol has no size this field will be NULL.
  const MCExpr *SymbolSize = nullptr;

  mutable unsigned BindingSet : 1;

public:
  MCSymbolELF(const StringMapEntry<bool> *Name, bool isTemporary)
      : MCSymbol(true, Name, isTemporary), BindingSet(false) {}
  void setSize(const MCExpr *SS) { SymbolSize = SS; }

  const MCExpr *getSize() const { return SymbolSize; }

  void setVisibility(unsigned Visibility);
  unsigned getVisibility() const;

  void setOther(unsigned Other);
  unsigned getOther() const;

  void setType(unsigned Type) const;
  unsigned getType() const;

  void setBinding(unsigned Binding) const;
  unsigned getBinding() const;

  bool isBindingSet() const { return BindingSet; }

  static bool classof(const MCSymbol *S) { return S->isELF(); }
};
}

#endif
