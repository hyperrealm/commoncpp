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

#include "commonc++/ConsoleLogger.h++"
#include "commonc++/Char.h++"

#include <cstdio>

namespace ccxx {

/*
 */

ConsoleLogger::ConsoleLogger(const String &format /* = "[%d %t] %@%m%." */)
  : Logger(format)
{
}

/*
 */

ConsoleLogger::~ConsoleLogger()
{
}

/*
 */

bool ConsoleLogger::write(CharBuffer &buffer)
{
#ifdef CCXX_OS_WINDOWS
  writeANSIString(buffer.getPointer(), buffer.getRemaining());
#else
  std::fwrite(buffer.getPointer(), 1, buffer.getRemaining(), stderr);
  std::fflush(stderr);
#endif

  return(true);
}

/*
 */

#ifdef CCXX_OS_WINDOWS

void ConsoleLogger::writeANSIString(const char *buf, size_t buflen)
{
  const char *p = buf, *bp = buf;
  enum State { NONE, ESC, ARG } state = NONE;
  bool sawArg = false;
  uint_t argc = 0;
  static const uint_t max_args = 4;
  uint_t argv[max_args];

  CCXX_ZERO(argv);

  for(size_t i = 0; i < buflen; ++i, ++p)
  {
    switch(state)
    {
      case NONE:
      {
        if(*p == 27)
        {
          if(bp)
          {
            // flush text
            _console.write(bp, (p - bp));
            bp = NULL;
          }

          state = ESC;
        }
        else if(! bp)
          bp = p;

        break;
      }

      case ESC:
      {
        if(*p == '[')
        {
          state = ARG;
          argc = 0;
          sawArg = false;
        }
        else
          state = NONE;

        break;
      }

      case ARG:
      {
        if(*p == ';')
        {
          if(++argc == max_args)
            state = NONE;
        }
        else if(Char::isAlpha(*p))
        {
          bp = NULL;
          if(sawArg)
            ++argc;

          processANSI(*p, argc, argv);
          CCXX_ZERO(argv);
          argc = 0;

          state = NONE;
        }
        else if(Char::isDigit(*p))
        {
          argv[argc] *= 10;
          argv[argc] += (*p - '0');
          sawArg = true;
        }
        else
          state = NONE;

        break;
      }
    }
  }

  if(bp)
  {
    // flush text
    _console.write(bp, (p - bp));
  }
}

/*
 */

void ConsoleLogger::processANSI(char cmd, uint_t argc, uint_t *argv)
{
  switch(cmd)
  {
    case 'm':
    {
      for(uint_t i = 0; i < argc; ++i)
      {
        switch(argv[i])
        {
          case 0:
            _console.resetTextStyle();
            break;

          case 1:
            _console.beginTextStyle(Console::StyleBold);
            break;

          case 4:
            _console.beginTextStyle(Console::StyleUnderline);
            break;

          case 7:
            _console.beginTextStyle(Console::StyleInverse);
            break;

          case 30:
            _console.setTextForeground(Console::ColorBlack);
            break;

          case 31:
            _console.setTextForeground(Console::ColorRed);
            break;

          case 32:
            _console.setTextForeground(Console::ColorGreen);
            break;

          case 33:
            _console.setTextForeground(Console::ColorYellow);
            break;

          case 34:
            _console.setTextForeground(Console::ColorBlue);
            break;

          case 35:
            _console.setTextForeground(Console::ColorMagenta);
            break;

          case 36:
            _console.setTextForeground(Console::ColorCyan);
            break;

          case 37:
          case 39:
            _console.setTextForeground(Console::ColorWhite);
            break;

          case 40:
          case 49:
            _console.setTextBackground(Console::ColorBlack);
            break;

          case 41:
            _console.setTextBackground(Console::ColorRed);
            break;

          case 42:
            _console.setTextBackground(Console::ColorGreen);
            break;

          case 43:
            _console.setTextBackground(Console::ColorYellow);
            break;

          case 44:
            _console.setTextBackground(Console::ColorBlue);
            break;

          case 45:
            _console.setTextBackground(Console::ColorMagenta);
            break;

          case 46:
            _console.setTextBackground(Console::ColorCyan);
            break;

          case 47:
            _console.setTextBackground(Console::ColorWhite);
            break;
        }
      }

      break;
    }

    case 'J':
    {
      if((argc == 1) && (argv[0] == 2))
        _console.clear();

      break;
    }

    case 'H':
    {
      if((argc == 2) && (argv[0] > 0) && (argv[1] > 0))
        _console.moveCursor(--argv[0], --argv[1]);

      break;
    }
  }
}

#endif // CCXX_OS_WINDOWS


} // namespace ccxx
