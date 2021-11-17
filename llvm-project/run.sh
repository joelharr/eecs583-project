#!/bin/bash
### run.sh

# Specify pass and input program
PASS_FOLDER=${1}
PASS_NAME=${2}
CPROG_NAME=${3}
export PASS_PATH=llvm/lib/Transforms/${PASS_FOLDER}/ # CMakeLists.txt reads this

mkdir -p build/progs

#Build the pass
cd ./build
cmake ..
make -j2
cd ..

# Convert source code to bitcode (IR)
# This approach has an issue with -O2, so we are going to stick with default optimization level (-O0)
clang -emit-llvm -c ./testcases/${CPROG_NAME}.c -o ./build/progs/${CPROG_NAME}.bc

# Apply pass to bitcode (IR)
opt -load ./build/${PASS_PATH}/${PASS_NAME}.so -${PASS_NAME} < ./build/progs/${CPROG_NAME}.bc > /dev/null