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

#include "commonc++/Common.h++"
#include "commonc++/DeleteFunctor.h++"
#include "commonc++/File.h++"
#include "commonc++/LogFormat.h++"
#include "commonc++/Private.h++"
#include "commonc++/Process.h++"
#include "commonc++/System.h++"
#include "commonc++/TerminalAttr.h++"
#include "commonc++/Thread.h++"

#include <algorithm>
#include <cstring>
#include <vector>

namespace ccxx {

/*
 */

static const char _formatTokens[] = { '=', 'T', 't', 'D', 'd', 'F', 'L', 'm',
                                      'h', 'p', 'o', 'a', 'v', 'u', 'H', 'l',
                                      '%', '*', '_', '#', '-', '0', '1', '2',
                                      '3', '4', '5', '6', '7', '8', '.', '@',
                                      'n', 'A' };

static const char _levelTokens[] = { NUL, 'D', 'I', NUL, 'W', NUL, NUL, NUL,
                                     'E' };

size_t LogFormat::_eolLen = std::strlen(File::eol);

/*
 * bounded string output functions
 */

static size_t __bsnputs(char *buf, size_t maxlen, const char *str)
{
  size_t len = 0;
  const char *q = str;
  for(char *p = buf; *q && maxlen; ++p, ++q, ++len, --maxlen)
    *p = *q;

  return(len);
}

/*
 */

static size_t __bsnputsf(char *buf, size_t maxlen, const char *str)
{
  size_t len = std::strlen(str);

  if(len > maxlen)
    return(0);

  std::strncpy(buf, str, len);
  return(len);
}

/*
 */

static size_t __bvsnprintf(char *buf, size_t maxlen, const char *fmt,
                           va_list vp)
{
  int n = ::vsnprintf(buf, maxlen, fmt, vp);

  // some platforms return a negative value if output was truncated, some a
  // value greater than the buffer length; this function always returns the
  // number of bytes written, since knowledge of a truncation isn't needed
  return(((n < 0) || (n > static_cast<int>(maxlen))) ? maxlen : n);
}

/*
 */

static size_t __bsnprintf(char *buf, size_t maxlen, const char *fmt, ...)
{
  va_list vp;

  va_start(vp, fmt);
  size_t n = __bvsnprintf(buf, maxlen, fmt, vp);
  va_end(vp);

  return(n);
}

/*
 */

class LogFormat::TokenList : public std::vector<Token *>
{
};

/*
 */

LogFormat::LogFormat(String format /* = "[%D %T] %F:%L %m" */)
  : _tokens(new TokenList()),
    _shortDateFormat("%0y/%0m/%0d"),
    _longDateFormat("%0d-%$m-%0Y"),
    _shortTimeFormat("%_H:%0M:%0S"),
    _longTimeFormat("%_H:%0M:%0S.%0s")
{
  setFormat(format);
}

/*
 */

LogFormat::~LogFormat()
{
  clearTokens();
  delete _tokens;
}

/*
 */

void LogFormat::setFormat(const String& format)
{
  bool escaped = false;
  int i, slen = 0;
  Token tok;
  TokenCode t;
  char c;
  const char *p, *s0 = NULL;

  clearTokens();

  CString cstr_format = format.toUTF8();

  for(i = 0, p = cstr_format.data(); *p; i++, p++)
  {
    c = *p;
    if(! s0)
      s0 = p;

    if(c == '%' && !escaped)
    {
      if(slen > 0)
      {
        Token *tok = new Token(TOK_LITERAL, s0, slen);
        _tokens->push_back(tok);

        slen = 0;
        s0 = NULL;
      }

      escaped = true;
    }
    else if(escaped)
    {
      t = lookupToken(c);
      if(t > TOK_LITERAL)
      {
        Token *tok = new Token(t);
        _tokens->push_back(tok);
      }

      escaped = false;
      s0 = NULL;
    }
    else
      slen++;
  }

  if((slen > 0) && s0)
  {
    Token *tok = new Token(TOK_LITERAL, s0, slen);
    _tokens->push_back(tok);
  }
}

/*
 */

void LogFormat::format(CharBuffer& buffer, LogLevel level, const char* file,
                       int line, const char* message, va_list args)
{
  const char* p;
  size_t len;
  DateTime now;

  size_t lim = buffer.getLimit();
  if(lim < _eolLen)
    return;

  buffer.setLimit(lim - _eolLen);

  for(TokenList::const_iterator iter = _tokens->begin();
      (iter != _tokens->end()) && buffer.hasRemaining();
      iter++)
  {
    Token *tok = *iter;

    switch(tok->_token)
    {
      case TOK_LITERAL:
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(),
                        tok->_text);
        buffer.bump(len);
        break;

      case TOK_TIME_LONG:
        len = _longTimeFormat.format(now, buffer.getPointer(),
                                     buffer.getRemaining());
        buffer.bump(len);
        break;

      case TOK_TIME_SHORT:
        len = _shortTimeFormat.format(now, buffer.getPointer(),
                                      buffer.getRemaining());
        buffer.bump(len);
        break;

      case TOK_DATE_LONG:
        len = _longDateFormat.format(now, buffer.getPointer(),
                                     buffer.getRemaining());
        buffer.bump(len);
        break;

      case TOK_DATE_SHORT:
        len = _shortDateFormat.format(now, buffer.getPointer(),
                                      buffer.getRemaining());
        buffer.bump(len);
        break;

      case TOK_FILE:
        p = std::strrchr(file, File::separator);
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(),
                        (p ? ++p : file));
        buffer.bump(len);
        break;

      case TOK_LINE:
        len = __bsnprintf(buffer.getPointer(), buffer.getRemaining(), "%d",
                          line);
        buffer.bump(len);
        break;

      case TOK_MESSAGE:
        len = __bvsnprintf(buffer.getPointer(), buffer.getRemaining(), message,
                           args);
        buffer.bump(len);
        break;

      case TOK_THREAD:
        len = __bsnprintf(buffer.getPointer(), buffer.getRemaining(),
                          "%8X", (Thread::currentThreadID()));
        buffer.bump(len);
        break;

      case TOK_PID:
        len = __bsnprintf(buffer.getPointer(), buffer.getRemaining(),
                          "%5d", (int)(Process::currentProcessID()));
        buffer.bump(len);
        break;

      case TOK_OSNAME:
      {
        CString cstr_osname = System::getOSName().toUTF8();
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(),
                        cstr_osname.data());
        buffer.bump(len);
        break;
      }

