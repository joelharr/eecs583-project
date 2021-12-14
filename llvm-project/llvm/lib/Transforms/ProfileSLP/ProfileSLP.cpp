#include "ProfileSLP.h"

using namespace llvm;

// Specify the list of analysis passes that will be used inside your pass.
void ProfileSLP::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BlockFrequencyInfoWrapperPass>();
    AU.addRequired<BranchProbabilityInfoWrapperPass>();
    // AU.addRequired<CFG>();
}

bool ProfileSLP::runOnFunction(Function &F) {
    bool changed = getSuperblocks(F);
    #ifdef GET_SLP //Enable and disable getting SLP
    changed |= getSLP(F);
    #endif
    return changed;
}

char ProfileSLP::ID = 0;
static RegisterPass<ProfileSLP> X("ProfileSLP", "Profile SLP Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
