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

#include "commonc++/XMLElementIterator.h++"
#include "commonc++/XMLElement.h++"

namespace ccxx {

/*
 */

XMLElementIterator::XMLElementIterator(
  XMLElement& elem, const String& name /* = String::null */)
  : _parent(&elem),
    _name(name),
    _prev(NULL),
    _current(NULL),
    _next(NULL)
{
  _findNext();
}

/*
 */

XMLElementIterator::~XMLElementIterator()
{
}

/*
 */

void XMLElementIterator::rewind()
{
  _prev = _current = NULL;
  _findNext();
}

/*
 */

XMLElement& XMLElementIterator::next()
{
  _prev = _current;
  _findNext();

  if(! _current)
    throw OutOfBoundsException();

  return(*_current);
}

/*
 */

void XMLElementIterator::_findNext()
{
  if(_next != NULL)
  {
    _current = _next;
    _next = NULL;
  }

  _current = (_current ? _current->_next : _parent->_firstChild);

  if(_name)
  {
    while(_current)
    {
      if(_current->_name == _name)
        break;

      _current = _current->_next;
    }
  }
}

/*
 */

bool XMLElementIterator::hasNext()
{
  return((_next != NULL) || ((_current != NULL) && (_current->_next != NULL)));
}

/*
 */

void XMLElementIterator::remove()
{
  if(_current == NULL)
    return;

  _next = _current->_next;

  if(_prev == NULL)
    _parent->_firstChild = _next;
  else
    _prev->_next = _next;

  delete _current;
  _current = NULL;
}


} // namespace ccxx
