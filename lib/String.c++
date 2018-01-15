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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/String.h++"
#include "commonc++/CharOps.h++"
#include "commonc++/UTF32Decoder.h++"
#include "commonc++/UTF8Decoder.h++"
#include "commonc++/UTF8Encoder.h++"

#include <cstdio>
#include <cstring>
#include <ostream>

namespace ccxx {

/*
 */

const uint_t String::END = ~0;

const String String::null = String();

const String String::nullRep = String("(null)");

const String String::empty = "";

const String String::whitespaceChars = " \t\f\r\n\v";

const StringVec StringVec::emptyVec;

/*
 */

class String::StringBuf
{
  friend class String;

  protected:

  char16_t* _data;
  uint_t _capacity;
  uint_t _length;
  AtomicCounter _refs;

  StringBuf(const StringBuf& other, uint_t length);
  StringBuf(char16_t* str = NULL, uint_t offset = 0, uint_t length = 0,
            bool copy = true);
  StringBuf(const char16_t* str, uint_t offset = 0, uint_t length = 0);
  ~StringBuf();

  void reserve(uint_t size);
  void compact();
  void makeNull();
  void makeCopy(const char16_t* str, uint_t offset, uint_t length);

  private:

  CCXX_COPY_DECLS(StringBuf);
};

/*
 */

String::String()
  : _buf(new StringBuf())
{
}

/*
 */

String::String(char16_t *str, uint_t offset /* = 0 */,
                 uint_t count /* = 0 */,
                 bool copy /* = true */)
  : _buf(new StringBuf(str, offset, count, copy))
{
}

String::String(const char16_t *str, uint_t offset /* = 0 */,
                 uint_t count /* = 0 */)
  : _buf(new StringBuf(str, offset, count))
{
}

String::String(const wchar_t *str)
  : _buf(new StringBuf())
{
  if(str != NULL)
    _fromWChar(str);
}

/*
 */

String::String(const char *str, uint_t offset /* = 0 */,
                 uint_t count /* = 0 */)
  : _buf(new StringBuf())
{
  if(str != NULL)
  {
    if(count == 0)
      count = std::strlen(str);

    _fromUTF8(str, offset, count);
  }
}

/*
 */

String::String(const String& other)
  : _buf(NULL)
{
  if(&other == &String::null)
    _buf = new StringBuf();
  else if(other._buf->_refs == -1)
    _buf = new StringBuf(*other._buf, other._buf->_length);
  else
  {
    ++(other._buf->_refs);
    _buf = other._buf;
  }
}

/*
 */

String& String::operator=(const char *other)
{
  _release();
  _buf = new StringBuf();

  if(other != NULL)
  {
    int count = std::strlen(other);
    _fromUTF8(other, 0, count);
  }

  return(*this);
}

/*
 */

String& String::operator=(const wchar_t *other)
{
  _release();
  _buf = new StringBuf();

  if(other != NULL)
    _fromWChar(other);

  return(*this);
}

/*
 */

String& String::operator=(const String &other)
{
  if(&other != this)
  {
    _release();

    if(&other == &String::null)
      _buf = new StringBuf();
    else
    {
      if(other._buf->_refs == -1) // unshareable
        _buf = new StringBuf(other._buf->_data, 0, other._buf->_length);
      else
      {
        ++(other._buf->_refs);
        _buf = other._buf;
      }
    }
  }

  return(*this);
}

/*
 */

String::~String()
{
  _release();
}

/*
 */

bool String::endsWith(const String &str, bool ignoreCase /* = false */) const
{
  uint_t len = str.getLength();

  if(_buf->_length < len)
    return(false);

  if(len == 0)
    return(true);

  int r = CharOps::compare(_buf->_data + _buf->_length - len, str._buf->_data,
                           ignoreCase);

  return(r == 0);
}

/*
 */

bool String::endsWith(const Char &c, bool ignoreCase /* = false */) const
{
  if(isEmpty())
    return(false);

  if(ignoreCase)
    return(c.toLowerCase()
           == Char::toLowerCase(*(_buf->_data + _buf->_length - 1)));
  else
    return(c == *(_buf->_data + _buf->_length - 1));
}

/*
 */

bool String::endsWithAnyOf(const String& chars,
                           bool ignoreCase /* = false */) const
{
  if(isEmpty())
    return(false);

  char16_t c = *(_buf->_data + _buf->_length - 1);
  bool matched = false;

  for(const char16_t* p = chars.data(); *p; ++p)
  {
    matched = (CharOps::compare(*p, c, ignoreCase) == 0);
    if(matched)
      break;
  }

  return(matched);
}

/*
 */

bool String::startsWith(const String &str, bool ignoreCase /* = false */) const
{
  uint_t len = str.getLength();

  if(_buf->_length < len)
    return(false);

  if(len == 0)
    return(true);

  return(CharOps::compare(_buf->_data, str._buf->_data, len, ignoreCase) == 0);
}

/*
 */

bool String::startsWith(const Char &c, bool ignoreCase /* = false */) const
{
  if(isEmpty())
    return(false);

  return(CharOps::compare(*(_buf->_data), c.toChar16(), ignoreCase) == 0);
}

/*
 */

bool String::startsWithAnyOf(const String& chars,
                             bool ignoreCase /* = false */) const
{
  if(isEmpty())
    return(false);

  char16_t c = *(_buf->_data);
  bool matched = false;

  for(const char16_t* p = chars.data(); *p; ++p)
  {
    matched = (CharOps::compare(*p, c, ignoreCase) == 0);
    if(matched)
      break;
  }

  return(matched);
}

/*
 */

uint_t String::getLength() const
{
  return(_buf == NULL ? 0 : _buf->_length);
}

/*
 */

void String::setLength(uint_t length)
{
  if(length < _buf->_length)
  {
    _makeCopy(length + 1);

    _buf->_length = length;
    *(_buf->_data + _buf->_length) = 0;
  }
}

/*
 */

void String::reserve(uint_t capacity)
{
  _buf->reserve(capacity + 1);
}

/*
 */

uint_t String::getCapacity() const
{
  return(_buf->_capacity);
}

/*
 */

void String::compact()
{
  // TODO
}

/*
 */

bool String::isEmpty() const
{
  return((_buf->_data == NULL) || (_buf->_length == 0));
}

/*
 */

bool String::isNull() const
{
  return(_buf->_data == NULL);
}

/*
 */

bool String::isInteger() const
{
  if(isEmpty())
    return(false);

  bool first = true;
  for(const char16_t* p = _buf->_data; *p; ++p)
  {
    if(! (Char::isDigit(*p) || (first && Char::isSign(*p))))
      return(false);

    first = false;
  }

  return(true);
}

/*
 */

bool String::isAlphaNumeric() const
{
  if(isEmpty())
    return(false);

  for(const char16_t* p = _buf->_data; *p; ++p)
  {
    if(! (Char::isAlphaNumeric(*p)))
      return(false);
  }

  return(true);
}

/*
 */

int String::compareTo(const String& other, bool ignoreCase /* = false */) const
{
  if(_buf == other._buf)
    return(0);

  // null is "less than" non-null

  if(isNull())
    return(other.isNull() ? 0 : -1);

  if(other.isNull())
    return(1);

  return(CharOps::compare(_buf->_data, other._buf->_data, ignoreCase));
}

/*
 */

String& String::trim(const String &chars /* = whitespaceChars */,
                     bool begin /* = true */,
                     bool end /* = true */)
{
  if(! isEmpty())
  {
    char16_t* p = _buf->_data, *q = p + _buf->_length - 1;

    // trim from beginning

    if(begin)
      for(; *p && CharOps::find(chars.data(), *p); ++p) {}

    // trim from end

    if(end)
      for(; CharOps::find(chars.data(), *q) && (q >= p); --q) {}

    uint_t nl = (q - p + 1);

    if(nl != _buf->_length)
    {
      _makeCopy(nl + 1);

      CharOps::move(_buf->_data, p, nl);
      _buf->_length = nl;
      *(_buf->_data + _buf->_length) = 0;
    }
  }

  return(*this);
}

/*
 */

String& String::chop(const Char &c)
{
  if((! isEmpty()) && (! c.isNul()))
  {
    char16_t* p = CharOps::find(_buf->_data, c.toChar16());
    if(p != NULL)
    {
      uint_t nl = (p - _buf->_data);

      _makeCopy(nl + 1);

      _buf->_length = nl;
      *(_buf->_data + _buf->_length) = 0;
    }
  }

  return(*this);
}

/*
 */

String& String::reverseChop(const Char &c)
{
  if(! isEmpty() && (! c.isNul()))
  {
    char16_t* p = CharOps::findLast(_buf->_data, c.toChar16());
    if(p != NULL)
    {
      uint_t nl = (p - _buf->_data);

      _makeCopy(nl + 1);

      _buf->_length = nl;
      *(_buf->_data + _buf->_length) = 0;
    }
  }

  return(*this);
}

/*
 */

String& String::replace(const Char &a, const Char &b)
{
  if((! isEmpty()) && (! a.isNul()) && (! b.isNul()) && (a != b))
  {
    char16_t *p = CharOps::find(_buf->_data, a.toChar16());
    if(p != NULL)
    {
      _makeCopy(_buf->_capacity);

      p = CharOps::find(_buf->_data, a.toChar16());

      for(char16_t *q = p; *q; q++)
      {
        if(a == *q)
          *q = b.toChar16();
      }
    }
  }

  return(*this);
}

/*
 */

String& String::replace(const String &a, const String &b)
{
  if(!isEmpty() && !a.isNull() && !b.isNull())
  {
    char16_t* p = CharOps::find(_buf->_data, a.data());

    if(p)
    {
      uint_t l = a.length();
      String copy;
      char16_t *q = _buf->_data;

      do
      {
        if(p != q)
          copy.append(q, (p - q));
        q = p + l;
        copy.append(b);
      }
      while((p = CharOps::find(q, a.data())) != NULL);

      if(! Char::isNul(*q))
        copy.append(q);

      *this = copy;
    }
  }

  return(*this);
}


/*
 */

CharRef String::operator[](int index)
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  bool extend = false;
  if(ind >= _buf->_length)
  {
    ind = _buf->_length;
    extend = true;
  }

