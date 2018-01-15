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

#ifndef __ccxx_Integers_hxx
#define __ccxx_Integers_hxx

#include <commonc++/Common.h++>

#ifdef CCXX_OS_WINDOWS
#include <windows.h>
#ifndef CCXX_OS_WINDOWS_GNU

typedef INT8   int8_t;
typedef UINT8  uint8_t;
typedef INT16  int16_t;
typedef UINT16 uint16_t;
typedef INT32  int32_t;
typedef UINT32 uint32_t;
typedef INT64  int64_t;
typedef UINT64 uint64_t;
#else // CCXX_OS_WINDOWS_GNU
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <stdint.h>
#endif // ! CCXX_OS_WINDOWS_GNU
#else // ! CCXX_OS_WINDOWS
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <stdint.h>
#endif // CCXX_OS_WINDOWS

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace ccxx {

/**
 * @typedef byte_t
 * An unsigned 8-bit value.
 */
typedef unsigned char byte_t;

/**
 * @typedef uint_t
 * An alias for <i>unsigned int</i>.
 */
typedef unsigned int uint_t;

#ifdef CCXX_OS_WINDOWS
typedef int64_t off64_t;
#endif

/**
 * @typedef time_s_t
 * A time expressed in seconds since the epoch (00:00:00, UTC,
 * January 1, 1970).
 */
typedef int32_t time_s_t;

/**
 * @typedef timespan_s_t
 * A timespan expressed in seconds.
 */
typedef int timespan_s_t;

/**
 * @typedef time_ms_t
 * A time expressed in milliseconds since the epoch (00:00:00, UTC,
 * January 1, 1970).
 */
typedef int64_t time_ms_t;

/**
 * @typedef timespan_ms_t
 * A timespan expressed in milliseconds.
 */
typedef int timespan_ms_t;

} // namespace ccxx

/**
 * @def INT64_CONST(I)
 * Declare a signed 64-bit integer constant I.
 *
 * @def UINT64_CONST(I)
 * Declare an unsigned 64-bit integer constant I.
 */
#if defined (CCXX_OS_WINDOWS) && ! defined (CCXX_OS_WINDOWS_GNU)

#define INT64_CONST(I)  (I ## i64)
#define UINT64_CONST(I) (I ## Ui64)

#else

#define INT64_CONST(I)  int64_t(I ## LL)
#define UINT64_CONST(I) uint64_t(I ## ULL)

#endif

/**
 * @def INT8_MIN
 * The minimum value for a signed 8-bit integer.
 * @def INT8_MAX
 * The maximum value for a signed 8-bit integer.
 * @def UINT8_MAX
 * The maximum value for an unsigned 8-bit integer.
 * @def INT16_MIN
 * The minimum value for a signed 16-bit integer.
 * @def INT16_MAX
 * The maximum value for a signed 16-bit integer.
 * @def UINT16_MAX
 * The maximum value for an unsigned 16-bit integer.
 * @def INT32_MIN
 * The minimum value for a signed 32-bit integer.
 * @def INT32_MAX
 * The maximum value for a signed 32-bit integer.
 * @def UINT32_MAX
 * The maximum value for an unsigned 32-bit integer.
 * @def INT64_MIN
 * The minimum value for a signed 64-bit integer.
 * @def INT64_MAX
 * The maximum value for a signed 64-bit integer.
 * @def UINT64_MAX
 * The maximum value for an unsigned 64-bit integer.
 */

#ifndef INT8_MIN
#define INT8_MIN (-128)
#endif

#ifndef INT16_MIN
#define INT16_MIN (-32767-1)
#endif

#ifndef INT32_MIN
#define INT32_MIN (-2147483647-1)
#endif

#ifndef INT64_MIN
#define INT64_MIN (INT64_CONST(-9223372036854775807)-1)
#endif

#ifndef INT8_MAX
#define INT8_MAX (127)
#endif

#ifndef INT16_MAX
#define INT16_MAX (32767)
#endif

#ifndef INT32_MAX
#define INT32_MAX (2147483647)
#endif

#ifndef INT64_MAX
#define INT64_MAX (INT64_CONST(9223372036854775807))
#endif

#ifndef UINT8_MAX
#define UINT8_MAX (255U)
#endif

