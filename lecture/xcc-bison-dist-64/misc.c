/* XCC: Experimental C-subset compiler.
  Copyright (c) 2002-2016, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: misc.c,v 1.1 2016/05/19 04:10:05 gondow Exp gondow $ */ 
/* --------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>  /* for strcpy */
#include <assert.h>  /* for assert */
#include <stdlib.h>  /* for malloc */
#include "misc.h"
/* --------------------------------------------------------------------- */
/* --------------------------------------------------------------------- */
void *
emalloc (size_t nbytes)
{ 
    void *p = malloc (nbytes);
    if (p == NULL) perror ("malloc");
    return p;
}

void
efree (void *ptr)
{
    free (ptr);
}
/* ---------------------------------------------------------------------- */
