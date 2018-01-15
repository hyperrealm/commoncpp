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

#ifndef __ccxx_CPUStats_hxx
#define __ccxx_CPUStats_hxx

#include <commonc++/SystemStats.h++>

namespace ccxx {

/**
 * %System CPU utilization statistics.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API CPUStats : public SystemStats
{
 public:

  /** Construct a new CPUStats object. */
  CPUStats();

  /** Destructor. */
  ~CPUStats();

  /**
   * Get the percent of CPU time spent in privileged (kernel) mode.
   *
   * @param cpu The CPU number or -1 for the average across all CPUs.
   * @throw OutOfBoundsException If the value of <i>cpu</i> is invalid.
   */
  float getSystem(int cpu = -1) const;

  /**
   * Get the percent of CPU time spent in unprivileged (user) mode.
   *
   * @param cpu The CPU number or -1 for the average across all CPUs.
   * @throw OutOfBoundsException If the value of <i>cpu</i> is invalid.
   */
  float getUser(int cpu = -1) const;

  /**
   * Get the percent of CPU time spent on low-priority tasks.
   *
   * @param cpu The CPU number or -1 for the average across all CPUs.
   * @throw OutOfBoundsException If the value of <i>cpu</i> is invalid.
   */
  float getNice(int cpu = -1) const;

  /**
   * Get the percent of CPU time spent on the idle task.
   *
   * @param cpu The CPU number or -1 for the average across all CPUs.
   * @throw OutOfBoundsException If the value of <i>cpu</i> is invalid.
   */
  float getIdle(int cpu = -1) const;

  /** Get the number of CPUs in the system. */
  inline uint_t getCPUCount() const
  { return(_cpus); }

  bool collect();
  void reset();

 private:

  uint_t lookupCPUCount() const;

  struct CPUMetrics; // fwd decl

  uint_t _cpus;
  CPUMetrics* _metrics;
#ifdef CCXX_OS_WINDOWS
  HANDLE _context;
#endif

  CCXX_COPY_DECLS(CPUStats);
};

} // namespace ccxx

#endif // __ccxx_CPUStats_hxx
