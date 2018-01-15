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

#include "commonc++/Application.h++"
#include "commonc++/System.h++"
#include "commonc++/TerminalAttr.h++"
#include "commonc++/File.h++"
#include "commonc++/Process.h++"
#include "commonc++/System.h++"

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <exception>
#include <signal.h>
#include <getopt.h>

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#endif

#ifdef CCXX_OS_WINDOWS
#include "cbits.h"
#endif

namespace ccxx {

#define OPT_HELP 0
#define OPT_VERSION 1

/*
 */

class Application::CommandOptionList : public std::vector<CommandOption>
{
};

/*
 */

Application *Application::_instance = NULL;

/*
 */

void Application::_sighandler(int sig)
{
  switch(sig)
  {
    case SIGINT:
    case SIGTERM:
      Application::_instance->doShutdown();
      break;
  }
}

/*
 */

static void __unhandledExceptionHandler()
{
  std::cerr << "Unhandled exception" << std::endl;
  System::printStackTrace();
  abort();
}

/*
 */

static void __unexpectedExceptionHandler()
{
  std::cerr << "Unexpected exception" << std::endl;
  System::printStackTrace();
  abort();
}

/*
 */

Application::Application(int argc, char** argv,
                         const String &version /* = "0.0" */,
                         const String &compileDate /* = "" */,
                         const String &compileTime /* = "" */)
  : _version(version),
    _compileDate(compileDate),
    _compileTime(compileTime),
    _options(new CommandOptionList()),
    _numLongOpts(0),
    _argsParsed(false),
#ifdef CCXX_OS_POSIX
    _useStyles(true)
#else
    _useStyles(false)
#endif
{
  std::set_terminate(__unhandledExceptionHandler);
  std::set_unexpected(__unexpectedExceptionHandler);

  // construct program name from path

  char *p = ::strrchr(*argv, File::separator);
  char *q = (p ? ++p : *argv);
  Process::setProgramName(q);
  argv[0] = q;

  // register default options

  registerOption(NUL, "help", "", "Display usage information.");
  registerOption(NUL, "version", "",  "Get version information.");

  _instance = this;

  ::signal(SIGINT, _sighandler);
  ::signal(SIGTERM, _sighandler);
}

/*
 */

bool Application::parseOptions(int argc, char **argv, int &index)
{
  if(! _argsParsed)
  {
    buildUsageText();
    _argsParsed = true;
  }

  int c = 0;
  struct option *longOpts = new struct option[_numLongOpts + 1];
  _optString = "";

  CommandOptionList::const_iterator iter;
  for(iter = _options->begin(); iter != _options->end(); iter++)
  {
    const CommandOption& opt = *iter;

    if(opt.hasLongOpt())
    {
      CString cstr_longopt = opt.getLongOpt().toUTF8();
      longOpts[c].name = ::strdup(cstr_longopt.data());
      longOpts[c].has_arg = (opt.hasArg() ? 1 : 0);
      longOpts[c].flag = NULL;
      longOpts[c].val = 0;

      c++;
    }

    char ch = opt.getOpt();
    if(ch != NUL)
    {
      _optString += ch;
      if(opt.hasArg())
        _optString += ':';
    }
  }

  // NULL-terminate the option array
  longOpts[c].name = NULL;

  // parse the command line

  int optc;

  CString cstr_optString = _optString.toUTF8();

  while((c = getopt_long(argc, argv, cstr_optString.data(), longOpts, &optc))
        != EOF)
  {
    switch(c)
    {
      case NUL:
      {
        switch(optc)
        {
          case OPT_HELP:
            printUsage();
            exit(EXIT_SUCCESS);
            break;

          case OPT_VERSION:
            std::cerr << Process::getProgramName() << ": Version " << _version
                      << std::endl;
            if(_compileDate != "" && _compileTime != "")
            {
              std::cerr << Process::getProgramName() << ": Compiled on "
                        << _compileDate;
              if(_compileTime != "")
                std::cerr << " at " << _compileTime;

              std::cerr << std::endl;
            }
            exit(EXIT_SUCCESS);
            break;

          default:
          {
            CommandOption& opt = (*_options)[optc];

            if(! processOption(opt.getOpt(), opt.getLongOpt(),
                               (optarg != NULL) ? String(optarg)
                               : String::null))
              return(false);
          }
        }

        break;
      }

      default:
        if(! processOption(c, String::null,
                           (optarg != NULL) ? String(optarg) : String::null))
          return(false);
    }
  }

  for(int i = 0; i < _numLongOpts; ++i)
    ::free((void *)longOpts[i].name);

  delete[] longOpts;

  index = optind;

  return(true);
}

/*
 */

Application::~Application()
{
  delete _options;
}

/*
 */

void Application::shutdown()
{
}

/*
 */

void Application::doShutdown()
{
  shutdown();
  Process::exit(0);
}

/*
 */

void Application::printUsage()
{
  std::cerr << Process::getProgramName() << ": Usage: ";

  if(_useStyles)
    std::cerr << TERMATTR_BOLD;

  std::cerr << Process::getProgramName();

  if(_useStyles)
    std::cerr << TERMATTR_NORMAL;

  std::cerr << " [options] [arguments]\n\n" "Where options include:\n\n"
            << _helpText << std::endl;
}

/*
 */

void Application::printError(const char* message, ...)
{
  va_list vp;

  CString progname = Process::getProgramName().toUTF8();

  std::fputs(progname.data(), stderr);
  std::fputs(": ", stderr);

  va_start(vp, message);
  std::vfprintf(stderr, message, vp);
  va_end(vp);

  std::fflush(stderr);
}

/*
 */

void Application::buildUsageText()
{
  static const int columnWidth = 24;

  // generate the optarg string

  _optString = "";

  // build optstring (short opts)

  CommandOptionList::const_iterator iter;
  for(iter = _options->begin(); iter != _options->end(); iter++)
  {
    const CommandOption& opt = *iter;

    if(opt.hasOpt())
    {
      _optString += opt.getOpt();
      if(opt.hasArg())
        _optString += ':';
    }
  }

  // generate the help text

  _helpText = "";
  int maxLen = 76 - columnWidth;

  String tab;
  tab.fill(' ', columnWidth);

  for(iter = _options->begin(); iter != _options->end(); iter++)
  {
    const CommandOption& opt = *iter;
    _helpText += "  ";
    int len = 2;

    if(opt.hasOpt())
    {
      if(_useStyles)
        _helpText += TERMATTR_BOLD;

      _helpText += '-';
      _helpText += opt.getOpt();
      len += 2;

      if(_useStyles)
        _helpText += TERMATTR_NORMAL;

      if(opt.hasArg())
      {
        _helpText += ' ';
        len++;

        if(_useStyles)
          _helpText += TERMATTR_UNDERLINE;
        else
        {
          _helpText += '<';
          len++;
        }

        _helpText += opt.getArgName();
        len += opt.getArgName().length();

        if(_useStyles)
          _helpText += TERMATTR_NORMAL;
        else
        {
          _helpText += '>';
          len++;
        }
      }
    }

    if(opt.hasLongOpt())
    {
      if(opt.hasOpt())
      {
        _helpText += File::eol;
        _helpText += "  ";
        len = 2;
      }

      if(_useStyles)
        _helpText += TERMATTR_BOLD;

      _helpText += "--";
      len += 2;

      _helpText += opt.getLongOpt();
      len += opt.getLongOpt().length();

      if(_useStyles)
        _helpText += TERMATTR_NORMAL;

      if(opt.hasArg())
      {
        _helpText += '=';
        len++;

        if(_useStyles)
          _helpText += TERMATTR_UNDERLINE;
        else
        {
          _helpText += '<';
          len++;
        }

        _helpText += opt.getArgName();
        len += opt.getArgName().length();

        if(_useStyles)
          _helpText += TERMATTR_NORMAL;
        else
        {
          _helpText += '>';
          len++;
        }
      }
    }

    if(len < columnWidth)
    {
      for(int i = 0; i < (columnWidth - len); i++)
        _helpText += ' ';
    }
    else
    {
      _helpText += File::eol;
      _helpText += tab;
    }

    const String arg = opt.getArgName();
    const String& help = opt.getArgDesc();
    CString cstr_help = help.toUTF8();

    char *txt = new char[cstr_help.length() + 1];
    std::strncpy(txt, cstr_help.data(), cstr_help.length() + 1);

    char *p, *pp = NULL;
    int lineLen = 0;

    for(p = ::strtok_r(txt, "\n\t ", &pp);
        p;
        p = ::strtok_r(NULL, "\n\t ", &pp))
    {
      int len = 0;
      char *markup = ::strstr(p, "@@");
      bool mf = (markup == p);

      if(mf)
        len = (_useStyles ? 0 : 2) + arg.getLength();
      else
        len = std::strlen(p);

      if((lineLen + len + (lineLen > 0)) > maxLen)
      {
        lineLen = 0;
        _helpText += File::eol;
        _helpText += tab;
      }

      if(lineLen > 0)
      {
        _helpText += ' ';
        lineLen++;
      }

      if(mf)
      {
        if(_useStyles)
          _helpText += TERMATTR_UNDERLINE;
        else
          _helpText += '<';

        _helpText += arg;

        if(_useStyles)
          _helpText += TERMATTR_NORMAL;
        else
          _helpText += '>';

        _helpText += (markup + 2);
      }
      else
        _helpText += p;

      lineLen += len;
    }

    delete[] txt;

    _helpText += File::eol;
  }
}

/*
 */

bool Application::registerOption(char opt, const String& longOpt,
                                 const String& argName,
                                 const String& argDesc)
{
  if(_argsParsed)
  {
    // Cannot call registerOption() after a call to parseCommandLine()
    return(false);
  }

  if((opt == NUL) && longOpt.isEmpty())
    return(false);

  // check for duplicates, and reject them

  for(CommandOptionList::const_iterator iter = _options->begin();
      iter != _options->end();
      iter++)
  {
    const CommandOption& copt = *iter;
    if(((opt != NUL) && copt.getOpt() == opt)
       || ((longOpt != "") && copt.getLongOpt() == longOpt))
    {
      // the switch is already defined!
      return(false);
    }
  }

  CommandOption option(opt, longOpt, argName, argDesc);
  _options->push_back(option);

  if(longOpt != "")
    _numLongOpts++;

  return(true);
}

/*
 */

bool Application::processOption(char opt, const String& longOpt,
                                const String& arg)
{
  return(false);
}

/*
 */

Application::CommandOption::CommandOption(char opt, const String& longOpt,
                                          const String& argName,
                                          const String& argDesc)
  : _opt(opt) ,
    _longOpt(longOpt),
    _argName(argName),
    _argDesc(argDesc)
{
  _hasArg = (argName.getLength() > 0);
}


} // namespace ccxx
