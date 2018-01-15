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

#include "commonc++/MemoryStats.h++"
#include "commonc++/FilePtr.h++"

#ifdef CCXX_OS_MACOSX
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach-o/arch.h>
#endif

#include <cstdlib>
#include <cstring>

namespace ccxx {

/*
 */

MemoryStats::MemoryStats()
{
  reset();
}

/*
 */

MemoryStats::~MemoryStats()
{
}

/*
 */

void MemoryStats::reset()
{
  _physTotal = _physFree = _physUsed = 0;
  _virtTotal = _virtFree = _virtUsed = 0;
}

/*
 */

bool MemoryStats::collect()
{
#if defined(CCXX_OS_WINDOWS)

  MEMORYSTATUSEX mem;
  CCXX_ZERO(mem);
  mem.dwLength = sizeof(mem);

  if(!::GlobalMemoryStatusEx(&mem))
    return(false);

  _physTotal = static_cast<uint32_t>(mem.ullTotalPhys);
  _physFree = static_cast<uint32_t>(mem.ullAvailPhys);

  _virtTotal = static_cast<uint32_t>(mem.ullTotalVirtual);
  _virtFree = static_cast<uint32_t>(mem.ullAvailVirtual);

  _physUsed = (_physTotal < _physFree ? 0 : (_physTotal - _physFree));
  _virtUsed = (_virtTotal < _virtFree ? 0 : (_virtTotal - _virtFree));

  return(true);

#elif defined(CCXX_OS_MACOSX)

  vm_size_t pageSize;
  ::host_page_size(::mach_host_self(), &pageSize);

  mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
  vm_statistics_data_t vm_stat;

  kern_return_t kret = ::host_statistics(::mach_host_self(), HOST_VM_INFO,
                                         (host_info_t)&vm_stat, &count);

  if(kret != KERN_SUCCESS)
  {
    // ::mach_error("FAILED to get host_statistics!", kret);
    return(false);
  }

  _physFree = (uint64_t)(vm_stat.free_count * pageSize);
  _physTotal = ((uint64_t)(vm_stat.free_count * pageSize)
                + (uint64_t)(vm_stat.active_count * pageSize)
                + (uint64_t)(vm_stat.inactive_count * pageSize)
                + (uint64_t)(vm_stat.wire_count * pageSize));

  // On Mac OS X there is no swap partition, so "total virtual memory"
  // is meaningless.

  _physUsed = (_physTotal < _physFree ? 0 : (_physTotal - _physFree));
  _virtUsed = (_virtTotal < _virtFree ? 0 : (_virtTotal - _virtFree));

  return(true);

#elif defined(CCXX_OS_POSIX)

  FilePtr fp("/proc/meminfo", "r");
  if(!fp)
    return(false);

  char buf[128];

  while(::fgets(buf, sizeof(buf), fp) != NULL)
  {
    if(!::strncmp(buf, "MemTotal:", 9))
    {
      _physTotal = ::atoi(buf + 9) * 1024;
    }
    else if(!::strncmp(buf, "MemFree:", 8))
    {
      _physFree = ::atoi(buf + 8) * 1024;
    }
    else if(!::strncmp(buf, "SwapTotal:", 10))
    {
      _virtTotal = ::atoi(buf + 10) * 1024;
    }
    else if(!::strncmp(buf, "SwapFree:", 9))
    {
      _virtFree = ::atoi(buf + 9) * 1024;
    }
  }

  _physUsed = (_physTotal < _physFree ? 0 : (_physTotal - _physFree));
  _virtUsed = (_virtTotal < _virtFree ? 0 : (_virtTotal - _virtFree));

  return(true);

#else

#warning "No implementation available"

  return(false);

#endif
}


} // namespace ccxx
