#!/bin/bash
### run.sh

# Specify pass stuff
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
llvm-profdata merge -output=${1}.profdata default.profraw

# Apply Superblock generation followed by SLP Vectorization
# Add -time-passes to time the pass
./viz.sh ${1}
opt -pgo-instr-use -pgo-test-profile-file=${1}.profdata -load ./build/${SLP_PATH}/ProfileSLP.so -ProfileSLP -S ${1}.bc -o ${1}_SLP.bc
./viz ${1}_SLP

# Cleanup
rm ${1}.bc
rm ${1}.prof.bc
rm ${1}.prof
rm ${1}.profdata
rm default.profraw