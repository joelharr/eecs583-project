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
    errs() << "SLP Vectorization Groups: \n"; 
    for(auto vec : SLP_vecs){
        errs() << "Group: \n";
        printInstrGroup(vec);
    }
    LLVMContext& context = F.getContext();
    vectorizeWrapper(SLP_vecs, context);
    return true;
}

char ProfileSLP::ID = 0;
static RegisterPass<ProfileSLP> X("ProfileSLP", "Profile SLP Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
