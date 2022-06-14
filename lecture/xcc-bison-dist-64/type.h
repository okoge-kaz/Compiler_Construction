/* XCC: Experimental C-subset Compiler.
  Copyright (c) 2002-2016, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: type.h,v 1.1 2016/05/19 04:18:57 gondow Exp gondow $ */ 
#ifndef XCC_TYPE_H
#define XCC_TYPE_H
/* ---------------------------------------------------------------------- */
enum PrimType {
    PRIM_TYPE_VOID,
    PRIM_TYPE_CHAR,
    PRIM_TYPE_INT,
    PRIM_TYPE_LONG,
};

enum TypeKind {
    TYPE_KIND_PRIM,         /* primitive type */
    TYPE_KIND_FUNCTION,     /* function type */
    TYPE_KIND_POINTER       /* pointer type */
};

struct Type {
    enum TypeKind   kind;
    int             size;    /* total size of this type */
    char            *id;     /* NULL if no identifier is associated */
    
    union {
	struct { enum PrimType  ptype; } t_prim;
    	struct { struct Type    *type; } t_pointer;
    	struct {
	    struct Type *ret_type;
	    int      	arg_num;
	    char        **arg_name;     /* array of argument names */
	    struct Type **arg_type;     /* array of argument type pointers */
    	} t_function;
    } u;
};

struct Types {
    struct Type *t_void;
    struct Type *t_char;
    struct Type *t_int;
    struct Type *t_long;
    struct Type *t_char_p;
};
/* ---------------------------------------------------------------------- */
struct Type *create_prim_type     (enum PrimType ptype);
struct Type *create_pointer_type  (struct Type *pointedto_type);
struct Type *type_analyze_declarator (struct AST *ast_decr, struct Type *type);
struct Type *type_add (struct Type *type1, struct Type *type2);
struct Type *type_sub (struct Type *type1, struct Type *type2);
struct Type *type_uop (struct AST *ast, struct Type *type1);
void type_dump (struct Type *type);
extern struct Types types;
/* ---------------------------------------------------------------------- */
#endif /* XCC_TYPE_H */
