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

#ifndef __ccxx_String_hxx
#define __ccxx_String_hxx

#include <commonc++/Common.h++>
#include <commonc++/AtomicCounter.h++>
#include <commonc++/Char.h++>
#include <commonc++/CharRef.h++>
#include <commonc++/CString.h++>
#include <commonc++/EncodingException.h++>
#include <commonc++/Hash.h++>
#include <commonc++/NullPointerException.h++>
#include <commonc++/OutOfBoundsException.h++>

#include <string>
#include <vector>
#include <cerrno>
#include <iosfwd>

namespace ccxx {

class StringVec;

/**
 * A flexible, reference counted, copy-on-write, thread-safe, nullable string.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API String
{
 public:

  typedef char16_t value_type;
  typedef const char16_t* const_iterator;

  /** Construct a new, empty string. */
  String();

  /**
   * Copy constructor.
   *
   * @param other The string to copy.
   */
  String(const String& other);

  /**
   * Construct a new string from a C string.
   *
   * @param str The C string to copy or inherit.
   * @param offset The number of characters from the beginning of the string
   * to skip over.
   * @param count The number of characters from the offset to use, or 0 to
   * use all the characters to the end of the string.
   * @param copy If <b>true</b>, the C string is copied into the String;
   * otherwise the C string is inherited and the length parameter is ignored.
   */
  String(char16_t* str, uint_t offset = 0, uint_t count = 0, bool copy = true);

  /**
   * Construct a new string from an array of UTF-16 characters. The array is
   * copied into the string.
   *
   * @param str The character array.
   * @param offset The number of characters from the beginning of the string
   * to skip over.
   * @param count The number of characters from the offset to use, or 0 to
   * use all the characters to the end of the string.
   */
  String(const char16_t* str, uint_t offset = 0, uint_t count = 0);

  /**
   * Construct a new string from a C wide string.
   *
   * @param str The C wide string.
   * @throw EncodingException If the string contains invalid code points.
   */
  String(const wchar_t* str);

  /**
   * Construct a new string from a C string encoded in UTF-8.
   *
   * @param str The C string.
   * @param offset The number of characters from the beginning of the string
   * to skip over.
   * @param count The number of characters from the offset to use, or 0 to
   * use all the characters to the end of the string.
   * @throw EncodingException If the string does not contain valid UTF-8.
   */
  String(const char* str, uint_t offset = 0, uint_t count = 0);

  /** Destructor. */
  ~String();

  /**
   * Test if the string ends with the given string.
   *
   * @param str The other string.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if this string ends with <i>str</i>, <b>false</b>
   * otherwise.
   */
  bool endsWith(const String& str, bool ignoreCase = false) const;

  /**
   * Test if the string ends with the given character.
   *
   * @param c The character.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the string ends with <i>c</i>, <b>false</b>
   * otherwise.
   */
  bool endsWith(const Char& c, bool ignoreCase = false) const;

  /**
   * Test if the string ends with any character in a given set of characters.
   *
   * @param chars The set of characters.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the string ends with one of the characters
   * in the set, <b>false</b> otherwise.
   */
  bool endsWithAnyOf(const String& chars, bool ignoreCase = false) const;

  /**
   * Test if the string starts with the given string.
   *
   * @param str The other string.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the String begins with <i>str</i>,
   * <b>false</b> otherwise.
   */
  bool startsWith(const String& str, bool ignoreCase = false) const;

  /**
   * Test if the string starts with the given character.
   *
   * @param c The character.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the string begins with <i>c</i>, <b>false</b>
   * otherwise.
   */
  bool startsWith(const Char& c, bool ignoreCase = false) const;

  /**
   * Test if the string starts with any character in a given set of
   * characters.
   *
   * @param chars The set of characters.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the string starts with one of the
   * characters in the set, <b>false</b> otherwise.
   */
  bool startsWithAnyOf(const String& chars, bool ignoreCase = false) const;

