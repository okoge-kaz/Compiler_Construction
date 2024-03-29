#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
static char *ptr;

static void parse_E(void);
static void parse_E2(void);
static void parse_T(void);
static void parse_T2(void);
static void parse_F(void);
/* ------------------------------------------------------- */
static void print_NT_begin(char *NT_name) {
    printf("{\"%s\":[\n", NT_name);
}

static void print_NT_end(void) {
    printf("]},\n");
}

static void print_token(void) {
    printf("{\"%c\": \"%c\"},\n", ptr[0], ptr[0]);
}
/* ------------------------------------------------------- */
static void parse_error(void) {
    fprintf(stderr, "parse error: %s\n", ptr);
    exit(1);
}

static char lookahead(int i) {
    return ptr[i - 1];
}

static char next_token(void) {
    return *ptr++;
}

static void consume_token(char c) {
    if (lookahead(1) == c) {
        print_token();
        next_token();
    } else {
        parse_error();
    }
}

/* ------------------------------------------------------- */
// E: T E2;
static void parse_E(void) {
    print_NT_begin("E");
    switch (lookahead(1)) {
        case '(':
        case 'i':
            parse_T();
            parse_E2();
            break;
        default:
            parse_error();
            break;
    }
    print_NT_end();
}

// E2: "+" T E2 | ε
static void parse_E2(void) {
    print_NT_begin("E2");

    switch (lookahead(1)) {
        case '+':
            consume_token('+');
            parse_T();
            parse_E2();
            break;
        case '\0':
        case ')':
            // do nothing
            break;
        default:
            parse_error();
            break;
    }
    print_NT_end();
}

// T: F T2;
static void parse_T(void) {
    print_NT_begin("T");

    switch (lookahead(1)) {
        case '(':
        case 'i':
            parse_F();
            parse_T2();
            break;
        default:
            parse_error();
            break;
    }
    print_NT_end();
}

// T2: "*" F T2 | ε;
static void parse_T2(void) {
    print_NT_begin("T2");

    switch (lookahead(1)) {
        case '*':
            consume_token('*');
            parse_F();
            parse_T2();
            break;
        case '+':
        case ')':
        case '\0':
            // do nothing
            break;
        default:
            parse_error();
            break;
    }
    print_NT_end();
}

// F: "(" E ")" | i;
static void parse_F(void) {
    print_NT_begin("F");

    switch (lookahead(1)) {
        case '(':
            consume_token('(');
            parse_E();
            consume_token(')');
            break;
        case 'i':
            consume_token('i');
            break;
        default:
            parse_error();
            break;
    }
    print_NT_end();
}

/* ------------------------------------------------------- */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s string\n", argv[0]);
        exit(1);
    }
    ptr = argv[1];
    parse_E();
    if (lookahead(1) != '\0') {
        parse_error();
    }
}
