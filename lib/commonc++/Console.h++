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

#ifndef __ccxx_Console_hxx
#define __ccxx_Console_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

#include <cstdio>

namespace ccxx {

/**
 * A class that provides basic control of the console, such as changing
 * text attributes, clearing the screen, and moving the cursor.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Console
{
 public:

  /** Text colors. */
  enum TextColor { ColorBlack, ColorRed, ColorGreen, ColorYellow, ColorBlue,
                   ColorMagenta, ColorCyan, ColorWhite };

  /** Text styles. */
  enum TextStyle { StyleBold, StyleUnderline, StyleInverse };

  /** Construct a new Console object. */
  Console();

  /** Destructor. */
  ~Console();

  /**
   * Read text from the console.
   *
   * @param text The array of characters to read into. The array will always
   * be NUL-terminated on return.
   * @param maxlen The maximum number of characters to read.
   * @return The number of characters actually read.
   */
  size_t read(char* text, size_t maxlen);

  /**
   * Write text to the console.
   *
   * @param text The array of characters to write.
   * @param len The number of characters to write, or 0 to write all
   * characters in the array, up to a NUL.
   */
  void write(const char* text, size_t len = 0);

  /**
   * Enable or disable the echoing of input characters on the console.
   *
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  bool setEcho(bool on);

  /**
   * Move the cursor to a new position. If the specified position is not
   * valid, the cursor is not moved.
   *
   * @param row The new row.
   * @param column The new column.
   */
  void moveCursor(uint_t row, uint_t column);

  /** Clear the console. */
  void clear();

  /**
   * Set the foreground color for subsequent text.
   *
   * @param color The text color.
   */
  void setTextForeground(TextColor color);

  /**
   * Set the background color for subsequent text.
   *
   * @param color The text color.
   */
  void setTextBackground(TextColor color);

  /**
   * Turn on a text style for subsequent text.
   *
   * @param style The style to turn on.
   */
  void beginTextStyle(TextStyle style);

  /**
   * Turn off a text style for subsequent text.
   *
   * @param style The style to turn off.
   */
  void endTextStyle(TextStyle style);

  /** Turn off all text styles for subsequent text. */
  void resetTextStyle();

  /**
   * Set the title of the console window.
   *
   * @param title The new title.
   */
  void setTitle(const String& title);

  /**
   * Get the size of the console, in rows and columns.
   *
   * @param columns A pointer to the value in which to store the number
   * of columns. May be NULL.
   * @param rows A pointer to the value in which to store the number
   * of rows. May be NULL.
   * @return <b>true</b> on success, <b>false</b> if the size could not
   * be determined.
   */
  bool getSize(uint_t* columns, uint_t* rows);

 private:

#ifdef CCXX_OS_WINDOWS
  HANDLE _console;
  DWORD _attributes;
  bool _intense;
#else
  FILE* _console;
  bool _underline;
  bool _inverse;
#endif
  bool _bold;
};

} // namespace ccxx

#endif // __ccxx_Console_hxx
