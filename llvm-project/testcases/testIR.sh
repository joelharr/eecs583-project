#!/bin/bash
rm *.bc
rm *.ll
clang -emit-llvm -S -O -Xclang -disable-llvm-passes test2.c
../build/bin/opt -debug -slp-vectorizer -stats --debug-pass-manager -S test2.ll -o test2_opt.ll
diff test2_opt.ll test2.ll