#ifndef UINT16_MAX
#define UINT16_MAX (65535U)
#endif

#ifndef UINT32_MAX
#define UINT32_MAX (4294967295U)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX (UINT64_CONST(18446744073709551615))
#endif

/**
 * @def INT64_FMT
 * The printf() formatting directive for signed 64-bit integers.
 * @def INT64_P_FMT(W)
 * The parameterized printf() formatting directive for signed 64-bit
 * integers; W indicates the field width.
 * @def UINT64_FMT
 * The printf() formatting directive for unsigned 64-bit integers.
 * @def UINT64_P_FMT(W)
 * The parameterized printf() formatting directive for unsigned 64-bit
 * integers; W indicates the field width.
 *
 * @def INT64_WIDE_FMT
 * The wprintf() (wide string) formatting directive for signed 64-bit
 * integers.
 * @def INT64_WIDE_P_FMT
 * The parameterized wprintf() (wide string) formatting directive for signed
 * 64-bit integers; W indicates the field width.
 * @def UINT64_WIDE_FMT
 * The wprintf() (wide string) formatting directive for unsigned 64-bit
 * integers.
 * @def UINT64_WIDE_P_FMT
 * The parameterized wprintf() (wide string) formatting directive for unsigned
 * 64-bit integers; W indicates the field width.
 */

#if defined(CCXX_OS_WINDOWS) || defined (CCXX_OS_WINDOWS_GNU)

#define INT64_FMT "%I64d"
#define UINT64_FMT "%I64u"

// parameterized width versions
#define INT64_P_FMT(W) "%" #W "I64d"
#define UINT64_P_FMT(W) "%" #W "I64u"

// parameterized width versions

#ifdef __GNUC__

#define INT64_WIDE_P_FMT(W) L"%" #W "I64d"
#define UINT64_WIDE_P_FMT(W) L"%" #W "I64u"

#else

#define INT64_WIDE_P_FMT(W) L"%" L#W L"I64d"
#define UINT64_WIDE_P_FMT(W) L"%" L#W L"I64u"

#endif // __GNUC__

#else

#define INT64_FMT "%" PRId64
#define UINT64_FMT "%" PRIu64

// parameterized width versions
#define INT64_P_FMT(W) "%" #W PRId64
#define UINT64_P_FMT(W) "%" #W PRIu64

#endif

/**
 * @def CCXX_BIT_SET(N, BIT)
 * Set the bit at index BIT (where 0 corresponds to the LSB) in N. The
 * arguments are evaluated only once.
 */
#define CCXX_BIT_SET(N, BIT)                    \
  ((N) |= (1 << (BIT)))

/**
 * @def CCXX_BIT_CLEAR(N, BIT)
 * Clear the bit at index BIT (where 0 corresponds to the LSB) in N. The
 * arguments are evaluated only once.
 */
#define CCXX_BIT_CLEAR(N, BIT)                  \
  (((N) &= ~(1 << (BIT))))

/**
 * @def CCXX_BIT_READ(N, BIT)
 * Read the bit at index BIT (where 0 corresponds to the LSB) in N. Returns
 * 0 or 1. The arguments are evaluated only once.
 */
#define CCXX_BIT_READ(N, BIT)                   \
  ((((N) & (1 << (BIT))) == 0) ? 0 : 1)

/**
 * @def CCXX_BIT_WRITE(N, BIT, VALUE)
 * Write the bit at index BIT (where 0 corresponds to the LSB) in N. VALUE
 * is zero (or <b>false</b>) to clear the bit, nonzero (or <b>true</b>) to
 * set the bit. The arguments are evaluated only once.
 */
#define CCXX_BIT_WRITE(N, BIT, VALUE)                                   \
  ((VALUE) ? CCXX_BIT_SET((N), (BIT)) : CCXX_BIT_CLEAR((N), (BIT)))

/**
 * @def CCXX_BIT_TEST(N, BIT)
 * Test the bit at index BIT (where 0 corresponds to the LSB) in N. Returns
 * <b>true</b> if the bit is set, <b>false</b> otherwise. The arguments are
 * evaluated only once.
 */
#define CCXX_BIT_TEST(N, BIT)                   \
  (((N) & (1 << (BIT))) != 0)

#endif // __ccxx_Integers_hxx
