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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/Process.h++"
#include "commonc++/Service.h++"
#include "commonc++/System.h++"
#include "commonc++/File.h++"

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#endif

#include <fstream>

namespace ccxx {

/*
 */

Service* Service::_instance = NULL;

/*
 */

#ifdef CCXX_OS_POSIX

void Service::_sighandler(int sig)
{
  switch(sig)
  {
    case SIGTERM:
    case SIGINT:
      Service::_instance->_shutdown = true;
      break;
  }
}

#endif

/*
 */

Service::Service(int argc, char** argv, const String& version,
                 const String& compileDate /* = "" */,
                 const String& compileTime /* = "" */)
  : Application(argc, argv, version, compileDate, compileTime),
    _shutdown(false),
    _debug(false),
    _pidFile(String::null),
#ifdef CCXX_OS_WINDOWS
    _action(None),
#endif
    _workingDir("/")
{
#ifdef CCXX_OS_POSIX

  registerOption(NUL, "pidfile", "file",
                 "Write the server's PID to @@ after detaching.");

#endif

  _name = _displayName = Process::getProgramName();

#ifdef CCXX_OS_WINDOWS

  registerOption(NUL, "name", "name", "Specify service name as @@.");
  registerOption(NUL, "displayname", "name",
                 "Specify service display name as @@.");
  registerOption(NUL, "desc", "text", "Specify service description as @@.");
  registerOption(NUL, "install", "", "Install service.");
  registerOption(NUL, "uninstall", "", "Uninstall service.");
  registerOption(NUL, "start", "", "Start the service.");
  registerOption(NUL, "stop", "", "Stop the service.");

#endif

  registerOption(NUL, "debug", "", "Run in debug (foreground) mode.");

  _instance = this;

#ifdef CCXX_OS_WINDOWS

  // Suppress interactive dialog boxes that are inappropriate for a service
  // process.
  _oldErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS
                                 | SEM_NOOPENFILEERRORBOX);

#endif
}

/*
 */

Service::~Service()
{
#ifdef CCXX_OS_WINDOWS

  ::SetErrorMode(_oldErrorMode);

#endif
}

/*
 */

bool Service::processOption(char opt, const String& longOpt, const String& arg)
{
#ifdef CCXX_OS_POSIX

  if(longOpt == "pidfile")
  {
    _pidFile = arg;
    return(true);
  }

#endif

#ifdef CCXX_OS_WINDOWS

  if(longOpt == "install")
  {
    if(_action != None)
    {
      std::cerr << "Warning: Only one control action can be specified."
                << std::endl;
      return(false);
    }

    _action = Install;
    return(true);
  }
  else if(longOpt == "uninstall")
  {
    if(_action != None)
    {
      std::cerr << "Warning: Only one control action can be specified."
                << std::endl;
      return(false);
    }

    _action = Uninstall;
    return(true);
  }
  else if(longOpt == "start")
  {
    if(_action != None)
    {
      std::cerr << "Warning: Only one control action can be specified."
                << std::endl;
      return(false);
    }

    _action = Start;
    return(true);
  }
  else if(longOpt == "stop")
  {
    if(_action != None)
    {
      std::cerr << "Warning: Only one control action can be specified."
                << std::endl;
      return(false);
    }

    _action = Stop;
    return(true);
  }
  else if(longOpt == "name")
  {
    _name = arg;
    return(true);
  }
  else if(longOpt == "displayname")
  {
    _displayName = arg;
    return(true);
  }
  else if(longOpt == "desc")
  {
    _desc = arg;
    return(true);
  }

#endif

  if(longOpt == "debug")
  {
    _debug = true;
    return(true);
  }
  else
    return(Application::processOption(opt, longOpt, arg));
}

/*
 */

