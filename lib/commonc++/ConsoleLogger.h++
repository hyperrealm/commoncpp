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

#ifndef __ccxx_ConsoleLogger_hxx
#define __ccxx_ConsoleLogger_hxx

#include <commonc++/Common.h++>
#include <commonc++/Console.h++>
#include <commonc++/Logger.h++>

namespace ccxx {

/**
 * A logger that writes to the console (the standard error stream).
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ConsoleLogger : public Logger
{
 public:

  /**
   * Construct a new ConsoleLogger with the given log message format.
   *
   * @param format The log message format.
   */
  ConsoleLogger(const String& format = "[%d %t] %@%m%.");

  /** Destructor. */
  ~ConsoleLogger();

 protected:

  /** Write a preformatted log message to the console. */
  virtual bool write(CharBuffer& buffer);

 private:

#ifdef CCXX_OS_WINDOWS
  void writeANSIString(const char* buf, size_t buflen);
  void processANSI(char cmd, uint_t argc, uint_t* argv);

  Console _console;
#endif
};

} // namespace ccxx

#endif // __ccxx_ConsoleLogger_hxx
