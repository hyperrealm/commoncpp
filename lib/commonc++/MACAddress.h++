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

#ifndef __ccxx_MACAddress_hxx
#define __ccxx_MACAddress_hxx

#include <commonc++/Common.h++>
#include <commonc++/OutOfBoundsException.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A network hardware (MAC) address.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API MACAddress
{
 public:

  /** Construct a new, null MACAddress (00:00:00:00:00:00). */
  MACAddress();

  /** Construct a new MACAddress with the given octets. */
  MACAddress(const byte_t octets[6]);

  /** Destructor. */
  ~MACAddress();

  /** Get the octets of the MAC address. */
  void getOctets(byte_t octets[6]) const;

  /** Set the octets of the MAC address. */
  void setOctets(const byte_t octets[6]);

  /**
   * Get the octet at the given index in the MAC address.
   *
   * @param index The index.
   * @return The octet at the given index.
   * @throw OutOfBoundsException If the index is invalid.
   */
  byte_t operator[](int index) const;

  /**
   * Get a reference to the octet at the given index in the MAC address.
   *
   * @param index The index.
   * @return A reference to the octet at the given index.
   * @throw OutOfBoundsException If the index is invalid.
   */
  byte_t& operator[](int index);

  /** Return a String representation of the MACAddress. */
  String toString() const;

  /** Equality operator. */
  bool operator==(const MACAddress& other) const;

  /** Inequality operator. */
  inline bool operator!=(const MACAddress& other) const
  { return(! operator==(other)); }

  /** Test if this is a null address (00:00:00:00:00:00). */
  bool operator!() const;

 private:

  mutable String _str;
  byte_t _octets[6];
};

} // namespace ccxx

#endif // __ccxx_MACAddress_hxx
