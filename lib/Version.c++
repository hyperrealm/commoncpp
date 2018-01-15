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

#include "commonc++/Version.h++"

namespace ccxx {

/*
 */

Version::Version(const String& version)
{
  fromString(version);
}

/*
 */

Version::Version(uint_t current /* = 0 */, uint_t revision /* = 0 */,
                 uint_t age /* = 0 */)
  : _current(current),
    _revision(revision),
    _age(age <= current ? age : current),
    _str(String::null),
    _vstr(String::null)
{
}

/*
 */

Version::Version(const Version& other)
  : _current(other._current),
    _revision(other._revision),
    _age(other._age),
    _str(String::null),
    _vstr(String::null)
{
}

/*
 */

Version::~Version()
{
}

/*
 */

bool Version::operator==(const Version& other) const
{
  return((_current == other._current) && (_revision == other._revision)
         && (_age == other._age));
}

/*
 */

Version& Version::operator=(const Version& other)
{
  if(&other != this)
  {
    _current = other._current;
    _revision = other._revision;
    _age = other._age;
    _str = String::null;
    _vstr = String::null;
  }

  return(*this);
}

/*
 */

bool Version::satisfies(const Version& requested) const
{
  return((_current >= requested._current)
         && ((_current - _age) <= requested._current));
}

/*
 */

bool Version::satisfiedBy(const Version& available) const
{
  return(((available._current - available._age) <= _current)
         && (available._current >= _current));
}

/*
 */

String Version::toCRAString() const
{
  if(_vstr.isNull())
    _vstr << _current << ':' << _revision << ':' << _age;

  return(_vstr);
}

/*
 */

void Version::interfacesAdded()
{
  ++_age;
  _str = String::null;
  _vstr = String::null;
}

/*
 */

void Version::interfacesRemoved()
{
  _age = 0;
  _str = String::null;
  _vstr = String::null;
}

/*
 */

void Version::interfacesChanged()
{
  ++_current;
  _revision = 0;
  _str = String::null;
  _vstr = String::null;
}

/*
 */

void Version::implementationChanged()
{
  ++_revision;
  _str = String::null;
  _vstr = String::null;
}

/*
 */

String Version::toString() const
{
  if(_str.isNull())
    _str << (_current - _age) << '.' << _age << '.' << _revision;

  return(_str);
}

/*
 */

void Version::fromString(const String& version)
{
  _str = String::null;
  _vstr = String::null;

  uint_t index = 0;
  uint_t v[3] = { 0, 0, 0 };
  String t;

  for(int i = 0; i < 3; i++)
  {
    t = version.nextToken(index, ":.");
    int x = t.toInt();
    if(x >= 0)
      v[i] = static_cast<uint_t>(x);
  }

  if(version.contains('.'))
  {
    // C-A.A.R

    _age = v[1];
    _revision = v[2];
    _current = v[0] + _age;

    if(_age > _current)
      _age = _current;
  }
  else
  {
    // C:R:A

    _current = v[0];
    _revision = v[1];
    _age = (v[2] <= _current) ? v[2] : _current;
  }
}

/*
 */

bool Version::operator<(const Version& other) const
{
  if(_current < other._current)
    return(true);
  else if(_current > other._current)
    return(false);
  else
    return(_revision < other._revision);
}

/*
 */

bool Version::operator>(const Version& other) const
{
  if(_current > other._current)
    return(true);
  else if(_current < other._current)
    return(false);
  else
    return(_revision > other._revision);
}


} // namespace ccxx
