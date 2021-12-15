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
    auto SLP_vecs = getSLP(F);
    errs() << "SLP Vectorization Groups: \n"; 
    for(auto vec : SLP_vecs){
        errs() << "Group: \n";
        printInstrGroup(vec);
    }
    LLVMContext& context = F.getContext();
    vectorize(SLP_vecs[0], context);
    vectorize(SLP_vecs[1], context);
    #endif
    return true;
}

char ProfileSLP::ID = 0;
static RegisterPass<ProfileSLP> X("ProfileSLP", "Profile SLP Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
