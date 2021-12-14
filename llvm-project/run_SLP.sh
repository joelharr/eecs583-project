#!/bin/bash
### run.sh

# Specify pass stuff
export SUPERBLOCK_PATH=llvm/lib/Transforms/Superblock/ # CMakeLists.txt reads this
export SLP_PATH=llvm/lib/Transforms/ProfileSLP/ # CMakeLists.txt reads this

mkdir -p build/

#Build the passes (Superblock and SLP)
cd ./build
cmake ..
make -j2
cd ..

# Convert source code to bitcode (IR)
# This approach has an issue with -O2, so we are going to stick with default optimization level (-O0)
clang -emit-llvm -c ./testcases/${1}.c -o ${1}.bc

# Instrument profiler
opt -pgo-instr-gen -instrprof ${1}.bc -o ${1}.prof.bc
# Generate binary executable with profiler embedded
clang -fprofile-instr-generate ${1}.prof.bc -o ${1}.prof
# Collect profiling data
./${1}.prof # ${INPUT}
# Translate raw profiling data into LLVM data format
llvm-profdata merge -output=pgo.profdata default.profraw

# Apply Superblock generation followed by SLP Vectorization
# Add -time-passes to time the pass
opt -pgo-instr-use -pgo-test-profile-file=pgo.profdata -load ./build/${SUPERBLOCK_PATH}/Superblock.so -Superblock -S ${1}.bc -o ${1}_superblock.bc
# opt -pgo-instr-use -pgo-test-profile-file=pgo.profdata -load ./build/${SLP_PATH}/ProfileSLP.so -ProfileSLP -S ${1}_superblock.bc -o ${1}_SLP.bc

# Cleanup
rm ${1}.bc
rm ${1}.prof.bc
rm ${1}.prof
rm pgo.profdata
rm default.profraw