  /**
   * Get the length of the string.
   *
   * @return The length, in characters, or 0 if the String is <b>null</b>.
   */
  uint_t getLength() const;

  /**
   * Get the length of the string.
   *
   * @return The length, in characters, or 0 if the string is <b>null</b>.
   */
  inline uint_t length() const
  { return(getLength()); }

  /**
   * Set the length of the string. Allows the string to be
   * truncated to an arbitrary length.
   *
   * @param length The new length. The new length must be less than the
   * current length; otherwise the string is left unmodified.
   */
  void setLength(uint_t length);

  /**
   * Reserve space in the string for the given number of characters. If the
   * new capacity is less than or equal to the current capacity, the string
   * is not modified.
   *
   * @param capacity The new capacity.
   */
  void reserve(uint_t capacity);

  /**
   * Get the current capacity of the string, in characters. This value
   * takes into account the space required for the final NUL terminator.
   */
  uint_t getCapacity() const;

  /**
   * Compact the string so that it occupies the minimum amount of memory
   * required.
   */
  void compact();

  /**
   * Clear the string. Sets the length of the string to 0.
   *
   * @return The string.
   */
  inline String& clear()
  {
    setLength(0);
    return(*this);
  }

  /**
   * Test if the string is empty.
   *
   * @return <b>true</b> if the string is <b>null</b> or has length 0,
   * <b>false</b> otherwise.
   */
  bool isEmpty() const;

  /**
   * Test if the string is <b>null</b>.
   *
   * @return <b>true</b> if the string is <b>null</b>, <b>false</b>
   * otherwise.
   */
  bool isNull() const;

  /**
   * Test if the string holds an integer. For example, "123", "+5", and
   * "-123" are integers.
   *
   * @return <b>true</b> if the string is an integer, <b>false</b>
   * otherwise.
   */
  bool isInteger() const;

  /**
   * Test if the string consists solely of alphanumeric characters.
   *
   * @return <b>true</b> if the string is alphanumeric, <b>false</b>
   * otherwise.
   */
  bool isAlphaNumeric() const;

  /**
   * Compare the string to another.
   *
   * @param other The string to compare to.
   * @param ignoreCase A flag indicating whether the compare should be
   * case insensitive.
   * @return A negative value if the string is lexicographically "less"
   * than <i>other</i>, a positive value if it is "greater" than
   * <i>other</i>, and 0 if they are "equal".
   */
  int compareTo(const String& other, bool ignoreCase = false) const;

  /**
   * Trim leading and trailing characters from the string.
   *
   * @return The string.
   */
  String& trim(const String &chars = whitespaceChars, bool begin = true,
               bool end = true);

  /**
   * Trim characters from the beginning of the string.
   *
   * @param chars The set of characters to trim (the set of whitespace
   * characters by default).
   */
  inline String& trimBegin(const String& chars = whitespaceChars)
  { return(trim(chars, true, false)); }

  /**
   * Trim characters from the end of the string.
   *
   * @param chars The set of characters to trim (the set of whitespace
   * characters by default).
   */
  inline String& trimEnd(const String& chars = whitespaceChars)
  { return(trim(chars, false, true)); }

  /**
   * Truncate the string at the first occurrence of the given character.
   *
   * @param c The character at which to truncate.
   * @return The string.
   */
  String& chop(const Char& c);

  /**
   * Truncate the string at the last occurrence of the given character.
   *
   * @param c The character at which to truncate.
   * @return The string.
   */
  String& reverseChop(const Char& c);

  /**
   * Replace all occurences of one character with another.
   *
   * @param a The character to replace.
   * @param b The replacement character.
   * @return The string.
   */
  String& replace(const Char& a, const Char& b);

  /**
   * Replace all occurences of one substring with another.
   *
   * @param a The substring to replace.
   * @param b The replacement substring.
   * @return The string.
   */
  String& replace(const String& a, const String& b);

