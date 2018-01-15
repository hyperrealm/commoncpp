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

#ifndef __ccxx_LoadAverageStats_hxx
#define __ccxx_LoadAverageStats_hxx

#include <commonc++/SystemStats.h++>

namespace ccxx {

/**
 * %System "load average" statistics. These statistics represent the average
 * system load over a period of time. These statistics are not available on
 * Windows.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API LoadAverageStats : public SystemStats
{
 public:

  /** Construct a new LoadAverageStats object. */
  LoadAverageStats();

  /** Destructor. */
  ~LoadAverageStats();

  /** Get the load average over the last 1 minute period. */
  inline double getLoad1Min() const
  { return(_load1Min); }

  /** Get the load average over the last 5 minute period. */
  inline double getLoad5Min() const
  { return(_load5Min); }

  /** Get the load average over the last 15 minute period. */
  inline double getLoad15Min() const
  { return(_load15Min); }

  bool collect();
  void reset();

 private:

  double _load1Min;
  double _load5Min;
  double _load15Min;

  CCXX_COPY_DECLS(LoadAverageStats);
};

} // namespace ccxx

#endif // __ccxx_LoadAverageStats_hxx
