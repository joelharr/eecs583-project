#include "ProfileSLP.h"
#include "llvm/IR/IRBuilder.h"

Instruction* ProfileSLP::vectorize(std::vector<Instruction*> instr, LLVMContext& context) {
	Instruction *firstI = *(instr.begin());
    Instruction *lastI = *(instr.begin()+(instr.size()-1));
    BasicBlock *parent = firstI->getParent();
    // auto &context = parent->getContext();

    IRBuilder<> build(context);
    unsigned int width = instr.size();
    auto *eltTy = firstI->getOperand(0)->getType();
    auto *Ty = IntegerType::get(context, 32);

    // input vectors
    Value *inVec1 = build.CreateVectorSplat(width, ConstantInt::get(Ty, 0));
    Value *inVec2 = build.CreateVectorSplat(width, ConstantInt::get(Ty, 0));

    Value *next1 = inVec1;
    Value *next2 = inVec2;
    build.SetInsertPoint(firstI);

    // load all values in vectors
    for (int i = 0; i < width; i++) {
    	if (auto *loadInst = dyn_cast<Instruction>(instr[i]->getOperand(0))) {
    		next1 = build.CreateInsertElement(next1, loadInst, build.getInt64(i));
      	} else {
      		auto *alloc = build.CreateAlloca(Ty, 0, nullptr, "");
      		build.CreateStore(instr[i]->getOperand(0), alloc);
      		auto *load = build.CreateLoad(Ty, alloc);
      		next1 = build.CreateInsertElement(next1, load, build.getInt64(i));
      	}
      	if (auto *loadInst = dyn_cast<Instruction>(instr[i]->getOperand(1))) {
    		next2 = build.CreateInsertElement(next2, loadInst, build.getInt64(i));
      	} else {
      		auto *alloc = build.CreateAlloca(Ty, 0, nullptr, "");
      		build.CreateStore(instr[i]->getOperand(1), alloc);
      		auto *load = build.CreateLoad(Ty, alloc);
      		next2 = build.CreateInsertElement(next2, load, build.getInt64(i));
      	}
    }

    // perform add on vectors
    Value* outVec = build.CreateAdd(next1, next2);
    // unpack
    for (int i = 0; i < width; i++) {
        Value *out = build.CreateExtractElement(outVec, build.getInt64(i));
        instr[i]->replaceAllUsesWith(out);
    }
    for (int i = 0; i < width; i++) {
        instr[i]->eraseFromParent();
    }

    return nullptr;
}