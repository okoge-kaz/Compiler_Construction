/* XCC: Experimental C-subset compiler.
  Copyright (c) 2002-2019, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: AST.c,v 1.2 2019/04/19 02:25:18 gondow Exp gondow $ */ 
/* --------------------------------------------------------------------- */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "AST.h"
#include "misc.h"
/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */
struct AST *
search_AST_bottom (struct AST *root, char *bottom, int *n)
{
    int    i = 1;
    struct AST *p;
    for (p = root; strcmp (p->ast_type, bottom); p = p->child [0]) {
        assert (p != NULL);
	i++;
    }
    if (n != NULL)
	*n = i;
    return p;
}

struct AST *
create_AST (char *ast_type, int argp_len, ...)
{
    va_list      argp;
    int          i;
    struct AST 	 *ast;

    ast = emalloc (sizeof (struct AST));
    ast->ast_type  = ast_type;

    va_start (argp, argp_len);
    
    if (!strcmp (ast_type, "AST_IDENTIFIER")
        || !strcmp (ast_type, "AST_expression_string")) {
    	ast->num_child = 0;
        ast->child = NULL;
	ast->u.id = strdup (va_arg (argp, char *));
        if (ast->u.id == NULL) {
            perror ("strdup");
        }
    } else if (!strcmp (ast_type, "AST_expression_int")
               || !strcmp (ast_type, "AST_expression_char")
               || !strcmp (ast_type, "AST_expression_long")) {
    	ast->num_child = 0;
        ast->child = NULL;
	ast->u.long_val = va_arg (argp, long);
    } else {
    	ast->num_child = argp_len;
    	ast->child = emalloc (sizeof(struct AST *) * argp_len);
	for (i = 0; i < argp_len; i++) {
	    struct AST *child = va_arg (argp, struct AST *);
	    ast->child [i] = child;
	    if (child != NULL) {
		child->parent = ast;
		child->nth    = i;
	    }
    	}
    }
    va_end (argp);

#ifdef XCC_VIS
    ast->ast_nth = 0;
#endif
    return ast;
}

void
dump_AST (struct AST *ast, int indent_depth)
{
    int i;
    printf ("%*s%s\n", indent_depth, "", ast->ast_type);
    for (i = 0; i < ast->num_child; i++) {
        dump_AST (ast->child [i], indent_depth + 1);
    }
}

#ifdef AST_MAIN
int
main (void)
{
    struct AST *ast1, *ast2, *ast3;
    ast1 = create_AST ("AST_int", 0, 10);
    ast2 = create_AST ("AST_id", 0, "x");
    ast3 = create_AST ("AST_add", 2, ast1, ast2);
    ast1 = create_AST ("AST_paren", 1, ast3);
    ast2 = create_AST ("AST_id", 0, "y");
    ast3 = create_AST ("AST_mult", 2, ast1, ast2);
    return 0;
}
#endif
/* --------------------------------------------------------------------- */
