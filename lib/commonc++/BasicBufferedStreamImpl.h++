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

#ifndef __ccxx_BasicBufferedStreamImpl_hxx
#define __ccxx_BasicBufferedStreamImpl_hxx

#ifndef __ccxx_BasicBufferedStream_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */
template<typename C>
  const size_t BasicBufferedStream<C>::DEFAULT_BUFFER_SIZE = 4096;

/*
 */

template<typename C> BasicBufferedStream<C>::BasicBufferedStream(
  Stream& stream, size_t bufferSize /* = DEFAULT_BUFFER_SIZE */)
  : std::iostream(_buf = new StreamBuf(bufferSize, stream)),
    _stream(stream)
{
}

/*
 */

template<typename C> BasicBufferedStream<C>::~BasicBufferedStream()
{
  close();

  delete _buf;
}

/*
 */

template<typename C>
  void BasicBufferedStream<C>::close(IOMode mode /* = IOReadWrite */)
{
  if(_buf)
    _buf->sync();

  _stream.close(mode);
}

/*
 */

template<typename C>
  BasicBufferedStream<C>::StreamBuf::StreamBuf(size_t bufferSize,
                                               Stream& stream)
    : _stream(stream),
      _inbuf(NULL),
      _outbuf(NULL),
      _readPos(0LL),
      _writePos(0LL),
      _lastOp(OpNone)
{
  if(stream.isReadable())
  {
    _inbuf = new CircularBuffer<C>(bufferSize);
    _resetg();
  }

  if(stream.isWritable())
  {
    _outbuf = new CircularBuffer<C>(bufferSize);
    _resetp();
  }
}

/*
 */

template<typename C>
  BasicBufferedStream<C>::StreamBuf::~StreamBuf()
{
  sync();

  if(_inbuf)
    delete _inbuf;

  if(_outbuf)
    delete _outbuf;
}

/*
 */

template<typename C>
  typename BasicBufferedStream<C>::StreamBuf::int_type
  BasicBufferedStream<C>::StreamBuf::overflow(int_type c /* = EOF */)
{
  if(! _stream.isWritable())
    return(std::char_traits<C>::eof());

  size_t nw = this->pptr() - _outbuf->getWritePos();

  _outbuf->advanceWritePos(nw);

  size_t x = _outbuf->getWriteExtent();
  if(x == 0)
  {
    // if last operation wasn't a write seek or write, we need to
    // reposition the seek pointer to the write position
    if((_lastOp != OpWriteSeek) && (_lastOp != OpWrite) && (_lastOp != OpSeek))
      _stream.seek(_writePos);

    size_t w = _outbuf->read(_stream);

    _writePos += w;
    _lastOp = OpWrite;
    x = _outbuf->getWriteExtent();
  }

  if(x == 0)
    return(std::char_traits<C>::eof());

  this->setp(_outbuf->getWritePos(), _outbuf->getWritePos() + x);

  if(! std::char_traits<C>::eq_int_type(c, std::char_traits<C>::eof()))
  {
    *(this->pptr()) = c;
    this->pbump(1);
  }

  return(std::char_traits<C>::not_eof(c));
}

/*
 */

template<typename C>
  typename BasicBufferedStream<C>::StreamBuf::int_type
  BasicBufferedStream<C>::StreamBuf::underflow()
{
  if(! _stream.isReadable())
    return(std::char_traits<C>::eof());

  size_t nr = this->gptr() - _inbuf->getReadPos();
  _inbuf->advanceReadPos(nr);

  size_t x = _inbuf->getReadExtent();
  if(x < 1)
  {
    // if last operation wasn't a read seek or read, we need to
    // reposition the seek pointer to the read position
    if((_lastOp != OpReadSeek) && (_lastOp != OpRead) && (_lastOp != OpSeek))
      _stream.seek(_readPos);

    size_t r = _inbuf->write(_stream);
    _readPos += r;
    _lastOp = OpRead;
    x = _inbuf->getReadExtent();
  }

  if(x < 1)
    return(std::char_traits<C>::eof());

  C *pos = _inbuf->getReadPos();

  this->setg(pos, pos, pos + x);

  int_type c = std::char_traits<C>::to_int_type(*(this->gptr()));
  this->gbump(1);

  return(c);
}

