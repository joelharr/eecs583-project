#include "ProfileSLP.h"

using namespace llvm;

bool ProfileSLP::reorder(
    std::vector<std::vector<Instruction*>> SLP_vecs,
    std::vector<std::vector<Instruction*>>* sortedOrders_p
){
    errs() << "Performing reordering... \n";
    int traceCtr = 0;
    auto sortedOrders = *sortedOrders_p;
    for(auto sortedOrder : sortedOrders){
        errs() << "Trace: " << traceCtr << "\n";
        ++traceCtr;
        int nInstrs = sortedOrder.size();
        if(nInstrs == 0){
            continue;
        }
        auto currentI = sortedOrder[0];
        auto currentBB = currentI->getParent();
        bool updateBBNext = false;
        for(int i = 1; i < sortedOrder.size(); ++i){
            auto in = sortedOrder[i];
            if(isa<PHINode>(in)){
                currentI = in;
                continue;
            }
            if(isa<BranchInst>(in)){ //Advance to filling the next BB
                updateBBNext = true;
            } else {
                auto nextI = in->clone();
                if(updateBBNext){ //Advanced to the next BB
                    currentBB = in->getParent();
                    auto firstInBB = currentBB->getIterator()->getFirstNonPHI();
                    currentI = firstInBB;
                    updateBBNext = false;
                }
                nextI->insertAfter(currentI);
                in->replaceAllUsesWith(nextI);
                currentI = nextI;
            }
        }
        for(int i = 1; i < nInstrs; ++i){
            auto in = sortedOrder[i];
            if(!isa<PHINode>(in) && !isa<BranchInst>(in)){
                in->eraseFromParent();
            }
        }
    }

    //Perform hoisting
    errs() << "Hoisting... \n";
    for(auto vec : SLP_vecs){ //Works because this vector was built in topological order
        auto first = vec[0];
        for(int i = 1; i < vec.size(); ++i){ //Hoist all others up to below first
            auto nextI = vec[i]->clone();
            nextI->insertAfter(first);
            vec[i]->replaceAllUsesWith(nextI);
        }
        for(int i = 1; i < vec.size(); ++i){
            vec[i]->eraseFromParent();
        }
    }
    return true;
}