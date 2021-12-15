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

    // input vectors
    Value *inVec1 = build.CreateVectorSplat(width, ConstantInt::get(eltTy, 0));
    Value *inVec2 = build.CreateVectorSplat(width, ConstantInt::get(eltTy, 0));

    Value *next1 = inVec1;
    Value *next2 = inVec2;
    build.SetInsertPoint(firstI);

    Value *lastInst = firstI;

    // load all values in vectors
    for (int i = 0; i < width; i++) {
    	errs() << instr[i]->getOperand(0) << " " << instr[i]->getOperand(1) << '\n';
      // if (auto *loadInst = dyn_cast<Instruction>(instr[i]->getOperand(0))) {
        next1 = build.CreateInsertElement(next1, dyn_cast<ConstantInt>(instr[i]->getOperand(0)), build.getInt64(i));
      // } else {
      	// next1 = InsertElementInst::Create(next1, instr[i]->getOperand(0), build.getInt64(i), "", );
      // }
      // if (auto *loadInst = dyn_cast<Instruction>(instr[i]->getOperand(1))) {
        next2 = build.CreateInsertElement(next2, instr[i]->getOperand(1), build.getInt64(i));
      // }
    }

    // perform add on vectors
    // Value* outVec = build.CreateAdd(next1, next2);
    // unpack
    // Value *out = build.CreateExtractElement(outVec, build.getInt64(0));

    return nullptr;
}