  _makeCopy(std::max(_buf->_length, static_cast<uint_t>(ind + 1)), true);

  if(extend)
  {
    ++_buf->_length;
    *(_buf->_data  + _buf->_length) = 0;
  }

  return(CharRef(*this, ind));
}

/*
 */

const Char String::operator[](int index) const
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  if(ind >= _buf->_length)
    throw OutOfBoundsException();

  return(Char(*(_buf->_data + ind)));
}

/*
 */

String::const_iterator String::begin() const
{
  if(isNull())
    throw NullPointerException();

  return(_buf->_data);
}

/*
 */

String::const_iterator String::end() const
{
  if(isNull())
    throw NullPointerException();

  return(_buf->_data + _buf->_length);
}

/*
 */

char16_t* String::_makeGap(uint_t index, uint_t gaplen)
{
  if((index > _buf->_length) || (index == END))
    index = static_cast<uint_t>(_buf->_length);

  _makeCopy(_buf->_length + gaplen + 1);

  uint_t movelen = _buf->_length - index;

  if(movelen > 0)
    CharOps::move(_buf->_data + index + gaplen, _buf->_data + index, movelen);

  _buf->_length += gaplen;
  *(_buf->_data + _buf->_length) = 0;

  return(_buf->_data + index);
}

/*
 */

