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
	pushq   _i(%rip)
	movq    $0x5, %rax
	pushq   %rax
	popq    %rax
	popq    %r10
	movq    %r10, 0(%rax)
	pushq   %r10
	addq    $8, %rsp
	pushq   _sum(%rip)
	movq    $0x0, %rax
	pushq   %rax
	popq    %rax
	popq    %r10
	movq    %r10, 0(%rax)
	pushq   %r10
	addq    $8, %rsp
.L_while_0:
	movq    $0x0, %rax
	pushq   %rax
	pushq   _i(%rip)
	popq    %rdx
	popq    %rax
	cmpq    %rax, %rdx
	pushq   %rax
	popq    %rax
	cmpq    $0, %rax
	je      .L_while_1
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	pushq   _i(%rip)
	leaq    L.XCC.STR0(%rip), %rax 	# "i = %ld\n"
	pushq   %rax
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
	pushq   _sum(%rip)
	pushq   _sum(%rip)
	pushq   _i(%rip)
	popq    %rdx
	popq    %rax
	addq    %rdx, %rax
	pushq   %rax
	popq    %rax
	popq    %r10
	movq    %r10, 0(%rax)
	pushq   %r10
	addq    $8, %rsp
	pushq   _i(%rip)
	pushq   _i(%rip)
	movq    $0x1, %rax
	pushq   %rax
	popq    %rdx
	popq    %rax
	subq    %rdx, %rax
	pushq   %rax
	popq    %rax
	popq    %r10
	movq    %r10, 0(%rax)
	pushq   %r10
	addq    $8, %rsp
	jmp     .L_while_0
.L_while_1:
# save caller-save registers
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	pushq   _sum(%rip)
	leaq    L.XCC.STR1(%rip), %rax 	# "sum = %ld\n"
	pushq   %rax
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
