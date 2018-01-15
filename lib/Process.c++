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
#include "commonc++/DynamicArray.h++"
#include "commonc++/Console.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"

#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <limits.h>
#endif

#ifdef CCXX_OS_MACOSX
#include <mach-o/dyld.h>
#endif // CCXX_OS_MACOSX

#ifdef CCXX_OS_WINDOWS
#include <io.h>
#include "cbits.h"

#include "commonc++/Windows.h++"
#endif // CCXX_OS_WINDOWS

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <signal.h>

namespace ccxx {

/*
 */

const int ExitStatus::SigAbort = SIGABRT;
const int ExitStatus::SigFloatingPointException = SIGFPE;
const int ExitStatus::SigIllegalInstruction = SIGILL;
const int ExitStatus::SigKeyboardInterrupt = SIGINT;
const int ExitStatus::SigSegmentationFault = SIGSEGV;
const int ExitStatus::SigTerminationRequest = SIGTERM;

const int ExitStatus::SUCCESS = EXIT_SUCCESS;
const int ExitStatus::FAILURE = EXIT_FAILURE;

String Process::_progname;

/*
 */

Process::Process(const String& path, const StringVec& args,
                 const StringVec& env /* = StringVec::emptyVec */,
                 const String& workingDir /* = "." */)
  : _path(path),
    _workingDir(workingDir),
    _started(false),
    _startTime(INT64_CONST(0)),
    _ins(NULL),
    _outs(NULL),
    _errs(NULL)
{
  StringVec::const_iterator iter;

  for(iter = args.begin(); iter != args.end(); ++iter)
    _args << *iter;

  for(iter = env.begin(); iter != env.end(); ++iter)
    _env << *iter;
}

/*
 */

Process::~Process()
{
  if(_ins != NULL)
    _ins->close();
  if(_outs != NULL)
    _outs->close();
  if(_errs != NULL)
    _errs->close();

  delete _ins;
  delete _outs;
  delete _errs;
}

/*
 */

void Process::start()
{
  if(_started)
    return;

#ifdef CCXX_OS_WINDOWS

  String cmd = "";

  for(StringVec::const_iterator iter = _args.begin();
      iter != _args.end();
      ++iter)
  {
    String s = (*iter);

    if(! cmd.isEmpty())
      cmd += ' ';

    cmd += '\"';
    cmd += *iter;
    cmd += '\"';
  }

  HANDLE hin[2], hout[2], herr[2];
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

  if(::CreatePipe(&hin[0], &hin[1], &sa, 0) == FALSE)
    throw SystemException(System::getErrorString("CreatePipe"));

  if(::CreatePipe(&hout[0], &hout[1], &sa, 0) == FALSE)
  {
    String err = System::getErrorString("CreatePipe");
    ::CloseHandle(hin[0]);
    ::CloseHandle(hin[1]);

    throw SystemException(err);
  }

  if(::CreatePipe(&herr[0], &herr[1], &sa, 0) == FALSE)
  {
    String err = System::getErrorString("CreatePipe");
    ::CloseHandle(hin[0]);
    ::CloseHandle(hin[1]);
    ::CloseHandle(hout[0]);
    ::CloseHandle(hout[1]);

    throw SystemException(err);
  }

  // these handles not needed in child
  if((::SetHandleInformation(hin[1], HANDLE_FLAG_INHERIT, 0) == FALSE)
     || (::SetHandleInformation(hout[0], HANDLE_FLAG_INHERIT, 0) == FALSE)
     || (::SetHandleInformation(herr[0], HANDLE_FLAG_INHERIT, 0) == FALSE))
  {
    String err = System::getErrorString("SetHandleInformation");
    ::CloseHandle(hin[0]);
    ::CloseHandle(hin[1]);
    ::CloseHandle(hout[0]);
    ::CloseHandle(hout[1]);
    ::CloseHandle(herr[0]);
    ::CloseHandle(herr[1]);

    throw SystemException(err);
  }

  PROCESS_INFORMATION pi;
  ::ZeroMemory(&pi, sizeof(pi));

  STARTUPINFO si;
  ::ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.wShowWindow = SW_HIDE;
  si.hStdInput = hin[0];
  si.hStdOutput = hout[1];
  si.hStdError = herr[1];

  char *envp = NULL;

  if(! _env.empty())
  {
    // env block size is: sum of var lengths +
    // var count (for NUL's) + 1 (for final NUL)

    size_t sz = 0;
    StringVec::const_iterator iter;

    for(iter = _env.begin(); iter != _env.end(); ++iter)
    {
      sz += (*iter).getLength();
      ++sz;
    }
    ++sz;

    char *p;
    p = envp = new char[sz];

    for(iter = _env.begin(); iter != _env.end(); ++iter)
    {
      const char *var = (*iter).c_str();
      std::strcpy(p, var);
      p += (*iter).getLength();
      *p = NUL;
      ++p;
    }

    *p = NUL;
  }

  char *cmdline = new char[cmd.getLength() + 1];
  std::strcpy(cmdline, cmd.c_str());

  BOOL ok = ::CreateProcess(_path.c_str(), cmdline, NULL, NULL, TRUE,
                            DETACHED_PROCESS, envp, _workingDir.c_str(), &si,
                            &pi);

  delete[] cmdline;

  if(envp)
    delete[] envp;

  if(ok != TRUE)
    throw SystemException(System::getErrorString("CreateProcess"));

  ::CloseHandle(pi.hThread);
  ::CloseHandle(hin[0]);
  ::CloseHandle(hout[1]);
  ::CloseHandle(herr[1]);

  _handle = pi.hProcess;

  _ins = new Stream(hin[1], false, false, true); // write-only
  _outs = new Stream(hout[0], false, true, false); // ready-only
  _errs = new Stream(herr[0], false, true, false); // read-only

#else

  int fdin[2], fdout[2], fderr[2];

  if(::pipe(fdin) != 0)
    throw SystemException(System::getErrorString("pipe"));

  if(::pipe(fdout) != 0)
  {
    String err = System::getErrorString("pipe");
    ::close(fdin[0]);
    ::close(fdin[1]);
    throw SystemException(err);
  }

  if(::pipe(fderr) != 0)
  {
    String err = System::getErrorString("pipe");
    ::close(fdin[0]);
    ::close(fdin[1]);
    ::close(fdout[0]);
    ::close(fdout[1]);
    throw SystemException(err);
  }

  // these descriptors not needed in child
  ::fcntl(fdin[1], F_SETFD, FD_CLOEXEC);
  ::fcntl(fdout[0], F_SETFD, FD_CLOEXEC);
  ::fcntl(fderr[0], F_SETFD, FD_CLOEXEC);

  pid_t pid = ::fork();
  if(pid < 0)
    throw SystemException(System::getErrorString("fork"));

  else if(pid == 0)
  {
    // child process

    ::signal(SIGPIPE, SIG_IGN);

    setWorkingDir(_workingDir);

    // redirect std streams

    if(::dup2(fdin[0], STDIN_FILENO) == -1)
      ::exit(ExitStatus::FAILURE);

    if(::dup2(fdout[1], STDOUT_FILENO) == -1)
      ::exit(ExitStatus::FAILURE);

    if(::dup2(fderr[1], STDERR_FILENO) == -1)
      ::exit(ExitStatus::FAILURE);

    // build arg and env vecs

    const char **argv = new const char *[_args.size() + 1];
    const char **envv = new const char *[_env.size() + 1];

    StringVec::const_iterator iter;
    int i;

    for(i = 0, iter = _args.begin(); iter != _args.end(); ++iter)
    {
      CString cstr_arg = (*iter).toUTF8();
      argv[i++] = ::strdup(cstr_arg.data());
    }

    argv[i] = NULL;

    for(i = 0, iter = _env.begin(); iter != _env.end(); ++iter)
    {
      CString cstr_env = (*iter).toUTF8();
      envv[i++] = ::strdup(cstr_env.data());
    }

    envv[i] = NULL;

    // execute program

    CString cstr_path = _path.toUTF8();

    if(_env.empty())
      ::execv(cstr_path.data(), const_cast<char * const *>(argv));
    else
      ::execve(cstr_path.data(), const_cast<char * const *>(argv),
               const_cast<char * const *>(envv));

    // if we get this far, there was an exec error

    for(const char **p = argv; *p; ++p)
      ::free((void *)*p);

    for(const char **p = envv; *p; ++p)
      ::free((void *)*p);

    delete[] argv;
    delete[] envv;

    ::exit(ExitStatus::FAILURE);
  }

  // parent process
  _handle = pid;

  ::close(fdin[0]);
  ::close(fdout[1]);
  ::close(fderr[1]);

  _ins = new Stream(fdin[1], false, false, true); // write-only
  _outs = new Stream(fdout[0], false, true, false); // read-only
  _errs = new Stream(fderr[0], false, true, false); // read-only

#endif

  _started = true;
  _startTime = System::currentTimeMillis();
}

/*
 */

void Process::stop()
{
  if(_started)
  {

#ifdef CCXX_OS_WINDOWS

    kill();

#else

    ::kill(_handle, SIGTERM);

#endif

    _startTime = INT64_CONST(0);
  }
}

/*
 */

bool Process::kill()
{
  if(! _started)
    return(false);

#ifdef CCXX_OS_WINDOWS

  return(::TerminateProcess(_handle, 0) == TRUE);

#else

  return(::kill(_handle, SIGKILL) == 0);

#endif
}

/*
 */

void Process::setPriority(Priority priority)
{
  if(! _started)
    return;

#ifdef CCXX_OS_WINDOWS

  DWORD prio = NORMAL_PRIORITY_CLASS;

  switch(priority)
  {
    case PrioLowest:
      prio = IDLE_PRIORITY_CLASS;
      break;

    case PrioLow:
      prio = BELOW_NORMAL_PRIORITY_CLASS;
      break;

    case PrioNormal:
      prio = NORMAL_PRIORITY_CLASS;
      break;

    case PrioHigh:
      prio = ABOVE_NORMAL_PRIORITY_CLASS;
      break;

    case PrioHighest:
      prio = HIGH_PRIORITY_CLASS;
      break;
  }

  ::SetPriorityClass(_handle, prio);

#else

  int prio = 0;

  switch(priority)
  {
    case PrioLowest:
      prio = 20;
      break;

    case PrioLow:
      prio = 10;
      break;

    case PrioNormal:
      prio = 0;
      break;

    case PrioHigh:
      prio = -10;
      break;

    case PrioHighest:
      prio = -20;
      break;
  }

  ::setpriority(PRIO_PROCESS, _handle, prio);

#endif
}

/*
 */

Priority Process::getPriority() const
{
  Priority priority = PrioNormal;

  if(! _started)
    return(priority);

#ifdef CCXX_OS_WINDOWS

  DWORD prio = ::GetPriorityClass(_handle);

  switch(prio)
  {
    case THREAD_PRIORITY_IDLE:
    case THREAD_PRIORITY_LOWEST:
      priority = PrioLowest;
      break;

    case THREAD_PRIORITY_BELOW_NORMAL:
      priority = PrioLow;
      break;

    case THREAD_PRIORITY_NORMAL:
      priority = PrioNormal;
      break;

    case THREAD_PRIORITY_ABOVE_NORMAL:
      priority = PrioHigh;
      break;

    case THREAD_PRIORITY_TIME_CRITICAL:
    case THREAD_PRIORITY_HIGHEST:
      priority = PrioHighest;
      break;
  }

#else

  errno = 0;
  int prio = ::getpriority(PRIO_PROCESS, _handle);

  if(errno != 0)
    prio = 0; // default to normal

  if(prio > 12)
    priority = PrioLowest;
  else if(prio > 4)
    priority = PrioLow;
  else if(prio > -4)
    priority = PrioNormal;
  else if(prio > -12)
    priority = PrioHigh;
  else
    priority = PrioHighest;

#endif

  return(priority);
}

/*
 */

bool Process::waitFor(ExitStatus& status)
{
  if(! _started)
    return(false);

#ifdef CCXX_OS_WINDOWS

  if(::WaitForSingleObjectEx(_handle, INFINITE, TRUE) != WAIT_OBJECT_0)
    return(false);

  DWORD exitCode = 0;

  ::GetExitCodeProcess(_handle, &exitCode);
  status._exitCode = exitCode;
  status._exitSignal = 0; // N/A for Windows
  status._exitedNormally = true;

  FILETIME tmCreate, tmExit, tmKernel, tmUser;

  if(::GetProcessTimes(_handle, &tmCreate, &tmExit, &tmKernel, &tmUser)
     == TRUE)
  {
    status._userTime = Windows::decodeFileTimeAsDelta(tmUser);
    status._kernelTime = Windows::decodeFileTimeAsDelta(tmKernel);
  }
  else
  {
    status._userTime = 0LL;
    status._kernelTime = 0LL;
  }

  SIZE_T minSize = 0, maxSize = 0;

  ::GetProcessWorkingSetSize(_handle, &minSize, &maxSize);

  size_t pgsz = System::getPageSize();
  status._maxSize = (maxSize + (pgsz - 1)) / pgsz;

  ::CloseHandle(_handle);

#else

  pid_t pid;
  struct rusage ru;
  int exitCode;

  pid = ::wait4(_handle, &exitCode, 0, &ru);
  if(pid <= 0)
    return(false);

  status._exitCode = WEXITSTATUS(exitCode);
  status._exitSignal = (WIFSIGNALED(exitCode) ? WTERMSIG(exitCode) : 0);
  status._exitedNormally = WIFEXITED(exitCode);

  status._userTime = POSIX::decodeTimevalAsDelta(ru.ru_utime);
  status._kernelTime = POSIX::decodeTimevalAsDelta(ru.ru_stime);
  status._maxSize = ru.ru_maxrss;

#endif

  _started = false;

  return(true);
}

/*
 */

void Process::exit(int status)
{
#ifdef CCXX_OS_WINDOWS

  ::ExitProcess(static_cast<UINT>(status));

#else

  ::exit(status);

#endif
}

/*
 */

String Process::getExecutablePath()
{
  String name = String::null;

#if defined CCXX_OS_WINDOWS

  TCHAR buf[MAX_PATH + 1];
  if(::GetModuleFileName(NULL, buf, MAX_PATH))
    name = buf;
  else
    throw SystemException(System::getErrorString());

#elif defined CCXX_OS_MACOSX

  DynamicArray<char> buf(PATH_MAX);
  uint32_t buflen;
  if(::_NSGetExecutablePath(buf.data(), &buflen) < 0)
  {
    buf.resize(buflen);
    if(::_NSGetExecutablePath(buf.data(), &buflen) < 0)
      buflen = 0;
  }

  name = String(buf.data());
  if(File::isSymLink(name))
      File::resolveSymLink(name);

#else

  // if have /proc filesytem, read that....

  char buf[32], path[PATH_MAX + 1];
  int r;

  ::snprintf(buf, sizeof(buf), "/proc/%d/exe", ::getpid());

  r = ::readlink(buf, path, PATH_MAX);
  if(r >= 0)
  {
    *(path + r) = NUL;
    name = path;
  }
  else
  {
    throw SystemException("unable to determine executable path");
  }

#endif

  return(name);
}

/*
 */

String Process::getExecutableDir()
{
  String path = getExecutablePath();

  int index = path.lastIndexOfAnyOf(File::validSeparators);
  if(index >= 0)
    path.setLength(index);

  return(path);
}

/*
 */

String Process::getWorkingDir()
{
  char *buf = NULL;

#ifdef CCXX_OS_WINDOWS

  size_t sz = 0;
  DWORD r = ::GetCurrentDirectory(static_cast<DWORD>(sz), NULL);
  sz = static_cast<size_t>(r);
  buf = new char[sz + 1];
  ::GetCurrentDirectory(static_cast<DWORD>(sz + 1), buf);

#else

  size_t sz = 32;

  for(;;)
  {
    buf = new char[sz];

    if(! ::getcwd(buf, sz))
    {
      delete[] buf;

      if(errno != ERANGE)
        return(String::null);

      sz *= 2;
    }
    else
      break;
  }

#endif

  String path(buf);
  return(path);
}

/*
 */

bool Process::setWorkingDir(const String& path)
{
  CString cstr_path = path.toUTF8();

#ifdef CCXX_OS_WINDOWS

  return(::SetCurrentDirectory(cstr_path.data()) == TRUE);

#else

  return(::chdir(cstr_path.data()) == 0);

#endif
}


/*
 */

bool Process::hasConsole()
{
  return(::isatty(STDIN_FILENO) && ::isatty(STDOUT_FILENO)
         && ::isatty(STDERR_FILENO));
}

/*
 */

bool Process::getConsoleSize(uint_t* columns, uint_t* rows)
{
  if(! hasConsole())
    return(false);

  Console console;
  return(console.getSize(columns, rows));
}

/*
 */

void Process::setConsoleTitle(const String& title)
{
  if(hasConsole())
  {
    Console console;
    console.setTitle(title);
  }
}

/*
 */

ProcessID Process::currentProcessID()
{
#ifdef CCXX_OS_WINDOWS

  return(static_cast<ProcessID>(::GetCurrentProcessId()));

#else

  return(static_cast<ProcessID>(::getpid()));

#endif
}


} // namespace ccxx
