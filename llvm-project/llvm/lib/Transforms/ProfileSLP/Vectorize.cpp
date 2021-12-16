#include "ProfileSLP.h"
#include "llvm/IR/IRBuilder.h"

Instruction* ProfileSLP::vectorizeWrapper(std::vector<std::vector<Instruction*>> &instr_groups, LLVMContext& context) {
    for (int i = 0; i < instr_groups.size(); i++) {
        vectorize(instr_groups[i], context);
    }
    return nullptr;
}

Instruction* ProfileSLP::vectorize(std::vector<Instruction*> instr, LLVMContext& context) {

	Instruction *firstI = *(instr.begin());
    Instruction *lastI = *(instr.begin()+(instr.size()-1));
    BasicBlock *parent = firstI->getParent();
    // auto &context = parent->getContext();

    IRBuilder<> build(context);
    unsigned int width = instr.size();
    auto *Ty = firstI->getOperand(0)->getType();
    auto *vType = ConstantInt::get(Ty, 0);

    Value *inVec1 = build.CreateVectorSplat(width, vType);
    Value *inVec2 = build.CreateVectorSplat(width, vType);

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

    Value *outVec;
    unsigned op = instr[0]->getOpcode();
    switch(op) {
        case Instruction::Add :
            outVec = build.CreateAdd(next1, next2);
            break;
        case Instruction::Sub :
            outVec = build.CreateSub(next1, next2);
            break;
        case Instruction::Mul :
            outVec = build.CreateMul(next1, next2);
            break;
        case Instruction::UDiv :
            outVec = build.CreateUDiv(next1, next2);
            break;
        case Instruction::SDiv :
            outVec = build.CreateSDiv(next1, next2);
            break;
        case Instruction::URem :
            outVec = build.CreateURem(next1, next2);
            break;
        case Instruction::Shl :
            outVec = build.CreateShl(next1, next2);
            break;
        case Instruction::LShr :
            outVec = build.CreateLShr(next1, next2);
            break;
        case Instruction::AShr :
            outVec = build.CreateAShr(next1, next2);
            break;
        case Instruction::And :
            outVec = build.CreateAnd(next1, next2);
            break;
        case Instruction::Or :
            outVec = build.CreateOr(next1, next2);
            break;
        case Instruction::Xor :
            outVec = build.CreateXor(next1, next2);
            break;
        case Instruction::ICmp :
            outVec = build.CreateICmpEQ(next1, next2);
            break;
        case Instruction::SRem :
            outVec = build.CreateSRem(next1, next2);
            break;

        default : 
            errs() << "Type Not Supported\n";
    }

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