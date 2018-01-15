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

#ifndef __ccxx_BitSet_hxx
#define __ccxx_BitSet_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

#include <iostream>

namespace ccxx {

/**
 * A compact representation of an array of bits (boolean values). The
 * bits are stored internally in an array of words, each word
 * being a 32-bit unsigned integer that represents 32 boolean values.
 * Complete words can be fetched if the caller needs efficient access to
 * multiple bits at a time.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API BitSet
{
 public:

  /** A "reference" to a bit within a BitSet.
   *
   * @author Mark Lindner
   */
  class COMMONCPP_API Bit
  {
    friend class BitSet;

   public:

    /** Copy constructor. */
    Bit(const Bit& other);

    /** Assignment operator. */
    Bit& operator=(const Bit& other);

    /** Assign a value to the bit. */
    Bit& operator=(bool val);

    /** Destructor. */
    ~Bit();

    /** Cast operator. */
    operator bool() const;

    /** Flip the bit. */
    Bit& flip();

   private:

    Bit(BitSet& set, uint_t pos);

    BitSet& _set;
    uint_t _pos;
  };

  /**
   * Construct a new BitSet of the given size.
   *
   * @param size The number of bits the BitSet will hold.
   */
  BitSet(uint_t size);

  /** Copy constructor. */
  BitSet(const BitSet& other);

  /**
   * Construct a BitSet from a String.
   *
   * @param bits A String consisting of only the characters '0' and '1';
   * any other characters will be interpreted as 0.
   */
  BitSet(const String& bits);

  /** Destructor. */
  virtual ~BitSet();

  /**
   * Set (or clear) a bit at the given position.
   *
   * @param pos The position of the bit.
   * @param value The value to set the bit to.
   */
  BitSet& set(uint_t pos, bool value = true);

  /**
   * Set (or clear) the bits in the given (inclusive) range.
   *
   * @param startPos The start position of the range.
   * @param endPos The end position of the range.
   * @param value The value to set the bits to.
   */
  BitSet& setRange(uint_t startPos, uint_t endPos, bool value = true);

  /**
   * Set (or clear) all of the bits in the BitSet.
   *
   * @param value The value to set all of the bits to.
   */
  BitSet& setAll(bool value = true);

  /**
   * Clear the bit at the given position.
   *
   * @param pos The position.
   */
  inline BitSet& clear(uint_t pos)
  { return(set(pos, false)); }

  /**
   * Clear the bits in the given (inclusive) range.
   *
   * @param startPos The start position of the range.
   * @param endPos The end position of the range.
   */
  inline BitSet& clearRange(uint_t startPos, uint_t endPos)
  { return(setRange(startPos, endPos, false)); }

  /** Clear all of the bits in the BitSet. */
  inline BitSet& clearAll()
  { return(setAll(false)); }

  /** Test if a bit at the given position is set. */
  bool isSet(int pos) const;

  /** Get the value of the bit at the given position. */
  inline bool get(uint_t pos) const
  { return(isSet(pos)); }

  /** Test if a bit at the given position is clear. */
  inline bool isClear(uint_t pos) const
  { return(! isSet(pos)); }

  /** Test if at least one bit is set in the BitSet. */
  bool isAnySet() const;

  /** Test if all of the bits are set in the BitSet. */
  bool isAllSet() const;

  /** Test if all of the bits are clear in the BitSet. */
  inline bool isAllClear() const
  { return(! isAnySet()); }

  /**
   * Find the index of the next bit that is set, beginning at the
   * given position.
   *
   * @param startPos The starting bit position to search from.
   * @return The index of the bit, or -1 if there are no set bits between
   * the start position and the end of the BitSet, inclusive.
   */
  inline int nextSetBit(uint_t startPos) const
  { return(_nextBit(startPos, true)); }

  /**
   * Find the index of the first bit that is set.
   *
   * @return The index of the bit, or -1 if there are no set bits in the
   * BitSet.
   */
  inline int firstSetBit() const
  { return(_nextBit(0, true)); }

  /**
   * Find the index of the next bit that is clear, beginning at the
   * given position.
   *
   * @param startPos The starting bit position to search from.
   * @return The index of the bit, or -1 if there are no clear bits between
   * the start position and the end of the BitSet, inclusive.
   */
  inline int nextClearBit(uint_t startPos) const
  { return(_nextBit(startPos, false)); }

  /**
   * Find the index of the first bit that is clear.
   *
   * @return The index of the bit, or -1 if there are no clear bits in the
   * BitSet.
   */
  inline int firstClearBit() const
  { return(_nextBit(0, false)); }

  /**
   * Test if all of the bits in the BitSet are cleared. Equivalent to
   * isAllClear().
   */
  inline bool operator!() const
  { return(! isAnySet()); }

