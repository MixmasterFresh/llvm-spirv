//===- PDBSymbolCompilandDetails.cpp - compiland details --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <utility>

#include "llvm/DebugInfo/PDB/PDBSymbol.h"
#include "llvm/DebugInfo/PDB/PDBSymbolCompilandDetails.h"

using namespace llvm;

PDBSymbolCompilandDetails::PDBSymbolCompilandDetails(
    const IPDBSession &PDBSession, std::unique_ptr<IPDBRawSymbol> Symbol)
    : PDBSymbol(PDBSession, std::move(Symbol)) {}

void PDBSymbolCompilandDetails::dump(raw_ostream &OS, int Indent,
                                     PDB_DumpLevel Level) const {}
