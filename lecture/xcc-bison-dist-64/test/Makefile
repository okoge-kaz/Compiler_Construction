XCC = ../xcc
SRC = ${wildcard *.c}
ASM = ${SRC:.c=.s}
AOUT = ${SRC:.c=}

all: ${ASM} ${AOUT} test

%.s: %.c
	${XCC}  $< > $@

%: %.s
	gcc -o $@ $<


test:
	-./kadai1
	-./kadai2
	-./kadai3 10

clean:
	-rm -f a.out ${ASM} ${AOUT} *~
