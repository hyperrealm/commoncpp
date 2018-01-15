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

#ifndef __ccxx_SocketException_hxx
#define __ccxx_SocketException_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>

namespace ccxx {

/**
 * A general-purpose socket exception.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SocketException : public IOException
{
 public:

  /** Construct a new SocketException with an optional cause message. */

  SocketException(String message = String::empty);

  inline virtual void write(std::ostream& stream) const
  { _write(stream, "SocketException"); }
};

/**
 * A general-purose socket I/O exception.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SocketIOException : public SocketException
{
 public:

  /** Construct a new SocketIOException with an optional cause message. */
  SocketIOException(String message = String::empty);

  inline virtual void write(std::ostream& stream) const
  { _write(stream, "SocketIOException"); }
};

/**
 * An exception indicating that a connection attempt was refused because
 * there was no process listening on the remote port.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API ConnectionRefusedException : public SocketException
{
 public:

  /**
   * Construct a new ConnectionRefusedException with an optional cause
   * message.
   */
  ConnectionRefusedException(String message = String::empty);

  inline virtual void write(std::ostream& stream) const
  { _write(stream, "ConnectionRefusedException"); }
};

/**
 * An exception indicating that a socket address could not be bound
 * because it is already in use.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API AddressInUseException : public SocketException
{
 public:

  /** Construct a new AddressInUseException with an optional cause message. */
  AddressInUseException(String message = String::empty);

  inline virtual void write(std::ostream& stream) const
  { _write(stream, "AddressInUseException"); }
};

} // namespace ccxx

#endif // __ccxx_SocketException_hxx
