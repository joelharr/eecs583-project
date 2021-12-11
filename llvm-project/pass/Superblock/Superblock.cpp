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
  std::vector<BasicBlock *> duplicates;
  ValueToValueMapTy VMap;

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

  // clones a bb
  BasicBlock *clone_bb(const BasicBlock *BB, Function *F) {
  	BasicBlock *NewBB = BasicBlock::Create(BB->getContext(), "", F);
	for (const Instruction &I : *BB) {
		Instruction *NewInst = I.clone();
		VMap[&I] = NewInst;
		NewBB->getInstList().push_back(NewInst);
	}
	errs() << "Cloned\n";
	VMap[BB] = NewBB;
	return NewBB;
  }

  // return index of bb in trace, or return -1 if not in it
  int in_trace(const BasicBlock *BB, int trace) {
  	for (int i = 0; i < traces[trace].size(); ++i) {
		if (traces[trace][i] == BB) {
			return i;
		}
	}
	return -1;
  }

  int get_inst_location(const BasicBlock *BB, const Instruction *find_in) {
  	int count = 0;
	for (auto &in: *BB) {
		if (&in == find_in) {
			return count;
		}
		count++;
	}
	return -1;
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
	
    errs() << "Side Entrances\n";

    // find side entrances for each trace
    for (int i = 0; i < traces.size(); ++i) {
	errs() << "Trace " << i << "\n";

	std::vector<bool> to_duplicate;
	std::vector<BasicBlock *> dups;
	for (int j = 0; j < traces[i].size(); ++j) {
		to_duplicate.push_back(false);
		dups.push_back(nullptr);
	}
    	for (int j = 1; j < traces[i].size(); ++j) {
		errs() << "BB num " << j << "\n";
		if (to_duplicate[j]) {
			BasicBlock *new_bb = clone_bb(traces[i][j], &F);
			dups[j] = new_bb;
			for (BasicBlock *pred : predecessors(traces[i][j])) {
				int location = in_trace(pred, i);

				// predecessor from outside the trace
				if (location == -1) {
					// get pred to branch to new_bb
					for (auto &in : *pred) {
						// for every inst, if it branches to traces[i][j], make it branch to new_bb instead
						if (llvm::isa <llvm::BranchInst> (&in)) {
							for (int k = 0; k < in.getNumSuccessors(); k++) {
								if (in.getSuccessor(k) == traces[i][j]) {
									in.setSuccessor(k, new_bb);
								}
							}
						}
					}
				}
				else if (dups[location] != nullptr) {
					// if pred is in the trace and has already been duplicated, update the dup[location] to branch to new_bb
					for (auto &in : *dups[location]) {
                                                // for every inst, if it branches to traces[i][j], make it branch to new_bb instead
                                                if (llvm::isa <llvm::BranchInst> (&in)) {
                                                        for (int k = 0; k < in.getNumSuccessors(); k++) {
                                                                if (in.getSuccessor(k) == traces[i][j]) {
                                                                        in.setSuccessor(k, new_bb);
                                                                }
                                                        }
						}
					}
				}
			}

			for (BasicBlock *succ : successors(traces[i][j])) {
				int location = in_trace(succ, i);
				if (location > -1) {
					to_duplicate[location] = true;
					if (dups[location] != nullptr) {
						// if it's already been duplicated, make new_bb branch to dups[location]
						for (auto &in : *new_bb) {
                                                	// for every inst, if it branches to traces[i][location], make it branch to dups[location] instead
                                                	if (llvm::isa <llvm::BranchInst> (&in)) {
                                                        	for (int k = 0; k < in.getNumSuccessors(); k++) {
                                                                	if (in.getSuccessor(k) == traces[i][location]) {
                                                                        	in.setSuccessor(k, dups[location]);
                                                                	}
                                                        	}
                                                	}
                                        	}
					}
				}
			}
		}
		else {
			bool will_be_duped = false;
			for (BasicBlock *pred : predecessors(traces[i][j])) {
				int location = in_trace(pred, i);

				// if it's from outside the trace
				if (location == -1) {
					will_be_duped = true;
					break;
				}
			}

			if (will_be_duped) {
				BasicBlock *new_bb = clone_bb(traces[i][j], &F);
                        	dups[j] = new_bb;

				for (BasicBlock *pred : predecessors(traces[i][j])) {
                                	int location = in_trace(pred, i);

                                	// predecessor from outside the trace
                                	if (location == -1) {
                                        	// get pred to branch to new_bb
                                        	for (auto &in : *pred) {
                                                	// for every inst, if it branches to traces[i][j], make it branch to new_bb instead
                                                	if (llvm::isa <llvm::BranchInst> (&in)) {
                                                        	for (int k = 0; k < in.getNumSuccessors(); k++) {
                                                                	if (in.getSuccessor(k) == traces[i][j]) {
                                                                        	in.setSuccessor(k, new_bb);
                                                                	}
                                                        	}
                                                	}
                                        	}
                                	}
                                	else if (dups[location] != nullptr) {
                                        	// if pred is in the trace and has already been duplicated, update the dup[location] to branch to new_bb
                                        	for (auto &in : *dups[location]) {
                                                	// for every inst, if it branches to traces[i][j], make it branch to new_bb instead
                                                	if (llvm::isa <llvm::BranchInst> (&in)) {
                                                        	for (int k = 0; k < in.getNumSuccessors(); k++) {
                                                                	if (in.getSuccessor(k) == traces[i][j]) {
                                                                        	in.setSuccessor(k, new_bb);
                                                                	}
                                                        	}
                                                	}
                                        	}
                                	}
                       		}

				for (BasicBlock *succ : successors(traces[i][j])) {
                                	int location = in_trace(succ, i);
                                	if (location > -1) {
                                        	to_duplicate[location] = true;
                                        	if (dups[location] != nullptr) {
                                                	// if it's already been duplicated, make new_bb branch to dups[location]
                                        		for (auto &in : *new_bb) {
                                                        	// for every inst, if it branches to traces[i][location], make it branch to dups[location] instead
                                                        	if (llvm::isa <llvm::BranchInst> (&in)) {
                                                                	for (int k = 0; k < in.getNumSuccessors(); k++) {
                                                                        	if (in.getSuccessor(k) == traces[i][location]) {
                                                                                	in.setSuccessor(k, dups[location]);
                                                                        	}
                                                                	}
                                                        	}
                                                	}
						}
                                	}
                        	}
			}
		}
	}
	// fix broken instructions
	for (int j = 0; j < dups.size(); ++j) {
		if (dups[j] == nullptr) {
			continue;
		}
		for (auto &in : *dups[j]) {
			for (int k = 0; k < in.getNumOperands(); k++) {
				if (VMap.find(in.getOperand(k)) != VMap.end()) {
					// if operand can be mapped, set it to the mapped value
					in.setOperand(k, VMap[in.getOperand(k)]);
				}
			}
		}
	}

	// fix phi nodes
	for (auto &bb : F) {
		int location = in_trace(&bb, i);
		if (location != -1) {
			// doesn't matter if phi is in the original trace blocks
			continue;
		}
		for (auto &in : bb) {
			if (PHINode *p = dyn_cast <PHINode>(&in)) {
				for (int k = 0; k < p->getNumIncomingValues(); k++) {
					int bb_loc = in_trace(p->getIncomingBlock(k), i);
					if (bb_loc > -1 && VMap.find(p->getIncomingBlock(k)) != VMap.end()) {
						p->addIncoming(VMap[p->getIncomingValue(k)], dups[bb_loc]);
					}
				}
			}
		}
	}
    }

    return false;
  }
}; // end of struct Hello
}  // end of anonymous namespace

char SB::ID = 0;
static RegisterPass<SB> X("Superblock", "Hello World Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
