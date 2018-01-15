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

#include "commonc++/LoadAverageStats.h++"
#include "commonc++/FilePtr.h++"

#ifdef CCXX_OS_MACOSX
#include <cstdlib>
#endif

namespace ccxx {

/*
 */

LoadAverageStats::LoadAverageStats()
{
  reset();
}

/*
 */

LoadAverageStats::~LoadAverageStats()
{
}

/*
 */

void LoadAverageStats::reset()
{
  _load1Min = _load5Min = _load15Min = 0.0;
}

/*
 */

bool LoadAverageStats::collect()
{
#if defined(CCXX_OS_WINDOWS)

  // No implementation is available for Windows.
  return(false);

#elif defined(CCXX_OS_MACOSX)

  double loadavg[3];
  int r = ::getloadavg(loadavg, CCXX_LENGTHOF(loadavg));

  if(r < 1)
    return(false);

  if(r > 0)
  {
    _load1Min = loadavg[0];
    if(r > 1)
    {
      _load5Min = loadavg[1];
      if(r > 2)
        _load15Min = loadavg[2];
    }
  }
  return(true);

#elif defined(CCXX_OS_POSIX)

  FilePtr fp("/proc/loadavg", "r");
  if(!fp)
    return(false);

  return(::fscanf(fp, "%lf %lf %lf", &_load1Min, &_load5Min, &_load15Min)
         == 3);

#else

#warning "No implementation available"

  return(false);

#endif
}


} // namespace ccxx
