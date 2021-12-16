#include "ProfileSLP.h"

using namespace llvm;

//Utility function
void ProfileSLP::printInstrGroup(std::vector<Instruction*> group){
    for(const auto instr : group){
        errs() << *instr << "\n";
    }
}

// Specify the list of analysis passes that will be used inside your pass.
void ProfileSLP::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BlockFrequencyInfoWrapperPass>();
    AU.addRequired<BranchProbabilityInfoWrapperPass>();
    // AU.addRequired<CFG>();
}

bool ProfileSLP::runOnFunction(Function &F) {
    bool changed = getSuperblocks(F);
    auto SLP_vecs = getSLP(F);
    changed |= hoist(SLP_vecs);
    //LLVMContext& context = F.getContext();
    //vectorize(SLP_vecs[0], context);
    //vectorize(SLP_vecs[1], context);
    return true;
}

char ProfileSLP::ID = 0;
static RegisterPass<ProfileSLP> X("ProfileSLP", "Profile SLP Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
