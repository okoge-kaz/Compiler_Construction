	.globl  _i
	 .data
	.p2align  3
_i:
	.skip   8

	.globl  _sum
	 .data
	.p2align  3
_sum:
	.skip   8

	.text
L.XCC.STR1:
	.ascii  "sum = %ld\n\0"
L.XCC.STR0:
	.ascii  "i = %ld\n\0"
	.text
	.globl  _main
	.p2align 4, 0x90
_main:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $0, %rsp
	# child 1:AST_expression_long
	movq    $0x5, %rax
	pushq   %rax
	# child 0:AST_expression_id
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable i
	popq    %rax
	popq    %rcx
	movq    %rcx, %rax
	# u.id = i
	movq    %rax, _i(%rip)
	pushq   %rcx
	addq    $8, %rsp
	# child 1:AST_expression_long
	movq    $0x0, %rax
	pushq   %rax
	# child 0:AST_expression_id
	# codegen_exp_id called
	pushq   _sum(%rip)
	# push global variable sum
	popq    %rax
	popq    %rcx
	movq    %rcx, %rax
	# u.id = sum
	movq    %rax, _sum(%rip)
	pushq   %rcx
	addq    $8, %rsp
L_while_0:
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable i
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
	# push global variable i
	leaq    L.XCC.STR0(%rip), %rax 	# "i = %ld\n"
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
	# child 1:AST_expression_add
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable i
	# codegen_exp_id called
	pushq   _sum(%rip)
	# push global variable sum
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
	pushq   %rax
	# child 0:AST_expression_id
	# codegen_exp_id called
	pushq   _sum(%rip)
	# push global variable sum
	popq    %rax
	popq    %rcx
	movq    %rcx, %rax
	# u.id = sum
	movq    %rax, _sum(%rip)
	pushq   %rcx
	addq    $8, %rsp
	# child 1:AST_expression_sub
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable i
	popq    %rax
	popq    %rdx
	subq    %rdx, %rax
	pushq   %rax
	# child 0:AST_expression_id
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable i
	popq    %rax
	popq    %rcx
	movq    %rcx, %rax
	# u.id = i
	movq    %rax, _i(%rip)
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
	# push global variable sum
	leaq    L.XCC.STR1(%rip), %rax 	# "sum = %ld\n"
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
