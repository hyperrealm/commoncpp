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

#ifndef __ccxx_Log_hxx
#define __ccxx_Log_hxx

#include <commonc++/Common.h++>
#include <commonc++/ConsoleLogger.h++>
#include <commonc++/CriticalSection.h++>
#include <commonc++/FileLogger.h++>
#include <commonc++/Logger.h++>
#include <commonc++/String.h++>

#include <cstdarg>

namespace ccxx {

/** @cond INTERNAL */

class COMMONCPP_API LogFunctor
{
 public:

  LogFunctor(const char *file, int line, LogLevel level = LogDebug,
             bool nodebug = false);
  ~LogFunctor();

  void operator()(const char *fmt, ...);
  void operator()(LogLevel level, const char *fmt, ...);

 private:

  const char *_file;
  int _line;
  LogLevel _level;
  bool _nodebug;
};
/** @endcond */

/**
 * Logging routines. Also see the macros <b>Log_debug()</b>,
 * <b>Log_info()</b>, <b>Log_warning()</b>, and <b>Log_error()</b>.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Log
{
  friend class LogFunctor;

 public:

  /**
   * Set the log format to be used when logging to a file.
   *
   * @param format The log format.
   */
  static void setFileLogFormat(const String& format);

  /**
   * Set the log format to be used when logging to the console.
   *
   * @param format The log format.
   */
  static void setConsoleLogFormat(const String& format);

  /**
   * Enable or disable logging to the console.
   *
   * @param flag <b>true</b> if logging to the console should be enabled,
   * <b>false</b> otherwise.
   */
  inline static void setUseConsoleLog(bool flag)
  { _useConsoleLog = flag; }

  /**
   * Set the FileLogger object to use for logging to a file. Any current
   * FileLogger object will be deleted. The passed in object must be
   * heap allocated, and should never be freed by the caller.
   */
  static void setFileLogger(FileLogger* logger);

  /** Get the FileLogger currently in use for logging to a file. */
  inline static FileLogger* getFileLogger()
  { return(_fileLog); }

  /**
   * Set the ConsoleLogger object to use for logging to the
   * console. Any current ConsoleLogger object will be deleted. The
   * passed in object must be heap allocated, and should never be
   * freed by the caller.
   */
  static void setConsoleLogger(ConsoleLogger* logger);

  /** Get the ConsoleLogger currently in use for logging to the console. */
  inline static ConsoleLogger* getConsoleLogger()
  { return(_consoleLog); }

  /**
   * Enable or disable logging to a file.
   *
   * @param flag <b>true</b> if logging to a file should be enabled,
   * <b>false</b> otherwise.
   */
  inline static void setUseFileLog(bool flag)
  { _useFileLog = flag; }

  /**
   * Set the directory and base name for log files.
   *
   * @param dir The directory in which the log file(s) will be created.
   * @param name The base name for the log file(s).
   */
  static void setLogFile(const String& dir, const String& name);

  /**
   * Set the maximum size for log files.
   *
   * @param maxLogSize The maximum log file size, in kilobytes.
   */
  static void setLogFileMaxSize(size_t maxLogSize);

  /**
   * Set the rotation log file count.
   *
   * @param rotateCount The maximum number of backlog files to create. The
   * value must range from 0 to 9, with 0 indicating that log rotation is
   * disabled.
   */
  static void setLogFileRotateCount(uint_t rotateCount);

  /** Write a log message to the log file, if file logging is enabled. */
  static void vlogFile(LogLevel level, const char* file, int line,
                       const char* message, va_list args);

  /** Write a log message to the console, if console logging is enabled. */
  static void vlogConsole(LogLevel level, const char* file, int line,
                          const char* message, va_list args);

  /** @cond INTERNAL */

  /**
   * For internal use only. The macros <b>Log_debug()</b>,
   * <b>Log_info()</b>, <b>Log_warning()</b>, and <b>Log_error()</b>
   * should be used to log messages; they automatically capture the source
   * file and line number of the log statement.
   */
  static void log(LogLevel level, const char* file, int line,
                  const char* message, ...) ___PRINTF(4, 5);

  /**
   * For internal use only. The Log_assert() macro should be used to
   * log assertions.
   */
  static bool assert_(const char* file, int line, const char* expr);