      case TOK_ARCH:
      {
        CString cstr_arch = System::getOSName().toUTF8();
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(),
                      cstr_arch.data());
        buffer.bump(len);
        break;
      }

      case TOK_OSVER:
      {
        CString cstr_osver = System::getOSVersion().toUTF8();
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(),
                        cstr_osver.data());
        buffer.bump(len);
        break;
      }

      case TOK_USERNAME:
      {
        CString cstr_username = System::getUserName().toUTF8();
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(),
                        cstr_username.data());
        buffer.bump(len);
        break;
      }

      case TOK_HOST:
      {
        CString cstr_hostname = System::getHostName().toUTF8();
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(),
                        cstr_hostname.data());
        buffer.bump(len);
        break;
      }

      case TOK_APPNAME:
      {
        CString progname = Process::getProgramName().toUTF8();
        p = progname.data();
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(),
                        (p ? p : "(N/A)"));
        buffer.bump(len);
        break;
      }

      case TOK_LEVEL:
        if(buffer.getRemaining() > 0)
        {
          char *p = buffer.getPointer();
          *p = _levelTokens[level];
          buffer.bump(1);
          break;
        }

      case TOK_PERCENT:
        len = __bsnputs(buffer.getPointer(), buffer.getRemaining(), "%");
        buffer.bump(len);
        break;

      case TOK_BOLD:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_BOLD);
        buffer.bump(len);
        break;

      case TOK_UNDERLINE:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_UNDERLINE);
        buffer.bump(len);
        break;

      case TOK_INVERSE:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_INVERSE);
        buffer.bump(len);
        break;

      case TOK_PLAIN:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_NORMAL);
        buffer.bump(len);
        break;

      case TOK_DEFAULT:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_NORMAL);
        buffer.bump(len);
        break;

      case TOK_RED:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_RED);
        buffer.bump(len);
        break;

      case TOK_GREEN:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_GREEN);
        buffer.bump(len);
        break;

      case TOK_YELLOW:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_YELLOW);
        buffer.bump(len);
        break;

      case TOK_BLUE:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_BLUE);
        buffer.bump(len);
        break;

      case TOK_MAGENTA:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_MAGENTA);
        buffer.bump(len);
        break;

      case TOK_CYAN:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_CYAN);
        buffer.bump(len);
        break;

      case TOK_WHITE:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_WHITE);
        buffer.bump(len);
        break;

      case TOK_BLACK:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_BLACK);
        buffer.bump(len);
        break;

      case TOK_STYLESOFF:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_FG_NORMAL);
        buffer.bump(len);
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_BG_NORMAL);
        buffer.bump(len);
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_NORMAL);
        buffer.bump(len);
        break;

      case TOK_AUTOCOLOR:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         TERMATTR_BOLD);
        buffer.bump(len);

        switch(level)
        {
          case LogDebug:
            len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
#ifdef CCXX_OS_WINDOWS
                             TERMATTR_FG_CYAN
#else
                             TERMATTR_FG_BLUE
#endif
                             );
            buffer.bump(len);
            break;

          case LogInfo:
            len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                           TERMATTR_FG_NORMAL);
            buffer.bump(len);
            break;

          case LogWarning:
            len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                           TERMATTR_FG_MAGENTA);
            buffer.bump(len);
            break;

          case LogError:
            len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                           TERMATTR_FG_RED);
            buffer.bump(len);
            break;
        }
        break;

      case TOK_NEWLINE:
        len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(),
                         File::eol);
        buffer.bump(len);
        break;

      case TOK_INVALID:
      case TOK_MAX:
        break;
    }
  }

  buffer.setLimit(lim);
  len = __bsnputsf(buffer.getPointer(), buffer.getRemaining(), File::eol);
  buffer.bump(len);
}

