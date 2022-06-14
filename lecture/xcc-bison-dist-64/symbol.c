/* XCC: Experimental C-subset Compiler.
  Copyright (c) 2002-2019, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: symbol.c,v 1.2 2016/05/19 04:24:54 gondow Exp gondow $ */ 
/* --------------------------------------------------------------------- */
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "xcc.h"
#include "misc.h"
#include "AST.h"
#include "type.h"
#include "symbol.h"

/* --------------------------------------------------------------------- */
struct SymTable sym_table = {NULL, NULL, NULL, NULL, -1};

/* --------------------------------------------------------------------- */
static int total_local_size;
static int local_size [MAX_BLOCK_DEPTH];

static struct Symbol *sym_lookup_at (char *name, struct Symbol *head);
static struct Symbol *sym_lookup_global (char *name);
static struct Symbol *sym_lookup_arg    (char *name);
static struct Symbol *sym_lookup_local  (char *name);
static void sym_free (struct Symbol *sym);
static void sym_dump (struct Symbol *sym);
static void string_dump (struct String *string);
static int set_offset (struct Symbol *sym, int base);
/* --------------------------------------------------------------------- */
static struct Symbol *
sym_lookup_at (char *name, struct Symbol *head)
{
    struct Symbol *sym;

    for (sym = head; sym != NULL; sym = sym->next) {
	if (!strcmp (sym->name, name))
	    return sym;
    }
    return NULL;
}

static void
sym_free (struct Symbol *sym)
{
    struct Symbol *p, *next;
    for (p = sym; p != NULL; p = next) {
         next = p->next;
         efree (p);
    }
}

static void
sym_dump (struct Symbol *sym)
{
    struct Symbol *p;
    for (p = sym; p != NULL; p = p->next) {
	printf ("%s, ", p->name);
    }
    printf ("\n");
}

static void
string_dump (struct String *string)
{
    struct String *p;
    for (p = string; p != NULL; p = p->next) {
	printf ("%s (%s), ", p->data, p->label);
    }
    printf ("\n");
}

static struct Symbol *
sym_lookup_global (char *name)
{
    return sym_lookup_at (name, sym_table.global);
}

static struct Symbol *
sym_lookup_arg (char *name)
{
    return sym_lookup_at (name, sym_table.arg);
}

static struct Symbol *
sym_lookup_current_local (char *name)
{
    return sym_lookup_at (name, sym_table.local [sym_table.local_index]);
}

static struct Symbol *
sym_lookup_local (char *name)
{
    int i;
    struct Symbol *sym;

    for (i = sym_table.local_index; i >= 0; i--) {
	sym = sym_lookup_at (name, sym_table.local [i]);
	if (sym != NULL)
	    return sym;
    }
    return NULL;
}

static int
set_offset (struct Symbol *sym, int base)
{
    int size;
    if (sym == NULL)
	return base;
    sym->offset = set_offset (sym->next, base);
    size = ROUNDUP_LONG (sym->type->size);
    return sym->offset + size;
}
/* --------------------------------------------------------------------- */
struct Symbol *
sym_lookup (char *name)
{
    struct Symbol *sym;
    sym = sym_lookup_local  (name);
    if (sym != NULL) return sym;
    sym = sym_lookup_arg    (name);
    if (sym != NULL) return sym;
    sym = sym_lookup_global (name);
    if (sym != NULL) return sym;
    return NULL;
}

struct Symbol *
sym_lookup_label (char *name)
{
    return sym_lookup_at (name, sym_table.label);
}

struct String *
string_lookup (char *data)
{
    struct String *p;

    for (p = sym_table.string; p != NULL; p = p->next) {
	if (!strcmp (p->data, data))
	    return p;
    }
    return NULL;
}

void
sym_entry (struct AST *ast)
{
    struct Symbol **entry;
    struct Symbol *sym = emalloc (sizeof (struct Symbol));
    struct Symbol *sym2;

    if (!strcmp (ast->ast_type, "AST_statement_label")) {
	sym->name = ast->child [0]->u.id;
    } else {
	sym->name = ast->type->id;
    }
    sym->type   = ast->type;
    sym->ast    = ast;
    sym->offset = 0;

#if 1 /* for debugging */
    if (!strcmp (sym->name, "sym_table_dump"))
	sym_table_dump ();
    {
    	char *s = "type_dump_";
	int len = strlen (s);
	if (!strncmp (sym->name, s, len)) {
	    struct Symbol *sym3;
	    char *name = sym->name + len;
	    sym3 = sym_lookup (name);
	    if (sym3 != NULL && sym3->type != NULL)
		type_dump (sym3->type);
	}
    }
#endif

    assert (strcmp (ast->ast_type, "AST_parameter_declaration"));

    if (!strcmp (ast->ast_type, "AST_statement_label")) {
	sym->name_space = NS_LABEL;
	sym2 = sym_lookup_label (sym->name);
	entry = &sym_table.label;  /* labels */
#if 0
    /* parameters are registered to the 'sym_table'
       in sym_entry_param(), not here. */
    } else if (!strcmp (ast->ast_type, "AST_parameter_declaration")) {
	sym->name_space = NS_ARG;
	sym2 = sym_lookup_arg (sym->name);
	entry = &sym_table.arg;    /* arguments */
#endif
    } else if (AST_is_local) {
	sym->name_space = NS_LOCAL;
	sym2 = sym_lookup_current_local (sym->name);
	entry = &sym_table.local [sym_table.local_index]; /* local variables */
    } else {
	sym->name_space = NS_GLOBAL;
	sym2 = sym_lookup_global (sym->name);
	entry = &sym_table.global; /* global variables */
    }
    if (sym2 != NULL) {
	fprintf (stderr, "redefinition of %s\n", sym->name);
	yyerror ("");
    }
    sym->next = *entry;
    *entry = sym;
}

