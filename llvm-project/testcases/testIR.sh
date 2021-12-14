#!/bin/bash
rm *.bc
rm *.ll
clang -std=c++17 -mavx -emit-llvm -S -O -Xclang -disable-llvm-passes vectorizable.cpp
opt -debug -slp-vectorizer -stats --debug-pass=Structure -S vectorizable.ll -o vectorizable_opt.ll
diff vectorizable_opt.ll vectorizable.ll