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

#ifndef __ccxx_MD5Password_hxx
#define __ccxx_MD5Password_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * MD5-hash password encryption and validation routines.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API MD5Password
{
 public:

  /**
   * Encrypt a password with an MD5 hash.
   *
   * @param password The plaintext password.
   * @return The encrypted password.
   */
  static String encrypt(const String& password);

  /**
   * Validate an MD5-hash password.
   *
   * @param password A plaintext password.
   * @param ciphertext The ciphertext form of the password to validate
   * against.
   * @return <b>true</b> if the passwords match, <b>false</b> otherwise.
   */
  static bool validate(const String& password, const String& ciphertext);

 private:

  static String encrypt(const String& password, const String& salt);

  MD5Password(); // not supported
  CCXX_COPY_DECLS(MD5Password);
};

} // namespace ccxx

#endif // __ccxx_MD5Password_hxx
