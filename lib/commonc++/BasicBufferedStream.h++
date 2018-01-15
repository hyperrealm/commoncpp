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

#ifndef __ccxx_BasicBufferedStream_hxx
#define __ccxx_BasicBufferedStream_hxx

#include <commonc++/Common.h++>
#include <commonc++/CircularBuffer.h++>
#include <commonc++/IOException.h++>
#include <commonc++/Stream.h++>

#include <istream>
#include <string>
#include <streambuf>

namespace ccxx {

/**
 * A buffered stream. This class is a subclass of
 * <b>std::basic_iostream</b> and serves as the "glue" between
 * commonc++ streams and stdc++ streams.
 *
 * @author Mark Lindner
 */
template <typename C> class BasicBufferedStream : public std::basic_iostream<C>
{
 public:

  /** Construct a new BasicBufferedStream for the given stream and buffer
   * size.
   *
   * @param stream The stream.
   * @param bufferSize The buffer size.
   */
  BasicBufferedStream(Stream& stream,
                      size_t bufferSize = DEFAULT_BUFFER_SIZE);

  /** The default buffer size. */
  static const size_t DEFAULT_BUFFER_SIZE;

  /** Destructor. */
  virtual ~BasicBufferedStream();

  /**
   * Close the stream for reading, writing, or both.
   *
   * @param mode The close mode.
   */
  virtual void close(IOMode mode = IOReadWrite);

 protected:

  /**
   * An implementation of basic_streambuf from the standard C++ library.
   *
   * @author Mark Lindner
   */
  class StreamBuf : public std::basic_streambuf<C>
  {
#ifndef _MSC_VER
    typedef typename std::char_traits<C>::int_type int_type;
    typedef typename std::char_traits<C>::pos_type pos_type;
    typedef typename std::char_traits<C>::off_type off_type;
#endif
    enum StreamOp { OpNone, OpRead, OpReadSeek, OpWrite, OpWriteSeek,
                    OpSeek };

   public:

    StreamBuf(size_t bufferSize, Stream& stream);
    ~StreamBuf();

    int sync();

   protected:

    int_type overflow(int_type c);
    int_type underflow();

    pos_type seekpos(pos_type streampos, std::ios::openmode mode);
    pos_type seekoff(off_type offset, std::ios::seekdir dir,
                     std::ios::openmode mode);
    std::streamsize showmanyc();

   private:

    inline void _resetp();
    inline void _resetg();

    Stream& _stream;
    CircularBuffer<C> *_inbuf;
    CircularBuffer<C> *_outbuf;
    int64_t _readPos;
    int64_t _writePos;
    StreamOp _lastOp;
  };

  /** @cond INTERNAL */
  Stream& _stream;
  StreamBuf *_buf;
  /** @endcond */
};

#include <commonc++/BasicBufferedStreamImpl.h++>

typedef BasicBufferedStream<char> BufferedCharStream;
typedef BasicBufferedStream<byte_t> BufferedByteStream;

} // namespace ccxx

#endif // __ccxx_BasicBufferedStream_hxx
