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

#include "commonc++/Pipe.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#include <fcntl.h>
#else
#include "commonc++/Windows.h++"
#endif

namespace ccxx {

/*
 */

Pipe::Pipe()
{
  _fd[0] = _fd[1] = CCXX_INVALID_FILE_HANDLE;
}

/*
 */

Pipe::~Pipe()
{
  close();
}

/*
 */

void Pipe::open()
{
  if(isOpen())
    return;

#ifdef CCXX_OS_WINDOWS

  BOOL r = ::CreatePipe(&_fd[0], &_fd[1], NULL, 0);
  if (r != TRUE)
      throw IOException(System::getErrorString("CreatePipe"));

#else

  int r = ::pipe(_fd);
  if(r != 0)
    throw IOException(System::getErrorString());

#endif
}

/*
 */

void Pipe::close()
{
  if(!isOpen())
    return;

#ifdef CCXX_OS_WINDOWS

  ::CloseHandle(_fd[0]);
  ::CloseHandle(_fd[1]);

#else

  ::close(_fd[0]);
  ::close(_fd[1]);

#endif

  _fd[0] = _fd[1] = CCXX_INVALID_FILE_HANDLE;
}

/*
 */

bool Pipe::isOpen() const
{
  return(_fd[0] != CCXX_INVALID_FILE_HANDLE);
}

/*
 */

void Pipe::setBlocking(bool blocking)
{
#ifdef CCXX_OS_WINDOWS

  if(!blocking)
    throw IOException("Windows anonymous pipes are always blocking");

#else

  for(int i = 0; i < 2; ++i)
  {
    long flags = ::fcntl(_fd[i], F_GETFL);
    flags |= O_NONBLOCK;
    int r = ::fcntl(_fd[i], F_SETFL, flags);
    if(r != 0)
      throw IOException(System::getErrorString());
  }

#endif
}

/*
 */

ssize_t Pipe::read(byte_t* buf, size_t count)
{
#ifdef CCXX_OS_WINDOWS

  DWORD numRead = 0;
  if(::ReadFile(_fd[0], static_cast<LPVOID>(buf), static_cast<DWORD>(count),
                &numRead, NULL) != TRUE)
    return(-1);

  return(static_cast<ssize_t>(numRead));

#else

  return(::read(_fd[0], buf, count));

#endif
}

/*
 */

ssize_t Pipe::write(const byte_t* buf, size_t count)
{
#ifdef CCXX_OS_WINDOWS

  DWORD numWritten = 0;
  if(::WriteFile(_fd[1], static_cast<LPVOID>(buf), static_cast<DWORD>(count),
                 &numWritten, NULL) != TRUE)
    return(-1);

  return(static_cast<ssize_t>(numWritten));

#else

  return(::write(_fd[1], buf, count));

#endif
}


} // namespace ccxx
