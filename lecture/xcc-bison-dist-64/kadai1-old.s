	.globl  _i
	 .data
_i:
	.skip   4

	.globl  _sum
	 .data
_sum:
	.skip   4

	.text
L.XCC.STR1:
	.ascii  "sum = %d\n\0"
L.XCC.STR0:
	.ascii  "i = %d\n\0"
	.text
	.globl  _main
	.p2align 4, 0x90
_main:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $0, %rsp
	# assign child 1:AST_expression_long
	movq    $0x5, %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	leaq   _i(%rip), %rax
	pushq   %rax
	# push global variable's address(left value) i
	# DEBUG: 0
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	# assign child 1:AST_expression_long
	movq    $0x0, %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	leaq   _sum(%rip), %rax
	pushq   %rax
	# push global variable's address(left value) sum
	# DEBUG: 0
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
L_while_0:
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
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
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	leaq    L.XCC.STR0(%rip), %rax 	# "i = %d\n"
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
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	# codegen_exp_id called
	pushq   _sum(%rip)
	# push global variable (right value) sum
	# DEBUG: 0
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	leaq   _sum(%rip), %rax
	pushq   %rax
	# push global variable's address(left value) sum
	# DEBUG: 0
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	# assign child 1:AST_expression_sub
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	popq    %rax
	popq    %rdx
	subq    %rdx, %rax
	pushq   %rax
	# assign child 0:AST_expression_id
	leaq   _i(%rip), %rax
	pushq   %rax
	# push global variable's address(left value) i
	# DEBUG: 0
	popq    %rax
	popq    %rcx
	movq    %rcx, (%rax)
	pushq   %rcx
	addq    $8, %rsp
	jmp     L_while_0
L_while_1:
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	# codegen_exp_id called
	pushq   _sum(%rip)
	# push global variable (right value) sum
	# DEBUG: 0
	leaq    L.XCC.STR1(%rip), %rax 	# "sum = %d\n"
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
L.XCC.RE.main:
	movq    %rbp, %rsp
	popq    %rbp
	retq
