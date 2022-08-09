	.text
	.globl  _mod
	.p2align 4, 0x90
_mod:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $0, %rsp
	# AST_expression_opt_single:
	# opt single child num = 1
	# opt single child[0]->ast_type = AST_expression_sub
	# opt single child[0]->u.long_val = 0
	# AST_expression_paren: num-child: 1
	# AST_expression_paren: child[0]: AST_expression_div
	# codegen_exp_id called
	pushq   %rdi
	# codegen_exp_id called
	pushq   %rsi
	popq    %r10
	popq    %rax
	cqto
	idivq   %r10
	pushq   %rax
	# codegen_exp_id called
	pushq   %rsi
	popq    %rdx
	popq    %rax
	imulq   %rdx, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   %rdi
	popq    %rax
	popq    %rdx
	subq    %rdx, %rax
	pushq   %rax
	popq    %rax
	popq    %rbp
	ret
L.XCC.RE.mod:
	movq    %rbp, %rsp
	popq    %rbp
	retq
	.text
	.globl  _swap
	.p2align 4, 0x90
_swap:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $16, %rsp
	# assign child 1:AST_expression_unary
	# AST_expression_unary: num-child: 2
	# AST_expression_unary: child[0]: AST_unary_operator_deref
	# AST_expression_unary: child[1]: AST_expression_id
	pushq   %rdi
	popq    %rax
	movq    (%rax), %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	# DEBUG NS_LOCAL left value tmp
	# DEBUG NS_LOCAL left value tmp
	leaq -8(%rbp), %rax
	pushq %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	# assign child 1:AST_expression_unary
	# AST_expression_unary: num-child: 2
	# AST_expression_unary: child[0]: AST_unary_operator_deref
	# AST_expression_unary: child[1]: AST_expression_id
	pushq   %rsi
	popq    %rax
	movq    (%rax), %rax
	pushq   %rax
	# assign unary operator child[1]:AST_expression_id
	pushq   %rdi
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	# assign child 1:AST_expression_id
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value tmp
	# DEBUG NS_LOCAL right value tmp
	pushq  -8(%rbp)
	# assign unary operator child[1]:AST_expression_id
	pushq   %rsi
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
L.XCC.RE.swap:
	movq    %rbp, %rsp
	popq    %rbp
	retq
	.text
	.globl  _bubble_sort
	.p2align 4, 0x90
_bubble_sort:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $16, %rsp
	# assign child 1:AST_expression_sub
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   %rsi
	popq    %rax
	popq    %rdx
	subq    %rdx, %rax
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
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	movq    $0x0, %rax
	pushq   %rax
	popq    %rax
	popq    %rdx
	cmpq    %rdx, %rax
	setl    %al
	movzbq  %al, %rax
	pushq   %rax
	popq    %rax
	cmpq    $0, %rax
	je      L_while_1
	# assign child 1:AST_expression_long
	movq    $0x0, %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	# DEBUG NS_LOCAL left value j
	# DEBUG NS_LOCAL left value j
	leaq -16(%rbp), %rax
	pushq %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