String& String::insert(const Char &c, uint_t index /* = 0 */)
{
  if(! c.isNul())
  {
    char16_t *gap = _makeGap(index, 1);
    *gap = c.toChar16();
  }

  return(*this);
}

/*
 */

void String::_insert(const char16_t *str, uint_t length, uint_t index,
                      uint_t count)
{
  if((count == 0) || (count > length))
    count = length;

  // If index is in between a surrogate pair, increment it.
  if ((index != END) && (Char::isHighSurrogate(*(str + index))))
    ++index;

  if(count > 0)
  {
    char16_t *gap = _makeGap(index, count);
    CharOps::copy(gap, str, count);
  }
}

/*
 */

String& String::insert(const String &str, uint_t index /* = 0 */,
                         uint_t count /* = 0 */)
{
  if(!str.isNull())
    _insert(str.data(), str.length(), index, count);

  return(*this);
}

/*
 */

String& String::insert(const char16_t *str, uint_t index /* = 0 */,
                         uint_t count /* = 0 */)
{
  if(str != NULL)
  {
    uint_t length = (count == 0) ? CharOps::length(str) : count;
    _insert(str, length, index, count);
  }

  return(*this);
}

/*
 */

String& String::remove(uint_t fromIndex, uint_t toIndex /* = END */)
{
  if(! isEmpty())
  {
    if(toIndex == END)
      toIndex = static_cast<uint_t>(_buf->_length) + 1;
    if(fromIndex == END)
      fromIndex = static_cast<uint_t>(_buf->_length) + 1;

    if(toIndex > fromIndex)
    {
      uint_t len = toIndex - fromIndex - 1;

      CharOps::move(_buf->_data + fromIndex, _buf->_data + toIndex,
                    (_buf->_length - toIndex + 1));

      _buf->_length -= len;
      *(_buf->_data + _buf->_length) = 0;
    }
  }

  return(*this);
}

