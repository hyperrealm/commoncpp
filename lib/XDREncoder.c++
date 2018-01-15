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

#include "commonc++/XDREncoder.h++"
#include "commonc++/UTF8Encoder.h++"
#include "commonc++/Variant.h++"

#include <cstring>

namespace ccxx {

static const size_t ALIGNMENT = 4;

/*
 */

XDREncoder::XDREncoder(DataWriter& writer)
  : _writer(writer)
{
  _writer.setEndianness(ccxx::BigEndian);
}

/*
 */

XDREncoder::~XDREncoder()
{
}

/*
 */

XDREncoder& XDREncoder::encode(const char* s, size_t maxlen /* = 0 */)
{
  if(s != NULL)
  {
    size_t slen = std::strlen(s);
    size_t len = (maxlen > 0) ? std::min(slen, maxlen) : slen;

    _writer << static_cast<uint32_t>(len)
            << DataEncoder::SetLength(len) << s
            << DataEncoder::Align(ALIGNMENT);
  }

  return(*this);
}

/*
 */

XDREncoder& XDREncoder::encode(const String& s, size_t maxlen /* = 0 */)
{
  int encodedLength = UTF8Encoder::encodedLength(s.data(), s.length(), maxlen);
  CString cstr_s = s.toUTF8();

  return(encode(cstr_s.data(), encodedLength));
}

/*
 */

XDREncoder& XDREncoder::encode(const byte_t* v, size_t len)
{
  if((v != NULL) && (len > 0))
  {
    _writer << DataEncoder::SetLength(len) << v
            << DataEncoder::Align(ALIGNMENT);
  }

  return(*this);
}

/*
 */

XDREncoder& XDREncoder::encode(const Variant& v)
{
  Variant::Type type = v.getType();

  if(type == Variant::TypeOpaque)
    throw IOException(String() << "unsupported Variant type: " << type);

  encode(static_cast<uint32_t>(type));

  switch(type)
  {
    case Variant::TypeBool:
      encode(v.toBool());
      break;

    case Variant::TypeChar:
      encode(v.toChar());
      break;

    case Variant::TypeChar16:
      encode(v.toChar16());
      break;

    case Variant::TypeInt:
      encode(v.toInt());
      break;

    case Variant::TypeUInt:
      encode(v.toUInt());
      break;

    case Variant::TypeFloat:
      encode(v.toFloat());
      break;

    case Variant::TypeDouble:
      encode(v.toDouble());
      break;

    case Variant::TypeInt64:
      encode(v.toInt64());
      break;

    case Variant::TypeUInt64:
      encode(v.toUInt64());
      break;

    case Variant::TypeString:
      encode(v.toString());
      break;

    case Variant::TypeBlob:
    {
      Blob blob = v.toBlob();
      encode(static_cast<uint32_t>(blob.getLength()));
      encode(blob.getData(), blob.getLength());
      break;
    }

    case Variant::TypeList:
    {
      size_t len = v.length();
      encode(static_cast<int32_t>(len));

      for(size_t i = 0; i < len; ++i)
      {
        const Variant &elem = v[i];
        encode(elem);
      }

      break;
    }

    case Variant::TypeMap:
    {
      StringVec vec;
      v.getKeys(vec);

      encode(static_cast<int32_t>(vec.size()));
      for(StringVec::const_iterator iter = vec.begin();
          iter != vec.end();
          ++iter)
      {
        const String &key = *iter;
        encode(key);
        const Variant &memb = v[key];
        encode(memb);
      }

      break;
    }

    case Variant::TypeNull:
    default:
      // do nothing
      break;
  }

  return(*this);
}


} // namespace ccxx