void Service::detach()
{
  if(_debug)
  {
#ifdef CCXX_OS_POSIX

    Process::setWorkingDir(_workingDir);
    ::signal(SIGTERM, _sighandler);
    ::signal(SIGINT, _sighandler);

#endif

    run();
  }
  else
  {
#ifdef CCXX_OS_WINDOWS

    switch(_action)
    {
      case Start:
        try
        {
          _serviceStart();
        }
        catch(SystemException& ex)
        {
          std::cerr << "Failed to start service." << std::endl;
          std::cerr << ex.getMessage() << std::endl;
        }
        return;

      case Stop:
        try
        {
          _serviceStop();
        }
        catch(SystemException& ex)
        {
          std::cerr << "Failed to stop service." << std::endl;
          std::cerr << ex.getMessage() << std::endl;
        }
        return;

      case Install:
        try
        {
          _serviceInstall();
        }
        catch(SystemException& ex)
        {
          std::cerr << "Failed to install service." << std::endl;
          std::cerr << ex.getMessage() << std::endl;
        }
        return;

      case Uninstall:
        try
        {
          _serviceUninstall();
        }
        catch(SystemException& ex)
        {
          std::cerr << "Failed to uninstall service." << std::endl;
          std::cerr << ex.getMessage() << std::endl;
        }
        return;

      case None:
      default:
        // fall through
        break;
    }

    SERVICE_TABLE_ENTRY svctable[2];
    svctable[0].lpServiceName = (LPSTR)_name.c_str();
    svctable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)_serviceMain;
    svctable[1].lpServiceName = NULL;
    svctable[1].lpServiceProc = NULL;

    StartServiceCtrlDispatcher(svctable);

#else

    pid_t pid = ::fork();
    if(pid < 0)
      throw SystemException(System::getErrorString("fork"));
    else if(pid > 0)
    {
      if(! _pidFile.isNull())
      {
        CString cstr_pidFile = _pidFile.toUTF8();
        std::ofstream of(cstr_pidFile.data());
        of << pid << std::endl;
        of.close();
      }

      exit(EXIT_SUCCESS);
    }
    else
    {
      // child

      Process::setWorkingDir(_workingDir);

      ::setsid();
      ::close(STDIN_FILENO);
      ::close(STDOUT_FILENO);
      ::close(STDERR_FILENO);

      // might need to fix this up so the signal is only delivered to calling
      // thread
      ::signal(SIGTERM, _sighandler);
      ::signal(SIGINT, SIG_IGN);

      run();
    }

#endif
  }
}

/*
 */

void Service::testShutdown()
{
  if(_shutdown)
  {
    shutdown();

#ifdef CCXX_OS_WINDOWS
    _status.dwWin32ExitCode = NO_ERROR;
    _status.dwCurrentState = SERVICE_STOPPED;
    _status.dwCheckPoint = 0;
    _status.dwWaitHint = 0;
    ::SetServiceStatus(_handle, &_status);
#else
    Process::exit(EXIT_SUCCESS);
#endif
  }
}

/*
*/

#ifdef CCXX_OS_WINDOWS

void Service::_serviceMain(int argc, char** argv)
{
  _instance->_serviceInit();
}

/*
*/

void Service::_serviceInit()
{
  _handle = ::RegisterServiceCtrlHandler(_name.c_str(),
                                         (LPHANDLER_FUNCTION)_controlHandler);

  if(_handle == (SERVICE_STATUS_HANDLE)0)
  {
    // error
  }

  _status.dwServiceType = SERVICE_WIN32;
  _status.dwCurrentState = SERVICE_START_PENDING;
  _status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
  _status.dwWin32ExitCode = 0;
  _status.dwCheckPoint = 0;
  _status.dwWaitHint = 0; // change this?

  // any initialization logic would go here

  _status.dwCurrentState = SERVICE_RUNNING;
  ::SetServiceStatus(_handle, &_status);

  run();
}

/*
*/

void Service::_serviceStart()
{
  SC_HANDLE scmh, sh;

  scmh = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

  if(! scmh)
    throw SystemException(System::getErrorString("OpenSCManager"));

  sh = ::OpenService(scmh, _name.c_str(),
                     (SERVICE_START | SERVICE_QUERY_STATUS));
  if(! sh)
  {
    ::CloseServiceHandle(scmh);
    throw SystemException(System::getErrorString("OpenService"));
  }

  SERVICE_STATUS status;
  if(::QueryServiceStatus(sh, &status)
    && (status.dwCurrentState == SERVICE_RUNNING))
  {
    ::CloseServiceHandle(sh);
    ::CloseServiceHandle(scmh);
    throw SystemException("The service is already running.");
  }

  LPCSTR *argv = { NULL };

  if(! ::StartService(sh, 0, argv))
  {
    ::CloseServiceHandle(sh);
    ::CloseServiceHandle(scmh);
    throw SystemException(System::getErrorString("StartService"));
  }

  bool ok = _waitForStatus(sh, SERVICE_START_PENDING, SERVICE_RUNNING);

  ::CloseServiceHandle(sh);
  ::CloseServiceHandle(scmh);

  if(! ok)
    throw SystemException("The service failed to start.");
}

/*
*/