static char *
create_string_label (void)
{
    static int num = 0;
    char *label = emalloc (32);
    snprintf (label, 32, "STR%d", num++);
    return label;
}

void
string_entry (char *data)
{
    struct String *string;

    string = string_lookup (data);
    if (string != NULL) /* already entried */
        return;

    string = emalloc (sizeof (struct String));
    string->data  = data;
    string->label = create_string_label ();
    string->next  = sym_table.string;
    sym_table.string = string;
}

void
sym_begin_function (void)
{
    AST_is_local     = 1;
    total_local_size = 0;
    memset (local_size, 0, sizeof (local_size));
    set_offset (sym_table.arg, 0);
}

void
sym_end_function (struct AST *ast)
{
    assert(!strcmp (ast->ast_type, "AST_function_definition"));

    AST_is_local = 0;
    ast->u.func.global = sym_table.global;
    ast->u.func.arg    = sym_table.arg;
    ast->u.func.label  = sym_table.label;
    ast->u.func.string = sym_table.string;
    ast->u.func.total_local_size = ROUNDUP_STACK (total_local_size);
    sym_table.arg    = NULL;
    sym_table.label  = NULL;
    sym_table.string = NULL;
}

void
sym_begin_block (void)
{
    sym_table.local_index++;
    if (sym_table.local_index >= MAX_BLOCK_DEPTH) {
	yyerror ("MAX_BLOCK_DEPTH exceeded\n");
    }
    assert (sym_table.local [sym_table.local_index] == NULL);
}

void
sym_middle_block (void)
{
    int depth = sym_table.local_index;
    if (depth == 0)
	local_size [depth] = set_offset (sym_table.local [depth], 0);
    else
	local_size [depth] = set_offset (sym_table.local [depth],
					 local_size [depth - 1]);
    if (local_size [depth] > total_local_size)
	total_local_size = local_size [depth];
}

void
sym_end_block (struct AST *ast)
{
    assert (!strcmp (ast->ast_type, "AST_compound_statement"));
    assert (sym_table.local_index >= 0);
    ast->u.local = sym_table.local [sym_table.local_index];
    sym_table.local [sym_table.local_index] = NULL;
    sym_table.local_index--;
}

void
sym_backpatch (struct AST *ast, struct Type *type)
{
    struct Symbol *sym;

    ast->type = type;
    sym = sym_lookup_global (type->id);
    assert (sym != NULL);
    sym->ast = ast;
}

void
sym_entry_param (struct Type *func_type)
{
    int    i, arg_num;
    char   **arg_name;
    struct Type **arg_type;
    struct Symbol *sym;

    assert (func_type->kind == TYPE_KIND_FUNCTION);
    arg_num  = func_type->u.t_function.arg_num;
    arg_name = func_type->u.t_function.arg_name;
    arg_type = func_type->u.t_function.arg_type;

    /* The same name argument is checked in type.c, not here. */
    for (i = 0; i < arg_num; i++) {
	sym = emalloc (sizeof (struct Symbol));
	sym->name   = arg_name [i];
	sym->type   = arg_type [i];
	sym->ast    = NULL; /* parameter_declaration should be set here */
	sym->offset = 0;
	sym->name_space  = NS_ARG;
	sym->next   = sym_table.arg;
	sym_table.arg = sym;
    }
}

struct Symbol *
sym_check_var_decl (struct AST *ast)
{
    char *id = ast->child [0]->u.id;
    struct Symbol *sym = sym_lookup (id);
    assert (!strcmp (ast->ast_type, "AST_expression_id"));
    if (sym == NULL) {
	fprintf (stderr, "varialble %s is used, but not declared\n", id);
	yyerror ("");
    }
    return sym;
}

void
sym_table_dump (void)
{
    int i;
    printf ("global:   ");
    sym_dump (sym_table.global);
    printf ("arg:      ");
    sym_dump (sym_table.arg);
    printf ("label:    ");
    sym_dump (sym_table.label);
    printf ("string:   ");
    string_dump (sym_table.string);
    for (i = 0; i <= sym_table.local_index; i++) {
	printf ("local[%d]: ", i);
	sym_dump (sym_table.local [i]);
    }
}
/* --------------------------------------------------------------------- */
