/* XCC: Experimental C-subset compiler.
  Copyright (c) 2002-2017, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: xcc.y,v 1.1 2017/04/28 07:36:23 gondow Exp gondow $ */ 
%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xcc.h"
#include "misc.h"
#include "AST.h"
#include "type.h"
#include "symbol.h"
#include "codegen.h"
#ifdef XCC_VIS
#include "vis/vis.h"
#endif
#define YYERROR_VERBOSE

struct AST *ast_root;
char       *filename;
FILE       *xcc_out;
int AST_is_local = 0;

static void init (int argc, char *argv []);
%}

%expect 1    /* for dangling-else */
%start translation_unit
%token-table

/* keywords (literal string tokens) */
%token CHAR     "char"
%token ELSE     "else"
%token GOTO     "goto"
%token IF       "if"
%token INT      "int"
%token LONG     "long"
%token RETURN   "return"
%token VOID     "void"
%token WHILE    "while"

/* operators */
%token EQ       "=="
%token AND      "&&"
%token OR       "||"

/* identifiers */
%token IDENTIFIER

/* literals */
%token STRING
%token INTEGER_CONSTANT
%token CHARACTER_CONSTANT

/* operator precedence & associativity */
%right '='
%left  "||"
%left  "&&"
%left  "=="
%left  '<'
%left  '+' '-'
%left  '*' '/'
%right UNARY_OP
%left  '('

%%
translation_unit
    : external_declaration
    	{ $$ = create_AST ("AST_translation_unit_single", 1, $1);
	  ast_root = $$; }
    | translation_unit external_declaration
    	{ $$ = create_AST ("AST_translation_unit_pair", 2, $1, $2);
	  ast_root = $$; }
    ;

external_declaration
    : function_definition
    	{ $$ = create_AST ("AST_external_declaration_func", 1, $1); }
    | declaration
    	{ $$ = create_AST ("AST_external_declaration_dec", 1, $1); 
	  sym_table.arg    = NULL;
  	  sym_table.label  = NULL;
  	  sym_table.string = NULL; }
    ;

function_definition
    : type_specifier declarator
	{ $$ = create_AST ("AST_dummy", 2, $1, $2);
	  $$->type = type_analyze_declarator ($2, $1->type); 
          /* conditonal to cause more natural syntax error */
          if ($$->type->kind == TYPE_KIND_FUNCTION) {
              sym_entry_param ($$->type);
              sym_entry ($$);
              sym_begin_function ();
          } }
      compound_statement
        { $$ = create_AST ("AST_function_definition", 3, $1, $2, $4);
          sym_backpatch ($$, $3->type);
	  sym_end_function ($$); }
    ;

declaration_list
    : 
    	{ $$ = create_AST ("AST_declaration_list_null", 0); }
    | declaration_list declaration
    	{ $$ = create_AST ("AST_declaration_list_pair", 2, $1, $2); }
    ;

declaration
    : type_specifier declarator ';'
    	{ $$ = create_AST ("AST_declaration", 2, $1, $2); 
	  $$->type = type_analyze_declarator ($2, $1->type);
	  sym_entry ($$); }
    ;

type_specifier
    : "void" { $$ = create_AST ("AST_type_specifier_void", 0);
               $$->type = create_prim_type (PRIM_TYPE_VOID); }
    | "char" { $$ = create_AST ("AST_type_specifier_char", 0);
               $$->type = create_prim_type (PRIM_TYPE_CHAR); }
    | "int"  { $$ = create_AST ("AST_type_specifier_int", 0);
               $$->type = create_prim_type (PRIM_TYPE_INT); }
    | "long" { $$ = create_AST ("AST_type_specifier_long", 0);
               $$->type = create_prim_type (PRIM_TYPE_LONG); }
    ;

