	.text
	.globl  _mrn
	.p2align 4, 0x90
_mrn:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $0, %rsp
	movq    $0x0, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   %rdi
	popq    %rax
	popq    %rdx
	cmpq    %rdx, %rax
	setl    %al
	movzbq  %al, %rax
	pushq   %rax
	# child 0:AST_expression_less
	popq    %rax
	cmpq    $0, %rax
	je      L_or_0
	# codegen_exp_id called
	pushq   %rdi
	movq    $0x0, %rax
	pushq   %rax
	popq    %rcx
	popq    %rax
	cmpq    %rax, %rcx
	sete    %al
	movzbq  %al, %rax
	pushq   %rax
	# child 1:AST_expression_eq
	popq    %rax
	pushq   %rax
	jmp     L_or_1
L_or_0:
	pushq   $0
L_or_1:
	popq    %rax
	cmpq    $0, %rax
	je      L_if_else_0
codegen_exp: unknown ast_type: AST_expression_opt_single
