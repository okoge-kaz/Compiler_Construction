/* XCC: Experimental C-subset Compiler.
  Copyright (c) 2002-2016, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: misc.h,v 1.1 2016/05/19 04:09:35 gondow Exp gondow $ */ 
#ifndef XCC_MISC_H
#define XCC_MISC_H
/* --------------------------------------------------------------------- */
// for x86 architecture
#define STACK_ALIGNMENT    (16)
#define ROUNDUP(p, n)      ((((unsigned int)p)+((n)-1)) & ~((n)-1))
#define ROUNDUP_LONG(p)    ROUNDUP((p), 8)
#define ROUNDUP_INT(p)     ROUNDUP((p), 4)
#define ROUNDUP_ANY(p)     ROUNDUP((p), 4)
#define ROUNDUP_STACK(p)   ROUNDUP((p), STACK_ALIGNMENT)
/* --------------------------------------------------------------------- */
void *emalloc (size_t nbytes);
void efree (void *ptr);
/* --------------------------------------------------------------------- */
#endif /* XCC_MISC_H */