  /**
   * Insert a character into the string.
   *
   * @param c The character to insert.
   * @param index The index at which to insert the character. If the index
   * is out of range, the character is appended to the end of the string.
   */
  String& insert(const Char& c, uint_t index = 0);

  /**
   * Insert a string into this string.
   *
   * @param str The string to insert.
   * @param index The index at which to insert the string. If the index
   * is out of range, the string is appended to the end of this string.
   * @param length The maximum number of characters from <i>str</i> to
   * insert.
   */
  String& insert(const String& str, uint_t index = 0, uint_t length = 0);

  /**
   * Insert a UTF-16 character array into this string.
   *
   * @param str The array to insert.
   * @param index The index at which to insert the characters. If the index
   * is out of range, the characters are appended to the end of this string.
   * @param count The number of characters from <i>str</i> to insert.
   */
  String& insert(const char16_t* str, uint_t index = 0, uint_t count = 0);

  /**
   * Insert a C wide string to this string.
   *
   * @param str The C wide string to insert.
   * @param index The index at which to insert the string. If the index
   * is out of range, the string is appended to the end of this string.
   * @param count The maximum number of characters from <i>str</i> to
   * insert.
   */
  inline String& insert(const wchar_t* str, uint_t index = 0, uint_t count = 0)
  { return(insert(String(str), index, count)); }

  /**
   * Insert a C string to this string.
   *
   * @param str The C wide string to insert.
   * @param index The index at which to insert the string. If the index
   * is out of range, the string is appended to the end of this string.
   * @param count The maximum number of characters from <i>str</i> to
   * insert.
   */
  inline String& insert(const char* str, uint_t index = 0, uint_t count = 0)
  { return(insert(String(str), index, count)); }

  /**
   * Append a string representation of a boolean value to the end of
   * the string.
   *
   * @param v The value to append.
   */
  String& append(bool v);

  /**
   * Append a character to the end of the string.
   *
   * @param c The character to append.
   */
  inline String& append(char c)
  { return(append(Char(c))); }

  /**
   * Append a UTF-16 character to the end of the string.
   *
   * @param c The character to append.
   */
  inline String& append(char16_t c)
  { return(append(Char(c))); }

  /**
   * Append a string representation of an integer value onto the end of
   * the string.
   *
   * @param val The value to append.
   */
  String& append(int val);

  /**
   * Append a string representation of an integer value in the
   * given base onto the end of the string.
   *
   * @param val The value to append.
   * @param base The numerical base in which to output the value. The valid
   * range is 2 - 36, inclusive.
   */
  String& append(int val, uint_t base);

  /**
   * Append a string representation of an unsigned integer value
   * onto the end of the string.
   *
   * @param val The value to append.
   */
  String& append(uint_t val);

  /**
   * Append a string representation of an unsigned integer value in the
   * given base onto the end of the string.
   *
   * @param val The value to append.
   * @param base The numerical base in which to output the value. The valid
   * range is 2 - 36, inclusive.
   */
  String& append(uint_t val, uint_t base);

  /**
   * Append a string representation of a long long (64-bit) integer onto
   * the end of the string.
   *
   * @param val The value to append.
   */
  String& append(const int64_t& val);

  /**
   * Append a string representation of double-precision floating point
   * value onto the end of the string.
   *
   * @param val The value to append.
   * @param precision The precision (the number of digits to include after
   * the radix).
   */
  String& append(const double& val, uint_t precision = 6);

  /**
   * Append a string representation of an unsigned long long
   * (64-bit) integer onto the end of the string.
   *
   * @param val The value to append.
   */
  String& append(const uint64_t& val);

  /**
   * Append a character to the end of the string.
   *
   * @param c The character to append.
   */
  inline String& append(const Char& c)
  { return(insert(c, END)); }

  /**
   * Append a string to the end of the string.
   *
   * @param str The character to append.
   * @param count The maximum number of characters from <i>str</i> to
   * append, or 0 to append all of <i>str</i>.
   */
  inline String& append(const String& str, uint_t count = 0)
  { return(insert(str, END, count)); }

