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

#ifndef __ccxx_TerminalAttr_hxx
#define __ccxx_TerminalAttr_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * @def TERMATTR_ESC
 * Terminal control escape character.
 *
 * @def TERMATTR_NORMAL
 * Normal font.
 * @def TERMATTR_BOLD
 * Bold font.
 * @def TERMATTR_DIM
 * Dim (half-intensity) font.
 * @def TERMATTR_UNDERLINE
 * Underlined font.
 * @def TERMATTR_BLINK
 * Blinking font.
 * @def TERMATTR_INVERSE
 * Inverse (reverse-video) font.
 *
 * @def TERMATTR_FG_BLACK
 * Set text foreground color to black.
 * @def TERMATTR_FG_RED
 * Set text foreground color to red.
 * @def TERMATTR_FG_GREEN
 * Set text foreground color to green.
 * @def TERMATTR_FG_YELLOW
 * Set text foreground color to yellow.
 * @def TERMATTR_FG_BLUE
 * Set text foreground color to blue.
 * @def TERMATTR_FG_MAGENTA
 * Set text foreground color to magenta.
 * @def TERMATTR_FG_CYAN
 * Set text foreground color to cyan.
 * @def TERMATTR_FG_WHITE
 * Set text foreground color to white.
 * @def TERMATTR_FG_NORMAL
 * Set text foreground color to default.
 *
 * @def TERMATTR_BG_BLACK
 * Set text background color to black.
 * @def TERMATTR_BG_RED
 * Set text background color to red.
 * @def TERMATTR_BG_GREEN
 * Set text background color to green.
 * @def TERMATTR_BG_YELLOW
 * Set text background color to yellow.
 * @def TERMATTR_BG_BLUE
 * Set text background color to blue.
 * @def TERMATTR_BG_MAGENTA
 * Set text background color to magenta.
 * @def TERMATTR_BG_CYAN
 * Set text background color to cyan.
 * @def TERMATTR_BG_WHITE
 * Set text background color to white.
 * @def TERMATTR_BG_NORMAL
 * Set text background color to default.
 *
 * @def TERMATTR_LINECHARS_ON
 * Turn on alternate (line drawing) character set.
 * @def TERMATTR_LINECHARS_OFF
 * Turn off alternate (line drawing) character set.
 *
 * @def TERMATTR_CLEAR_SCREEN
 * Clear the screen.
 * @def TERMATTR_CLEAR_LINE
 * Clear the line at the cursor position.
 *
 * @def TERMATTR_CURSOR(Y, X)
 * Move the cursor to row Y, column X.
 * @def TERMATTR_CURSOR_UP(N)
 * Move the cursor up N lines.
 * @def TERMATTR_CURSOR_DOWN(N)
 * Move the cursor down N lines.
 * @def TERMATTR_CURSOR_RIGHT(N)
 * Move the cursor right N characters.
 * @def TERMATTR_CURSOR_LEFT(N)
 * Move the cursor left N characters.
 * @def TERMATTR_CURSOR_HOME
 * Move the cursor to home position.
 *
 * @def TERMATTR_SCROLL_UP
 * Scroll the screen up one line.
 * @def TERMATTR_SCROLL_DOWN
 * Scroll the screen down one line.
 */

#define TERMATTR_ESC "\033"

#define TERMATTR_NORMAL               TERMATTR_ESC "[0m"
#define TERMATTR_BOLD                 TERMATTR_ESC "[1m"
#define TERMATTR_DIM                  TERMATTR_ESC "[2m"
#define TERMATTR_UNDERLINE            TERMATTR_ESC "[4m"
#define TERMATTR_BLINK                TERMATTR_ESC "[5m"
#define TERMATTR_INVERSE              TERMATTR_ESC "[7m"

#define TERMATTR_FG_BLACK             TERMATTR_ESC "[30m"
#define TERMATTR_FG_RED               TERMATTR_ESC "[31m"
#define TERMATTR_FG_GREEN             TERMATTR_ESC "[32m"
#define TERMATTR_FG_YELLOW            TERMATTR_ESC "[33m"
#define TERMATTR_FG_BLUE              TERMATTR_ESC "[34m"
#define TERMATTR_FG_MAGENTA           TERMATTR_ESC "[35m"
#define TERMATTR_FG_CYAN              TERMATTR_ESC "[36m"
#define TERMATTR_FG_WHITE             TERMATTR_ESC "[37m"
#define TERMATTR_FG_NORMAL            TERMATTR_ESC "[39m"

#define TERMATTR_BG_BLACK             TERMATTR_ESC "[40m"
#define TERMATTR_BG_RED               TERMATTR_ESC "[41m"
#define TERMATTR_BG_GREEN             TERMATTR_ESC "[42m"
#define TERMATTR_BG_YELLOW            TERMATTR_ESC "[43m"
#define TERMATTR_BG_BLUE              TERMATTR_ESC "[44m"
#define TERMATTR_BG_MAGENTA           TERMATTR_ESC "[45m"
#define TERMATTR_BG_CYAN              TERMATTR_ESC "[46m"
#define TERMATTR_BG_WHITE             TERMATTR_ESC "[47m"
#define TERMATTR_BG_NORMAL            TERMATTR_ESC "[49m"

#define TERMATTR_LINECHARS_ON         TERMATTR_ESC "(0"
#define TERMATTR_LINECHARS_OFF        TERMATTR_ESC "(B"

#define TERMATTR_CLEAR_SCREEN         TERMATTR_ESC "[2J"
#define TERMATTR_CLEAR_LINE           TERMATTR_ESC "[2K"

#define TERMATTR_CURSOR(Y, X)         TERMATTR_ESC "[" #Y ";" #X "H"
#define TERMATTR_CURSOR_UP(N)         TERMATTR_ESC "[" #N "A"
#define TERMATTR_CURSOR_DOWN(N)       TERMATTR_ESC "[" #N "B"
#define TERMATTR_CURSOR_RIGHT(N)      TERMATTR_ESC "[" #N "C"
#define TERMATTR_CURSOR_LEFT(N)       TERMATTR_ESC "[" #N "D"
#define TERMATTR_CURSOR_HOME          TERMATTR_ESC "[H"

#define TERMATTR_SCROLL_UP            TERMATTR_ESC "D"
#define TERMATTR_SCROLL_DOWN          TERMATTR_ESC "M"

} // namespace ccxx

#endif // __ccxx_TerminalAttr_hxx
