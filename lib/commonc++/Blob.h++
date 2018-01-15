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

#ifndef __ccxx_Blob_hxx
#define __ccxx_Blob_hxx

#include <commonc++/Common.h++>
#include <commonc++/AtomicCounter.h++>
#include <commonc++/OutOfBoundsException.h++>

#include <sstream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif // _MSC_VER

namespace ccxx {

/**
 * A reference-counting, copy-on-write, threadsafe container for arbitrary
 * binary data.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Blob
{
 public:

  /** Construct a new, empty Blob. */
  Blob();

  /**
   * Construct a new Blob from a byte array.
   *
   * @param data The byte array to copy or inherit.
   * @param length The length of the array.
   * @param copy If <b>true</b>, the byte array is copied into the Blob;
   * otherwise the byte array is inherited.
   */
  Blob(byte_t* data, uint_t length, bool copy = true);

  /** Copy constructor. */
  Blob(const Blob& other);

  /**
   * Construct a new Blob from a byte array.
   *
   * @param data The array to copy.
   * @param length The length of the array.
   */
  Blob(const byte_t* data, uint_t length);

  /** Destructor. */
  ~Blob();

  /**
   * Get the length of the Blob.
   *
   * @return The length, in bytes, or 0 if the Blob is <b>null</b>.
   */
  inline uint_t getLength() const
  { return(_buf == NULL ? 0 : _buf->_length); };

  /**
   * Get the length of the Blob.
   *
   * @return The length, in bytes, or 0 if the Blob is <b>null</b>.
   */
  inline uint_t length() const
  { return(getLength()); }

  /**
   * Set the length of the Blob. If the length is smaller than the current
   * length, the Blob is truncated to the new length. If the new length is
   * greater than the current length, the Blob is extended to the new length,
   * and the new bytes are initialized to 0.
   *
   * @param length The new length.
   */
  void setLength(uint_t length);

  /** Get a pointer to the beginning of the data in the Blob. */
  inline byte_t* getData()
  { return(_buf == NULL ? NULL : _buf->_data); }

  /** Get a pointer to the beginning of the data in the Blob. */
  inline const byte_t* getData() const
  { return(_buf == NULL ? NULL : _buf->_data); }

  /** Clear the Blob. Sets the length of the Blob to 0. */
  inline Blob& clear()
  {
    setLength(0);
    return(*this);
  }

  /**
   * Test if the Blob is empty.
   *
   * @return <b>true</b> if the Blob is <b>null</b> or has length 0,
   * <b>false</b> otherwise.
   */
  inline bool isEmpty() const
  { return((_buf->_data == NULL) || (_buf->_length == 0)); }

  /**
   * Test if the Blob is <b>null</b>.
   *
   * @return <b>true</b> if the Blob is <b>null</b>, <b>false</b>
   * otherwise.
   */
  inline bool isNull() const
  { return(_buf->_data == NULL); }

  /**
   * Append a byte array to the Blob.
   *
   * @param data The array to append.
   * @param length The length of the array.
   */
  Blob& append(const byte_t* data, uint_t length);

  /**
   * Append a character array to the Blob.
   *
   * @param str The array to append.
   * @param length The number of characters from the string to append, or 0
   * to append the entire string (up to, but not including, the NUL
   * terminator).
   */
  Blob& append(const char* str, uint_t length = 0);

  /**
   * Find the first occurrence of a given byte in the Blob.
   *
   * @param b The byte to search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of the byte, or -1
   * if the byte was not found.
   */
  int indexOf(byte_t b, uint_t fromIndex = 0) const;

  /**
   * Find the first occurrence of a given byte sequence in the Blob.
   *
   * @param buf The buffer containing the byte sequence to search for.
   * @param length The length of the sequence.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of the byte sequence, or -1
   * if the sequence was not found.
   */
  int indexOf(const byte_t* buf, uint_t length, uint_t fromIndex = 0) const;

  /**
   * Find the last occurrence of a given byte in the Blob.
   *
   * @param b The byte to search for.
   * @param fromIndex The index from which to begin searching backwards,
   * or <b>END</b> to start at the end of the Blob.
   * @return The index of the last occurrence of the byte, or -1
   * if the byte was not found.
   */
  int lastIndexOf(byte_t b, uint_t fromIndex = END) const;

  /**
   * Find the last occurrence of a given byte sequence in the Blob.
   *
   * @param buf The buffer containing the byte sequence to search for.
   * @param length The length of the sequence.
   * @param fromIndex The index from which to begin searching backwards,
   * or <b>END</b> to start at the end of the Blob.
   * @return The index of the last occurrence of the byte sequence, or -1
   * if the sequence was not found.
   */
  int lastIndexOf(const byte_t* buf, uint_t length, uint_t fromIndex = END)
    const;