/*
 */

void LogFormat::clearTokens()
{
  std::for_each(_tokens->begin(), _tokens->end(), DeleteFunctor());
  _tokens->clear();
}

/*
 */

void LogFormat::setShortDateFormat(const String &format)
{
  _shortDateFormat.setFormat(format);
}

/*
 */

void LogFormat::setLongDateFormat(const String &format)
{
  _longDateFormat.setFormat(format);
}


/*
 */

void LogFormat::setShortTimeFormat(const String &format)
{
  _shortTimeFormat.setFormat(format);
}

/*
 */

void LogFormat::setLongTimeFormat(const String &format)
{
  _longTimeFormat.setFormat(format);
}

/*
 */

LogFormat::TokenCode LogFormat::lookupToken(char c)
{
  for(int i = 1; i < TOK_MAX; ++i)
  {
    if(c == _formatTokens[i])
      return(static_cast<TokenCode>(i));
  }

  return(TOK_INVALID);
}

/*
 */

LogFormat::Token::Token(LogFormat::TokenCode token /* = TOK_LITERAL */,
                        const char *text /* = NULL */, int len /* = 0 */)
  : _token(token)
{
  if(text != NULL)
  {
    _text = new char[len + 1];
    std::memcpy(_text, text, len);
    _text[len] = NUL;
  }
  else
    _text = NULL;
}

/*
 */

LogFormat::Token::~Token()
{
  delete[] _text;
}


} // namespace ccxx
