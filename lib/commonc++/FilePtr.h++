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

#ifndef __ccxx_FilePtr_hxx
#define __ccxx_FilePtr_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

#include <cstdio>

namespace ccxx {

/**
 * A smart pointer for stdio streams.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API FilePtr
{
 public:

  /**
   * Construct a new FilePtr for the given filename and attributes.
   * The constructor calls <b>fopen()</b> to open the stream.
   *
   * @param name The filename.
   * @param mode The open mode.
   */
  FilePtr(const char* name, const char* mode);

  /**
   * Construct a new FilePtr for the given filename and attributes.
   * The constructor calls <b>fopen()</b> to open the stream.
   *
   * @param name The filename.
   * @param mode The open mode.
   */
  FilePtr(const String& name, const String& mode);

  /**
   * Construct a new FilePtr for an existing stdio stream.
   *
   * @param fp The stream pointer.
   */
  FilePtr(FILE* fp);

  /** Destructor. Closes the stream. */
  ~FilePtr();

  /** Cast operator. */
  inline operator FILE*()
  { return(_fp); }

  /** Determine if the stream is valid (non-NULL). */
  inline bool isValid() const
  { return(_fp != NULL); }

  /** Determine if the stream is invalid (NULL). */
  inline bool operator!()
  { return(! isValid()); }

 private:

  FILE* _fp;

  CCXX_COPY_DECLS(FilePtr);
};

} // namespace ccxx

#endif // __ccxx_FilePtr_hxx
