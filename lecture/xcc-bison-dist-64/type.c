/* XCC: Experimental C-subset Compiler.
  Copyright (c) 2002-2016, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: type.c,v 1.2 2016/05/19 04:23:35 gondow Exp gondow $ */ 
/* --------------------------------------------------------------------- */
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "xcc.h"
#include "misc.h"
#include "type.h"
#include "AST.h"

struct Types types; /* initialized by init() in xcc.y */
static struct Type *create_function_type (struct Type *ret_type);
static void set_arg_types (struct AST *ast, struct Type *type);
static void type_dump_sub (struct Type *type, int depth);
/* --------------------------------------------------------------------- */
static struct Type *
create_function_type (struct Type *ret_type)
{
    struct Type *type = emalloc (sizeof (struct Type));
    type->kind = TYPE_KIND_FUNCTION;
    type->size = -1; /* sizeof op. can not be applied to function */
    type->id   = NULL;
    type->u.t_function.ret_type = ret_type;

    /* the following members to be set later by set_arg_types(). */
    type->u.t_function.arg_num  = 0;
    type->u.t_function.arg_name = NULL;
    type->u.t_function.arg_type = NULL;
    return type;
}

static void
set_arg_types (struct AST *ast_plist, struct Type *type)
{
    struct AST *ast, *ast_par_decn;
    int    i, arg_num;

    assert (!strcmp (ast_plist->ast_type, "AST_parameter_list_single")
	    || !strcmp (ast_plist->ast_type, "AST_parameter_list_pair"));
    assert (type->kind == TYPE_KIND_FUNCTION);

    ast = search_AST_bottom (ast_plist, "AST_parameter_list_single", &arg_num);
    type->u.t_function.arg_name = emalloc (sizeof (char *) * arg_num);
    type->u.t_function.arg_type = emalloc (sizeof (struct Type *) * arg_num);
    type->u.t_function.arg_num  = arg_num;

    for (i = 0; ; i++) {
	int j;
        if (!strcmp (ast->ast_type, "AST_parameter_list_single")) {
	    ast_par_decn = ast->child [0]; 
        } else if (!strcmp (ast->ast_type, "AST_parameter_list_pair")) {
	    ast_par_decn = ast->child [1];
        } else {
	    assert (0);
	}

	type->u.t_function.arg_name [i] = ast_par_decn->type->id;
	type->u.t_function.arg_type [i] = ast_par_decn->type;

	/* check arguments with the same name as the previous one.
	   (1) int foo (int a, int a);           NG
           (2) int foo (int a, int (*a)());      NG
           (3) int foo (int a, int (*b)(int a)); OK             */
	for (j = 0; j < i; j++) {
	    char **arg_name = type->u.t_function.arg_name;
	    if (!strcmp (arg_name [i], arg_name [j])) {
		fprintf (stderr, "redefinition of %s\n", arg_name [i]);
		yyerror ("");
	    }
	}

	if (ast == ast_plist) break;
	ast = ast->parent;
    }
}

static void
type_dump_sub (struct Type *type, int depth)
{
    int  i;
    char *kind;

    switch (type->kind) {
    case TYPE_KIND_PRIM:     kind = "PRIMITIVE"; break;
    case TYPE_KIND_POINTER:  kind = "POINTER  "; break;
    case TYPE_KIND_FUNCTION: kind = "FUNCTION "; break;
    default:                 assert (0);         break;
    }

    printf ("%*s%s: %d %s: ", depth * 2, "", kind, type->size,
	    (type->id == NULL ? "" : type->id));

    switch (type->kind) {
    case TYPE_KIND_PRIM:
	switch (type->u.t_prim.ptype) {
	case PRIM_TYPE_VOID: printf ("void\n"); break;
	case PRIM_TYPE_CHAR: printf ("char\n"); break;
	case PRIM_TYPE_INT:  printf ("int\n");  break;
	case PRIM_TYPE_LONG: printf ("long\n"); break;
	default:             assert (0);        break;
	}
	break;
    case TYPE_KIND_POINTER:
	printf ("\n");
	type_dump_sub (type->u.t_pointer.type, depth + 1);
	break;
    case TYPE_KIND_FUNCTION:
	printf ("\n%*s=>return\n", depth * 2, "");
	type_dump_sub (type->u.t_function.ret_type, depth + 1);
	for (i = 0; i < type->u.t_function.arg_num; i++) {
	    printf ("%*s=>arg [%d]: %s\n", depth, "", i,
                    type->u.t_function.arg_name [i]);
	    type_dump_sub (type->u.t_function.arg_type [i], depth + 1);
	}
	break;
    default:
	assert (0); break;
    }
}
/* ---------------------------------------------------------------------- */
struct Type *
create_prim_type (enum PrimType ptype)
{
    struct Type *type = emalloc (sizeof (struct Type));
    type->kind = TYPE_KIND_PRIM;
    type->id   = NULL;
    type->u.t_prim.ptype = ptype;
    switch (ptype) {
    case PRIM_TYPE_VOID: type->size = -1; break;
    case PRIM_TYPE_CHAR: type->size = 1;  break;
    case PRIM_TYPE_INT:  type->size = 4;  break;
    case PRIM_TYPE_LONG: type->size = 8;  break;
    default:             assert (0);      break;
    }
    return type;
}

