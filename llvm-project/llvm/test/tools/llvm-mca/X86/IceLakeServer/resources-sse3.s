# NOTE: Assertions have been autogenerated by utils/update_mca_test_checks.py
# RUN: llvm-mca -mtriple=x86_64-unknown-unknown -mcpu=icelake-server -instruction-tables < %s | FileCheck %s

addsubpd  %xmm0, %xmm2
addsubpd  (%rax),  %xmm2

addsubps  %xmm0, %xmm2
addsubps  (%rax), %xmm2

haddpd    %xmm0, %xmm2
haddpd    (%rax), %xmm2

haddps    %xmm0, %xmm2
haddps    (%rax), %xmm2

hsubpd    %xmm0, %xmm2
hsubpd    (%rax), %xmm2

hsubps    %xmm0, %xmm2
hsubps    (%rax), %xmm2

lddqu     (%rax), %xmm2

monitor

movddup   %xmm0, %xmm2
movddup   (%rax), %xmm2

movshdup  %xmm0, %xmm2
movshdup  (%rax), %xmm2

movsldup  %xmm0, %xmm2
movsldup  (%rax), %xmm2

mwait

# CHECK:      Instruction Info:
# CHECK-NEXT: [1]: #uOps
# CHECK-NEXT: [2]: Latency
# CHECK-NEXT: [3]: RThroughput
# CHECK-NEXT: [4]: MayLoad
# CHECK-NEXT: [5]: MayStore
# CHECK-NEXT: [6]: HasSideEffects (U)

# CHECK:      [1]    [2]    [3]    [4]    [5]    [6]    Instructions:
# CHECK-NEXT:  1      4     0.50                        addsubpd	%xmm0, %xmm2
# CHECK-NEXT:  2      10    0.50    *                   addsubpd	(%rax), %xmm2
# CHECK-NEXT:  1      4     0.50                        addsubps	%xmm0, %xmm2
# CHECK-NEXT:  2      10    0.50    *                   addsubps	(%rax), %xmm2
# CHECK-NEXT:  3      6     2.00                        haddpd	%xmm0, %xmm2
# CHECK-NEXT:  4      12    2.00    *                   haddpd	(%rax), %xmm2
# CHECK-NEXT:  3      6     2.00                        haddps	%xmm0, %xmm2
# CHECK-NEXT:  4      12    2.00    *                   haddps	(%rax), %xmm2
# CHECK-NEXT:  3      6     2.00                        hsubpd	%xmm0, %xmm2
# CHECK-NEXT:  4      12    2.00    *                   hsubpd	(%rax), %xmm2
# CHECK-NEXT:  3      6     2.00                        hsubps	%xmm0, %xmm2
# CHECK-NEXT:  4      12    2.00    *                   hsubps	(%rax), %xmm2
# CHECK-NEXT:  1      6     0.50    *                   lddqu	(%rax), %xmm2
# CHECK-NEXT:  1      100   0.25                  U     monitor
# CHECK-NEXT:  1      1     1.00                        movddup	%xmm0, %xmm2
# CHECK-NEXT:  1      5     0.50    *                   movddup	(%rax), %xmm2
# CHECK-NEXT:  1      1     1.00                        movshdup	%xmm0, %xmm2
# CHECK-NEXT:  1      6     0.50    *                   movshdup	(%rax), %xmm2
# CHECK-NEXT:  1      1     1.00                        movsldup	%xmm0, %xmm2
# CHECK-NEXT:  1      6     0.50    *                   movsldup	(%rax), %xmm2
# CHECK-NEXT:  10     20    2.50    *      *      U     mwait

# CHECK:      Resources:
# CHECK-NEXT: [0]   - ICXDivider
# CHECK-NEXT: [1]   - ICXFPDivider
# CHECK-NEXT: [2]   - ICXPort0
# CHECK-NEXT: [3]   - ICXPort1
# CHECK-NEXT: [4]   - ICXPort2
# CHECK-NEXT: [5]   - ICXPort3
# CHECK-NEXT: [6]   - ICXPort4
# CHECK-NEXT: [7]   - ICXPort5
# CHECK-NEXT: [8]   - ICXPort6
# CHECK-NEXT: [9]   - ICXPort7
# CHECK-NEXT: [10]  - ICXPort8
# CHECK-NEXT: [11]  - ICXPort9

# CHECK:      Resource pressure per iteration:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    [10]   [11]
# CHECK-NEXT:  -      -     6.67   6.67   5.00   5.00    -     24.67  4.00    -      -      -

# CHECK:      Resource pressure by instruction:
# CHECK-NEXT: [0]    [1]    [2]    [3]    [4]    [5]    [6]    [7]    [8]    [9]    [10]   [11]   Instructions:
# CHECK-NEXT:  -      -     0.50   0.50    -      -      -      -      -      -      -      -     addsubpd	%xmm0, %xmm2
# CHECK-NEXT:  -      -     0.50   0.50   0.50   0.50    -      -      -      -      -      -     addsubpd	(%rax), %xmm2
# CHECK-NEXT:  -      -     0.50   0.50    -      -      -      -      -      -      -      -     addsubps	%xmm0, %xmm2
# CHECK-NEXT:  -      -     0.50   0.50   0.50   0.50    -      -      -      -      -      -     addsubps	(%rax), %xmm2
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     2.33    -      -      -      -     haddpd	%xmm0, %xmm2
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     2.33    -      -      -      -     haddpd	(%rax), %xmm2
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     2.33    -      -      -      -     haddps	%xmm0, %xmm2
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     2.33    -      -      -      -     haddps	(%rax), %xmm2
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     2.33    -      -      -      -     hsubpd	%xmm0, %xmm2
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     2.33    -      -      -      -     hsubpd	(%rax), %xmm2
# CHECK-NEXT:  -      -     0.33   0.33    -      -      -     2.33    -      -      -      -     hsubps	%xmm0, %xmm2
# CHECK-NEXT:  -      -     0.33   0.33   0.50   0.50    -     2.33    -      -      -      -     hsubps	(%rax), %xmm2
# CHECK-NEXT:  -      -      -      -     0.50   0.50    -      -      -      -      -      -     lddqu	(%rax), %xmm2
# CHECK-NEXT:  -      -     0.25   0.25    -      -      -     0.25   0.25    -      -      -     monitor
# CHECK-NEXT:  -      -      -      -      -      -      -     1.00    -      -      -      -     movddup	%xmm0, %xmm2
# CHECK-NEXT:  -      -      -      -     0.50   0.50    -      -      -      -      -      -     movddup	(%rax), %xmm2
# CHECK-NEXT:  -      -      -      -      -      -      -     1.00    -      -      -      -     movshdup	%xmm0, %xmm2
# CHECK-NEXT:  -      -      -      -     0.50   0.50    -      -      -      -      -      -     movshdup	(%rax), %xmm2
# CHECK-NEXT:  -      -      -      -      -      -      -     1.00    -      -      -      -     movsldup	%xmm0, %xmm2
# CHECK-NEXT:  -      -      -      -     0.50   0.50    -      -      -      -      -      -     movsldup	(%rax), %xmm2
# CHECK-NEXT:  -      -     1.75   1.75    -      -      -     2.75   3.75    -      -      -     mwait