  /**
   * Append a UTF-16 character array to the end of the string.
   *
   * @param str The character array to append.
   * @param count The maximum number of characters from <i>str</i> to
   * append, or 0 to append all of <i>str</i>.
   */
  inline String& append(const char16_t* str, uint_t count = 0)
  { return(insert(str, END, count)); }

  /**
   * Append a string to the end of the string.
   *
   * @param str The string to append.
   * @param count The maximum number of characters from <i>str</i> to
   * append, or 0 to append all of <i>str</i>.
   */
  inline String& append(const char* str, uint_t count = 0)
  { return(insert(String(str), END, count)); }

  /**
   * Append a wide string to the end of the string.
   *
   * @param str The string to append.
   * @param count The maximum number of characters from <i>str</i> to
   * append, or 0 to append all of <i>str</i>.
   */
  inline String& append(const wchar_t* str, uint_t count = 0)
  { return(insert(str, END, count)); }

  /**
   * Prepend a character to the beginning of the string.
   *
   * @param c The character to prepend.
   */
  inline String& prepend(char c)
  { return(insert(Char(c), 0)); }

  /**
   * Prepend a character to the beginning of the string.
   *
   * @param c The character to prepend.
   */
  inline String& prepend(char16_t c)
  { return(insert(Char(c), 0)); }

  /**
   * Prepend a character to the beginning of the string.
   *
   * @param c The character to prepend.
   */
  inline String& prepend(const Char& c)
  { return(insert(c, 0)); }

  /**
   * Prepend a string to the beginning of the string.
   *
   * @param str The character to prepend.
   * @param count The maximum number of characters from <i>str</i> to
   * prepend, or 0 to prepend all of <i>str</i>.
   */
  inline String& prepend(const String& str, uint_t count = 0)
  { return(insert(str, 0, count)); }

  /**
   * Prepend a string to the beginning of the string.
   *
   * @param str The character to prepend.
   * @param count The maximum number of characters from <i>str</i> to
   * prepend, or 0 to prepend all of <i>str</i>.
   */
  inline String& prepend(const char16_t* str, uint_t count = 0)
  { return(insert(str, 0, count)); }

  /**
   * Prepend a string to the beginning of the string.
   *
   * @param str The character to prepend.
   * @param count The maximum number of characters from <i>str</i> to
   * prepend, or 0 to prepend all of <i>str</i>.
   */
  inline String& prepend(const char* str, uint_t count = 0)
  { return(insert(String(str), 0, count)); }

  /**
   * Prepend a wide string to the beginning of the string.
   *
   * @param str The character to prepend.
   * @param count The maximum number of characters from <i>str</i> to
   * prepend, or 0 to prepend all of <i>str</i>.
   */
  inline String& prepend(const wchar_t* str, uint_t count = 0)
  { return(insert(str, 0, count)); }

  /**
   * Remove a portion of the string.
   *
   * @param fromIndex The start index of the range to remove.
   * @param toIndex The end index of the range to remove, or <i>END</i>
   * to remove to the end of the string.
   */
  String& remove(uint_t fromIndex, uint_t toIndex = END);

  /**
   * Return the character at the given index.
   *
   * @param index The index.
   * @return The character at the given index.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  Char charAt(uint_t index) const;

  /**
   * Set the character at the given index.
   *
   * @param index The index.
   * @param c The character to set.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  String& setCharAt(uint_t index, const Char& c);

  /**
   * Compute a hashcode for the string.
   *
   * @param modulo The desired range for the hashcode.
   * @return A hashcode in the range [0, <i>modulo</i>).
   */
  uint_t hash(uint_t modulo = 256) const;

  /**
   * Extract a substring from the string.
   *
   * @param fromIndex The start index of the substring.
   * @param toIndex The end index of the substring, or <i>END</i> to indicate
   * the end of the string.
   * @return The substring as a new string. If fromIndex and toIndex are
   * equal, an empty string is returned.
   */
  String substring(uint_t fromIndex, uint_t toIndex = END) const;