Char String::charAt(uint_t index) const
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  if(ind >= _buf->_length)
    throw OutOfBoundsException();

  return(Char(*(_buf->_data)));
}

/*
 */

String& String::setCharAt(uint_t index, const Char &c)
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  if(ind >= _buf->_length)
    throw OutOfBoundsException();

  *(_buf->_data) = c.toChar16();

  return(*this);
}

/*
 */

uint_t String::hash(uint_t modulo /* = 256 */) const
{
  uint32_t hash = 0;

  if(! isEmpty())
    hash = Hash::SDBMHash(reinterpret_cast<const byte_t *>(_buf->_data),
                          _buf->_length * sizeof(char16_t));

  return(hash % modulo);
}

/*
 */

String String::substring(uint_t fromIndex, uint_t toIndex /* = END */) const
{
  if(isNull() || (fromIndex == toIndex))
    return(String::empty);

  if(fromIndex > toIndex)
    fromIndex = toIndex;

  String substr(_buf->_data, fromIndex, (toIndex - fromIndex));

  return(substr);
}

/*
 */

int String::indexOf(const Char &c, uint_t fromIndex /* = 0 */) const
{
  if(isNull() || (fromIndex >= _buf->_length) || c.isNul())
    return(-1);

  int index = fromIndex;

  for(const char16_t* p = _buf->_data + fromIndex; *p; p++, index++)
    if(c == *p)
      return(index);

  return(-1);
}

/*
 */

int String::indexOfAnyOf(const String& chars, uint_t fromIndex /* = 0 */) const
{
  if(isNull() || (fromIndex >= _buf->_length) || (chars == NULL))
    return(-1);

  int index = fromIndex;

  for(const char16_t* p = _buf->_data + fromIndex; *p; p++, index++)
    if(CharOps::find(chars.data(), *p) != NULL)
      return(index);

  return(-1);
}

/*
 */

int String::indexOf(const String& str, uint_t fromIndex /* = 0 */) const
{
  if(isEmpty() || str.isNull() || (fromIndex >= _buf->_length))
    return(-1);

  const char16_t* p = CharOps::find(_buf->_data + fromIndex, str.data());

  return(p ? static_cast<int>(p - _buf->_data) : -1);
}

/*
 */

int String::lastIndexOf(const Char &c, uint_t fromIndex /* = END */) const
{
  if(isEmpty() || c.isNul())
    return(-1);

  if((fromIndex >= _buf->_length) || (fromIndex == END))
    fromIndex = _buf->_length - 1;

  int index = fromIndex;

  for(const char16_t* p = _buf->_data + fromIndex; index >= 0; index--, p--)
    if(*p == c.toChar16())
      return(index);

  return(-1);
}

/*
 */

int String::lastIndexOfAnyOf(const String& chars, uint_t fromIndex /* = END */)
  const
{
  if(isEmpty() || (chars == NULL))
    return(-1);

  if((fromIndex >= _buf->_length) || (fromIndex == END))
    fromIndex = _buf->_length - 1;

  int index = fromIndex;

  for(const char16_t* p = _buf->_data + fromIndex; index >= 0; index--, p--)
    if(CharOps::find(chars.data(), *p) != NULL)
      return(index);

  return(-1);
}

/*
 */

int String::lastIndexOf(const String& str, uint_t fromIndex /* = END */) const
{
  if(isEmpty() || (str == NULL))
    return(-1);

  if(_buf->_length < str.length())
    return(-1);

  if((fromIndex >= _buf->_length) || (fromIndex == END))
    fromIndex = _buf->_length - 1;

  char16_t* p = CharOps::findLast(_buf->_data, str.data(), fromIndex);

  return(p ? (p - _buf->_data) : -1);
}

/*
 */

