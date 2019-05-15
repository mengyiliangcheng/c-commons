	.file	"sum.c"
	.text
	.globl	plus
	.type	plus, @function
plus:
.LFB11:
	.cfi_startproc
	leaq	(%rdi,%rsi), %rax
	ret
	.cfi_endproc
.LFE11:
	.size	plus, .-plus
	.globl	sumstore
	.type	sumstore, @function
sumstore:
.LFB12:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdx, %rbx
	call	plus
	movq	%rax, (%rbx)
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE12:
	.size	sumstore, .-sumstore
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%ld + %ld = %ld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB13:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	movq	%rsi, %rbp
	movq	8(%rsi), %rdi
	movl	$0, %eax
	call	atoi
	movslq	%eax, %rbx
	movq	16(%rbp), %rdi
	movl	$0, %eax
	call	atoi
	movslq	%eax, %rbp
	leaq	8(%rsp), %rdx
	movq	%rbp, %rsi
	movq	%rbx, %rdi
	call	sumstore
	movq	8(%rsp), %rcx
	movq	%rbp, %rdx
	movq	%rbx, %rsi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE13:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-16)"
	.section	.note.GNU-stack,"",@progbits
