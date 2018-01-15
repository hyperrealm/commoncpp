/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2011  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
 */

#ifndef __commoncpp_stacktrace_h
#define __commoncpp_stacktrace_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STACKTRACE_FULL_PATHS    0x01
#define STACKTRACE_INLINE_FUNCS  0x02

extern void stacktrace_init();

extern void stacktrace_print(FILE *stream, unsigned int max_frames, int flags);

extern void stacktrace_cleanup();

#ifdef __cplusplus
}
#endif

#endif // __commoncpp_stacktrace_h
