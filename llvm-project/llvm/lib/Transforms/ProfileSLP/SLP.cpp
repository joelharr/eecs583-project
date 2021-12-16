#include "ProfileSLP.h"

using namespace llvm;

//#define ALLOW_LOADS
//#define ALLOW_FP

struct IsomorphicGroup {
    bool validOp;
    int size;
    std::string opName;
    ProfileSLP::OpType opType;
    std::vector<Instruction*> instrs;

    IsomorphicGroup(Instruction* instr){
        opName = instr->getOpcodeName();
        opType = ProfileSLP::opToInstr[opName];
        validOp = (opType == ProfileSLP::OpType::IALU)
                  #ifdef ALLOW_FP 
                    || (opType == ProfileSLP::OpType::FALU); 
                  #else 
                    ; 
                  #endif
                  #ifdef ALLOW_LOADS 
                    || (opType == ProfileSLP::OpType::LOAD); 
                  #else 
                    ; 
                  #endif
        if(validOp){
            instrs.push_back(instr);
            size = 1;
        } else {
            size = 0;
        }
    }

    bool isIsomorphicToGroup(Instruction* newInstr){
        std::string newOpName = newInstr->getOpcodeName();
        return validOp && (opName == newOpName);
    }

    void insertIfIsomorphic(Instruction* newInstr){
        if (isIsomorphicToGroup(newInstr)){
            instrs.push_back(newInstr);
            ++size;
        }
    }
};

template <class T, class I> 
void ProfileSLP::addIfInMap(T item, std::map<T, I>* map, std::vector<I>* vec){
    auto found = map->find(item);
    if(found != map->end()){
        vec->push_back(found->second);
    }
}

//Breadth-first toplogical sort
std::vector<int> ProfileSLP::BF_ToplogicalSort(
    std::map<Instruction*, int> &instr_map, 
    std::vector<Instruction*> &instrs
){
    //Construct a DAG for the program dependencies
    errs() << "Building DAG \n";
    int nInstrs = instrs.size();
    std::vector<std::vector<int>> DAG(nInstrs);
    Instruction* lastBranch = nullptr;
    std::vector<Instruction*> prevStores;
    std::vector<Instruction*> prevLoads;
    std::vector<Value*> prevDests;
    for(auto &from : instrs){
        auto def = instr_map.find(from);

        //Def-Use (data)
        for(User *U : from->users()){
            if (Instruction *inst = dyn_cast<Instruction>(U)) {
                if(!isa<PHINode>(inst)){ //Phi nodes aren't users in traces UNLESS they're circular dependencies
                    addIfInMap<Instruction*, int>(inst, &instr_map, &DAG[def->second]);
                }
            }
        }

        //Control dependencies
        //Make branches depend on the completion of all instructions before them in program order
        if(isa<BranchInst>(from)){
            if(lastBranch != nullptr){
                auto br = instr_map.find(lastBranch);
                DAG[br->second].push_back(def->second);
            }
            lastBranch = from;
        } else {
            //Store-->Load dependencies
            //Make loads dependent on all previous stores and vice versa
            if(isa<LoadInst>(from)){
                for(auto prevStore : prevStores){
                    auto st = instr_map.find(prevStore);
                    DAG[st->second].push_back(def->second);
                }
                prevLoads.push_back(from);
            } else if(isa<StoreInst>(from)){
                for(auto prevLoad : prevLoads){
                    auto ld = instr_map.find(prevLoad);
                    DAG[ld->second].push_back(def->second);
                }
                prevStores.push_back(from);
            }

            //All non-branch instructions should execute before their closest branch
            auto closestBranch = from->getParent()->getTerminator();
            addIfInMap<Instruction*, int>(closestBranch, &instr_map, &DAG[def->second]);
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

    if(q.size() == 0){ //Check for circular dependence
        errs() << "FOUND CIRCULAR DEPENDENCE \n";
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

std::vector<std::vector<Instruction*>> ProfileSLP::getSLP(Function &F){
    errs() << "Running getSLP... \n";
    //Iterate through all traces
    std::vector<std::vector<BasicBlock *>> traces = *m_traces;
    std::vector<std::vector<Instruction*>> vec_confirmed; //Move isomorphic groups here once they're full
    int vectorizedCount = 0;
    for(int i = 0; i < traces.size(); ++i){
        errs() << "SLP Trace " << i << "\n";

        //Extract all instructions
        std::map<Instruction*, int> instrMap;
        std::vector<Instruction*> instrs;
        int instr_num = 0;
        for(BasicBlock *b : traces[i]){
            errs() << "BB: " << *b << "\n";
            for(auto &in : *b){ 
                instrMap[&in] = instr_num;
                instrs.push_back(&in);
                ++instr_num;
            }
        }
        errs() << "End trace printout \n";

        //Find the breadth-first topological ordering of the def-use chains within this superblock
        auto topsort_order = BF_ToplogicalSort(instrMap, instrs);

        //Build vectorizable groups
        //Store candidates
        std::map<std::string, IsomorphicGroup*> vec_cands;
        IsomorphicGroup* currentGroup;
        //Store info about current layer
        std::map<Instruction*, int> instrLayers;
        std::unordered_set<Value*> layerDests;
        int currentLayer = 0;
        for(auto idx : topsort_order){
            auto in = instrs[idx];

            //Figure out what layer in the dependency DAG we're in. Only vectorize things in the same layer
            if(!isa<StoreInst>(in)){
                int numOps = in->getNumOperands();
                for(int op = 0; op < numOps; ++op){
                    if(layerDests.find(in->getOperand(op)) != layerDests.end()){
                        ++currentLayer;
                        //Now we're on a new layer, so we can't vectorize with previous entries
                        layerDests.clear();
                        vec_cands.clear(); //Need to reclaim memory here too
                        break;
                    }
                }
            }
            layerDests.insert(in);
            instrLayers.insert(std::pair<Instruction*, int>(in, currentLayer));

            std::string opName = in->getOpcodeName();
            if(vec_cands.find(opName) == vec_cands.end()){ //Start a new isomorphic group with a given op type
                currentGroup = new IsomorphicGroup(in);
                vec_cands.insert(std::pair<std::string, IsomorphicGroup*>(opName, currentGroup));
            } else {
                currentGroup = vec_cands[opName];
                currentGroup->insertIfIsomorphic(in);
                if(currentGroup->size == ProfileSLP::SIMD_WIDTH){ //Upgrade to confirmed vectorization once we've reached SIMD width
                    //errs() << "Found a vector of ops: " << opName << "\n";
                    //printInstrGroup(currentGroup->instrs);
                    vec_confirmed.push_back(currentGroup->instrs);
                    vec_cands.erase(opName);
                    ++vectorizedCount;
                }
            }
        }

        //Print topological order
        errs() << "Topological order:\n";
        for(auto idx : topsort_order){
            auto in = instrs[idx];
            errs() << "Layer: " << instrLayers.find(in)->second << " Instr: " << *in << "\n";
        }
    }

    errs() << "SLP Vectorization Groups: \n"; 
    for(auto vec : vec_confirmed){
        errs() << "Group: \n";
        printInstrGroup(vec);
    }

    delete m_traces; //Free up memory
    return vec_confirmed;
}

std::map<std::string, ProfileSLP::OpType> ProfileSLP::opToInstr = {
    // Branch Instructions
    {"br", BRANCH}, // Start as unbiased, change to biased
    {"switch", BRANCH}, 
    {"indirectbr", BRANCH},

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