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

#ifndef __ccxx_Service_hxx
#define __ccxx_Service_hxx

#include <commonc++/Common.h++>
#include <commonc++/Application.h++>
#include <commonc++/SystemException.h++>

namespace ccxx {

/**
 * Basic functionality for a system service, such as a daemon on UNIX or an
 * "NT Service" on Windows.
 * <p>
 * On POSIX systems, the service recognizes the <tt>--pidfile</tt> command
 * line switch, whose argument is the path of a file to which the daemon
 * process's PID should be written.
 * <p>
 * On Windows, the service recognizes the following command line switches:
 * <dl>
 * <dt><tt>--name</tt>=<i>name</i>
 * <dd>Specifies the name of the %service; this is the logical name by which
 * the service is known to Windows.
 * <dt><tt>--displayname</tt>=<i>name</i>
 * <dd>Specifies the display name of the %service; this is the name by which
 * the service is known to the %Service Control Manager (SCM), and appears
 * in the "Name" column in the %Service Manager applet.
 * <dt><tt>--desc</tt>=<i>description</i>
 * <dd>Specifies a brief description for the service; this description
 * appears in the "Description" column in the %Service Manager applet.
 * <dt><tt>--install</tt>
 * <dd>Installs the service into the %Service Control Manager.
 * <dt><tt>--uninstall</tt>
 * <dd>Uninstalls the service from the %Service Control Manager.
 * <dt><tt>--start</tt>
 * <dd>Starts the service.
 * <dt><tt>--stop</tt>
 * <dd>Stops the service.
 * </dl>
 * <p>
 * On all systems, the following switch is recognized:
 * <dl>
 * <dt><tt>--debug</tt>
 * <dd>Indicates that the service should run in debug (foreground) mode.
 * In this mode, the detach() function passes control directly to the
 * run() function without first detaching from the console. In addition, a
 * keyboard interrupt (typically Control-C) will cause a termination request.
 * </dl>
 * <p>
 * This class is implicitly a singleton; there can be only one instantiation
 * of the class per process. The class should be subclassed and instantiated
 * within the program's <b>main()</b> function.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Service : public Application
{
 public:

  /** Construct a new Service.
   *
   * @param argc The argument count (from <b>main()</b>).
   * @param argv The argument list (from <b>main()</b>).
   * @param version The optional version of the program; if not supplied,
   * it defaults to "0.0".
   * @param compileDate The optional compile date of the program;
   * typically provided by the compiler-defined macro <b>__DATE__</b>.
   * @param compileTime The optional compile time of the program;
   * typically provided by the compiler-defined macro <b>__TIME__</b>.
   */
  Service(int argc, char** argv, const String& version,
          const String& compileDate = "", const String& compileTime = "");

  /** Destructor. */
  virtual ~Service();

 protected:

  /**
   * Set the service description. On Windows, this is used for the
   * description in the %Service Manager applet when the service is
   * installed; on other platforms, it serves no purpose.
   *
   * @param desc The new description.
   */
  inline void setDescription(const String& desc)
  { _desc = desc; }

  /**
   * On POSIX systems, set the working directory for the service
   * daemon process. The default working directory is the root
   * directory ("/").
   *
   * @param dir The working directory.
   */
  inline void setWorkingDir(const String& dir)
  { _workingDir = dir; }

  /*
   */
  virtual bool processOption(char opt, const String& longOpt,
                             const String& arg = String::null);

  /**
   * Detach the process from the console. This method must be
   * called from the constructor once command line option processing
   * and any other necessary initialization is complete. On POSIX
   * systems, it forks a daemon process, while on Windows it passes
   * control to the %Service Control Manager (SCM) subsystem. On all
   * platforms, execution resumes in the <b>run()</b> method, where
   * the body of the service must be implemented.
   *
   * @throw SystemException If the detach operation fails.
   */
  void detach();

  /**
   * The body of the service. This method must be implemented to provide
   * the actual service processing loop. It must periodically call
   * <b>testShutdown()</b> to ensure that an asynchronous shutdown request
   * will be processed in a timely manner.
   */
  virtual void run() = 0;

  /**
   * Test for asynchronous termination requests. The service must
   * regularly call this method to check for termination requests. The
   * method will delegate to <b>shutdown()</b> if it has determined that a
   * termination request is pending.
   */
  void testShutdown();

  /**
   * Shutdown handler. The method must be overriden to provide cleanup
   * and shutdown logic for the service. It is invoked as a side effect of
   * calling <b>testShutdown()</b> when the service has been stopped via
   * a TERM signal (UNIX), the %Service Manager applet (Windows), or through
   * use of the "--stop" command line switch (Windows). This function should
   * <i>not</i> call <code>exit()</code> or otherwise terminate the process.
   */
  virtual void shutdown() = 0;

  /** Test if debug (foreground) mode is enabled. */
  inline bool isDebug() const
  { return(_debug); }

 private:

#ifdef CCXX_OS_WINDOWS
  static void _serviceMain(int argc, char** argv);
  void _serviceInit();
  static void _controlHandler(DWORD code);
  void _serviceStart();
  void _serviceStop();
  void _serviceInstall();
  void _serviceUninstall();
  bool _waitForStatus(SC_HANDLE handle, DWORD pending, DWORD final);

  SERVICE_STATUS_HANDLE _handle;
  SERVICE_STATUS _status;
  UINT _oldErrorMode;
#endif

#ifdef CCXX_OS_POSIX
  static void _sighandler(int sig);
#endif

  String _name;
  String _displayName;
  String _desc;
  bool _shutdown;
  bool _debug;
  String _pidFile;

#ifdef CCXX_OS_WINDOWS
  enum Action { None, Start, Stop, Install, Uninstall } _action;
#endif

  String _workingDir;
  static Service *_instance;
};

} // namespace ccxx

#endif // __ccxx_Service_hxx
