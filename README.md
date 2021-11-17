# eecs583-project

## Building and Running a Generic Pass
### LLVM Full Build
Prior to building individual passes, we want to complete an LLVM full build. Note that this build took roughly 20 minutes for me.
```
cd llvm-project/build
cmake -G CDT4\ -\ Ninja -DLLVM_TARGETS_TO_BUILD=host ../llvm/
ninja
```
Note: after the inital build, subsequent builds are very fast.

### Building Test Pass
Our generic pass lives in `llvm-project/llvm/lib/Transforms/TestPass`. To compile it (might have compiled in initial build):
```
cd llvm-project/build
ninja
```
A .so file will be made in build/lib for our pass.

### Running Test Pass
A simple test file lives under `llvm-project/testcases`. To generate the IR for this .c file:
```
cd llvm-project/testcases
clang -emit-llvm -S test1.c
```
A corresponding .ll file will have been created. Finally to run our LLVM generic pass on the test file:
```
opt -load ../build/lib/LLVMTestPass.so -TestPass < test1.ll > /dev/null

### Building and Running the Generic pass on eecs583 servers
If you're using the eecs583 servers, you may build and run the generic pass using the run.sh script:
```
cd llvm-project
./run.sh ${PASS_FOLDER} ${PASS_NAME} ${CPROG_NAME}
```
Where PASS_FOLDER is the folder to use in Transforms, and PASS_NAME is the name of the pass file. Make sure the pass .cpp file name, the registered pass name
(from RegisterPass), and the pass name from the bottom-level CMakeLists.txt file are the same.
Example:
```
cd llvm-project
./run.sh TestPass TestPass test1
```