  /**
   * Find the first occurrence of a given character in the string.
   *
   * @param c The character to search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of the character, or -1
   * if the character was not found.
   */
  int indexOf(const Char& c, uint_t fromIndex = 0) const;

  /**
   * Find the first occurrence of a given string in this string.
   *
   * @param str The string to search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of the string, or -1
   * if the string was not found.
   */
  inline int indexOf(const char* str, uint_t fromIndex = 0) const
  { return(indexOf(String(str), fromIndex)); }

  /**
   * Find the first occurrence in this string of any character in a set
   * of characters.
   *
   * @param chars The set of characters search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of any character in the set,
   * or -1 if no occurrence was not found.
   */
  int indexOfAnyOf(const String& chars, uint_t fromIndex = 0) const;

  /**
   * Find the first occurrence of a given string in this string.
   *
   * @param str The string to search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of the string, or -1
   * if the string was not found.
   */
  int indexOf(const String& str, uint_t fromIndex = 0) const;

  /**
   * Find the last occurrence of a given character in this string.
   *
   * @param c The character to search for.
   * @param fromIndex The index from which to begin searching backwards,
   * or <b>END</b> to start at the end of the string.
   * @return The index of the last occurrence of the character, or -1
   * if the character was not found.
   */
  int lastIndexOf(const Char& c, uint_t fromIndex = END) const;

  /**
   * Find the last occurrence in the string of any character in a set
   * of characters.
   *
   * @param chars The set of characters search for.
   * @param fromIndex The index from which to begin searching backwards,
   * or <b>END</b> to start at the end of the string.
   * @return The index of the last occurrence of any character in the set,
   * or -1 if no occurrence was not found.
   */
  int lastIndexOfAnyOf(const String& chars, uint_t fromIndex = END) const;

  /**
   * Find the last occurrence of a given string in this string.
   *
   * @param str The string to search for.
   * @param fromIndex The index from which to begin searching backwards,
   * or <b>END</b> to start at the end of the string.
   * @return The index of the last occurrence of the string, or -1
   * if the string was not found.
   */
  int lastIndexOf(const String& str, uint_t fromIndex = END) const;

  /**
   * Determine if the string contains a given character.
   *
   * @param c The character to search for.
   * @return <b>true</b> if the string contains at least one occurrence of
   * the character, <b>false</b> otherwise.
   */
  inline bool contains(const Char& c) const
  { return(indexOf(c) >= 0); }

  /**
   * Determine if the string contains a given substring.
   *
   * @param str The substring to search for.
   * @return <b>true</b> if the string contains at least one occurrence of
   * the substring, <b>false</b> otherwise.
   */
  inline bool contains(const String& str) const
  { return(indexOf(str) >= 0); }

  /**
   * Determine if the string contains any character in a given set of
   * characters.
   *
   * @param chars The set of characters.
   * @return <b>true</b> if the string contains at least one occurrence of
   * any character in the set, <b>false</b> otherwise.
   */
  inline bool containsAnyOf(const char* chars) const
  { return(indexOfAnyOf(chars) >= 0); }

  /**
   * Extract a token from the string.
   *
   * @param fromIndex The start index at which to begin searching for a
   * token. Will be modified on return to point to one character past the
   * extracted token.
   * @param delim The set of possible token delimiter characters (the set
   * of whitespace characters by default).
   * @return The extracted token, or <b>String::null</b> if the end of the
   * string has been reached.
   */
  String nextToken(uint_t& fromIndex, const String& delim = whitespaceChars)
    const;

  /**
   * Split the string into a series of tokens, without modifying the
   * string.
   *
   * @param tokens The vector in which to place the extracted tokens. If
   * a non-empty vector is supplied, the tokens are added to the end of
   * the vector.
   * @param delim The set of possible token delimiter characters (the set
   * of whitespace characters by default).
   * @param limit The maximum number of tokens to extract, or 0 for no
   * limit (the default).
   * @return The number of tokens extracted.
   */
  uint_t split(StringVec& tokens, const String& delim = whitespaceChars,
               uint_t limit = 0) const;

