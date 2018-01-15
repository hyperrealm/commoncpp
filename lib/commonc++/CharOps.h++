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

#ifndef __ccxx_CharOps_hxx
#define __ccxx_CharOps_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * UTF-16 character array operations. Unless stated otherwise, all methods
 * assume that UTF-16 character array arguments are NUL-terminated.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API CharOps
{
 public:

  /**
   * Compare two UTF-16 character arrays.
   *
   * @param a The first array.
   * @param b The second array.
   * @param ignoreCase Whether the comparison should be case-insensitive.
   * @return A value less than 0 if a is lexicographically less than b,
   * a value greater than 0 if it a is lexicographically greater than b,
   * or 0 if the arrays are equal.
   */
  static int compare(const char16_t* a, const char16_t* b,
                     bool ignoreCase = false);

  /**
   * Compare two UTF-16 characters.
   *
   * @param a The first character.
   * @param b The second character.
   * @param ignoreCase Whether the comparison should be case-insensitive.
   * @return A value less than 0 if a is lexicographically less than b,
   * a value greater than 0 if it a is lexicographically greater than b,
   * or 0 if the characters are equal.
   */
  static int compare(const char16_t a, const char16_t b,
                     bool ignoreCase = false);

  /**
   * Compare two UTF-16 character arrays.
   *
   * @param a The first character.
   * @param b The second character.
   * @param n The number of characters to compare.
   * @param ignoreCase Whether the comparison should be case-insensitive.
   * @return A value less than 0 if a is lexicographically less than b,
   * a value greater than 0 if it a is lexicographically greater than b,
   * or 0 if the character arrays are equal.
   */
  static int compare(const char16_t* a, const char16_t* b, uint_t n,
                     bool ignoreCase = false);

  /**
   * Move an array of UTF-16 characters to another, possibly overlapping
   * array.
   *
   * @param dest The destination array.
   * @param src The source array.
   * @param n The number of characters to move.
   * @return The destination array.
   */
  static char16_t* move(char16_t* dest, char16_t* src, uint_t n);

  /**
   * Find a UTF-16 character in a UTF-16 character array.
   *
   * @param str The array to search.
   * @param c The character to find.
   * @param len The length of the array to search.
   * @return A pointer to the character, if found, otherwise <b>NULL</b>.
   */
  static const char16_t* find(const char16_t* str, char16_t c, uint_t len = 0);

  /**
   * Find a UTF-16 character in a UTF-16 character array.
   *
   * @param str The array to search.
   * @param c The character to find.
   * @param len The length of the array to search.
   * @return A pointer to the character, if found, otherwise <b>NULL</b>.
   */
  static char16_t* find(char16_t* str, char16_t c, uint_t len = 0);

  /**
   * Find a UTF-16 character sequence in a UTF-16 character array.
   *
   * @param str The array to search.
   * @param s The NUL-terminated character sequence to find.
   * @return A pointer to the beginning of the sequence, if found; otherwise
   * <code>NULL</code>.
   */
  static const char16_t* find(const char16_t* str, const char16_t* s);

  /**
   * Find a UTF-16 character sequence in a UTF-16 character array.
   *
   * @param str The array to search.
   * @param s The NUL-terminated character sequence to find.
   * @return A pointer to the beginning of the sequence, if found; otherwise
   * <code>NULL</code>.
   */
  static char16_t* find(char16_t* str, const char16_t* s);

  /**
   * Find the last occurrence of a UTF-16 character in a UTF-16 character
   * array.
   *
   * @param str The array to search.
   * @param c The character to find.
   * @param fromIndex The end index from which to start the (reverse) search,
   * or END to start from the end of the array.
   * @return A pointer to the character, if found, otherwise <b>NULL</b>.
   */
  static const char16_t* findLast(const char16_t* str, char16_t c,
                                  uint_t fromIndex = END);

  /**
   * Find the last occurrence of a UTF-16 character in a UTF-16 character
   * array.
   *
   * @param str The array to search.
   * @param c The character to find.
   * @param fromIndex The end index from which to start the (reverse) search,
   * or END to start from the end of the array.
   * @return A pointer to the character, if found, otherwise <b>NULL</b>.
   */
  static char16_t* findLast(char16_t* str, char16_t c, uint_t fromIndex = END);

  /**
   * Find the last occurrence of a UTF-16 character sequence in a UTF-16
   * character* array.
   *
   * @param str The array to search.
   * @param s The character sequence to find.
   * @param fromIndex The end index from which to start the (reverse) search,
   * or END to start from the end of the array.
   * @return A pointer to the beginning of the sequence, if found, otherwise
   * <b>NULL</b>.
   */
  static const char16_t* findLast(const char16_t* str, const char16_t* s,
                                  uint_t fromIndex = END);

  /**
   * Find the last occurrence of a UTF-16 character sequence in a UTF-16
   * character* array.
   *
   * @param str The array to search.
   * @param s The character sequence to find.
   * @param fromIndex The end index from which to start the (reverse) search,
   * or END to start from the end of the array.
   * @return A pointer to the beginning of the sequence, if found, otherwise
   * <b>NULL</b>.
   */
  static char16_t* findLast(char16_t* str, const char16_t* s,
                            uint_t fromIndex = END);

  /**
   * Calculate the length (in characters) of a UTF-16 character array.
   *
   * @param str The array.
   * @return The length of the array, in unicode characters.
   */
  static uint_t length(const char16_t* str);

  /**
   * Copy at most <i>n</i> unicode characters from one UTF-16 character array
   * to another.
   *
   * @param dest The destination array.
   * @param src The sourcde array.
   * @param n The maximum number of unicode characters to copy.
   * @return The destination array.
   */
  static char16_t* copy(char16_t* dest, const char16_t* src, uint_t n);

  /**
   * Copy exactly <i>n</i> unicode characters from one UTF-16 character array
   * to another, without regard for NUL characters.
   *
   * @param dest The destination array.
   * @param src The sourcde array.
   * @param n The exact number of unicode characters to copy.
   * @return The destination array.
   */
  static char16_t* copyRaw(char16_t* dest, const char16_t* src, uint_t n);

  /**
   * Parse a 32-bit integer value from a UTF-16 character array.
   *
   * @param str The array to parse.
   * @param base The base of the numeric value.
   * @return The parsed value, or 0 if a valid value could not be parsed.
   */
  static int32_t toLong(const char16_t* str, int base = 10);

  /**
   * Parse an unsigned 32-bit integer value from a UTF-16 character array.
   *
   * @param str The array to parse.
   * @param base The base of the numeric value.
   * @return The parsed value, or 0 if a valid value could not be parsed.
   */
  static uint32_t toULong(const char16_t* str, int base = 10);

  /**
   * Parse a 64-bit integer value from a UTF-16 character array.
   *
   * @param str The array to parse.
   * @param base The base of the numeric value.
   * @return The parsed value, or 0 if a valid value could not be parsed.
   */
  static int64_t toLongLong(const char16_t* str, int base = 10);

  /**
   * Parse an unsigned 64-bit integer value from a UTF-16 character array.
   *
   * @param str The array to parse.
   * @param base The base of the numeric value.
   * @return The parsed value, or 0 if a valid value could not be parsed.
   */
  static uint64_t toULongLong(const char16_t* str, int base = 10);

  /**
   * Parse a floating point value from a UTF-16 character array.
   *
   * @param str The array to parse.
   * @return The parsed value, or 0.0f if a valid value could not be parsed.
   */
  static float toFloat(const char16_t* str);

  /**
   * Parse a double-precision floating point value from a UTF-16 character
   * array.
   *
   * @param str The array to parse.
   * @return The parsed value, or 0.0 if a valid value could not be parsed.
   */
  static double toDouble(const char16_t* str);

  /** A pseudo-index indicating the end of a UTF-16 character array. */
  static const uint_t END;
  static const char16_t TRUE_REP[];
  static const char16_t FALSE_REP[];
  static const char16_t DIGITS[];

 private:

  static char* _toASCII(const char16_t* str, char* buf, uint_t len);
};

} // namespace ccxx

#endif // __ccxx_CharOps_hxx
