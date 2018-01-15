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

#include "commonc++/Random.h++"
#include "commonc++/Process.h++"
#include "commonc++/System.h++"
#include "commonc++/Thread.h++"

#include <cstdlib>
#include <fcntl.h>

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#endif

#ifdef CCXX_OS_WINDOWS
#include "cbits.h"
#endif

#ifdef CCXX_OS_ANDROID
#define RANDOM_DEVICE "/dev/random"
#else
#define RANDOM_DEVICE "/dev/urandom"
#endif

#ifndef HAVE_RAND_R

/* This algorithm is mentioned in the ISO C standard, here extended
   for 32 bits. */
static int rand_r (unsigned int *seed)
{
  unsigned int next = *seed;
  int result;
  next *= 1103515245;
  next += 12345;
  result = (unsigned int) (next / 65536) % 2048;
  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;
  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;
  *seed = next;
  return result;
}

#endif // HAVE_RAND_R

namespace ccxx {

/*
 */

Random::Random()
  : _state(static_cast<uint32_t>(System::currentTime())
           ^ ((uint32_t)(Process::currentProcessID())
              + (uint32_t)(uint64_t)(Thread::currentThreadID())))
{
}

Random::Random(uint32_t seed)
  : _state(seed)
{
}

/*
 */

Random::~Random()
{
}

/*
 */

uint32_t Random::nextInt(uint32_t range)
{
  // TODO: fix this mess
#ifdef __MINGW32__
  return(static_cast<uint32_t>(rand_r((long unsigned int *)&_state) / (RAND_MAX + 1.0) * range));
#else
  return(static_cast<uint32_t>(rand_r(&_state) / (RAND_MAX + 1.0) * range));
#endif
}

/*
 */

double Random::nextDouble()
{
#ifdef __MINGW32__
  uint32_t r = static_cast<uint32_t>(rand_r((long unsigned int *)&_state));
#else
  uint32_t r = static_cast<uint32_t>(rand_r(&_state));
#endif

  return(static_cast<double>(r) / static_cast<double>(RAND_MAX));
}

/*
 */

bool Random::getBytes(byte_t* buf, size_t len)
{
#if defined(CCXX_OS_WINDOWS) && !defined(CCXX_OS_WINDOWS_GNU)

  HCRYPTPROV prov;

  if(! ::CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL,
                             (CRYPT_VERIFYCONTEXT | CRYPT_SILENT)))
    return(false);

  bool ok = (::CryptGenRandom(prov, static_cast<DWORD>(len), buf));
  ::CryptReleaseContext(prov, 0);

  return(ok);

#else

  int fd = ::open(RANDOM_DEVICE, O_RDONLY);
  if(fd >= 0)
  {
    int r = ::read(fd, buf, len);
    ::close(fd);

    return(r == static_cast<int>(len));
  }
  else
  {
    byte_t *end = buf + len;
    uint32_t r;

    for(byte_t *p = buf; p < end; ++p)
    {
      r = nextInt(256);
      *p = (r & 0xFF);
    }

    return(true);
  }

#endif
}


} // namespace ccxx
