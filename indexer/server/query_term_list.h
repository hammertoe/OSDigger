/**************************************************************************
 *
 * term_lists.h -- description
 * Copyright (C) 1994  Authors
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
 * $Id: query_term_list.h,v 1.1.1.1 1999/07/22 20:42:40 matt Exp $
 *
 **************************************************************************/

/*
   $Log: query_term_list.h,v $
   Revision 1.1.1.1  1999/07/22 20:42:40  matt
   Initial server code import

   * Revision 1.1  1994/10/20  03:57:08  tes
   * I have rewritten the boolean query optimiser and abstracted out the
   * components of the boolean query.
   *
 */

#ifndef QUERY_TERM_LIST_H
#define QUERY_TERM_LIST_H

#include "sysfuncs.h"

typedef struct QueryTermEntry
  {
    int Count;			/* The number of times the word occurs in the query */
    u_char *Term;		/* The query term */
    int stem_method;            /* The steming method to apply to the term : -1 is default */
  }
QueryTermEntry;

typedef struct QueryTermList
  {
    int list_size;
    int num;
    QueryTermEntry QTE[1];
  }
QueryTermList;

/* --- prototypes --- */
void ConvertQueryTermsToString (QueryTermList * query_term_list, char *str);
int AddQueryTerm (QueryTermList ** query_term_list, u_char * Term, int Count, int stem_method);
void ResetQueryTermList (QueryTermList ** tl);
void FreeQueryTermList (QueryTermList ** the_qtl);
QueryTermList *MakeQueryTermList (int n);


#endif