void Service::_serviceStop()
{
 SC_HANDLE scmh, sh;

  scmh = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

  if(! scmh)
    throw SystemException(System::getErrorString("OpenSCManager"));

  sh = ::OpenService(scmh, _name.c_str(),
                     (SERVICE_INTERROGATE | SERVICE_QUERY_STATUS
                      | SERVICE_USER_DEFINED_CONTROL | SERVICE_START
                      | SERVICE_STOP));
  if(! sh)
  {
    ::CloseServiceHandle(scmh);
    throw SystemException(System::getErrorString("OpenService"));
  }

  SERVICE_STATUS status;
  if(! ::QueryServiceStatus(sh, &status))
  {
    ::CloseServiceHandle(sh);
    ::CloseServiceHandle(scmh);
    throw SystemException(System::getErrorString("QueryServiceStatus"));
  }


  if(status.dwCurrentState == SERVICE_STOPPED)
  {
    ::CloseServiceHandle(sh);
    ::CloseServiceHandle(scmh);
    throw SystemException("The service is not running.");
  }

  if(! ::ControlService(sh, SERVICE_CONTROL_STOP, &status))
  {
    ::CloseServiceHandle(sh);
    ::CloseServiceHandle(scmh);
    throw SystemException(System::getErrorString("ControlService"));
  }

  bool ok = _waitForStatus(sh, SERVICE_STOP_PENDING, SERVICE_STOPPED);

  ::CloseServiceHandle(sh);
  ::CloseServiceHandle(scmh);

  if(! ok)
    throw SystemException("The service failed to stop.");
 }

/*
*/

void Service::_serviceInstall()
{
  SC_HANDLE scmh, sh;
  char exepath[MAX_PATH];

  if(! ::GetModuleFileName(NULL, exepath, sizeof(exepath)))
    throw SystemException(System::getErrorString("GetModuleFileName"));

  scmh = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if(! scmh)
    throw SystemException(System::getErrorString("OpenSCManager"));

  String command = exepath;

  sh = ::CreateService(scmh,
                       _name.c_str(), // service name
                       _displayName.c_str(), // display name
                       SERVICE_ALL_ACCESS, // access required
                       SERVICE_WIN32_OWN_PROCESS, // service type
                       SERVICE_AUTO_START, // start type
                       SERVICE_ERROR_NORMAL, // error control type
                       command.c_str(), // launch command
                       NULL, // no load service group
                       NULL, // no tag identifier
                       "Tcpip\0Afd\0", // dependencies
                       NULL, // use SYSTEM account
                       NULL // no password
                       );

  if(! sh)
  {
    ::CloseServiceHandle(scmh);
    throw SystemException(System::getErrorString("CreateService"));
  }

  const char *desc = _desc.c_str();
  ::ChangeServiceConfig2(sh, SERVICE_CONFIG_DESCRIPTION, (LPVOID)&desc);

  ::CloseServiceHandle(sh);
  ::CloseServiceHandle(scmh);
}

/*
*/

void Service::_serviceUninstall()
{
  SC_HANDLE scmh, sh;

  scmh = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if(! scmh)
    throw SystemException(System::getErrorString("OpenSCManager"));

  sh = ::OpenService(scmh, _name.c_str(), DELETE);
  if(! sh)
  {
    ::CloseServiceHandle(scmh);
    throw SystemException(System::getErrorString("OpenService"));
  }

  if(! ::DeleteService(sh))
  {
    ::CloseServiceHandle(sh);
    ::CloseServiceHandle(scmh);
    throw SystemException(System::getErrorString("DeleteService"));
  }

  ::CloseServiceHandle(sh);
  ::CloseServiceHandle(scmh);
}

/*
*/

void Service::_controlHandler(DWORD code)
{
  switch(code)
  {
    case SERVICE_CONTROL_INTERROGATE:
      ::SetServiceStatus(Service::_instance->_handle,
                         &Service::_instance->_status);
      break;

    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
      Service::_instance->_status.dwWin32ExitCode = NO_ERROR;
      Service::_instance->_status.dwCurrentState = SERVICE_STOP_PENDING;
      Service::_instance->_status.dwCheckPoint = 0;
      Service::_instance->_status.dwWaitHint = INFINITE;
      ::SetServiceStatus(Service::_instance->_handle,
                         &Service::_instance->_status);

      Service::_instance->_shutdown = true;
      break;

    default:
      // do nothing
      break;
  }
}

/*
*/

bool Service::_waitForStatus(SC_HANDLE handle, DWORD pending, DWORD final)
{
  SERVICE_STATUS status;

  do
  {
    ::Sleep(1000);

    if(! ::QueryServiceStatus(handle, &status))
      return(false);
  }
  while(status.dwCurrentState == pending);

  return(status.dwCurrentState == final);
}

#endif // CCXX_OS_WINDOWS


} // namespace ccxx
