/* No user fns here.  Pesch 15apr92. */

/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "local.h"

/* SN Systems: the runtime has no malloc, so __sinit links a static glue of
   ten more FILEs and __sfp reports when they run out; std is prototyped and
   leaves _bf._size alone.  See docs/sn_runtime.md.  */
static FILE __sf_more[10];
static struct _glue __sglue_more;

static void
std (FILE *ptr, int flags, int file, struct _reent *data)
{
  ptr->_p = 0;
  ptr->_r = 0;
  ptr->_w = 0;
  ptr->_flags = flags;
  ptr->_file = file;
  ptr->_bf._base = 0;
  ptr->_lbfsize = 0;
  ptr->_cookie = ptr;
  ptr->_read = __sread;
  ptr->_write = __swrite;
  ptr->_seek = __sseek;
  ptr->_close = __sclose;
  ptr->_data = data;
}

struct _glue *
__sfmoreglue (d, n)
     struct _reent *d;
     register int n;
{
  struct _glue *g;
  FILE *p;

  g = (struct _glue *) _malloc_r (d, sizeof (*g) + n * sizeof (FILE));
  if (g == NULL)
    return NULL;
  p = (FILE *) (g + 1);
  g->_next = NULL;
  g->_niobs = n;
  g->_iobs = p;
  memset (p, 0, n * sizeof (FILE));
  return g;
}

/*
 * Find a free FILE for fopen et al.
 */

FILE *
__sfp (d)
     struct _reent *d;
{
  FILE *fp;
  int n;
  struct _glue *g;

  if (!d->__sdidinit)
    __sinit (d);
  /* SN Systems: retail strips __sfp and keeps only this message, so the
     loop below is a reconstruction.  */
  for (g = &d->__sglue; g != NULL; g = g->_next)
    {
      for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
	if (fp->_flags == 0)
	  goto found;
    }
  _write (1, "\nfopen error:  **too many open streams**\n", 0x2a);
  d->_errno = ENOMEM;
  return NULL;

found:
  fp->_flags = 1;		/* reserve this slot; caller sets real flags */
  fp->_p = NULL;		/* no current pointer */
  fp->_w = 0;			/* nothing to read or write */
  fp->_r = 0;
  fp->_bf._base = NULL;		/* no buffer */
  fp->_bf._size = 0;
  fp->_lbfsize = 0;		/* not line buffered */
  fp->_file = -1;		/* no file */
  /* fp->_cookie = <any>; */	/* caller sets cookie, _read/_write etc */
  fp->_ub._base = NULL;		/* no ungetc buffer */
  fp->_ub._size = 0;
  fp->_lb._base = NULL;		/* no line buffer */
  fp->_lb._size = 0;
  fp->_data = d;
  return fp;
}

/*
 * __sinit() is called whenever stdio's internal variables must be set up.
 * SN Systems: retail places __sinit before _cleanup_r.
 */

void
__sinit (s)
     struct _reent *s;
{
  /* make sure we clean up on exit */
  s->__cleanup = _cleanup_r;	/* conservative */
  s->__sdidinit = 1;

  std (s->__sf + 0, __SRD, 0, s);
  std (s->__sf + 1, __SWR | __SLBF, 1, s);
  std (s->__sf + 2, __SWR | __SNBF, 2, s);

  s->__sglue._next = &__sglue_more;
  s->__sglue._niobs = 3;
  s->__sglue._iobs = &s->__sf[0];

  /* SN Systems: see __sf_more.  */
  __sglue_more._next = NULL;
  __sglue_more._niobs = 10;
  __sglue_more._iobs = __sf_more;
}

/*
 * exit() calls _cleanup() through *__cleanup, set whenever we
 * open or buffer a file.  This chicanery is done so that programs
 * that do not use stdio need not link it all in.
 *
 * The name `_cleanup' is, alas, fairly well known outside stdio.
 */

void
_cleanup_r (ptr)
     struct _reent *ptr;
{
  /* (void) _fwalk(fclose); */
  (void) _fwalk (ptr, fflush);	/* `cheating' */
}

#ifndef _REENT_ONLY
void
_cleanup ()
{
  _cleanup_r (_REENT);
}
#endif
