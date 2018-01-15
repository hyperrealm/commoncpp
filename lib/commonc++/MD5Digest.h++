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

#ifndef __ccxx_MD5Digest_hxx
#define __ccxx_MD5Digest_hxx

#include <commonc++/Digest.h++>

namespace ccxx {

/**
 * A class for generating MD5 digests.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API MD5Digest : public Digest
{
 public:

  /** Construct a new MD5Digest. */
  MD5Digest();

  /** Destructor. */
  virtual ~MD5Digest();

  inline virtual size_t size() const
  { return(16); }

  virtual void update(const byte_t* buf, size_t len);

  virtual void finish(byte_t* digest);

  virtual void reset();

 private:

  void _byteReverse(uint32_t* buf, int count);
  void transform(uint32_t buf[4], uint32_t const in[16]);

  uint32_t _buf[4];
  uint32_t _bits[2];
  uint32_t _in[16];

  CCXX_COPY_DECLS(MD5Digest);
};

} // namespace ccxx

#endif // __ccxx_MD5Digest_hxx
