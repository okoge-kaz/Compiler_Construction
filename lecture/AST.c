#include <stdio.h>
#include <stdarg.h> // for 可変長リスト
#include <stdlib.h> // for malloc
#include <string.h> // for strcmp
#include "AST.h"

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
    ast->lexeme = NULL;
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
create_leaf (char *ast_type, char *lexeme)
{
    struct AST *ast;
    ast = malloc (sizeof (struct AST));
    ast->parent    = NULL;
    ast->nth       = -1;
    ast->ast_type  = ast_type;
    ast->num_child = 0;
    ast->child     = NULL;
    ast->lexeme    = lexeme;
    return ast;
}

int main (void)
{
    struct AST *ast1, *ast2, *ast3, *ast4, *ast5, *ast6;
    // (10+x)*y の構文木を作る
    ast1 = create_leaf ("AST_int", "10");
    ast2 = create_leaf ("AST_id", "x");
    ast3 = create_AST ("AST_add", 2, ast1, ast2);
    ast4 = create_AST ("AST_paren", 1, ast3);
    ast5 = create_leaf ("AST_id", "y");
    ast6 = create_AST ("AST_mult", 2, ast4, ast5);
    return 0;
}
