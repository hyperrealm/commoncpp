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

#ifndef __ccxx_SystemStats_hxx
#define __ccxx_SystemStats_hxx

#include <commonc++/Common.h++>
#include <commonc++/OutOfBoundsException.h++>
#include <commonc++/SystemException.h++>

namespace ccxx {

/**
 * A collector of system performance or resource utilization statistics.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SystemStats
{
 public:

  /** Destructor. */
  virtual ~SystemStats() { }

  /** Reset the stats object. */
  virtual void reset() = 0;

  /** Collect (update) the stats represented by this object.
   *
   * @return <b>true</b> if the stats were collected successfully,
   * <b>false</b> otherwise.
   */
  virtual bool collect() = 0;
};

} // namespace ccxx

#endif // __ccxx_SystemStats_hxx
