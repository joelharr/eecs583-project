	.text
	.file	"simple_dependency.c"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$1, -4(%rbp)
	movd	-4(%rbp), %xmm1                 # xmm1 = mem[0],zero,zero,zero
	movl	$2, -8(%rbp)
	movd	-8(%rbp), %xmm3                 # xmm3 = mem[0],zero,zero,zero
	movl	$3, -12(%rbp)
	movd	-12(%rbp), %xmm0                # xmm0 = mem[0],zero,zero,zero
	punpcklqdq	%xmm1, %xmm0            # xmm0 = xmm0[0],xmm1[0]
	xorps	%xmm2, %xmm2
	shufps	$66, %xmm2, %xmm0               # xmm0 = xmm0[2,0],xmm2[0,1]
	movl	$4, -16(%rbp)
	movd	-16(%rbp), %xmm1                # xmm1 = mem[0],zero,zero,zero
	punpcklqdq	%xmm3, %xmm1            # xmm1 = xmm1[0],xmm3[0]
	shufps	$66, %xmm2, %xmm1               # xmm1 = xmm1[2,0],xmm2[0,1]
	paddd	%xmm1, %xmm0
	movd	%xmm0, %esi
	pshufd	$85, %xmm0, %xmm1               # xmm1 = xmm0[1,1,1,1]
	movd	%xmm1, %ecx
	movl	$2, -20(%rbp)
	movd	-20(%rbp), %xmm3                # xmm3 = mem[0],zero,zero,zero
	movl	$4, -24(%rbp)
	movd	-24(%rbp), %xmm1                # xmm1 = mem[0],zero,zero,zero
	punpcklqdq	%xmm3, %xmm1            # xmm1 = xmm1[0],xmm3[0]
	shufps	$66, %xmm2, %xmm1               # xmm1 = xmm1[2,0],xmm2[0,1]
	paddd	%xmm1, %xmm0
	movd	%xmm0, %edx
	pshufd	$85, %xmm0, %xmm0               # xmm0 = xmm0[1,1,1,1]
	movd	%xmm0, %r8d
	movl	%r8d, %r9d
	addl	%ecx, %r9d
	movabsq	$.L.str, %rdi
	movb	$0, %al
	callq	printf
	xorl	%eax, %eax
	addq	$32, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%d %d %d %d %d\n"
	.size	.L.str, 16

	.ident	"clang version 12.0.1 (https://github.com/llvm/llvm-project.git fed41342a82f5a3a9201819a82bf7a48313e296b)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym printf
