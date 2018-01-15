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

#include "commonc++/XDRDecoder.h++"
#include "commonc++/Variant.h++"

namespace ccxx {

/*
 */

static const size_t ALIGNMENT = 4;

/*
 */

XDRDecoder::XDRDecoder(DataReader& reader)
  : _reader(reader)
{
  _reader.setEndianness(ccxx::BigEndian);
}

/*
 */

XDRDecoder::~XDRDecoder()
{
}

/*
 */

XDRDecoder& XDRDecoder::decode(char* s, size_t maxlen /* = 0 */)
{
  // decode the length
  uint32_t slen;
  _reader >> slen;

  // error if length exceeds maximum requested length
  if((maxlen > 0) && (slen > maxlen))
    throw IOException("string length exceeds maximum");

  _reader >> DataEncoder::SetLength(slen) >> s
          >> DataEncoder::Align(ALIGNMENT);

  return(*this);
}

/*
 */

XDRDecoder& XDRDecoder::decode(String& s, size_t maxlen /* = 0 */)
{
  // decode the length
  uint32_t slen;
  _reader >> slen;

  // error if length exceeds maximum requested length
  if((maxlen > 0) && (slen > maxlen))
    throw IOException("string length exceeds maximum");

  _reader >> DataEncoder::SetLength(slen) >> s
          >> DataEncoder::Align(ALIGNMENT);

  return(*this);
}

/*
 */

XDRDecoder& XDRDecoder::decode(byte_t* v, size_t len)
{
  _reader >> DataEncoder::SetLength(len) >> v
          >> DataEncoder::Align(ALIGNMENT);

  return(*this);
}

/*
 */

XDRDecoder& XDRDecoder::decode(Variant& v)
{
  uint32_t type;
  decode(type);

  switch(type)
  {
    case Variant::TypeNull:
      v.setNull();
      break;

    case Variant::TypeBool:
    {
      bool val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeChar:
    {
      char val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeChar16:
    {
      uint16_t val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeInt:
    {
      int32_t val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeUInt:
    {
      uint32_t val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeFloat:
    {
      float val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeDouble:
    {
      double val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeInt64:
    {
      int64_t val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeUInt64:
    {
      uint64_t val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeString:
    {
      String val;
      decode(val);
      v = val;
      break;
    }

    case Variant::TypeBlob:
    {
      uint32_t len;
      decode(len);
      Blob val;
      val.setLength(len);
      decode(val.getData(), val.getLength());
      v = val;
      break;
    }

    case Variant::TypeList:
    {
      uint32_t len;
      decode(len);
      v.setType(Variant::TypeList);
      for(uint32_t i = 0; i < len; ++i)
      {
        Variant &elem = v[i];
        decode(elem);
      }
      break;
    }

    case Variant::TypeMap:
    {
      int32_t len;
      decode(len);
      v.setType(Variant::TypeMap);
      for(int32_t i = 0; i < len; ++i)
      {
        String key;
        decode(key);
        Variant &memb = v[key];
        decode(memb);
      }
      break;
    }

    default:
    {
      throw IOException(String() << "unsupported Variant type: " << type);
      break;
    }
  }

  return(*this);
}


} // namespace ccxx
