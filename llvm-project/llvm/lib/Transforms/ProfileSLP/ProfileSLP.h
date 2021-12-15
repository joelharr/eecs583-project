#ifndef PROFILE_SLP_H
#define PROFILE_SLP_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/IR/ValueMap.h"
#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>

#define GET_SLP

using namespace llvm;

struct ProfileSLP : public FunctionPass {
    
    enum OpType { //Ordering in this enum specifies the stat's print order
        IALU,
        FALU,
        MEM,
        B_BRANCH,
        UB_BRANCH,
        OTHER,
        ENUM_END //Not an instruction type
    };
    static char ID;
    static std::map<std::string, ProfileSLP::OpType> opToInstr;
    static const int SIMD_WIDTH = 2; //2 is good for testing, maybe 4 for performance. Never choose 1

    ProfileSLP() : FunctionPass(ID) {}

    // keep track of unvisited nodes
    // std::unordered_set<BasicBlock *> visited;
    BranchProbability THRESHOLD;
    ValueToValueMapTy VMap;
    std::vector<std::vector<BasicBlock *>> m_traces;

    void getAnalysisUsage(AnalysisUsage &AU) const;
    bool runOnFunction(Function &F) override;

    //Functions for getting superblocks
    BasicBlock* getSeed(BlockFrequencyInfo &bfi, std::unordered_set<BasicBlock *> &unvisited);
    BasicBlock* best_successor(BasicBlock *source, BranchProbabilityInfo &bpi, SmallVector<std::pair<const BasicBlock *, const BasicBlock *> > &backedges, std::unordered_set<BasicBlock *> &unvisited);
    BasicBlock* best_predecessor(BasicBlock *dest, BranchProbabilityInfo &bpi, SmallVector<std::pair<const BasicBlock *, const BasicBlock *> > &backedges, std::unordered_set<BasicBlock *> &unvisited);
    BasicBlock* clone_bb(const BasicBlock *BB, Function *F);
    int in_trace(const BasicBlock *BB, int trace, std::vector<std::vector<BasicBlock *> > &traces);
    int get_inst_location(const BasicBlock *BB, const Instruction *find_in);

    bool getSuperblocks(Function &F);

    //Functions for performing SLP
    std::vector<int> BF_ToplogicalSort(const std::map<Instruction*, int> &instrs);
    bool getSLP(Function &F);
};

#endif
