/**************************************************************************
 *
 * memlib.c -- Malloc wrappers
 * Copyright (C) 1994  Neil Sharman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: memlib.c,v 1.1.1.1 1999/07/22 20:34:56 matt Exp $
 *
 **************************************************************************/

/*
   $Log: memlib.c,v $
   Revision 1.1.1.1  1999/07/22 20:34:56  matt
   Intitial import of MG lib



   * Revision 1.1  1994/08/22  00:24:47  tes
   * Initial placement under CVS.
   *
 */

static char *RCSID = "$Id: memlib.c,v 1.1.1.1 1999/07/22 20:34:56 matt Exp $";


#include "sysfuncs.h"
#include "memlib.h"

/* Defined as strdup is not an ANSI function */
char *
my_strdup(const char *str)
{
  char *ret_str = malloc(strlen(str)+1);
  if (ret_str) return strcpy(ret_str, str);
  else return (char*) 0;
}


Malloc_func Xmalloc = malloc;

Realloc_func Xrealloc = realloc;

Free_func Xfree = free;

Strdup_func Xstrdup = my_strdup;