  /** Convert the string to lower case. */
  String& toLowerCase();

  /** Convert the string to upper case. */
  String& toUpperCase();

  /**
   * Fill the string with a character.
   *
   * @param c The fill character.
   * @param count The number of characters to fill.
   */
  String& fill(const Char& c, uint_t count);

  /**
   * Pad the string with a character.
   *
   * @param c The pad character.
   * @param count The number of characters to pad.
   */
  String& pad(const Char& c, uint_t count);

  /**
   * Parse a boolean value from the string.
   *
   * @param index The index to begin parsing at.
   * @return The parsed value.
   */
  bool toBool(uint_t index = 0) const;

  /**
   * Parse an integer from the string.
   *
   * @param index The index to begin parsing at.
   * @param base The base (2 - 36).
   * @return The parsed value.
   */
  int toInt(uint_t index = 0, uint_t base = 10) const;

  /**
   * Parse an unsigned integer from the string.
   *
   * @param index The index to begin parsing at.
   * @param base The base (2 - 36).
   * @return The parsed value.
   */
  uint_t toUInt(uint_t index = 0, uint_t base = 10) const;

  /**
   * Parse a 64-bit integer from the string.
   *
   * @param index The index to begin parsing at.
   * @param base The base (2 - 36).
   * @return The parsed value.
   */
  int64_t toInt64(uint_t index = 0, uint_t base = 10) const;

  /**
   * Parse an unsigned 64-bit integer from the string.
   *
   * @param index The index to begin parsing at.
   * @param base The base (2 - 36).
   * @return The parsed value.
   */
  uint64_t toUInt64(uint_t index = 0, uint_t base = 10) const;

  /**
   * Parse a floating point value from the string.
   *
   * @param index The index to begin parsing at.
   * @return The parsed value.
   */
  float toFloat(uint_t index = 0) const;

  /**
   * Parse a double precision floating point value from the string.
   *
   * @param index The index to begin parsing at.
   * @return The parsed value.
   */
  double toDouble(uint_t index = 0) const;

  /** Pointer cast operator. */
  operator const void*() const;

  /**
   * Returns a pointer to the UTF-16 data for this string.
   */
  const char16_t* data() const;

  /** Return the contents of the string as an immutable C string. */
  CString toUTF8() const;

  /**
   * Assignment operator.
   *
   * @throw EncodingException If the input is not valid UTF-8.
   */
  String& operator=(const char* other);

  /**
   * Assignment operator.
   *
   * @throw EncodingException If the input contains invalid UTF-32 characters.
   */
  String& operator=(const wchar_t* other);

  /** Assignment operator. */
  String& operator=(const String& other);

  /**
   * Assignment operator.
   *
   * @throw EncodingException If the input is not valid UTF-8.
   */
  inline String& operator=(const std::string& other)
  { return(operator=(other.c_str())); }

  /**
   * %Array index operator. Obtain a reference to the character at the given
   * offset in the string.
   *
   * @param index The index.
   * @return A reference to the character at the specified index.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  CharRef operator[](int index);

  /**
   * %Array index operator. Obtain a copy of the character at the given
   * offset in the string.
   *
   * @param index The index.
   * @return The character at the specified index, as a Char.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  const Char operator[](int index) const;

  /**
   * Return a const_iterator to the beginning of the string.
   *
   * @throw NullPointerException If the string is <b>null</b>.
   */
  const_iterator begin() const;

  /**
   * Return a const_iterator to the end of the string.
   *
   * @throw NullPointerException If the string is <b>null</b>.
   */
  const_iterator end() const;

  /**
   * Substring operator. Extract a substring from the string.
   *
   * @param fromIndex The start index of the substring.
   * @param toIndex The end index of the substring, or <i>END</i> to indicate
   * the end of the string.
   * @return The substring as a new string. If fromIndex and toIndex are
   * equal, an empty string is returned.
   */
  inline String operator()(uint_t fromIndex, uint_t toIndex = END) const
  { return(substring(fromIndex, toIndex)); }

