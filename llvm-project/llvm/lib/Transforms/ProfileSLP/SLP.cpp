#include "ProfileSLP.h"

using namespace llvm;

struct IsomorphicGroup {
    bool validOp;
    int size;
    std::string opName;
    ProfileSLP::OpType opType;
    std::vector<Instruction*> instrs;

    IsomorphicGroup(Instruction &instr){
        opName = instr.getOpcodeName();
        opType = ProfileSLP::opToInstr[opName];
        validOp = (opType == ProfileSLP::OpType::IALU) || (opType == ProfileSLP::OpType::FALU); 
        size = 1;
    }

    bool isIsomorphicToGroup(Instruction &newInstr){
        if(validOp){
            std::string newOpName = newInstr.getOpcodeName();
            if(opName == newOpName){ //Compatible operations for vectorization
                //TODO: Check DU chains here
                return true;
            }
        }
        return false;
    }

    void insertIfIsomorphic(Instruction &newInstr){
        if (isIsomorphicToGroup(newInstr)){
            instrs.push_back(&newInstr);
            ++size;
        }
    }
};

bool ProfileSLP::getSLP(Function &F){
    //Iterate through all traces
    int vectorizedCount = 0;
    for(int i = 0; i < m_traces.size(); ++i){
        errs() << "SLP Trace " << i << "\n";
        std::map<std::string, IsomorphicGroup*> vec_cands;
        std::vector<IsomorphicGroup*> vec_confirmed; //Move isomorphic groups here once they're full
        IsomorphicGroup* currentGroup;
        for(int j = 0; j < m_traces[i].size(); ++j){
            BasicBlock* b = m_traces[i][j];
            errs() << "BB: " << *b << "\n";
            for(auto &in : *b){
                if (isa<StoreInst>(&in)){ //Don't try to hoist up through stores, so reset vectorization candidates
                    //Process isomorphic groups
                    vec_cands.clear();
                } else {
                    std::string opName = in.getOpcodeName();
                    //errs() << opName << "\n";
                    if(vec_cands.find(opName) == vec_cands.end()){ //Start a new isomorphic group with a given op type
                        currentGroup = new IsomorphicGroup(in);
                        vec_cands.insert(std::pair<std::string, IsomorphicGroup*>(opName, currentGroup));
                    } else {
                        currentGroup = vec_cands[opName];
                        //errs() << "Found group: " << currentGroup->opName << " from key: " << opName << "\n";
                        currentGroup->insertIfIsomorphic(in);
                        if(currentGroup->size == ProfileSLP::SIMD_WIDTH){ //Upgrade to confirmed vectorization once we've reached SIMD width
                            errs() << "Found a vector of ops: " << opName << "\n";
                            vec_confirmed.push_back(currentGroup);
                            vec_cands.erase(opName);
                            ++vectorizedCount;
                        }
                    }
                }
            }
        }
    }

    return false;
}

std::map<std::string, ProfileSLP::OpType> ProfileSLP::opToInstr = {
    // Branch Instructions
    {"br", UB_BRANCH}, // Start as unbiased, change to biased
    {"switch", UB_BRANCH}, 
    {"indirectbr", UB_BRANCH},

    // Integer ALU Instructions
    {"add", IALU},
    {"sub", IALU},
    {"mul", IALU},
    {"udiv", IALU},
    {"sdiv", IALU},
    {"urem", IALU},
    {"shl", IALU},
    {"lshr", IALU},
    {"ashr", IALU},
    {"and", IALU},
    {"or", IALU},
    {"xor", IALU},
    {"icmp", IALU},
    {"srem", IALU},

    // Floating Point ALU Instructions
    {"fadd", FALU},
    {"fsub", FALU},
    {"fmul", FALU},
    {"fdiv", FALU},
    {"frem", FALU},
    {"fcmp", FALU},

    // Memory Instructions
    {"alloca", MEM},
    {"load", MEM},
    {"store", MEM},
    {"getelementptr", MEM},
    {"fence", MEM},
    {"cmpxchg", MEM},
    {"atomicrmw", MEM}
};