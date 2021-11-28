#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"

using namespace llvm;

namespace {
struct ProfileSLP : public FunctionPass {
  static char ID;
  ProfileSLP() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BranchProbabilityInfoWrapperPass>(); // Analysis pass to load branch probability
  }

  bool runOnFunction(Function &F) override {
    errs() << "ProfileSLP Hello: ";
    errs().write_escaped(F.getName()) << '\n';
    BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI();
    return false;
  }
}; // end of struct ProfileSLP
}  // end of anonymous namespace

char ProfileSLP::ID = 0;
static RegisterPass<ProfileSLP> X("ProfileSLP", "ProfileSLP Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);