  /**
   * Unary NOT operator.
   *
   * @return <b>true</b> if the string is <b>null</b>, <b>false</b>
   * otherwise.
   */
  inline bool operator!() const
  { return(isNull()); }

  /**
   * Append a character onto the end of the string.
   *
   * @param c The character to append.
   */
  inline String& operator+=(char c)
  { return(append(c)); }

  /**
   * Append a UTF-16 character onto the end of the string.
   *
   * @param c The character to append.
   */
  inline String& operator+=(char16_t c)
  { return(append(c)); }

  /**
   * Append a character onto the end of the string.
   *
   * @param c The character to append.
   */
  inline String& operator+=(const Char& c)
  { return(append(c)); }

  /**
   * Append a C string onto the end of this string.
   *
   * @param str The string to append.
   */
  inline String& operator+=(const char* str)
  { return(append(str)); }

  /**
   * Append a string onto the end of this string.
   *
   * @param str The string to append.
   */
  inline String& operator+=(const String& str)
  { return(append(str)); }

  /**
   * Append a string representation of a boolean value onto the end of
   * the string.
   *
   * @param val The value to append.
   */
  inline String& operator+=(bool val)
  { return(append(val)); }

  /**
   * Append a string representation of an integer value onto the end of
   * the string.
   *
   * @param val The value to append.
   */
  inline String& operator+=(int val)
  { return(append(val)); }

  /**
   * Append a string representation of an unsigned integer value onto the
   * end of the string.
   *
   * @param val The value to append.
   */
  inline String& operator+=(uint_t val)
  { return(append(val)); }

  /**
   * Append a string representation of double-precision floating point
   * value onto the end of the string.
   *
   * @param val The value to append.
   */
  inline String& operator+=(double val)
  { return(append(val)); }

  /**
   * Append a string representation of a long long (64-bit) integer onto
   * the end of the string.
   *
   * @param val The value to append.
   */
  inline String& operator+=(int64_t val)
  { return(append(val)); }

  /**
   * Append a string representation of an unsigned long long
   * (64-bit) integer onto the end of the string.
   *
   * @param val The value to append.
   */
  inline String& operator+=(uint64_t val)
  { return(append(val)); }

  /** Append operator. */
  inline String& operator<<(const char* str)
  { return(append(str)); }

  /** Append operator. */
  inline String& operator<<(const wchar_t* str)
  { return(append(str)); }

  /** Append operator. */
  inline String& operator<<(const String& str)
  { return(append(str)); }

  /** Append operator. */
  inline String& operator<<(bool val)
  { return(append(val)); }

  /** Append operator. */
  inline String& operator<<(char c)
  { return(append(c)); }

  /** Append operator. */
  inline String& operator<<(char16_t c)
  { return(append(c)); }

  /** Append operator. */
  inline String& operator<<(const Char& c)
  { return(append(c)); }

  /** Append operator. */
  inline String& operator<<(int val)
  { return(append(val)); }

  /** Append operator. */
  inline String& operator<<(uint_t val)
  { return(append(val)); }

  /** Append operator. */
  inline String& operator<<(const int64_t& val)
  { return(append(val)); }

  /** Append operator. */
  inline String& operator<<(const uint64_t& val)
  { return(append(val)); }

  /** Append operator. */
  inline String& operator<<(const double& val)
  { return(append(val)); }

  /** Prepend operator. */
  inline String& operator^=(char c)
  { return(prepend(c)); }

  /** Prepend operator. */
  inline String& operator^=(char16_t c)
  { return(prepend(c)); }

  /** Prepend operator. */
  inline String& operator^=(const Char& c)
  { return(prepend(c)); }

  /** Prepend operator. */
  inline String& operator^=(const char* str)
  { return(prepend(str)); }