declarator
    : identifier
	{ $$ = create_AST ("AST_declarator_id", 1, $1); }
    | '*' declarator
	{ $$ = create_AST ("AST_declarator_pointer", 1, $2); }
    | '(' declarator ')'
	{ $$ = create_AST ("AST_declarator_paren", 1, $2); }
    | declarator '(' parameter_list ')'
	{ $$ = create_AST ("AST_declarator_func1", 2, $1, $3); }
    | declarator '(' ')'
	{ $$ = create_AST ("AST_declarator_func2", 1, $1); }
    ;

parameter_list
    : parameter_declaration
	{ $$ = create_AST ("AST_parameter_list_single", 1, $1); }
    | parameter_list ',' parameter_declaration
	{ $$ = create_AST ("AST_parameter_list_pair", 2, $1, $3); }
    ;

parameter_declaration
    : type_specifier declarator
	{ $$ = create_AST ("AST_parameter_declaration", 2, $1, $2);
	  $$->type = type_analyze_declarator ($2, $1->type);
	  /* sym_entry ($$); */ }
    ;

statement_list
    : statement
	{ $$ = create_AST ("AST_statement_list_single", 1, $1); }
    | statement_list statement
	{ $$ = create_AST ("AST_statement_list_pair", 2, $1, $2); }
    ;

statement
    : expression_opt ';'
	{ $$ = create_AST ("AST_statement_exp", 1, $1); }
    | compound_statement
	{ $$ = create_AST ("AST_statement_comp", 1, $1); }
    | "if" '(' expression ')' statement
	{ $$ = create_AST ("AST_statement_if", 2, $3, $5); }
    | "if" '(' expression ')' statement "else" statement
	{ $$ = create_AST ("AST_statement_ifelse", 3, $3, $5, $7); }
    | "while" '(' expression ')' statement
	{ $$ = create_AST ("AST_statement_while", 2, $3, $5); }
    | "goto" identifier ';'
	{ $$ = create_AST ("AST_statement_goto", 1, $2); }
    | identifier ':' statement
	{ $$ = create_AST ("AST_statement_label", 2, $1, $3);
	  sym_entry ($$); }
    | "return" expression_opt ';'
	{ $$ = create_AST ("AST_statement_return", 1, $2); }
    ;

compound_statement
    : '{' 
        { sym_begin_block (); }
      declaration_list
        { sym_middle_block (); }
      statement_list '}'
	{ $$ = create_AST ("AST_compound_statement", 2, $3, $5);
	  sym_end_block ($$); }
    ;

expression_opt
    : /* expression */
	{ $$ = create_AST ("AST_expression_opt_null", 0); }
    | expression
        { $$ = create_AST ("AST_expression_opt_single", 1, $1); }
    ;

