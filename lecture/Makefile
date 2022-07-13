# gcc bin
GCC_BIN=/usr/bin/

# gcc compile options
GCC_OPTIONS = -Wall -Wextra -Wshadow -fsanitize=address,undefined -g

# command to compile c source file
GCC=${GCC_BIN}gcc ${GCC_OPTIONS}

# target file
EXES=xcc-small

# make commands
xcc-small: xcc-small.c
	${GCC} -o $@ $<
xcc-small-test:

test: test1 test2
test1: $(EXES)
	@for x in test1/t*.c; do ./$(EXES) $$x > out/$$x ; done
test2: $(EXES)
	@for x in test2/t*.c; do ./$(EXES) $$x > out/$$x ; done

clean:
	rm -f ${EXES}

check:
	@for x in test1/t*.c; do ${GCC} $$x; done