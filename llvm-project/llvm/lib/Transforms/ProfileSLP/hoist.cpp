#include "ProfileSLP.h"

using namespace llvm;

bool ProfileSLP::hoist(std::vector<std::vector<Instruction*>> SLP_vecs){
    errs() << "Hoisting... \n";
    for(auto vec : SLP_vecs){ //Works because this vector was built in topological order
        auto first = vec[0];
        for(int i = 1; i < vec.size(); ++i){ //Hoist all others up to below first
            auto newInst = vec[i]->clone();
            newInst->insertAfter(first);
            vec[i]->replaceAllUsesWith(newInst);
        }
        for(int i = 1; i < vec.size(); ++i){
            vec[i]->eraseFromParent();
        }
    }
    return SLP_vecs.size() != 0; //We hoisted something as long as there was at least one vector
}