  /* temporary hackery */
  static void disableCleanup();

  /** @endcond */

 private:

  static CriticalSection _lock;
  static ConsoleLogger* _consoleLog;
  static FileLogger* _fileLog;
  static bool _useConsoleLog;
  static bool _useFileLog;

  Log(); // not supported
  CCXX_COPY_DECLS(Log);
};

/** Log an assertion. */
#define Log_assert(EXPR)                                                \
  (void)((EXPR)|| ccxx::Log::assert_(__FILE__, __LINE__, #EXPR))

#if (defined CCXX_OS_WINDOWS) && (defined _MSC_VER) && (_MSC_VER < 1400)

// no variadic macros; emulate with LogFunctor

#if (defined DEBUG) || (defined DEBUG_LOG_MESSAGES)

/**
 * Log a debug message. Compiled in only if DEBUG or DEBUG_LOG_MESSAGES is
 * defined.
 */
#define Log_debug                                       \
  ccxx::LogFunctor(__FILE__, __LINE__, ccxx::LogDebug)

#else // ! (DEBUG || DEBUG_LOG_MESSAGES)

#define Log_debug                                               \
  ccxx::LogFunctor(__FILE__, __LINE__, ccxx::LogDebug, true)

#endif // DEBUG || DEBUG_LOG_MESSAGES

/** Log an informational message. */
#define Log_info                                        \
  ccxx::LogFunctor(__FILE__, __LINE__, ccxx::LogInfo)

/** Log a warning message. */
#define Log_warning                                             \
  ccxx::LogFunctor(__FILE__, __LINE__, ccxx::LogWarning)

/** Log an error message. */
#define Log_error                                       \
  ccxx::LogFunctor(__FILE__, __LINE__, ccxx::LogError)

#elif (defined __GNUC__)

// gcc-style variadic macros (for compatibility with older versions of GCC)

#if (defined DEBUG) || (defined DEBUG_LOG_MESSAGES)

/**
 * Log a debug message. Compiled in only if DEBUG or DEBUG_LOG_MESSAGES is
 * defined.
 */
#define Log_debug(M, args...)                                           \
  ccxx::Log::log(ccxx::LogDebug, __FILE__, __LINE__, M, ## args)

#else // ! (DEBUG || DEBUG_LOG_MESSAGES)

#define Log_debug(M, args...)

#endif // DEBUG || DEBUG_LOG_MESSAGES

/** Log an informational message. */
#define Log_info(M, args...)                                    \
  ccxx::Log::log(ccxx::LogInfo, __FILE__, __LINE__, M, ## args)

/** Log a warning message. */
#define Log_warning(M, args...)                                         \
  ccxx::Log::log(ccxx::LogWarning, __FILE__, __LINE__, M, ## args)

/** Log an error message. */
#define Log_error(M, args...)                                           \
  ccxx::Log::log(ccxx::LogError, __FILE__, __LINE__,  M, ## args)

#else // assume ANSI compiler with support for C99 variadic macros

#if (defined DEBUG) || defined(DEBUG_LOG_MESSAGES)

/**
 * Log a debug message. Compiled in only if DEBUG or DEBUG_LOG_MESSAGES is
 * defined.
 */
#define Log_debug(M, ...)                                               \
  ccxx::Log::log(ccxx::LogDebug, __FILE__, __LINE__, M, __VA_ARGS__)

#else // ! (DEBUG || DEBUG_LOG_MESSAGES)

#define Log_debug(M, ...)

#endif // DEBUG || DEBUG_LOG_MESSAGES

/** Log an informational message. */
#define Log_info(M, ...)                                                \
  ccxx::Log::log(ccxx::LogInfo, __FILE__, __LINE__, M, __VA_ARGS__)

/** Log a warning message. */
#define Log_warning(M, ...)                                             \
  ccxx::Log::log(ccxx::LogWarning, __FILE__, __LINE__, M, __VA_ARGS__)

/** Log an error message. */
#define Log_error(M, ...)                                               \
  ccxx::Log::log(ccxx::LogError, __FILE__, __LINE__,  M, __VA_ARGS__)

#endif // variadic checks

} // namespace ccxx

#endif // __ccxx_Log_hxx
