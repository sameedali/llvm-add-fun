#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Type.h"
#include <string>
#include <iostream>

using namespace llvm;

namespace {
struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
        LLVMContext &Ctx = F.getContext();
        std::vector<Type*> paramTypes = {};
        Type *retType = Type::getVoidTy(Ctx);
        FunctionType *gunFuncType = FunctionType::get(retType, paramTypes, false);
        Constant * gunFunc = F.getParent()->getOrInsertFunction("gun", gunFuncType);

        for (auto &B : F) {
            outs() << "curent func: " << F.getName() << "\n";
            for (auto &I : B) {
                if (auto *op = dyn_cast<CallInst>(&I)) {
                    // Insert at the point where the instruction `op` appears.
                    std::string s = op->getCalledFunction()->getName();
                    outs() << "call instr calling func:" << s << "\n";
                    if (s.compare("fun") == 0) {
                        outs() << "adding target function before: fun\n";
                        I.print(outs());
                        outs() << "\n";

                        // BasicBlock::iterator ii(op);
                        // Value* args[] = {op};
                        // ReplaceInstWithInst(op->getParent()->getInstList(), ii, llvm::CallInst::Create(gunFunc, args));

                        IRBuilder<> builder(op);
                        builder.SetInsertPoint(&B, builder.GetInsertPoint());
                        // Value* args[] = {};
                        auto gunCall = builder.CreateCall(gunFunc);
                        gunCall->print(outs());
                        outs() << "\n";

                        // We modified the code.
                        return true;
                    }
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
// static RegisterPass<skeletonpass> X("Skeletonpass", "Insert custom pass protectors", false, false);
