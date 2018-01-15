/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2014  Mark A Lindner

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

#ifndef __ccxx_Private_hxx
#define __ccxx_Private_hxx

#include <commonc++/Common.h++>

#ifdef CCXX_OS_WINDOWS

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif

#define SOCKET_EINTR WSAEINTR
#define SOCKET_errno WSAGetLastError()

#define ssize_t int

#define EWOULDBLOCK  WSAEWOULDBLOCK
#define EINPROGRESS  WSAEINPROGRESS
#define ECONNABORTED WSAECONNABORTED
#define ECONNRESET   WSAECONNRESET
#define ECONNREFUSED WSAECONNREFUSED
#define EADDRINUSE   WSAEADDRINUSE
#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif

#define MSG_NOSIGNAL 0

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

typedef u_long in_addr_t;

#else  // ! CCXX_OS_WINDOWS

#define SOCKET_EINTR EINTR
#define SOCKET_errno errno

#endif // CCXX_OS_WINDOWS

#endif // __ccxx_Private_hxx
