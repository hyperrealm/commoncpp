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

#ifndef __ccxx_XMLElementConstIterator_hxx
#define __ccxx_XMLElementConstIterator_hxx

#include <commonc++/Common.h++>
#include <commonc++/Iterator.h++>
#include <commonc++/String.h++>
#include <commonc++/UnsupportedOperationException.h++>

namespace ccxx {

class XMLElement; // fwd decl

/**
 * An XMLElement const iterator.
 *
 * @author Mark Lindner
 */
class COMMONCPPXML_API XMLElementConstIterator
  : public Iterator<const XMLElement&>
{
 public:

  /**
   * Construct a new XMLElementConstIterator.
   *
   * @param elem The XMLElement whose child elements will be iterated over.
   * @param name If not String::null, indicates that only elements with
   * the given name should be iterated over.
   */
  XMLElementConstIterator(const XMLElement& elem,
                          const String& name = String::null);

  /** Destructor. */
  ~XMLElementConstIterator();

  void rewind();

  /**
   * @throw OutOfBoundsException If the iterator is already at the end.
   */
  const XMLElement& next();

  bool hasNext();

  /**
   * @throw UnsupportedOperationException If the iterator does not support
   * this operation.
   */
  void remove();

 private:

  void _findNext();

  const XMLElement* _parent;
  String _name;
  XMLElement* _next;
  XMLElement* _prev;

  CCXX_COPY_DECLS(XMLElementConstIterator);
};

} // namespace ccxx

#endif // __ccxx_XMLElementConstIterator_hxx
