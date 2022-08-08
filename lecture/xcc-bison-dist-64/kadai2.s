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
	# or and child 0:AST_expression_less
	popq    %rax
	cmpq    $0, %rax
	jne      L_or_0
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
	# or child 1:AST_expression_eq
	popq    %rax
	pushq   %rax
	jmp     L_or_1
L_or_0:
	pushq   $1
L_or_1:
	popq    %rax
	cmpq    $0, %rax
	je      L_if_else_0
	# AST_expression_opt_single:
	# opt single child num = 1
	# opt single child[0]->ast_type = AST_expression_long
	# opt single child[0]->u.long_val = 0
	movq    $0x0, %rax
	pushq   %rax
	popq    %rax
	popq    %rbp
	ret
	jmp     L_if_else_1
L_if_else_0:
	# AST_expression_opt_single:
	# opt single child num = 1
	# opt single child[0]->ast_type = AST_expression_add
	# opt single child[0]->u.long_val = 0
	# codegen_exp_id called
	pushq   %rdi
	movq    $0xa, %rax
	pushq   %rax
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   %rdi
	popq    %rax
	popq    %rdx
	subq    %rdx, %rax
	pushq   %rax
	# codegen_exp_id called
	leaq    _mrn(%rip), %rax
	pushq   %rax
	popq    %rax
	popq    %rdi
	call    *%rax
# restore caller-save registers
	popq   %r9
	popq   %r8
	popq   %rcx
	popq   %rdx
	popq   %rsi
	popq   %rdi
	pushq   %rax
	popq    %rdx
	popq    %rax
	imulq   %rdx, %rax
	pushq   %rax
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
	pushq   %rax
	popq    %rax
	popq    %rbp
	ret
L_if_else_1:
L.XCC.RE.mrn:
	movq    %rbp, %rsp
	popq    %rbp
	retq
	.text
L.XCC.STR0:
	.ascii  "mrn(%ld) = %ld\n\0"
	.text
	.globl  _main
	.p2align 4, 0x90
_main:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $16, %rsp
	# assign child 1:AST_expression_long
	movq    $0x0, %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	# DEBUG NS_LOCAL left value i
	# DEBUG NS_LOCAL left value i
	leaq -8(%rbp), %rax
	pushq %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
L_while_0:
	movq    $0xb, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	popq    %rax
	popq    %rdx
	cmpq    %rdx, %rax
	setl    %al
	movzbq  %al, %rax
	pushq   %rax
	popq    %rax
	cmpq    $0, %rax
	je      L_while_1
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	# codegen_exp_id called
	leaq    _mrn(%rip), %rax
	pushq   %rax
	popq    %rax
	popq    %rdi
	call    *%rax
# restore caller-save registers
	popq   %r9
	popq   %r8
	popq   %rcx
	popq   %rdx
	popq   %rsi
	popq   %rdi
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	leaq    L.XCC.STR0(%rip), %rax 	# "mrn(%ld) = %ld\n"
	pushq   %rax
	# codegen_exp_id called
	movq    _printf@GOTPCREL(%rip), %rax
	pushq   %rax
	popq    %rax
	popq    %rdi
	popq    %rsi
	popq    %rdx
	call    *%rax
# restore caller-save registers
	popq   %r9
	popq   %r8
	popq   %rcx
	popq   %rdx
	popq   %rsi
	popq   %rdi
	pushq   %rax
	addq    $8, %rsp
	# assign child 1:AST_expression_add
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	# DEBUG NS_LOCAL left value i
	# DEBUG NS_LOCAL left value i
	leaq -8(%rbp), %rax
	pushq %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	jmp     L_while_0
L_while_1:
L.XCC.RE.main:
	movq    %rbp, %rsp
	popq    %rbp
	retq