/*
 */

template<typename C>
  typename BasicBufferedStream<C>::StreamBuf::pos_type
  BasicBufferedStream<C>::StreamBuf::seekpos(pos_type streampos,
                                             std::ios::openmode mode)
{
  if(! _stream.isSeekable())
    return(static_cast<pos_type>(-1));

  if(sync() != 0)
    return(static_cast<pos_type>(-1));

  if(_inbuf)
    _inbuf->clear();

  int64_t pos = _stream.seek(streampos);

  if(mode & std::ios_base::in)
  {
    _readPos = pos;
    _lastOp = OpReadSeek;
  }

  if(mode & std::ios_base::out)
  {
    _writePos = pos;
    _lastOp = (mode & std::ios_base::in) ? OpSeek : OpWriteSeek;
  }

  return(static_cast<pos_type>(pos));
}

/*
 */

template<typename C>
  typename BasicBufferedStream<C>::StreamBuf::pos_type
  BasicBufferedStream<C>::StreamBuf::seekoff(off_type offset,
                                             std::ios::seekdir dir,
                                             std::ios::openmode mode)
{
  if(! _stream.isSeekable())
    return(static_cast<pos_type>(-1));

  if(sync() != 0)
    return(static_cast<pos_type>(-1));

  SeekMode seekMode = SeekAbsolute;

  switch(dir)
  {
    case std::ios_base::beg:
      seekMode = SeekAbsolute;
      break;

    case std::ios_base::cur:
      seekMode = SeekRelative;
      break;

    case std::ios_base::end:
      seekMode = SeekEnd;
      break;

    default:
      break;
  }

  if(_inbuf)
    _inbuf->clear();

  int64_t pos = _stream.seek(static_cast<int64_t>(offset), seekMode);

  if(mode & std::ios_base::in)
  {
    _readPos = pos;
    _lastOp = OpReadSeek;
  }

  if(mode & std::ios_base::out)
  {
    _writePos = pos;
    _lastOp = (mode & std::ios_base::in) ? OpSeek : OpWriteSeek;
  }

  return(static_cast<pos_type>(pos));
}

/*
 */

template<typename C>
  int BasicBufferedStream<C>::StreamBuf::sync()
{
  if(_outbuf)
  {
    size_t nw = this->pptr() - _outbuf->getWritePos();
    _outbuf->advanceWritePos(nw);

    if(! _outbuf->isEmpty())
    {
      _outbuf->read(_stream);
      this->setp(reinterpret_cast<char *>(_outbuf->getWritePos()),
                 reinterpret_cast<char *>(_outbuf->getWritePos()
                                          + _outbuf->getWriteExtent()));
    }
  }

  return(0);
}

/*
 */

template<typename C>
  std::streamsize BasicBufferedStream<C>::StreamBuf::showmanyc()
{
  if(! _inbuf)
    return(0);

  return(static_cast<int>(_inbuf->getRemaining()));
}

/*
 */

template<typename C>
  void BasicBufferedStream<C>::StreamBuf::_resetg()
{
  this->setg(reinterpret_cast<char *>(_inbuf->getBase()),
             reinterpret_cast<char *>(_inbuf->getBase()),
             reinterpret_cast<char *>(_inbuf->getBase()
                                      + _inbuf->getSize()));
}

/*
 */

template<typename C>
  void BasicBufferedStream<C>::StreamBuf::_resetp()
{
  this->setp(reinterpret_cast<char *>(_outbuf->getBase()),
             reinterpret_cast<char *>(_outbuf->getBase()
                                      + _outbuf->getSize()));
}

#endif // __ccxx_BasicBufferedStreamImpl_hxx