struct Type *
create_pointer_type (struct Type *pointedto_type)
{
    struct Type *type = emalloc (sizeof (struct Type));
    type->kind  = TYPE_KIND_POINTER;
    type->size  = 8;
    type->id    = NULL;
    type->u.t_pointer.type = pointedto_type;
    return type;
}


struct Type *
type_analyze_declarator (struct AST *ast_decr, struct Type *type)
{
    struct Type *type1, *type2;

    if (!strcmp (ast_decr->ast_type, "AST_declarator_id")) {
	type->id = ast_decr->child[0]->u.id;
	return type;
    } else if (!strcmp (ast_decr->ast_type, "AST_declarator_pointer")) {
	type1 = create_pointer_type (type);
	type2 = type_analyze_declarator (ast_decr->child [0], type1);
	type->id = type2->id;
	return type2;
    } else if (!strcmp (ast_decr->ast_type, "AST_declarator_paren")) {
	type1 = type_analyze_declarator (ast_decr->child [0], type);
	return type1;
    } else if (!strcmp (ast_decr->ast_type, "AST_declarator_func1")) {
	type1 = create_function_type (type);
	type2 = type_analyze_declarator (ast_decr->child [0], type1);
	type->id = type2->id;
	set_arg_types (ast_decr->child [1], type1);
	return type2;
    } else if (!strcmp (ast_decr->ast_type, "AST_declarator_func2")) {
	type1 = create_function_type (type);
	type2 = type_analyze_declarator (ast_decr->child [0], type1);
	type->id = type2->id;
	return type2;
    }

    assert (0);
    return NULL;
}

struct Type *
type_add (struct Type *type1, struct Type *type2)
{
    if ((type1->kind == TYPE_KIND_POINTER)
	&& (type2->kind == TYPE_KIND_POINTER)) {
	yyerror ("pointer + pointer prohibited\n");
    }
    if (type1->kind == TYPE_KIND_POINTER) return type1;
    if (type2->kind == TYPE_KIND_POINTER) return type2;
    return types.t_long;
}

struct Type *
type_sub (struct Type *type1, struct Type *type2)
{
    if ((type1->kind == TYPE_KIND_POINTER)
	&& (type2->kind == TYPE_KIND_POINTER)) {
	if (type1->size != type2->size) {
	    yyerror ("operand sizes are different\n");
	}
	return types.t_long;
    }
    if (type1->kind == TYPE_KIND_POINTER) return type1;
    if (type2->kind == TYPE_KIND_POINTER) {
	yyerror ("scalar - pointer prohibited\n");
    }
    return types.t_long;
}

struct Type *
type_uop (struct AST *ast, struct Type *type1)
{
    if (   !strcmp (ast->ast_type, "AST_unary_operator_plus")
        || !strcmp (ast->ast_type, "AST_unary_operator_minus")
        || !strcmp (ast->ast_type, "AST_unary_operator_negative")) {
	return types.t_long;
    } else if (!strcmp (ast->ast_type, "AST_unary_operator_address")) {
	return create_pointer_type (type1);
    } else if (!strcmp (ast->ast_type, "AST_unary_operator_deref")) {
	if (type1->kind == TYPE_KIND_POINTER) {
	    return type1->u.t_pointer.type;
	} else {
	    yyerror ("no pointer type dereferenced\n");
	}
    } else {
        assert (0);
    }
    return NULL; /* not reached here */
}


void
type_dump (struct Type *type)
{
    type_dump_sub (type, 0);
}
/* --------------------------------------------------------------------- */