String String::nextToken(uint_t &fromIndex,
                         const String& delim /* = whitespaceChars */) const
{
  if(isNull() || (fromIndex >= _buf->_length) || delim.isNull())
    return(String::null);

  char16_t *p, *q;

  for(p = _buf->_data + fromIndex; *p; p++)
  {
    if(! CharOps::find(delim.data(), *p))
      break;
  }

  if(Char::isNul(*p))
    return(String::null);

  for(q = p + 1; *q; q++)
  {
    if(CharOps::find(delim.data(), *q))
      break;
  }

  fromIndex = static_cast<uint_t>(q - _buf->_data);

  String tok(p, 0, static_cast<uint_t>(q - p));

  return(tok);
}

/*
 */

uint_t String::split(StringVec& tokens,
                     const String& delim /* = whitespaceChars */,
                     uint_t limit /* = 0 */) const
{
  uint_t index = 0, count = 0;

  for(;;)
  {
    String s = nextToken(index, delim);
    if(s.isNull())
      break;

    tokens << s;
    ++count;

    if(limit && (count == limit))
      break;
  }

  return(count);
}

/*
 */

String& String::toLowerCase()
{
  if(! isEmpty())
  {
    _makeCopy(_buf->_capacity);

    for(char16_t* p = _buf->_data; *p; p++)
      *p = Char::toLowerCase(*p);
  }

  return(*this);
}

/*
 */

String& String::toUpperCase()
{
  if(! isEmpty())
  {
    _makeCopy(_buf->_capacity);

    for(char16_t* p = _buf->_data; *p; p++)
      *p = Char::toUpperCase(*p);
  }

  return(*this);
}

/*
 */

String& String::fill(const Char &c, uint_t count)
{
  if((! c.isNul()) && (count > 0))
  {
    _makeCopy(count + 1);

    uint_t n = count;
    for(char16_t *p = _buf->_data; n; ++p, --n)
      *p = c.toChar16();

    _buf->_length = count;
    *(_buf->_data + _buf->_length) = 0;
  }

  return(*this);
}

/*
 */

String& String::pad(const Char &c, uint_t count)
{
  if((! c.isNul()) && (count > 0))
  {
    _makeCopy(_buf->_length + count + 1);

    uint_t n = count;
    for(char16_t* p = _buf->_data + _buf->_length; n; ++p, --n)
      *p = c.toChar16();

    _buf->_length += count;
    *(_buf->_data + _buf->_length) = 0;
  }

  return(*this);
}

/*
 */

bool String::toBool(uint_t index /* = 0 */) const
{
  if(isNull() || (index >= _buf->_length))
    return(false);

  return(! CharOps::compare(_buf->_data + index, CharOps::TRUE_REP, true));
}

/*
 */

int String::toInt(uint_t index /* = 0 */, uint_t base /* = 10 */) const
{
  if((base < 2) || (base > 36) || isNull() || (index >= _buf->_length))
    return(0);

  return(CharOps::toLong(_buf->_data + index, base));
}

/*
 */

uint_t String::toUInt(uint_t index /* = 0 */, uint_t base /* = 10 */) const
{
  int v = toInt(index, base);
  return(static_cast<uint_t>(v < 0 ? 0 : v));
}

/*
 */

int64_t String::toInt64(uint_t index /* = 0 */, uint_t base /* = 10 */) const
{
  if((base < 2) || (base > 36) || isNull() || (index >= _buf->_length))
    return(0);

  return(CharOps::toLongLong(_buf->_data + index, base));
}

/*
 */

uint64_t String::toUInt64(uint_t index /* = 0 */, uint_t base /* = 10 */) const
{
  if((base < 2) || (base > 36) || isNull() || (index >= _buf->_length))
    return(0);

  return(CharOps::toULongLong(_buf->_data + index, base));
}

/*
 */

float String::toFloat(uint_t index /* = 0 */) const
{
  if(isNull() || (index >= _buf->_length))
    return(0.0f);

  return(CharOps::toFloat(_buf->_data + index));
}

/*
 */

double String::toDouble(uint_t index /* = 0 */) const
{
  if(isNull() || (index >= _buf->_length))
    return(0.0);

  return(CharOps::toDouble(_buf->_data + index));
}

/*
 */

String::operator const void *() const
{
  return(_buf == NULL ? NULL : _buf->_data);
}

/*
 */

const char16_t* String::data() const
{
  return(_buf == NULL ? NULL : _buf->_data);
}

