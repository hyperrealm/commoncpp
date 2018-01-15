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

#ifndef __ccxx_Char_hxx
#define __ccxx_Char_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A class representing a UTF-16 character.
 */
class COMMONCPP_API Char
{
 public:

  /** Construct a new, NUL character. */
  Char();

  /** Construct a new character with the given ASCII value. */
  explicit Char(char c);

  /** Construct a new character with the given UTF-16 value. */
  explicit Char(char16_t c);

  /** Construct a new character with the given unicode value. Values above
   * 0xFFFF are truncated.
   */
  Char(int c);

  /** Destructor. */
  ~Char();

  /** Assignment operator. */
  Char& operator=(char c);

  /** Assignment operator. */
  Char& operator=(char16_t c);

  /** Assignment operator. */
  Char& operator=(int c);

  /** Tests if the character is NUL. */
  bool isNul() const;

  /** Tests if the character is a control character. */
  bool isControl() const;

  /** Tests if the character is printable. */
  bool isPrintable() const;

  /** Tests if the character is graphical. */
  bool isGraphical() const;

  /** Tests if the character is punctuation. */
  bool isPunctuation() const;

  /** Tests if the character is a letter. */
  bool isLetter() const;

  /** Tests if the character is a digit. */
  bool isDigit() const;

  /** Tests if the character is a hexadecimal digit. */
  bool isHexDigit() const;

  /** Tests if the character is alphabetic. */
  bool isAlphabetic() const;

  /** Tests if the character is alphanumeric. */
  bool isAlphaNumeric() const;

  /** Tests if the character is a sign symbol (+ or -). */
  bool isSign() const;

  /** Tests if the character is whitespace. */
  bool isWhitespace() const;

  /** Tests if the character is a lowercase letter. */
  bool isLowerCase() const;

  /** Tests if the character is an uppercase letter. */
  bool isUpperCase() const;

  /** Returns the lowercase form of the character */
  Char toLowerCase() const;

  /** Returns the uppercase form of the character */
  Char toUpperCase() const;

  /** Tests if the character is ASCII. */
  bool isASCII() const;

  /** Tests if the character is low ASCII. */
  bool isLowASCII() const;

  /** Tests if the character is high ASCII. */
  bool isHighASCII() const;

  /** Tests if the character is a low surrogate. */
  bool isLowSurrogate() const;

  /** Tests if the character is a high surrogate. */
  bool isHighSurrogate() const;

  /** Returns the char16_t for this character. */
  char16_t toChar16() const
  { return(_char); }

  /**
   * Returns the ASCII character equivalent for this character, or 0 if
   * this character is not ASCII.
   */
  char toASCII() const;

  /** Equality operator. */
  bool operator==(const Char& other) const
  { return(_char == other._char); }

  /** Equality operator. */
  bool operator==(char16_t other) const
  { return(_char == other); }

  /** Inequality operator. */
  bool operator!=(const Char& other) const
  { return(_char != other._char); }

  bool operator<(const Char& other) const
  { return(_char < other._char); }

  bool operator>(const Char& other) const
  { return(_char > other._char); }

  bool operator<=(const Char& other) const
  { return(_char <= other._char); }

  bool operator>=(const Char& other) const
  { return(_char >= other._char); }

  /** Equivalent to isNul(). */
  inline bool operator!() const
  { return(isNul()); }

  /** Increment operator (prefix). */
  inline Char& operator++()
  {
    ++_char;
    return(*this);
  }

  /** Increment operator (postfix). */
  inline Char operator++(int)
  { return(Char(_char++)); }

  /** Decrement operator (prefix). */
  inline Char& operator--()
  {
    --_char;
    return(*this);
  }

  /** Decrement operator (postfix). */
  inline Char operator--(int)
  { return(Char(_char--)); }

  /** The NUL character. */
  static const Char nul;

  /** The bell character. */
  static const Char BELL;

  /** The backspace character. */
  static const Char BACKSPACE;

  /** The tab character. */
  static const Char TAB;

