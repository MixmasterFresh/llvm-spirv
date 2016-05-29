//===- PDBSymbolFunc.cpp - --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/DebugInfo/PDB/IPDBEnumChildren.h"
#include "llvm/DebugInfo/PDB/IPDBSession.h"
#include "llvm/DebugInfo/PDB/PDBSymbol.h"
#include "llvm/DebugInfo/PDB/PDBSymbolFunc.h"
#include "llvm/DebugInfo/PDB/PDBSymbolFuncDebugEnd.h"
#include "llvm/DebugInfo/PDB/PDBSymbolFuncDebugStart.h"
#include "llvm/DebugInfo/PDB/PDBSymbolTypeFunctionSig.h"
#include "llvm/DebugInfo/PDB/PDBSymbolTypeUDT.h"
#include "llvm/Support/Format.h"
#include <utility>

using namespace llvm;
PDBSymbolFunc::PDBSymbolFunc(const IPDBSession &PDBSession,
                             std::unique_ptr<IPDBRawSymbol> Symbol)
    : PDBSymbol(PDBSession, std::move(Symbol)) {}

void PDBSymbolFunc::dump(raw_ostream &OS, int Indent,
                         PDB_DumpLevel Level) const {
  OS << stream_indent(Indent);
  if (Level >= PDB_DumpLevel::Normal) {
    uint32_t FuncStart = getRelativeVirtualAddress();
    uint32_t FuncEnd = FuncStart + getLength();
    if (FuncStart == 0 && FuncEnd == 0) {
      OS << "func [???]";
    } else {
      OS << "func ";
      OS << "[" << format_hex(FuncStart, 8);
      if (auto DebugStart = findOneChild<PDBSymbolFuncDebugStart>())
        OS << "+" << DebugStart->getRelativeVirtualAddress() - FuncStart;
      OS << " - " << format_hex(FuncEnd, 8);
      if (auto DebugEnd = findOneChild<PDBSymbolFuncDebugEnd>())
        OS << "-" << FuncEnd - DebugEnd->getRelativeVirtualAddress();
      OS << "] ";
    }

    PDB_RegisterId Reg = getLocalBasePointerRegisterId();
    if (Reg == PDB_RegisterId::VFrame)
      OS << "(VFrame)";
    else if (hasFramePointer())
      OS << "(" << Reg << ")";
    else
      OS << "(FPO)";

    OS << " ";
    uint32_t FuncSigId = getTypeId();
    if (auto FuncSig = Session.getConcreteSymbolById<PDBSymbolTypeFunctionSig>(
            FuncSigId)) {
      OS << "(" << FuncSig->getCallingConvention() << ") ";
    }

    uint32_t ClassId = getClassParentId();
    if (ClassId != 0) {
      if (auto Class = Session.getSymbolById(ClassId)) {
        if (auto UDT = dyn_cast<PDBSymbolTypeUDT>(Class.get()))
          OS << UDT->getName() << "::";
        else
          OS << "{class " << Class->getSymTag() << "}::";
      }
    }
    OS << getName();
  } else {
    OS << getName();
  }
}
