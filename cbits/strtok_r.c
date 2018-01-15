/* Reentrant string tokenizer.  Generic version.
   Copyright (C) 1991, 1996, 1997, 1998, 1999, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <string.h>

char *strtok_r(char *s, const char *delim, char **save_ptr)
{
  char *token;

  if (s == NULL)
    s = *save_ptr;

  // Scan leading delimiters.
  s += strspn(s, delim);
  if (*s == '\0')
  {
    *save_ptr = s;
    return NULL;
  }

  // Find the end of the token.
  token = s;
  s = strpbrk(token, delim);
  if(s == NULL)
    // This token finishes the string.
    *save_ptr = strchr(token, '\0');
  else
  {
    // Terminate the token and make *SAVE_PTR point past it.
    *s = '\0';
    *save_ptr = s + 1;
  }
  return(token);
}