  /**
   * Determine if the Blob contains a given byte.
   *
   * @param b The byte to search for.
   * @return <b>true</b> if the Blob contains at least one occurrence of
   * the byte, <b>false</b> otherwise.
   */
  inline bool contains(byte_t b) const
  { return(indexOf(b) >= 0); }

  /**
   * Compare this Blob to another.
   *
   * @param other The Blob to compare to.
   * @return A negative value if this Blob is "less"
   * than <i>other</i>, a positive value if it is "greater" than
   * <i>other</i>, and 0 if they are "equal".
   */
  int compareTo(const Blob& other) const;

  /** Assignment operator. */
  Blob& operator=(const Blob& other);

  /**
   * %Array index operator. Obtain a reference to the byte at the given
   * offset in the Blob.
   *
   * @param index The index.
   * @return A reference to the byte at the specified index.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  byte_t& operator[](int index);

  /**
   * %Array index operator. Obtain a copy of the byte at the given
   * offset in the Blob.
   *
   * @param index The index.
   * @return The byte at the specified index.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  byte_t operator[](int index) const;

  /**
   * Unary NOT operator.
   *
   * @return <b>true</b> if the Blob is <b>null</b>, <b>false</b>
   * otherwise.
   */
  inline bool operator!() const
  { return(isNull()); }

  /**
   * Append a byte onto the end of the Blob.
   *
   * @param b The byte to append.
   */
  inline Blob& operator+=(byte_t b)
  { return(append(&b, 1)); }

  /**
   * Append a value onto the end of the Blob.
   *
   * @param v The value to append. The value will be converted to a byte,
   * which will then be appended to the blob.
   */
  inline Blob& operator+=(int v)
  { return(operator+=(static_cast<byte_t>(v & 0xFF))); }

  /**
   * Append a character string onto the end of the Blob.
   *
   * @param str The string to append.
   */
  inline Blob& operator+=(const char *str)
  { return(append(str)); }

  /**
   * Append (a copy of) the contents of another Blob onto the end
   * of the Blob.
   *
   * @param blob The Blob to append.
   */
  inline Blob& operator+=(const Blob& blob)
  { return(append(blob.getData(), blob.getLength())); }

  /** Append operator. */
  inline Blob& operator<<(byte_t val)
  { return(append(&val, 1)); }

  /** Append operator. */
  inline Blob& operator<<(int val)
  { return(operator+=(static_cast<byte_t>(val & 0xFF))); }

  /** Append operator. */
  inline Blob& operator<<(const char* str)
  { return(append(str)); }

  /**
   * Compute a hashcode for the Blob.
   *
   * @param modulo The desired range for the hashcode.
   * @return A hashcode in the range [0, <i>modulo</i>).
   */
  uint_t hash(uint_t modulo = 256) const;

  /** The <b>null</b> Blob. */
  static const Blob null;

  /** A pseudo-index indicating the end of the blob. */
  static const uint_t END;

 private:

  class BlobBuf
  {
    friend class Blob;

   protected:

    byte_t *_data;
    uint_t _capacity;
    uint_t _length;
    AtomicCounter _refs;

    BlobBuf(const BlobBuf& other, uint_t length);
    BlobBuf(byte_t *data = NULL, uint_t offset = 0, uint_t length = 0,
            bool copy = true);
    BlobBuf(const byte_t *data, uint_t offset = 0, uint_t length = 0);
    ~BlobBuf();

    void reserve(uint_t size);
    void makeNull();
    void makeCopy(const byte_t *data, uint_t offset, uint_t length);

   private:

    CCXX_COPY_DECLS(BlobBuf);
  };

  BlobBuf *_buf;

  void _release();
  void _makeCopy(uint_t size, bool unshareable = false);
};

inline bool operator==(const Blob& b1, const Blob& b2)
{ return(b1.compareTo(b2) == 0); }

inline bool operator!=(const Blob& b1, const Blob& b2)
{ return(b1.compareTo(b2) != 0); }

inline bool operator<(const Blob& b1, const Blob& b2)
{ return(b1.compareTo(b2) < 0); }

inline bool operator>(const Blob& b1, const Blob& b2)
{ return(b1.compareTo(b2) > 0); }

inline bool operator<=(const Blob& b1, const Blob& b2)
{ return(b1.compareTo(b2) <= 0); }

inline bool operator>=(const Blob& b1, const Blob& b2)
{ return(b1.compareTo(b2) >= 0); }

inline std::ostream& operator<<(std::ostream& stream, const Blob& b)
{
  return(stream.write(reinterpret_cast<const char *>(b.getData()),
                      static_cast<std::streamsize>(b.getLength())));
}

} // namespace ccxx

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // __ccxx_Blob_hxx
