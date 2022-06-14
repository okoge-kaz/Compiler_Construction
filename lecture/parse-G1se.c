#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "AST.h"

static char *ptr;

static struct AST* parse_E  (void);
static struct AST* parse_T  (void);
static struct AST* parse_F  (void);
/* ------------------------------------------------------- */
static struct AST*
create_AST (char *ast_type, int num_child, ...)
{
    va_list ap;
    struct AST *ast;
    ast = malloc (sizeof (struct AST));
    ast->parent = NULL;
    ast->nth    = -1;
    ast->ast_type = ast_type;
    ast->num_child = num_child;
    va_start (ap, num_child);
    if (num_child == 0) {
        ast->child = NULL;
    } else {
        int i;
    	ast->child = malloc (sizeof(struct AST *) * num_child);
	for (i = 0; i < num_child; i++) {
	    struct AST *child = va_arg (ap, struct AST *);
	    ast->child [i] = child;
	    if (child != NULL) {
		child->parent = ast;
		child->nth    = i;
	    }
    	}
    }
    va_end (ap);
    return ast;
}

static struct AST*
add_AST (struct AST *ast, int num_child, ...)
{
    va_list ap;
    int i, start = ast->num_child;
    ast->num_child += num_child;
    assert (num_child > 0);
    ast->child = realloc (ast->child, sizeof(struct AST *) * ast->num_child);
    va_start (ap, num_child);
    for (i = start; i < ast->num_child; i++) {
        struct AST *child = va_arg (ap, struct AST *);
        ast->child [i] = child;
        if (child != NULL) {
            child->parent = ast;
            child->nth    = i;
        }
    }
    va_end (ap);
    return ast;
}

static void
show_AST (struct AST *ast, int depth)
{
    int i;
    printf ("%*s%s\n", depth, "", ast->ast_type); // 空文字列をdepth幅で印字
    for (i = 0; i < ast->num_child; i++) {
        if (ast->child [i] != NULL) {
            show_AST (ast->child [i], depth +1);
        }
    }
}
/* ------------------------------------------------------- */
static void
parse_error (void)
{
    fprintf (stderr, "parse error: %s\n", ptr);
    exit (1);
}

static char
lookahead (int i)
{
    return ptr [i - 1];
}

static char
next_token (void)
{
    return *ptr++;
}

static void
consume_token (char c)
{
    if (lookahead (1) == c) {
        next_token ();
    } else {
        parse_error ();
    }
}

/* ------------------------------------------------------- */
// E: T ("+" T)*;
static struct AST* parse_E (void)
{
    struct AST *ast, *ast1;
    switch (lookahead (1)) {
    case '(':
    case 'i':
        ast1 = parse_T ();
        ast = create_AST ("AST_E", 1, ast1);
        break;
    default:
        parse_error ();
        break;
    }
    while (1) {
        switch (lookahead (1)) {
        case '+':
            consume_token ('+');
            ast1 = parse_T ();
            ast = add_AST (ast, 1, ast1);
            break;
        default:
            goto loop_exit;
        }
    }
loop_exit:
    return ast;
}

// T: F ("*" F)*;
static struct AST* parse_T (void)
{
    struct AST *ast, *ast1;
    switch (lookahead (1)) {
    case '(':
    case 'i':
        ast1 = parse_F ();
        ast = create_AST ("AST_T", 1, ast1);
        break;
    default:
        parse_error ();
        break;
    }
    while (1) {
        switch (lookahead (1)) {
        case '*':
            consume_token ('*');
            ast1 = parse_F ();
            ast = add_AST (ast, 1, ast1);
            break;
        default:
            goto loop_exit;
        }
    }
loop_exit:
    return ast;
}

// F: "(" E ")" | i;
static struct AST*
parse_F (void)
{
    struct AST *ast, *ast1;
    switch (lookahead (1)) {
    case '(':
        consume_token ('(');
        ast1 = parse_E ();
        consume_token (')');
        ast = create_AST ("AST_F_()", 1, ast1);
        break;
    case 'i':
        consume_token ('i');
        ast = create_AST ("AST_F_i", 0);
        break;
    default:
        parse_error ();
        break;
    }
    return ast;
}

/* ------------------------------------------------------- */
int
main (int argc, char *argv[])
{
    struct AST *ast;
    if (argc != 2) {
        fprintf (stderr, "Usage: %s string\n", argv[0]);
        exit (1);
    }
    ptr = argv [1];
    ast = parse_E ();
    if (lookahead (1) != '\0') {
        parse_error ();
    }
    show_AST (ast, 0);
}
