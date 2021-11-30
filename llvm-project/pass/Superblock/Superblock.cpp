#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/CFG.h"
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace llvm;

namespace {
struct SB : public FunctionPass {
  static char ID;
  SB() : FunctionPass(ID) {}

  // keep track of unvisited nodes
  // std::unordered_set<BasicBlock *> visited;
  std::unordered_set<BasicBlock *> unvisited;
  std::vector<std::vector<BasicBlock *> > traces;
  BranchProbability THRESHOLD;

  // Specify the list of analysis passes that will be used inside your pass.
  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<BlockFrequencyInfoWrapperPass>();
    AU.addRequired<BranchProbabilityInfoWrapperPass>();
    // AU.addRequired<CFG>();
  }

  // returns highest executed branch from the unvisited set
  // also removes selected block from unvisited and adds to visited
  BasicBlock *getSeed(BlockFrequencyInfo &bfi) {
  	uint64_t max_count;
  	BasicBlock *max_bb = nullptr;
  	for (BasicBlock *bb : unvisited) {
  		if (!max_bb || bfi.getBlockProfileCount(bb).getValue() > max_count) {
  			max_bb = bb;
  			max_count = bfi.getBlockProfileCount(bb).getValue();
  		}
  	}
  	unvisited.erase(unvisited.find(max_bb));
  	return max_bb;
  }

  // assumes threshold is above 50% 
  BasicBlock *best_successor(BasicBlock *source, BranchProbabilityInfo &bpi, SmallVector<std::pair<const BasicBlock *, const BasicBlock *> > &backedges) {
  	for (BasicBlock *dest : successors(source)) {
      if (bpi.getEdgeProbability(source, dest) >= THRESHOLD) {
      	if (unvisited.find(dest) == unvisited.end()){
      		// already visited
      		return nullptr;
      	}
      	for (auto &p : backedges) {
      	  if (source == p.first && dest == p.second) {
      	    // is a backedge
            return nullptr;
      	  }
      	}
      	// add this successor to trace
      	return dest;
      }
  	}
  	return nullptr;
  }

  // different enough from successor to just make a new function
  // note source/dest changes meaning because edges are coming towards this basic block now
  BasicBlock *best_predecessor(BasicBlock *dest, BranchProbabilityInfo &bpi, SmallVector<std::pair<const BasicBlock *, const BasicBlock *> > &backedges) {
  	for (BasicBlock *source : predecessors(dest)) {
      if (bpi.getEdgeProbability(source, dest) >= THRESHOLD) {
      	if (unvisited.find(source) == unvisited.end()){
      		// already visited
      		return nullptr;
      	}
      	for (auto &p : backedges) {
      	  if (source == p.first && dest == p.second) {
      	    // is a backedge
            return nullptr;
      	  }
      	}
      	// add this successor to trace
      	return source;
      }
  	}
  	return nullptr;
  }

  bool runOnFunction(Function &F) override {
  	THRESHOLD = BranchProbability(8,10);
    BranchProbabilityInfo &bpi = getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI();
    BlockFrequencyInfo &bfi = getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();

    SmallVector<std::pair<const BasicBlock *, const BasicBlock *> > backedges;
    FindFunctionBackedges(F, backedges);

    // initialize the unvisited list
    for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    	unvisited.insert(&(*bb));
    }

    errs() << "Number of Basic Blocks:" << unvisited.size() << '\n';

    int trace = 0;
    while (!unvisited.empty()) {
    	BasicBlock* seed = getSeed(bfi);
    	traces.push_back({seed});
    	BasicBlock *current = seed;

    	// grow trace forward
    	while(true) {
    		BasicBlock *next = best_successor(current, bpi, backedges);
    		if (!next) {
    			// stop expanding forward
    			break;
    		}
    		traces[trace].push_back(next);
    		unvisited.erase(unvisited.find(next));
    		current = next;
    	}

    	// grow trace backward
    	current = seed;
    	while(true) {
    		BasicBlock *next = best_predecessor(current, bpi, backedges);
    		if (!next) {
    			// stop expanding forward
    			break;
    		}
    		// keeps topological order
    		traces[trace].insert(traces[trace].begin(), next);
    		unvisited.erase(unvisited.find(next));
    		current = next;
    	}
    	errs() << "Trace Size:" << traces[trace].size() << '\n';
    	trace++;
    }
    return false;
  }
}; // end of struct Hello
}  // end of anonymous namespace

char SB::ID = 0;
static RegisterPass<SB> X("Superblock", "Hello World Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);