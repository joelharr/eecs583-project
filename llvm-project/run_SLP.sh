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
clang -O0 -Xclang -disable-O0-optnone ./testcases/${1}.c -emit-llvm -c -o ${1}.bc

USE_UNROLL=True #Disable unroll if desired
if [[ $USE_UNROLL == True ]]; then
    ./viz.sh ${1}
    opt -mem2reg -simplifycfg -indvars -loop-unroll -unroll-count=4 -simplifycfg ${1}.bc -o ${1}_unrolled.bc
    #opt -mem2reg -loop-unroll -unroll-count=4 ${1}.bc -o ${1}_unrolled.bc
    SLP_BC_IN=${1}_unrolled
else
    SLP_BC_IN=${1}
fi

# Apply loop unrolling
# Add -time-passes to time the pass
./viz.sh ${SLP_BC_IN}

# Instrument profiler
opt -pgo-instr-gen -instrprof ${SLP_BC_IN}.bc -o ${1}.prof.bc
# Generate binary executable with profiler embedded
clang -fprofile-instr-generate ${1}.prof.bc -o ${1}.prof
# Collect profiling data
./${1}.prof > /dev/null # ${INPUT}
# Translate raw profiling data into LLVM data format
llvm-profdata merge -output=${1}_SLP.profdata default.profraw

# run SLP pass on unrolled code
opt -pgo-instr-use -pgo-test-profile-file=${1}_SLP.profdata -load ./build/${SLP_PATH}/ProfileSLP.so -ProfileSLP ${SLP_BC_IN}.bc -o ${1}_SLP.bc
./viz.sh ${1}_SLP

# Generate binary excutable before SLP: Unoptimzied code
clang ${1}.bc -o ${1}_no_SLP
# Generate binary executable after SLP: Optimized code
clang ${1}_SLP.bc -o ${1}_SLP

# Generate x86 assembly
clang ${1}_SLP.bc -S

./${1}_SLP > slp_output
./${1}_no_SLP > slp_correct

if [ "$(diff slp_output slp_correct)" != "" ]; then
    echo -e ">> FAIL\n"
    diff slp_output slp_correct > FAIL.txt
else
    echo -e ">> PASS\n"
fi

# Cleanup
rm ${1}.bc
rm ${1}_unrolled.bc
rm ${1}_SLP.bc
rm ${1}.prof.bc
rm ${1}.prof
rm ${1}_SLP.profdata
rm default.profraw
rm slp_output
rm slp_correct
rm ${1}_SLP
rm ${1}_no_SLP
