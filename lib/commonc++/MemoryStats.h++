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

#ifndef __ccxx_MemoryStats_hxx
#define __ccxx_MemoryStats_hxx

#include <commonc++/SystemStats.h++>

namespace ccxx {

/**
 * %System memory utilization statistics. These statistics include the
 * total amount of physical and virtual memory and the amount of each that
 * is currently in use.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API MemoryStats : public SystemStats
{
 public:

  /** Construct a new MemoryStats object. */
  MemoryStats();

  /** Destructor. */
  ~MemoryStats();

  /** Get the total amount of physical memory, in bytes. */
  inline uint_t getPhysicalMemoryTotal() const
  { return(_physTotal); }

  /** Get the amount of free physical memory, in bytes. */
  inline uint_t getPhysicalMemoryFree() const
  { return(_physFree); }

  /** Get the amount of used physical memory, in bytes. */
  inline uint_t getPhysicalMemoryUsed() const
  { return(_physUsed); }

  /**
   * Get the total amount of virtual memory, in bytes. On Mac OS X,
   * there is no fixed-size swap partition; the amount of virtual
   * memory is limited only by disk space on the root partition, so
   * this value is always 0.
   */
  inline uint_t getVirtualMemoryTotal() const
  { return(_virtTotal); }

  /** Get the amount of free virtual memory, in bytes. */
  inline uint_t getVirtualMemoryFree() const
  { return(_virtFree); }

  /** Get the amount of used virtual memory, in bytes. */
  inline uint_t getVirtualMemoryUsed() const
  { return(_virtUsed); }

  bool collect();
  void reset();

 private:

  uint32_t _physTotal;
  uint32_t _physFree;
  uint32_t _physUsed;
  uint32_t _virtTotal;
  uint32_t _virtFree;
  uint32_t _virtUsed;

  CCXX_COPY_DECLS(MemoryStats);
};

} // namespace ccxx

#endif // __ccxx_MemoryStats_hxx
