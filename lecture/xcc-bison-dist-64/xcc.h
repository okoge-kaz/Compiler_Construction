/* XCC: Experimental C-subset Compiler.
  Copyright (c) 2002-2017, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: xcc.h,v 1.1 2017/04/28 07:36:06 gondow Exp gondow $ */ 

#ifndef XCC_XCC_H
#define XCC_XCC_H
/* --------------------------------------------------------------------- */
#include <stdio.h>
#include <stddef.h> /* for size_t */
#define YYSTYPE struct AST *
/* --------------------------------------------------------------------- */
extern int   lineno;
extern char  *yytext;
extern FILE  *yyin;
extern char  str_buf [];

extern struct AST *ast_root;
extern char  *filename;
extern FILE  *xcc_out;
extern int   AST_is_local;
int yyerror (const char *mesg);
int yylex   (void);
int yywrap  (void);
/* --------------------------------------------------------------------- */
#endif /* XCC_XCC_H */
