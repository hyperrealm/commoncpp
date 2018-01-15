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

#ifndef __ccxx_Common_hxx
#define __ccxx_Common_hxx

/**
 * @mainpage
 *
 *
 * <b>commonc++</b> is a C++ class library for developing
 * cross-platform systems software for POSIX systems (GNU/Linux, Mac
 * OS X, Solaris) and Windows (Visual Studio or GNU toolchain via
 * MinGW). It provides C++ wrappers for common operating system
 * facilities like threads, mutexes, semaphores, sockets, shared
 * memory, regular expressions, filesystem access, etc., as well as
 * abstractions for strings, buffers, object pools, timers, logging,
 * and more.
 *
 * <b>commonc++</b> actually consists of four libraries: the core
 * library (commonc++), an extension library for interfacing with a
 * Java Virtual Machine (commonc++jvm), an extension library for
 * embedding SQL databases (commonc++db), and an extension library for
 * simple XML document manipulation (commonc++xml).
 */

/** @def CCXX_VERSION
 * Library version identifier. The three low-order bytes
 * represent the major, minor, and micro version numbers (e.g.,
 * <tt>0x00000602</tt> represents version 0.6.2) The remaining byte is
 * reserved for future use.
 */

#define CCXX_VERSION 0x00000606
#define CCXX_VERSION_MAJOR ((uint_t)((CCXX_VERSION >> 16) & 0xFF))
#define CCXX_VERSION_MINOR ((uint_t)((CCXX_VERSION >> 8) & 0xFF))
#define CCXX_VERSION_MICRO ((uint_t)(CCXX_VERSION & 0xFF))

// platform detection

/** @def CCXX_OS_POSIX
 * Defined if the host system is a POSIX OS.
 * @def CCXX_OS_WINDOWS
 * Defined if the host system is Windows.
 * @def CCXX_OS_MACOSX
 * Defined if the host system is Mac OS X.
 * @def CCXX_OS_WINDOWS_GNU
 * Defined if the host system is Windows and the compiler is GCC.
 * @def CCXX_OS_ANDROID
 * Defined if the host system is Android.
 * @internal COMMONCPP_API
 * @internal COMMONCPPDB_API
 * @internal COMMONCPPJVM_API
 * @internal COMMONCPPXML_API
 */

#if defined(__CYGWIN__)
#define CCXX_OS_POSIX
#define COMMONCPP_API
#define COMMONCPPDB_API
#define COMMONCPPJVM_API
#define COMMONCPPXML_API
#else
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) \
  || defined(_WIN64) || defined(WIN64)
#define CCXX_OS_WINDOWS

#ifdef __MINGW32__
#define CCXX_OS_WINDOWS_GNU
#endif

#if defined(COMMONCPP_EXPORTS)
#define COMMONCPP_API __declspec(dllexport)
#else // ! COMMONCPP_EXPORTS
#define COMMONCPP_API __declspec(dllimport)
#endif // COMMONCPP_EXPORTS

#if defined(COMMONCPPDB_EXPORTS)
#define COMMONCPPDB_API __declspec(dllexport)
#else // ! COMMONCPPDB_EXPORTS
#define COMMONCPPDB_API __declspec(dllimport)
#endif // COMMONCPPDB_EXPORTS

#if defined(COMMONCPPJVM_EXPORTS)
#define COMMONCPPJVM_API __declspec(dllexport)
#else // ! COMMONCPPJVM_EXPORTS
#define COMMONCPPJVM_API __declspec(dllimport)
#endif // COMMONCPPJVM_EXPORTS

#if defined(COMMONCPPXML_EXPORTS)
#define COMMONCPPXML_API __declspec(dllexport)
#else // ! COMMONCPPXML_EXPORTS
#define COMMONCPPXML_API __declspec(dllimport)
#endif // COMMONCPPXML_EXPORTS

#else // !WIN32
#define CCXX_OS_POSIX

#ifdef __APPLE__
#define CCXX_OS_MACOSX
#endif

#ifdef __ANDROID__
#define CCXX_OS_ANDROID
#endif

#define COMMONCPP_API
#define COMMONCPPDB_API
#define COMMONCPPJVM_API
#define COMMONCPPXML_API
#endif // WIN32
#endif // __CYGWIN__

#ifdef CCXX_OS_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define _WINSOCKAPI_
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0500 // Windows 2000 or later ONLY
#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 1024
#include <winsock2.h> // compilation of MulticastSocket fails otherwise.
#include <windows.h>
#endif // CCXX_OS_WINDOWS

/** @def ___DEPRECATED
 * Macro for tagging functions and classes as "deprecated".
 *
 * @def ___UNUSED
 * Macro for tagging parameters and variables as "unused".
 *
 * @def ___NORETURN
 * Macro for tagging functions as never returning.
 */

#if defined(__GNUC__) && ((__GNUC__ > 3)                                \
                          || ((__GNUC__ == 3) && (__GNUC_MINOR__ > 0)))

#define ___DEPRECATED(DECL) DECL __attribute__((deprecated))
#define ___UNUSED __attribute__((unused))
#define ___NORETURN(DECL) DECL __attribute__((noreturn))
#define ___PRINTF(M, N) __attribute__((format(printf, M, N)))