/*
 */

CString String::toUTF8() const
{
  char *outbuf = NULL;

  if(!isNull())
  {
    const char16_t* buf = _buf->_data;
    int buflen = _buf->_length;

    const int len = UTF8Encoder::encodedLength(buf, buflen);

    if(len < 0)
      return(CString());

    outbuf = new char[len + 1];

    UTF8Encoder encoder;
    char *output = outbuf;
    int outputLen = len;
    int status = encoder.encode(&buf, &buflen, &output, &outputLen);
    if(status == UTF8Encoder::STATUS_OK)
      *(outbuf + len) = 0;
    else
    {
      delete[] outbuf;
      outbuf = NULL;
    }
  }

  return(CString(outbuf));
}

/*
 */

String& String::append(bool v)
{
  return(insert((v ? CharOps::TRUE_REP : CharOps::FALSE_REP), END));
}

/*
 */

String& String::append(int val)
{
  char buf[16];

  ::snprintf(buf, CCXX_LENGTHOF(buf), "%d", val);
  append(String(buf));

  return(*this);
}

/*
 */

String& String::append(int val, uint_t base)
{
  if((base >= 2) && (base <= 36))
  {
    char buf[34];
    bool neg = false;
    int i = 33;
    buf[i] = 0;

    if(val < 0)
    {
      neg = true;
      val = -val;

      if(val < 0) // int overflow
      {
        buf[--i] = CharOps::DIGITS[-(val + base) % base];
        val = -(val / base);
      }
    }

    do
    {
      buf[--i] = CharOps::DIGITS[val % base];
      val /= base;
    }
    while(val > 0);

    if(neg)
      buf[--i] = '-';

    append(String(buf));
  }

  return(*this);
}

/*
 */

String& String::append(uint_t val)
{
  char buf[16];

  ::snprintf(buf, CCXX_LENGTHOF(buf), "%u", val);
  append(String(buf));

  return(*this);
}

/*
 */

String& String::append(uint_t val, uint_t base)
{
  if((base >= 2) && (base <= 36))
  {
    char buf[34];
    int i = 33;
    buf[i] = 0;

    do
    {
      buf[--i] = CharOps::DIGITS[val % base];
      val /= base;
    }
    while(val > 0);

    append(String(buf));
  }

  return(*this);
}

/*
 */

String& String::append(const int64_t& val)
{
  char buf[32];

  ::snprintf(buf, CCXX_LENGTHOF(buf), INT64_FMT, val);
  append(String(buf));

  return(*this);
}

/*
 */

String& String::append(const uint64_t& val)
{
  char buf[32];

  ::snprintf(buf, CCXX_LENGTHOF(buf), UINT64_FMT, val);
  append(String(buf));

  return(*this);
}

/*
 */

String& String::append(const double& val, uint_t precision /* = 6 */)
{
  char buf[64];

  ::snprintf(buf, CCXX_LENGTHOF(buf), "%.*f", precision, val);
  append(String(buf));

  return(*this);
}

/*
 */

void String::_release()
{
  if(--(_buf->_refs) <= 0)
    delete _buf;
}

/*
 */

void String::_makeCopy(uint_t size, bool unshareable /* = false */)
{
  if(_buf->_refs > 1) // and implicitly != -1 (unshareable)
  {
    StringBuf* buf = new StringBuf(*_buf, size);

    if(--(_buf->_refs) < 1)
      delete buf; // in case two threads are doing this at the same time
    else
      _buf = buf;
  }
  else
    _buf->reserve(size);

  _buf->_refs = (unshareable ? -1 : 1);
}

/*
 */

String::StringBuf::StringBuf(const StringBuf& other, uint_t length)
  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  reserve(std::max(other._length, length) + 1);

  if(other._data != NULL)
    CharOps::copyRaw(_data, other._data, other._length);

  _length = other._length;
  *(_data + _length) = 0;
}

/*
 */

String::StringBuf::StringBuf(const char16_t* str,
                             uint_t offset /* = 0 */,
                             uint_t length /* = 0 */)
  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  makeCopy(str, offset, length);
}

/*
 */

String::StringBuf::StringBuf(char16_t* str /* = NULL */,
                             uint_t offset /* = 0 */,
                             uint_t length /* = 0 */,
                             bool copy /* = true */)
  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  if(! copy || (str == NULL))
  {
    _data = str;
    _length = (str == NULL ? 0 : CharOps::length(str));
    _capacity = (str == NULL ? 0 : _length + 1);
  }
  else
    makeCopy(str, offset, length);
}

