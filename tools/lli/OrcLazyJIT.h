//===--- OrcLazyJIT.h - Basic Orc-based JIT for lazy execution --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Simple Orc-based JIT. Uses the compile-on-demand layer to break up and
// lazily compile modules.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TOOLS_LLI_ORCLAZYJIT_H
#define LLVM_TOOLS_LLI_ORCLAZYJIT_H

#include "llvm/ADT/Triple.h"
#include "llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LazyEmittingLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/IR/LLVMContext.h"

namespace llvm {

class OrcLazyJIT {
public:

  typedef orc::JITCompileCallbackManagerBase CompileCallbackMgr;
  typedef orc::ObjectLinkingLayer<> ObjLayerT;
  typedef orc::IRCompileLayer<ObjLayerT> CompileLayerT;
  typedef orc::LazyEmittingLayer<CompileLayerT> LazyEmitLayerT;
  typedef orc::CompileOnDemandLayer<LazyEmitLayerT,
                                    CompileCallbackMgr> CODLayerT;
  typedef CODLayerT::ModuleSetHandleT ModuleHandleT;

  OrcLazyJIT(std::unique_ptr<TargetMachine> TM, LLVMContext &Context)
    : Error(false), TM(std::move(TM)),
      Mang(this->TM->getDataLayout()),
      ObjectLayer([](){ return llvm::make_unique<SectionMemoryManager>(); }),
      CompileLayer(ObjectLayer, orc::SimpleCompiler(*this->TM)),
      LazyEmitLayer(CompileLayer),
      CCMgr(createCallbackMgr(Triple(this->TM->getTargetTriple()), Context)),
      CODLayer(LazyEmitLayer, *CCMgr) { }

  bool Ok() const { return !Error; }

  ModuleHandleT addModule(std::unique_ptr<Module> M) {
    // Attach a data-layout if one isn't already present.
    if (M->getDataLayout().isDefault())
      M->setDataLayout(*TM->getDataLayout());

    std::vector<std::unique_ptr<Module>> S;
    S.push_back(std::move(M));
    return CODLayer.addModuleSet(std::move(S));
  }

  orc::JITSymbol findSymbol(const std::string &Name) {
    return CODLayer.findSymbol(mangle(Name), true);
  }

  orc::JITSymbol findSymbolIn(ModuleHandleT H, const std::string &Name) {
    return CODLayer.findSymbolIn(H, mangle(Name), true);
  }

private:

  std::unique_ptr<CompileCallbackMgr>
  createCallbackMgr(Triple T, LLVMContext &Context);

  std::string mangle(const std::string &Name) {
    std::string MangledName;
    {
      raw_string_ostream MangledNameStream(MangledName);
      Mang.getNameWithPrefix(MangledNameStream, Name);
    }
    return MangledName;
  }

  bool Error;
  std::unique_ptr<TargetMachine> TM;
  Mangler Mang;

  ObjLayerT ObjectLayer;
  CompileLayerT CompileLayer;
  LazyEmitLayerT LazyEmitLayer;
  std::unique_ptr<CompileCallbackMgr> CCMgr;
  CODLayerT CODLayer;
};

int runOrcLazyJIT(std::unique_ptr<Module> M, int ArgC, char* ArgV[]);

} // end namespace llvm

#endif