L_while_2:
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value j
	# DEBUG NS_LOCAL right value j
	pushq  -16(%rbp)
	popq    %rax
	popq    %rdx
	cmpq    %rdx, %rax
	setl    %al
	movzbq  %al, %rax
	pushq   %rax
	popq    %rax
	cmpq    $0, %rax
	je      L_while_3
	# AST_expression_unary: num-child: 2
	# AST_expression_unary: child[0]: AST_unary_operator_deref
	# AST_expression_unary: child[1]: AST_expression_paren
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	# codegen_exp_address: *(AST_expression_add)
	# codegen_exp_address: *(a + b) : a := AST_expression_id
	# codegen_exp_address: *(a + b) : b := AST_expression_id
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	pushq   %rdi
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value j
	# DEBUG NS_LOCAL right value j
	pushq  -16(%rbp)
	popq    %rdx
	popq    %rax
	imulq  $8, %rdx
	addq    %rdx, %rax
	pushq   %rax
	popq    %rax
	movq    (%rax), %rax
	pushq   %rax
	# AST_expression_unary: num-child: 2
	# AST_expression_unary: child[0]: AST_unary_operator_deref
	# AST_expression_unary: child[1]: AST_expression_paren
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	# codegen_exp_address: *(AST_expression_add)
	# codegen_exp_address: *(a + b) : a := AST_expression_id
	# codegen_exp_address: *(a + b) : b := AST_expression_paren
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	pushq   %rdi
	# AST_expression_paren: num-child: 1
	# AST_expression_paren: child[0]: AST_expression_add
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value j
	# DEBUG NS_LOCAL right value j
	pushq  -16(%rbp)
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
	pushq   %rax
	popq    %rdx
	popq    %rax
	imulq  $8, %rdx
	addq    %rdx, %rax
	pushq   %rax
	popq    %rax
	movq    (%rax), %rax
	pushq   %rax
	popq    %rax
	popq    %rdx
	cmpq    %rdx, %rax
	setl    %al
	movzbq  %al, %rax
	pushq   %rax
	popq    %rax
	cmpq    $0, %rax
	je      L_if_0
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# AST_expression_paren: num-child: 1
	# AST_expression_paren: child[0]: AST_expression_add
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value j
	# DEBUG NS_LOCAL right value j
	pushq  -16(%rbp)
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
	pushq   %rax
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	pushq   %rdi
	popq    %rax
	popq    %rdx
	imulq   $8, %rdx
	addq    %rdx, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value j
	# DEBUG NS_LOCAL right value j
	pushq  -16(%rbp)
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	pushq   %rdi
	popq    %rax
	popq    %rdx
	imulq   $8, %rdx
	addq    %rdx, %rax
	pushq   %rax
	# codegen_exp_id called
	leaq    _swap(%rip), %rax
	pushq   %rax
	popq    %rax
	popq    %rdi
	popq    %rsi
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
L_if_0:
	# assign child 1:AST_expression_add
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value j
	# DEBUG NS_LOCAL right value j
	pushq  -16(%rbp)
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	# DEBUG NS_LOCAL left value j
	# DEBUG NS_LOCAL left value j
	leaq -16(%rbp), %rax
	pushq %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	jmp     L_while_2
L_while_3:
	# assign child 1:AST_expression_sub
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	popq    %rax
	popq    %rdx
	subq    %rdx, %rax
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
L.XCC.RE.bubble_sort:
	movq    %rbp, %rsp
	popq    %rbp
	retq
	.text
	.globl  _make_array
	.p2align 4, 0x90
_make_array:
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
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# codegen_exp_id called
	pushq   %rdx
	# codegen_exp_id called
	movq    _srandom@GOTPCREL(%rip), %rax
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
	addq    $8, %rsp
L_while_4:
	# codegen_exp_id called
	pushq   %rsi
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
	je      L_while_5
	# assign child 1:AST_expression_funcall2
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# codegen_exp_id called
	movq    _random@GOTPCREL(%rip), %rax
	pushq   %rax
	popq    %rax
	call    *%rax
# restore caller-save registers
	popq   %r9
	popq   %r8
	popq   %rcx
	popq   %rdx
	popq   %rsi
	popq   %rdi
	pushq   %rax
	# assign child 0:AST_expression_id
	# DEBUG NS_LOCAL left value j
	# DEBUG NS_LOCAL left value j
	leaq -16(%rbp), %rax
	pushq %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	# assign child 1:AST_expression_funcall1
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# codegen_exp_id called
	pushq   %rsi
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value j
	# DEBUG NS_LOCAL right value j
	pushq  -16(%rbp)
	# codegen_exp_id called
	leaq    _mod(%rip), %rax
	pushq   %rax
	popq    %rax
	popq    %rdi
	popq    %rsi
	call    *%rax
# restore caller-save registers
	popq   %r9
	popq   %r8
	popq   %rcx
	popq   %rdx
	popq   %rsi
	popq   %rdi
	pushq   %rax
	# assign unary operator child[1]:AST_expression_paren
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	# codegen_exp_address: *(AST_expression_add)
	# codegen_exp_address: *(a + b) : a := AST_expression_id
	# codegen_exp_address: *(a + b) : b := AST_expression_id
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	pushq   %rdi
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	popq    %rdx
	popq    %rax
	imulq  $8, %rdx
	addq    %rdx, %rax
	pushq   %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
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
	jmp     L_while_4
