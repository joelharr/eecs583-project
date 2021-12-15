#include "ProfileSLP.h"

using namespace llvm;

struct IsomorphicGroup {
    bool validOp;
    int size;
    std::string opName;
    ProfileSLP::OpType opType;
    std::vector<Instruction*> instrs;

    IsomorphicGroup(Instruction* instr){
        opName = instr->getOpcodeName();
        opType = ProfileSLP::opToInstr[opName];
        validOp = (opType == ProfileSLP::OpType::IALU) || (opType == ProfileSLP::OpType::FALU); 
        size = 1;
    }

    bool isIsomorphicToGroup(Instruction* newInstr){
        if(validOp){
            std::string newOpName = newInstr->getOpcodeName();
            if(opName == newOpName){ //Compatible operations for vectorization
                //Don't pack this instruction into the group if it uses the results of previous 
                // instructions within the same group.
                for(auto groupInst : instrs){
                    for(User *U : groupInst->users()){
                        if (Instruction *inst = dyn_cast<Instruction>(U)) {
                            if(newInstr == inst){
                                return false;
                            }
                        }
                    }
                }
                return true;
            }
        }
        return false;
    }

    void insertIfIsomorphic(Instruction* newInstr){
        if (isIsomorphicToGroup(newInstr)){
            instrs.push_back(newInstr);
            ++size;
        }
    }
};

//Breadth-first toplogical sort
std::vector<int> ProfileSLP::BF_ToplogicalSort(const std::map<Instruction*, int> &instrs){
    //Construct a DAG for the def-use chains
    int nInstrs = instrs.size();
    std::vector<std::vector<int>> DAG(nInstrs);
    for(const auto &from : instrs){
        for(User *U : from.first->users()){
            if (Instruction *inst = dyn_cast<Instruction>(U)) {
                DAG[from.second].push_back(instrs.find(inst)->second);
            }
        }
    }

    //Compute the number of dependencies that each instruction has
    std::vector<int> numDeps(nInstrs, 0);
    for(int i = 0; i < nInstrs; ++i){
        for(int j : DAG[i]){
            ++numDeps[DAG[i][j]];
        }
    }

    //Queue instructions that have no dependencies
    std::queue<int> q;
    for(int i = 0; i < nInstrs; ++i){
        if(numDeps[i] == 0){
            q.push(i);
        }
    }

    //Dequeue nodes with zero dependencies, then iteratively update the dependency list
    std::vector<int> solution;
    while(q.size() > 0){
        //Add to solution from queue
        int current = q.front();
        q.pop();
        solution.push_back(current);

        //Update dependency list
        //Anything that depends on the 'current' instruction now has one less dependency
        for(int i : DAG[current]){ 
            int dependent = DAG[current][i];
            --numDeps[dependent];
            if(numDeps[dependent] == 0) { 
                q.push(dependent); //No dependencies left, so add to queue
            }
        }
    }
    return solution;
}

bool ProfileSLP::getSLP(Function &F){
    //Iterate through all traces
    int vectorizedCount = 0;
    for(int i = 0; i < m_traces.size(); ++i){
        errs() << "SLP Trace " << i << "\n";

        //Extract all instructions
        std::map<Instruction*, int> instr_map;
        std::vector<Instruction*> instrs;
        for(int j = 0; j < m_traces[i].size(); ++j){
            BasicBlock* b = m_traces[i][j];
            errs() << "BB: " << *b << "\n";
            for(auto &in : *b){ 
                instr_map[&in] = j;
                instrs.push_back(&in);
            }
        }

        //Find the breadth-first topological ordering of the def-use chains within this superblock
        auto topsort_order = BF_ToplogicalSort(instr_map);

        //Build vectorizable groups
        std::map<std::string, IsomorphicGroup*> vec_cands;
        std::vector<IsomorphicGroup*> vec_confirmed; //Move isomorphic groups here once they're full
        IsomorphicGroup* currentGroup;
        for(auto idx : topsort_order){
            auto in = instrs[idx];
            //This is the "visit" function 
            if (isa<StoreInst>(in)){ //Don't try to hoist up through stores, so reset vectorization candidates
                //Process isomorphic groups
                vec_cands.clear();
            } else {
                std::string opName = in->getOpcodeName();
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