/*
 */

String::StringBuf::~StringBuf()
{
  delete[] _data;
}

/*
 */

void String::StringBuf::makeCopy(const char16_t* str, uint_t offset,
                                 uint_t length)
{
  if(str != NULL)
  {
    uint_t len = CharOps::length(str);

    if(length == 0)
      length = len - offset;
    else
      length = std::min(length, len - offset);

    reserve(length + 1);
    CharOps::copyRaw(_data, str + offset, length);
    *(_data + length) = 0;
    _length = length;
  }
}

/*
 */

void String::StringBuf::reserve(uint_t size)
{
  if(_capacity < size)
  {
    uint_t newsz = (_capacity << 1) - (_capacity >> 1); // _capacity * 1.5
    uint_t needed = std::max<uint_t>(newsz, size);
    uint_t newcap = (needed > 0 ? 4 * ((needed - 1) / 4 + 1) : 0);
    char16_t* newdata = (newcap > 0 ? new char16_t[newcap] : NULL);

    if(newdata != NULL)
    {
      if(_data != NULL)
        CharOps::copyRaw(newdata, _data, _length);
    }

    if(_data != NULL)
      delete[] _data;

    _data = newdata;
    _capacity = newcap;
  }
}

/*
 */

void String::StringBuf::compact()
{
  if(_data)
  {
    uint_t needed = _length + 1;
    if(_capacity > needed)
    {
      char16_t *newdata = new char16_t[needed];
      CharOps::copyRaw(newdata, _data, _length);
      newdata[_length] = 0;

      delete[] _data;
      _data = newdata;

      _capacity = needed;
    }
  }
}

/*
 */

void String::StringBuf::makeNull()
{
  if(_data)
  {
    delete[] _data;
    _data = NULL;
  }

  _length = 0;
  _capacity = 0;
}

/*
 */

void String::write(std::ostream& stream) const
{
  if(!isNull())
  {
    CString cs = toUTF8();
    stream.write(cs.data(), cs.length());
  }
  else
    stream << "(null)";
}

/*
 */

String String::operator+(const String& str)
{
  String s = *this;
  s += str;

  return(s);
}

/*
 */

String String::operator+(const char *str)
{
  String s = *this;
  s += str;

  return(s);
}

/*
 */

String String::operator+(const Char &c)
{
  String s = *this;
  s += c;

  return(s);
}

/*
 */

void String::_fromUTF8(const char *str, uint_t offset, uint_t count)
{
  int length = UTF8Decoder::decodedLength(str + offset, count);
  if(length < 0)
    throw EncodingException();

  reserve(length);

  UTF8Decoder decoder;
  const char* input = str;
  char16_t *output = _buf->_data;
  int outlen = length;
  int inputLen = count;

  int status = decoder.decode(&input, &inputLen, &output, &outlen);
  if(status == UTFDecoder::STATUS_OK)
  {
    _buf->_length = length;
    *(_buf->_data + _buf->_length) = 0;
  }
  else
    throw EncodingException();
}

/*
 */

void String::_fromUTF32(const char32_t* str, uint_t offset, uint_t count)
{
  int length = UTF32Decoder::decodedLength(str + offset, count);
  if(length < 0)
    throw EncodingException();

  reserve(length);

  UTF32Decoder decoder;
  char16_t *output = _buf->_data;
  int outlen = length;
  int inputLen = count;

  int status = decoder.decode(&str, &inputLen, &output, &outlen);
  if(status == UTFDecoder::STATUS_OK)
  {
    _buf->_length = length;
    *(_buf->_data + _buf->_length) = 0;
  }
  else
    throw EncodingException();
}

/*
 */

void String::_fromWChar(const wchar_t* str)
{
  int count = std::wcslen(str);

  if(sizeof(wchar_t) == 2)
  {
    reserve(count);
    std::memcpy(reinterpret_cast<void *>(_buf->_data),
                reinterpret_cast<const void *>(str),
                count * sizeof(wchar_t));
    _buf->_length = count;
    *(_buf->_data + _buf->_length) = 0;
  }
  else
    _fromUTF32(reinterpret_cast<const char32_t*>(str), 0, count);
}


} // namespace ccxx
