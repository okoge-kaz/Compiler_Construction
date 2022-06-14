#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
static char *ptr;

static void parse_E  (void);
static void parse_E2 (void);
static void parse_T  (void);
static void parse_T2 (void);
static void parse_F  (void);
/* ------------------------------------------------------- */
static void print_NT_begin (char *NT_name)
{
    printf ("{\"%s\":[\n", NT_name);
}

static void print_NT_end (void)
{
    printf ("]},\n");
}

static void print_token (void)
{
    printf ("{\"%c\": \"%c\"},\n", ptr [0], ptr [0]);

}
/* ------------------------------------------------------- */
static void parse_error (void)
{
    fprintf (stderr, "parse error: %s\n", ptr);
    exit (1);
}

static char lookahead (int i)
{
    return ptr [i - 1];
}

static char next_token (void)
{
    return *ptr++;
}

static void consume_token (char c)
{
    if (lookahead (1) == c) {
        print_token ();
        next_token ();
    } else {
        parse_error ();
    }
}


/* ------------------------------------------------------- */
// E: T ("+" T)*;
static void parse_E (void)
{
    print_NT_begin ("E");
    switch (lookahead (1)) {
    case '(':
    case 'i':
        parse_T ();
        break;
    default:
        parse_error ();
        break;
    }
    while (1) {
        switch (lookahead (1)) {
        case '+':
            consume_token ('+');
            parse_T ();
            break;
        default:
            goto loop_exit;
        }
    }
loop_exit:
    print_NT_end ();
}

// T: F ("*" F)*;
static void parse_T (void)
{
    print_NT_begin ("T");
    switch (lookahead (1)) {
    case '(':
    case 'i':
        parse_F ();
        break;
    default:
        parse_error ();
        break;
    }
    while (1) {
        switch (lookahead (1)) {
        case '*':
            consume_token ('*');
            parse_F ();
            break;
        default:
            goto loop_exit;
        }
    }
loop_exit:
    print_NT_end ();
}

// F: "(" E ")" | i;
static void parse_F (void)
{
    print_NT_begin ("F");

    switch (lookahead (1)) {
    case '(':
        consume_token ('(');
        parse_E ();
        consume_token (')');
        break;
    case 'i':
        consume_token ('i');
        break;
    default:
        parse_error ();
        break;
    }
    print_NT_end ();
}


/* ------------------------------------------------------- */
int
main (int argc, char *argv[])
{
    if (argc != 2) {
        fprintf (stderr, "Usage: %s string\n", argv[0]);
        exit (1);
    }
    ptr = argv [1];
    parse_E ();
    if (lookahead (1) != '\0') {
        parse_error ();
    }
}
