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

#ifndef __ccxx_SHA1Digest_hxx
#define __ccxx_SHA1Digest_hxx

#include <commonc++/Digest.h++>

namespace ccxx {

/**
 * A class for generating SHA-1 digests.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SHA1Digest : public Digest
{
 public:

  /** Construct a new SHA1Digest. */
  SHA1Digest();

  /** Destructor. */
  virtual ~SHA1Digest();

  inline virtual size_t size() const
  { return(20); }

  virtual void update(const byte_t* buf, size_t len);

  virtual void finish(byte_t* digest);

  virtual void reset();

 private:

  void processBlock();
  void padBlock();
  inline uint32_t rotateLeft(uint32_t word, int bits);

  uint32_t _h[5];
  uint64_t _length;
  byte_t _block[64];
  int _offset;

  CCXX_COPY_DECLS(SHA1Digest);
};

} // namespace ccxx

#endif // __ccxx_SHA1Digest_hxx
