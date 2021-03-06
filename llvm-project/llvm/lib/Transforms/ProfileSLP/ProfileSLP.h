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
#include "llvm/IR/IRBuilder.h"
#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>

using namespace llvm;

struct ProfileSLP : public FunctionPass {
    
    enum OpType { //Ordering in this enum specifies the stat's print order
        ENUM_START,
        IALU,
        FALU,
        LOAD,
        STORE,
        MEM_OTHER,
        BRANCH,
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
    std::vector<std::vector<BasicBlock *>>* m_traces;

    void printInstrGroup(std::vector<Instruction*> group);
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

    //Functions for identifying vectorizable groups
    std::vector<int> BF_ToplogicalSort(std::map<Instruction*, int> &instr_map, std::vector<Instruction*> &instrs);
    std::vector<std::vector<Instruction*>> getSLP(std::vector<std::vector<Instruction*>>* sortedOrders);
    template <class T, class I> void addIfInMap(T item, std::map<T, I>* map, std::vector<I>* vec);

    //Functions for hoisting functions into their vectorizable groups
    bool reorder(std::vector<std::vector<Instruction*>>* SLP_vecs, std::vector<std::vector<Instruction*>>* sortedOrders_p);

    //Function for emitting Vector IR
    Instruction* vectorize(std::vector<Instruction*> instr, LLVMContext& context);
    Instruction* vectorizeWrapper(std::vector<std::vector<Instruction*>> &instr_groups, LLVMContext& context);
};

#endif
