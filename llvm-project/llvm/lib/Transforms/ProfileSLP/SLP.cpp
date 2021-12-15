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
        validOp = (opType == ProfileSLP::OpType::IALU) ||
                  (opType == ProfileSLP::OpType::FALU) ||
                  (opType == ProfileSLP::OpType::LOAD);
        if(validOp){
            instrs.push_back(instr);
            size = 1;
        } else {
            size = 0;
        }
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
std::vector<int> ProfileSLP::BF_ToplogicalSort(std::map<Instruction*, int> &instrs){
    //Construct a DAG for the def-use chains
    errs() << "Building DAG \n";
    int nInstrs = instrs.size();
    std::vector<std::vector<int>> DAG(nInstrs);
    for(auto &from : instrs){
        for(User *U : from.first->users()){
            if (Instruction *inst = dyn_cast<Instruction>(U)) {
                if(instrs.find(inst) != instrs.end()){ //Must actually be in the trace
                    DAG[from.second].push_back(instrs.find(inst)->second);
                }
            }
        }
    }

    //Compute the number of dependencies that each instruction has
    errs() << "Computing dependencies \n";
    std::vector<int> numDeps(nInstrs, 0);
    for(int i = 0; i < nInstrs; ++i){
        for(int j : DAG[i]){
            ++numDeps[j];
        }
    }

    //Queue instructions that have no dependencies
    errs() << "Queueing dependencies \n";
    std::queue<int> q;
    for(int i = 0; i < nInstrs; ++i){
        if(numDeps[i] == 0){
            q.push(i);
        }
    }

    //Dequeue nodes with zero dependencies, then iteratively update the dependency list
    errs() << "Dequeueing \n";
    std::vector<int> solution;
    while(q.size() > 0){
        //Add to solution from queue
        int current = q.front();
        q.pop();
        solution.push_back(current);

        //Update dependency list
        //Anything that depends on the 'current' instruction now has one less dependency
        for(int i : DAG[current]){ 
            --numDeps[i];
            if(numDeps[i] == 0) { 
                q.push(i); //No dependencies left, so add to queue
            }
        }
    }
    return solution;
}

void ProfileSLP::printInstrGroup(std::vector<Instruction*> group){
    for(const auto instr : group){
        errs() << *instr << "\n";
    }
}

std::vector<std::vector<Instruction*>> ProfileSLP::getSLP(Function &F){
    errs() << "Running getSLP... \n";
    //Iterate through all traces
    std::vector<std::vector<BasicBlock *>> traces = *m_traces;
    std::vector<std::vector<Instruction*>> vec_confirmed; //Move isomorphic groups here once they're full
    int vectorizedCount = 0;
    for(int i = 0; i < traces.size(); ++i){
        errs() << "SLP Trace " << i << "\n";

        //Extract all instructions
        std::map<Instruction*, int> instr_map;
        std::vector<Instruction*> instrs;
        int instr_num = 0;
        for(BasicBlock *b : traces[i]){
            errs() << "BB: " << *b << "\n";
            for(auto &in : *b){ 
                instr_map[&in] = instr_num;
                instrs.push_back(&in);
                ++instr_num;
            }
        }
        errs() << "End trace printout \n";

        //Find the breadth-first topological ordering of the def-use chains within this superblock
        auto topsort_order = BF_ToplogicalSort(instr_map);

        //Print topological order
        errs() << "Topological order:\n";
        for(auto idx : topsort_order){
            auto in = instrs[idx];
            errs() << *in << "\n";
        }

        //Build vectorizable groups
        std::map<std::string, IsomorphicGroup*> vec_cands;
        IsomorphicGroup* currentGroup;
        for(auto idx : topsort_order){
            auto in = instrs[idx];
            //This is the "visit" function 
            if (isa<StoreInst>(in)){ //Don't try to hoist up through stores, so reset vectorization candidates
                //Process isomorphic groups
                vec_cands.clear();
            } else {
                std::string opName = in->getOpcodeName();
                if(vec_cands.find(opName) == vec_cands.end()){ //Start a new isomorphic group with a given op type
                    currentGroup = new IsomorphicGroup(in);
                    vec_cands.insert(std::pair<std::string, IsomorphicGroup*>(opName, currentGroup));
                } else {
                    currentGroup = vec_cands[opName];
                    currentGroup->insertIfIsomorphic(in);
                    if(currentGroup->size == ProfileSLP::SIMD_WIDTH){ //Upgrade to confirmed vectorization once we've reached SIMD width
                        errs() << "Found a vector of ops: " << opName << "\n";
                        printInstrGroup(currentGroup->instrs);
                        vec_confirmed.push_back(currentGroup->instrs);
                        vec_cands.erase(opName);
                        ++vectorizedCount;
                    }
                }
            }
        }
    }
    delete m_traces; //Free up memory
    return vec_confirmed;
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
    {"load", LOAD},
    {"store", STORE},
    {"alloca", MEM_OTHER},
    {"getelementptr", MEM_OTHER},
    {"fence", MEM_OTHER},
    {"cmpxchg", MEM_OTHER},
    {"atomicrmw", MEM_OTHER}
};