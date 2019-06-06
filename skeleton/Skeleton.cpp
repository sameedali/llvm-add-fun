#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/InstrTypes.h"
#include <string>
#include <iostream>

using namespace llvm;

namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> paramTypes = {Type::getVoidTy(Ctx)};
      Type *retType = Type::getVoidTy(Ctx);
      FunctionType *gunFuncType = FunctionType::get(retType, paramTypes, false);
      Constant *gunFunc = F.getParent()->getOrInsertFunction("gun", gunFuncType);

      for (auto &B : F) {
	errs() << "curent func: " << F.getName() << "\n";

        for (auto &I : B) {
          if (auto *op = dyn_cast<CallInst>(&I)) {
            // Insert at the point where the instruction `op` appears.
 	    std::string s = op->getCalledFunction()->getName();
            errs() << "Called func:" << s << "\n";
	    if (s.compare("fun") == 0) {
		errs() << "found func fun\n";
            	IRBuilder<> builder(op);
            	builder.SetInsertPoint(&B, builder.GetInsertPoint());
	    	Value* args[] = {op};
            	builder.CreateCall(gunFunc, args);
	    }
            // We modified the code.
            return true;
          }
        }
      }

      return false;
    }
  };
}

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
