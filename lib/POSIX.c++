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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/POSIX.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"
#include "commonc++/Socket.h++"

#include <cerrno>

namespace ccxx {

pthread_once_t POSIX::_once = PTHREAD_ONCE_INIT;
pthread_key_t POSIX::_threadKey;

/*
 */

void POSIX::decodePermissions(const mode_t mode, Permissions& perm)
{
  perm = (uint_t)(mode & 0777);
}

/*
 */

void POSIX::encodePermissions(const Permissions& perm, mode_t& mode)
{
  mode = (mode_t)perm.getMask();
}

/*
 */

bool POSIX::verifyIPCName(const String& name)
{
  return((name.getLength() <= 14) && (name.indexOf(File::separator, 0) < 0));
}

/*
 */

uint64_t POSIX::decodeTimevalAsDelta(struct timeval& tv)
{
  uint64_t val = static_cast<uint64_t>(tv.tv_sec) * 1000LL;
  val += static_cast<uint64_t>(tv.tv_usec) / 1000LL;

  return(val);
}

/*
 */

void POSIX::waitForIO(int fd, timespan_ms_t timeout, bool read)
{
  fd_set fds;
  struct timeval tv;
  div_t dv;
  time_ms_t end = System::currentTimeMillis() + timeout;

  for(;;)
  {
    dv = div((end - System::currentTimeMillis()), 1000);
    tv.tv_sec = dv.quot;
    tv.tv_usec = dv.rem * 1000;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    int r = ::select(FD_SETSIZE, (read ? &fds : NULL), (!read ? &fds : NULL),
                     NULL, &tv);
    if(r < 0)
    {
      if(errno == EINTR)
        continue;
      else
        throw IOException(System::getErrorString("select"));
    }
    else if(r == 0)
      throw TimeoutException();
    else
      break;
  }
}

/*
 */

void POSIX::timespecForDelta(timespan_ms_t timeout, struct timespec& tspec,
                             time_ms_t tv /* = 0 */)
{
  if(tv == 0)
    tv = System::currentTimeMillis() + timeout;

  tspec.tv_sec = tv / 1000;
  tv %= 1000;
  tspec.tv_nsec = tv * 1000000;
}

/*
 */

void POSIX::setCurrentThread(void *thread)
{
  pthread_once(&_once, &POSIX::_initThreadKey);

  pthread_setspecific(_threadKey, thread);
}

/*
 */

void *POSIX::getCurrentThread()
{
  pthread_once(&_once, &POSIX::_initThreadKey);

  return(pthread_getspecific(_threadKey));
}

/*
 */

void POSIX::_initThreadKey()
{
  pthread_key_create(&_threadKey, NULL);
}


} // namespace ccxx
