/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2012  Mark A Lindner

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

#ifndef __ccxx_cbits_h
#define __ccxx_cbits_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#include <inttypes.h>
#endif

extern __int32 rand_r(unsigned __int32 *seed);

extern char *strtok_r(char *s, const char *delim, char **save_ptr);

#ifdef __cplusplus
}
#endif

#ifdef RAND_MAX
#undef RAND_MAX
#endif
#define RAND_MAX 0x7FFFFFF

#define strcasecmp _stricmp
#define wcscasecmp _wcsicmp
#define strncasecmp _strnicmp
#define wcsncasecmp _wcsnicmp

#ifndef __MINGW32__
#define strtoll  _strtoi64
#define wcstoll  _wcstoi64
#define strtoull _strtoui64
#define wcstoull _wcstoui64
#define snprintf _snprintf
#define snwprintf _snwprintf
#define vsnprintf _vsnprintf
#define vsnwprintf _vsnwprintf
#endif // __MINGW32__

#define isatty _isatty

#ifndef STDIN_FILENO
#define STDIN_FILENO _fileno(stdin)
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO _fileno(stdout)
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO _fileno(stderr)
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#ifndef INADDR_NONE
#define INADDR_NONE ((in_addr_t)-1)
#endif

#define ssize_t int

#ifndef PIPE_BUF
#define PIPE_BUF 4096
#endif

#endif // __ccxx_cbits_h