  /** Prepend operator. */
  inline String& operator^=(const wchar_t* str)
  { return(prepend(str)); }

  /** Prepend operator. */
  inline String& operator^=(const String& str)
  { return(prepend(str)); }

  String operator+(const String& str);
  String operator+(const char* str);
  String operator+(const Char& c);

  /** Write the string (in UTF-8 form) to a stream. */
  void write(std::ostream& stream) const;

  /** A pseudo-index indicating the end of the string. */
  static const uint_t END;

  /** The <b>null</b> string. */
  static const String null;

  /** The string representation of the null value. */
  static const String nullRep;

  /** The empty string. */
  static const String empty;

  /** The set of whitespace delimiter characters. */
  static const String whitespaceChars;

 private:

  class StringBuf;

  StringBuf* _buf;

  void _insert(const char16_t* str, uint_t length, uint_t index, uint_t count);
  void _release();
  void _makeCopy(uint_t size, bool unshareable = false);
  char16_t* _makeGap(uint_t index, uint_t gaplen);
  void _fromUTF8(const char* str, uint_t len, uint_t offset);
  void _fromUTF32(const char32_t* str, uint_t len, uint_t offset);
  void _fromWChar(const wchar_t* str);
};

/** A String vector. A trivial extension of
 * <b>std::vector<String ></b>.
 *
 * @author Mark Lindner
 */
class StringVec : public std::vector<String>
{
 public:

  StringVec()
  { }

  /** Append operator. Pushes a string onto the end (back) of the vector. */
  inline StringVec& operator<<(String s)
  { push_back(s); return(*this); }

  /** An empty StringVec. */
  static const StringVec emptyVec;
};

inline bool operator==(const String& s1, const String& s2)
{ return(s1.compareTo(s2) == 0); }

inline bool operator==(const String& s1, const char* s2)
{ return(s1.compareTo(s2) == 0); }

inline bool operator==(const char* s1, const String& s2)
{ return(s2.compareTo(s1) == 0); }

inline bool operator!=(const String& s1, const String& s2)
{ return(s1.compareTo(s2) != 0); }

inline bool operator!=(const String& s1, const char* s2)
{ return(s1.compareTo(s2) != 0); }

inline bool operator!=(const char* s1, const String& s2)
{ return(s2.compareTo(s1) != 0); }

inline bool operator<(const String& s1, const String& s2)
{ return(s1.compareTo(s2) < 0); }

inline bool operator<(const String& s1, const char* s2)
{ return(s1.compareTo(s2) < 0); }

inline bool operator<(const char* s1, const String& s2)
{ return(s2.compareTo(s1) > 0); }

inline bool operator>(const String& s1, const String& s2)
{ return(s1.compareTo(s2) > 0); }

inline bool operator>(const String& s1, const char* s2)
{ return(s1.compareTo(s2) > 0); }

inline bool operator>(const char* s1, const String& s2)
{ return(s2.compareTo(s1) < 0); }

inline bool operator<=(const String& s1, const String& s2)
{ return(s1.compareTo(s2) <= 0); }

inline bool operator<=(const String& s1, const char* s2)
{ return(s1.compareTo(s2) <= 0); }

inline bool operator<=(const char* s1, const String& s2)
{ return(s2.compareTo(s1) > 0); }

inline bool operator>=(const String& s1, const String& s2)
{ return(s1.compareTo(s2) >= 0); }

inline bool operator>=(const String& s1, const char* s2)
{ return(s1.compareTo(s2) >= 0); }

inline bool operator>=(const char* s1, const String& s2)
{ return(s2.compareTo(s1) < 0); }

inline String operator+(const char* s1, const String& s2)
{ return(String(s1) + s2); }

inline String operator+(const Char& c, const String& s)
{ String t; t += c; t += s; return(t); }

inline std::ostream& operator<<(std::ostream& stream, const String& s)
{ s.write(stream); return(stream); }

} // namespace ccxx

#endif // __ccxx_String_hxx
