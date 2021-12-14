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

#define GET_SLP

using namespace llvm;
struct ProfileSLP : public FunctionPass {
    
    static char ID;
    ProfileSLP() : FunctionPass(ID) {}

    // keep track of unvisited nodes
    // std::unordered_set<BasicBlock *> visited;
    std::unordered_set<BasicBlock *> unvisited;
    std::vector<std::vector<BasicBlock *> > traces;
    BranchProbability THRESHOLD;
    std::vector<BasicBlock *> duplicates;
    ValueToValueMapTy VMap;

    void getAnalysisUsage(AnalysisUsage &AU) const;
    bool runOnFunction(Function &F) override;

    BasicBlock* getSeed(BlockFrequencyInfo &bfi);
    BasicBlock* best_successor(BasicBlock *source, BranchProbabilityInfo &bpi, SmallVector<std::pair<const BasicBlock *, const BasicBlock *> > &backedges);
    BasicBlock* best_predecessor(BasicBlock *dest, BranchProbabilityInfo &bpi, SmallVector<std::pair<const BasicBlock *, const BasicBlock *> > &backedges);
    BasicBlock* clone_bb(const BasicBlock *BB, Function *F);
    int in_trace(const BasicBlock *BB, int trace);
    int get_inst_location(const BasicBlock *BB, const Instruction *find_in);

    bool getSuperblocks(Function &F);
    bool getSLP(Function &F);
};

#endif