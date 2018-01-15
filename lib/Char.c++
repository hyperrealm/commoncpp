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

#include "commonc++/Char.h++"

#include <cwctype>

namespace ccxx {

// static
const Char Char::nul;
const Char Char::BELL = Char('\a');
const Char Char::BACKSPACE = Char('\b');
const Char Char::TAB = Char('\t');
const Char Char::NEWLINE = Char(0x0A);
const Char Char::VERTICAL_TAB = Char('\v');
const Char Char::FORM_FEED = Char('\f');
const Char Char::RETURN = Char('\r');
const Char Char::ESCAPE = Char(27);
const Char Char::SPACE = Char(' ');
const Char Char::DELETE = Char(127);

/*
 */

Char::Char()
  : _char(0)
{
}

/*
 */

Char::Char(char c)
  : _char(static_cast<char16_t>(c))
{
}

/*
 */

Char::Char(char16_t c)
  : _char(c)
{
}

/*
 */

Char::Char(int c)
  : _char(static_cast<char16_t>(c & 0xFFFF))
{
}

/*
 */

Char::~Char()
{
}

/*
 */

Char& Char::operator=(char c)
{
  _char = static_cast<char16_t>(c);

  return(*this);
}

/*
 */

Char& Char::operator=(char16_t c)
{
  _char = c;

  return(*this);
}

/*
 */

Char& Char::operator=(int c)
{
  _char = static_cast<char16_t>(c & 0xFFFF);

  return(*this);
}

/*
 */

bool Char::isNul() const
{
  return(Char::isNul(_char));
}

/*
 */

bool Char::isControl() const
{
  return(Char::isControl(_char));
}

/*
 */

bool Char::isPrintable() const
{
  return(Char::isPrintable(_char));
}

/*
 */

bool Char::isGraphical() const
{
  return(Char::isGraphical(_char));
}

/*
 */

bool Char::isUpperCase() const
{
  return(isUpperCase(_char));
}

/*
 */

bool Char::isLowerCase() const
{
  return(isLowerCase(_char));
}

/*
 */

Char Char::toLowerCase() const
{
  return(Char(toLowerCase(_char)));
}

/*
 */

Char Char::toUpperCase() const
{
  return(Char(toUpperCase(_char)));
}

/*
 */

bool Char::isSign() const
{
  return(isSign(_char));
}

/*
 */

bool Char::isLetter() const
{
  return(isLetter(_char));
}

/*
 */

bool Char::isDigit() const
{
  return(isDigit(_char));
}

/*
 */

bool Char::isHexDigit() const
{
  return(isHexDigit(_char));
}

/*
 */

bool Char::isAlphabetic() const
{
  return(Char::isAlphabetic(_char));
}

/*
 */

bool Char::isAlphaNumeric() const
{
  return(Char::isAlphaNumeric(_char));
}

/*
 */

bool Char::isWhitespace() const
{
  return(Char::isWhitespace(_char));
}

/*
 */

bool Char::isASCII() const
{
  return(Char::isASCII(_char));
}

/*
 */

char Char::toASCII() const
{
  if(!Char::isASCII(_char))
    return(0);

  return(static_cast<char>(_char & 0xFF));
}

/*
 */

bool Char::isLowASCII() const
{
  return(Char::isLowASCII(_char));
}

/*
 */

bool Char::isHighASCII() const
{
  return(Char::isHighASCII(_char));
}

/*
 */

bool Char::isLowSurrogate() const
{
  return(Char::isLowSurrogate(_char));
}

/*
 */

bool Char::isHighSurrogate() const
{
  return(Char::isHighSurrogate(_char));
}

/*
 */

// static
bool Char::isNul(char16_t c)
{
  return(c == 0);
}

/*
 */

// static
bool Char::isControl(char16_t c)
{
  return(::iswcntrl(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isPrintable(char16_t c)
{
  return(::iswprint(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isGraphical(char16_t c)
{
  return(::iswgraph(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isPunctuation(char16_t c)
{
  return(::iswpunct(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isLetter(char16_t c)
{
  return(::iswalpha(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isDigit(char16_t c)
{
  return(::iswdigit(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isHexDigit(char16_t c)
{
  return(::iswxdigit(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isAlphabetic(char16_t c)
{
  return(::iswalpha(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isAlphaNumeric(char16_t c)
{
  return(::iswalnum(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isSign(char16_t c)
{
  return((c == '+') || (c == '-'));
}

/*
 */

//static
bool Char::isWhitespace(char16_t c)
{
  return(::iswspace(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isLowerCase(char16_t c)
{
  return(::iswlower(static_cast<wint_t>(c)));
}

/*
 */

// static
bool Char::isUpperCase(char16_t c)
{
  return(::iswupper(static_cast<wint_t>(c)));
}

/*
 */

// static
char16_t Char::toLowerCase(char16_t c)
{
  return(static_cast<char16_t>(::towlower(static_cast<wint_t>(c))));
}

/*
 */

// static
char16_t Char::toUpperCase(char16_t c)
{
  return(static_cast<char16_t>(::towupper(static_cast<wint_t>(c))));
}

/*
 */

// static
bool Char::isASCII(char16_t c)
{
  return(c <= 0x00FF);
}

/*
 */

// static
bool Char::isLowASCII(char16_t c)
{
  return(c < 0x0080);
}

/*
 */

// static
bool Char::isHighASCII(char16_t c)
{
  return((c >= 0x0080) && (c <= 0x00FF));
}

/*
 */

// static
bool Char::isLowSurrogate(char16_t c)
{
  return((c & 0xFC00) == 0xDC00);
}

/*
 */

// static
bool Char::isHighSurrogate(char16_t c)
{
  return((c & 0xFC00) == 0xD800);
}

/*
 */

// static
char Char::toASCII(char16_t c)
{
  return((c <= 0xFF) ? static_cast<char>(c & 0xFF) : 0);
}


} // namespace ccxx
