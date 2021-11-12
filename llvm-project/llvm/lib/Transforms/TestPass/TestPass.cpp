#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct TestPass : public FunctionPass {
  static char ID;
  TestPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    errs() << "Hello: ";
    errs().write_escaped(F.getName()) << '\n';
    return false;
  }
}; // end of struct Hello
}  // end of anonymous namespace

char TestPass::ID = 0;
static RegisterPass<TestPass> X("TestPass", "TestPass Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
