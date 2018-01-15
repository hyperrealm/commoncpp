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

#include "commonc++/Console.h++"
#include "commonc++/TerminalAttr.h++"

#ifdef CCXX_OS_POSIX
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <cstring>

namespace ccxx {

/*
 */

#ifdef CCXX_OS_WINDOWS

#ifndef COMMON_LVB_REVERSE_VIDEO
#define COMMON_LVB_REVERSE_VIDEO 0x4000
#endif

#ifndef COMMON_LVB_UNDERSCORE
#define COMMON_LVB_UNDERSCORE 0x8000
#endif

Console::Console()
  : _attributes(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE),
    _intense(false),
    _bold(false)
{
  _console = ::GetStdHandle(STD_ERROR_HANDLE);
  ::SetConsoleTextAttribute(_console, _attributes);
}

#else

Console::Console()
  : _underline(false),
    _inverse(false),
    _bold(false)
{
  _console = stderr;
}

#endif

/*
 */

Console::~Console()
{
}

/*
 */

size_t Console::read(char *text, size_t maxlen)
{
#ifdef CCXX_OS_WINDOWS

  DWORD r = 0;

  if(! ::ReadConsole(_console, reinterpret_cast<VOID *>(text),
                     static_cast<DWORD>(maxlen - 1), &r, NULL))
    return(0);

  *(text + r) = NUL;
  return(static_cast<size_t>(r));

#else

  if(! ::fgets(text, maxlen, stdin))
    return(0);

  return(std::strlen(text));

#endif
}

/*
 */

void Console::write(const char *text, size_t len /* = 0 */)
{
#ifdef CCXX_OS_WINDOWS

  DWORD r;

  if(len == 0)
    len = std::strlen(text);

  ::WriteConsole(_console, reinterpret_cast<const VOID *>(text),
                 static_cast<DWORD>(len), &r, NULL);

#else

  ::fwrite(text, len, 1, stderr);
  ::fflush(stderr);

#endif
}

/*
 */

bool Console::setEcho(bool on)
{
#ifdef CCXX_OS_WINDOWS

  DWORD mode = 0;
  if(! ::GetConsoleMode(_console, &mode))
    return(false);

  if(on)
    mode |= (ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
  else
    mode &= ~ENABLE_ECHO_INPUT;

  return(::SetConsoleMode(_console, mode) == TRUE);

#else

  struct termios tattr;

  if(::tcgetattr(STDIN_FILENO, &tattr) < 0)
    return(false);

  if(on)
    tattr.c_lflag |= ECHO;
  else
    tattr.c_lflag &= ~ECHO;

  return(::tcsetattr(STDIN_FILENO, TCSANOW, &tattr) == 0);

#endif
}

/*
 */

void Console::moveCursor(uint_t row, uint_t column)
{
#ifdef CCXX_OS_WINDOWS

  if(column > INT16_MAX)
    column = 0;

  if(row > INT16_MAX)
    row = 0;

  COORD pos = { (SHORT)column, (SHORT)row };

  ::SetConsoleCursorPosition(_console, pos);

#else


  ::fprintf(_console, TERMATTR_ESC "[%d;%dH", row, column);
  ::fflush(_console);

#endif
}

/*
 */

void Console::clear()
{
#ifdef CCXX_OS_WINDOWS

  CONSOLE_SCREEN_BUFFER_INFO csbi;

  if(! ::GetConsoleScreenBufferInfo(_console, &csbi))
    return;

  DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  DWORD r;
  COORD coord = { 0, 0 };

  ::FillConsoleOutputCharacter(_console, (TCHAR)' ', dwConSize, coord, &r);

  ::GetConsoleScreenBufferInfo(_console, &csbi);

  ::FillConsoleOutputAttribute(_console, csbi.wAttributes, dwConSize, coord,
                               &r);

  ::SetConsoleCursorPosition(_console, coord);

#else

  ::fputs(TERMATTR_CLEAR_SCREEN, _console);
  ::fputs(TERMATTR_CURSOR(0, 0), _console);
  ::fflush(_console);

#endif
}

/*
 */

void Console::setTextForeground(TextColor color)
{
#ifdef CCXX_OS_WINDOWS

  _attributes &= ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  switch(color)
  {
    case ColorBlack:
      _intense = false;
      if(_bold)
        _attributes |= FOREGROUND_INTENSITY;
      break;

    case ColorRed:
      _attributes |= FOREGROUND_RED | FOREGROUND_INTENSITY;
      _intense = true;
      break;

    case ColorGreen:
      _attributes |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
      _intense = true;
      break;

    case ColorYellow:
      _attributes |= (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
      _intense = true;
      break;

    case ColorBlue:
      _attributes |= (FOREGROUND_BLUE | FOREGROUND_INTENSITY);
      _intense = true;
      break;

    case ColorMagenta:
      _attributes |= (FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
      _intense = true;
      break;

    case ColorCyan:
      _attributes |= (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
      _intense = true;
      break;

    case ColorWhite:
      _attributes |= (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
      _intense = false;
      if(_bold)
        _attributes |= FOREGROUND_INTENSITY;
      else
        _attributes &= ~FOREGROUND_INTENSITY;
      break;
  }

  ::SetConsoleTextAttribute(_console, _attributes);

#else

  switch(color)
  {
    case ColorBlack:
      ::fputs(TERMATTR_FG_BLACK, _console);
      break;

    case ColorRed:
      ::fputs(TERMATTR_FG_RED, _console);
      break;

    case ColorGreen:
      ::fputs(TERMATTR_FG_GREEN, _console);
      break;

    case ColorYellow:
      ::fputs(TERMATTR_FG_YELLOW, _console);
      break;

    case ColorBlue:
      ::fputs(TERMATTR_FG_BLUE, _console);
      break;

    case ColorMagenta:
      ::fputs(TERMATTR_FG_MAGENTA, _console);
      break;

    case ColorCyan:
      ::fputs(TERMATTR_FG_CYAN, _console);
      break;

    case ColorWhite:
      ::fputs(TERMATTR_FG_WHITE, _console);
      break;
  }

  ::fflush(_console);

#endif
}

/*
 */

void Console::setTextBackground(TextColor color)
{
#ifdef CCXX_OS_WINDOWS

  _attributes &= ~(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);

  switch(color)
  {
    case ColorBlack:
      break;

    case ColorRed:
      _attributes |= BACKGROUND_RED;
      break;

    case ColorGreen:
      _attributes |= BACKGROUND_GREEN;
      break;

    case ColorYellow:
      _attributes |= (BACKGROUND_RED | BACKGROUND_GREEN);
      break;

    case ColorBlue:
      _attributes |= BACKGROUND_BLUE;
      break;

    case ColorMagenta:
      _attributes |= (BACKGROUND_RED | BACKGROUND_BLUE);
      break;

    case ColorCyan:
      _attributes |= (BACKGROUND_GREEN | BACKGROUND_BLUE);
      break;

    case ColorWhite:
      _attributes |= (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
  }

  ::SetConsoleTextAttribute(_console, _attributes);

#else

  switch(color)
  {
    case ColorBlack:
      ::fputs(TERMATTR_BG_BLACK, _console);
      break;

    case ColorRed:
      ::fputs(TERMATTR_BG_RED, _console);
      break;

    case ColorGreen:
      ::fputs(TERMATTR_BG_GREEN, _console);
      break;

    case ColorYellow:
      ::fputs(TERMATTR_BG_YELLOW, _console);
      break;

    case ColorBlue:
      ::fputs(TERMATTR_BG_BLUE, _console);
      break;

    case ColorMagenta:
      ::fputs(TERMATTR_BG_MAGENTA, _console);
      break;

    case ColorCyan:
      ::fputs(TERMATTR_BG_CYAN, _console);
      break;

    case ColorWhite:
      ::fputs(TERMATTR_BG_WHITE, _console);
      break;
  }

  ::fflush(_console);

#endif
}

/*
 */

void Console::beginTextStyle(TextStyle style)
{
#ifdef CCXX_OS_WINDOWS

  switch(style)
  {
    case StyleBold:
      _attributes |= FOREGROUND_INTENSITY;
      _bold = true;
      break;

    case StyleUnderline:
      // currently not supported by Win32
      _attributes |= COMMON_LVB_UNDERSCORE;
      break;

    case StyleInverse:
      // currently not supported by Win32
      _attributes |= COMMON_LVB_REVERSE_VIDEO;
      break;
  }

  ::SetConsoleTextAttribute(_console, _attributes);

#else

  switch(style)
  {
    case StyleBold:
      ::fputs(TERMATTR_BOLD, _console);
      _bold = true;
      break;

    case StyleUnderline:
      ::fputs(TERMATTR_UNDERLINE, _console);
      _underline = true;
      break;

    case StyleInverse:
      ::fputs(TERMATTR_INVERSE, _console);
      _inverse = true;
      break;
  }

  ::fflush(_console);

#endif
}

/*
 */

void Console::endTextStyle(TextStyle style)
{
#ifdef CCXX_OS_WINDOWS

  switch(style)
  {
    case StyleBold:
      if(! _intense)
        _attributes &= ~FOREGROUND_INTENSITY;
      _bold = false;
      break;

    case StyleUnderline:
      _attributes &= ~COMMON_LVB_UNDERSCORE;
      break;

    case StyleInverse:
      _attributes &= ~COMMON_LVB_REVERSE_VIDEO;
      break;
  }

  ::SetConsoleTextAttribute(_console, _attributes);

#else

  ::fputs(TERMATTR_NORMAL, _console);

  switch(style)
  {
    case StyleBold:
      _bold = false;
      break;

    case StyleUnderline:
      _underline = false;
      break;

    case StyleInverse:
      _inverse = false;
      break;
  }

  if(_bold)
    ::fputs(TERMATTR_BOLD, _console);
  if(_underline)
    ::fputs(TERMATTR_UNDERLINE, _console);
  if(_inverse)
    ::fputs(TERMATTR_INVERSE, _console);

  ::fflush(_console);

#endif
}

/*
 */

void Console::resetTextStyle()
{
#ifdef CCXX_OS_WINDOWS

  _attributes &= ~(FOREGROUND_INTENSITY | COMMON_LVB_REVERSE_VIDEO
                   | COMMON_LVB_UNDERSCORE);
  _bold = false;

  ::SetConsoleTextAttribute(_console, _attributes);

#else

  _bold = _underline = _inverse = false;
  ::fputs(TERMATTR_NORMAL, _console);
  ::fflush(_console);

#endif
}

/*
 */

void Console::setTitle(const String &title)
{
#ifdef CCXX_OS_WINDOWS

  ::SetConsoleTitleW(title.data());

#else

  CString cstr_title = title.toUTF8();
  ::fputs("\033]0;", _console);
  ::fputs(cstr_title.data(), _console);
  ::fputs("\a", _console);
  ::fflush(_console);

#endif
}

/*
 */

bool Console::getSize(uint_t *columns, uint_t *rows)
{
#ifdef CCXX_OS_WINDOWS

  CONSOLE_SCREEN_BUFFER_INFO csbi;

  if(::GetConsoleScreenBufferInfo(_console, &csbi) == FALSE)
    return(false);

  if(columns)
    *columns = csbi.dwSize.X;

  if(rows)
    *rows = csbi.dwSize.Y;

  return(true);

#else

#if defined(TIOCGWINSZ)

  struct winsize ws;

  if(::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
  {
    if(columns)
      *columns = ws.ws_col;

    if(rows)
      *rows = ws.ws_row;

    return(true);
  }
  else
    return(false);

#elif defined(TIOCGSIZE)

  struct ttysize ts;

  if(::ioctl(STDOUT_FILENO, TIOCGSIZE, &ts) == 0)
  {
    if(columns)
      *columns = ts.ts_cols;

    if(rows)
      *rows = ts.ts_lines;
    return(true);
  }
  else
    return(false);

#else

  // no way to determine terminal size

  return(false);

#endif // TIOCGSIZE

#endif
}


} // namespace ccxx
