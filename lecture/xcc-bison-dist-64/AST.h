/* XCC: Experimental C-subset Compiler.
  Copyright (c) 2002-2019, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: AST.h,v 1.2 2019/04/19 02:25:04 gondow Exp gondow $ */ 
#ifndef XCC_AST_H
#define XCC_AST_H
/* ---------------------------------------------------------------------- */
struct AST {
    /* common members */
    char           *ast_type; /* AST type of this node */
    struct AST	   *parent;   /* back pointer for parent. NULL for root */
    int	       	   nth;	      /* this AST is the nth child of the parent */
    int            num_child; /* num of children */
    struct AST     **child;   /* array of pointers to children */

    /* misc values for specific AST nodes */
    struct Type    *type; /* type for declaration and expression */
    union {
	char   *id;       /* for AST_IDENTIFIER, AST_expression_string */
	long   long_val;   /* for AST_expression_int, AST_expression_char, AST_expression_long */
	struct {
            int    total_local_size;
            struct Symbol *global;
	    struct Symbol *arg;
	    struct Symbol *label;
	    struct String *string;
	} func;                /* for AST_function_definition */
	struct Symbol *local;  /* for AST_compound_statement */
	// int    arg_size;       /* for AST_argument_expression_list_* */
    } u;

#ifdef XCC_VIS
    int ast_nth;               /* seq. number for AST instances */
#endif
};
/* ---------------------------------------------------------------------- */
struct AST *search_AST_bottom (struct AST *root, char *bottom, int *n);
struct AST *create_AST (char *ast_type, int argp_len, ...);
void dump_AST (struct AST *ast, int indent_depth);
/* ---------------------------------------------------------------------- */
#endif /* XCC_AST_H */