expression
    : identifier
        { struct Symbol *sym;
          $$ = create_AST ("AST_expression_id", 1, $1);
          sym = sym_check_var_decl ($$);
	  $$->type = sym->type; }
    | INTEGER_CONSTANT
	{ $$ = create_AST ("AST_expression_long", 1, atol (yytext));
            $$->type = types.t_long; }
    | CHARACTER_CONSTANT
	{ 
          char c;
          if (yytext [1] == '\\') { /* escape sequence */
              switch (yytext [2]) {
              case 'n': c = '\n';   break;
              default:  assert (0); break;
              }
          } else { /* normal character */
              c = yytext [1];
          }
          $$ = create_AST ("AST_expression_char", 1, c);
	  $$->type = types.t_long; }
    | STRING
	{ $$ = create_AST ("AST_expression_string", 1, str_buf);
	  $$->type = types.t_char_p; string_entry ($$->u.id); }
    | expression '='  expression
	{ $$ = create_AST ("AST_expression_assign", 2, $1, $3);
	  $$->type = $1->type; }
    | expression "||" expression
	{ $$ = create_AST ("AST_expression_lor",    2, $1, $3);
	  $$->type = types.t_long; }
    | expression "&&" expression
	{ $$ = create_AST ("AST_expression_land",   2, $1, $3);
	  $$->type = types.t_long; }
    | expression "==" expression
	{ $$ = create_AST ("AST_expression_eq",     2, $1, $3);
	  $$->type = types.t_long; }
    | expression '<'  expression
	{ $$ = create_AST ("AST_expression_less",   2, $1, $3);
	  $$->type = types.t_long; }
    | expression '+'  expression
	{ $$ = create_AST ("AST_expression_add",    2, $1, $3);
	  $$->type = type_add ($1->type, $3->type); }
    | expression '-'  expression
	{ $$ = create_AST ("AST_expression_sub",    2, $1, $3);
	  $$->type = type_sub ($1->type, $3->type); }
    | expression '*'  expression
	{ $$ = create_AST ("AST_expression_mul",    2, $1, $3);
	  $$->type = types.t_long; }
    | expression '/'  expression
	{ $$ = create_AST ("AST_expression_div",    2, $1, $3);
	  $$->type = types.t_long; }
    | unary_operator  expression %prec UNARY_OP
	{ $$ = create_AST ("AST_expression_unary",  2, $1, $2);
	  $$->type = type_uop ($1, $2->type); }
    | expression '(' argument_expression_list ')'
	{ $$ = create_AST ("AST_expression_funcall1", 2, $1, $3);
	  $$->type = $1->type->u.t_function.ret_type; }
    | expression '(' ')'
	{ $$ = create_AST ("AST_expression_funcall2", 1, $1);
	  $$->type = $1->type->u.t_function.ret_type; }
    | '(' expression ')'
	{ $$ = create_AST ("AST_expression_paren", 1, $2);
	  $$->type = $2->type; }
    ;

unary_operator
    : '&' { $$ = create_AST ("AST_unary_operator_address",  0); }
    | '*' { $$ = create_AST ("AST_unary_operator_deref",    0); }
    | '+' { $$ = create_AST ("AST_unary_operator_plus",     0); }
    | '-' { $$ = create_AST ("AST_unary_operator_minus",    0); }
    | '!' { $$ = create_AST ("AST_unary_operator_negative", 0); }
    ;

argument_expression_list
    : expression    	      
	{ $$ = create_AST ("AST_argument_expression_list_single", 1, $1);
            // $$->u.arg_size = ROUNDUP_LONG ($1->type->size); // xxx
        }
    | argument_expression_list ',' expression
	{ $$ = create_AST ("AST_argument_expression_list_pair", 2, $1, $3);
            // $$->u.arg_size = $1->u.arg_size + ROUNDUP_LONG ($3->type->size); // xxx
        }
    ;

identifier
    : IDENTIFIER { $$ = create_AST ("AST_IDENTIFIER", 1, yytext); }
    ;
%%
/* ---------------------------------------------------------------------- */
static void
init (int argc, char *argv [])
{
    /* handling command line arguments */
    if (argc < 2) {
	fprintf (stderr, "Usage: %s filename\n", argv [0]);
	exit (1);
    }
    filename = argv [1];
    yyin = fopen (filename, "r");
    if (yyin == NULL) {
	fprintf (stderr, "Cannot open file: %s\n", argv [1]);
	exit (1);
    }

    /* initialization for types */
    types.t_void   = create_prim_type (PRIM_TYPE_VOID);
    types.t_char   = create_prim_type (PRIM_TYPE_CHAR);
    types.t_int    = create_prim_type (PRIM_TYPE_INT);
    types.t_long   = create_prim_type (PRIM_TYPE_LONG);
    types.t_char_p = create_pointer_type (types.t_char);

    xcc_out = stdout;
}
/* --------------------------------------------------------------------- */
int
yyerror (const char *mesg)
{
    fprintf (stderr, "yyerror: %d: %s around `%s'\n",
	     lineno, mesg, yytext);
    exit (1);
}

int
main (int argc, char *argv [])
{
    init (argc, argv);
    yyparse (); /* semantic analysis is also done in actions */
#ifdef XCC_VIS
    xcc_vis_main (ast_root, 0);
#endif
    codegen ();
    return 0;
}

int yywrap (void) {
    return 1;
}
/* --------------------------------------------------------------------- */
