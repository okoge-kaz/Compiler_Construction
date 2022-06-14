/* XCC: Experimental C-subset Compiler.
  Copyright (c) 2002-2016, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: symbol.h,v 1.1 2016/05/19 04:24:18 gondow Exp gondow $
*/ 
#ifndef XCC_SYMBOL_H
#define XCC_SYMBOL_H
/* --------------------------------------------------------------------- */
/* 
   ANSI C has 4 name spaces for identifiers: labels, tags, fields, 
   and the others including variable and typedef names.
   NameSpace here is much simplified and different from ANSI C's.
*/
enum NameSpace {
    NS_GLOBAL, NS_LOCAL, NS_ARG, NS_LABEL
};

struct Symbol {
    char             *name;    /* symbol name */
    struct Type      *type;    /* symbol type */
    struct AST       *ast;     /* AST where the symbol being declared */
    int              offset;   /* for local vars and arguments */
    enum NameSpace   name_space;
    struct Symbol    *next;    /* pointer to next entry */
};

struct String {
    char             *data;    /* content of string literals */
    char             *label;   /* uniq labels for string literals */
    struct String    *next;
};

enum { MAX_BLOCK_DEPTH = 128 };
struct SymTable {
    struct Symbol    *global;
    struct Symbol    *arg;
    struct Symbol    *label;
    struct String    *string;
    int              local_index;
    struct Symbol    *local [MAX_BLOCK_DEPTH];
};
extern int max_arg_size;
extern struct SymTable sym_table;
/* --------------------------------------------------------------------- */
struct Symbol *sym_lookup (char *name);
struct Symbol *sym_lookup_label  (char *name);
struct String *string_lookup (char *data);
void sym_entry (struct AST *ast);
void string_entry (char *data);
void sym_backpatch (struct AST *ast, struct Type *type);
void sym_begin_function (void);
void sym_end_function (struct AST *ast);
void sym_begin_block (void);
void sym_middle_block (void);
void sym_end_block (struct AST *ast);
void sym_entry_param (struct Type *func_type);
struct Symbol *sym_check_var_decl (struct AST *ast);
void sym_table_dump (void);
/* --------------------------------------------------------------------- */
#endif /* XCC_SYMBOL_H */
