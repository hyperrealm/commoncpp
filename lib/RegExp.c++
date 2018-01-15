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

#include "commonc++/RegExp.h++"
#include "commonc++/ParseException.h++"

#ifdef CCXX_OS_WINDOWS_GNU
#define PCRE_STATIC
#endif

#include "pcre.h"

namespace ccxx {

/*
 */

RegExp::RegExp()
  : _pattern(String::null),
    _regex(NULL),
    _compiled(false)
{
}

/*
 */

RegExp::RegExp(const RegExp& other)
  : _regex(NULL),
    _compiled(false)
{
  setPattern(other._pattern);
}

/*
 */

RegExp::~RegExp()
{
  if(_compiled)
    pcre16_free(_regex);
}

/*
 */

RegExp& RegExp::operator=(const RegExp& other)
{
  if(this != &other)
    setPattern(other._pattern);

  return(*this);
}

/*
 */

void RegExp::setPattern(const String& pattern,
                        bool caseInsensitive /* = false */)
{
  if(_compiled)
  {
    pcre16_free(_regex);
    _compiled = false;
  }

  int options = 0;
  if(caseInsensitive)
    options |= PCRE_CASELESS;

  const char *error_message;
  int error_offset;
  _regex = pcre16_compile((PCRE_SPTR16)(pattern.data()), options,
                          &error_message, &error_offset, NULL);
  if(_regex == NULL) {
    throw ParseException(String(error_message), 0, error_offset);
  }

  _pattern = pattern;
  _compiled = true;
}

/*
 */

bool RegExp::match(const String& text) const
{
  RegExpMatch matches[1];

  if(!match(text, matches, 1))
    return(false);

  return(matches[0].isValid());
}

/*
 */

bool RegExp::match(const String& text, RegExpMatch matches[], int numMatches)
  const
{
  if(! _compiled)
    return(false);

  int options = 0;
  int* ovector = new int[numMatches * 3];

  int r =  pcre16_exec(reinterpret_cast<pcre16*>(_regex), NULL,
                       reinterpret_cast<PCRE_SPTR16>(text.data()),
                       (text.length() * sizeof(char16_t)),
                       0, options, ovector, numMatches * 3);
  if (r <= 0)
  {
    delete[] ovector;
    return(false);
  }

  int* n = ovector;
  for(int i = 0; i < r; ++i)
  {
    matches[i].rm_so = *n;
    ++n;
    matches[i].rm_eo = *n;
    ++n;
  }

  delete[] ovector;

  for(int i = r; i < numMatches; ++i)
    matches[i].rm_so = matches[i].rm_eo = -1;

  return(true);
}

/*
 */

String RegExp::escapeMeta(const String& text)
{
  if(text.isEmpty())
    return(text);

  String s;

  for(uint_t i = 0; i < text.length(); ++i)
  {
    Char c = text[i];

    if(!(c.isAlphaNumeric() || (c == '_')) || !c.isLowASCII())
      s += '\\';

    s += c;
  }

  return(s);
}


} // namespace ccxx
