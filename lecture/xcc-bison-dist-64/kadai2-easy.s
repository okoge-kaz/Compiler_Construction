	.globl  _i
	 .data
	.p2align  3
_i:
	.skip   8

	.text
L.XCC.STR6:
	.ascii  "i: %d\n\0"
L.XCC.STR5:
	.ascii  "test: %d\n\0"
L.XCC.STR4:
	.ascii  "else\n\0"
L.XCC.STR3:
	.ascii  "2 < %d < 8\n\0"
L.XCC.STR2:
	.ascii  "%d * %d == %d\n\0"
L.XCC.STR1:
	.ascii  "2 / 2 == 1\n\0"
L.XCC.STR0:
	.ascii  "i == 2\n\0"
	.text
	.globl  _main
	.p2align 4, 0x90
_main:
	pushq   %rbp
	movq    %rsp, %rbp
	subq    $0, %rsp
	# assign child 1:AST_expression_long
	movq    $0x0, %rax
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
L_while_0:
	movq    $0xb, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	popq    %rax
	popq    %rdx
	cmpq    %rdx, %rax
	setl    %al
	movzbq  %al, %rax
	pushq   %rax
	popq    %rax
	cmpq    $0, %rax
	je      L_while_1
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	movq    $0x2, %rax
	pushq   %rax
	popq    %rcx
	popq    %rax
	cmpq    %rax, %rcx
	sete    %al
	movzbq  %al, %rax
	pushq   %rax
	popq    %rax
	cmpq    $0, %rax
	je      L_if_else_0
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	leaq    L.XCC.STR0(%rip), %rax 	# "i == 2\n"
	pushq   %rax
	# codegen_exp_id called
	movq    _printf@GOTPCREL(%rip), %rax
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
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	movq    $0x2, %rax
	pushq   %rax
	popq    %r10
	popq    %rax
	cqto
	idivq   %r10
	pushq   %rax
	movq    $0x1, %rax
	pushq   %rax
	popq    %rcx
	popq    %rax
	cmpq    %rax, %rcx
	sete    %al
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
	leaq    L.XCC.STR1(%rip), %rax 	# "2 / 2 == 1\n"
	pushq   %rax
	# codegen_exp_id called
	movq    _printf@GOTPCREL(%rip), %rax
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
L_if_0:
	jmp     L_if_else_1
L_if_else_0:
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	popq    %rdx
	popq    %rax
	imulq   %rdx, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	popq    %rcx
	popq    %rax
	cmpq    %rax, %rcx
	sete    %al
	movzbq  %al, %rax
	pushq   %rax
	popq    %rax
	cmpq    $0, %rax
	je      L_if_1
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
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	leaq    L.XCC.STR2(%rip), %rax 	# "%d * %d == %d\n"
	pushq   %rax
	# codegen_exp_id called
	movq    _printf@GOTPCREL(%rip), %rax
	pushq   %rax
	popq    %rax
	popq    %rdi
	popq    %rsi
	popq    %rdx
	popq    %rcx
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
L_if_1:
L_if_else_1:
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	movq    $0x2, %rax
	pushq   %rax
	popq    %rax
	popq    %rdx
	cmpq    %rdx, %rax
	setl    %al
	movzbq  %al, %rax
	pushq   %rax
	# or and child 0:AST_expression_less
	popq    %rax
	cmpq    $0, %rax
	je      L_and_0
	movq    $0x8, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	popq    %rax
	popq    %rdx
	cmpq    %rdx, %rax
	setl    %al
	movzbq  %al, %rax
	pushq   %rax
	# and child 1:AST_expression_less
	popq    %rax
	pushq   %rax
	jmp     L_and_1
L_and_0:
	pushq   $0
L_and_1:
	popq    %rax
	cmpq    $0, %rax
	je      L_if_else_2
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
	leaq    L.XCC.STR3(%rip), %rax 	# "2 < %d < 8\n"
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
	jmp     L_if_else_3
L_if_else_2:
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	leaq    L.XCC.STR4(%rip), %rax 	# "else\n"
	pushq   %rax
	# codegen_exp_id called
	movq    _printf@GOTPCREL(%rip), %rax
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
L_if_else_3:
	# assign child 1:AST_expression_add
	movq    $0x1, %rax
	pushq   %rax
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
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
L_while_2:
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	movq    $0x2, %rax
	pushq   %rax
	popq    %rcx
	popq    %rax
	cmpq    %rax, %rcx
	sete    %al
	movzbq  %al, %rax
	pushq   %rax
	# or and child 0:AST_expression_eq
	popq    %rax
	cmpq    $0, %rax
	jne      L_or_2
	# codegen_exp_id called
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	movq    $0xb, %rax
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
	jmp     L_or_3
L_or_2:
	pushq   $1
L_or_3:
	popq    %rax
	cmpq    $0, %rax
	je      L_while_3
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
	leaq    L.XCC.STR5(%rip), %rax 	# "test: %d\n"
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
	pushq   _i(%rip)
	# push global variable (right value) i
	# DEBUG: 0
	popq    %rax
	popq    %rdx
	addq    %rdx, %rax
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
	jmp     L_while_2
L_while_3:
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
	leaq    L.XCC.STR6(%rip), %rax 	# "i: %d\n"
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
