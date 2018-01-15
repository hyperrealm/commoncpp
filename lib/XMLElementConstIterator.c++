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

#include "commonc++/XMLElementConstIterator.h++"
#include "commonc++/XMLElement.h++"

namespace ccxx {

/*
 */

XMLElementConstIterator::XMLElementConstIterator(
  const XMLElement& elem, const String& name /* = String::null */)
  : _parent(&elem),
    _name(name),
    _next(NULL),
    _prev(NULL)
{
  _findNext();
}

/*
 */

XMLElementConstIterator::~XMLElementConstIterator()
{
}

/*
 */

void XMLElementConstIterator::rewind()
{
  _next = _prev = NULL;
  _findNext();
}

/*
 */

const XMLElement& XMLElementConstIterator::next()
{
  if(! _next)
    throw OutOfBoundsException();

  XMLElement *current = _next;
  _findNext();

  return(*current);
}

/*
 */

void XMLElementConstIterator::_findNext()
{
  _next = (_next ? _next->_next : _parent->_firstChild);

  if(_name)
  {
    while(_next)
    {
      if(_next->_name == _name)
        break;

      _next = _next->_next;
    }
  }

}

/*
 */

bool XMLElementConstIterator::hasNext()
{
  return(_next != NULL);
}

/*
 */

void XMLElementConstIterator::remove()
{
  throw UnsupportedOperationException();
}


} // namespace ccxx
