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

#ifndef __ccxx_UUID_hxx
#define __ccxx_UUID_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A universally unique identifier.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API UUID
{
 public:

  /** Construct a new UUID. */
  UUID();

  /** Destructor. */
  ~UUID();

  /**
   * Get the UUID bytes.
   *
   * @return A 16-byte array containing the UUID.
   */
  inline const byte_t* getBytes() const
  { return(_uuid); }

  /**
   * Get a String representation of the UUID.
   *
   * @return A string of the form XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX,
   * where each X is a hexadecimal digit.
   */
  String toString() const;

 private:

  void _createRandomUUID(byte_t* uuid);

  byte_t _uuid[16];
};

} // namespace ccxx

#endif // __ccxx_UUID_hxx