  /** Flip (toggle) all of the bits in the BitSet. */
  BitSet& flip();

  /**
   * Flip (toggle) the bit at the given position.
   *
   * @throw OutOfBoundsException If the position is invalid.
   */
  BitSet& flip(uint_t pos);

  /**
   * Swap the bits of this BitSet with those of another.
   *
   * @throw OutOfBoundsException If both BitSets are not of equal size.
   */
  void swap(BitSet &other);

  /**
   * Obtain a "reference" to a bit at the given position.
   *
   * @param pos The position.
   * @return The bit.
   * @throw OutOfBoundsException If the position is invalid.
   */
  Bit operator[](int pos);

  /**
   * Get the value of a bit at the given position.
   *
   * @param pos The position.
   * @return The value of the bit.
   * @throw OutOfBoundsException If the position is invalid.
   */
  inline bool operator[](int pos) const
  { return(isSet(pos)); }

  /** Get the size of the BitSet, in bits. */
  inline uint_t getSize() const
  { return(_size); }

  /**
   * Get the number of words in the BitSet. (Each word is a 32-bit
   * integer.)
   */
  inline uint_t getWordCount() const
  { return(_length); }

  /**
   * Get the word at the specified index in the BitSet.
   *
   * @param index The index. If the index is out of range, a value of 0 is
   * returned.
   * @return The word.
   */
  inline uint32_t getWord(uint_t index) const
  { return(index >= _length ? 0 : _words[index]); }

  /**
   * "Diff" the word at the specificed index in the BitSet against
   * another word. Essentially, this function simply returns a bitwise XOR
   * of the two words.
   *
   * @param index The index. If the index is out of range, a value of 0 is
   * returned.
   * @param other The other word to "diff" against.
   * @return The "diff" of the two words.
   */
  inline uint32_t getWordDiff(uint_t index, uint32_t other) const
  { return(getWord(index) ^ other); }

  /**
   * Set the word at the specified index in the BitSet.
   *
   * @param index The index. If the index is out of range, the method has
   * no effect.
   * @param word The word.
   */
  void setWord(uint_t index, uint32_t word);

  /** Get a String representation of the BitSet. */
  String toString() const;

  /** Write a string representation of the BitSet to a stream. */
  void write(std::ostream &stream) const;

  /** Equality operator. */
  bool operator==(const BitSet& other) const;

  /** Inequality operator. */
  inline bool operator!=(const BitSet& other) const
  { return(! operator==(other)); }

  /** Assignment operator. */
  BitSet& operator=(const BitSet& other);

  /**
   * Bitwise OR assignment operator.
   *
   * @throw OutOfBoundsException If the two BitSets are not of equal size.
   */
  BitSet& operator|=(const BitSet& other);

  /**
   * Bitwise AND assignment operator.
   *
   * @throw OutOfBoundsException If the two BitSets are not of equal size.
   */
  BitSet& operator&=(const BitSet& other);

  /**
   * Bitwise XOR assignment operator.
   *
   * @throw OutOfBoundsException If the two BitSets are not of equal size.
   */
  BitSet& operator^=(const BitSet& other);

  /**
   * Bitwise right-shift assignment operator.
   *
   * @param n The number of bit positions by which to shift the BitSet.
   */
  BitSet& operator>>=(uint_t n);

  /**
   * Bitwise left-shift assignment operator.
   *
   * @param n The number of bit positions by which to shift the BitSet.
   */
  BitSet& operator<<=(uint_t n);

  /**
   * Bitwise right-shift operator.
   *
   * @param n The number of bit positions by which to shift the BitSet.
   * @return A new BitSet that is a bit-shifted version of the original.
   */
  inline BitSet operator>>(uint_t n) const
  { return(BitSet(*this) >>= n); }

  /**
   * Bitwise left-shift operator.
   *
   * @param n The number of bit positions by which to shift the BitSet.
   * @return A new BitSet that is a bit-shifted version of the original.
   */
  inline BitSet operator<<(uint_t n) const
  { return(BitSet(*this) <<= n); }

  /**
   * Bitwise NOT operator.
   *
   * @return A new BitSet that is a bit-flipped version of the original.
   */
  inline BitSet operator~() const
  { return(BitSet(*this).flip()); }

 private:

  void _init(uint_t size);
  int _nextBit(uint_t startPos, bool set) const;

  uint_t _size;
  uint_t _length;
  uint32_t* _words;
  uint32_t _tailMask;
};

inline std::ostream& operator<<(std::ostream& stream, const BitSet& bs)
{
  bs.write(stream);
  return(stream);
}

} // namespace ccxx

#endif // __ccxx_BitSet_hxx
