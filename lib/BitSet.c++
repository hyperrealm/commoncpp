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

#include "commonc++/BitSet.h++"
#include "commonc++/Private.h++"

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace ccxx {

/*
 */

// masks for 'first N bits' (counting from LSB)

static const uint32_t __firstN[]
  = { 0x00000000U,
      0x00000001U, 0x00000003U, 0x00000007U, 0x0000000FU,
      0x0000001FU, 0x0000003FU, 0x0000007FU, 0x000000FFU,
      0x000001FFU, 0x000003FFU, 0x000007FFU, 0x00000FFFU,
      0x00001FFFU, 0x00003FFFU, 0x00007FFFU, 0x0000FFFFU,
      0x0001FFFFU, 0x0003FFFFU, 0x0007FFFFU, 0x000FFFFFU,
      0x001FFFFFU, 0x003FFFFFU, 0x007FFFFFU, 0x00FFFFFFU,
      0x01FFFFFFU, 0x03FFFFFFU, 0x07FFFFFFU, 0x0FFFFFFFU,
      0x1FFFFFFFU, 0x3FFFFFFFU, 0x7FFFFFFFU, 0xFFFFFFFFU };

// masks for 'last N bits' (counting from MSB)

static const uint32_t __lastN[]
  = { 0x00000000U,
      0x80000000U, 0xC0000000U, 0xE0000000U, 0xF0000000U,
      0xF8000000U, 0xFC000000U, 0xFE000000U, 0xFF000000U,
      0xFF800000U, 0xFFC00000U, 0xFFE00000U, 0xFFF00000U,
      0xFFF80000U, 0xFFFC0000U, 0xFFFE0000U, 0xFFFF0000U,
      0xFFFF8000U, 0xFFFFC000U, 0xFFFFE000U, 0xFFFFF000U,
      0xFFFFF800U, 0xFFFFFC00U, 0xFFFFFE00U, 0xFFFFFF00U,
      0xFFFFFF80U, 0xFFFFFFC0U, 0xFFFFFFE0U, 0xFFFFFFF0U,
      0xFFFFFFF8U, 0xFFFFFFFCU, 0xFFFFFFFEU, 0xFFFFFFFFU };

/*
 */

BitSet::Bit::Bit(BitSet& set, uint_t pos)
  : _set(set), _pos(pos)
{
}

/*
 */

BitSet::Bit::Bit(const BitSet::Bit &other)
  : _set(other._set), _pos(other._pos)
{
}

/*
 */

BitSet::Bit &BitSet::Bit::operator=(const BitSet::Bit &other)
{
  _set = other._set;
  _pos = other._pos;

  return(*this);
}

/*
 */

BitSet::Bit::~Bit()
{
}

/*
 */

BitSet::Bit& BitSet::Bit::operator=(bool val)
{
  _set.set(_pos, val);

  return(*this);
}

/*
 */

BitSet::Bit::operator bool() const
{
  return(_set.isSet(_pos));
}

/*
 */

BitSet::Bit& BitSet::Bit::flip()
{
  _set.set(_pos, ! _set.isSet(_pos));

  return(*this);
}

/*
 */

BitSet::BitSet(uint_t size)
{
  _init(size);

  std::memset(_words, 0, _length * sizeof(uint32_t));
}

/*
 */

BitSet::BitSet(const BitSet& other)
{
  _init(other._size);

  std::memcpy(_words, other._words, _length * sizeof(uint32_t));
}

/*
 */

BitSet::BitSet(const String &bits)
{
  _init(bits.getLength());

  for(uint_t i = 0; i < _size; ++i)
    set(i, (bits[i] == '1'));
}

/*
 */

void BitSet::_init(uint_t size)
{
  if (size < 1)
    size = 1;

  _length = (size + 31) / 32;

  _words = new uint32_t[_length];

  _size = size;

  int mod = size % 32;
  if(mod == 0)
    mod = 32;

  _tailMask = __firstN[mod];
}

/*
 */

BitSet::~BitSet()
{
  delete[] _words;
}

/*
 */

BitSet& BitSet::set(uint_t pos, bool value /* = true */)
{
  if(pos < _size)
  {
    div_t dv = std::div(pos, 32);

    if(value)
      CCXX_BIT_SET(_words[dv.quot], dv.rem);
    else
      CCXX_BIT_CLEAR(_words[dv.quot], dv.rem);
  }

  return(*this);
}

/*
 */

BitSet& BitSet::setRange(uint_t startPos, uint_t endPos,
                         bool value /* = true */)
{
  div_t dv1, dv2;

  dv1 = std::div(startPos, 32);
  dv2 = std::div(endPos, 32);

  for(int i = dv1.quot; i <= dv2.quot; ++i)
  {
    uint32_t mask = 0;

    if(i == dv1.quot)
    {
      mask = __lastN[32 - dv1.rem];
      if(i == dv2.quot)
        mask &= __firstN[dv2.rem + 1];
    }
    else if(i == dv2.quot)
      mask = __firstN[dv2.rem + 1];
    else
      mask = 0xFFFFFFFF;

    if(value)
      _words[i] |= mask;
    else
      _words[i] &= ~mask;
  }

  return(*this);
}

/*
 */

BitSet& BitSet::setAll(bool value /* = true */)
{
  uint32_t val = (value ? 0xFFFFFFFF : 0);
  std::memset(_words, val, _length * sizeof(uint32_t));

  if(val)
    _words[_length - 1] &= _tailMask;

  return(*this);
}

/*
 */

bool BitSet::isSet(int pos) const
{
  if((pos >= 0) && (pos < static_cast<int>(_size)))
    return(CCXX_BIT_TEST(_words[pos / 32], (pos % 32)));
  else
    return(false);
}

/*
 */

bool BitSet::isAnySet() const
{
  for(uint_t i = 0; i < _length; ++i)
  {
    if(_words[i] != 0)
      return(true);
  }

  return(false);
}

/*
 */

bool BitSet::isAllSet() const
{
  for(uint_t i = 0; i < _length - 1; ++i)
  {
    if(_words[i] != static_cast<uint_t>(0xFFFFFFFF))
      return(false);
  }

  return(_words[_length - 1] == _tailMask);
}

/*
 */

BitSet& BitSet::flip()
{
  for(uint_t i = 0; i < _length; ++i)
    _words[i] = ~_words[i];

  _words[_length - 1] &= _tailMask;

  return(*this);
}

/*
 */

int BitSet::_nextBit(uint_t startPos, bool set) const
{
  if(startPos < _size)
  {
    uint_t offset = startPos / 32;
    uint32_t mask = 1 << startPos % 32;

    while(offset < _length)
    {
      const uint32_t &word = _words[offset];
      do
      {
        if(((word & mask) == 0) ^ set)
          return(startPos);

        mask <<= 1;

        if(++startPos == _size)
          return(-1);
      }
      while(mask != 0);
      mask = 1;
      ++offset;
    }
  }

  return(-1);
}

/*
 */

BitSet& BitSet::flip(uint_t pos)
{
  if(pos >= _size)
    throw OutOfBoundsException();

  uint32_t &word = _words[pos / 32];
  uint_t bit = pos % 32;

  if(CCXX_BIT_TEST(word, bit))
    CCXX_BIT_CLEAR(word, bit);
  else
    CCXX_BIT_SET(word, bit);

  return(*this);
}

/*
 */

void BitSet::swap(BitSet& other)
{
  if(_size != other._size)
    throw OutOfBoundsException();

  for(uint_t i = 0; i < _length; ++i)
  {
    uint32_t tmp = _words[i];
    _words[i] = other._words[i];
    other._words[i] = tmp;
  }
}

/*
 */

BitSet::Bit BitSet::operator[](int pos)
{
  if((pos < 0) || (pos >= static_cast<int>(_size)))
    throw OutOfBoundsException();

  return(Bit(*this, pos));
}

/*
 */

void BitSet::setWord(uint_t index, uint32_t word)
{
  if(index >= _length)
    return;

  _words[index] = word;
  if(index == (_length - 1))
    _words[index] &= _tailMask;
}

/*
 */

String BitSet::toString() const
{
  String s;

  // slow implementation, but we don't expect to be doing this often...
  for(uint_t i = 0; i < _size; ++i)
    s << (get(i) ? '1' : '0');

  return(s);
}

/*
 */

bool BitSet::operator==(const BitSet& other) const
{
  if(other._size != _size)
    return(false);

  return(std::memcmp(_words, other._words, _length * sizeof(uint32_t)) == 0);
}

/*
 */

BitSet& BitSet::operator=(const BitSet& other)
{
  if(other._length != _length)
  {
    uint32_t *newwords = new uint32_t[other._length];

    delete[] _words;
    _words = newwords;
    _length = other._length;
  }

  _size = other._size;
  std::memcpy(_words, other._words, _length * sizeof(uint32_t));

  return(*this);
}

/*
 */

BitSet& BitSet::operator|=(const BitSet& other)
{
  if(_size != other._size)
    throw OutOfBoundsException();

  for(uint_t i = 0; i < _length; ++i)
    _words[i] |= other._words[i];

  return(*this);
}

/*
 */

BitSet& BitSet::operator&=(const BitSet& other)
{
  if(_size != other._size)
    throw OutOfBoundsException();

  for(uint_t i = 0; i < _length; ++i)
    _words[i] &= other._words[i];

  return(*this);
}

/*
 */

BitSet& BitSet::operator^=(const BitSet& other)
{
  if(_size != other._size)
    throw OutOfBoundsException();

  for(uint_t i = 0; i < _length; ++i)
    _words[i] ^= other._words[i];

  return(*this);
}

/*
 */

BitSet& BitSet::operator>>=(uint_t n)
{
  n %= _size;
  div_t dv = std::div(n, 32);
  uint_t nwm = _length - dv.quot; // number of words to move

  if(nwm > 0)
  {
    // shift words (backward in memory)

    std::memmove(&_words[0], &_words[dv.quot], nwm * sizeof(uint32_t));
    std::memset(&_words[nwm], 0, dv.quot * sizeof(uint32_t));
  }

  // shift bits

  uint_t nbm = 32 - dv.rem;
  uint_t mx = nwm - 1;

  for(uint_t i = 0; i < mx; ++i)
  {
    _words[i] >>= dv.rem;
    _words[i] |= ((_words[i + 1] & __firstN[dv.rem]) << nbm);
  }

  _words[mx] >>= dv.rem;

  return(*this);
}

/*
 */

BitSet& BitSet::operator<<=(uint_t n)
{
  n %= _size;
  div_t dv = std::div(n, 32);
  uint_t nwm = _length - dv.quot; // number of words to move

  if(nwm > 0)
  {
    // shift words (forward in memory)

    std::memmove(&_words[dv.quot], &_words[0], nwm * sizeof(uint32_t));
    std::memset(&_words[0], 0, dv.quot * sizeof(uint32_t));
  }

  // shift bits

  uint_t nbm = 32 - dv.rem;

  for(int i = _length - 1; i > dv.quot; --i)
  {
    _words[i] <<= dv.rem;
    _words[i] |= ((_words[i - 1] & __lastN[dv.rem]) >> nbm);
  }

  _words[dv.quot] <<= dv.rem;
  _words[_length - 1] &= _tailMask;

  return(*this);
}

/*
 */

void BitSet::write(std::ostream &stream) const
{
  // slow implementation, but we don't expect to be doing this often...

  for(uint_t i = 0; i < _size; ++i)
    stream << (get(i) ? '1' : '0');
}


} // namespace ccxx
