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

#ifndef __ccxx_Process_hxx
#define __ccxx_Process_hxx

#include <commonc++/Common.h++>
#include <commonc++/Stream.h++>
#include <commonc++/SystemException.h++>
#include <commonc++/String.h++>

#include <vector>
#include <memory>

#ifdef CCXX_OS_POSIX
#include <sys/types.h>
#include <fcntl.h>
#endif

namespace ccxx {

/**
 * The exit status of a process.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ExitStatus
{
  friend class Process;

 public:

  /** Construct a new <b>ExitStatus</b>. */
  ExitStatus() { }

  /** Destructor. */
  ~ExitStatus() { }

  /** Get the exit code from the process. */
  inline int getExitCode() const
  { return(_exitCode); }

  /**
   * Get the exit signal.
   *
   * @return The signal number that caused the process to exit, or 0 if
   * the process did not exit due to a signal.
   */
  inline int getExitSignal() const
  { return(_exitSignal); }

  /** Determine if the process exited normally. */
  inline bool isNormal() const
  { return(_exitedNormally); }

  /**
   * Get the total amount of user-space time, in milliseconds, that the
   * process consumed.
   */
  inline uint64_t getUserTime() const
  { return(_userTime); }

  /**
   * Get the total amount of kernel-space time, in milliseconds,
   * that the process consumed.
   */
  inline uint64_t getKernelTime() const
  { return(_kernelTime); }

  /**
   * Get the process's maximum resident set size, in pages. This
   * information is not available on some systems (e.g., it is
   * always 0 on Linux).
   */
  inline uint_t getMaxRSS() const
  { return(_maxSize); }

  /** Signal indicating the process was aborted. */
  static const int SigAbort;

  /**
   * Signal indicating the process encountered a floating-point exception
   * (such as division by zero).
   */
  static const int SigFloatingPointException;

  /**
   * Signal indicating the process encountered an illegal machine
   * instruction.
   */
  static const int SigIllegalInstruction;

  /**
   * Signal indicating the process received a keyboard interrupt
   * (Control-C).
   */
  static const int SigKeyboardInterrupt;

  /** Signal indicating the process attempted an illegal memory access. */
  static const int SigSegmentationFault;

  /** Signal indicating the process received a termination request. */
  static const int SigTerminationRequest;

  /** The platform's default "success" exit status. */
  static const int SUCCESS;

  /** The platform's default "failure" exit status. */
  static const int FAILURE;

 private:

  int _exitCode;
  int _exitSignal;
  bool _exitedNormally;
  uint64_t _userTime;
  uint64_t _kernelTime;
  uint_t _maxSize;
};

/**
 * A system process. This class may be used to execute a program,
 * communicate with the program via its standard I/O streams,
 * terminate the program, and collect its exit status. The class also
 * includes a number of static utility functions that affect the
 * current process.
 *
 * When the Process is created, its standard input, output, and error
 * streams are automatically connected to the parent process; these
 * streams can be accessed using the getInputStream(),
 * getOutputStream(), and getErrorStream() methods. Any output that
 * the process produces on its standard output and error streams must
 * be read in a timely manner by the calling process to avoid
 * deadlock. Alternatively, if the calling process is not interested
 * in the output, it can immediately close these streams.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Process
{
 public:

  /**
   * Construct a new Process.
   *
   * @param path The path to the executable.
   * @param args The command-line arguments for the executable.
   * @param env Environment variable settings for the new process.
   * @param workingDir The working directory for the new process.
   */
  Process(const String& path, const StringVec& args,
          const StringVec& env = StringVec::emptyVec,
          const String& workingDir = ".");

  /** Destructor. The destructor does <i>not</i> terminate the process. */
  ~Process();

  /**
   * Start the process. If the process has already been started, this
   * method has no effect.
   *
   * @throw SystemException If an error occurred.
   */
  void start();

  /**
   * Stop the process. On Windows systems, this method has the same
   * effect as <b>kill()</b>. On POSIX systems, this method sends
   * a (catchable) <b>SIGTERM</b> signal to the process. If the process
   * has not been started, this method has no effect.
   */
  void stop();

  /**
   * Forcibly kill the process. If the process has not been started,
   * this method has no effect.
   *
   * @return <b>true</b> if the process was successfully killed,
   * <b>false</b> otherwise.
   */
  bool kill();

  /** Set the process priority. */
  void setPriority(Priority priority);

  /** Get the process priority. */
  Priority getPriority() const;

  /**
   * Wait for the process to terminate. Once a process has exited (for
   * whatever reason) and its exit status has been collected via a
   * call to this method, it can be started again with start().
   *
   * @param status The object in which to store the exit status.
   * @return <b>true</b> if the process exited, <b>false</b> if the process
   * was not running.
   */
  bool waitFor(ExitStatus& status);

  /** Determine if the process has been started. */
  inline bool isStarted() const
  { return(_started); }

  /**
   * Get the input stream for the process. This stream is connected to the
   * standard input of the process.
   */
  inline Stream& getInputStream()
  { return(*_ins); }

  /**
   * Get the output stream for the process. This stream is connected to the
   * standard output of the process.
   */
  inline Stream& getOutputStream()
  { return(*_outs); }

  /**
   * Get the error stream for the process. This stream is connected to the
   * standard error of the process.
   */
  inline Stream& getErrorStream()
  { return(*_errs); }

  /** Get the time at which the process was started. */
  inline time_ms_t getStartTime() const
  { return(_startTime); }

  /**
   * Terminate the calling process.
   *
   * @param status The exit status to exit with.
   */
  static void exit(int status);

  /** Get the name of the calling program. */
  inline static String getProgramName()
  { return(_progname); }

  /**
   * Get the full path to the calling program's executable file.
   *
   * @throw SystemException If an error occurs.
   */
  static String getExecutablePath();

  /**
   * Get the full path to the directory containing the calling program's
   * executable file.
   *
   * @throw SystemException If an error occurs.
   */
  static String getExecutableDir();

  /** Set the name of the calling program. */
  inline static void setProgramName(const char* name)
  { _progname = name; }

  /** Get the calling process's working directory. */
  static String getWorkingDir();

  /**
   * Set the calling process's working directory.
   *
   * @param path The new working directory.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool setWorkingDir(const String& path);

  /**
   * Test for the presence of a console.
   *
   * @return <b>true</b> if the calling process is connected to a console,
   * <b>false</b> otherwise.
   */
  static bool hasConsole();

  /**
   * Get the console dimensions for the calling process, in
   * characters. On POSIX systems, this refers to the dimensions of
   * the terminal window. On Windows, it refers to the "console buffer
   * size."
   *
   * @param columns A pointer to the value where the width of the console
   * should be stored. May be <b>NULL</b>.
   * @param rows A pointer to the value where the height of the console
   * should be stored. May be <b>NULL</b>.
   * @return <b>true</b> if the size could be determined, <b>false</b>
   * otherwise.
   */
  static bool getConsoleSize(uint_t* columns, uint_t* rows);

  /**
   * Set the console title for the calling process.
   *
   * @param title The new title.
   */
  static void setConsoleTitle(const String& title);

  /** Get the calling process ID. */
  static ProcessID currentProcessID();

 private:

  String _path;
  String _workingDir;
  bool _started;
  time_ms_t _startTime;
  ProcessHandle _handle;
  StringVec _args;
  StringVec _env;
  Stream* _ins;
  Stream* _outs;
  Stream* _errs;

  static String _progname;

  Process(); // not supported
  CCXX_COPY_DECLS(Process);
};

} // namespace ccxx

#endif // __ccxx_Process_hxx
