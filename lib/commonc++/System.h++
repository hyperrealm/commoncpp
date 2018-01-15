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

#ifndef __ccxx_System_hxx
#define __ccxx_System_hxx

#include <commonc++/Common.h++>
#include <commonc++/Mutex.h++>
#include <commonc++/SystemException.h++>
#include <commonc++/String.h++>

#include <time.h>
#include <memory>

namespace ccxx {

/**
 * Various system-level utility functions.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API System
{
 public:

  /** Get the system time, in seconds since the epoch. */
  static time_s_t currentTime();

  /** Get the system time, in milliseconds since the epoch. */
  static time_ms_t currentTimeMillis();

  /**
   * Set the system time. On most platforms, superuser or administrator
   * privileges are required to set the system time.
   *
   * @param time The new time.
   * @throw SystemException If an error occurs.
   */
  static void setSystemTime(const time_ms_t& time);

  /**
   * Get the value of an environment variable.
   *
   * @param name The name of the variable.
   * @return The value of the variable, or <b>String::null</b> if it is
   * not defined.
   */
  static String getEnvVariable(const String& name);

  /**
   * Set the value of an environment variable.
   *
   * @param name The name of the variable.
   * @param value The vaule for the variable.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool setEnvVariable(const String& name, const String& value);

  /**
   * Delete (unset) an environment variable.
   *
   * @param name The name of the variable.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool delEnvVariable(const String& name);

  /** Get the username of the current user. */
  static String getUserName();

  /** Get the name of the host OS. */
  static String getOSName();

  /** Get the version of the host OS. */
  static String getOSVersion();

  /** Get the name of the host system architecture. */
  static String getArchitecture();

  /** Get the host name. */
  static String getHostName();

  /** Get the current user's home directory. */
  static String getHomeDir();

  /** Get the system temporary directory. */
  static String getTempDir();

  /** @cond INTERNAL */

  static inline void lockGlobalMutex()
  { _globalLock.lock(); }

  static inline void unlockGlobalMutex()
  { _globalLock.unlock(); }

  /** @endcond */

  /**
   * Get the system error message string for the most recent error.
   *
   * @param text Optional text to prepend to the resulting message.
   * @return The error message.
   */
  static String getErrorString(const String& text = String::null);

  /** Get the system page size. */
  static size_t getPageSize();

  /** Round a value up to a multiple of the system page size. */
  static void roundToPageSize(size_t& size);

  /** Print a stack trace to standard error. */
  static void printStackTrace(uint_t maxFrames = 20);

  /** Test if the host OS is a POSIX OS. */
  static bool isPOSIX();

  /** Test if the host OS is Mac OS X. */
  static bool isMacOSX();

  /** Test if the host OS is Microsoft Windows. */
  static bool isWindows();

 private:

  static void _getSystemInfo();
  static Mutex _globalLock;

  static String _username;
  static String _osname;
  static String _osver;
  static String _arch;
  static String _hostname;
  static String _homedir;
  static String _tempdir;
  static bool _inited;

  CCXX_COPY_DECLS(System);
};

} // namespace ccxx

#endif // __ccxx_System_hxx
