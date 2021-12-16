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
    std::vector<std::vector<Instruction*>> sortedOrders(m_traces->size());
    auto SLP_vecs = getSLP(&sortedOrders);
    changed |= reorder(&SLP_vecs, &sortedOrders);

    LLVMContext& context = F.getContext();
    vectorizeWrapper(SLP_vecs, context);
    //Print the final program
    errs() << "FINAL PROGRAM: \n";
    for (auto &bb : F) {
        errs() << bb << "\n";
    }
    return true;
}

char ProfileSLP::ID = 0;
static RegisterPass<ProfileSLP> X("ProfileSLP", "Profile SLP Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