#elif defined(_MSC_VER) && (_MSC_VER >= 1300)

#define ___DEPRECATED(DECL) __declspec(deprecated) DECL
#define ___UNUSED
#define ___NORETURN(DECL) __declspec(noreturn) DECL
#define ___PRINTF(M, N)

#else

#define ___DEPRECATED(DECL) DECL
#define ___UNUSED
#define ___NORETURN(DECL) DECL
#define ___PRINTF(M, N)

#endif

#include <cstddef>

#ifndef CCXX_OS_WINDOWS
#include <sys/types.h>
#include <pthread.h>
#endif

#include <commonc++/Integers.h++>

namespace ccxx {

#if __cplusplus < 201103L

/**
 * @typedef ccxx::char16_t
 * A UTF-16 character.
 */
//typedef uint16_t char16_t;

/**
 * @typedef ccxx::char32_t
 * A UTF-32 character.
 */
//typedef uint32_t char32_t;

#endif // __cplusplus < 201103L

#ifdef CCXX_OS_WINDOWS

typedef HANDLE ProcessHandle;
typedef DWORD ProcessID;
typedef HANDLE ThreadHandle;
typedef DWORD ThreadID;
typedef HANDLE FileHandle;

#define CCXX_INVALID_FILE_HANDLE INVALID_HANDLE_VALUE

#else // ! CCXX_OS_WINDOWS

typedef pid_t ProcessHandle;
typedef pid_t ProcessID;
typedef pthread_t ThreadHandle;
typedef pthread_t ThreadID;
typedef int FileHandle;

/** @internal CCXX_INVALID_FILE_HANDLE */

#define CCXX_INVALID_FILE_HANDLE (-1)

#endif // CCXX_OS_WINDOWS

/** @def NUL
 * The NUL character (ASCII value 0).
 */

#ifdef NUL
#undef NUL
#endif
#define NUL '\0'

/** @def CCXX_LENGTHOF(A)
 * Calculates the length of (the number of elements in) the
 * statically-allocated array A.
 */

#define CCXX_LENGTHOF(A)                        \
  (sizeof(A) / sizeof(A[0]))

/** @def CCXX_OFFSETOF(S, F)
 * Computes the offset, in bytes, of the field F in the aggregate type S.
 */

#define CCXX_OFFSETOF(S, F)                                     \
  ((size_t)(((void *)(&(((S *)NULL)->F))) - ((void *)NULL)))

/** @def CCXX_ZERO(V)
 * Zero the memory occupied by the variable V.
 */

#define CCXX_ZERO(V)                            \
  (std::memset(&(V), 0, sizeof(V)))

/** @internal __CCXX_STRINGIFY__ */

#define __CCXX_STRINGIFY__(S) #S

/** @def CCXX_STRINGIFY(S)
 * Evaulates to a string constant representation of S.
 */

#define CCXX_STRINGIFY(S) __CCXX_STRINGIFY__(S)

/** @internal __CCXX_UNIQUE_VARNAME__
 * @internal CCXX_UNIQUE_VARNAME
 */

#define __CCXX_UNIQUE_VARNAME__(pfx, id, sfx) pfx ## _ ## id ## _ ## sfx
#define CCXX_UNIQUE_VARNAME(pfx, id, sfx)       \
  __CCXX_UNIQUE_VARNAME__(pfx, id, sfx)

/** Introduces a static initializer code block. */

#define CCXX_STATIC_INITIALIZER                                         \
  static void __ccxx_static_init__(void);                               \
  static bool __ccxx_static_init_done__ ___UNUSED =                     \
    (__ccxx_static_init__(), true);                                     \
  static void __ccxx_static_init__(void)

/** @def CCXX_COPY_DECLS(CLASS)
 *
 * Inlines declarations of a copy constructor and assignment operator for
 * the class CLASS.
 */

#define CCXX_COPY_DECLS(CLASS)                  \
  CLASS(const CLASS &other);                    \
  CLASS& operator=(const CLASS &other);

#define CCXX_PIMPL_DECL(TYPE)                   \
  class TYPE ## Private;                        \
  TYPE ## Private *pimpl_;

#define CCXX_PIMPL(TYPE)                        \
  TYPE ## Private *pimpl = this->pimpl_;

/** Thread and Process priority levels. */
enum Priority {
  /** Lowest priority. */
  PrioLowest,
  /** Low priority. */
  PrioLow,
  /** Normal (default) priority. */
  PrioNormal,
  /** High priority. */
  PrioHigh,
  /** Highest priority. */
  PrioHighest
};

/** Byte endianness. */
enum Endianness {
  /** Big-endian (network byte order). */
  BigEndian = 0,
  /** Little-endian. */
  LittleEndian = 1
};

}; // namespace ccxx

/** @def CCXX_FWD_DECL(CLASS)
 *
 * Inline a forward-declaration for the commonc++ class CLASS.
 */

#define CCXX_FWD_DECL(CLASS)                    \
  namespace ccxx { class CLASS; };

#endif // __ccxx_Common_hxx
