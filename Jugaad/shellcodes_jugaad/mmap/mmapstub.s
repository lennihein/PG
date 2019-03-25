	.text
	.file	"mmapstub.c"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp2:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	xorl	%eax, %eax
	movl	%eax, %ecx
	movl	$20000, %eax            # imm = 0x4E20
	movl	%eax, %esi
	movl	$7, %edx
	movl	$34, %eax
	movl	$4294967295, %r8d       # imm = 0xFFFFFFFF
	movl	$0, -4(%rbp)
	movq	%rcx, %rdi
	movq	%rcx, -16(%rbp)         # 8-byte Spill
	movl	%eax, %ecx
	movq	-16(%rbp), %r9          # 8-byte Reload
	callq	mmap
	xorl	%ecx, %ecx
	movq	%rax, -24(%rbp)         # 8-byte Spill
	movl	%ecx, %eax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.ident	"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"
	.section	".note.GNU-stack","",@progbits