L_while_5:
L.XCC.RE.make_array:
	movq    %rbp, %rsp
	popq    %rbp
	retq
	.text
L.XCC.STR0:
	.ascii  "%ld\n\0"
	.text
	.globl  _main
	.p2align 4, 0x90
_main:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $32, %rsp
	# assign child 1:AST_expression_funcall1
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# AST_expression_unary: num-child: 2
	# AST_expression_unary: child[0]: AST_unary_operator_deref
	# AST_expression_unary: child[1]: AST_expression_paren
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	# codegen_exp_address: *(AST_expression_add)
	# codegen_exp_address: *(a + b) : a := AST_expression_id
	# codegen_exp_address: *(a + b) : b := AST_expression_long
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	pushq   %rsi
	movq    $0x1, %rax
	pushq   %rax
	popq    %rdx
	popq    %rax
	imulq  $8, %rdx
	addq    %rdx, %rax
	pushq   %rax
	popq    %rax
	movq    (%rax), %rax
	pushq   %rax
	# codegen_exp_id called
	movq    _atol@GOTPCREL(%rip), %rax
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
	# assign child 0:AST_expression_id
	# DEBUG NS_LOCAL left value size
	# DEBUG NS_LOCAL left value size
	leaq -24(%rbp), %rax
	pushq %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	# assign child 1:AST_expression_funcall1
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	movq    $0x8, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value size
	# DEBUG NS_LOCAL right value size
	pushq  -24(%rbp)
	popq    %rdx
	popq    %rax
	imulq   %rdx, %rax
	pushq   %rax
	# codegen_exp_id called
	movq    _malloc@GOTPCREL(%rip), %rax
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
	# assign child 0:AST_expression_id
	# DEBUG NS_LOCAL left value p
	# DEBUG NS_LOCAL left value p
	leaq -16(%rbp), %rax
	pushq %rax
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	movq    $0x0, %rax
	pushq   %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value size
	# DEBUG NS_LOCAL right value size
	pushq  -24(%rbp)
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value p
	# DEBUG NS_LOCAL right value p
	pushq  -16(%rbp)
	# codegen_exp_id called
	leaq    _make_array(%rip), %rax
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
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value size
	# DEBUG NS_LOCAL right value size
	pushq  -24(%rbp)
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value p
	# DEBUG NS_LOCAL right value p
	pushq  -16(%rbp)
	# codegen_exp_id called
	leaq    _bubble_sort(%rip), %rax
	pushq   %rax
	popq    %rax
	popq    %rdi
	popq    %rsi
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
L_while_6:
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value size
	# DEBUG NS_LOCAL right value size
	pushq  -24(%rbp)
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
	je      L_while_7
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# AST_expression_unary: num-child: 2
	# AST_expression_unary: child[0]: AST_unary_operator_deref
	# AST_expression_unary: child[1]: AST_expression_paren
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	# codegen_exp_address: *(AST_expression_add)
	# codegen_exp_address: *(a + b) : a := AST_expression_id
	# codegen_exp_address: *(a + b) : b := AST_expression_id
	# codegen_exp_address is called
	# codegen_exp_address num_child: 1
	# DEBUG NS_LOCAL left value p
	# DEBUG NS_LOCAL left value p
	leaq -16(%rbp), %rax
	pushq %rax
	# codegen_exp_id called
	# DEBUG NS_LOCAL right value i
	# DEBUG NS_LOCAL right value i
	pushq  -8(%rbp)
	popq    %rdx
	popq    %rax
	imulq  $8, %rdx
	addq    %rdx, %rax
	pushq   %rax
	popq    %rax
	movq    (%rax), %rax
	pushq   %rax
	leaq    L.XCC.STR0(%rip), %rax 	# "%ld\n"
	pushq   %rax
	# codegen_exp_id called
	movq    _printf@GOTPCREL(%rip), %rax
	pushq   %rax
	popq    %rax
	popq    %rdi
	popq    %rsi
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
	jmp     L_while_6
L_while_7:
L.XCC.RE.main:
	movq    %rbp, %rsp
	popq    %rbp
	retq