  /** The newline character. */
  static const Char NEWLINE;

  /** The vertical tab character. */
  static const Char VERTICAL_TAB;

  /** The form feed character. */
  static const Char FORM_FEED;

  /** The carriage return character. */
  static const Char RETURN;

  /** The escape character. */
  static const Char ESCAPE;

  /** The space character. */
  static const Char SPACE;

  /** The delete character. */
  static const Char DELETE;

  /** Tests if a character is NUL. */
  static bool isNul(char16_t c);

  /** Tests if a character is a control character. */
  static bool isControl(char16_t c);

  /** Tests if a character is a printable character. */
  static bool isPrintable(char16_t c);

  /** Tests if a character is a graphical character. */
  static bool isGraphical(char16_t c);

  /** Tests if a character is a punctuation character. */
  static bool isPunctuation(char16_t c);

  /** Tests if the character is a letter. */
  static bool isLetter(char16_t c);

  /** Tests if a character is a digit. */
  static bool isDigit(char16_t c);

  /** Tests if a character is a hexadeciaml digit. */
  static bool isHexDigit(char16_t c);

  /** Tests if a character is alphabetic. */
  static bool isAlphabetic(char16_t c);

  /** Tests if a character is alphanumeric. */
  static bool isAlphaNumeric(char16_t c);

  /** Tests if a character is a sign symbol (+ or -). */
  static bool isSign(char16_t c);

  /** Tests if a character is whitespace. */
  static bool isWhitespace(char16_t c);

  /** Tests if a character is a lowercase letter. */
  static bool isLowerCase(char16_t c);

  /** Tests if a character is an uppercase letter. */
  static bool isUpperCase(char16_t c);

  /** Returns the lowercase form of a character */
  static char16_t toLowerCase(char16_t c);

  /** Returns the uppercase form of a character */
  static char16_t toUpperCase(char16_t c);

  /** Tests if a character is high ASCII. */
  static bool isASCII(char16_t c);

  /** Tests if a character is low ASCII. */
  static bool isLowASCII(char16_t c);

  /** Tests if a character is high ASCII. */
  static bool isHighASCII(char16_t c);

  /** Tests if a character is a low surrogate. */
  static bool isLowSurrogate(char16_t c);

  /** Tests if a character is a high surrogate. */
  static bool isHighSurrogate(char16_t c);

  /**
   * Returns the character in ASCII, or NUL if the character is not in the
   * range of ASCII characters.
   */
  static char toASCII(char16_t c);

 private:

  char16_t _char;
};

inline bool operator==(char c1, const Char& c2)
{ return(static_cast<char16_t>(c1) == c2.toChar16()); }

inline bool operator!=(char c1, const Char& c2)
{ return(static_cast<char16_t>(c1) != c2.toChar16()); }

inline bool operator<(char c1, const Char& c2)
{ return(static_cast<char16_t>(c1) < c2.toChar16()); }

inline bool operator<=(char c1, const Char& c2)
{ return(static_cast<char16_t>(c1) <= c2.toChar16()); }

inline bool operator>(char c1, const Char& c2)
{ return(static_cast<char16_t>(c1) > c2.toChar16()); }

inline bool operator>=(char c1, const Char& c2)
{ return(static_cast<char16_t>(c1) >= c2.toChar16()); }

inline bool operator==(char16_t c1, const Char& c2)
{ return(c1 == c2.toChar16()); }

inline bool operator!=(char16_t c1, const Char& c2)
{ return(c1 != c2.toChar16()); }

inline bool operator<(char16_t c1, const Char& c2)
{ return(c1 < c2.toChar16()); }

inline bool operator<=(char16_t c1, const Char& c2)
{ return(c1 <= c2.toChar16()); }

inline bool operator>(char16_t c1, const Char& c2)
{ return(c1 > c2.toChar16()); }

inline bool operator>=(char16_t c1, const Char& c2)
{ return(c1 >= c2.toChar16()); }

} // namespace ccxx

#endif // __ccxx_Char_hxx
