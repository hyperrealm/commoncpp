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

#ifndef __ccxx_ProgressTracker_hxx
#define __ccxx_ProgressTracker_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * An interface for tracking the progress of a lengthy
 * operation. See ProgressTrackerDelegate for an implementation that
 * delegates to a method in an object.
 *
 * @author Mark Lindner
 */
class ProgressTracker
{
 public:

  /** Destructor. */
  virtual ~ProgressTracker() { }

  /**
   * Notify the tracker of an update in the progress of a task.
   *
   * @param percent The percentage of the task that has been completed.
   * A negative value indicates indeterminate progress.
   * @param status An optional status message.
   */
  virtual void updateProgress(int percent,
                              const String& status = String::null) = 0;
};

/**
 * An implementation of ProgressTracker that delegates to a method in an
 * object.
 *
 * @author Mark Lindner
 */
template<class T> class ProgressTrackerDelegate : public ProgressTracker
{
 public:

  /**
   * Construct a new ProgressTrackerDelegate for an object.
   *
   * @param object The object.
   * @param func A member function (method) in the object which will serve
   * as the progress tracker.
   */
  ProgressTrackerDelegate(T* object, void (T::* func)(int, const String&))
    : _object(object), _func(func)
  {
  }

 protected:

  inline void updateProgress(int percent,
                             const String& status = String::null)
  { (_object->*_func)(percent, status); }

 private:

  T* _object;
  void (T::* _func)(int, const String&);
};

} // namespace ccxx

#endif // __ccxx_ProgressTracker_hxx
