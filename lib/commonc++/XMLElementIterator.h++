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

#ifndef __ccxx_XMLElementIterator_hxx
#define __ccxx_XMLElementIterator_hxx

#include <commonc++/Common.h++>
#include <commonc++/Iterator.h++>
#include <commonc++/String.h++>
#include <commonc++/UnsupportedOperationException.h++>

namespace ccxx {

class XMLElement; // fwd decl

/**
 * An XMLElement iterator.
 *
 * @author Mark Lindner
 */
class COMMONCPPXML_API XMLElementIterator : public Iterator<XMLElement&>
{
 public:

  /**
   * Construct a new XMLElementIterator.
   *
   * @param elem The XMLElement whose child elements will be iterated over.
   * @param name If not String::null, indicates that only elements with
   * the given name should be iterated over.
   */
  XMLElementIterator(XMLElement& elem, const String& name = String::null);

  /** Destructor. */
  ~XMLElementIterator();

  void rewind();

  /**
   * @throw OutOfBoundsException If the iterator is already at the end.
   */
  XMLElement& next();

  bool hasNext();

  void remove();

 private:

  void _findNext();

  XMLElement* _parent;
  String _name;
  XMLElement* _prev;
  XMLElement* _current;
  XMLElement* _next;

  CCXX_COPY_DECLS(XMLElementIterator);
};

} // namespace ccxx

#endif // __ccxx_XMLElementIterator_hxx
